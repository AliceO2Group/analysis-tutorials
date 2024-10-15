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
/// \brief This task is an empty skeleton that fills a simple eta histogram.
///        it is meant to be a blank page for further developments.
/// \author everyone

#include "Framework/runDataProcessing.h"
#include "Framework/AnalysisTask.h"
#include "Common/DataModel/TrackSelectionTables.h"
#include "Framework/ASoAHelpers.h"

using namespace o2;
using namespace o2::framework;
using namespace o2::framework::expressions;

struct myExampleTask {
  // Histogram registry: an object to hold your histograms
  HistogramRegistry histos{"histos", {}, OutputObjHandlingPolicy::AnalysisObject};

  Configurable<int> nBinsPt{"nBinsPt", 100, "N bins in pT histo"};

  Filter trackDCA = nabs(aod::track::dcaXY) < 0.2f;

  //This is an example of a convenient declaration of "using"
  using myCompleteTracks = soa::Join<aod::Tracks, aod::TracksExtra, aod::TracksDCA, aod::McTrackLabels>;
  using myFilteredTracks = soa::Filtered<myCompleteTracks>;

  Preslice<aod::Tracks> perCollision = aod::track::collisionId;

  void init(InitContext const&)
  {
    // define axes you want to use
    const AxisSpec axisCounter{1, 0, +1, ""};
    const AxisSpec axisEta{30, -1.5, +1.5, "#eta"};
    const AxisSpec axisPt{nBinsPt, 0, 10, "p_{T}"};
    const AxisSpec axisDeltaPt{100, -1.0, +1.0, "#Delta(p_{T})"};
    // create histograms
    histos.add("eventCounter", "eventCounter", kTH1F, {axisCounter});
    histos.add("etaHistogram", "etaHistogram", kTH1F, {axisEta});
    histos.add("ptHistogram", "ptHistogram", kTH1F, {axisPt});
    histos.add("ptResolution", "ptResolution", kTH2F, {axisPt, axisDeltaPt});

    histos.add("ptHistogramPion", "ptHistogramPion", kTH1F, {axisPt});
    histos.add("ptHistogramKaon", "ptHistogramKaon", kTH1F, {axisPt});
    histos.add("ptHistogramProton", "ptHistogramProton", kTH1F, {axisPt});
    histos.add("ptGeneratedPion", "ptGeneratedPion", kTH1F, {axisPt});
    histos.add("ptGeneratedKaon", "ptGeneratedKaon", kTH1F, {axisPt});
    histos.add("ptGeneratedProton", "ptGeneratedProton", kTH1F, {axisPt});

    histos.add("numberOfRecoCollisions", "numberOfRecoCollisions", kTH1F, {{10,-0.5f, 9.5f}});
    histos.add("multiplicityCorrelation", "multiplicityCorrelations", kTH2F, {{100, -0.5f, 99.5f}, {100,-0.5f, 99.5f}});
  }

  void processReco(aod::Collision const& collision, myFilteredTracks const& tracks, aod::McParticles const&)
  {
    histos.fill(HIST("eventCounter"), 0.5);
    for (const auto& track : tracks) {
      if( track.tpcNClsCrossedRows() < 70 ) continue; //badly tracked
      histos.fill(HIST("etaHistogram"), track.eta());
      histos.fill(HIST("ptHistogram"), track.pt());
      if(track.has_mcParticle()){ 
        auto mcParticle = track.mcParticle(); 
        histos.fill(HIST("ptResolution"), track.pt(), track.pt() - mcParticle.pt());
        if(mcParticle.isPhysicalPrimary() && fabs(mcParticle.y())<0.5){ // do this in the context of the track ! (context matters!!!)
          if(abs(mcParticle.pdgCode())==211) histos.fill(HIST("ptHistogramPion"), mcParticle.pt());
          if(abs(mcParticle.pdgCode())==321) histos.fill(HIST("ptHistogramKaon"), mcParticle.pt());
          if(abs(mcParticle.pdgCode())==2212) histos.fill(HIST("ptHistogramProton"), mcParticle.pt());
        }
      }
    }
  }
  PROCESS_SWITCH(myExampleTask, processReco, "process reconstructed information", true);

  void processSim(aod::McCollision const& mcCollision, soa::SmallGroups<soa::Join<aod::McCollisionLabels, aod::Collisions>> const& collisions, aod::McParticles const& mcParticles, myFilteredTracks const& tracks)
  {
    histos.fill(HIST("numberOfRecoCollisions"), collisions.size()); // number of times coll was reco-ed

    //Now loop over each time this collision has been reconstructed and aggregate tracks
    std::vector<int> numberOfTracks;
    for (auto& collision : collisions) {
      auto groupedTracks = tracks.sliceBy(perCollision, collision.globalIndex());
      // size of grouped tracks may help in understanding why event was split!
      numberOfTracks.emplace_back(groupedTracks.size());
    }
    if( collisions.size() == 2 ) histos.fill(HIST("multiplicityCorrelation"), numberOfTracks[0], numberOfTracks[1]);

    //Loop over particles in this mcCollision (first argument of process: iterator)
    for (const auto& mcParticle : mcParticles) {
      if(mcParticle.isPhysicalPrimary() && fabs(mcParticle.y())<0.5){ // do this in the context of the MC loop ! (context matters!!!)
        if(abs(mcParticle.pdgCode())==211) histos.fill(HIST("ptGeneratedPion"), mcParticle.pt());
        if(abs(mcParticle.pdgCode())==321) histos.fill(HIST("ptGeneratedKaon"), mcParticle.pt());
        if(abs(mcParticle.pdgCode())==2212) histos.fill(HIST("ptGeneratedProton"), mcParticle.pt());
      }
    }
  }
  PROCESS_SWITCH(myExampleTask, processSim, "process pure simulation information", true);
};

WorkflowSpec defineDataProcessing(ConfigContext const& cfgc)
{
  return WorkflowSpec{
    adaptAnalysisTask<myExampleTask>(cfgc)};
}

