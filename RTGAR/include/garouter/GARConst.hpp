#ifndef __GAR_CONST_HPP
#define __GAR_CONST_HPP

#include <string>


/**
 * This class contains the definition of the custom constants necessary
 * for the development of the custom applications.
 */
class GARConst {

public:
    /**
     * File name for the GAR model configuration parameters
     */
     const static std::string GAR_PARAMS_CONFIG_FILE;

     /**
      * Time constants
      */
     const static unsigned int SECS_PER_MIN;  //!< Seconds per minute

     const static unsigned int MINS_PER_HOUR; //!< Minutes per hour

     const static unsigned int HOURS_PER_DAY; //!< Hours per day

     const static unsigned int SECS_PER_HOUR; //!< Seconds per hour

     const static unsigned int MINS_PER_DAY;  //!< Minutes per day

     const static unsigned int SECS_PER_DAY;  //!< Seconds per day

     const static unsigned int BEGIN_OF_DAY_SECS;	//!< Begin of a day in seconds

     const static unsigned int END_OF_DAY_SECS;		//!< End of a day in seconds

     /**
      * Default values
      */
     const static unsigned int DEFAULT_BEGIN; //!< Default begin time in seconds

     const static unsigned int DEFAULT_END;	  //!< Default end time in seconds

     const static unsigned int DEFAULT_STEP;  //!< Default step time in seconds between two successive flow data sets

     const static bool NO_HIGHWAY_MODE;  	  //!< Default highway mode set to the network as used by the router

     /**
      * XML tags
      */
     const static std::string OD_GROUP_ROOT_TAG;	//!< The root tag in the TAZ groups file

     const static std::string OD_GROUP_TAG;			//!< The TAZ group tag

     const static std::string TAZ_ROOT_TAG;			//!< The root tag in the TAZ file

     const static std::string TAZ_TAG;				//!< The TAZ tag

     const static std::string TAZ_SOURCE_TAG;		//!< The TAZ tag

     const static std::string TAZ_SINK_TAG;			//!< The TAZ tag

     const static std::string ID_ATTRIB;			//!< The id attribute

     const static std::string EDGES_ATTRIB;			//!< The edges attribute

};

#endif
