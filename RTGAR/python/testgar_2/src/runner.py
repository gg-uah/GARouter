#!/usr/bin/env python
"""
@file    runner.py
@author  Carlos Tomas
@date    2015-04-02
@version $Id: runner.py 17235 2014-11-03 10:53:02Z $

This script is a test runner for the disaggregation scenario.

SUMO, Simulation of Urban MObility; see http://sumo.dlr.de/
Copyright (C) 2008-2014 DLR (http://www.dlr.de/) and contributors

This file is part of SUMO.
SUMO is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 3 of the License, or
(at your option) any later version.
"""
__author__ = 'ctomas'

import __builtin__
import shutil
import glob
import subprocess
import logging
import logging.config
import odtripsgen
import writedetfiles
import meas2flows
from constants import *

# Import modules from commonutils package
try:
    from commonutils import fileutils, timeutils, const
except ImportError:
    sys.exit("Failed to import from \'commonutils\' package")

# Import checkBinary from sumolib package
try:
    sys.path.append(os.path.join(os.environ.get("SUMO_HOME"), "tools"))
    from sumolib import checkBinary
    import traci
except ImportError:
    sys.exit("Please declare environment variable 'SUMO_HOME' as the root directory of your sumo installation (it should contain folders 'bin' and 'tools')")


def getAppLogger():
    '''
    Get a cross-module logger for the application
    '''
    logging.config.fileConfig(const.LOG_CFG_FILE)
    __builtin__.logger = logging.getLogger(const.APP_NAME)

    # Make logger a cross-module scope variable
    return __builtin__.logger


def doClean():
    '''
    Removes the output and log files
    '''
    for f in glob.glob(os.path.join(const.BASE_DIR, const.DATA_DIR, properties['FILE_OUTPUT_PREFIX']) + "*"):
        logger.info("Delete output file \'%s\'" % f)
        try:
            os.remove(f)
        except Exception:
            logger.error("Fail to delete output file \'%s\'" % f, exc_info=True)

    for f in glob.glob(os.path.join(const.BASE_DIR, const.LOG_DIR, const.APP_NAME) + "*.log"):
        logger.info("Delete log file \'%s\'" % f)
        try:
            os.remove(f)
        except Exception:
            logger.error("Fail to delete output file \'%s\'" % f, exc_info=True)


def generateNetwork(nodFile, edgFile, typFile, conFile, netFile):
    '''
    Run the netconvert application to generate the network file.
    :param nodFile: The node definition file
    :param edgFile: The edge definition file
    :param typFile: The edge type definition file
    :param conFile: The connections definition file
    :param netFile: The network file
    :return:
    '''
    logger.info("................ Generate the network (netconvert) ...")
    logger.info("\tNode file: %s" % nodFile)
    logger.info("\tEdge file: %s" % edgFile)
    logger.info("\tType file: %s" % typFile)
    logger.info("\tConn file: %s" % conFile)
    logger.info("\tNet file : %s" % netFile)

    # Call to netconvert
    netconvertBin = checkBinary('netconvert')
    options = ["--node-files", nodFile,
               "--edge-files", edgFile,
               "--output-file", netFile,
               "--no-turnarounds",
               "--verbose"]

    # Optional network type file
    if typFile:
        options += ["--type-files", typFile]

    #Optional connection file
    if conFile:
        options += ["--connection-files", conFile]

    status = subprocess.call([netconvertBin] + options,
                             stdout = sys.stdout,
                             stderr = sys.stderr)
    if status != 0:
        logger.critical("Fail: netconvert terminated with error status [%d]" % status, exc_info=True)
        sys.exit(status)
    else:
        logger.info("Success: netconvert finished with status [%d]" % status)

    sys.stdout.flush()


