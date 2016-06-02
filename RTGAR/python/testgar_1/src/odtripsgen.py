#!/usr/bin/python
"""
@file    odtripsgen.py
@author  Carlos Tomas
@date    2015-03-26
@version $Id: runner.py 17235 2014-11-03 10:53:02Z $

This script generates traffic demand in a network between specified groups of TAZs
by building an OD matrix and generate trips between TAZs of different groups.

SUMO, Simulation of Urban MObility; see http://sumo.dlr.de/
Copyright (C) 2008-2014 DLR (http://www.dlr.de/) and contributors

This file is part of SUMO.
SUMO is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 3 of the License, or
(at your option) any later version.
"""
import subprocess
import numpy
import math
import re
import optparse
import StringIO
import logging
import logging.config
import __builtin__
from constants import *

# Import modules from commonutils package
try:
    from commonutils import timeutils, const, logutils, fileutils
except ImportError:
    sys.exit("Failed to import from \'commonutils\' package")

# Import etree from lxml package
try:
    from lxml import etree
except ImportError:
    sys.exit("Failed to import ElementTree from lxml")

# Import sumolib module
try:
    sys.path.append(os.path.join(os.environ.get("SUMO_HOME"), "tools"))
    from sumolib import checkBinary
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


# .................................................. Local functions ...
def _getCommandOptions(arguments, properties, logger):
    """
    Get the command line arguments and their values (if they have one assigned)
    :param arguments:   The argument list passed to the script
    :param properties:  The project properties loaded from the configuration file
    :param logger:      The module logger
    :return:            A dictionary mapping keys as the argument names
                        and values as the corresponding argument values
    """
    # Set the command line options
    parser = optparse.OptionParser()

    parser.add_option("-n", "--net-file", dest="netfile",
                      help="define the network file (mandatory)")

    parser.add_option("-t", "--taz-file", dest="tazfile",
                      help = "define the Traffic Area Zone (TAZ) filename (mandatory)")

    parser.add_option("-z", "--nettaz-file", dest="nettazfile", default=const.DEFAULT_NETTAZ_FILE,
                      help = "define the network with districts (TAZ) filename (\'%s\' by default)" % const.DEFAULT_NETTAZ_FILE)

    parser.add_option("-y", "--route-vtype-file", dest="rtypfile",
                      help="define the route vtype filename (Optional)")

    parser.add_option("-d", "--output-od-file", dest="odfile", default=const.DEFAULT_OD_FILE,
                      help="define the output trip filename (\'%s\' by default)" % const.DEFAULT_OD_FILE)

    parser.add_option("-o", "--output-trip-file", dest="tripfile", default=const.DEFAULT_TRIP_FILE,
                      help="define the output trip filename (\'%s\' by default)" % const.DEFAULT_TRIP_FILE)

    parser.add_option("-r", "--output-route-file", dest="routefile", default=const.DEFAULT_ROUTE_FILE,
                      help="generates route file with duarouter, using the shortest path computation (\'%s\' by default)" % const.DEFAULT_ROUTE_FILE)

    parser.add_option("-b", "--begin-time", dest="begin", default=const.DEFAULT_BEGIN,
                      help="the beginning time of the traffic demand; format <HH.MM> (\'%s\' by default)" % const.DEFAULT_BEGIN)

    parser.add_option("-e", "--end-time", dest="end", default=const.DEFAULT_END,
                      help="the end time of the traffic demand; format <HH.MM> (\'%s\'by default)" % const.DEFAULT_END)

    parser.add_option("-g", "--od-groups", dest="odgroups", default="",
                      help="define the origin/destination district groups among which the traffic demand is routed. "
                           "This value can be either a FILE containing the group definition or an EXPRESSION in which"
                           "each group is enclosed between parentheses and the TAZs inside a group are separated by a comma; "
                           "e.g. \'(TAZ1,TAZ2,TAZ3,TAZ4),(TAZ5,TAZ6,TAZ7,TAZ8)\' (mandatory)")

    parser.add_option("-m", "--mean-number", type=int, dest="meannum", default=const.DEFAULT_MEAN_NUM,
                      help="the mean value of the traffic demand amounts outgoing from every TAZ (\'%s\' by default) "
                           "in the O/D matrix" % const.DEFAULT_MEAN_NUM)

    parser.add_option("-s", "--std-dev", type=float, dest="stddev", default=const.DEFAULT_STD_DEV,
                      help="the standard deviation of the normal distribution that depicts "
                           "the traffic demand amounts outgoing from every TAZ (\'%s\' by default) "
                           "in the O/D matrix" % const.DEFAULT_MEAN_NUM)

    parser.add_option("-v", "--vehicle-type", dest="vtype", default="",
                      help="the vehicle type of the generated traffic demand; this parameter makes reference"
                           "to one of the vTypes defined in the route additional file (optional)")

    parser.add_option("--rebuild", dest="rebuild", action="store_true", default=False,
                      help="forces to build the network with taz and the OD-matrix files (optional)")

    (options, args) = parser.parse_args(args=arguments)

    if not options.netfile  or  not options.tazfile:
        logger.critical("Missing mandatory arguments")
        sys.exit(1)

    return options


