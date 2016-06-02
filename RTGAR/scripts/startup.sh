#!/bin/bash

export RTGAR=~/RTGAR
export DYLD_LIBRARY_PATH=${RTGAR}/lib:/opt/boost_1_57_0/stage/lib
export SUMO_HOME=/Users/ctomas/SUMO-0.22.0
export PATH=${PATH}:${SUMO_HOME}/bin

echo "RTGAR = "${RTGAR}
echo "DYLD_LIBRARY_PATH = "${DYLD_LIBRARY_PATH}
echo "SUMO_HOME = "${SUMO_HOME}
echo "PATH = "${PATH}

echo
echo "Start up the garouter..."
cd ${RTGAR}/python/testgar/config
${RTGAR}/bin/garouter -c ${RTGAR}/python/testgar/config/garouter.garcfg
#${RTGAR}/bin/garouter -c ${RTGAR}/python/testgar/config/garouter.garcfg

exit 0
