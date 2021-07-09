// For GRSISort 4 use the below commands 
//g++ -fPIC BGOHistogramGenerator.cxx -o BGOHistogramGenerator -std=c++0x -I$GRSISYS/include -L$GRSISYS/lib `grsi-config --cflags --all-libs --GRSIData-libs` -I$GRSISYS/GRSIData/include -L$GRSISYS/GRSIData/lib `root-config --cflags --libs` -lTreePlayer -lMathMore -lSpectrum -lMinuit -lPyROOT
#include <iostream>
#include <iomanip>

#include "TH2.h"
#include "TList.h"
#include "TChain.h"
#include "TTree.h"

#include "TTigress.h"
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
  TH2D *GeBgoHP = new TH2D("GeBgoHP", "Ge vs Bgo Hit Pattern", 325, 0, 325, 64, 0, 64);
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
  TChain* AnalysisTree = (TChain*) inputFile->Get("AnalysisTree");
  printf("%i tree files, details:\n", AnalysisTree->GetNtrees());
  Int_t nEntries = AnalysisTree->GetEntries();

  printf("Reading calibration file: %s\n", calFile);
  TChannel::ReadCalFile(calFile);

  TTigress *tigress = NULL;
  TTigressHit *tigress_hit;
  TGriffin* griffin = NULL;
  TGriffinBgo* griffin_bgo = NULL;
  TGriffinHit* griffin_hit;
  TBgoHit* bgo_hit;
  
  int active_positions[] = {1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16}; //Full array

  bool tig = true;
  if (AnalysisTree->FindBranch("TTigress")) {
    AnalysisTree->SetBranchAddress("TTigress", & tigress);
     cout << "Generating TIGRESS" << endl;
     tig = true;
  } else {
	  if (AnalysisTree->FindBranch("TGriffin")) {
		AnalysisTree->SetBranchAddress("TGriffin", & griffin);
		AnalysisTree->SetBranchAddress("TGriffinBgo", &griffin_bgo);
		cout << "Generating GRIFFIN" << endl;
		tig = false;
	  } else {
    cout << "No TTigress or TGriffin Branch Found. Things will go wrong..." << endl;
    }
  }

  // Must turn Cross-Talk correction off otherwise seg-fault
   TGRSIOptions::AnalysisOptions()->SetCorrectCrossTalk(false);

  printf("Begin sort\n");
  int arrayLowerBound, arrayUpperBound;
  int bgoNum, crystalNum;
  for (int jEntry = 0; jEntry < (nEntries - 1); jEntry++){
    AnalysisTree->GetEntry(jEntry);
    if(tig) {
      for(int i = 0; i < tigress->GetMultiplicity(); i++){
        if( tigress->GetMultiplicity() == tigress->GetBGOMultiplicity() && tigress->GetMultiplicity()==1){
          tigress_hit = tigress->GetTigressHit(i);
          int * ap = find(begin(active_positions), end(active_positions), tigress_hit->GetDetector()); 
          if (ap != end(active_positions)) {  
            bgo_hit = &tigress->GetBGO(i);
            bgoNum = ((bgo_hit->GetDetector()-1)*4+bgo_hit->GetCrystal());
            if(bgo_hit->GetCharge() > 15 && bgoNum == tigress_hit->GetArrayNumber()) {
              GeBgoHP->Fill(bgo_hit->GetArrayNumber(), tigress_hit->GetArrayNumber());
              gamma_singles[bgoNum][bgo_hit->GetSegment()-1]->Fill(tigress_hit->GetEnergy(),bgo_hit->GetCharge());
	          }
          }
        }
      }
    } else {
      for (int i = 0; i < griffin->GetMultiplicity(); i++){
        griffin_hit = griffin->GetGriffinHit(i);
        int * ap = find(begin(active_positions), end(active_positions), griffin_hit->GetDetector()); 
        if (ap != end(active_positions)) {  
          // GRIFFIN-BGO
          for (int j = 0; j < griffin_bgo->GetMultiplicity(); j++){
            bgo_hit = griffin_bgo->GetBgoHit(j);
            crystalNum = griffin_hit->GetArrayNumber() - 1; // GetArrayNumber starts at 1
            arrayLowerBound = crystalNum * 5;
            arrayUpperBound = arrayLowerBound + 6;
            bgoNum = bgo_hit->GetArrayNumber() - arrayLowerBound - 1;
            // only plot the BGO channels for the individual crystals 
            if (bgo_hit->GetArrayNumber() > arrayLowerBound && bgo_hit->GetArrayNumber() < arrayUpperBound){
              GeBgoHP->Fill(bgo_hit->GetArrayNumber(), griffin_hit->GetArrayNumber() - 1);
              gamma_singles[crystalNum][bgoNum]->Fill(griffin_hit->GetEnergy(), bgo_hit->GetCharge());
            }
          }
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
  GeBgoHP->Write();
  list->Write();
  myFile->Close();
   
}// end GenerateHistograms

int main(int argc, char **argv){
   char const *inFile;
   char const *outFile;
   char const *calFile;

   if (argc == 1){
      cout << "Insufficient arguments." << endl;
      cout << "./MakeBGOHistograms analysis_tree calibration_file [out_file_name]" << endl;
      return EXIT_FAILURE;
   } if (argc == 2){
      inFile = argv[1];
      calFile = "CalibrationFile.cal";
      outFile = "outFile.root";
   } else if (argc == 3){
      inFile = argv[1];
      calFile = argv[2];
      outFile = "outFile.root";
   } else if (argc == 4){
      inFile = argv[1];
      calFile = argv[2];
      outFile = argv[3];
   }
   else{
      cout << "Incorrect number of arguments" << endl;
   }
   printf("Input file:%s\nCalibration file: %s\nOutput file: %s\n",inFile,calFile,outFile);
   GenerateHistograms(inFile, calFile, outFile);
   return EXIT_SUCCESS;
} // end main
