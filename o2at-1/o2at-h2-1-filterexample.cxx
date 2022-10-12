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
///        in the second part of the tutorial. These are 5 steps, and at the end,
///        the participant is expected to have a two-particle correlation spectrum.
/// \author
/// \since

#include "Framework/runDataProcessing.h"
#include "Framework/AnalysisTask.h"
#include "Common/DataModel/TrackSelectionTables.h"
#include "Framework/ASoAHelpers.h"

using namespace o2;
using namespace o2::framework;
using namespace o2::framework::expressions;

//This is an example of a conveient declaration of "using"
using MyCompleteTracks = soa::Join<aod::Tracks, aod::TracksExtra, aod::TracksDCA>;

//STEP 1
//This is a simple fitler example
//Don't forget to add soa::Filtered in the subscription!
//Otherwise, you'll get an error.
//Careful with tpc crossed rows: one cannot filter on dynamic columns
//Therefore, one has to write the expression to filter on by hand
struct filterexample {
  Filter etaFilter = nabs(aod::track::eta) < 0.5f;
  Filter trackDCA = nabs(aod::track::dcaXY) < 0.2f;

  //Configurable for number of bins
  Configurable<int> nBins{"nBins", 100, "N bins in all histos"};

  // histogram defined with HistogramRegistry
  HistogramRegistry registry{
    "registry",
    {
      {"hVertexZ", "hVertexZ", {HistType::kTH1F, {{nBins, -15., 15.}}}},
      {"etaHistogram", "etaHistogram", {HistType::kTH1F, {{nBins, -1., +1}}}},
      {"ptHistogram", "ptHistogram", {HistType::kTH1F, {{nBins, 0., 10.0}}}}
    }
  };

  void process(aod::Collision const& collision, soa::Filtered<MyCompleteTracks> const& tracks)
  {
    registry.get<TH1>(HIST("hVertexZ"))->Fill(collision.posZ());
    //This will take place once per event!
    for (auto& track : tracks) {
      if(track.tpcNClsCrossedRows() < 70 ) continue; //can't filter on dynamic
      registry.get<TH1>(HIST("etaHistogram"))->Fill(track.eta()); //<- this should show the selection
      registry.get<TH1>(HIST("ptHistogram"))->Fill(track.pt());
    }
  }
};

WorkflowSpec defineDataProcessing(ConfigContext const& cfgc)
{
  return WorkflowSpec{
    adaptAnalysisTask<filterexample>(cfgc)
  };
}
