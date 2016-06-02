/****************************************************************************/
/// @file    GARRouteDesc.h
/// @author  Daniel Krajzewicz
/// @author  Michael Behrisch
/// @date    Thu, 16.03.2006
/// @version $Id: GARRouteDesc.h 17235 2014-11-03 10:53:02Z behrisch $
///
// A route within the DFROUTER
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.dlr.de/
// Copyright (C) 2001-2014 DLR (http://www.dlr.de/) and contributors
/****************************************************************************/
//
//   This file is part of SUMO.
//   SUMO is free software: you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation, either version 3 of the License, or
//   (at your option) any later version.
//
/****************************************************************************/
#ifndef GARRouteDesc_h
#define GARRouteDesc_h

// ===========================================================================
// included modules
// ===========================================================================
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

#include <vector>
#include <set>
#include <string>
#include <utils/common/SUMOTime.h>

// ===========================================================================
// class declarations
// ===========================================================================
class ROEdge;
class GARDetector;

// ===========================================================================
// struct definitions
// ===========================================================================
/**
 * @struct GARRouteDesc
 * @brief A route within the DFROUTER
 */
struct GARRouteDesc {
	std::vector<ROEdge*> edges2Pass; 	/// @brief The edges the route is made of
	std::string routename;				/// @brief The name of the route
	SUMOReal duration_2;				/// @brief The route duration
	SUMOReal distance;					/// @brief The route distance
	int passedNo;
	const ROEdge* endDetectorEdge;
	const ROEdge* lastDetectorEdge;
	SUMOReal distance2Last;
	SUMOTime duration2Last;
	SUMOReal overallProb;
	SUMOReal factor;

	bool operator==(const GARRouteDesc& rhs) const {
		return this->routename == rhs.routename;
	}

	bool operator==(const std::string& routeName) const {
		return this->routename == routeName;
	}
};


namespace gar {

/**
 * Sorting criterion to keep the route set ordered by the duration, number of edges and route name.
 * It also prevents from allocating two routes with the same duration, number of edges and name.
 */
struct lex_compare_rou {
    bool operator() (const std::shared_ptr<const GARRouteDesc>& lhs,
    				 const std::shared_ptr<const GARRouteDesc>& rhs) const {
    	if (lhs->duration2Last < rhs->duration2Last) {
    		return true;
    	} else if (lhs->duration2Last > rhs->duration2Last) {
    		return false;
    	} else if ((lhs->edges2Pass).size() < (rhs->edges2Pass).size()) {
    		return true;
    	} else if ((lhs->edges2Pass).size() > (rhs->edges2Pass).size()) {
    		return false;
    	} else if (lhs->routename < rhs->routename) {
    		return true;
    	} else {
    		return false;
    	}
    }
};

/**
 * A data set of constant GARRouteDesc pointers.
 * The routes in the set are sorted regarding the 'lex_compare_rou' criterion.
 */
typedef std::set<std::shared_ptr<const GARRouteDesc>,lex_compare_rou> route_set;

/**
 * A data vector of constant GARRouteDesc pointers.
 */
typedef std::vector<std::shared_ptr<const GARRouteDesc>> route_vector;

} /* namespace gar */

#endif

/****************************************************************************/

