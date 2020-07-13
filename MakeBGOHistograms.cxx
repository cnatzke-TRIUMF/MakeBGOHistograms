// For GRSISort 4 use the below commands 
//g++ -fPIC BGOHistogramGenerator.cxx -o BGOHistogramGenerator -std=c++0x -I$GRSISYS/include -L$GRSISYS/lib `grsi-config --cflags --all-libs --GRSIData-libs` -I$GRSISYS/GRSIData/include -L$GRSISYS/GRSIData/lib `root-config --cflags --libs` -lTreePlayer -lMathMore -lSpectrum -lMinuit -lPyROOT
#include <iostream>
#include <iomanip>

#include "TH2.h"
#include "TList.h"
#include "TChain.h"
#include "TTree.h"

#include "TGriffin.h"
#include "TGriffinBgo.h"
#include "TGRSIOptions.h"
//#include "TParserLibrary.h"
//#include "TEnv.h"

using namespace std;

void GenerateHistograms(const char* inFile, const char* calFile, const char* outFile){
   Double_t nBinsX = 75; 
   Double_t minX = 0;
   Double_t maxX = 600;
   
   Double_t nBinsY = 64; 
   Double_t minY = 0;
   Double_t maxY = 1024;
   
   TList *list = new TList;

   // setting up list of gammas singles histograms
   TH2F *gamma_singles[64][5];
   TH2D *griffinBgoHP = new TH2D("griffinBgoHP", "Griffin vs Bgo Hit Pattern; BGO; GRIFFIN", 325, 0, 325, 64, 0, 64);
   char hName[64];
   for (int i = 0; i < 64; i++){
      for (int j = 0; j < 5; j++){
         int k = 1 + i/4;
         sprintf(hName, "h_%d_%d_%d", k, i, j);
         gamma_singles[i][j] = new TH2F(hName, Form("Position %1.1i BGO %1.1d", i, j), nBinsX, minX, maxX, nBinsY, minY, maxY);
      }
   }

   TFile * inputFile = new TFile(inFile, "READ");
   if (!inputFile->IsOpen()){
      cout << inputFile << "cannot be opened. Aborting" << endl;
      return;
   }

   // preparing the chain
   TChain* dataChain = (TChain*) inputFile->Get("AnalysisTree");
   cout << dataChain->GetNtrees() + 1 << " tree(s) found." << endl; // I think it counts from 0 i.e. 0 = 1?
   Int_t nEntries = dataChain->GetEntries();

   cout << "Reading CalFile: " << calFile << endl;
   TChannel::ReadCalFile(calFile);

   TGriffin* fGrif = NULL;
   TGriffinBgo* fGriffinBgo = NULL;
   TGriffinHit* grifHit;
   TBgoHit* bgoHit;

   dataChain->SetBranchAddress("TGriffin", &fGrif);
   dataChain->SetBranchAddress("TGriffinBgo", &fGriffinBgo);
   // Must turn Cross-Talk correction off otherwise seg-fault
   TGRSIOptions::AnalysisOptions()->SetCorrectCrossTalk(false);

   cout << ":: Beginning sort" << endl;
   int arrayLowerBound, arrayUpperBound;
   int bgoNum, crystalNum;
   for (int jEntry = 0; jEntry < (nEntries - 1); jEntry++){
      dataChain->GetEntry(jEntry);
      for (int i = 0; i < fGrif->GetMultiplicity(); i++){
         grifHit = fGrif->GetGriffinHit(i);
         // GRIFFIN-BGO
         for (int j = 0; j < fGriffinBgo->GetMultiplicity(); j++){
            bgoHit = fGriffinBgo->GetBgoHit(j);
            crystalNum = grifHit->GetArrayNumber() - 1; // GetArrayNumber starts at 1
            arrayLowerBound = crystalNum * 5;
            arrayUpperBound = arrayLowerBound + 6;
            bgoNum = bgoHit->GetArrayNumber() - arrayLowerBound - 1;
            // only plot the BGO channels for the indivual crystals 
            if (bgoHit->GetArrayNumber() > arrayLowerBound && bgoHit->GetArrayNumber() < arrayUpperBound){
               griffinBgoHP->Fill(bgoHit->GetArrayNumber(), grifHit->GetArrayNumber());
               //if (jEntry > 2500 && jEntry < 2600){
               //   cout << "Entry: " << jEntry << endl;
               //   cout << "Filling " << grifHit->GetEnergy() << ":" << bgoHit->GetCharge() << endl;
               //}
               gamma_singles[crystalNum][bgoNum]->Fill(grifHit->GetEnergy(), bgoHit->GetCharge());
            }
         }
      }
      if (jEntry % 10000 == 0) cout << setiosflags(ios::fixed) << "GRIFFIN Entry " << jEntry << " of " << nEntries << ", " << 100 * jEntry / nEntries << "% complete" << "\r" << flush;
   } // end jEntry

   for (int i = 0; i < 64; i++){
      for (int j = 0; j < 5; j++){
         list->Add(gamma_singles[i][j]);
      }
   }

   cout << "Entry " << nEntries << " of " << nEntries << ", 100% Complete!" << endl;
   cout << ":: Sort complete, writing histograms." << endl;

   cout << "Writing histograms to file: " << outFile << endl;
   TFile *myFile = new TFile(outFile, "RECREATE");
   myFile->cd();
   griffinBgoHP->Write();
   list->Write();
   myFile->Close();
   
}// end GenerateHistograms

/****************************************************************************
 * Prints usage message
 ***************************************************************************/
void PrintUsage(char* argv[]){
   std::cerr << argv[0] << "\n"
             << "usage: " << argv[0] << "  analysis_tree calibration_file [out_file_name]\n"
             << " analysis_tree:       analysis tree to process (must end with .root)\n"
             << " calibration_file:    calibration file (must end with .cal)\n"
             << " out_file_name:       optional, name of the output file\n"
             << std::endl;
} // end PrintUsage

/****************************************************************************
 * Main
 ***************************************************************************/
int main(int argc, char **argv){
   char const *inFile;
   char const *outFile;
   char const *calFile;

   if (argc == 1){
      PrintUsage(argv);
      return EXIT_FAILURE;
   }
   else if (argc == 3){
      inFile = argv[1];
      calFile = argv[2];
      outFile = "outFile.root";
   }
   else{
      PrintUsage(argv);
      return EXIT_FAILURE;
   }

   GenerateHistograms(inFile, calFile, outFile);
   return EXIT_SUCCESS;
} // end main