def _getTazsFromFile(tazFile, logger):
    """
    Read the taz file and return a list of the TAZs defined in the taz file

    :param tazFile: The taz file name
    :param logger:  The module logger
    :return:        A list containing the name of the TAZs defined in the taz file
    """
    if not os.path.isfile(tazFile):
        logger.critical("Missing TAZ file \'%s\'" % tazFile)
        sys.exit(1)

    # Parse the TAZ file
    tree = etree.parse(tazFile)

    # The root points to the <tazs> tag
    root = tree.getroot()

    # Add the taz id to the TAZ list
    tazs = [child.attrib['id'] for child in root]

    return tazs


def _getTazGroupsFromFile(grpFile, logger):
    '''
    Get the O/D TAZ groups from an XML file. These groups which will be used to generate
    the traffic demand in the simulation.
    :param grpFile:     The XML file depicting the OD TAZ groups.
    :param logger:      The module logger
    :return:            A list of tuples; each tuple depicts a collection of TAZ id's,
                        e.g. [(TAZ1,TAZ2,TAZ3,TAZ4),(TAZ5,TAZ6,TAZ7,TAZ8)]
    '''
    odGroups = list()

    if not os.path.isfile(grpFile):
        logger.critical("Missing O/D group file \'%s\'" % grpFile)
        sys.exit(1)

    try:
        # Parse the OD group file
        tree = etree.parse(grpFile)

        # The root points to the <odgroups> tag
        root = tree.getroot()

        # Add the taz id to the TAZ list
        groups = set([ child.attrib['id'] for child in root ])

        for grp in groups:
            tazs = root.xpath("./odgroup[@id='{0}']/taz/@id".format(grp))
            odGroups.append(tuple(tazs))

        return odGroups
    except Exception as ex:
        logger.error("Fail to get OD groups from file '%s'" % grpFile)
        return list()


def _getTazGroupsFromExpression(optGroups, logger):
    '''
    Get the O/D district (TAZ) groups from an expression.
    :param optGroups:   The O/D TAZ group expression; the groups are delimited by parentheses,
                        and inside a group the districts are separated by a comma character, e.g. "(TAZ1,TAZ2),(TAZ3,TAZ4)"
    :param logger:      The module logger
    :return:            A list of tuples; each tuple depicts a collection of TAZ id's,
                        e.g. [(TAZ1,TAZ2),(TAZ3,TAZ4)]
    '''
    tazGroups = list()

    try:
        # Validate the TAZ groups argument
        result = re.match(const.TAZ_GROUPS_PATTERN, optGroups)

        if result == None  or  result.group() != optGroups:
            logger.critical('Bad expression for the TAZ groups: %s' % optGroups)
            sys.exit(1)

        # Split on the group comma
        groups = re.split(const.SPLIT_TAZ_GROUPS_PATTERN, optGroups)

        for grp in groups:
            # Remove the enclosing parentheses
            grp = grp.translate(None,'()')

            # Split the group on the comma character and remove empty elements
            tazs = filter(None, grp.split(','))

            # Add the taz group as a tuple element
            tazGroups.append(tuple(tazs))
        return tazGroups
    except Exception as ex:
        logger.error('Bad expression for the TAZ groups: %s' % optGroups)
        return list()


