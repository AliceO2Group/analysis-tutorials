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

#include <climits>
#include <cstdlib>
#include <map>
#include <memory>
#include <sstream>
#include <string>
#include <vector>

#include "Math/Vector4D.h"

#include "Framework/ASoA.h"
#include "Framework/AnalysisDataModel.h"
#include "Framework/AnalysisTask.h"
#include "Framework/HistogramRegistry.h"
#include "Framework/runDataProcessing.h"

#include "Common/DataModel/Centrality.h"
#include "Common/DataModel/EventSelection.h"
#include "Common/DataModel/PIDResponse.h"

#include "DataFormatsEMCAL/AnalysisCluster.h"
#include "DataFormatsEMCAL/Cell.h"
#include "DataFormatsEMCAL/Constants.h"
#include "EMCALBase/Geometry.h"
#include "EMCALCalib/BadChannelMap.h"
#include "PWGJE/DataModel/EMCALClusters.h"

#include "CommonDataFormat/InteractionRecord.h"

// \struct ClusterTutorial
/// \brief Skeleton task for EMCal cluster tutorial
/// \author Florian Jonas <florian.jonas@cern.ch>, Oak Ridge National Laoratory; Joshua König <joshua.konig@cern.ch>, Goethe-University Frankfurt; Marvin Hemmer <marvin.hemmer@cern.ch>, Goethe-University Frankfurt
/// \since 09.04.2023
using namespace o2::framework;
using namespace o2::framework::expressions;
using collisionEvSelIt = o2::soa::Join<o2::aod::Collisions, o2::aod::EvSels>::iterator;

// we need to filter the emcal clusters, since we only want clusters from one specific clusterizer algorithm
using selectedClusters = o2::soa::Filtered<o2::aod::EMCALClusters>;

struct ClusterTutorial {
  HistogramRegistry mHistManager{"ClusterMonitorHistograms"};

  Preslice<o2::aod::EMCALClusterCells> perCluster = o2::aod::emcalclustercell::emcalclusterId;
  Preslice<o2::aod::EMCALMatchedTracks> perClusterMatchedTracks = o2::aod::emcalclustercell::emcalclusterId;
  // configurable parameters
  Configurable<bool> mDoEventSel{"doEventSel", 0, "demand kINT7"};
  Configurable<double> mVertexCut{"vertexCut", -1, "apply z-vertex cut with abs value in cm"};
  Configurable<int> mClusterDefinition{"clusterDefinition", 10, "cluster definition to be selected, e.g. 10=kV3Default"};

  // define cluster filter. It selects only those clusters which are of the type.
  Filter clusterDefinitionSelection = (o2::aod::emcalcluster::definition == mClusterDefinition);

  /// \brief Create output histograms and initialize geometry
  void init(InitContext const&)
  {
    const AxisSpec dRAxis{100, 0.0, 0.4, "d#it{R}"};

    int MaxMatched = 20; // maximum number of matched tracks, hardcoded in emcalCorrectionTask.cxx!
    const AxisSpec nmatchedtrack{MaxMatched, -0.5, MaxMatched + 0.5};

    mHistManager.add("clusterE", "Energy of cluster;'it{E} (GeV)", HistType::kTH1F, {{200, 0., 40.}});
    mHistManager.add("clusterAmpFractionLeadingCell", "Fraction of energy in leading cell", HistType::kTH1F, {{100, 0, 1}});
    mHistManager.add("mGammaGamma", "inv mass of two clusters;#it{m}_{inv} (GeV/#it{c}^{2})", HistType::kTH1F, {{300, 0., 0.3}});
    mHistManager.add("clusterTM_dRTN", "cluster trackmatching dR/TN;d#it{R};#it{N}_{matched tracks}", HistType::kTH2F, {dRAxis, nmatchedtrack});
  }

