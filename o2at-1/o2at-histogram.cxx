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

//STEP 1
//This showcases how to add simple histograms to get information about tracks.
//It is meant to be executed unassisted, i.e. no extra workflows are necessary.
//It subscribes only to the most simple tables.
//Please keep in mind that for full analysis, further steps need to be taken!
struct OutputObjects {
  // histogram created with OutputObj<TH1F>
  OutputObj<TH1F> phiHistogram1{TH1F("phiHistogram1", "phiHistogram1", 100, 0., 2. * M_PI)};
  OutputObj<TH1F> etaHistogram1{TH1F("etaHistogram1", "etaHistogram1", 200, -1., +1)};
  OutputObj<TH1F> ptHistogram1{TH1F("ptHistogram1", "ptHistogram1", 100, 0., 10.0)};

  void process(aod::TracksIU const& tracks)
  {
    for (auto& track : tracks) {
      phiHistogram1->Fill(track.phi());
      etaHistogram1->Fill(track.eta());
      ptHistogram1->Fill(track.pt());
    }
  }
};

//STEP 2
//In this example, the user initialises the histograms using
//the init function of the task struct. This is another alternative.
struct OutputObjectsInit {
  // incomplete definition of an OutputObj
  OutputObj<TH1F> phiHistogram2{"phiHistogram2"};
  OutputObj<TH1F> etaHistogram2{"etaHistogram2"};
  OutputObj<TH1F> ptHistogram2{"ptHistogram2"};

  void init(InitContext const&) //<- example of different initialisation
  {
    // complete the definition of the OutputObj
    phiHistogram2.setObject(new TH1F("phiHistogram2", "phiHistogram2", 100, 0., 2. * M_PI));
    etaHistogram2.setObject(new TH1F("etaHistogram2", "etaHistogram2", 200, -1., +1));
    ptHistogram2.setObject(new TH1F("ptHistogram2", "ptHistogram2", 100, 0., 10.0));
  }

  void process(aod::TracksIU const& tracks)
  {
    for (auto& track : tracks) {
      phiHistogram2->Fill(track.phi());
      etaHistogram2->Fill(track.eta());
      ptHistogram2->Fill(track.pt());
    }
  }
};

//STEP 3
//This further step adds 'configurables', which allow the user to
//change the operating parameters of the task as desired and requred
//without needing to actually change code for that. The configurables
//can be set at runtime.
struct OutputObjectsConfig {
  //Configurable for number of bins
  Configurable<int> nBinsPhi3{"nBinsPhi3", 100, "N bins in phi histo"}; //<- example of configurable
  Configurable<int> nBinsEta3{"nBinsEta3", 100, "N bins in eta histo"};
  Configurable<int> nBinsPt3{"nBinsPt3", 100, "N bins in pT histo"};
  
  // incomplete definition of an OutputObj
  OutputObj<TH1F> phiHistogram3{"phiHistogram3"};
  OutputObj<TH1F> etaHistogram3{"etaHistogram3"};
  OutputObj<TH1F> ptHistogram3{"ptHistogram3"};

  void init(InitContext const&)
  {
    // complete the definition of the OutputObj
    phiHistogram3.setObject(new TH1F("phiHistogram3", "phiHistogram3", nBinsPhi3, 0., 2. * M_PI));
    etaHistogram3.setObject(new TH1F("etaHistogram3", "etaHistogram3", nBinsEta3, -1., +1));
    ptHistogram3.setObject(new TH1F("ptHistogram3", "ptHistogram3", nBinsPt3, 0., 10.0));
  }

  void process(aod::TracksIU const& tracks)
  {
    for (auto& track : tracks) {
      phiHistogram3->Fill(track.phi());
      etaHistogram3->Fill(track.eta());
      ptHistogram3->Fill(track.pt());
    }
  }
};

//STEP 4
//The usage of the "HistogramRegistry" output class is
//preferred versuys OutputObj, since the number of OutputObj slots
//is typically limited but the HistogramRegistry is not really
//limited and has full flexibility for storing histograms
//(up to memory limitations, of course, which are relevant
//in case very very large histograms are created!)
struct OutputRegistry {
  //Configurable for number of bins
  Configurable<int> nBinsPhi4{"nBinsPhi4", 100, "N bins in phi histo"};
  Configurable<int> nBinsEta4{"nBinsEta4", 100, "N bins in eta histo"};
  Configurable<int> nBinsPt4{"nBinsPt4", 100, "N bins in pT histo"};

