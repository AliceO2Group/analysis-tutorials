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
using namespace o2::framework::expressions;


//STEP 1
//This is an example of a conveient declaration of "using"
using MyCompleteTracks = soa::Join<aod::Tracks, aod::TracksExtra, aod::TracksDCA>;

//This is a simple fitler example
//Don't forget to add soa::Filtered in the subscription!
//Otherwise, you'll get an error.
//Careful with tpc crossed rows: one cannot filter on dynamic columns
//Therefore, one has to write the expression to filter on by hand
//When necessary, check the data model online:
// https://aliceo2group.github.io/analysis-framework/docs/datamodel/ao2dTables.html
struct FilterTrackExample {
  Filter etaFilter = nabs(aod::track::eta) < 0.5f;
  Filter trackQuality = aod::track::tpcNClsFindable - aod::track::tpcNClsFindableMinusCrossedRows >= 70;
  Filter trackDCA = nabs(aod::track::dcaXY) <= .2;

  //Configurable for number of bins
  Configurable<int> nBins1{"nBins1", 100, "N bins in all histos"};

  // histogram defined with HistogramRegistry
  HistogramRegistry registry{
    "registry",
    {
      {"hVertexZ1", "hVertexZ1", {HistType::kTH1F, {{nBins1, -15., 15.}}}},
      {"phiHistogram1", "phiHistogram1", {HistType::kTH1F, {{nBins1, 0., 2. * M_PI}}}},
      {"etaHistogram1", "etaHistogram1", {HistType::kTH1F, {{nBins1, -1., +1}}}},
      {"ptHistogram1", "ptHistogram1", {HistType::kTH1F, {{nBins1, 0., 10.0}}}}
    }
  };

  void process(aod::Collision const& collision, soa::Filtered<MyCompleteTracks> const& tracks) //<- this is the main change
  {
    //Fill the event counter
    //check getter here: https://aliceo2group.github.io/analysis-framework/docs/datamodel/ao2dTables.html
    registry.get<TH1>(HIST("hVertexZ1"))->Fill(collision.posZ());
    //This will take place once per event!
    for (auto& track : tracks) {
      registry.get<TH1>(HIST("phiHistogram1"))->Fill(track.phi());
      registry.get<TH1>(HIST("etaHistogram1"))->Fill(track.eta()); //<- this should show the selection
      registry.get<TH1>(HIST("ptHistogram1"))->Fill(track.pt());
    }
  }
};

//STEP 2: partition
//This is a plain partitioning example.
//Note that the subscription to tracks is actually unchanged,
//but the usage of a specific partition inside the process function can now be used.
//This is all pretty similar to PYTHON list handling.
struct PartitionTrackExample {
  Partition<o2::aod::Tracks> leftTracks = aod::track::eta < 0;
  Partition<o2::aod::Tracks> rightTracks = aod::track::eta >= 0;

  //Configurable for number of bins
  Configurable<int> nBins2{"nBins2", 100, "N bins in all histos"};

  // histogram defined with HistogramRegistry
  HistogramRegistry registry{
    "registry",
    {
      {"hVertexZ2", "hVertexZ2", {HistType::kTH1F, {{nBins2, -15., 15.}}}},
      {"phiHistogram2left", "phiHistogram2left", {HistType::kTH1F, {{nBins2, 0., 2. * M_PI}}}},
      {"etaHistogram2left", "etaHistogram2left", {HistType::kTH1F, {{nBins2, -1., +1}}}},
      {"ptHistogram2left", "ptHistogram2left", {HistType::kTH1F, {{nBins2, 0., 10.0}}}},
      {"phiHistogram2right", "phiHistogram2right", {HistType::kTH1F, {{nBins2, 0., 2. * M_PI}}}},
      {"etaHistogram2right", "etaHistogram2right", {HistType::kTH1F, {{nBins2, -1., +1}}}},
      {"ptHistogram2right", "ptHistogram2right", {HistType::kTH1F, {{nBins2, 0., 10.0}}}}
      
    }
  };

