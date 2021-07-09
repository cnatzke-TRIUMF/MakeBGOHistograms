# MakeBGOHistograms
This project is designed to sort and histogram 137Cs source data from the
GRIFFIN or TIGRESS HPGe arrays at TRIUMF-ISAC for the purpose of voltage matching the BGO
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
./MakeBGOHistograms analysis_tree calibration_file [out_file_name]
```

##### Parameters
```
analysis_tree     The unpacked analysis tree containing 137Cs source data
calibration_file  GRIFFIN/TIGRESS calibration file
out_file_name     optional, Name of output file
                  default: outFile.root
```

##### Outputs
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

