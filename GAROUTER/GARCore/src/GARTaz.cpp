/*
s * GARTaz.cpp
 *
 *  Created on: Apr 26, 2015
 *      Author: ctomas
 */

#include <GARTaz.hpp>
#include <GARUtils.hpp>

using std::string;

namespace gar {

//................................................. Entirely parameterized constructor ...
GARTaz::GARTaz(const string& id,
			   const edge_set& sources,
			   const edge_set& sinks,
			   const edge_set& undefs)
: id (id),
  sources (sources),
  sinks (sinks),
  undefs (undefs){
	// Intentionally left empty
}


//................................................. Partially parameterized constructor ...
GARTaz::GARTaz(const string& id)
: GARTaz(id,
		 edge_set(),
		 edge_set(),
		 edge_set()) {
	// Intentionally left empty
}


//................................................. Get the TAZ identifier ...
std::string GARTaz::getId(void) const {
	return this->id;
}


//................................................. Get the TAZ sources ...
const gar::edge_set& GARTaz::getSources(void) const {
	return this->sources;
}


//................................................. Get the TAZ sinks ...
const gar::edge_set& GARTaz::getSinks(void) const {
	return this->sinks;
}


//................................................. Get the TAZ undefined edges ...
const gar::edge_set& GARTaz::getUndefs(void) const {
	return this->undefs;
}


//................................................. Get all the edges ...
const gar::edge_set GARTaz::getEdges(void) const {
	edge_set allEdges;

	// Append source edges
	allEdges.insert(sources.begin(), sources.end());

	// Append sink edges
	allEdges.insert(sinks.begin(), sinks.end());

	// Append undefined edges
	allEdges.insert(undefs.begin(), undefs.end());

	return allEdges;
}

//................................................. Checks whether the TAZ has a source edge  ...
bool GARTaz::hasSourceEdge(const string& id) const {
	auto edgeIdPredicate = [&id] (const GAREdge const* pEdge) { return pEdge->getID() == id; };

	auto it = std::find_if(sources.begin(), sources.end(), edgeIdPredicate);

	if (it == sources.end()) {
		return false;
	}

	return true;
}


//................................................. Checks whether the TAZ has a sink edge  ...
bool GARTaz::hasSinkEdge(const string& id) const {
	auto edgeIdPredicate = [&id] (const GAREdge const* pEdge) { return pEdge->getID() == id; };

	auto it = std::find_if(sinks.begin(), sinks.end(), edgeIdPredicate);

	if (it == sinks.end()) {
		return false;
	}

	return true;
}


//................................................. Checks whether the TAZ has an undefined edge  ...
bool GARTaz::hasUndefEdge(const string& id) const {
	auto edgeIdPredicate = [&id] (const GAREdge const* pEdge) { return pEdge->getID() == id; };

	auto it = std::find_if(undefs.begin(), undefs.end(), edgeIdPredicate);

	if (it == undefs.end()) {
		return false;
	}

	return true;
}


//................................................. Checks whether the TAZ has an edge  ...
bool GARTaz::hasEdge(const std::string& id) const {
	// Search in the sources
	if (hasSourceEdge(id)) {
		return true;
	}

	// Search in the sinks
	if (hasSinkEdge(id)) {
		return true;
	}

	// Search in the undefs
	if (hasUndefEdge(id)) {
		return true;
	}

	return false;
}


//................................................. Gets the stringfied TAZ data  ...
string GARTaz::toString(void) const {
	return "id: [" + id
			+ "], sources: [" + GARUtils::toString(sources)
			+ "], sinks: [" + GARUtils::toString(sinks)
			+ "], undefs: [" + GARUtils::toString(undefs) + "]";
}


//................................................. 'Is equal to' operator overloading ...
bool GARTaz::operator==(const GARTaz& rhs) const {
	return this->id == rhs.id;
}

} /* namespace gar */
