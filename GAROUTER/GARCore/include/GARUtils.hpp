/*
 * GARUtils.h
 *
 *  Created on: Apr 25, 2015
 *      Author: ctomas
 */

#ifndef GARUTILS_HPP_
#define GARUTILS_HPP_

#include <router/ROEdge.h>
#include <GARTaz.hpp>
#include <GARTazGroup.hpp>
#include <GARRouteCont.hpp>
#include <GARRouteDesc.hpp>
#include <GARTrip.hpp>
#include <GARDetectorFlow.hpp>
#include <vector>

namespace gar {


/**
 * This class implements several utilities to be used only
 * by GAROUTER.
 */
class GARUtils {
public:
	static const std::string SEPARATOR;

	/**
	 * Deleted default constructor.
	 * Prevents to create an instance of this class.
	 */
	GARUtils() = delete;

	/**
	 * Deleted virtual destructor.
	 * Prevents to destroy an instance of this class.
	 */
	virtual ~GARUtils() = delete;

	/**
	 * Converts a vector of ROEdge pointers into a comma-separated string.
	 * It gets the edge identifier of every element in the vector and appends it
	 * to the comma-separated string to be returned.
	 * @param[in] edgeVector  A vector that contains pointers to edges.
	 * @return				  A comma separated string that reports the edge identifiers in the vector.
	 */
	static std::string toString(const std::vector<ROEdge*>& edgeVector);

	/**
	 * Converts a set of GAREdge pointers into a comma-separated string.
	 * It gets the edge identifier of every element in the set and appends it
	 * to the comma-separated string to be returned.
	 * @param[in] edgeSet A set that contains pointers to edges.
	 * @return		      A comma separated string that reports the edge identifiers in the set.
	 */
	static std::string toString(const gar::edge_set& edgeSet);

	/**
	 * Converts a set of GARTaz pointers into a comma-separated string.
	 * It gets the TAZ identifier of every element in the set and appends it
	 * to the comma-separated string to be returned.
	 * @param[in] tazSet  A set that contains pointers to TAZs.
	 * @return			  A comma separated string that reports the TAZ identifiers in the set.
	 */
	static std::string toString(const gar::taz_set& tazSet);

	/**
	 * Converts a set of GARTazGroup pointers into a comma-separated string.
	 * It gets the O/D group identifier of every element in the set and appends it
	 * to the comma-separated string to be returned.
	 * @param[in] grpSet  A set that contains pointers to O/D TAZ groups.
	 * @return			  A comma separated string that reports the O/D group identifiers in the set.
	 */
	static std::string toString(const gar::grp_set& grpSet);

	/**
	 * Convets a set of GARRouteDesc pointers into a comma-separated string.
	 * It gets the route name of every element in the set and appends it
	 * to the comma-separated string to be returned.
	 * @param[in] rouSet  A set than contains pointers to route descriptions.
	 * @return			  A comma separated string that reports the route names in the set.
	 *
	 */
	static std::string toString(const gar::route_set& rouSet);

	/**
	 * Logs a vector of trips. For each trip, the origin and destination edges and the
	 * route data set are logged.
	 * @param vTrips	A vector of trips.
	 * @param logger	A reference to the ga-router logger.
	 */
	static void logTrips(const gar::trip_vector& vTrips, CLogger* logger);

	/**
	 * Logs the detector flow data.
	 * @param flowData	The detector flow data.
	 * @param logger	A reference to the ga-router logger.
	 */
	static void logDetectorFlows(const GARDetectorFlows& flowData, CLogger* logger);

	/**
	 * @brief Get the edge at the origin of a route.
	 * @param pRouCont	The route descriptions container.
	 * @param routeName	The route identifier.
	 * @return			A pointer to the edge data at the origin of a route.
	 */
	static const GAREdge const* getOriginEdge(const GARRouteCont const* pRouCont,
											  const std::string& routeName);
};

} /* namespace gar */

#endif /* GARUTILS_H_ */