def generateRoutesFromOD(netFile, tazFile, nettazFile, rtypFile, odFile, tripFile, routFile, odGroups, begin, end, vehType, meanNum, stdDev):
    '''
    Generate the route file from traffic volume definition among districts (TAZ) as an origin/destination matrix.
    :param netFile:     The network file
    :param tazFile:     The district (TAZs) file
    :param nettazFile:  The newtork with districts file
    :param rtypFile:    The vehicle type using route file
    :param odFile:      The matrix O/D file
    :param tripFile:    The trip file
    :param routFile:    The route file
    :param odGroups:    The districts groups emitting traffic flow to each other
    :param begin:       The beginning time
    :param end:         The end time
    :param vehType:     The vehicle type
    :param meanNum:     The mean number of the normal distribution that depicts the traffic flow amounts among TAZs
    :param stdDev:      The standard deviation of the normal distribution that depicts the traffic flow amounts among TAZs
    :return:
    '''
    logger.info("................ Generate the O/D trips ...")
    logger.info("\tNet file   : %s" % netFile)
    logger.info("\tTaz file   : %s" % tazFile)
    logger.info("\tNettaz file: %s" % nettazFile)
    logger.info("\tRtyp file  : %s" % rtypFile)
    logger.info("\tO/D file   : %s" % odFile)
    logger.info("\tTrip file  : %s" % tripFile)
    logger.info("\tRoute file : %s" % routFile)
    logger.info("\tO/D groups : %s" % odGroups)
    logger.info("\tBegin      : %s" % begin)
    logger.info("\tEnd        : %s" % end)
    logger.info("\tVehType    : %s" % vehType)
    logger.info("\tMean Num   : %s" % meanNum)
    logger.info("\tStd Dev    : %s" % stdDev)

    arguments = ["--net-file", netFile,
                 "--taz-file", tazFile,
                 "--nettaz-file", nettazFile,
                 "--output-od-file", odFile,
                 "--output-trip-file", tripFile,
                 "--output-route-file", routFile,
                 "--od-groups", odGroups,
                 "--begin", begin,
                 "--end", end,
                 "--mean-number", str(meanNum),
                 "--std-dev", str(stdDev),
                 "--rebuild"
                 ]

    # Optional route vtype file and vehicle type
    if rtypFile:
        arguments += ["--route-vtype-file", rtypFile]
    if vehType:
        arguments += ["--vehicle-type", vehType]

    # Call the odtripsgen script
    status = odtripsgen.run(properties, arguments)

    if status != 0:
        logger.critical("Fail: odtripsgen terminated with error status [%d]" % status, exc_info=False)
        sys.exit(status)
    else:
        logger.info("Success: odtripsgen finished with status [%d]" % status)


def writeDetectionFiles(netFile, loopFile, detFile, measFile, measFreq):
    logger.info("............... Write the induction loop and detection files ...")
    logger.info("\tNet file     : %s" % netFile)
    logger.info("\tLoop file    : %s" % loopFile)
    logger.info("\tDet file     : %s" % detFile)
    logger.info("\tMeas file    : %s" % measFile)
    logger.info("\tMeasure freq : %s" % measFreq)

    # Call the writedetfiles script
    arguments = ["--net-file", netFile,
                 "--output-loop-file", loopFile,
                 "--output-detector-file", detFile,
                 "--measure-freq", str(measFreq),
                 "--measure-file", measFile]

    status = writedetfiles.run(properties, arguments)

    if status != 0:
        logger.critical("Fail: writedetfiles terminated with error status [%d]" % status, exc_info=False)
        sys.exit(status)
    else:
        logger.info("Success: writedetfiles finished with status [%d]" % status)


def traciSimulation(port):
    '''
    Execute the TraCI control loop on the SUMO simulation.
    :param port:    The port used for communicating with your sumo instance
    :return:        The end second time of the simulation
    '''
    try:
        # Init the TraCI server
        traci.init(port)

        # TraCI runs the simulation step by step
        step = 0
        while traci.simulation.getMinExpectedNumber() > 0:
            traci.simulationStep()
            step += 1
    except traci.TraCIException as ex:
        logger.fatal("Exception in simulation step %d: %s" % (step, ex.message))
        return -1
    except traci.FatalTraCIError as ex:
        logger.fatal("Fatal error in simulation step %d: %s" % (step, ex.message))
        return -1
    else:
        return step + 1
    finally:
        # Close the TraCI server
        traci.close()


