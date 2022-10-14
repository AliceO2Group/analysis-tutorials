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

using namespace o2;
using namespace o2::framework;
using namespace o2::framework::expressions;

//STEP 2
//V0 property filtering and selecting

struct vzerofilterexample {
  //Configurable for number of bins
  Configurable<int> nBins{"nBins", 100, "N bins in all histos"};
  
  // Selection criteria: 5 basic V0 selection criteria!
  Configurable<double> v0cospa{"v0cospa", 0.97, "V0 CosPA"}; // double -> N.B. dcos(x)/dx = 0 at x=0)
  Configurable<float> dcav0dau{"dcav0dau", 1.0, "DCA V0 Daughters"};
  Configurable<float> dcanegtopv{"dcanegtopv", .1, "DCA Neg To PV"};
  Configurable<float> dcapostopv{"dcapostopv", .1, "DCA Pos To PV"};
  Configurable<float> v0radius{"v0radius", 0.5, "v0radius"};

  //Cannot filter on dynamic columns, so we cut on DCA to PV and DCA between daus only!
  Filter preFilterV0 = nabs(aod::v0data::dcapostopv) > dcapostopv&&
                       nabs(aod::v0data::dcanegtopv) > dcanegtopv&&
                       aod::v0data::dcaV0daughters < dcav0dau;
  
  // histogram defined with HistogramRegistry
  HistogramRegistry registry{
    "registry",
    {
      {"hVertexZ", "hVertexZ", {HistType::kTH1F, {{nBins, -15., 15.}}}},
      {"hMassK0Short", "hMassK0Short", {HistType::kTH1F, {{200, 0.450f, 0.550f}}}}
    }
  };

  void process(soa::Join<aod::Collisions, aod::EvSels>::iterator const& collision, soa::Filtered<aod::V0Datas> const& V0s)
  {
    //Basic event selection (all helper tasks are now included!)
    if (!collision.sel8()) {
      return;
    }
    //check getter here: https://aliceo2group.github.io/analysis-framework/docs/datamodel/ao2dTables.html
    registry.get<TH1>(HIST("hVertexZ"))->Fill(collision.posZ());
    for (auto& v0 : V0s) {
      if (v0.v0radius() > v0radius && v0.v0cosPA(collision.posX(), collision.posY(), collision.posZ()) > v0cospa){
        registry.fill(HIST("hMassK0Short"), v0.mK0Short());
      }
    }
  }
};

WorkflowSpec defineDataProcessing(ConfigContext const& cfgc)
{
  return WorkflowSpec{
    adaptAnalysisTask<vzerofilterexample>(cfgc)
  };
}
