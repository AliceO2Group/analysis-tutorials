#!/bin/bash
#
# O2AT Simulation example
# This is provided as reference
# Simulations typically take a significant amount of time
#
# A example workflow MC->RECO->AOD for a simple pp min bias
# production, targetting test beam conditions.
#
# make sure O2DPG + O2 is loaded
# PLEASE ADJUST THE O2DPG PATH ACCORDINGLY
export O2DPG_ROOT=$HOME/alice/O2DPG
export CURRENTDIR=`pwd`

[ ! "${O2DPG_ROOT}" ] && echo "Error: This needs O2DPG loaded" && exit 1
[ ! "${O2_ROOT}" ] && echo "Error: This needs O2 loaded" && exit 1

# ----------- LOAD UTILITY FUNCTIONS --------------------------
. ${O2_ROOT}/share/scripts/jobutils.sh

# ----------- START ACTUAL JOB  -----------------------------

NWORKERS=${NWORKERS:-2}
MODULES="--skipModules ZDC"
SIMENGINE=${SIMENGINE:-TGeant3}
NSIGEVENTS=${NSIGEVENTS:-20}
NTIMEFRAMES=${NTIMEFRAMES:-5}
INTRATE=${INTRATE:-500000}
SYSTEM=${SYSTEM:-pp}
ENERGY=${ENERGY:-13600}
[[ ${SPLITID} != "" ]] && SEED="-seed ${SPLITID}" || SEED=""

echo "Create workflow"
echo "Current directory: ${CURRENTDIR}"
# create workflow.json
# this file list all jobs that need to be started and how they depend on one another
# it can be parsed and manipulated if needed, or alternatively, you can
# also change the generating recipe (it's a python script) to add extra
# components and their dependences
${O2DPG_ROOT}/MC/bin/o2dpg_sim_workflow.py -eCM ${ENERGY} -col ${SYSTEM} -gen pythia8pp -j ${NWORKERS} -ns ${NSIGEVENTS} -tf ${NTIMEFRAMES} -confKey "Diamond.width[2]=6." -e ${SIMENGINE} ${SEED} -mod "--skipModules ZDC"  -field -5

# run workflow
echo "Actually run"
# Note on CPU limit: this tells the workflow runner how many CPUs your computer has.
# Be cautious, as it may mean a lot of processes are spawned!
${O2DPG_ROOT}/MC/bin/o2_dpg_workflow_runner.py -f workflow.json -tt aod --cpu-limit 4
