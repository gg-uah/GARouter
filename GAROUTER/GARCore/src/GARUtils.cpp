/*
 * GARUtils.cpp
 *
 *  Created on: Apr 25, 2015
 *      Author: ctomas
 */

#include <GARUtils.hpp>
#include <utils/common/SUMOTime.h>
#include <StringUtilities.hpp>
#include <algorithm>

using common::StringUtilities;
using std::string;

namespace gar {

const std::string GARUtils::SEPARATOR = ",";

//................................................. Get a comma-separated string of edge id's in a vector ...
std::string GARUtils::toString(const std::vector<ROEdge*>& edgeVector) {
	std::string str = "";
	auto addIdToStr = [&str] (ROEdge* pEdge) { str += (pEdge->getID() + SEPARATOR + " ");  };

	//For each edge in the vector, add the edge id to the string
	std::for_each(edgeVector.begin(), edgeVector.end(), addIdToStr);

	//Remove white spaces at the end of the string
	str = StringUtilities::trim(str);

	//Remove the last comma character
	if (StringUtilities::endsWith(str, SEPARATOR)) {
		str = StringUtilities::removeAtEnd(str, SEPARATOR);
	}

	return str;
}


//................................................. Get a comma-separated string of edge id's in a set ...
std::string GARUtils::toString(const gar::edge_set& edgeSet) {
	std::string str = "";
	auto addIdToStr = [&str] (const GAREdge const* pEdge) { str += (pEdge->getID() + SEPARATOR + " ");  };

	//For each edge in the vector, add the edge id to the string
	std::for_each(edgeSet.begin(), edgeSet.end(), addIdToStr);

	//Remove white spaces at the end of the string
	str = StringUtilities::trim(str);

	//Remove the last comma character
	if (StringUtilities::endsWith(str, SEPARATOR)) {
		str = StringUtilities::removeAtEnd(str, SEPARATOR);
	}

	return str;
}


//................................................. Get a comma-separated string of TAZ id's in a set ...
std::string GARUtils::toString(const gar::taz_set& tazSet) {
	std::string str = "";
	auto addIdToStr = [&str] (std::shared_ptr<const GARTaz> pTaz) { str += (pTaz->getId() + SEPARATOR + " "); };

	//For each edge in the vector, add the edge id to the string
	std::for_each(tazSet.begin(), tazSet.end(), addIdToStr);

	//Remove white spaces at the end of the string
	str = StringUtilities::trim(str);

	//Remove the last comma character
	if (StringUtilities::endsWith(str, SEPARATOR)) {
		str = StringUtilities::removeAtEnd(str, SEPARATOR);
	}

	return str;
}


//................................................. Get a comma-separated string of O/D group id's in a set ...
std::string GARUtils::toString(const gar::grp_set& grpSet) {
	std::string str = "";
	auto addIdToStr = [&str] (std::shared_ptr<const GARTazGroup> pGrp) { str += (pGrp->getId() + SEPARATOR + " "); };

	//For each edge in the vector, add the edge id to the string
	std::for_each(grpSet.begin(), grpSet.end(), addIdToStr);

	//Remove white spaces at the end of the string
	str = StringUtilities::trim(str);

	//Remove the last comma character
	if (StringUtilities::endsWith(str, SEPARATOR)) {
		str = StringUtilities::removeAtEnd(str, SEPARATOR);
	}

	return str;
}


//................................................. Get a comma-separated string of route names in a set ...
std::string GARUtils::toString(const gar::route_set& rouSet) {
	std::string str = "";
	auto addIdToStr = [&str] (std::shared_ptr<const GARRouteDesc> pRou) { str += (pRou->routename + SEPARATOR + " "); };

	//For each edge in the vector, add the edge id to the string
	std::for_each(rouSet.begin(), rouSet.end(), addIdToStr);

	//Remove white spaces at the end of the string
	str = StringUtilities::trim(str);

	//Remove the last comma character
	if (StringUtilities::endsWith(str, SEPARATOR)) {
		str = StringUtilities::removeAtEnd(str, SEPARATOR);
	}

	return str;
}


//................................................. Logs a trip vector ...
void GARUtils::logTrips(const gar::trip_vector& vTrips, CLogger* logger) {
	for (std::shared_ptr<const GARTrip> pTrip : vTrips) {
		logger->info("Trip from [" + pTrip->getpOrig()->getID() + "] to [" + pTrip->getpDest()->getID()+ "]");

		for (std::shared_ptr<const GARRouteDesc> pRd : pTrip->getOdRoutes()) {
			logger->info("\tRoute [" + pRd->routename + "]: " + GARUtils::toString(pRd->edges2Pass));
		}
	}
}


//................................................. Logs the detector flow data ...
void GARUtils::logDetectorFlows(const GARDetectorFlows& flowData, CLogger* logger) {
	for (auto flow : flowData.getFlows()) {
		std::string detectorId = flow.first;
		std::vector<FlowDef> flowDefs = flow.second;

		logger->info("Flow data for detector [" + detectorId + "]");
		for (const FlowDef& fd : flowDefs) {
			logger->info("\tfirstSet: [" + StringUtilities::toString<bool>(fd.firstSet)
						+ "], qPKW: [" + StringUtilities::toString<SUMOReal>(fd.qPKW)
						+ "], vPKW: [" + StringUtilities::toString<SUMOReal>(fd.vPKW)
						+ "], isLKW: [" + StringUtilities::toString<SUMOReal>(fd.isLKW)
						+ "], qLKW: [" + StringUtilities::toString<SUMOReal>(fd.qLKW)
						+ "] vLKW: [" + StringUtilities::toString<SUMOReal>(fd.vLKW)
						+ "] fLKW: [" + StringUtilities::toString<SUMOReal>(fd.fLKW)
						+ "]");
		}
	}
}


//................................................. Get the edge id at the origin of a route ...
const GAREdge const* GARUtils::getOriginEdge(const GARRouteCont const* pRouCont,
											 const string& routeName) {
	if (pRouCont == nullptr) {
		return nullptr;
	}

	std::shared_ptr<const GARRouteDesc> pRd = pRouCont->getRouteDesc(routeName);
	if (pRd == nullptr) {
		return nullptr;
	}
	const GAREdge const* pEdge = static_cast<const GAREdge*>(pRd->edges2Pass[0]);

	return pEdge;
}

} /* namespace gar */
