__author__ = 'ctomas'

import optparse
import logging
import logging.config
import StringIO
import __builtin__
from constants import *

# Import modules from commonutils package
try:
    from commonutils import const, logutils, netutils, fileutils
except ImportError:
    sys.exit("Failed to import from \'commonutils\' package")


def getAppLogger():
    '''
    Get a cross-module logger for the application
    '''
    logging.config.fileConfig(const.LOG_CFG_FILE)
    __builtin__.logger = logging.getLogger(const.APP_NAME)

    # Make logger a cross-module scope variable
    return __builtin__.logger


# .................................................. Local functions ...
def _getCommandOptions(arguments, logger):
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

    parser.add_option("-m", "--measure-file", dest="measfile", default=const.DEFAULT_MEAS_FILE,
                      help="define the output filename where the induction loops measurements are to be registered (\'%s\' by default)" % const.DEFAULT_MEAS_FILE)

    parser.add_option("-f", "--measure-freq", type=int, dest="measfreq", default=const.DEFAULT_MEAS_FREQ,
                      help="define the frequency in seconds of the induction loop measurements (\'%s\' by default)" % const.DEFAULT_MEAS_FREQ)

    parser.add_option("-l", "--output-loop-file", dest="loopfile", default=const.DEFAULT_LOOP_FILE,
                      help="define the output loop filename (\'%s\' by default)" % const.DEFAULT_LOOP_FILE)

    parser.add_option("-d", "--output-detector-file", dest="detfile", default=const.DEFAULT_DET_FILE,
                      help="define the output detector filename (\'%s\' by default)" % const.DEFAULT_DET_FILE)

    (options, args) = parser.parse_args(args=arguments)

    if not options.netfile:
        logger.critical("Missing mandatory arguments")
        sys.exit(1)

    return options


def _writeInductionLoopFile(lanes, loopfile, measfile, freq, logger):
    '''
    Build the induction loop file from the network file.
    An induction loop is placed in every lane in the network.
    :param lanes:       The network lane list
    :param loopfile:    The induction loop file
    :param measfile:    The induction loop measurements file
    :param freq:        The frequency in seconds at which the induction loops register the measurements
    :param properties:  The project properties loaded from the configuration file
    :param logger:      The module logger
    :return:
    '''
    try:
        with open(loopfile, 'w') as f:
            f.write(const.ADD_FILE_HEADER)
            for lane in lanes:
                id = lane['id']
                len = lane['length']
                midPos = float(len) / 2

                f.write("\n\t<{0} ".format(const.INDUCTION_LOOP_TAG))
                f.write("{0}=\"loop_{1}\" ".format(const.ID_ATTRIB, id))
                f.write("{0}=\"{1}\" ".format(const.LANE_ATTRIB, id))
                f.write("{0}=\"{1}\" ".format(const.POS_ATTRIB, midPos))
                f.write("{0}=\"{1}\" ".format(const.FREQ_ATTRIB, freq))
                f.write("{0}=\"{1}\"/>".format(const.FILE_ATTRIB, measfile))
            f.write("\n{0}".format(const.ADD_FILE_FOOTER))
    except IOError as ex:
        logger.critical('Fail to create the additional loop file \'%s\': %s' % (loopfile, ex.strerror))
        sys.exit(1)
    else:
        logger.info("Success: Created induction loop file \'%s\'" % loopfile)


def _writeDetectorFile(lanes, detfile, logger):
    '''
    Build the detector file from the network file.
    A detector is placed in every lane in the network.
    :param lanes:       The network lane list
    :param detfile:     The detector file
    :param properties:  The project properties loaded from the configuration file
    :param logger:      The module logger
    :return:
    '''
    try:
        with open(detfile, 'w') as f:
            f.write(const.DET_FILE_HEADER)
            for lane in lanes:
                id = lane['id']
                len = lane['length']
                midPos = float(len) / 2

                f.write("\n\t<{0} ".format(const.DET_DEF_TAG))
                f.write("{0}=\"det_{1}\" ".format(const.ID_ATTRIB, id))
                f.write("{0}=\"{1}\" ".format(const.LANE_ATTRIB, id))
                f.write("{0}=\"{1}\"/>".format(const.POS_ATTRIB, midPos))
            f.write("\n{0}".format(const.DET_FILE_FOOTER))
    except IOError as ex:
        logger.critical('Fail to create the additional detector file \'%s\': %s' % (detfile, ex.strerror))
        sys.exit(1)
    else:
        logger.info("Success: Created detector file \'%s\'" % detfile)


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
    options = _getCommandOptions(arguments, logger)

    # Log the options
    with logutils.redirectStdout(StringIO.StringIO()) as newStdout:
        sys.stdout.write(options)
    newStdout.seek(0)
    logger.info("Options: " + newStdout.read())

    logger.info("Get lanes from file [%s]" % options.netfile)
    lanes = netutils.getLanesFromNetFile(options.netfile)

    # Build the induction loop file
    logger.info("Build the induction loop file")
    _writeInductionLoopFile(lanes, options.loopfile, options.measfile, options.measfreq, logger)

    # Build the detectors file
    logger.info("Build the detector file")
    _writeDetectorFile(lanes, options.detfile, logger)

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