  void process(aod::Collision const& collision, soa::Filtered<MyCompleteTracks> const& tracks)
  {
    //Fill the event counter
    //check getter here: https://aliceo2group.github.io/analysis-framework/docs/datamodel/ao2dTables.html
    registry.get<TH1>(HIST("hVertexZ2"))->Fill(collision.posZ());
    //This will take place once per event!
    for (auto track : leftTracks) { //<- only for a subset
      registry.get<TH1>(HIST("phiHistogram2left"))->Fill(track.phi());
      registry.get<TH1>(HIST("etaHistogram2left"))->Fill(track.eta()); //<- this should show the selection
      registry.get<TH1>(HIST("ptHistogram2left"))->Fill(track.pt());
    }
    for (auto track : rightTracks) { //<- only for a subset
      registry.get<TH1>(HIST("phiHistogram2right"))->Fill(track.phi());
      registry.get<TH1>(HIST("etaHistogram2right"))->Fill(track.eta()); //<- this should show the selection
      registry.get<TH1>(HIST("ptHistogram2right"))->Fill(track.pt());
    }
  }
};

//STEP 3: Partition and Filter simultaneously (and check results)
//This is a logical combination of filtering and partitioning.
//In practice, the partitions act on top of the already filtered data.
//This example also provides specific histograms to inspect the outcome.
struct PartitionAndFilterTrackExample {
  Partition<o2::aod::Tracks> leftTracks = aod::track::eta < 0;
  Partition<o2::aod::Tracks> rightTracks = aod::track::eta >= 0;
  Filter etaFilter = nabs(aod::track::eta) < 0.5f;
  Filter trackQuality = aod::track::tpcNClsFindable - aod::track::tpcNClsFindableMinusCrossedRows >= 70;
  Filter trackDCA = nabs(aod::track::dcaXY) <= .2;

  //Configurable for number of bins
  Configurable<int> nBins3{"nBins3", 100, "N bins in all histos"};

  // histogram defined with HistogramRegistry
  HistogramRegistry registry{
    "registry",
    {
      {"hVertexZ3", "hVertexZ3", {HistType::kTH1F, {{nBins3, -15., 15.}}}},
      {"phiHistogram3left", "phiHistogram3left", {HistType::kTH1F, {{nBins3, 0., 2. * M_PI}}}},
      {"etaHistogram3left", "etaHistogram3left", {HistType::kTH1F, {{nBins3, -1., +1}}}},
      {"ptHistogram3left", "ptHistogram3left", {HistType::kTH1F, {{nBins3, 0., 10.0}}}},
      {"phiHistogram3right", "phiHistogram3right", {HistType::kTH1F, {{nBins3, 0., 2. * M_PI}}}},
      {"etaHistogram3right", "etaHistogram3right", {HistType::kTH1F, {{nBins3, -1., +1}}}},
      {"ptHistogram3right", "ptHistogram3right", {HistType::kTH1F, {{nBins3, 0., 10.0}}}}
      
    }
  };

  void process(aod::Collision const& collision, soa::Filtered<MyCompleteTracks> const& tracks)
  {
    //Fill the event counter
    //check getter here: https://aliceo2group.github.io/analysis-framework/docs/datamodel/ao2dTables.html
    registry.get<TH1>(HIST("hVertexZ3"))->Fill(collision.posZ());
    //This will take place once per event!
    for (auto track : leftTracks) { //<- only for a subset
      registry.get<TH1>(HIST("phiHistogram3left"))->Fill(track.phi());
      registry.get<TH1>(HIST("etaHistogram3left"))->Fill(track.eta()); //<- this should show the selection
      registry.get<TH1>(HIST("ptHistogram3left"))->Fill(track.pt());
    }
    for (auto track : rightTracks) { //<- only for a subset
      registry.get<TH1>(HIST("phiHistogram3right"))->Fill(track.phi());
      registry.get<TH1>(HIST("etaHistogram3right"))->Fill(track.eta()); //<- this should show the selection
      registry.get<TH1>(HIST("ptHistogram3right"))->Fill(track.pt());
    }
  }
};

