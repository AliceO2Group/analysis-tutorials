# O2AT - Second edition - PWG-EM
This tutorial will cover the basis on how to get clusters from the EMCal, how to access the cells that make these clusters and how to access pre matched tracks.

## Clusters
In the AO2D.root files that we get from the reconstruction we only have the cell information of the calorimeter saved inside the `Calo` table.
For most users instead of raw cell information finished clusters are the desired observable.
To get EMCal clusters the EMCal group provides a task, `o2-analysis-je-emcal-correction-task` which takes in the cells from the `Calo` table and outputs multiple tables.
The most important tables are `EMCALCluster`, `EMCALClusterCell` and `EMCALMatchedTracks` (ref: [here](https://aliceo2group.github.io/analysis-framework/docs/datamodel/pwgTables.html#pwg-je)).
The clusters will be written in the `EMCALCluster` and can then be used in other tasks as an argument for a process function to subscribe to this table.

## Cells of cluster
For some analysis it might be useful to get the cell information of all cells inside a cluster.
This can be achieved with the `EMCALClusterCell` table, which only has two columns: one that points to the index of the `EMCALCluster` table and one to the index of the `Calo` table.
Through this one can use the `sliceBy` functionality to select the cells to any specific cluster.

## Prematched tracks
To simply the track matching the `o2-analysis-je-emcal-correction-task` will prematch tracks and clusters.
For each cluster there can be up to 20 (this number might change in the future depending) matched tracks.
A track is marked as matched when the distance between track and cluster dR < 0.4.
Similar to the `EMCALClusterCell` table the `EMCALMatchedTracks` table has two columns, one that points to the index of the `EMCALCluster` table and one that points to the index of the `Track` table.
Again with the `sliceBy` functionality one can select all the prematched tracks for any specific cluster.

## Tutorial Steps
### Step 0
The starting point. Here you can already see how to subscribe to the cluster table and how to ascess them.
We encourage participants to try and add a histogram to look at the M02 distribution of the clusters, since this is one of the most important variables to cut out "noise" or select between merged and single particle clusters.

### Step 1
In the first real step you will be asked to access the cell information of the clusters and for exmaple try to get the amplitudefraction of the largest contributing cell for each cluster.

### Step 2
In the second step you will be asked to combine clusters pairwise and caluclate the invariant mass and the transerve momentum of a potential mother particle under the assumptaion that each cluster stems from a single photon.

### Step 3
In the third (optional) step you will be asked to access the prematched tracks and look at the number of matched tracks as a function of dR.
If you like you could also try to use TPC PID to identiy e+/e- tracks and look at the cluster energy over track momentum.
