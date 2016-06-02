/****************************************************************************/
/// @file    GARRouteCont.cpp
/// @author  Daniel Krajzewicz
/// @author  Laura Bieker
/// @author  Michael Behrisch
/// @date    Thu, 16.03.2006
/// @version $Id: GARRouteCont.cpp 17235 2014-11-03 10:53:02Z behrisch $
///
// A container for routes
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

// ===========================================================================
// included modules
// ===========================================================================
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

#include <fstream>
#include <cassert>
#include "GARRouteDesc.hpp"
#include "GARRouteCont.hpp"
#include "GARNet.hpp"
#include <router/ROEdge.h>
#include <utils/common/ToString.h>
#include <utils/iodevices/OutputDevice.h>

#ifdef CHECK_MEMORY_LEAKS
#include <foreign/nvwa/debug_new.h>
#endif // CHECK_MEMORY_LEAKS


// ===========================================================================
// method definitions
// ===========================================================================
GARRouteCont::GARRouteCont() {
	myRoutes = std::vector<GARRouteDesc>();
	myConnectionOccurences = std::map<std::pair<ROEdge*, ROEdge*>, int>();

}

GARRouteCont::~GARRouteCont() {
}

std::vector<GARRouteDesc>::iterator GARRouteCont::addRouteDesc(GARRouteDesc& desc) {
	std::vector<GARRouteDesc>::iterator it;

	it = find_if(myRoutes.begin(), myRoutes.end(), route_finder(desc));

	// routes may be duplicate as in-between routes may have different starting points
	if (it == myRoutes.end()) {
		// compute route id
		this->setID(desc);
		it = myRoutes.insert(myRoutes.end(), desc);
	} else {
		GARRouteDesc& prev = *it;
		prev.overallProb += desc.overallProb;
	}

	return it;
}

bool GARRouteCont::removeRouteDesc(GARRouteDesc& desc) {
	std::vector<GARRouteDesc>::const_iterator j = find_if(myRoutes.begin(),
			myRoutes.end(), route_finder(desc));
	if (j == myRoutes.end()) {
		return false;
	}
	return true;
}

bool GARRouteCont::save(std::vector<std::string>& saved,
		const std::string& prependix, OutputDevice& out) {
	bool haveSavedOneAtLeast = false;
	for (std::vector<GARRouteDesc>::const_iterator j = myRoutes.begin();
			j != myRoutes.end(); ++j) {
		const GARRouteDesc& desc = (*j);
		if (find(saved.begin(), saved.end(), desc.routename) != saved.end()) {
			continue;
		}
		saved.push_back((*j).routename);
		assert(desc.edges2Pass.size() >= 1);
		out.openTag(SUMO_TAG_ROUTE).writeAttr(SUMO_ATTR_ID,
				prependix + desc.routename);
		out << " edges=\"";
		for (std::vector<ROEdge*>::const_iterator k = desc.edges2Pass.begin();
				k != desc.edges2Pass.end(); k++) {
			if (k != desc.edges2Pass.begin()) {
				out << ' ';
			}
			out << (*k)->getID();
		}
		out << '"';
		out.closeTag();
		haveSavedOneAtLeast = true;
	}
	return haveSavedOneAtLeast;
}

void GARRouteCont::sortByDistance() {
	sort(myRoutes.begin(), myRoutes.end(), by_distance_sorter());
}

void GARRouteCont::removeIllegal(
		const std::vector<std::vector<ROEdge*> >& illegals) {
	for (std::vector<GARRouteDesc>::iterator i = myRoutes.begin();
			i != myRoutes.end();) {
		GARRouteDesc& desc = *i;
		bool remove = false;
		for (std::vector<std::vector<ROEdge*> >::const_iterator j =
				illegals.begin(); !remove && j != illegals.end(); ++j) {
			int noFound = 0;
			for (std::vector<ROEdge*>::const_iterator k = (*j).begin();
					!remove && k != (*j).end(); ++k) {
				if (find(desc.edges2Pass.begin(), desc.edges2Pass.end(), *k)
						!= desc.edges2Pass.end()) {
					noFound++;
					if (noFound > 1) {
						remove = true;
					}
				}
			}
		}
		if (remove) {
			i = myRoutes.erase(i);
		} else {
			++i;
		}
	}
}


std::shared_ptr<const GARRouteDesc> GARRouteCont::getRouteDesc(const std::string& routeName) const {
	auto searchByRouteName = [&routeName] (const GARRouteDesc& rd) { return rd.routename == routeName; };

	std::vector<GARRouteDesc>::const_iterator it = std::find_if(myRoutes.begin(), myRoutes.end(), searchByRouteName);
	if (it == myRoutes.end()) {
		return nullptr;
	}

	return std::make_shared<const GARRouteDesc>(*it);
}


void GARRouteCont::setID(GARRouteDesc& desc) const {
	std::pair<ROEdge*, ROEdge*> c(desc.edges2Pass[0], desc.edges2Pass.back());
	desc.routename = c.first->getID() + "_to_" + c.second->getID();
	if (myConnectionOccurences.find(c) == myConnectionOccurences.end()) {
		myConnectionOccurences[c] = 0;
	} else {
		myConnectionOccurences[c] = myConnectionOccurences[c] + 1;
		desc.routename = desc.routename + "_"
				+ toString(myConnectionOccurences[c]);
	}
}

/****************************************************************************/

