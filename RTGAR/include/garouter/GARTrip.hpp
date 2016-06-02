/*
 * GARTrip.hpp
 *
 *  Created on: May 2, 2015
 *      Author: ctomas
 */

#ifndef GARTRIP_HPP_
#define GARTRIP_HPP_

#include <GARRouteDesc.hpp>
#include <GAREdge.hpp>
#include <CLogger.hpp>

using common::CLogger;

namespace gar {

/**
 * This class implements a trip from an origin to a destination edge.
 * It contains a set of the feasible routes that can be taken by a
 * user to perform the trip.
 */
class GARTrip {
public:
	/**
	 * Deleted default constructor.
	 * Prevents from creating an empty instance of this class.
	 */
	GARTrip() = delete;

	/**
	 * Deleted copy constructor.
	 * Prevents from creating a copy instance of this class.
	 * @param other	 The other instance of this class to be copied.
	 */
	GARTrip(const GARTrip& other) = default;

	/**
	 * Entirely parameterized constructor.
	 * @param pOrig	 	A pointer to the origin edge.
	 * @param pDest	 	A pointer to the destination edge.
	 * @param odRoutes	A set of routes between the origin and destination edges.
	 * @param logger  Reference to the ga-router logger.
	 */
	GARTrip(const GAREdge const* pOrig,
			const GAREdge const* pDest,
			const gar::route_set odRoutes,
			CLogger* logger);

	/**
	 * Partially parameterized constructor.
	 * @param pOrig	 A pointer to the origin edge.
	 * @param pDest	 A pointer to the destination edge.
	 * @param logger  Reference to the ga-router logger.
	 */
	GARTrip(const GAREdge const* pOrig,
			const GAREdge const* pDest,
			CLogger* logger);

	/**
	 * Default virtual destructor.
	 */
	virtual ~GARTrip() = default;

	/**
	 * Gets the origin edge pointer.
	 * @return	The origin edge pointer.
	 */
	const GAREdge const* getpOrig(void) const;

	/**
	 * Gets the destination edge pointer.
	 * @return	The destination edge pointer.
	 */
	const GAREdge const* getpDest(void) const;

	/**
	 * Gets the route data set between the origin and the destination.
	 * @return	The route data set between the origin and destination.
	 */
	const gar::route_set& getOdRoutes(void) const;

	/**
	 * Gets the route data set that defines a chromosome allele of the genetic algorithm.
	 * @param shortRate	Sets up the selection rate of the short routes in the allele.
	 * @param longRate	Sets up the selection rate of the long routes in the allele.
	 * @return			A route data set containing the set of routes being part of a chromosome allele
	 * 					of the genetic algorithm.
	 */
	gar::route_set getSelectedRoutes(const float& shortRate, const float& longRate) const;

	/**
	 * Adds a route to the route data set.
	 * @param rdd	A route description between the origin
	 * 				and destination edges.
	 */
	void addRoute(const GARRouteDesc& rd);

	/**
	 * Adds a route pointer to the route data set.
	 * @param pRd	A pointer to the route description between the origin
	 * 				and destination edges.
	 */
	void addRoute(const std::shared_ptr<const GARRouteDesc>& pRd);

	/**
	 * Gets the stringfied trip data.
	 * @return	A string containing the trip data.
	 */
	std::string toString(void) const;

protected:
	/**
	 * Get the short routes being part of the chromosome allele.
	 * @param numShort	The number of short routes to be selected.
	 * @return			A vector that comprises the short routes being part of the
	 * 					chromosome allele.
	 */
	gar::route_vector getShortRoutes(const size_t& numShort) const;

	/**
	 * Get the long routes being part of the chormosome allele.
	 * @param numLong	The number of long routes to be selected.
	 * @param numShort	The number of short routes.
	 * @return			A vector that comprises the long routes being part of the
	 * 					chromosome allele.
	 */
	gar::route_vector getLongRoutes(const size_t& numLong, const size_t& numShort) const;

private:
	//! The trip origin edge pointer.
	const GAREdge const* pOrig;

	//! The trip destination edge pointer.
	const GAREdge const* pDest;

	//! All the routes from the origin to the destination edge.
	gar::route_set odRoutes;

	//! Reference to the ga-router logger.
	CLogger* logger;
};


/**
 * Sorting criterion to keep the trip set ordered by the origin and destination edges.
 * It also prevents from allocating two trips with the same origin and destination edges.
 */
struct lex_compare_trip {
    bool operator() (const std::shared_ptr<const GARTrip>& lhs,
    				 const std::shared_ptr<const GARTrip>& rhs) const {
    	if (lhs->getpOrig() < rhs->getpOrig()) {
    		return true;
    	} else if (lhs->getpOrig() > rhs->getpOrig()) {
    		return false;
    	} else if (lhs->getpDest() < rhs->getpDest()) {
    		return true;
    	} else {
    		return false;
    	}
    }
};

/**
 * A data set of constant GARTrip pointers.
 * The trips in the set are sorted regarding the 'lex_compare_trip' criterion.
 */
typedef std::set<std::shared_ptr<const GARTrip>,lex_compare_trip> trip_set;

/**
 * A data vector of constant GARTrip pointers.
 */
typedef std::vector<std::shared_ptr<const GARTrip>> trip_vector;

} /* namespace gar */

#endif /* GARTRIP_HPP_ */
