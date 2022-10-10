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

//STEP 5
//This example changes the input information from the simple TracksIU table
//to additional information, such as collisions and extra track information.
//By mentioning collisions before tracks, the framework also understands that
//tracks should be grouped to appropriately indexed collisions: this means
//that the process function will be invoked once per collision, and each time
//it is invoked, only the tracks indexed to that particular collision will be
//made available.
struct subscriptionexample {
  //Configurable for number of bins
  Configurable<int> nBinsEta{"nBinsEta", 100, "N bins in eta histo"};
  Configurable<int> nBinsPt{"nBinsPt", 100, "N bins in pT histo"};

  // histogram defined with HistogramRegistry
  HistogramRegistry registry{
    "registry",
    {
      {"hVertexZ", "hVertexZ", {HistType::kTH1F, {{120, -15., 15.}}}},
      {"etaHistogram", "etaHistogram", {HistType::kTH1F, {{nBinsEta, -1., +1}}}},
      {"ptHistogram", "ptHistogram", {HistType::kTH1F, {{nBinsPt, 0., 10.0}}}}
    }
  };

  void process(aod::Collision const& collision, soa::Join<aod::TracksIU, aod::TracksExtra> const& tracks) //<- this is the main change
  {
    //Fill the event counter
    //check getter here: https://aliceo2group.github.io/analysis-framework/docs/datamodel/ao2dTables.html
    registry.get<TH1>(HIST("hVertexZ"))->Fill(collision.posZ());
    //This will take place once per event!
    for (auto& track : tracks) {
      if( track.tpcNClsCrossedRows() < 70 ) continue; //skip stuff not tracked well by TPC
      registry.get<TH1>(HIST("etaHistogram"))->Fill(track.eta());
      registry.get<TH1>(HIST("ptHistogram"))->Fill(track.pt());
    }
  }
};

WorkflowSpec defineDataProcessing(ConfigContext const& cfgc)
{
  return WorkflowSpec{
    adaptAnalysisTask<subscriptionexample>(cfgc)
  };
}