  // histogram defined with HistogramRegistry
  HistogramRegistry registry{ //<- example of histogram registry
    "registry",
    {
      {"phiHistogram4", "phiHistogram4", {HistType::kTH1F, {{nBinsPhi4, 0., 2. * M_PI}}}},
      {"etaHistogram4", "etaHistogram4", {HistType::kTH1F, {{nBinsEta4, -1., +1}}}},
      {"ptHistogram4", "ptHistogram4", {HistType::kTH1F, {{nBinsPt4, 0., 10.0}}}}
    }
  };

  void process(aod::TracksIU const& tracks)
  {
    for (auto& track : tracks) {
      registry.get<TH1>(HIST("phiHistogram4"))->Fill(track.phi());
      registry.get<TH1>(HIST("etaHistogram4"))->Fill(track.eta());
      registry.get<TH1>(HIST("ptHistogram4"))->Fill(track.pt());
    }
  }
};

//STEP 5
//This example changes the input information from the simple TracksIU table
//to additional information, such as collisions and extra track information.
//By mentioning collisions before tracks, the framework also understands that
//tracks should be grouped to appropriately indexed collisions: this means
//that the proess function will be invoked once per collision, and each time
//it is invoked, only the tracks indexed to that particular collision will be
//made available.
struct SubscriptionExample {
  //Configurable for number of bins
  Configurable<int> nBinsPhi5{"nBinsPhi5", 100, "N bins in phi histo"};
  Configurable<int> nBinsEta5{"nBinsEta5", 100, "N bins in eta histo"};
  Configurable<int> nBinsPt5{"nBinsPt5", 100, "N bins in pT histo"};

  // histogram defined with HistogramRegistry
  HistogramRegistry registry{
    "registry",
    {
      {"hVertexZ5", "hVertexZ5", {HistType::kTH1F, {{120, -15., 15.}}}},
      {"phiHistogram5", "phiHistogram5", {HistType::kTH1F, {{nBinsPhi5, 0., 2. * M_PI}}}},
      {"etaHistogram5", "etaHistogram5", {HistType::kTH1F, {{nBinsEta5, -1., +1}}}},
      {"ptHistogram5", "ptHistogram5", {HistType::kTH1F, {{nBinsPt5, 0., 10.0}}}}
    }
  };

  void process(aod::Collision const& collision, soa::Join<aod::TracksIU, aod::TracksExtra> const& tracks) //<- this is the main change
  {
    //Fill the event counter
    //check getter here: https://aliceo2group.github.io/analysis-framework/docs/datamodel/ao2dTables.html
    registry.get<TH1>(HIST("hVertexZ5"))->Fill(collision.posZ());
    //This will take place once per event!
    for (auto& track : tracks) {
      if( track.tpcNClsCrossedRows() < 70 ) continue; //skip stuff not tracked well by TPC
      registry.get<TH1>(HIST("phiHistogram5"))->Fill(track.phi());
      registry.get<TH1>(HIST("etaHistogram5"))->Fill(track.eta());
      registry.get<TH1>(HIST("ptHistogram5"))->Fill(track.pt());
    }
  }
};

//STEP 6
//This example now adds a DCA cut and subscribes to the different Tracks (not IU) table.
//This is the very first instance of helper tasks being required by the user!
//made available.
struct HelperExample {
  //Configurable for number of bins
  Configurable<int> nBinsPhi5{"nBinsPhi5", 100, "N bins in phi histo"};
  Configurable<int> nBinsEta5{"nBinsEta5", 100, "N bins in eta histo"};
  Configurable<int> nBinsPt5{"nBinsPt5", 100, "N bins in pT histo"};

  // histogram defined with HistogramRegistry
  HistogramRegistry registry{
    "registry",
    {
      {"hVertexZ5", "hVertexZ5", {HistType::kTH1F, {{120, -15., 15.}}}},
      {"phiHistogram5", "phiHistogram5", {HistType::kTH1F, {{nBinsPhi5, 0., 2. * M_PI}}}},
      {"etaHistogram5", "etaHistogram5", {HistType::kTH1F, {{nBinsEta5, -1., +1}}}},
      {"ptHistogram5", "ptHistogram5", {HistType::kTH1F, {{nBinsPt5, 0., 10.0}}}}
    }
  };

