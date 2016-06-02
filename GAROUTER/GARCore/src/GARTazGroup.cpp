/*
 * GARTazGroup.cpp
 *
 *  Created on: Apr 26, 2015
 *      Author: ctomas
 */

#include <GARTazGroup.hpp>
#include <GARUtils.hpp>

using std::string;
using std::shared_ptr;

namespace gar {

//................................................. Entirely parameterized construtor ...
GARTazGroup::GARTazGroup(const string& id,
		    			 const taz_set& odtazs)
: id (id),
  odtazs (odtazs) {
	// Intentionally left empty
}


//................................................. Partially parameterized constructor ...
GARTazGroup::GARTazGroup(const string& id)
: GARTazGroup(id, taz_set()) {
	// Intentionally left empty
}


//................................................. Gets the group identifier ...
const string& GARTazGroup::getId(void) const {
	return this->id;
}


//................................................. Gets the group TAZ data set ...
const taz_set& GARTazGroup::getTazs(void) const {
	return this->odtazs;
}


//................................................. Checks whether the O/D TAZ group has a source edge ...
bool GARTazGroup::hasSourceEdge(const string& id) const {
	auto hasSourceEdgeId = [&id] (const shared_ptr<const GARTaz>& pTaz) { return pTaz->hasSourceEdge(id); };

	auto it = std::find_if(odtazs.begin(), odtazs.end(), hasSourceEdgeId);

	if (it == odtazs.end()) {
		return false;
	}

	return true;
}


//................................................. Checks whether the O/D TAZ group has a sink edge ...
bool GARTazGroup::hasSinkEdge(const string& id) const {
	auto hasSinkEdgeId = [&id] (const shared_ptr<const GARTaz>& pTaz) { return pTaz->hasSinkEdge(id); };

	auto it = std::find_if(odtazs.begin(), odtazs.end(), hasSinkEdgeId);

	if (it == odtazs.end()) {
		return false;
	}

	return true;
}


//................................................. Gets the stringfied O/D group data ...
string GARTazGroup::toString(void) const {
	return "OD group id: [" + this->id
			+ "], tazs: [" + GARUtils::toString(this->odtazs) + "]";
}


//................................................. Overloads the 'is equal to' operator ...
bool GARTazGroup::operator==(const GARTazGroup& rhs) const {
	return this->id == rhs.id;
}


//................................................. Overloads the 'is less than' operator ...
bool GARTazGroup::operator<(const GARTazGroup& rhs) const {
	return this->id < rhs.id;
}


} /* namespace gar */
