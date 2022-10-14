export OPTIONS="-b --configuration json://config-h3.json"

o2-analysistutorial-h3-5-v0mcexample ${OPTIONS} | o2-analysis-timestamp ${OPTIONS} | o2-analysis-track-propagation ${OPTIONS} | o2-analysis-event-selection ${OPTIONS} | o2-analysis-lf-lambdakzerobuilder ${OPTIONS} | o2-analysis-pid-tpc ${OPTIONS} | o2-analysis-multiplicity-table ${OPTIONS}
