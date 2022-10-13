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

//STEP 4: Use to write two-particle correlation using for loops
//This is a simple two-particle correlation function filler
//that makes use of both filters and partitions.
//The core part of the 2pc filling utilises two for loops.
struct twoparcorexample {
  // all defined filters are applied
  Filter etaFilter = nabs(aod::track::eta) < 0.5f;
  Filter trackDCA = nabs(aod::track::dcaXY) < 0.2f;
  using MyFilteredTracks = soa::Filtered<MyCompleteTracks>;
  Partition<MyFilteredTracks> triggerTracks = aod::track::pt > 2.0f;
  Partition<MyFilteredTracks> assocTracks = aod::track::pt < 2.0f;
  
  //Configurable for number of bins
  Configurable<int> nBins{"nBins", 100, "N bins in all histos"};
  // histogram defined with HistogramRegistry
  HistogramRegistry registry{
    "registry",
    {
      {"hVertexZ", "hVertexZ", {HistType::kTH1F, {{nBins, -15., 15.}}}},
      {"etaHistogramTrigger", "etaHistogramTrigger", {HistType::kTH1F, {{nBins, -1., +1}}}},
      {"ptHistogramTrigger", "ptHistogramTrigger", {HistType::kTH1F, {{nBins, 0., 10.0}}}},
      {"etaHistogramAssoc", "etaHistogramAssoc", {HistType::kTH1F, {{nBins, -1., +1}}}},
      {"ptHistogramAssoc", "ptHistogramAssoc", {HistType::kTH1F, {{nBins, 0., 10.0}}}},
      {"correlationFunction", "correlationFunction", {HistType::kTH1F, {{40,-0.5*M_PI, 1.5*M_PI}}}}
      
    }
  };
  
  Double_t ComputeDeltaPhi( Double_t phi1, Double_t phi2) {
      //To be completely sure, use inner products
      Double_t x1, y1, x2, y2;
      x1 = TMath::Cos( phi1 );
      y1 = TMath::Sin( phi1 );
      x2 = TMath::Cos( phi2 );
      y2 = TMath::Sin( phi2 );
      Double_t lInnerProd  = x1*x2 + y1*y2;
      Double_t lVectorProd = x1*y2 - x2*y1;
      
      Double_t lReturnVal = 0;
      if( lVectorProd > 1e-8 ) {
          lReturnVal = TMath::ACos(lInnerProd);
      }
      if( lVectorProd < -1e-8 ) {
          lReturnVal = -TMath::ACos(lInnerProd);
      }
      
      if( lReturnVal < -TMath::Pi()/2. ) {
          lReturnVal += 2.*TMath::Pi();
      }
      
      return lReturnVal;
  }

  void process(aod::Collision const& collision, MyFilteredTracks const& tracks) //<- this is the main change
  {
    //Fill the event counter
    //check getter here: https://aliceo2group.github.io/analysis-framework/docs/datamodel/ao2dTables.html
    registry.get<TH1>(HIST("hVertexZ"))->Fill(collision.posZ());
    
    //Inspect the trigger and associated populations
    for (auto& track : triggerTracks) { //<- only for a subset
      if(track.tpcNClsCrossedRows() < 70 ) continue; //can't filter on dynamic
      registry.get<TH1>(HIST("etaHistogramTrigger"))->Fill(track.eta()); //<- this should show the selection
      registry.get<TH1>(HIST("ptHistogramTrigger"))->Fill(track.pt());
    }
    for (auto& track : assocTracks) { //<- only for a subset
      if(track.tpcNClsCrossedRows() < 70 ) continue; //can't filter on dynamic
      registry.get<TH1>(HIST("etaHistogramAssoc"))->Fill(track.eta()); //<- this should show the selection
      registry.get<TH1>(HIST("ptHistogramAssoc"))->Fill(track.pt());
    }
    
    //Now we do two-particle correlations, but still manually
    for (auto trackTrigger : triggerTracks) { //<- only for trigger
      if(trackTrigger.tpcNClsCrossedRows() < 70 ) continue; //can't filter on dynamic
      for (auto trackAssoc : assocTracks) { //<- only for associated
        if(trackAssoc.tpcNClsCrossedRows() < 70 ) continue; //can't filter on dynamic
        registry.get<TH1>(HIST("correlationFunction"))->Fill(ComputeDeltaPhi(trackTrigger.phi(), trackAssoc.phi() ));
      }
    }
  }
};

WorkflowSpec defineDataProcessing(ConfigContext const& cfgc)
{
  return WorkflowSpec{
    adaptAnalysisTask<twoparcorexample>(cfgc)
  };
}
