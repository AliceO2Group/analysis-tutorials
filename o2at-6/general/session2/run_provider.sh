#!/bin/bash

o2-analysistutorial-derived-basic-provider -b --configuration json://configuration.json |
o2-analysis-event-selection-service -b -- configuration json://configuration.json |
o2-analysis-propagationservice -b --configuration json://configuration.json --aod-file @input_data.txt --aod-writer-json OutputDirector.json;