//STEP 4: Use to write two-particle correlation
//This is a simple two-particle correlation function filler
//that makes use of both filters and partitions.
//The core part of the 2pc filling still uses a nested loop.
struct TwoParticleCorrelation {
  //Fully declarative!
  Partition<o2::aod::Tracks> triggerTracks = aod::track::pt > 2;
  Partition<o2::aod::Tracks> assocTracks = aod::track::pt < 2;
  Filter etaFilter = nabs(aod::track::eta) < 0.5f;
  Filter trackQuality = aod::track::tpcNClsFindable - aod::track::tpcNClsFindableMinusCrossedRows >= 70;
  Filter trackDCA = nabs(aod::track::dcaXY) <= .2;

  //Configurable for number of bins
  Configurable<int> nBins4{"nBins4", 100, "N bins in all histos"};

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
  
  // histogram defined with HistogramRegistry
  HistogramRegistry registry{
    "registry",
    {
      {"hVertexZ4", "hVertexZ4", {HistType::kTH1F, {{nBins4, -15., 15.}}}},
      {"phiHistogramTrigger", "phiHistogramTrigger", {HistType::kTH1F, {{nBins4, 0., 2. * M_PI}}}},
      {"etaHistogramTrigger", "etaHistogramTrigger", {HistType::kTH1F, {{nBins4, -1., +1}}}},
      {"ptHistogramTrigger", "ptHistogramTrigger", {HistType::kTH1F, {{nBins4, 0., 10.0}}}},
      {"phiHistogramAssoc", "phiHistogramAssoc", {HistType::kTH1F, {{nBins4, 0., 2. * M_PI}}}},
      {"etaHistogramAssoc", "etaHistogramAssoc", {HistType::kTH1F, {{nBins4, -1., +1}}}},
      {"ptHistogramAssoc", "ptHistogramAssoc", {HistType::kTH1F, {{nBins4, 0., 10.0}}}},
      {"correlationFunction", "correlationFunction", {HistType::kTH2F, {{40, -1.6, 1.6}, {40,-0.5*M_PI, 1.5*M_PI}}}}
      
    }
  };

  void process(aod::Collision const& collision, soa::Filtered<MyCompleteTracks> const& tracks)
  {
    //Fill the event counter
    //check getter here: https://aliceo2group.github.io/analysis-framework/docs/datamodel/ao2dTables.html
    registry.get<TH1>(HIST("hVertexZ4"))->Fill(collision.posZ());
    //Inspect the trigger and associated populations
    for (auto track : triggerTracks) { //<- only for a subset
      registry.get<TH1>(HIST("phiHistogramTrigger"))->Fill(track.phi());
      registry.get<TH1>(HIST("etaHistogramTrigger"))->Fill(track.eta()); //<- this should show the selection
      registry.get<TH1>(HIST("ptHistogramTrigger"))->Fill(track.pt());
    }
    for (auto track : assocTracks) { //<- only for a subset
      registry.get<TH1>(HIST("phiHistogramAssoc"))->Fill(track.phi());
      registry.get<TH1>(HIST("etaHistogramAssoc"))->Fill(track.eta()); //<- this should show the selection
      registry.get<TH1>(HIST("ptHistogramAssoc"))->Fill(track.pt());
    }
    //Now we do two-particle correlations, but still manually
    for (auto trackTrigger : triggerTracks) { //<- only for trgger
      for (auto trackAssoc : assocTracks) { //<- only for associated
        registry.get<TH2>(HIST("correlationFunction"))->Fill(
                                                             trackTrigger.eta()-trackAssoc.eta(),
                                                             ComputeDeltaPhi(trackTrigger.phi(), trackAssoc.phi() ));
      }
    }
  }
};

