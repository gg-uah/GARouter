/*
 * GARDetLoader.cpp
 *
 *  Created on: Jun 4, 2015
 *      Author: ctomas
 */

#include <GARDetLoader.hpp>
#include <GARDetectorHandler.hpp>
#include <GARUtils.hpp>
#include <StringUtilities.hpp>
#include <utils/xml/XMLSubSys.h>
#include <utils/common/FileHelpers.h>

using std::vector;
using std::map;
using std::string;
using gar::GARUtils;
using common::StringUtilities;


namespace gar {

//................................................. Parameterized constructor ...
GARDetLoader::GARDetLoader(const OptionsCont& options,
		 	 	 	 	   const GARNet& net,
						   CLogger* logger)
: pDetCont (new GARDetectorCon()),
  options  (options),
  net      (net),
  logger   (logger) {

	// Intentionally left empty
}


//................................................. Gets the detector data container ...
std::shared_ptr<GARDetectorCon> GARDetLoader::getpDetCont(void) const {
	return this->pDetCont;
}


//................................................. Sets the detector data container ...
void GARDetLoader::setpDetCont(const std::shared_ptr<GARDetectorCon>& pDetCont) {
	this->pDetCont = pDetCont;
}


//................................................. Runs the detector data loader ...
int GARDetLoader::run(void) {
	try {
		if (!options.isSet("detector-files")) {
			logger->error("No detector file given (use --detector-files <FILE>).");
			return 1;
		}

		// Read definitions stored in XML-format
		vector<string> detFiles = options.getStringVector("detector-files");
		for (auto file : detFiles) {
			logger->info("Loading detectors from file [" + file + "]");

			if (!FileHelpers::isReadable(file)) {
				throw ProcessError("Could not open detector file '" + file + "'");
			}

			PROGRESS_BEGIN_MESSAGE("Loading detector definitions from '" + file + "'");
			logger->info("Loading detector definitions from '" + file + "'");

			GARDetectorHandler handler(net,
									   options.getBool("ignore-invalid-detectors"),
									   *pDetCont,
									   file);
			if (XMLSubSys::runParser(handler, file)) {
				PROGRESS_DONE_MESSAGE();
				logger->info("Done");
			} else {
				PROGRESS_FAILED_MESSAGE();
				logger->info("Fail to load detectors from file '" + file + "'");
				throw ProcessError();
			}
		}

		if (pDetCont->getDetectors().empty()) {
			throw ProcessError("No detectors found.");
		}

	} catch(std::exception& ex) {
		logger->error("Fail to load detectors: " + string(ex.what()));
		return 1;
	}

	// Set the type of the detectors
	computeDetectorTypes();

	logger->info(StringUtilities::toString((long)pDetCont->getDetectors().size()) + " detectors loaded");

	return 0;
}


//................................................. Computes the detector types ...
int GARDetLoader::computeDetectorTypes(void) {
	try {
		if (!pDetCont->detectorsHaveCompleteTypes()  ||  options.getBool("revalidate-detectors")) {
			net.computeTypes(*pDetCont, options.getBool("strict-sources"));
		}
	} catch (std::exception& ex) {
		logger->error("Fail to compute the detector types: " + string(ex.what()));
		return 1;
	}

	return 0;
}


//................................................. Check the detector data ...
int GARDetLoader::checkDetectors(void) const {
	// Check whether the detectors are valid
	if (!pDetCont->detectorsHaveCompleteTypes()) {
		logger->error("The detector types are not defined; use in combination with a network");
		return 1;
	}

	// Check whether the detectors have routes
	if (!pDetCont->detectorsHaveRoutes()) {
		logger->error("The detectors have no routes; use in combination with a network");
		return 1;
	}

	return 0;
}


//................................................. Save the detectors into data files ...
int GARDetLoader::saveDetectors(void) {
	try {
		// Save the detectors if wished
		if (options.isSet("detector-output")) {
			logger->info("Saving detectors into [" + options.getString("detector-output") + "]");
			pDetCont->save(options.getString("detector-output"));
		}

		// Save their positions as POIs if wished
		if (options.isSet("detectors-poi-output")) {
			logger->info("Saving detectors POI into [" + options.getString("detectors-poi-output") + "]");
			pDetCont->saveAsPOIs(options.getString("detectors-poi-output"));
		}
	} catch (const std::exception& ex) {
		logger->error("Fail to save detectors: " + string(ex.what()));
		return 1;
	}

	return 0;
}


//................................................. Logs the detector data ...
void GARDetLoader::logDetectors(void) const {
	logger->info("............... Detector list ...");
	for (GARDetector* pDet : this->pDetCont->getDetectors()) {
		logger->info("Detector: [" + pDet->getID() + "]");
		logger->info("\tType: [" + StringUtilities::toString(pDet->getType()) + "]");
		logger->info("\tEdge: [" + pDet->getEdgeID() + "]");
		logger->info("\tLane: ["+ pDet->getLaneID() + "]");
		logger->info("\tPosition: [" + StringUtilities::toString(pDet->getPos()) + "]");
		for (map<GAREdge*, SUMOReal> mapProbabilities : pDet->getSplitProbabilities()) {
			for (std::pair<GAREdge*, SUMOReal> prob : mapProbabilities) {
				GAREdge* pEdge = prob.first;
				SUMOReal prVal = prob.second;
				logger->info("\tSplit probability for edge '" + pEdge->getID() + "': [" + StringUtilities::toString(prVal) + "]");
			}
		}
		for (GARRouteDesc rd : pDet->getRouteVector()) {
			this->logRouteDesc(rd);
		}
	}
}


//................................................. Logs the route description ...
void GARDetLoader::logRouteDesc(const GARRouteDesc& rd) const {
	logger->info("\tRoute [" + rd.routename
			+ "], distance [" + StringUtilities::toString(rd.distance)
			+ "], duration [" + StringUtilities::toString(rd.duration_2)
			//+ "], passedNo [" + StringUtilities::toString(rd.passedNo)
			//+ "], endDetEdge [" + rd.endDetectorEdge->getID()
			//+ "], lastDetEdge [" + rd.lastDetectorEdge->getID()
			+ "], distance2last [" + StringUtilities::toString(rd.distance2Last)
			+ "], duration2last [" + StringUtilities::toString(rd.duration2Last)
			+ "], overallProb [" + StringUtilities::toString(rd.overallProb)
			+ "], factor [" + StringUtilities::toString(rd.factor) + "]");
	logger->info("\t\tedges [" + GARUtils::toString(rd.edges2Pass) + "]");
}


} /* namespace gar */
