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

//STEP 4
//The usage of the "HistogramRegistry" output class is
//preferred versus OutputObj, since the number of OutputObj slots
//is typically limited but the HistogramRegistry is not really
//limited and has full flexibility for storing histograms
//(up to memory limitations, of course, which are relevant
//in case very very large histograms are created!)
struct histogramregistry {
  //Configurable for number of bins
  Configurable<int> nBinsEta{"nBinsEta", 100, "N bins in eta histo"};
  Configurable<int> nBinsPt{"nBinsPt", 100, "N bins in pT histo"};

  // histogram defined with HistogramRegistry
  HistogramRegistry registry{ //<- example of histogram registry
    "registry",
    {
      {"etaHistogram", "etaHistogram", {HistType::kTH1F, {{nBinsEta, -1., +1}}}},
      {"ptHistogram", "ptHistogram", {HistType::kTH1F, {{nBinsPt, 0., 10.0}}}}
    }
  };

  void process(aod::TracksIU const& tracks)
  {
    for (auto& track : tracks) {
      registry.get<TH1>(HIST("etaHistogram"))->Fill(track.eta());
      registry.get<TH1>(HIST("ptHistogram"))->Fill(track.pt());
    }
  }
};

WorkflowSpec defineDataProcessing(ConfigContext const& cfgc)
{
  return WorkflowSpec{
    adaptAnalysisTask<histogramregistry>(cfgc)
  };
}
