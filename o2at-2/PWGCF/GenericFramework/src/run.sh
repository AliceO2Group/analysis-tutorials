#!/bin/bash

#Specify whether this is run2 or run3
run="${1:-run2}"

if [ "$run" = "run2" ]
then
    config="configuration.json"
    aodfile="AO2D.root"
    tracktable="o2-analysis-trackextension"
else 
    config="configuration_run3.json"
    aodfile="AO2D_run3.root"   
    tracktable="o2-analysis-track-propagation"
fi

#Options to pass to each workflow: -b = batch mode, --configuration specifies configuration file
opt="-b --configuration json://${config}"

${tracktable} ${opt} |
o2-analysis-collision-converter ${opt} |
o2-analysis-timestamp ${opt} | 
o2-analysis-multiplicity-table ${opt} | 
o2-analysis-trackselection ${opt} | 
o2-analysis-centrality-table ${opt} |
o2-analysis-event-selection ${opt} | 
o2-analysis-pid-tof-full ${opt} |
o2-analysis-pid-tof-base ${opt} |
o2-analysis-pid-tpc-full ${opt} |
o2-analysis-pid-tpc-base ${opt} |
o2-analysis-pid-bayes ${opt} |
o2-analysis-cf-flow-gfw-tutorial ${opt}  --aod-file ${aodfile} --aod-memory-rate-limit 429496320