#!/bin/bash

run="${1:-run2}"
if [ "$run" = "run3" ]
then
    file="/alice/data/2022/LHC22s/529418/apass4/0000/AOD/001/AO2D.root"
    alien_cp alien://${file} file:AO2D_run3.root
else
    file="/alice/data/2018/LHC18q/000296132/pass3/PWGZZ/Run3_Conversion/339_20220722-1611_child_2/AOD/001/AO2D.root"
    alien_cp alien://${file} file:AO2D.root
fi

exit 0