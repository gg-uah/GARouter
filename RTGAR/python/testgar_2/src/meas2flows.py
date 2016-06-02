__author__ = 'ctomas'

import optparse
import logging
import logging.config
import StringIO
import __builtin__
from constants import *

# Import modules from commonutils package
try:
    from commonutils import const, netutils, logutils, fileutils
except ImportError:
    sys.exit("Failed to import from \'commonutils\' package")

# Import etree from lxml package
try:
    from lxml import etree
except ImportError:
    sys.exit("Failed to import ElementTree from lxml")


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

    parser.add_option("-m", "--measure-files", dest="measfile",
                      help="define the induction loop measurement files (mandatory)")

    parser.add_option("-o", "--output-flow-file", dest="flowfile", default=const.DEFAULT_FLOW_FILE,
                      help="define the flow output filename for feeding the \'dfrouter\' (\'%s\' by default)" % const.DEFAULT_FLOW_FILE)

    parser.add_option("-b", "--begin", dest="begin",
                      help="start time (mandatory)")

    (options, args) = parser.parse_args(args=arguments)

    if not options.netfile  or  not options.measfile:
        logger.critical("Missing mandatory arguments")
        sys.exit(1)

    return options


def _addBeginMinAttrib(measfile, measInterval, logger):
    '''
    Add a 'beginmin' attribute to every 'interval' element in the induction loop measurement file.
    The 'beginmin' attribute reports the interval time period beginning in minutes.
    :param measfile:     The induction loop measurement file
    :param measInterval: The flow interval for collecting the detector data
    :param logger:       The application logger
    :return:             The XML document containing the 'beginmin' attribute in the 'interval' elements
    '''
    try:
        # Parse the measurements file
        tree = etree.parse(measfile)

        # The root points to the <detector> tag
        doc = tree.getroot()

        # Get the begin times from the loop measurements
        beginlist = sorted(set([ interval.get('begin') for interval in doc.findall('interval') ]))

        # The start time is the minimum begin time
        if beginlist:
            startSecs = int(float(min(beginlist)))
        else:
            startSecs = 0

        # Add a new attribute 'beginmin' to every 'interval' element that reports the interval time period beginning (in minutes)
        for interval in doc:
            beginSecs = int(float(interval.attrib['begin']))
            beginMins = (startSecs / const.SECONDS_PER_MIN) + int((beginSecs - startSecs) / (const.SECONDS_PER_MIN * measInterval)) * measInterval
            interval.attrib['mbegin'] = str(beginMins)
    except etree.XMLSyntaxError as ex:
        return None
    else:
        return doc


def _getFlowMeasurements(lanes, measfile, measInterval, logger):
    '''
    Collect the measurements from every induction loop in the network
    :param lanes:        The lanes in the network
    :param measfile:     The induction loop measurement file
    :param measInterval: The flow interval for collecting the detector data
    :param logger:       The module logger
    :return:             A list containing the flow data in every lane in the network
    '''
    flows = list()

    if not os.path.isfile(measfile):
        logger.critical("Missing measurement file \'%s\'" % measfile)
        sys.exit(1)

    doc = _addBeginMinAttrib(measfile, measInterval, logger)
    if doc is None:
        logger.error("Fail to add the time period begin in minutes to 'interval' element in the measurement file [%s]" % measfile, exc_info=True);
        sys.exit(1)

    # Get the interval registrations for every lane
    for id in [ lane['id'] for lane in lanes ]:
        # Sort the list of unique 'mbegin' attribute list
        for mbegin in sorted(set([ interval.attrib['mbegin'] for interval in doc.findall('interval') ])):
            num = 0
            totalVeh = 0
            totalSpeed = 0.0
            for interval in doc.findall("interval[@mbegin='{0}'][@id='loop_{1}']".format(mbegin, id)):
                logger.debug("interval id=[%s], mbegin=[%s] begin=[%s], end=[%s], speed=[%s], nVehEntered=[%s]" % (
                        interval.attrib['id'], interval.attrib['mbegin'], interval.attrib['begin'], interval.attrib['end'], interval.attrib['speed'], interval.attrib['nVehEntered']))

                nVehEntered = int(interval.attrib['nVehEntered'])
                speed = float(interval.attrib['speed'])
                mbegin = float(interval.attrib['mbegin'])
                if nVehEntered > 0:
                    num += 1
                    totalVeh += nVehEntered
                    totalSpeed += speed

            avgSpeed = totalSpeed/num if num > 0 else totalSpeed

            # Append flow data
            logger.debug('Append flow data -> id: [{0}], mbegin: [{1}], totalVeh: [{2}], avgSpeed: [{3}]'.format(id, mbegin, totalVeh, avgSpeed))
            flowData = {'id' : id, 'mbegin' : int(mbegin), 'totalVeh' : totalVeh, 'avgSpeed' : avgSpeed}
            flows.append(flowData)

    # Return the flow list sorted by the 'mbegin' key
    return sorted(flows, key = lambda k: k['mbegin'])


def _writeDetectorFlowFile(detflows, flowfile, logger):
    '''
    Write the detector flow file
    :param detflows:    A list containing the detector flow data in every lane of the network
    :param flowfile:    The flow measurement file
    :param properties:  The project parameters loaded from the configuration file
    :param logger:      The module logger
    :return:
    '''
    try:
        with open(flowfile, 'w') as f:
            f.write(const.FLOW_FILE_HEADER)
            for flow in detflows:
                id       = flow['id']
                mbegin   = flow['mbegin']
                totalVeh = flow['totalVeh']
                avgSpeed = flow['avgSpeed']

                f.write("\ndet_{0};{1};{2};{3};{4:.2f};{5}".format(id, mbegin, totalVeh, 0, avgSpeed, 0))
            f.write('\n')
    except IOError as ex:
        logger.critical('Fail to create the detector flow file \'%s\': %s' % (flowfile, ex.strerror))
        sys.exit(1)
    else:
        logger.info("Success: Created detector flow file \'%s\'" % flowfile)


def run(properties, arguments = None):
    """
    Actions to be performed when this module is executed as a script.
    :param properties:  The project properties loaded from the configuration file
    :param arguments:   Argument list passed in the script call
    """
    const = __import__('constants').const

    # Get the application logger
    logger = __builtin__.logger

    # Get the arguments from the command line
    options = _getCommandOptions(arguments, properties, logger)

    # Log the options
    with logutils.redirectStdout(StringIO.StringIO()) as newStdout:
        sys.stdout.write(options)
    newStdout.seek(0)
    logger.info("Options: " + newStdout.read())

    # Get the network lanes
    logger.info("Get lanes from file [%s]" % options.netfile)
    lanes = netutils.getLanesFromNetFile(options.netfile)

    # Collect the measurements from the induction loops
    logger.info("Collect measurements from induction loops")
    flows = _getFlowMeasurements(lanes, options.measfile, properties['MEAS_INTERVAL_MINS'], logger)

    # Write the detector flow file
    _writeDetectorFlowFile(flows, options.flowfile, logger)

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
    logger = getAppLogger()

    # Execute the script
    run(properties = properties)
