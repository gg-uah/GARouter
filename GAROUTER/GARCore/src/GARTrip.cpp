/*
 * GARTrip.cpp
 *
 *  Created on: May 2, 2015
 *      Author: ctomas
 */

#include <GARTrip.hpp>
#include <GARUtils.hpp>
#include <StringUtilities.hpp>
#include <random>

using std::shared_ptr;
using std::string;
using common::StringUtilities;

namespace gar {

//................................................. Entirely parameterized constructor ...
GARTrip::GARTrip(const GAREdge const* pOrig,
				 const GAREdge const* pDest,
				 const gar::route_set odRoutes,
				 CLogger* logger)
: pOrig (pOrig),
  pDest (pDest),
  odRoutes (odRoutes),
  logger (logger) {
	// Intentionally left empty
}


//................................................. Partially parameterized constructor ...
GARTrip::GARTrip(const GAREdge const* pOrig,
				 const GAREdge const* pDest,
				 CLogger* logger)
: GARTrip(pOrig,
		  pDest,
		  gar::route_set(),
		  logger) {
	// Intentionally left empty
}


//................................................. Gets the origin edge pointer ...
const GAREdge const* GARTrip::getpOrig(void) const {
	return this->pOrig;
}


//................................................. Gets the destination edge pointer ...
const GAREdge const* GARTrip::getpDest(void) const {
	return this->pDest;
}


//................................................. Gets the O/D route data set ...
const route_set& GARTrip::getOdRoutes(void) const {
	return this->odRoutes;
}


//................................................. Gets the route data set being part of a chromosome allele ...
route_set GARTrip::getSelectedRoutes(const float& shortRate, const float& longRate) const {
	try {
		route_vector routes;
		size_t numShort = int(odRoutes.size() * shortRate);
		size_t numLong  = int(odRoutes.size() * longRate);

		// Get the short routes
		logger->debug("\tInsert [" + StringUtilities::toString(int(numShort)) + "] short routes");
		routes = this->getShortRoutes(numShort);

		// Insert the long routes
		logger->debug("\tInsert [" + StringUtilities::toString(int(numLong)) + "] long routes");
		route_vector longRoutes = this->getLongRoutes(numLong, numShort);
		std::copy(longRoutes.begin(), longRoutes.end(), std::back_inserter(routes));

		return route_set(routes.begin(), routes.end());
	} catch (const std::exception& ex) {
		logger->error("Fail to select the routes being part of the alleles from the trip from [" + pOrig->getID() + "] to [" + pDest->getID() + "]: " + string(ex.what()));
		return route_set();
	}
}


//................................................. Adds a route to the route data set ...
void GARTrip::addRoute(const GARRouteDesc& rd) {
	std::shared_ptr<const GARRouteDesc> pRd = std::make_shared<const GARRouteDesc>(rd);
	this->addRoute(pRd);
}


//................................................. Adds a route pointer to the route data set ...
void GARTrip::addRoute(const std::shared_ptr<const GARRouteDesc>& pRd) {
	if (pRd != nullptr) {
		odRoutes.insert(pRd);
	}
}

//................................................. Gets the stringfied trip data ...
string GARTrip::toString(void) const {
	return "origin: [" + pOrig->getID()
			+ "], dest: [" + pDest->getID()
			+ "], routes: [" + GARUtils::toString(odRoutes) + "]";
}


//................................................. Gets the short routes being part of a chromosome allele ...
gar::route_vector GARTrip::getShortRoutes(const size_t& numShort) const {
	try {
		//The route data set is ordered by duration
		route_vector rv(odRoutes.begin(), odRoutes.end());

		//Return the first shortest routes in the data set
		rv.resize(numShort);
		return rv;
	} catch (const std::exception& ex) {
		logger->error("Fail to get the short routes from the trip from [" + pOrig->getID() + "] to [" + pDest->getID() + "]: " + string(ex.what()));
		return route_vector();
	}
}


//................................................. Gets the long routes being part of a chromosome allele ...
gar::route_vector GARTrip::getLongRoutes(const size_t& numLong, const size_t& numShort) const {
	try {
		route_set rs;
		route_vector vecRoutes (odRoutes.begin(), odRoutes.end());
		int longSize = vecRoutes.size() - numShort;

		double mean = double(numShort);
		double stdev = double(longSize * 0.1);

		// Create a random uniform distribution to get long routes randomly
		std::default_random_engine generator;
	    std::normal_distribution<double> distribution(mean, stdev);
		//std::uniform_int_distribution<int> distribution(numShort, odRoutes.size()-1);

		for (int i = 0; i < numLong;) {
			int num = int(distribution(generator));  //Generates a number in the range numShort...odRoutes.size()-1
			if (num < numShort  ||  num > vecRoutes.size()-1) {
				continue;
			}
			auto res = rs.insert(vecRoutes.at(num));
			// In case of successful insertion, select another route
			if (res.second == true) {
				i++;
			}
		}
		return route_vector(rs.begin(), rs.end());
	} catch (const std::exception& ex) {
		logger->error("Fail to get the long routes from the trip from [" + pOrig->getID() + "] to [" + pDest->getID() + "]: " + string(ex.what()));
		return route_vector();
	}
}

} /* namespace gar */