def runSumo(netFile, routFile, loopFile, beginSecs):
    logger.info("............... Run the SUMO simulation ...")
    logger.info("\tNet file  : %s" % netFile)
    logger.info("\tTrip file : %s" % routFile)
    logger.info("\tLoop file : %s" % loopFile)
    logger.info("\tBegin secs: %s" % beginSecs)
    logger.info("\tAlgorithm : %s" % properties['ROUTING_ALGORITHM'])

    # Start SUMO as a subprocess and then connect the traci and run
    sumoBin = checkBinary('sumo')
    options = ["--net-file", netFile,
               "--route-files", routFile,
               "--additional-files", loopFile,
               "--begin", str(beginSecs),
               "--routing-algorithm", properties['ROUTING_ALGORITHM'],
               "--remote-port", str(properties['REMOTE_PORT']),
               "--verbose"]

    sumoProcess = subprocess.Popen([sumoBin] + options,
                                   stdout=sys.stdout,
                                   stderr=sys.stderr)
    runSecs = traciSimulation(properties['REMOTE_PORT'])
    sumoProcess.wait()

    if runSecs < 0:
        logger.critical("Fail: Abnormal termination of the SUMO simulation")
        sys.exit(1)
    else:
        logger.info("Success: SUMO simulation finished with no errors")

    endSecs = beginSecs + runSecs
    logger.info("End seconds: [%d]" % endSecs)

    sys.stdout.flush()
    sys.stderr.flush()

    return int(endSecs)


def collectMeasIntoFlows(netFile, measFile, flowFile, beginSecs):
    logger.info("............... Collect the induction loop measurements ...")
    logger.info("\tNet file  : %s" % netFile)
    logger.info("\tMeas file : %s" % measFile)
    logger.info("\tFlow file : %s" % flowFile)
    logger.info("\tBegin secs: %d" % beginSecs)

    # Call the meas2flows script
    arguments = ["--net-file", netFile,
                 "--measure-files", measFile,
                 "--output-flow-file", flowFile,
                 "--begin", str(beginSecs)]

    status = meas2flows.run(properties, arguments)

    if status != 0:
        logger.critical("Fail: meas2flows terminated with error status [%d]" % status, exc_info=False)
        sys.exit(status)
    else:
        logger.info("Success: meas2flows finished with status [%d]" % status)


