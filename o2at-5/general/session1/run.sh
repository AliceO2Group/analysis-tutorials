OPTION="-b --configuration json://myConfig.json --pipeline track-propagation:4"

o2-analysistutorial-mm-my-example-task ${OPTION} | o2-analysis-track-propagation ${OPTION} | o2-analysis-timestamp ${OPTION}

