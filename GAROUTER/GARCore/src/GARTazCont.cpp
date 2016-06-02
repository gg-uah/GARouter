/*
 * GARTazCont.cpp
 *
 *  Created on: Apr 26, 2015
 *      Author: ctomas
 */

#include "GARTazCont.hpp"
#include <algorithm>

using std::set;
using std::string;
using std::shared_ptr;

namespace gar {

//................................................. Parameterized constructor ...
GARTazCont::GARTazCont(CLogger* logger)
: logger (logger) {
	// Intentionally left empty
}


//................................................. Gets the O/D group data set ...
const grp_set& GARTazCont::getOdGroups(void) const {
	return this->odGroups;
}


//................................................. Gets the source edges within a district ...
const gar::edge_set& GARTazCont::getSourceEdges(const std::string& taz) const {
	return this->getTaz(taz)->getSources();
}


//................................................. Gets the sink edges within a district ...
const gar::edge_set& GARTazCont::getSinkEdges(const std::string& taz) const {
	return this->getTaz(taz)->getSinks();
}


//................................................. Gets the undefined edges within a district ...
const gar::edge_set& GARTazCont::getUndefEdges(const std::string& taz) const {
	return this->getTaz(taz)->getUndefs();
}


//................................................. Gets the TAZ data set ...
const taz_set& GARTazCont::getTazs(void) const {
	return this->tazs;
}


//................................................. Gets a TAZ from the container by specifying its id ...
shared_ptr<const GARTaz> GARTazCont::getTaz(const string& id) const {
	auto findById = [&id] (const shared_ptr<const GARTaz>& pTaz) { return pTaz->getId() == id; };

	taz_set::iterator it = std::find_if(tazs.begin(), tazs.end(), findById);

	if (it == tazs.end()) {
		return nullptr;
	}
	return *it;
}


//................................................. Gets an O/D group from the container by specifying its id ...
shared_ptr<const GARTazGroup> GARTazCont::getOdGroup(const string& id) const {
	auto findById = [&id] (const shared_ptr<const GARTazGroup>& pGrp) { return pGrp->getId() == id; };

	grp_set::iterator it = std::find_if(odGroups.begin(), odGroups.end(), findById);

	if (it == odGroups.end()) {
		return nullptr;
	}
	return *it;
}


//................................................. Sets the TAZ data set ...
void GARTazCont::setTazs(const taz_set& tazs) {
	this->tazs = tazs;
}


//................................................. Sets the O/D group data set ...
void GARTazCont::setOdGroups(const grp_set& odgroups) {
	this->odGroups = odgroups;
}


//................................................. Inserts a TAZ into data set ...
taz_set::iterator GARTazCont::insertTaz(const shared_ptr<const GARTaz>& pTaz) {
	std::pair<taz_set::iterator,bool> result;

	logger->info("Adding TAZ " + pTaz->toString() + " to the TAZ container");
	result = tazs.insert(pTaz);
	if (result.second == false) {
		logger->error("Fail to insert TAZ " + pTaz->toString());
		return tazs.end();
	}

	return result.first;
}


//................................................. Inserts a TAZ group into data set ...
grp_set::iterator GARTazCont::insertOdGroup(const shared_ptr<const GARTazGroup>& pGroup) {
	std::pair<grp_set::iterator,bool> result;

	logger->info("Adding O/D group " + pGroup->toString() + " to the OD-group container");
	result = odGroups.insert(pGroup);
	if (result.second == false) {
		logger->error("Fail to insert O/D group " + pGroup->toString());
		return odGroups.end();
	}

	return result.first;
}


//................................................. Finds the TAZ that a source edge belongs to ...
shared_ptr<const GARTaz> GARTazCont::findSourceEdgeTaz(const string& id) const {
	auto sourcePredicate = [&id] (shared_ptr<const GARTaz> pTaz) { return pTaz->hasSourceEdge(id); };
	taz_set::iterator it = std::find_if(tazs.begin(), tazs.end(), sourcePredicate);

	if (it == tazs.end()) {
		return nullptr;
	}

	return *it;
}


//................................................. Finds the TAZ that a sink edge belongs to ...
shared_ptr<const GARTaz> GARTazCont::findSinkEdgeTaz(const string& id) const {
	auto sinkPredicate   = [&id] (shared_ptr<const GARTaz> pTaz) { return pTaz->hasSinkEdge(id); };
	taz_set::iterator it = std::find_if(tazs.begin(), tazs.end(), sinkPredicate);

	if (it == tazs.end()) {
		return nullptr;
	}

	return *it;
}


//................................................. Finds the O/D TAZ group that a source edge belongs to ...
shared_ptr<const GARTazGroup> GARTazCont::findSourceEdgeGroup(const string& id) const {
	auto sourcePredicate   = [&id] (const shared_ptr<const GARTazGroup>& pGroup) { return pGroup->hasSourceEdge(id); };
	grp_set::iterator it = std::find_if(odGroups.begin(), odGroups.end(), sourcePredicate);

	if (it == odGroups.end()) {
		return nullptr;
	}

	return *it;
}


//................................................. Finds the O/D TAZ group that a source edge belongs to ...
shared_ptr<const GARTazGroup> GARTazCont::findSinkEdgeGroup(const string& id) const {
	auto sinkPredicate   = [&id] (const shared_ptr<const GARTazGroup>& pGroup) { return pGroup->hasSinkEdge(id); };
	grp_set::iterator it = std::find_if(odGroups.begin(), odGroups.end(), sinkPredicate);

	if (it == odGroups.end()) {
		return nullptr;
	}

	return *it;
}

} /* namespace gar */
