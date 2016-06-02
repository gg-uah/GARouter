#include <GARConst.hpp>
#include <stdlib.h>


using std::string;

//...................................................... GAR model parameters configuration file ...
const string GARConst::GAR_PARAMS_CONFIG_FILE(getenv("RTGAR") + string("/config/garouter_params_config.dat"));


//...................................................... Time constants ...
const unsigned int GARConst::SECS_PER_MIN  = 60;

const unsigned int GARConst::MINS_PER_HOUR = 60;

const unsigned int GARConst::HOURS_PER_DAY = 24;

const unsigned int GARConst::SECS_PER_HOUR = GARConst::MINS_PER_HOUR * GARConst::SECS_PER_MIN;

const unsigned int GARConst::MINS_PER_DAY  = GARConst::HOURS_PER_DAY * GARConst::MINS_PER_HOUR;

const unsigned int GARConst::SECS_PER_DAY  = GARConst::MINS_PER_DAY  * GARConst::SECS_PER_MIN;

const unsigned int GARConst::DEFAULT_BEGIN = 12 * GARConst::SECS_PER_HOUR; //12:00

const unsigned int GARConst::DEFAULT_END   = GARConst::DEFAULT_BEGIN + 30 * GARConst::SECS_PER_MIN;  //12:30

const unsigned int GARConst::DEFAULT_STEP  = GARConst::SECS_PER_MIN;

const unsigned int GARConst::BEGIN_OF_DAY_SECS = 0;

const unsigned int GARConst::END_OF_DAY_SECS = GARConst::SECS_PER_DAY;

const bool GARConst::NO_HIGHWAY_MODE = false;

const std::string GARConst::OD_GROUP_ROOT_TAG = "odgroups";

const std::string GARConst::OD_GROUP_TAG = "odgroup";

const std::string GARConst::TAZ_ROOT_TAG = "tazs";

const std::string GARConst::TAZ_TAG = "taz";

const std::string GARConst::TAZ_SOURCE_TAG = "tazSource";

const std::string GARConst::TAZ_SINK_TAG = "tazSink";

const std::string GARConst::ID_ATTRIB = "<xmlattr>.id";

const std::string GARConst::EDGES_ATTRIB = "<xmlattr>.edges";