def _getTazGroups(optGroups, logger):
    '''
    Get the OD district groups which will be used to generate traffic demand in the simulation.d
    :param optGroups:   A file or a string containing the TAZs grouped between parentheses;
    :param properties:  The project properties loaded from the configuration file
    :param logger:      The module logger
    :return:            A tuple containing the TAZ groups; each element in the group tuple is a tuple
                        containing the TAZ id's, e.g. [(TAZ1,TAZ2),(TAZ3,TAZ4)]
    '''
    tazGroups = list()

    # Check if the group option concerns to either a file or an expression
    grpFile = os.path.join(const.BASE_DIR, const.DATA_DIR, optGroups)
    if os.path.isfile(grpFile):
        logger.info("Get the groups from file '%s'" % grpFile)
        tazGroups = _getTazGroupsFromFile(grpFile, logger)
    else:
        logger.info("Get the OD groups from expression '%s'" % optGroups)
        tazGroups = _getTazGroupsFromExpression(optGroups, logger)

    if not tazGroups:
        logger.error("Fail to get OD district groups")
        exit(1)

    return tuple(tazGroups)


def _addNetLines(netFile, fnettaz, netEndTag, logger):
    """
    Add the network data lines to the network with districts data file
    :param netFile:     The network file name
    :param fnettaz:     The network (districts) data file
    :param netEndTag:   The end tag of the network file
    :param logger:      The module logger
    """
   # Read data from the network file
    try:
        with open(netFile) as fnet:
            for line in fnet:
                # Write data into the network with taz data file
                if line.strip() != netEndTag:
                    fnettaz.write(line)
                else:
                    break
    except IOError as ex:
        logger.error("Fail to write net data from net file \'%s\' into the network (districts) file: " % (netFile, ex.strerror))


def _addTazLines(tazFile, fnettaz, logger):
    """
    Add the taz data lines to the network with districts data file
    :param tazFile:     The districts file name
    :param fnettaz:     The network (districts) data file
    :param logger:      The module network
    """
    # Read data from the districts file
    try:
        with open(tazFile) as ftaz:
            for line in ftaz:
                fnettaz.write(line)
    except IOError as ex:
        logger("Fail to add TAZ lines from districs file {0} to network file {1}: {2}".format(tazFile, fnettaz, ex.strerror))
        sys.exit(1)


def _buildNetTazFile(netFile, tazFile, nettazFile, logger):
    '''
    Create a net file embedding the districts data into the network data
    :param netFile:     The network file
    :param tazFile:     The districts file
    :param nettazFile:  The network (districts) data file
    :param properties:  The project properties loaded from the configuration file
    :param logger:      The module logger
    :return:
    '''
    netEndTag = const.NET_END_TAG
    try:
        # Create the network with districts data file for writing
        with open(nettazFile, 'w') as f:
            # Add network lines to the network (districts) file
            _addNetLines(netFile, f, netEndTag, logger)

            # Add taz lines to the network (districts) file
            _addTazLines(tazFile, f, logger)

            # Add the end of network tag
            f.write(netEndTag)
    except IOError as ex:
        logger.critical('Fail when building the network (districts) data file \'%s\': %s' % (nettazFile, ex.strerror))
        sys.exit(1)
    except Exception as ex:
        logger.critical('Fail when building the network (districts) data file \'%s\': %s' % (nettazFile, ex.message))
        sys.exit(1)


def _tazHasSources(tazId, tazfile):
    '''
    Checks if a district has sources defined
    :param tazId:       The taz identifier to check
    :param tazfile:     The taz files containing the taz definitions
    :return:            'True' if the taz has sources, 'False' otherwise
    '''
    root = etree.parse(tazfile).getroot()

    if root.findall("taz[@id='{0}']/tazSource".format(tazId)):
        return True
    else:
        return False


def _tazHasSinks(tazId, tazfile):
    '''
    Checks if a district has sinks defined
    :param tazId:       The taz identifier to check
    :param tazfile:     The taz files containing the taz definitions
    :return:            'True' if the taz has sinks, 'False' otherwise
    '''
    root = etree.parse(tazfile).getroot()

    if root.findall("taz[@id='{0}']/tazSink".format(tazId)):
        return True
    else:
        return False