  /// \brief Process EMCAL clusters that are matched to a collisions
  void processCalo(collisionEvSelIt const& theCollision, selectedClusters const& clusters, o2::aod::EMCALClusterCells const& emccluscells, o2::aod::Calos const& allcalos, o2::aod::EMCALMatchedTracks const& matchedtracks, o2::aod::FullTracks const& fulltracks)
  {

    // do event selection if mDoEventSel is specified
    // currently the event selection is hard coded to kINT7
    // but other selections are possible that are defined in TriggerAliases.h
    bool isSelected = true;
    if (mDoEventSel) {
      if (theCollision.bc().runNumber() < 300000) {
        if (!theCollision.alias()[kINT7]) {
          isSelected = false;
        }
      } else {
        if (!theCollision.alias()[kTVXinEMC]) {
          isSelected = false;
        }
      }
    }
    if (!isSelected) {
      return;
    }
    if (mVertexCut > 0 && TMath::Abs(theCollision.posZ()) > mVertexCut) {
      return;
    }
    // loop over all clusters from accepted collision
    for (const auto& cluster : clusters) {
      mHistManager.fill(HIST("clusterE"), cluster.energy());

      // example of loop over all cells of current cluster
      // cell.calo() allows to access the cell properties as defined in
      // AnalysisDataModel In this exammple, we loop over all cells and find the
      // cell of maximum energy and plot the fraction it carries of the whole
      // cluster
      auto cellsofcluster = emccluscells.sliceBy(perCluster, cluster.globalIndex());
      double maxamp = 0;
      double ampfraction = 0;
      for (const auto& cell : cellsofcluster) {
        // example how to get any information of the cell associated with
        // cluster
        if (cell.calo().amplitude() > maxamp) {
          maxamp = cell.calo().amplitude();
        }
      }
      ampfraction = maxamp / cluster.energy();
      mHistManager.fill(HIST("clusterAmpFractionLeadingCell"), ampfraction);


      // Track Matching part
      double dEta, dPhi;
      auto tracksofcluster = matchedtracks.sliceBy(perClusterMatchedTracks, cluster.globalIndex());
      int t = 0;
      for (const auto& match : tracksofcluster) {
        dEta = match.track_as<o2::aod::FullTracks>().trackEtaEmcal() - cluster.eta();
        dPhi = match.track_as<o2::aod::FullTracks>().trackPhiEmcal() - cluster.phi();
        mHistManager.fill(HIST("clusterTM_dRTN"), std::sqrt(dPhi * dPhi + dEta * dEta), t);
        t++;
      }
    }

    // exmaple of combination loop to combine clusters pairwise
    for (auto& [g1, g2] : o2::soa::combinations(o2::soa::CombinationsStrictlyUpperIndexPolicy(clusters, clusters))) {
      // calculate pT for both clusters assuming they are photons (so no mass)
      float pt1 = sqrt(g1.energy() * g1.energy()) / cosh(g1.eta());
      float pt2 = sqrt(g2.energy() * g2.energy()) / cosh(g2.eta());
      // using ROOTs 4Vector class to add the two vour vectors and calculate the mass of the mother particle
      ROOT::Math::PtEtaPhiMVector v1(pt1, g1.eta(), g1.phi(), 0.);
      ROOT::Math::PtEtaPhiMVector v2(pt2, g2.eta(), g2.phi(), 0.);
      ROOT::Math::PtEtaPhiMVector v12 = v1 + v2;
      mHistManager.fill(HIST("mGammaGamma"), v12.M());
    }
  }
  PROCESS_SWITCH(ClusterTutorial, processCalo, "process only reconstructed info", true);

  void processDummy(collisionEvSelIt const& collision)
  {
    // do nothing
  }
  PROCESS_SWITCH(ClusterTutorial, processDummy, "Dummy function", false);
};

WorkflowSpec defineDataProcessing(ConfigContext const& cfgc)
{
  return WorkflowSpec{adaptAnalysisTask<ClusterTutorial>(cfgc)};
}
