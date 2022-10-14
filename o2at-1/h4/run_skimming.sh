#This is the skimming.
#For the simple reading, it should suffice to do:
#o2-analysistutorial-h4-4-skimming --aod-file AO2D.root
#...with the resulting file! 
export OPTIONS="-b --configuration json://dpl-config-skimming.json --resources-monitoring 2 --aod-memory-rate-limit 1000000000 --shm-segment-size 7500000000"
echo "options: ${OPTIONS}"
o2-analysis-timestamp ${OPTIONS} | \
o2-analysis-event-selection ${OPTIONS} | \
o2-analysis-multiplicity-table ${OPTIONS} | \
o2-analysis-track-propagation ${OPTIONS} | \
o2-analysis-trackselection ${OPTIONS} | \
o2-analysis-hf-track-index-skims-creator ${OPTIONS} | \
o2-analysis-hf-candidate-creator-2prong ${OPTIONS} | \
o2-analysistutorial-h4-3-skimming ${OPTIONS} --aod-writer-json OutputDirector.json --fairmq-ipc-prefix .
