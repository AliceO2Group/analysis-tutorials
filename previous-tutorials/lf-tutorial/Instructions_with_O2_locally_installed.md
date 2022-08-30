# First hands-on LF O2 analysis workshop - 29th March 2021

## Implementing your first task:

Go into the PWGLF task directory in O2
`cd O2/Analysis/Tasks/PWGLF`

Create your task file `touch MyAwesomeTask.cxx && git add MyAwesomeTask.cxx`

Add your task to the local `CMakeLists.txt`, add:
```c++:
o2_add_dpl_workflow(my-awesome-task
                    SOURCES MyAwesomeTask.cxx
                    PUBLIC_LINK_LIBRARIES O2::Framework O2::DetectorsBase O2::AnalysisDataModel O2::AnalysisCore
                    COMPONENT_NAME Analysis)
```

Implement your first task in `MyAwesomeTask.cxx`:

```c++:
// Copyright CERN and copyright holders of ALICE O2. This software is
// distributed under the terms of the GNU General Public License v3 (GPL
// Version 3), copied verbatim in the file "COPYING".
//
// See http://alice-o2.web.cern.ch/license for full licensing information.
//
// In applying this license CERN does not waive the privileges and immunities
// granted to it by virtue of its status as an Intergovernmental Organization
// or submit itself to any jurisdiction.

// O2 includes
#include "Framework/AnalysisTask.h"
#include "Framework/runDataProcessing.h"

using namespace o2;
using namespace o2::framework;

struct MyAwesomeTask { // This is a task
  // The histogram registry is the container of the output histograms
  HistogramRegistry histos{"Histos", {}, OutputObjHandlingPolicy::AnalysisObject};

  // Equivalent of the AliRoot task UserCreateOutputObjects
  void init(o2::framework::InitContext&)
  {
    // Histogram is added to the ouput registry
    histos.add("p", "Momentum distribution;#it{p} (GeV/#it{c})", kTH1F, {{100, 0, 20}});
  }

  // Equivalent of the AliRoot task UserExec
  void process(aod::Tracks const& inputTracks)
  {
    for (auto track : inputTracks) { // Loop over tracks
      histos.fill(HIST("p"), track.p());
    }
  }
};

WorkflowSpec defineDataProcessing(ConfigContext const& cfgc) // This puts your task in the DPL workflow
{
  // Equivalent to the AddTask in AliPhysics
  WorkflowSpec workflow{adaptAnalysisTask<MyAwesomeTask>(cfgc)};
  return workflow;
}

```

Now you can compile the O2 __from your parent directory__ `aliBuild build O2 --defaults o2`

Once this is done you can enter the O2 environment `alienv enter O2/latest`

## Downloading the input data
Once you entered the O2 environment you can download the input data from alien (requires certificate)

__Data__: `alien_cp alien:///alice/data/2015/LHC15o/000244918/pass5_lowIR/PWGZZ/Run3_Conversion/148_20210304-0829_child_1/0017/AO2D.root /tmp/AO2D_data.root`

__MC__: `alien_cp alien:///alice/sim/2020/LHC20f6/244918/PWGZZ/Run3_Conversion/151_20210305-2339/0017/AO2D.root /tmp/AO2D_mc.root`

__N.B.__ This can take a while!

If you did not manage to get the input files from alien you can do try with:

__Data__: `https://cernbox.cern.ch/index.php/s/EEQoTmZ1bVNtbnH`

__MC__: `https://cernbox.cern.ch/index.php/s/JvACZuc26GITAYQ`

## Running your task

While in the O2 environment
`o2-analysis-my-awesome-task --aod-file AO2D_data.root`

If running on a remote machine `o2-analysis-my-awesome-task --aod-file AO2D_data.root -b`

You should get an output file `AnalysisResults.root` you can inspect it via `TBrowser`

## Extending the workflow

Now you can extend the workflow, adding the PID response task for TOF to your task `o2-analysis-my-awesome-task --aod-file AO2D_data.root | o2-analysis-pid-tof --add-qa 1`

In addition to the usual output you should get an additional file `QAResults.root` with the TOF PID QA histograms, you can inspect it via `TBrowser`

## Extending the task

Now subscribe the task also to collisions!

```c++:
  // Equivalent of the AliRoot task UserExec
  void process(aod::Collision const& coll, aod::Tracks const& inputTracks)
  {
    for (auto track : inputTracks) { // Loop over tracks
      histos.fill(HIST("p"), track.p());
    }
  }
```

## First checkpoint: subscribing to collisions and tracks

`wget https://raw.githubusercontent.com/njacazio/AliceO2/LF-workshop/Analysis/Tasks/LFTutorial/LFTutorialTask0.cxx && mv -S 0 --backup LFTutorialTask0.cxx MyAwesomeTask.cxx`

after rebuilding O2

`o2-analysis-my-awesome-task --aod-file AO2D_data.root`

The vertex information should be available in the output file


## Second checkpoint: using Joined tables to get the pion PID response information from tracks

`wget https://raw.githubusercontent.com/njacazio/AliceO2/LF-workshop/Analysis/Tasks/LFTutorial/LFTutorialTask1.cxx && mv -S 1 --backup LFTutorialTask1.cxx MyAwesomeTask.cxx`

after rebuilding O2

`o2-analysis-my-awesome-task --aod-file AO2D_data.root | o2-analysis-centrality-table | o2-analysis-multiplicity-table | o2-analysis-timestamp | o2-analysis-event-selection | o2-analysis-pid-tof-split --pid-pi 1`

## Third checkpoint: applying some custom Filter via task Configurables

`wget https://raw.githubusercontent.com/njacazio/AliceO2/LF-workshop/Analysis/Tasks/LFTutorial/LFTutorialTask2.cxx && mv -S 2 --backup LFTutorialTask2.cxx MyAwesomeTask.cxx`

after rebuilding O2

`o2-analysis-my-awesome-task --aod-file AO2D_data.root | o2-analysis-centrality-table | o2-analysis-multiplicity-table | o2-analysis-timestamp | o2-analysis-event-selection | o2-analysis-pid-tof-split --pid-pi 1`

## Fourth checkpoint: analyzing MC

`wget https://raw.githubusercontent.com/njacazio/AliceO2/LF-workshop/Analysis/Tasks/LFTutorial/LFTutorialTask3.cxx && mv -S 3 --backup LFTutorialTask3.cxx MyAwesomeTask.cxx`

after rebuilding O2

`o2-analysis-my-awesome-task --aod-file AO2D_mc.root | o2-analysis-centrality-table | o2-analysis-multiplicity-table | o2-analysis-timestamp | o2-analysis-event-selection | o2-analysis-pid-tof-split --pid-pi 1 | o2-analysis-trackextension`