def computeRoutesFromFlows(netFile, detFile, flowFile, tazFile, grpFile, dfrouFile, emittFile, poiFile, vssFile,
                           beginSecs, endSecs, departPos, keepLongerRoutes, incUnusedRoutes, xmlValidation, routesForAll):
    shortRate = 0.1
    removeEmpty = True

    logger.info("............... Run the garouter ...")
    logger.info("Net file           : %s" % netFile)
    logger.info("Detection file     : %s" % detFile)
    logger.info("Flow measure file  : %s" % flowFile)
    logger.info("Taz file           : %s" % tazFile)
    logger.info("O/D group file     : %s" % grpFile)
    logger.info("DFRoute file       : %s" % dfrouFile)
    logger.info("Emitter file       : %s" % emittFile)
    logger.info("POI file           : %s" % poiFile)
    #logger.info("VSS file           : %s" % vssFile)
    logger.info("Begin secs         : %d" % beginSecs)
    logger.info("End secs           : %d" % endSecs)
    logger.info("Depart pos         : %s" % departPos)
    #logger.info("Keep longer routes : %s" % keepLongerRoutes)
    #logger.info("Inc. unused routes : %s" % incUnusedRoutes)
    logger.info("XML validation     : %s" % xmlValidation)
    logger.info("Shortest routes rate: %f" % shortRate)
    logger.info("Remove empty dets  : %s" % str(removeEmpty))
    #logger.info("Routes for all     : %s" % routesForAll)


    # Run the garouter
    garouterBin = checkBinary('garouter')
    options = ["--net-file", netFile,
               "--taz-file", tazFile,
               "--od-groups-file", grpFile,
               "--detector-files", detFile,
               "--measure-files", flowFile,
               "--routes-output", dfrouFile,
               "--emitters-output", emittFile,
              #"--routes-for-all", routesForAll,
               "--detectors-poi-output", poiFile,
               #"--variable-speed-sign-output", vssFile,
               "--departpos", departPos,
               "--shortest-routes-rate", str(shortRate),
               "--remove-empty-detectors", str(removeEmpty),
                #"--keep-longer-routes", keepLongerRoutes,
               #"--include-unused-routes", incUnusedRoutes,
               "--begin", str(beginSecs),
               "--end", str(endSecs),
               #"--xml-validation", xmlValidation,
               "--verbose"]
    #options = ["-c", os.path.join(const.BASE_DIR, const.CONFIG_DIR, "garouter.garcfg")]
    status = subprocess.call([garouterBin] + options,
                             stdout = sys.stdout,
                             stderr = sys.stderr)

    if status != 0:
        logger.critical("Fail: garouter terminated with error status [%s]" % status)
        sys.exit(1)
    else:
        logger.info("Success: garouter finished with status [%s]" % status)

    sys.stdout.flush()