def _writeODFile(odFile, begin, end, tazs, tazFile, odGroups, meanNum, stDev, vType, logger):
    '''
    Create the O/D matrix file using the data from the command-line arguments
    :param begin:       The beginning time of the traffic demand <HH.MM>
    :param end:         The end time of the traffic demand <HH.MM>
    :param tazs:        A comma-separated string reporting the TAZs as specified in the TAZ file
    :param tazFile:     The disctrict (TAZ) fiel
    :param odGroups:    A tuple containing the TAZ groups, each element of the tuple is a TAZ group, which in turn,
                        maps to a tuple containing the TAZ disctricts belonging to the group
    :param meanNum:     The mean number of vehicle amount that leave each TAZ district
    :param stDev:       The standard deviation of a normal distribuition modelling the outgoing traffic flow
    :param vType:       The vehicle type performing the trip
    :param logger:      The module logger
    :return:
    '''
    try:
        with open(odFile, 'w') as f:
            f.write('$VMR\n')
            f.write('* Vehicle type\n')
            f.write('{0}\n'.format(vType))
            f.write('{0:12} {1:}'.format('* From-Time', 'To-Time\n'))
            f.write('{0:12} {1:}\n'.format(begin, end))
            f.write('*\n')
            f.write('* Factor\n')
            f.write('1.00\n')
            f.write('*\n')
            f.write('* Number of districts\n')
            f.write('%d\n' % len(tazs))
            f.write('*\n')
            f.write('* District names\n')
            for taz in tazs:
                f.write('{:>12}'.format(taz))
            for oTaz in tazs:
                f.write('\n* Amount of vehicles that leave the district %s\n' % oTaz)
                for dTaz in tazs:
                    # The origin TAZ and destination TAZ's can't be in the same group.
                    # Origin and destination TAZs must have sources and sinks respectively
                    if (not odGroups  or  [ group for group in odGroups if oTaz not in group and dTaz in group ])  \
                            and (_tazHasSources(oTaz, tazFile)  and  _tazHasSinks(dTaz, tazFile)):
                        if (stDev > 0):
                            odFlow = int(math.fabs(numpy.random.normal(meanNum, stDev, 1)))
                        else:
                            odFlow = meanNum
                    else:
                        odFlow = 0

                    f.write('{:>12}'.format(odFlow))
    except IOError as ex:
        logger.critical('Fail to create the O/D matrix file \'%s\': %s' % (odFile, ex.strerror))


def _callOD2trips(netFile, odFile, tripFile, begin, end, logger):
    '''
    Imports O/D-matrices and splits them into single vehicle trips.
    :param netFile:     The network file
    :param odFile:      The OD-matrix file
    :param tripFile:    The trip file
    :param begin:       The beginning time <HH.MM>. Previous trips will be discarded
    :param end:         The end time <HH.MM>. Later trips will be discarded
    :param logger:      The module logger
    :return:            The status code returned from the od2trips call
    '''
    # Get the traffic demand beginning time
    beginSecs = timeutils.getTimeSeconds(begin)

    # Get the traffic demand end time
    endSecs = timeutils.getTimeSeconds(end)

    od2tripsBin = checkBinary('od2trips')

    options = ["--net-file", netFile,
              "--od-matrix-files", odFile,
              "--output-file", tripFile,
              "--begin", str(beginSecs),
              "--end", str(endSecs),
              "--spread.uniform",
              "--random",
              "--verbose"]

    status = subprocess.call([od2tripsBin] + options,
                             stdout = sys.stdout,
                             stderr = sys.stderr)

    if status != 0:
        logger.critical("Fail: od2trips terminated with error status [%d]" % status, exc_info = False)
        sys.exit(status)
    else:
        logger.info("Success: od2trips finished with status [%d]" % status)

    sys.stdout.flush()

    return status


