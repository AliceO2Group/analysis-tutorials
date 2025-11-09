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

using namespace o2;
using namespace o2::framework;

struct myExampleTask {
  // Histogram registry: an object to hold your histograms
  HistogramRegistry histos{"histos", {}, OutputObjHandlingPolicy::AnalysisObject};

  Configurable<int> nBinsPt{"nBinsPt", 100, "N bins in pT histo"};
  Configurable<float> maxDCAxy{"maxDCAxy", 0.2, "max DCAxy (in cm)"};
  Configurable<float> minTPCCrossedRows{"minTPCCrossedRows", 70, "min crossed rows in the TPC"};
  Configurable<float> maxPVz{"maxPVz", 100.0f, "max value of PVz (cm)"};

  void init(InitContext const&)
  {
    // define axes you want to use
    const AxisSpec axisCounter{1, 0.0f, +1.0f, ""};
    const AxisSpec axisEta{30, -1.5f, +1.5f, "#eta"};
    const AxisSpec axisPt{nBinsPt, 0.0f, 10.0f, "p_{T}"};
    // create histograms
    histos.add("eventCounter", "eventCounter", kTH1F, {axisCounter});
    histos.add("etaHistogram", "etaHistogram", kTH1F, {axisEta});
    histos.add("ptHistogram", "ptHistogram", kTH1F, {axisPt});
  }

  void process(aod::Collision const&, soa::Join<aod::Tracks, aod::TracksExtra, aod::TracksDCA> const& tracks)
  {
    if(fabs(collision.posZ())>maxPVz){
      return; // skip outside a certain region
    }
    histos.fill(HIST("eventCounter"), 0.5f);
    for (auto& track : tracks) {
      if( track.tpcNClsCrossedRows() < minTPCCrossedRows ) continue; //badly tracked
      if( fabs(track.dcaXY()) > maxDCAxy) continue; //doesn’t point to primary vertex
      histos.fill(HIST("etaHistogram"), track.eta());
      histos.fill(HIST("ptHistogram"), track.pt());
    }
  }
};

WorkflowSpec defineDataProcessing(ConfigContext const& cfgc)
{
  return WorkflowSpec{
    adaptAnalysisTask<myExampleTask>(cfgc)};
}