def resolveFileNames(properties):
    '''
    Resolve the complete path file names to be used in the script.
    - Resolve the network description file names. The input
    - Resolve the network file name
    - Resolve the output file names, by adding an output suffix
    :param properties:  The properties from the properties configuration file
    :return:            A list comprising the appropriated path file names
    '''
    files = dict()

    # Resolve the network description file names
    files['nodFile']  = os.path.join(const.BASE_DIR, const.DATA_DIR, properties['NET_NODE_FILE'])
    files['edgFile']  = os.path.join(const.BASE_DIR, const.DATA_DIR, properties['NET_EDGE_FILE'])
    files['conFile']  = os.path.join(const.BASE_DIR, const.DATA_DIR, properties['NET_CONN_FILE'])
    files['typFile']  = os.path.join(const.BASE_DIR, const.DATA_DIR, properties['NET_TYPE_FILE'])
    files['tazFile']  = os.path.join(const.BASE_DIR, const.DATA_DIR, properties['TAZ_FILE'])
    files['grpFile']  = os.path.join(const.BASE_DIR, const.DATA_DIR, properties['OD_GROUPS'])
    files['rtypFile'] = os.path.join(const.BASE_DIR, const.DATA_DIR, properties['ROU_VTYP_FILE'])

    # Resolve the network file name (which can be provided or not)
    try:
        files['netFile'] = fileutils.find(properties['NET_FILE'], os.path.join(const.BASE_DIR, const.DATA_DIR))[0]
    except IndexError as ex:
        # Network file not found: resolve the output network file name to be generated
        files['netFile']  = fileutils.resolveOutputFile(properties['NET_FILE'],
                                                        properties['FILE_OUTPUT_PREFIX'],
                                                        os.path.join(const.BASE_DIR, const.DATA_DIR),
                                                        const.DEFAULT_NET_FILE)
    else:
        # Network file is found: keep the name
        files['netFile'] = os.path.join(const.BASE_DIR, const.DATA_DIR, properties['NET_FILE'])

    # Resolve the output file names
    files['odFile']   = fileutils.resolveOutputFile(properties['OD_FILE'],
                                                    properties['FILE_OUTPUT_PREFIX'],
                                                    os.path.join(const.BASE_DIR, const.DATA_DIR),
                                                    const.DEFAULT_OD_FILE)
    files['tripFile'] = fileutils.resolveOutputFile(properties['TRIP_FILE'],
                                                    properties['FILE_OUTPUT_PREFIX'],
                                                    os.path.join(const.BASE_DIR, const.DATA_DIR),
                                                    const.DEFAULT_TRIP_FILE)
    files['routFile'] = fileutils.resolveOutputFile(properties['ROUTE_FILE'],
                                                    properties['FILE_OUTPUT_PREFIX'],
                                                    os.path.join(const.BASE_DIR, const.DATA_DIR),
                                                    const.DEFAULT_ROUTE_FILE)
    files['loopFile'] = fileutils.resolveOutputFile(properties['LOOP_FILE'],
                                                    properties['FILE_OUTPUT_PREFIX'],
                                                    os.path.join(const.BASE_DIR, const.DATA_DIR),
                                                    const.DEFAULT_LOOP_FILE)
    files['detFile']  = fileutils.resolveOutputFile(properties['DET_FILE'],
                                                    properties['FILE_OUTPUT_PREFIX'],
                                                    os.path.join(const.BASE_DIR, const.DATA_DIR),
                                                    const.DEFAULT_DET_FILE)
    files['measFile'] = fileutils.resolveOutputFile(properties['MEAS_FILE'],
                                                    properties['FILE_OUTPUT_PREFIX'],
                                                    os.path.join(const.BASE_DIR, const.DATA_DIR),
                                                    const.DEFAULT_MEAS_FILE)
    files['flowFile'] = fileutils.resolveOutputFile(properties['FLOW_FILE'],
                                                    properties['FILE_OUTPUT_PREFIX'],
                                                    os.path.join(const.BASE_DIR, const.DATA_DIR),
                                                    const.DEFAULT_FLOW_FILE)
    files['dfrouFile'] = fileutils.resolveOutputFile(properties['DFROUTE_FILE'],
                                                     properties['FILE_OUTPUT_PREFIX'],
                                                     os.path.join(const.BASE_DIR, const.DATA_DIR),
                                                     const.DEFAULT_DFROUTE_FILE)
    files['emittFile'] = fileutils.resolveOutputFile(properties['EMITT_FILE'],
                                                     properties['FILE_OUTPUT_PREFIX'],
                                                     os.path.join(const.BASE_DIR, const.DATA_DIR),
                                                     const.DEFAULT_EMITT_FILE)
    files['poiFile']   = fileutils.resolveOutputFile(properties['POI_FILE'],
                                                     properties['FILE_OUTPUT_PREFIX'],
                                                     os.path.join(const.BASE_DIR, const.DATA_DIR),
                                                     const.DEFAULT_POI_FILE)
    files['vssFile']   = fileutils.resolveOutputFile(properties['VSS_FILE'],
                                                     properties['FILE_OUTPUT_PREFIX'],
                                                     os.path.join(const.BASE_DIR, const.DATA_DIR),
                                                     const.DEFAULT_VSS_FILE)
    netSplit = os.path.splitext(properties['NET_FILE'])
    ntazFilename = netSplit[0] + 'taz' + netSplit[1]
    files['nettazFile'] = fileutils.resolveOutputFile(ntazFilename,
                                                      properties['FILE_OUTPUT_PREFIX'],
                                                      os.path.join(const.BASE_DIR, const.DATA_DIR),
                                                      const.DEFAULT_NETTAZ_FILE)
    return files


def copyDataFilesToRTGAR():
    '''
    Copy all the files in the data directory to the GAROUTER data directory.
    :return:
    '''
    fromDataDir = os.path.join(const.BASE_DIR, const.DATA_DIR)
    toDataDir = os.path.join(properties['RTGAR'], const.DATA_DIR)

    # List the files in the 'data' directory
    dataFiles = os.listdir(fromDataDir)

    # Copy every data file to the 'RTGAR' directory
    for fileName in dataFiles:
        fullFileName = os.path.join(fromDataDir, fileName)
        if (os.path.isfile(fullFileName)):
            logger.info("Copy file '%s' to '%s' directory" % (fullFileName,toDataDir))
            shutil.copy(fullFileName, toDataDir)