def _callDuaRouter(netFile, vtypeFile, tripFile, rouFile, rouAlgorithm, logger):
    '''
    Imports trips definitions and computes vehicle routes using shortest path computation.
    :param netFile:      The network file
    :param vtypeFile:    The duarouter additional file containing the vehicle types
    :param tripFile:     The trip file
    :param rouFile:      The route file
    :param rouAlgorithm: The dynamic user assignment route algorithm
    :param logger:       The module logger
    :return:             The status code returned from the duarouter call
    '''
    duarouterBin = checkBinary('duarouter')

    options = ["--net-file", netFile,
              "--trip-files", tripFile,
              "--output-file", rouFile,
              "--routing-algorithm", rouAlgorithm,
              "--ignore-errors",
              "--verbose"]
    # Optional route vtype file
    if vtypeFile:
        options += ["--additional-files", vtypeFile]

    status = subprocess.call([duarouterBin] + options,
                             stdout = sys.stdout,
                             stderr = sys.stderr)

    if status != 0:
        logger.critical("Fail: duarouter terminated with error status [%d]" % status, exc_info = False)
        sys.exit(status)
    else:
        logger.info("Success: duarouter finished with status [%d]" % status)

    sys.stdout.flush()

    return status


def run(properties, arguments = None):
    """
    Actions to be performed when this module is executed as a script.
    :param properties:  The project properties loaded from the configuration file
    :param arguments:   Argument list passed in the script call
    """
    const = __import__('constants').const

    # Get the application logger
    logger = __builtin__.logger

    # Get the command line arguments
    options = _getCommandOptions(arguments, properties, logger)

    # Log the options
    with logutils.redirectStdout(StringIO.StringIO()) as newStdout:
        sys.stdout.write(options)
    newStdout.seek(0)
    logger.info("Options: " + newStdout.read())

    # Get the TAZs from the TAZ file
    logger.info("Get TAZs from file")
    tazs = _getTazsFromFile(options.tazfile, logger)
    logger.info('TAZs: ' + ','.join(tazs))

    # Get the taz groups from the command line option value
    if options.odgroups:
        logger.info("Get TAZs groups");
        odgroups = _getTazGroups(options.odgroups, logger)
    else:
        # If no OD groups defined, generate traffic demand from every TAZ to every TAZ
        odgroups = tuple()
    logger.info("O/D Groups: %s" % (odgroups,))

    # Build the nettaz file if it doesn't exist
    if options.rebuild  or  not os.path.isfile(options.nettazfile):
        logger.info("Build the network with TAZ file")
        _buildNetTazFile(options.netfile,
                         options.tazfile,
                         options.nettazfile,
                         logger)
        logger.info("Success: network with TAZ data file \"%s\" has been created" % options.nettazfile)

    # Build the O/D matrix file
    if options.rebuild  or  not os.path.isfile(options.odfile):
        logger.info("Build the O/D matrix file")
        _writeODFile(options.odfile,
                     options.begin,
                     options.end,
                     tazs,
                     options.tazfile,
                     odgroups,
                     options.meannum,
                     options.stddev,
                     options.vtype,
                     logger)
        logger.info("Success: O/D matrix file \"%s\" has been created" % options.odfile)

    # Run the od2trips application
    logger.info("Call od2trips application")
    status = _callOD2trips(options.nettazfile,
                           options.odfile,
                           options.tripfile,
                           options.begin,
                           options.end,
                           logger)

    if not os.path.isfile(options.tripfile):
        sys.exit("Missing trips output file \'%s\'" % options.tripfile)

    # Run the duarouter application
    if options.routefile:
        logger.info("Generate routes with duarouter")
        status = _callDuaRouter(options.netfile,
                                options.rtypfile,
                                options.tripfile,
                                options.routefile,
                                properties['ROUTING_ALGORITHM'],
                                logger)

        if not os.path.isfile(options.routefile):
            sys.exit("Missing routes output file \'%s\'" % options.routefile)

    return 0


# .................................................. This is the main entry point of this script ...
if __name__ == "__main__":
    # Check if the SUMO_HOME environment variable is set
    try:
        const.SUMO_HOME = os.environ['SUMO_HOME']
    except KeyError:
        error = "Environment variable \'SUMO_HOME\' is not defined. Set it to the path where your SUMO distribution is installed"
        sys.exit(error)

    # Load the properties file
    properties = {}
    execfile(const.PROPERTIES_FILE, properties)

    # Get a logger for the entire application
    import runner
    logger = runner.getAppLogger()

    # Execute the script
    run(properties = properties)

