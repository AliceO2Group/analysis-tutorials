#!/bin/bash

# available periods and runs can be found here: https://alimonitor.cern.ch/hyperloop/datasets
# For development, try to find files with reasonable size (<1GB) for faster execution
File="/alice/data/2022/LHC22f/520471/apass3/1450/o2_ctf_run00520471_orbit0649772974_tf0000043923_epn026/001/AO2D.root"

alien_cp alien://${File} file://AO2D.root

[ ! -f Input.txt ] && echo "AO2D.root" >>Input.txt

exit 0
