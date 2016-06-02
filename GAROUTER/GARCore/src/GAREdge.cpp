/****************************************************************************/
/// @file    GAREdge.cpp
/// @author  Daniel Krajzewicz
/// @author  Michael Behrisch
/// @author  Jakob Erdmann
/// @author  Yun-Pang Floetteroed
/// @date    Thu, 16.03.2006
/// @version $Id: GAREdge.cpp 17235 2014-11-03 10:53:02Z behrisch $
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

// ===========================================================================
// included modules
// ===========================================================================
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

#include <algorithm>
#include <utils/common/MsgHandler.h>
#include "GAREdge.hpp"

#ifdef CHECK_MEMORY_LEAKS
#include <foreign/nvwa/debug_new.h>
#endif // CHECK_MEMORY_LEAKS

// ===========================================================================
// method definitions
// ===========================================================================
GAREdge::GAREdge(const std::string& id, RONode* from, RONode* to,
		unsigned int index, const int priority) :
		ROEdge(id, from, to, index, priority) {
}


GAREdge::~GAREdge() {
}


void GAREdge::setFlows(const std::vector<FlowDef>& flows) {
	myFlows = flows;
}


const std::vector<FlowDef>&
GAREdge::getFlows() const {
	return myFlows;
}


bool GAREdge::operator< (const GAREdge& rhs) const {
	return this->myID < rhs.getID();
}


bool GAREdge::operator> (const GAREdge& rhs) const {
	return this->myID > rhs.getID();
}


bool GAREdge::operator== (const GAREdge& rhs) const {
	return this->myID == rhs.getID();
}

/****************************************************************************/

