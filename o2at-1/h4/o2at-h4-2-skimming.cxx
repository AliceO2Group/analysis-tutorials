// Copyright 2019-2020 CERN and copyright holders of ALICE O2.
// See https://alice-o2.web.cern.ch/copyright for details of the copyright
// holders. All rights not expressly granted are reserved.
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
#include "Framework/HistogramRegistry.h"
#include "PWGHF/DataModel/CandidateReconstructionTables.h"
#include "PWGHF/DataModel/CandidateSelectionTables.h"

using namespace o2;
using namespace o2::framework;
using namespace o2::framework::expressions;

// STEP 2
//<- starting point, define the derived table to be stored
// this can be done in a separated header file, but for semplicity we do it in
// the same file here

namespace o2::aod
{
namespace mytable
{
DECLARE_SOA_COLUMN(InvMassD0, invMassD0, float);           //!
DECLARE_SOA_COLUMN(InvMassD0bar, invMassD0bar, float);     //!
DECLARE_SOA_COLUMN(Pt, pt, float);                         //!
DECLARE_SOA_COLUMN(CosinePointing, cosinePointing, float); //!
DECLARE_SOA_INDEX_COLUMN(Collision, collision);            //!
} // namespace mytable

DECLARE_SOA_TABLE(MyTable, "AOD", "MYTABLE", //!
                  mytable::InvMassD0,
                  mytable::InvMassD0bar,
                  mytable::Pt,
                  mytable::CosinePointing,
                  mytable::CollisionId)

} // namespace o2::aod

struct ProduceDerivedTable { //<- workflow that loops over HF 2-prong
                             // candidates and fills a derived table

  Produces<aod::MyTable> tableWithDzeroCandidates;

  void process(aod::HfCand2Prong const& cand2Prongs, aod::Tracks const&)
  {

    // loop over 2-prong candidates
    for (auto& cand : cand2Prongs) {

      // check first if the HF 2-prong candidate is tagged as a D0
      bool isD0Sel = TESTBIT(cand.hfflag(), aod::hf_cand_2prong::DecayType::D0ToPiK);

      // let's select only D0 andidates with pT > 4 GeV/c
      if (!isD0Sel || cand.pt() < 4.) {
        continue;
      }

      auto invMassD0 = invMassD0ToPiK(cand);
      auto invMassD0bar = invMassD0barToKPi(cand);

      LOG(debug) << "Candidate with mass(D0) = " << invMassD0
                 << ", mass(D0bar) = " << invMassD0bar
                 << ", pt = " << cand.pt()
                 << ", cos(theta_P) = " << cand.cpa();

      // we retrieve also the event index from one of the daughters
      auto dauTrack = cand.prong0_as<aod::Tracks>(); // positive daughter

      tableWithDzeroCandidates(invMassD0, invMassD0bar, cand.pt(), cand.cpa(), dauTrack.collisionId());
    }
  }
};

WorkflowSpec defineDataProcessing(ConfigContext const& cfgc)
{
  return WorkflowSpec{adaptAnalysisTask<ProduceDerivedTable>(cfgc)};
}
