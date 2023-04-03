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
/// \brief Joined tables can be used as argument to the process function.
/// \author
/// \since

#include "Framework/runDataProcessing.h"
#include "Framework/AnalysisTask.h"

#include "Common/DataModel/Centrality.h"

using namespace o2;
using namespace o2::framework;

struct firsto2physics {
  Configurable<float> cfgZVtxCut = {"zvtxcut", 7.0, "Vertex z cut. Default 7 cm"};
  Configurable<float> cfgPtCutMin = {"minpt", 0.2, "Minimum accepted track pT. Default 0.2 GeV"};
  Configurable<float> cfgPtCutMax = {"maxpt", 5.0, "Maximum accepted track pT. Default 5.0 GeV"};
  Configurable<float> cfgEtaCut = {"etacut", 0.8, "Eta cut. Default 0.8"};

  HistogramRegistry registry{"MyFirstO2TaskResults", {}, OutputObjHandlingPolicy::AnalysisObject, true, true};
  void init(InitContext&)
  {
    registry.add("before/reco/zvtx", "vtx_{#it{z}}", kTH1F, {{300, -15.0, 15.0, "vtx_{#it{z}} (cm)"}});
    registry.add("before/reco/multiplicity", "V0M multiplicity class", kTH1F, {{100, 0.0, 100.0, "V0M multiplicity (%)"}});
    registry.add("before/reco/pt", "Charged particles #it{p}_{T}", kTH1F, {{150, 0.0, 15.0, "#it{p}_{T} (GeV/#it{c})"}});
    registry.add("before/reco/eta", "Charged particles #eta", kTH1F, {{400, -2.0, 2.0, "#eta"}});
    registry.add("before/reco/phi", "Charged particles #varphi", kTH1F, {{360, 0.0, constants::math::TwoPI, "#varphi"}});

    registry.addClone("before/reco/", "after/reco/");
    registry.get<TH1>(HIST("after/reco/zvtx"))->GetXaxis()->SetRangeUser(-cfgZVtxCut, cfgZVtxCut);
    registry.get<TH1>(HIST("after/reco/pt"))->GetXaxis()->SetRangeUser(cfgPtCutMin, cfgPtCutMax);
    registry.get<TH1>(HIST("after/reco/eta"))->GetXaxis()->SetRangeUser(-cfgEtaCut, cfgEtaCut);

    registry.addClone("before/reco/", "before/gen/");
    registry.addClone("after/reco/", "after/gen/");
  }

  template <typename CollisionInstance>
  bool isGenCollisionAccepted(CollisionInstance const& col)
  {
    bool accepted = true;

    return accepted;
  }

  template <typename ParticleInstance>
  bool isParticleAccepted(ParticleInstance const& particle)
  {
    bool accepted = true;

    return accepted;
  }

  template <typename CollisionInstance>
  bool isCollisionAccepted(CollisionInstance const& col)
  {
    bool accepted = true;

    return accepted;
  }

  template <typename TrackInstance>
  bool isTrackAccepted(TrackInstance const& track)
  {
    if constexpr (framework::has_type_v<aod::mctracklabel::McParticleId, typename TrackInstance::all_columns>) {
      if (track.mcParticleId() < 0) {
        return false;
      }
      auto particle = track.mcParticle();
      if (particle.pdgCode() == 411) {
        LOGF(info, "Got a pion");
      }
    }
    bool accepted = true;

    return accepted;
  }

  template <typename CollisionInstance, typename TracksListInstance>
  void processReconstructed(CollisionInstance const& collision, TracksListInstance const& tracks)
  {
    registry.fill(HIST("before/reco/zvtx"), collision.posZ());
    registry.fill(HIST("before/reco/multiplicity"), collision.centRun2V0M());

    if (isCollisionAccepted(collision)) {
      LOGF(debug, "Collision with global index %ld accepted", collision.globalIndex());

      registry.fill(HIST("after/reco/zvtx"), collision.posZ());
      registry.fill(HIST("after/reco/multiplicity"), collision.centRun2V0M());

      for (auto const& track : tracks) {
        registry.fill(HIST("before/reco/pt"), track.pt());
        registry.fill(HIST("before/reco/eta"), track.eta());
        registry.fill(HIST("before/reco/phi"), track.phi());

        if (isTrackAccepted(track)) {
          registry.fill(HIST("after/reco/pt"), track.pt());
          registry.fill(HIST("after/reco/eta"), track.eta());
          registry.fill(HIST("after/reco/phi"), track.phi());
        }
      }
    }
  }

  using CollisionInstance = soa::Join<aod::Collisions, aod::CentRun2V0Ms>::iterator;

  void processRecoData(CollisionInstance const& collision, aod::Tracks const& tracks)
  {
    processReconstructed(collision, tracks);
  }
  PROCESS_SWITCH(firsto2physics, processRecoData, "Process data reconstructed collisions and tracks", true);

  void processDetectorLevel(CollisionInstance const& collision, soa::Join<aod::Tracks, aod::McTrackLabels> const& tracks, aod::McParticles const&)
  {
    processReconstructed(collision, tracks);
  }
  PROCESS_SWITCH(firsto2physics, processDetectorLevel, "Process MC detector level collisions and tracks", false);

  void processGen(aod::McCollision const& gencollision, aod::McParticles const& particles)
  {
    registry.fill(HIST("before/gen/zvtx"), gencollision.posZ());

    if (isGenCollisionAccepted(gencollision)) {

      registry.fill(HIST("after/gen/zvtx"), gencollision.posZ());

      for (auto const& particle : particles) {
        registry.fill(HIST("before/gen/pt"), particle.pt());
        registry.fill(HIST("before/gen/eta"), particle.eta());
        registry.fill(HIST("before/gen/phi"), particle.phi());

        if (isParticleAccepted(particle)) {
          registry.fill(HIST("after/gen/pt"), particle.pt());
          registry.fill(HIST("after/gen/eta"), particle.eta());
          registry.fill(HIST("after/gen/phi"), particle.phi());
        }
      }
    }
  }
  PROCESS_SWITCH(firsto2physics, processGen, "Process reconstructed collisions and tracks", false);
};

WorkflowSpec defineDataProcessing(ConfigContext const& cfgc)
{
  return WorkflowSpec{
    adaptAnalysisTask<firsto2physics>(cfgc),
  };
}
