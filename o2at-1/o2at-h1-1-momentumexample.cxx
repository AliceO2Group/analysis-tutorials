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
/// \brief This task refers to step 1 of the analysis tutorial.
///        The user is supposed to include a pT histogram. 
/// \author
/// \since

#include "Framework/runDataProcessing.h"
#include "Framework/AnalysisTask.h"

using namespace o2;
using namespace o2::framework;

//STEP 1
//This showcases how to add simple histograms to get information about tracks.
//It is meant to be executed unassisted, i.e. no extra workflows are necessary.
//It subscribes only to the most simple tables.
//Please keep in mind that for full analysis, further steps need to be taken!
struct momentumexample {
  // histogram created with OutputObj<TH1F>
  OutputObj<TH1F> etaHistogram{TH1F("etaHistogram", "etaHistogram", 200, -1., +1)};
  OutputObj<TH1F> ptHistogram{TH1F("ptHistogram", "ptHistogram", 100, 0., 10.0)};

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
    adaptAnalysisTask<momentumexample>(cfgc)
  };
}
