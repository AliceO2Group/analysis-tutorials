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
#include "Common/DataModel/TrackSelectionTables.h"

using namespace o2;
using namespace o2::framework;

//STEP 7
//This more sophisticated example exemplifies the access of MC information
//to calculate a simple transverse momentum resolution histogram.
//
//N.B.: This is still a *simple* example, meaning no extra workflows are required.
//Therefore, tracks are used at their innermost update (IU) and not closest to the
//primary vertex. This will change slightly the momentum of low-momentum tracks
//if material corrections are used. The usage of tracks propagated to the
//primary vertex will be covered in a later tutorial. 
struct momentumresolution {
  //Configurable for number of bins
  Configurable<int> nBinsEta{"nBinsEta", 100, "N bins in eta histo"};
  Configurable<int> nBinsPt{"nBinsPt", 100, "N bins in pT histo"};
  
  // histogram defined with HistogramRegistry
  HistogramRegistry registry{
    "registry",
    {
      {"hVertexZ", "hVertexZ", {HistType::kTH1F, {{120, -15., 15.}}}},
      {"etaHistogram", "etaHistogram", {HistType::kTH1F, {{nBinsEta, -1., +1}}}},
      {"ptHistogram", "ptHistogram", {HistType::kTH1F, {{nBinsPt, 0., 10.0}}}},
      {"resoHistogram", "resoHistogram", {HistType::kTH2F, {{nBinsPt, 0., 10.0}, {100, -10.0, 10.0}}}}
    }
  };

  void process(aod::Collision const& collision, soa::Join<aod::Tracks, aod::TracksExtra, aod::TracksDCA, aod::McTrackLabels> const& tracks, aod::McParticles const&) //<- this is the main change
  {
    //Fill the event counter
    //check getter here: https://aliceo2group.github.io/analysis-framework/docs/datamodel/ao2dTables.html
    registry.get<TH1>(HIST("hVertexZ"))->Fill(collision.posZ());
    //This will take place once per event!
    for (auto& track : tracks) {
      if( track.tpcNClsCrossedRows() < 70 ) continue; //skip stuff not tracked well by TPC
      if( fabs(track.dcaXY()) > .2 ) continue; //skip stuff that doesn't point to PV (example, can be elaborate!)
      registry.get<TH1>(HIST("etaHistogram"))->Fill(track.eta());
      registry.get<TH1>(HIST("ptHistogram"))->Fill(track.pt());
      
      //Resolve MC track - no need to touch index!
      auto mcParticle = track.mcParticle_as<aod::McParticles>();
      
      //Very rough momentum resolution
      float delta = track.pt() - mcParticle.pt();
      registry.get<TH2>(HIST("resoHistogram"))->Fill(track.pt(), delta);
    }
  }
};

WorkflowSpec defineDataProcessing(ConfigContext const& cfgc)
{
  return WorkflowSpec{
    adaptAnalysisTask<momentumresolution>(cfgc)
  };
}
