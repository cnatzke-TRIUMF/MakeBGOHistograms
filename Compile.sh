#!/bin/bash 

echo "::: Building BGOHistogramGenerator ..."
g++ -fPIC BGOHistogramGenerator.cxx -o BGOHistogramGenerator -Wall -Wextra -std=c++0x -I$GRSISYS/include -I$GRSISYS/GRSIData/include `grsi-config --cflags --all-libs --GRSIData-libs` -lTGRSIDetector `root-config --cflags --libs` -lTreePlayer -lMathMore -lSpectrum -lMinuit -lPyROOT
echo "::: Building BGOHistogramGenerator ... [DONE]"
