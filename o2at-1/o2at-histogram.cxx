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
struct OutputObjects { //<- basic starting point, ask for addition of pT histogram
  // histogram created with OutputObj<TH1F>
  OutputObj<TH1F> phiHistogram1{TH1F("phiHistogram1", "phiHistogram1", 100, 0., 2. * M_PI)};
  OutputObj<TH1F> etaHistogram1{TH1F("etaHistogram1", "etaHistogram1", 200, -1., +1)};
  OutputObj<TH1F> ptHistogram1{TH1F("ptHistogram1", "ptHistogram1", 100, 0., 10.0)};

  void process(aod::Tracks const& tracks)
  {
    for (auto& track : tracks) {
      phiHistogram1->Fill(track.phi());
      etaHistogram1->Fill(track.eta());
      ptHistogram1->Fill(track.pt());
    }
  }
};

//STEP 2
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

  void process(aod::Tracks const& tracks)
  {
    for (auto& track : tracks) {
      phiHistogram2->Fill(track.phi());
      etaHistogram2->Fill(track.eta());
      ptHistogram2->Fill(track.pt());
    }
  }
};

//STEP 3
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

  void process(aod::Tracks const& tracks)
  {
    for (auto& track : tracks) {
      phiHistogram3->Fill(track.phi());
      etaHistogram3->Fill(track.eta());
      ptHistogram3->Fill(track.pt());
    }
  }
};

//STEP 4
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

  void process(aod::Tracks const& tracks)
  {
    for (auto& track : tracks) {
      registry.get<TH1>(HIST("phiHistogram4"))->Fill(track.phi());
      registry.get<TH1>(HIST("etaHistogram4"))->Fill(track.eta());
      registry.get<TH1>(HIST("ptHistogram4"))->Fill(track.pt());
    }
  }
};

//STEP 5
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

  void process(aod::Collision const& collision, aod::Tracks const& tracks) //<- this is the main change
  {
    //Fill the event counter
    //check getter here: https://aliceo2group.github.io/analysis-framework/docs/datamodel/ao2dTables.html
    registry.get<TH1>(HIST("hVertexZ5"))->Fill(collision.posZ());
    //This will take place once per event!
    for (auto& track : tracks) {
      registry.get<TH1>(HIST("phiHistogram5"))->Fill(track.phi());
      registry.get<TH1>(HIST("etaHistogram5"))->Fill(track.eta());
      registry.get<TH1>(HIST("ptHistogram5"))->Fill(track.pt());
    }
  }
};

//STEP 6
struct MCAccessExample {
  //Configurable for number of bins
  Configurable<int> nBinsPhi6{"nBinsPhi6", 100, "N bins in phi histo"};
  Configurable<int> nBinsEta6{"nBinsEta6", 100, "N bins in eta histo"};
  Configurable<int> nBinsPt6{"nBinsPt6", 100, "N bins in pT histo"};

  // histogram defined with HistogramRegistry
  HistogramRegistry registry{
    "registry",
    {
      {"hVertexZ6", "hVertexZ6", {HistType::kTH1F, {{120, -15., 15.}}}},
      {"phiHistogram6", "phiHistogram6", {HistType::kTH1F, {{nBinsPhi6, 0., 2. * M_PI}}}},
      {"etaHistogram6", "etaHistogram6", {HistType::kTH1F, {{nBinsEta6, -1., +1}}}},
      {"ptHistogram6", "ptHistogram6", {HistType::kTH1F, {{nBinsPt6, 0., 10.0}}}},
      {"resoHistogram6", "resoHistogram6", {HistType::kTH2F, {{nBinsPt6, 0., 10.0}, {100, -10.0, 10.0}}}}
    }
  };

  void process(aod::Collision const& collision, soa::Join<aod::Tracks, aod::McTrackLabels> const& tracks, aod::McParticles const&) //<- this is the main change
  {
    //Fill the event counter
    //check getter here: https://aliceo2group.github.io/analysis-framework/docs/datamodel/ao2dTables.html
    registry.get<TH1>(HIST("hVertexZ6"))->Fill(collision.posZ());
    //This will take place once per event!
    for (auto& track : tracks) {
      registry.get<TH1>(HIST("phiHistogram6"))->Fill(track.phi());
      registry.get<TH1>(HIST("etaHistogram6"))->Fill(track.eta());
      registry.get<TH1>(HIST("ptHistogram6"))->Fill(track.pt());
      
      //Resolve MC track - no need to touch index!
      auto mcParticle = track.mcParticle_as<aod::McParticles>();
      
      //Momentum resolution should always be done via pT/delta(1/pT)
      float delta = 1./track.pt() - 1./mcParticle.pt();
      registry.get<TH2>(HIST("resoHistogram6"))->Fill(track.pt(), delta);
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
