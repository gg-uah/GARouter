import sys
import os

# Import const module from commonutils package
try:
    from commonutils import const
except ImportError:
    sys.exit("Failed to import from \'commonutils\' package")

# Application name
const.APP_NAME           = "testgar"

# Project scaffolding
const.BASE_DIR            = os.path.join(os.path.dirname(__file__), '..')
const.DATA_DIR            = "data"
const.LOG_DIR             = "log"
const.CONFIG_DIR          = "config"
const.SOURCE_DIR          = "src"

# Default input values
const.DEFAULT_BEGIN       = "12.00"
const.DEFAULT_END         = "12.30"
const.DEFAULT_MEAN_NUM    = 50
const.DEFAULT_STD_DEV     = 0.0
const.DEFAULT_MEAS_FREQ   = 1

# Default output files
const.DEFAULT_NET_FILE      = "network.net.xml"
const.DEFAULT_NETTAZ_FILE   = "network.nettaz.xml"
const.DEFAULT_TRIP_FILE     = "trips.trip.xml"
const.DEFAULT_ROUTE_FILE    = "routes.rou.xml"
const.DEFAULT_ROU_VTYP_FILE = "routes.vtyp.xml"
const.DEFAULT_OD_FILE       = "od-matrix.dat"
const.DEFAULT_LOOP_FILE     = "induction_loops.add.xml"
const.DEFAULT_DET_FILE      = "detectors.det.xml"
const.DEFAULT_MEAS_FILE     = "loops_measurements.xml"
const.DEFAULT_FLOW_FILE     = "detector_flows.dat"
const.DEFAULT_DFROUTE_FILE  = "dfroutes.rou.xml"
const.DEFAULT_EMITT_FILE    = "emitters.rou.xml"
const.DEFAULT_POI_FILE      = "detectors.poi.xml"
const.DEFAULT_VSS_FILE      = "detectors_sink.vss.xml"

# Configuration files
const.PROPERTIES_FILE     = os.path.join(const.BASE_DIR, const.CONFIG_DIR, 'properties.cfg')
const.LOG_CFG_FILE        = os.path.join(const.BASE_DIR, const.CONFIG_DIR, 'logging.cfg')

# Induction loop file tags
const.ADD_FILE_HEADER     = "<additional xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\" xsi:noNamespaceSchemaLocation=\"http://sumo.dlr.de/xsd/additional_file.xsd\">"
const.ADD_FILE_FOOTER     = "</additional>"
const.INDUCTION_LOOP_TAG  = "inductionLoop"

# Detector file tags
const.DET_FILE_HEADER     = "<detectors>"
const.DET_FILE_FOOTER     = "</detectors>"
const.DET_DEF_TAG         = "detectorDefinition"

# Measurement file tags
const.INTERVAL_TAG        = "interval"

# Tag attributes
const.ID_ATTRIB           = "id"
const.LANE_ATTRIB         = "lane"
const.POS_ATTRIB          = "pos"
const.FREQ_ATTRIB         = "freq"
const.FILE_ATTRIB         = "file"

const.BEGIN_ATTRIB        = "begin"
const.END_ATTRIB          = "end"
const.NVEHCONTRIB_ATTRIB  = "nVehContrib"
const.FLOW_ATTRIB         = "flow"
const.OCCUPANCY_ATTRIB    = "occupancy"
const.SPEED_ATTRIB        = "speed"
const.LENGTH              = "length"
const.NVEHENTERED_ATTRIB  = "nVehEntered"
const.MBEGIN_ATTRIB       = "mbegin"

# Flow file definitions
const.FLOW_FILE_HEADER      = "Detector;Time;qPKW;qLKW;vPKW;vLKW"
const.FLOW_ROU_FILE_HEADER  = "Detector;Time;qPKW"

# Regular expression pattern definitions
const.TAZ_GROUPS_PATTERN       = r'\((\w+(,\w+)*)\)(,\((\w+(,\w+)*)\))*'
const.SPLIT_TAZ_GROUPS_PATTERN = ',(?=\()'

# Miscellaneous constants
const.NET_END_TAG       = "</net>"
const.SECONDS_PER_MIN   = 60