//STEP 4: Use to write two-particle correlation but with combination
//This is a simple two-particle correlation function filler
//that makes use of both filters and partitions.
//The core part of the 2pc filling now utilises a combination declaration
//that is in principle more efficient.
struct TwoParticleCorrelationCombination {
  //Fully declarative!
  Partition<o2::aod::Tracks> triggerTracks = aod::track::pt > 2;
  Partition<o2::aod::Tracks> assocTracks = aod::track::pt < 2;
  Filter etaFilter = nabs(aod::track::eta) < 0.5f;
  Filter trackQuality = aod::track::tpcNClsFindable - aod::track::tpcNClsFindableMinusCrossedRows >= 70;
  Filter trackDCA = nabs(aod::track::dcaXY) <= .2;
  //Configurable for number of bins
  Configurable<int> nBins5{"nBins5", 100, "N bins in all histos"};
  // histogram defined with HistogramRegistry
  HistogramRegistry registry{
    "registry",
    {
      {"hVertexZ5", "hVertexZ5", {HistType::kTH1F, {{nBins5, -15., 15.}}}},
      {"phiHistogramTrigger2", "phiHistogramTrigger2", {HistType::kTH1F, {{nBins5, 0., 2. * M_PI}}}},
      {"etaHistogramTrigger2", "etaHistogramTrigger2", {HistType::kTH1F, {{nBins5, -1., +1}}}},
      {"ptHistogramTrigger2", "ptHistogramTrigger2", {HistType::kTH1F, {{nBins5, 0., 10.0}}}},
      {"phiHistogramAssoc2", "phiHistogramAssoc2", {HistType::kTH1F, {{nBins5, 0., 2. * M_PI}}}},
      {"etaHistogramAssoc2", "etaHistogramAssoc2", {HistType::kTH1F, {{nBins5, -1., +1}}}},
      {"ptHistogramAssoc2", "ptHistogramAssoc2", {HistType::kTH1F, {{nBins5, 0., 10.0}}}},
      {"correlationFunction2", "correlationFunction2", {HistType::kTH2F, {{40, -1.6, 1.6}, {40,-0.5*M_PI, 1.5*M_PI}}}}
      
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

  void process(aod::Collision const& collision, soa::Filtered<MyCompleteTracks> const& tracks) //<- this is the main change
  {
    //Fill the event counter
    //check getter here: https://aliceo2group.github.io/analysis-framework/docs/datamodel/ao2dTables.html
    registry.get<TH1>(HIST("hVertexZ5"))->Fill(collision.posZ());
    
    //Inspect the trigger and associated populations
    for (auto track : triggerTracks) { //<- only for a subset
      registry.get<TH1>(HIST("phiHistogramTrigger2"))->Fill(track.phi());
      registry.get<TH1>(HIST("etaHistogramTrigger2"))->Fill(track.eta()); //<- this should show the selection
      registry.get<TH1>(HIST("ptHistogramTrigger2"))->Fill(track.pt());
    }
    for (auto track : assocTracks) { //<- only for a subset
      registry.get<TH1>(HIST("phiHistogramAssoc2"))->Fill(track.phi());
      registry.get<TH1>(HIST("etaHistogramAssoc2"))->Fill(track.eta()); //<- this should show the selection
      registry.get<TH1>(HIST("ptHistogramAssoc2"))->Fill(track.pt());
    }
    
    //Now we do two-particle correlations, but still manually
    for (auto& [trackTrigger, trackAssoc] : combinations(triggerTracks, assocTracks)) {
        registry.get<TH2>(HIST("correlationFunction2"))->Fill(
                                                             trackTrigger.eta()-trackAssoc.eta(),
                                                             ComputeDeltaPhi(trackTrigger.phi(), trackAssoc.phi() ));
    }
  }
};

WorkflowSpec defineDataProcessing(ConfigContext const& cfgc)
{
  return WorkflowSpec{
    adaptAnalysisTask<FilterTrackExample>(cfgc),
    adaptAnalysisTask<PartitionTrackExample>(cfgc),
    adaptAnalysisTask<PartitionAndFilterTrackExample>(cfgc),
    adaptAnalysisTask<TwoParticleCorrelation>(cfgc),
    adaptAnalysisTask<TwoParticleCorrelationCombination>(cfgc),
  };
}
