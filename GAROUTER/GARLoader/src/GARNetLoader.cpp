/*
 * GARNetLoader.cpp
 *
 *  Created on: May 25, 2015
 *      Author: ctomas
 */

#include <GARNetLoader.hpp>
#include <GAREdgeBuilder.hpp>
#include <router/ROLoader.h>
#include <router/ROLane.h>
#include <StringUtilities.hpp>

using std::string;
using std::vector;
using common::StringUtilities;

namespace gar {

//................................................. Parameterized constructor ...
GARNetLoader::GARNetLoader(const OptionsCont& options, CLogger *logger)
: pNet (new GARNet()),
  options (options),
  logger (logger) {

	// Intentionally left empty
}


//................................................. Gets the pointer to the network data ...
std::shared_ptr<GARNet> GARNetLoader::getpNet(void) const {
	return this->pNet;
}


//................................................. Sets the pointer to the network data ...
void GARNetLoader::setpNet(const std::shared_ptr<GARNet>& pNet) {
	this->pNet = pNet;
}


//................................................. Loads the network data ...
int GARNetLoader::run(void) {
	if (!options.isSet("net-file")) {
		logger->error("No network file given (use --net-file <FILE>).");
		return 1;
	}

	logger->info("Loading network from file '" + options.getString("net-file") + "'");
	try {
		ROLoader loader(const_cast<OptionsCont&>(options), false, !options.getBool("no-step-log"));
		GAREdgeBuilder builder;

		//Load the network edges and set the approaching and approached edges
		loader.loadNet(*pNet, builder);
		pNet->buildApproachList();

	} catch (ProcessError& ex) {
		logger->fatal("Fail to load network: " + string(ex.what()));
		return 1;
	}

	logger->info(StringUtilities::toString(pNet->getEdgeNo()) + " edges loaded");

	return 0;
}


//................................................. Logs the network data ...
void GARNetLoader::logNet(void) const {
	logger->info("............... Network data ...");
	for (auto edgeMap : pNet->getEdgeMap()) {
		string edgeId = edgeMap.first;
		ROEdge* pEdge  = edgeMap.second;

		// Omit internal edges starting with ':'
		if (StringUtilities::startsWith(edgeId, ":")) {
			continue;
		}

		logger->info("Edge [" + edgeId + "]");
		logger->info("\tType: " + StringUtilities::toString(pEdge->getType()));
		logger->info("\tLength: " + StringUtilities::toString(pEdge->getLength()));
		logger->info("\tSpeed : " + StringUtilities::toString(pEdge->getSpeed()));
		for (auto lane : pEdge->getLanes()) {
			logger->info("\tLane: " + lane->getID());
		}
		for (int i = 0; i < pEdge->getNumSuccessors(); i++) {
			ROEdge* sucessor = pEdge->getSuccessor(i);
			logger->info("\tSucessor [" + StringUtilities::toString(i) + "]: " + sucessor->getID());
		}
		for (int i = 0; i < pEdge->getNumPredecessors(); i++) {
			ROEdge* predecessor = pEdge->getPredecessor(i);
			logger->info("\tPredecessor [" + StringUtilities::toString(i) + "]: " + predecessor->getID());
		}
		try {
			vector<string> detectors = pNet->getDetectorList(pEdge);
			if (!detectors.empty()) {
				logger->info("\tDetectors: " + StringUtilities::toString(pNet->getDetectorList(pEdge)));
			} else {
				logger->info("\tNo detectors defined");
			}
		} catch (std::exception &ex) {
			logger->debug("\tNo detectors defined");
		}
	}
}

} /* namespace gar */
