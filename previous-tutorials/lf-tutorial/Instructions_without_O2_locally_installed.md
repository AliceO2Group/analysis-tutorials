# First hands-on LF O2 analysis workshop - 29th March 2021

If you don't have O2 locally installed then use lxplus

Enter the O2 environment (from lxplus) `source /cvmfs/alice.cern.ch/etc/login.sh && alienv enter VO_ALICE@O2::nightly-20210328-1`

## Downloading the input data
Once you entered the O2 environment you can download the input data from alien (requires certificate)

__Data__: `alien_cp alien:///alice/data/2015/LHC15o/000244918/pass5_lowIR/PWGZZ/Run3_Conversion/148_20210304-0829_child_1/0017/AO2D.root /tmp/AO2D_data.root`

__MC__: `alien_cp alien:///alice/sim/2020/LHC20f6/244918/PWGZZ/Run3_Conversion/151_20210305-2339/0017/AO2D.root /tmp/AO2D_mc.root`

__N.B.__ This can take a while!

If you did not manage to get the input files from alien you can do try with:

__Data__: `https://cernbox.cern.ch/index.php/s/EEQoTmZ1bVNtbnH`

__MC__: `https://cernbox.cern.ch/index.php/s/JvACZuc26GITAYQ`


Run a O2 task for nuclex with: `o2-analysis-nuclei-spectra --aod-file AO2D_data.root | o2-analysis-trackselection | o2-analysis-trackextension  | o2-analysis-pid-tpc --add-qa 1| o2-analysis-pid-tof --add-qa 1 | o2-analysis-centrality-table | o2-analysis-event-selection | o2-analysis-multiplicity-table | o2-analysis-timestamp`

Check the output in the `AnalysisResults.root` and `QAResults.root`
