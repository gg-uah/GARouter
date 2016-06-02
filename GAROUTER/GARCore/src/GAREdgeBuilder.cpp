/****************************************************************************/
/// @file    GAREdgeBuilder.cpp
/// @author  Daniel Krajzewicz
/// @author  Eric Nicolay
/// @author  Michael Behrisch
/// @author  Yun-Pang Floetteroed
/// @date    Thu, 16.03.2006
/// @version $Id: GAREdgeBuilder.cpp 17235 2014-11-03 10:53:02Z behrisch $
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

// ===========================================================================
// included modules
// ===========================================================================
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

#include "GAREdgeBuilder.hpp"
#include "GAREdge.hpp"

#ifdef CHECK_MEMORY_LEAKS
#include <foreign/nvwa/debug_new.h>
#endif // CHECK_MEMORY_LEAKS


// ===========================================================================
// method definitions
// ===========================================================================
GAREdgeBuilder::GAREdgeBuilder() {
}

GAREdgeBuilder::~GAREdgeBuilder() {
}

ROEdge*
GAREdgeBuilder::buildEdge(const std::string& name, RONode* from, RONode* to,
		const int priority) {
	return new GAREdge(name, from, to, getNextIndex(), priority);
}

/****************************************************************************/

