#!/bin/bash 

export OPTIONS="-b --configuration json://${PWD}/json_strangenesstutorialMC.json --resources-monitoring 2 --aod-memory-rate-limit 1000000000 --shm-segment-size 6000000000"
o2-analysis-timestamp ${OPTIONS} \
| o2-analysis-collision-converter ${OPTIONS} \
| o2-analysis-event-selection ${OPTIONS} \
| o2-analysis-lf-lambdakzerobuilder ${OPTIONS} \
| o2-analysis-lf-lambdakzerolabelbuilder ${OPTIONS} \
| o2-analysis-pid-tpc-base ${OPTIONS} \
| o2-analysis-pid-tpc ${OPTIONS} \
| o2-analysis-track-propagation ${OPTIONS} \
| o2-analysistutorial-lf-strangeness-step3 ${OPTIONS} 
