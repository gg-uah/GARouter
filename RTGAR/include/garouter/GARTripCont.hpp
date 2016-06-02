/*
 * GARTripCont.hpp
 *
 *  Created on: May 2, 2015
 *      Author: ctomas
 */

#ifndef GARTRIPCONT_HPP_
#define GARTRIPCONT_HPP_

#include <GARTrip.hpp>
#include <GAREdge.hpp>
#include <CLogger.hpp>
#include <set>

using common::CLogger;

namespace gar {

enum class RouteMode : short { All, Selected };

class GARTripCont {
public:
	/**
	 * Deleted default constructor.
	 * Prevents from creating an empty instance of this class.
	 */
	GARTripCont(void) = delete;

	/**
	 * Deleted copy constructor.
	 * Prevents from creating an instance of this class as a copy of other class.
	 * @param other	 The instance to be copied.
	 */
	GARTripCont(const GARTripCont& other) = delete;

	/**
	 * Parameterized constructor.
	 * @param rouMode	Define the route set to be considered from an origin to a
	 * 					destination edge of a trip.
	 * 					If <code>RouteSet::All</code>, all the routes from the origin
	 * 					to the destination edge of a trip are considered.
	 * 					If <code>RouteSet::Selected</code>, only the shortest routes
	 * 					and a random small part of the longest routes are considered.
	 * @param logger  	Reference to the ga-router logger.
	 */
	GARTripCont(const RouteMode& rouMode, CLogger* logger);

	/**
	 * Default virtual constructor.
	 */
	virtual ~GARTripCont(void) = default;

	/**
	 * Get the route data set mode.
	 * @return	<code>RouteSet::All</code>
	 */
	RouteMode getRouteMode(void) const;

	/**
	 * Gets the trip data set between the origin source and destination sink edges.
	 * @return	The origin and destination trip data set.
	 */
	const gar::trip_set& getOdTrips(void) const;

	/**
	 * Sets the trip data set from the origin source to the destination sink edges.
	 * @param odTrips	The trip data set from the origin to destination edges.
	 */
	void setOdTrips(const gar::trip_set& odTrips);

	/**
	 * Gets the trip between the specified origin and destination edges.
	 * @param pOrig	 A pointer to the origin edge.
	 * @param pDest	 A pointer to the destination edge.
	 * @return	     A pointer to the trip in the data set between the specified origin and destination edges.
	 */
	gar::trip_set::iterator getTrip(const GAREdge const* pOrig,
								    const GAREdge const* pDest) const;

	/**
	 * Adds a route to the trip data set.
	 * @param pOrig	 The origin edge.
	 * @param pDest	 The destination edge.
	 * @param rd	 The route description.
	 */
	void addRouteDesc(const GAREdge const* pOrig,
					  const GAREdge const* pDest,
					  const GARRouteDesc& rd);

	/**
	 * Builds a trip data set each of them containing only the selected routes to be considered
	 * in the genetic algorithm.
	 * @param shortRate	The selected shortest routes rate.
	 * @param longRate	The selected longest routes rate.
	 * @return
	 */
	gar::trip_set buildTripsWithGASelectedRoutes(const float& shortRate, const float& longRate) const;

private:
	//! The routes to be included in every trip [All|Selected].
	RouteMode rouMode;

	//! The origin-destination trip data set.
	gar::trip_set odTrips;

	//! Reference to the ga-router logger.
	CLogger* logger;

};

} /* namespace gar */

#endif /* GARTRIPCONT_HPP_ */
