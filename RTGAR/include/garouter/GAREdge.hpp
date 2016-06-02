/****************************************************************************/
/// @file    GAREdge.h
/// @author  Daniel Krajzewicz
/// @author  Michael Behrisch
/// @author  Yun-Pang Floetteroed
/// @date    Thu, 16.03.2006
/// @version $Id: GAREdge.h 17235 2014-11-03 10:53:02Z behrisch $
///
// An edge within the DFROUTER
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.dlr.de/
// Copyright (C) 2006-2014 DLR (http://www.dlr.de/) and contributors
/****************************************************************************/
//
//   This file is part of SUMO.
//   SUMO is free software: you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation, either version 3 of the License, or
//   (at your option) any later version.
//
/****************************************************************************/
#ifndef GAREdge_h
#define GAREdge_h

// ===========================================================================
// included modules
// ===========================================================================
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

#include <string>
#include <map>
#include <vector>
#include <router/ROEdge.h>
#include <utils/geom/Position.h>
#include "GARDetectorFlow.hpp"

// ===========================================================================
// class declarations
// ===========================================================================
class ROLane;

// ===========================================================================
// class definitions
// ===========================================================================
/**
 * @class GAREdge
 */

class GAREdge: public ROEdge {
public:
	/** @brief Constructor
	 *
	 * @param[in] id The id of the edge
	 * @param[in] from The node the edge begins at
	 * @param[in] to The node the edge ends at
	 * @param[in] index The numeric id of the edge
	 */
	GAREdge(const std::string& id, RONode* from, RONode* to,
			unsigned int index, const int priority);

	/// @brief Destructor
	~GAREdge();

	void setFlows(const std::vector<FlowDef>& flows);

	const std::vector<FlowDef>& getFlows() const;

	/**
	 * Overloads the 'less than' comparison operator.
	 * @param rhs	The right-hand side operand.
	 * @return		<code>true</code> if the left-hand side edge id is less
	 * 				than the right-hand side edge id, <code>false</code> otherwise.
	 */
	bool operator< (const GAREdge& rhs) const;

	/**
	 * Overloads the 'greater than' comparison operator.
	 * @param rhs	The right-hand side operand.
	 * @return		<code>true</code> if the left-hand side edge id is greater
	 * 				than the right-hand side edge id, <code>false</code> otherwise.
	 */
	bool operator> (const GAREdge& rhs) const;

	/**
	 * Overloads the 'is equal to' comparison operator.
	 * @param rhs	The right-hand side operator.
	 * @return		<code>true</code> if the left-hand side edge id is equal to
	 * 				the right-hand side edge id, <code>false</code> otherwise.
	 */
	bool operator== (const GAREdge& rhs) const;

private:
	std::vector<FlowDef> myFlows;

private:
	/// @brief Invalidated copy constructor
	GAREdge(const GAREdge& src);

	/// @brief Invalidated assignment operator
	GAREdge& operator=(const GAREdge& src);

};

namespace gar {

/**
 * Sorting criterion to keep the edge set ordered by the edge id.
 * It also prevents from allocating two edges with the same id.
 */
struct lex_compare_edg {
    bool operator() (const GAREdge const* lhs,
    				 const GAREdge const* rhs) const {
        return lhs->getID() < rhs->getID();
    }
};

/**
 * A set of constant GAREdge pointers to constant GAREdge data.
 * The edges in a set are sorted following the 'lex_compare_edg' criterion.
 */
typedef std::set<const GAREdge const*,lex_compare_edg> edge_set;

} /* namespace gar */

#endif

/****************************************************************************/

