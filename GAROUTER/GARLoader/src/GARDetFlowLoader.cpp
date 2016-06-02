/*
  * GARDetFlowLoader.cpp
 *
 *  Created on: Jun 4, 2015
 *      Author: ctomas
 */

#include <GARDetFlowLoader.hpp>
#include <StringUtilities.hpp>
#include <utils/importio/LineReader.h>
#include <utils/importio/NamedColumnsParser.h>
#include <utils/common/MsgHandler.h>
#include <utils/common/FileHelpers.h>
#include <utils/common/TplConvert.h>
#include <utils/common/UtilExceptions.h>

using std::string;
using common::StringUtilities;

namespace gar {

//................................................. Parameterized constructor ...
GARDetFlowLoader::GARDetFlowLoader(const OptionsCont& options,
								   const GARDetectorCon& detCont,
								   CLogger* logger)
: pFlows  (new GARDetectorFlows(string2time(options.getString("begin")),
	 	 	 	  	  	  	    string2time(options.getString("end")),
								string2time(options.getString("time-step")))),
  options (options),
  detCont (detCont),
  logger  (logger) {

	// Intentionally left empty
}


//................................................. Gets the detector flows data ...
std::shared_ptr<GARDetectorFlows> GARDetFlowLoader::getpFlows(void) const {
	return this->pFlows;
}


//................................................. Sets the detector flows data ...
void GARDetFlowLoader::setpFlows(const std::shared_ptr<GARDetectorFlows>& pFlows) {
	this->pFlows = pFlows;
}


//................................................. Runs the detector flows data loader ...
int GARDetFlowLoader::run(void) {
	bool warnedOverridingBoundaries = false; // Whether a warning about overriding boundaries was already written
	bool warnedPartialDefs = false;  		 // Whether a warning about partial definitions was already written

	// Get the first time step to read from the options
	if (!options.isSet("begin")) {
		logger->fatal("The 'begin' option is not specified.");
		return 1;
	}
	const SUMOTime startTime = string2time(options.getString("begin"));

	// Get the the last time step to read from the options
	if (!options.isSet("end")) {
		logger->fatal("The 'end' option is not specified.");
		return 1;
	}
	const SUMOTime endTime = string2time(options.getString("end"));

	// Get the time offset to apply to read time values from the options
	if (!options.isSet("time-offset")) {
		logger->fatal("The 'time-offset' option is not specified.");
		return 1;
	}
	const SUMOTime timeOffset = string2time(options.getString("time-offset"));

	// Get the time scale to apply to read time values from the options
	if (!options.isSet("time-factor")) {
		logger->fatal("The 'time-factor' option is not specified.");
		return 1;
	}
	const SUMOTime timeScale = string2time(options.getString("time-factor"));

	// Get the detector flow measurement files from the options
	if (!options.isSet("measure-files")) {
		// ok, not given, return an empty container
		return 1;
	}

	try {
		// Check whether the file exists
		for (string file : options.getStringVector("measure-files")) {
			logger->info("Loading flow amounts from file [" + file + "]");

			if (!FileHelpers::isReadable(file)) {
				throw ProcessError("The measure-file '" + file + "' can not be opened.");
			}

			// Parse the measure file
			PROGRESS_BEGIN_MESSAGE("Reading flows from '" + file + "'");
			this->read(file,
					   startTime,
					   endTime,
					   timeOffset,
					   timeScale,
					   warnedOverridingBoundaries,
					   warnedPartialDefs);
			PROGRESS_DONE_MESSAGE();
		}
	} catch (std::exception& ex) {
		logger->error("Fail to load detector flows: " + string(ex.what()));
		return 1;
	}

	return 0;
}


//................................................. Reads the detector flow measurements file ...
void GARDetFlowLoader::read(const std::string& file,
		  	  	  	  	    SUMOTime startTime,
							SUMOTime endTime,
							SUMOTime timeOffset,
							SUMOTime timeScale,
							bool& warnedOverridingBoundaries,
							bool& warnedPartialDefs) {
	LineReader lr (file);
	NamedColumnsParser myLineHandler;  // The value extractor

	// Parse first line
	myLineHandler.reinit(lr.readLine(), ";", ";", true, true);

	// Parse values
	while (lr.hasMore()) {
		string line = lr.readLine();
		if (line.find(';') == string::npos) {
			continue;
		}

		myLineHandler.parseLine(line);

		try {
			std::string detName = myLineHandler.get("detector");
			if (!detCont.knows(detName)) {
				continue;
			}

			const SUMOReal parsedTime = TplConvert::_2SUMOReal(myLineHandler.get("time").c_str()) * timeScale - timeOffset;

			// Parsing as float to handle values which would cause int overflow
			if (parsedTime < startTime || parsedTime >= endTime) {
				if (!warnedOverridingBoundaries) {
					warnedOverridingBoundaries = true;
					WRITE_WARNING("At least one value lies beyond given time boundaries.");
				}
				continue;
			}

			const SUMOTime time = (SUMOTime) (parsedTime + .5);

			FlowDef fd;
			fd.isLKW = 0;
			fd.qPKW = TplConvert::_2SUMOReal(myLineHandler.get("qpkw").c_str());
			fd.vPKW = 0;
			if (myLineHandler.know("vPKW")) {
				fd.vPKW = TplConvert::_2SUMOReal(myLineHandler.get("vpkw").c_str());
			}
			fd.qLKW = 0;
			if (myLineHandler.know("qLKW")) {
				fd.qLKW = TplConvert::_2SUMOReal(myLineHandler.get("qlkw").c_str());
			}
			fd.vLKW = 0;
			if (myLineHandler.know("vLKW")) {
				fd.vLKW = TplConvert::_2SUMOReal(myLineHandler.get("vlkw").c_str());
			}
			if (fd.qLKW < 0) {
				fd.qLKW = 0;
			}
			if (fd.qPKW < 0) {
				fd.qPKW = 0;
			}

			pFlows->addFlow(detName, time, fd);

			if (!warnedPartialDefs  &&  !myLineHandler.hasFullDefinition()) {
				warnedPartialDefs = true;
				WRITE_WARNING("At least one line does not contain the correct number of columns.");
			}
			continue;
		} catch (UnknownElement&) {
		} catch (OutOfBoundsException&) {
		} catch (NumberFormatException&) {
		} throw ProcessError("The detector-flow-file '" + lr.getFileName()
							+ "' is corrupt;\n"
							+ " The following values must be supplied : 'Detector', 'Time', 'qPKW'\n"
							+ " The according column names must be given in the first line of the file.");
	}
}


//................................................. Logs the detector flow data set ...
void GARDetFlowLoader::logDetectorFlows(void) const{
	logger->info("............... Detector flow list ...");
	for (GARDetector* pDet : detCont.getDetectors()) {
		string detId = pDet->getID();
		logger->info("Flow measurements for detector [" + detId + "]");
		for (FlowDef fd : pFlows->getFlowDefs(detId)) {
			logger->info("\tfirstSet: [" + StringUtilities::toString(fd.firstSet)
					+ "], qPKW:  [" + StringUtilities::toString(fd.qPKW)
					+ "], vPKW:  [" + StringUtilities::toString(fd.vPKW)
					+ "], isLKW: [" + StringUtilities::toString(fd.isLKW)
					+ "], qLKW:  [" + StringUtilities::toString(fd.qLKW)
					+ "], vLKW:  [" + StringUtilities::toString(fd.vLKW)
					+ "], fLKW:  [" + StringUtilities::toString(fd.fLKW) + "]");		}
	}

	if (options.getBool("print-absolute-flows")) {
		PROGRESS_BEGIN_MESSAGE("Absolute flows\n");
		pFlows->printAbsolute();
		PROGRESS_DONE_MESSAGE();
	}
}


} /* namespace gar */
