
if [ $1 == "SVertexer" ]; then
    time o2-analysis-bc-converter --configuration json://config_pcm_tutorial1.json -b |
    o2-analysis-tracks-extra-converter --configuration json://config_pcm_tutorial1.json -b |
    o2-analysis-timestamp --configuration json://config_pcm_tutorial1.json -b |
    o2-analysis-event-selection --configuration json://config_pcm_tutorial1.json -b |
    o2-analysis-track-propagation --configuration json://config_pcm_tutorial1.json -b |
    o2-analysis-pid-tpc-base --configuration json://config_pcm_tutorial1.json -b |
    o2-analysis-pid-tpc-full --configuration json://config_pcm_tutorial1.json -b |
    o2-analysis-lf-lambdakzerobuilder --configuration json://config_pcm_tutorial1.json -b |
    o2-analysistutorial-em-pcm-tutorial --configuration json://config_pcm_tutorial1.json -b;

    mv AnalysisResults.root AnalysisResults_SVertexer.root
    
elif [ $1 == "createPCM" ]; then
    time o2-analysis-bc-converter --configuration json://config_pcm_tutorial2.json -b |
    o2-analysis-tracks-extra-converter --configuration json://config_pcm_tutorial2.json -b |
    o2-analysis-timestamp --configuration json://config_pcm_tutorial2.json -b |
    o2-analysis-event-selection --configuration json://config_pcm_tutorial2.json -b |
    o2-analysis-track-propagation --configuration json://config_pcm_tutorial2.json -b |
    o2-analysis-pid-tpc-base --configuration json://config_pcm_tutorial2.json -b |
    o2-analysis-pid-tpc-full --configuration json://config_pcm_tutorial2.json -b |
    o2-analysis-em-create-pcm --configuration json://config_pcm_tutorial2.json -b |
    o2-analysistutorial-em-pcm-tutorial --configuration json://config_pcm_tutorial2.json -b;

    mv AnalysisResults.root AnalysisResults_createPCM.root 

elif [ $1 == "DlData" ]; then
    alien_cp alien:///alice/data/2022/LHC22f/520472/apass4/1540/o2_ctf_run00520472_orbit0680884910_tf0000148280_epn087/001/AO2D.root file://./AO2D.root;
fi