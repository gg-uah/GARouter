/****************************************************************************/
/// @file    GAREdgeBuilder.h
/// @author  Daniel Krajzewicz
/// @author  Eric Nicolay
/// @author  Michael Behrisch
/// @author  Yun-Pang Floetteroed
/// @date    Thu, 16.03.2006
/// @version $Id: GAREdgeBuilder.h 17235 2014-11-03 10:53:02Z behrisch $
///
// Interface for building instances of dfrouter-edges
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
#ifndef GAREdgeBuilder_h
#define GAREdgeBuilder_h

// ===========================================================================
// included modules
// ===========================================================================
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

#include <router/ROAbstractEdgeBuilder.h>

// ===========================================================================
// class declarations
// ===========================================================================
class ROEdge;

// ===========================================================================
// class definitions
// ===========================================================================
/**
 * @class GAREdgeBuilder
 * @brief Interface for building instances of dfrouter-edges
 *
 * This ROAbstractEdgeBuilder implementation builds edges for the duarouter
 *  (instances of GAREdge).
 *
 * @see GAREdge
 */

class GAREdgeBuilder: public ROAbstractEdgeBuilder {
public:
	/// @brief Constructor
	GAREdgeBuilder();

	/// @brief Destructor
	~GAREdgeBuilder();

	/// @name Methods to be implemented, inherited from ROAbstractEdgeBuilder
	/// @{

	/** @brief Builds an edge with the given name
	 *
	 * This implementation builds a GAREdge.
	 *
	 * @param[in] name The name of the edge
	 * @param[in] from The node the edge begins at
	 * @param[in] to The node the edge ends at
	 * @param[in] priority The edge priority (road class)
	 * @return A proper instance of the named edge
	 * @see GAREdge
	 */
	ROEdge* buildEdge(const std::string& name, RONode* from, RONode* to,
			const int priority);
	/// @}

};

#endif

/****************************************************************************/