  void process(aod::Collision const& collision, soa::Join<aod::Tracks, aod::TracksExtra, aod::TracksDCA> const& tracks) //<- this is the main change
  {
    //Fill the event counter
    //check getter here: https://aliceo2group.github.io/analysis-framework/docs/datamodel/ao2dTables.html
    registry.get<TH1>(HIST("hVertexZ5"))->Fill(collision.posZ());
    //This will take place once per event!
    for (auto& track : tracks) {
      if( track.tpcNClsCrossedRows() < 70 ) continue; //skip stuff not tracked well by TPC
      if( fabs(track.dcaXY()) > .2 ) continue; //skip stuff that doesn't point to PV (example, can be elaborate!)
      registry.get<TH1>(HIST("phiHistogram5"))->Fill(track.phi());
      registry.get<TH1>(HIST("etaHistogram5"))->Fill(track.eta());
      registry.get<TH1>(HIST("ptHistogram5"))->Fill(track.pt());
    }
  }
};

//STEP 7
//This more sophisticated example exemplifies the access of MC information
//to calculate a simple transverse momentum resolution histogram.
//
//N.B.: This is still a *simple* example, meaning no extra workflows are required.
//Therefore, tracks are used at their innermost update (IU) and not closest to the
//primary vertex. This will change slightly the momentum of low-momentum tracks
//if material corrections are used. The usage of tracks propagated to the
//primary vertex will be covered in a later tutorial. 
struct MCAccessExample {
  //Configurable for number of bins
  Configurable<int> nBinsPhi7{"nBinsPhi7", 100, "N bins in phi histo"};
  Configurable<int> nBinsEta7{"nBinsEta7", 100, "N bins in eta histo"};
  Configurable<int> nBinsPt7{"nBinsPt7", 100, "N bins in pT histo"};

  // histogram defined with HistogramRegistry
  HistogramRegistry registry{
    "registry",
    {
      {"hVertexZ7", "hVertexZ7", {HistType::kTH1F, {{120, -15., 15.}}}},
      {"phiHistogram7", "phiHistogram7", {HistType::kTH1F, {{nBinsPhi6, 0., 2. * M_PI}}}},
      {"etaHistogram7", "etaHistogram7", {HistType::kTH1F, {{nBinsEta6, -1., +1}}}},
      {"ptHistogram7", "ptHistogram7", {HistType::kTH1F, {{nBinsPt6, 0., 10.0}}}},
      {"resoHistogram7", "resoHistogram7", {HistType::kTH2F, {{nBinsPt6, 0., 10.0}, {100, -10.0, 10.0}}}}
    }
  };

  void process(aod::Collision const& collision, soa::Join<aod::Tracks, aod::TracksExtra, aod::TracksDCA, aod::McTrackLabels> const& tracks, aod::McParticles const&) //<- this is the main change
  {
    //Fill the event counter
    //check getter here: https://aliceo2group.github.io/analysis-framework/docs/datamodel/ao2dTables.html
    registry.get<TH1>(HIST("hVertexZ7"))->Fill(collision.posZ());
    //This will take place once per event!
    for (auto& track : tracks) {
      if( track.tpcNClsCrossedRows() < 70 ) continue; //skip stuff not tracked well by TPC
      if( fabs(track.dcaXY()) > .2 ) continue; //skip stuff that doesn't point to PV (example, can be elaborate!)
      registry.get<TH1>(HIST("phiHistogram7"))->Fill(track.phi());
      registry.get<TH1>(HIST("etaHistogram7"))->Fill(track.eta());
      registry.get<TH1>(HIST("ptHistogram7"))->Fill(track.pt());
      
      //Resolve MC track - no need to touch index!
      auto mcParticle = track.mcParticle_as<aod::McParticles>();
      
      //Very rough momentum resolution
      float delta = track.pt() - mcParticle.pt();
      registry.get<TH2>(HIST("resoHistogram7"))->Fill(track.pt(), delta);
    }
  }
};

WorkflowSpec defineDataProcessing(ConfigContext const& cfgc)
{
  return WorkflowSpec{
    adaptAnalysisTask<OutputObjects>(cfgc),
    adaptAnalysisTask<OutputObjectsInit>(cfgc),
    adaptAnalysisTask<OutputObjectsConfig>(cfgc),
    adaptAnalysisTask<OutputRegistry>(cfgc),
    adaptAnalysisTask<SubscriptionExample>(cfgc),
    adaptAnalysisTask<MCAccessExample>(cfgc)
  };
}
