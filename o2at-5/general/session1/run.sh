OPTION="-b --configuration json://myConfig.json --pipeline propagation-service:4"

o2-analysistutorial-mm-my-example-task ${OPTION} | o2-analysis-propagationservice ${OPTION} | o2-analysis-event-selection-service ${OPTION}
