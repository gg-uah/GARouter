/*
 * GARTripCont.cpp
 *
 *  Created on: May 2, 2015
 *      Author: ctomas
 */

#include <GARTripCont.hpp>
#include <StringUtilities.hpp>
#include <memory>

using std::string;
using std::shared_ptr;
using common::StringUtilities;

namespace gar {

//................................................. Parameterized constructor ...
GARTripCont::GARTripCont(const RouteMode& rouMode, CLogger* logger)
: rouMode (rouMode),
  odTrips (trip_set()),
  logger  (logger) {
	// Intentionally left empty
}


//................................................. Parameterized constructor ...
RouteMode GARTripCont::getRouteMode(void) const {
	return this->rouMode;
}


//................................................. Gets the O/D trips data set ...
const trip_set& GARTripCont::getOdTrips(void) const {
	return this->odTrips;
}


//................................................. Gets the O/D trips data set ...
void GARTripCont::setOdTrips(const gar::trip_set& odTrips) {
	this->odTrips = odTrips;
}


//................................................. Gets the trip between specified O/D ...
trip_set::iterator GARTripCont::getTrip(const GAREdge const* pOrig,
									    const GAREdge const* pDest) const {
	auto searchByOd = [&pOrig, &pDest] (const shared_ptr<const GARTrip>& pTrip) {
							return (*(pTrip->getpOrig()) == *pOrig  &&  *(pTrip->getpDest()) == *pDest); };

	auto it = std::find_if(odTrips.begin(), odTrips.end(), searchByOd);

	return it;
}


//................................................. Add a route to the trip container ...
void GARTripCont::addRouteDesc(const GAREdge const* pOrig,
				  	  	  	   const GAREdge const* pDest,
							   const GARRouteDesc& rd) {
	route_set routes;
	shared_ptr<const GARTrip> pTrip;

	// Search the trip between the origin and destination edges
	trip_set::iterator it = this->getTrip(pOrig, pDest);

	if (it != odTrips.end()) {
		// Get the found trip
		logger->debug("Found trip between origin [" + pOrig->getID() + "] and destination [" + pDest->getID() + "]");
		pTrip = (*it);

	} else {
		// Create a new trip
		logger->debug("Insert new trip between origin [" + pOrig->getID() + "] and destination [" + pDest->getID() + "]");
		pTrip = std::make_shared<const GARTrip>(GARTrip(pOrig, pDest, logger));
		odTrips.insert(pTrip);
	}

	// Add route to trip
	logger->debug("Add route [" + rd.routename + "] to the trip");
	const_cast<GARTrip&>(*pTrip).addRoute(rd);
}


//................................................. Add a route to the trip container ...
trip_set GARTripCont::buildTripsWithGASelectedRoutes(const float& shortRate, const float& longRate) const {
	try {
		trip_set tripSet;

		for (auto pTrip : odTrips) {
			// Select the routes being part of the allele set
			logger->debug("Select routes for trip from [" + pTrip->getpOrig()->getID() + "] to [" + pTrip->getpDest()->getID() + "]");
			route_set alRoutes = pTrip->getSelectedRoutes(shortRate, longRate);

			// Insert a new trip containing the selected routes
			shared_ptr<const GARTrip> pAlTrip = std::make_shared<const GARTrip>(GARTrip(pTrip->getpOrig(),
																						pTrip->getpDest(),
																						alRoutes,
																						logger));
			auto res = tripSet.insert(pAlTrip);
			if (res.second == false) {
				logger->warning("Fail to insert trip with allele routes from [" + pTrip->getpOrig()->getID() + "] to [" + pTrip->getpDest()->getID() + "]");
			}
		}

		return tripSet;
	} catch (const std::exception& ex) {
		logger->error("Fail to build the trip data set containing only the routes being part of the alleles: " + string(ex.what()));
		return trip_set();
	}
}

} /* namespace gar */
