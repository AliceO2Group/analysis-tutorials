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
/// \brief This task is an empty skeleton that fills a simple eta histogram.
///        it is meant to be a blank page for further developments.
/// \author everyone

#include "Framework/runDataProcessing.h"
#include "Framework/AnalysisTask.h"
#include "Common/DataModel/TrackSelectionTables.h"
#include "Framework/ASoAHelpers.h"

using namespace o2;
using namespace o2::framework;
using namespace o2::framework::expressions;

namespace o2::aod
{
// helper for finding out which tracks are ambiguous
namespace helperSpace
{
// Global bool
DECLARE_SOA_COLUMN(IsAmbiguous, isAmbiguous, bool); //! is this ambiguous? 
}
DECLARE_SOA_TABLE(HelperTable, "AOD", "HELPERTABLE",
                  helperSpace::IsAmbiguous);
}

struct myExampleProducer {
  Produces<aod::HelperTable> myHelperTable;
  void init(InitContext const&) {}
  void process(o2::aod::Tracks const& tracks, o2::aod::AmbiguousTracks const& ambiTracks)
  {
    std::vector<bool> ambiguousOrNot;
    ambiguousOrNot.reserve(tracks.size());
    for (int ii = 0; ii < tracks.size(); ii++)
      ambiguousOrNot[ii] = false;
    for (auto const& ambiTrack : ambiTracks) {
      ambiguousOrNot[ambiTrack.trackId()] = true;
    }
    for (int ii = 0; ii < tracks.size(); ii++) {
      myHelperTable(ambiguousOrNot[ii]);
    }
  }
};

//This is an example of a convenient declaration of "using"
using myCompleteTracks = soa::Join<aod::Tracks, aod::TracksExtra, aod::TracksDCA, aod::HelperTable>;

struct myExampleTask {
  // Histogram registry: an object to hold your histograms
  HistogramRegistry histos{"histos", {}, OutputObjHandlingPolicy::AnalysisObject};

  Configurable<int> nBinsPt{"nBinsPt", 100, "N bins in pT histo"};

  Filter trackDCA = nabs(aod::track::dcaXY) < 0.2f;
  Filter trackIsAmbi = aod::helperSpace::isAmbiguous == true;
  using myFilteredTracks = soa::Filtered<myCompleteTracks>;

  void init(InitContext const&)
  {
    // define axes you want to use
    const AxisSpec axisCounter{1, 0, +1, ""};
    const AxisSpec axisEta{30, -1.5, +1.5, "#eta"};
    const AxisSpec axisPt{nBinsPt, 0, 10, "p_{T}"};
    // create histograms
    histos.add("eventCounter", "eventCounter", kTH1F, {axisCounter});
    histos.add("etaHistogramAmbi", "etaHistogramAmbi", kTH1F, {axisEta});
    histos.add("ptHistogramAmbi", "ptHistogramAmbi", kTH1F, {axisPt});
  }

  void process(myFilteredTracks const& tracks, o2::aod::AmbiguousTracks const& ambiTracks)
  {
    for (const auto& track : tracks) {
      if( track.tpcNClsCrossedRows() < 70 ) continue; //badly tracked

      if( track.isAmbiguous() ){
        histos.fill(HIST("etaHistogramAmbi"), track.eta());
        histos.fill(HIST("ptHistogramAmbi"), track.pt());
      }
    }
  }
};

WorkflowSpec defineDataProcessing(ConfigContext const& cfgc)
{
  return WorkflowSpec{
    adaptAnalysisTask<myExampleProducer>(cfgc),
    adaptAnalysisTask<myExampleTask>(cfgc)};
}

