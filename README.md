# MakeBGOHistograms
This project is designed to sort and histogram 137Cs source data from the
GRIFFIN HPGe array at TRIUMF-ISAC for the purpose of voltage matching the BGO
suppressor shields.

## Table of Contents
  * [Installation](#installation)
  * [Running MakeBGOHistograms](#running-makebgohistograms)
  * [Helper scripts](#helper-scripts)
    + [Compile.sh](#compilesh)

# Installation
0. Requires GRSISort 4.X.X
1. Get the code, either via tarball or from github
```
git clone https://github.com/cnatzke-TRIUMF/MakeBGOHistograms.git
```
2. Compile the programs via the included script
 ``` 
 cd MakeBGOHistograms
 ./Compile.sh
 ```

# Running MakeBGOHistograms
The general form of input is:
```
./MakeBGOHistograms analysis_tree calibration_file
```

#### Parameters
```
analysis_tree     The unpacked analysis tree containing 137Cs source data
calibration_file  GRIFFIN calibration file
```

#### Outputs
```
outFile.root   ROOT file containing BGO and HPGe histograms 
```

# Helper scripts
Included is a simple (very simple) bash script to make compiling easier.

### Compile.sh
This script compiles the main code.
```
./Compile.sh 
```

