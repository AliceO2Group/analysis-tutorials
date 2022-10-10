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
/// \brief this is a starting point for the third session of the tutorial
/// \author
/// \since

#include "Framework/runDataProcessing.h"
#include "Framework/AnalysisTask.h"
#include "Common/DataModel/TrackSelectionTables.h"
#include "PWGLF/DataModel/LFStrangenessTables.h"
#include "Common/DataModel/EventSelection.h"
#include "Common/DataModel/PIDResponse.h"

using namespace o2;
using namespace o2::framework;
using namespace o2::framework::expressions;

//STEP 5
//Use MC label of V0s to fill pid histograms 
//based on MC true information
using MyTracksRun2 = soa::Join<aod::Tracks, aod::TracksExtra, aod::TracksCov, aod::TracksDCA, aod::pidTPCPi, aod::pidTPCPr>;
using MyTracksRun3 = soa::Join<aod::TracksIU, aod::TracksExtra, aod::TracksCovIU, aod::TracksDCA, aod::pidTPCPi, aod::pidTPCPr>;
using LabeledV0s = soa::Join<aod::V0Datas, aod::McV0Labels>;

struct vzeromcexample {
  //Configurable for number of bins
  Configurable<int> nBins{"nBins", 100, "N bins in all histos"};
  
  // Selection criteria: 5 basic V0 selection criteria!
  Configurable<double> v0cospa{"v0cospa", 0.97, "V0 CosPA"}; // double -> N.B. dcos(x)/dx = 0 at x=0)
  Configurable<float> dcav0dau{"dcav0dau", 1.0, "DCA V0 Daughters"};
  Configurable<float> dcanegtopv{"dcanegtopv", .1, "DCA Neg To PV"};
  Configurable<float> dcapostopv{"dcapostopv", .1, "DCA Pos To PV"};
  Configurable<float> v0radius{"v0radius", 0.5, "v0radius"};

  //Cannot filter on dynamic columns, so we cut on DCA to PV and DCA between daus only!
  Filter preFilterV0 = nabs(aod::v0data::dcapostopv) > dcapostopv&& nabs(aod::v0data::dcanegtopv) > dcanegtopv&& aod::v0data::dcaV0daughters < dcav0dau;
  
  // histogram defined with HistogramRegistry
  HistogramRegistry registry{
    "registry",
    {
      {"hVertexZ", "hVertexZ", {HistType::kTH1F, {{nBins, -15., 15.}}}},
      {"hMassK0Short", "hMassK0Short", {HistType::kTH1F, {{200, 0.450f, 0.550f}}}},
      {"hMassLambda", "hMassLambda", {HistType::kTH1F, {{200, 1.015f, 1.215f}}}},
      {"hMassAntiLambda", "hMassAntiLambda", {HistType::kTH1F, {{200, 1.015f, 1.215f}}}},
      {"hMassTrueK0Short", "hMassTrueK0Short", {HistType::kTH1F, {{200, 0.450f, 0.550f}}}},
      {"hMassTrueLambda", "hMassTrueLambda", {HistType::kTH1F, {{200, 1.015f, 1.215f}}}},
      {"hMassTrueAntiLambda", "hMassTrueAntiLambda", {HistType::kTH1F, {{200, 1.015f, 1.215f}}}}
    }
  };
  
  template <class TMyTracks, typename TV0>
  void processV0Candidate(TV0 const& v0, float const& pvx, float const& pvy, float const& pvz)
  //function to process a vzero candidate freely, actually with the right track type!
  {
    auto posTrackCast = v0.template posTrack_as<TMyTracks>();
    auto negTrackCast = v0.template negTrack_as<TMyTracks>();
    
    float nsigma_pos_proton = TMath::Abs(posTrackCast.tpcNSigmaPr());
    float nsigma_neg_proton = TMath::Abs(posTrackCast.tpcNSigmaPr());
    float nsigma_pos_pion = TMath::Abs(negTrackCast.tpcNSigmaPi());
    float nsigma_neg_pion = TMath::Abs(negTrackCast.tpcNSigmaPi());
    
    if (v0.v0radius() > v0radius && v0.v0cosPA(pvx, pvy, pvz) > v0cospa){
      if( nsigma_pos_pion < 4 && nsigma_neg_pion < 4 ){
        registry.fill(HIST("hMassK0Short"), v0.mK0Short());
      }
      if( nsigma_pos_proton < 4 && nsigma_neg_pion < 4 ){
        registry.fill(HIST("hMassLambda"), v0.mLambda());
      }
      if( nsigma_pos_pion < 4 && nsigma_neg_proton < 4 ){
        registry.fill(HIST("hMassAntiLambda"), v0.mAntiLambda());
      }
      
      if( v0.has_mcParticle()){ //<- some association was made!
        auto v0mcparticle = v0.mcParticle();
        //check particle PDG code to see if it's the one you want
        if ( v0mcparticle.pdgCode() == 310 ) registry.fill(HIST("hMassTrueK0Short"), v0.mK0Short());
        if ( v0mcparticle.pdgCode() == 3122 ) registry.fill(HIST("hMassLambda"), v0.mK0Short());
        if ( v0mcparticle.pdgCode() ==-3122 ) registry.fill(HIST("hMassAntiLambda"), v0.mK0Short());
      }
      
    }
  }
  
  //define first process function, used to process Run2 data
  void processRun2(soa::Join<aod::Collisions, aod::EvSels>::iterator const& collision, soa::Filtered<LabeledV0s> const& V0s, MyTracksRun2 const& tracks)
  {
    //Basic event selection (all helper tasks are now included!)
    if (!collision.sel7()) {
      return;
    }
    //check getter here: https://aliceo2group.github.io/analysis-framework/docs/datamodel/ao2dTables.html
    registry.get<TH1>(HIST("hVertexZ"))->Fill(collision.posZ());
    for (auto& v0 : V0s) {
      processV0Candidate<MyTracksRun2>(v0, collision.posX(), collision.posY(), collision.posZ());
    }
  }
  PROCESS_SWITCH(vzeromcexample, processRun2, "Process Run 2 data", false);

  //define first process function, used to process Run3 data
  void processRun3(soa::Join<aod::Collisions, aod::EvSels>::iterator const& collision, soa::Filtered<LabeledV0s> const& V0s, MyTracksRun3 const& tracks)
  {
    //Basic event selection (all helper tasks are now included!)
    if (!collision.sel8()) {
      return;
    }
    //check getter here: https://aliceo2group.github.io/analysis-framework/docs/datamodel/ao2dTables.html
    registry.get<TH1>(HIST("hVertexZ"))->Fill(collision.posZ());
    for (auto& v0 : V0s) {
      processV0Candidate<MyTracksRun3>(v0, collision.posX(), collision.posY(), collision.posZ());
    }
  }
  PROCESS_SWITCH(vzeromcexample, processRun3, "Process Run 3 data", true);
  
};

WorkflowSpec defineDataProcessing(ConfigContext const& cfgc)
{
  return WorkflowSpec{
    adaptAnalysisTask<vzeromcexample>(cfgc)
  };
}
