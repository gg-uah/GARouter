/*
 * GAROptions.cpp
 *
 *  Created on: Apr 24, 2015
 *      Author: ctomas
 */

#include <GAROptions.hpp>
#include <GARFrame.hpp>
#include <utils/common/StdDefs.h>
#include <utils/options/Option.h>
#include <utils/options/OptionsIO.h>
#include <utils/common/SystemFrame.h>
#include <utils/xml/XMLSubSys.h>
#include <StringUtilities.hpp>


using std::string;
using common::StringUtilities;

namespace gar {

//................................................. Default constructor ...
GAROptions::GAROptions(void)
: optCont (OptionsCont::getOptions()) {

}


//................................................. Default constructor ...
int GAROptions::init(const int& argc, char ** argv, CLogger* logger) {
	try {
		//Give some application descriptions
		optCont.setApplicationDescription("Builds vehicle routes for SUMO using detector values and genetic algorithms.");
		optCont.setApplicationName("garouter", "SUMO garouter Version " + getBuildName(VERSION_STRING));

		// Initialize the XML system
		logger->debug("Initialize the XML system");
		XMLSubSys::init();

		// Initialize the options
		logger->debug("Initialize the ga-router options");
		GARFrame::fillOptions();
		OptionsIO::getOptions(true, argc, argv);

		// Check for the right option configuration
		if (optCont.processMetaOptions(argc < 2)) {
			logger->fatal("Shutdown: GA router started up with wrong or no options");
			SystemFrame::close();
			return 1;
		}

		// Check options for the ga-router
		if (!GARFrame::checkOptions()) {
			logger->error("Fail to check ga-router options");
			throw ProcessError();
		}

	} catch (std::exception& ex) {
		logger->fatal("Fail to initialize the ga-route configuration options: " + string(ex.what()));
		return 1;
	}

	this->logOptions(logger);

	return 0;
}


//................................................. Get the option container ...
const OptionsCont& GAROptions::getOptions(void) const {
	return this->optCont;
}


//................................................. Log the ga-router options ...
void GAROptions::logOptions(CLogger* logger) const {
	logger->debug("............... GA router options ...");
	for (string subtopic : optCont.getSubTopics()) {
		logger->debug("Subtopic [" + subtopic + "]");

		for (string entry: optCont.getSubTopicsEntries(subtopic)) {
			try {
				logger->debug("\tEntry [" + entry + "]: " + optCont.getString(entry));
			} catch (InvalidArgument&) {
				try {
					logger->debug("\tEntry [" + entry + "]: " + StringUtilities::toString(optCont.getBool(entry)));
				} catch (InvalidArgument&) {
					try {
						logger->debug("\tEntry [" + entry + "]: " + StringUtilities::toString(optCont.getInt(entry)));
					} catch (InvalidArgument&) {
						try {
							logger->debug("\tEntry [" + entry + "]: " + StringUtilities::toString(optCont.getFloat(entry)));
						} catch (InvalidArgument&) {
							try {
								logger->debug("\tEntry [" + entry +"]: " + StringUtilities::toString(optCont.getStringVector(entry)));
							} catch (InvalidArgument&) {
								try {
									logger->debug("\tEntry [" + entry +"]: " + StringUtilities::toString(optCont.getIntVector(entry)));
								} catch (InvalidArgument&) {
									logger->error("Unknown type for entry [" + entry + "]");
								}
							}
						}
					}
				}
			}
		}
	}
}


} /* namespace gar */