def run(properties, logger):
    '''
    Run the script that launches the disaggregation process.
    1. Generate the network file if not provided
    2. Generate de OD matrix and the OD trips/routes
    3. Build the induction loop and detection files in every lane of the network
    4. Run the sumo simulation with the induction loops
    5. Compute the traffic flows by collecting the measurements from every induction loop
    6. Calculate the routes from the aggregated flow data
    :param properties:  The configuration properties for configuring the script running
    :param logger:      The application logger
    '''
    # Change to the base directory
    os.chdir(const.BASE_DIR)

    # ........................................ Resolve the path file names ...
    files = resolveFileNames(properties)

    # ........................................ Generate the network file ...
    # If the network file is found in the data directory, skip network generation
    if not os.path.isfile(files['netFile']):
        generateNetwork(files['nodFile'],
                        files['edgFile'],
                        files['typFile'],
                        files['conFile'],
                        files['netFile'])

    # Check the network file generation
    if not os.path.isfile(files['netFile']):
        logger.critical("Missing output network file \'%s\'" % files['netFile'])
        sys.exit(1)

    # ........................................ Generate the traffic demand among districts ...
    generateRoutesFromOD(files['netFile'],
                         files['tazFile'],
                         files['nettazFile'],
                         files['rtypFile'],
                         files['odFile'],
                         files['tripFile'],
                         files['routFile'],
                         properties['OD_GROUPS'],
                         properties['BEGIN'],
                         properties['END'],
                         properties['VEH_TYPE'],
                         properties['MEAN_NUM'],
                         properties['STD_DEV'])

    # ........................................ Build the induction loop and detector files ...
    writeDetectionFiles(files['netFile'],
                        files['loopFile'],
                        files['detFile'],
                        files['measFile'],
                        properties['MEAS_FREQ'])

    # ........................................ Run the SUMO simulation ...
    beginSecs = timeutils.getTimeSeconds(properties['BEGIN'])

    endSecs = runSumo(files['netFile'],
                      files['routFile'],
                      files['loopFile'],
                      beginSecs)

    '''
    endSecs = runSumo(files['netFile'],
                      '/Users/ctomas/RTGAR/python/testgar/data/myroutes.rou.xml',
                      files['loopFile'],
                      beginSecs)
    '''


    # ........................................ Collect the induction loop measurements into flow amounts...
    collectMeasIntoFlows(files['netFile'],
                         files['measFile'],
                         files['flowFile'],
                         beginSecs)

    # ........................................ Copy the data files to the RTGAR data directory ...
    copyDataFilesToRTGAR()

    sys.exit(0)

    # ........................................ Compute the vehicle routes from the detection flows ...
    keepLongerRoutes = "false"
    incUnusedRoutes = "false"
    xmlValidation = "never"
    routesForAll = "false"
    departPos = "10"

    computeRoutesFromFlows(files['netFile'],
                           files['detFile'],
                           files['flowFile'],
                           files['tazFile'],
                           files['grpFile'],
                           files['dfrouFile'],
                           files['emittFile'],
                           files['poiFile'],
                           files['vssFile'],
                           beginSecs, endSecs, departPos, keepLongerRoutes, incUnusedRoutes, xmlValidation, routesForAll)


# ..................................................
# ..................................................
# .................................................. This is the main entry point of this script ...
if __name__ == "__main__":
    # Load the properties file
    properties = {}
    execfile(const.PROPERTIES_FILE, properties)

    # Get a logger for the entire application
    logger = getAppLogger()

    # Clean output and log files
    if '--clean' in sys.argv:
        doClean()
        sys.exit(0)

    # Execute the runner script
    run(properties, logger)
