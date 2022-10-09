// Copyright 2019-2020 CERN and copyright holders of ALICE O2.
// See https://alice-o2.web.cern.ch/copyright for details of the copyright holders.
// All rights not expressly granted are reserved.
//
// This software is distributed under the terms of the GNU General Public
// License v3 (GPL Version 3), copied verbatim in the file "COPYING".
//
// In applying this license CERN does not waive the privileges and immunities
// granted to it by virtue of its status as an Intergovernmental Organization
// or submit itself to any jurisdiction.
///
/// \brief This task contains the individual steps that are to be taken
///        in the first part of the tutorial. These are 6 steps, and at the end,
///        the participant is expected to have a pT resolution histogram
///        as a function of transverse momentum.
/// \author
/// \since

#include "Framework/runDataProcessing.h"
#include "Framework/AnalysisTask.h"

using namespace o2;
using namespace o2::framework;

//STEP 2
//In this example, the user initialises the histograms using
//the init function of the task struct. This is another alternative.
struct initexample {
  // incomplete definition of an OutputObj
  OutputObj<TH1F> etaHistogram{"etaHistogram"};
  OutputObj<TH1F> ptHistogram{"ptHistogram"};

  void init(InitContext const&) //<- example of different initialisation
  {
    // complete the definition of the OutputObj
    etaHistogram.setObject(new TH1F("etaHistogram", "etaHistogram", 200, -1., +1));
    ptHistogram.setObject(new TH1F("ptHistogram", "ptHistogram", 100, 0., 10.0));
  }

  void process(aod::TracksIU const& tracks)
  {
    for (auto& track : tracks) {
      etaHistogram->Fill(track.eta());
      ptHistogram->Fill(track.pt());
    }
  }
};
WorkflowSpec defineDataProcessing(ConfigContext const& cfgc)
{
  return WorkflowSpec{
    adaptAnalysisTask<initexample>(cfgc)
  };
}
