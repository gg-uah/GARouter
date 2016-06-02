/****************************************************************************/
/// @file    GARNet.cpp
/// @author  Daniel Krajzewicz
/// @author  Eric Nicolay
/// @author  Jakob Erdmann
/// @author  Michael Behrisch
/// @date    Thu, 16.03.2006
/// @version $Id: GARNet.cpp 17235 2014-11-03 10:53:02Z behrisch $
///
// A DFROUTER-network
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

#include <cassert>
#include <iostream>
#include <map>
#include <vector>
#include <iterator>
#include "GARNet.hpp"
#include "GARDetector.hpp"
#include "GARRouteDesc.hpp"
#include "GARDetectorFlow.hpp"
#include "GAREdge.hpp"
#include "GARTaz.hpp"
#include "GARTazGroup.hpp"
#include <cmath>
#include <utils/common/MsgHandler.h>
#include <utils/common/ToString.h>
#include <utils/common/UtilExceptions.h>
#include <utils/geom/GeomHelper.h>

#ifdef CHECK_MEMORY_LEAKS
#include <foreign/nvwa/debug_new.h>
#endif // CHECK_MEMORY_LEAKS

using common::CLogger;
using gar::GARTazCont;
using gar::GARTaz;
using gar::GARTazGroup;
using std::string;
using std::shared_ptr;


// ===========================================================================
// method definitions
// ===========================================================================
GARNet::GARNet(void)
: RONet(),
  myAmInHighwayMode(false),
  mySourceNumber(0),
  mySinkNumber(0),
  myInBetweenNumber(0),
  myInvalidNumber(0) {
  myDisallowedEdges = OptionsCont::getOptions().getStringVector("disallowed-edges");
  myKeepTurnarounds = OptionsCont::getOptions().getBool("keep-turnarounds");
}

GARNet::~GARNet(void) {
}

void GARNet::buildApproachList() {
	const std::map<std::string, ROEdge*>& edges = getEdgeMap();
	for (std::map<std::string, ROEdge*>::const_iterator rit = edges.begin();
			rit != edges.end(); ++rit) {
		ROEdge* ce = (*rit).second;
		unsigned int i = 0;
		unsigned int length_size = ce->getNumSuccessors();
		for (i = 0; i < length_size; i++) {
			ROEdge* help = ce->getSuccessor(i);
			if (find(myDisallowedEdges.begin(), myDisallowedEdges.end(),
					help->getID()) != myDisallowedEdges.end()) {
				// edges in sinks will not be used
				continue;
			}
			if (!myKeepTurnarounds && help->getToNode() == ce->getFromNode()) {
				// do not use turnarounds
				continue;
			}
			// add the connection help->ce to myApproachingEdges
			if (myApproachingEdges.find(help) == myApproachingEdges.end()) {
				myApproachingEdges[help] = std::vector<ROEdge*>();
			}
			myApproachingEdges[help].push_back(ce);

			// add the connection ce->help to myApproachedEdges
			if (myApproachedEdges.find(ce) == myApproachedEdges.end()) {
				myApproachedEdges[ce] = std::vector<ROEdge*>();
			}
			myApproachedEdges[ce].push_back(help);
		}
	}
}


void GARNet::buildNodeList(void) {
	// Insert the from and to nodes of every edge in the network
	for (auto edge : this->getEdgeMap()) {
		std::string edgeId = edge.first;
		const ROEdge const* pEdge = edge.second;

		if (pEdge->getType() == ROEdge::EdgeType::ET_INTERNAL) {
			continue;
		}

		myNodes.insert(pEdge->getFromNode());
		myNodes.insert(pEdge->getToNode());
	}
}


void GARNet::buildDetectorEdgeDependencies(GARDetectorCon& detcont) const {
	myDetectorsOnEdges.clear();
	myDetectorEdges.clear();
	const std::vector<GARDetector*>& dets = detcont.getDetectors();
	for (std::vector<GARDetector*>::const_iterator i = dets.begin();
			i != dets.end(); ++i) {
		ROEdge* e = getDetectorEdge(**i);
		myDetectorsOnEdges[e].push_back((*i)->getID());
		myDetectorEdges[(*i)->getID()] = e;
	}
}

void GARNet::computeTypes(GARDetectorCon& detcont, bool sourcesStrict) const {
	PROGRESS_BEGIN_MESSAGE("Computing detector types");
	const std::vector<GARDetector*>& dets = detcont.getDetectors();
	// build needed information. first
	buildDetectorEdgeDependencies(detcont);
	// compute detector types then
	for (std::vector<GARDetector*>::const_iterator i = dets.begin();
			i != dets.end(); ++i) {
		if (isSource(**i, detcont, sourcesStrict)) {
			(*i)->setType(SOURCE_DETECTOR);
			mySourceNumber++;
		}
		if (isDestination(**i, detcont)) {
			(*i)->setType(SINK_DETECTOR);
			mySinkNumber++;
		}
		if ((*i)->getType() == TYPE_NOT_DEFINED) {
			(*i)->setType(BETWEEN_DETECTOR);
			myInBetweenNumber++;
		}
	}
	// recheck sources
	for (std::vector<GARDetector*>::const_iterator i = dets.begin();
			i != dets.end(); ++i) {
		if ((*i)->getType() == SOURCE_DETECTOR && isFalseSource(**i, detcont)) {
			(*i)->setType(DISCARDED_DETECTOR);
			myInvalidNumber++;
			mySourceNumber--;
		}
	}
	// print results
	PROGRESS_DONE_MESSAGE();
	WRITE_MESSAGE("Computed detector types:");
	WRITE_MESSAGE(" " + toString(mySourceNumber) + " source detectors");
	WRITE_MESSAGE(" " + toString(mySinkNumber) + " sink detectors");
	WRITE_MESSAGE(" " + toString(myInBetweenNumber) + " in-between detectors");
	WRITE_MESSAGE(" " + toString(myInvalidNumber) + " invalid detectors");
}

bool GARNet::hasInBetweenDetectorsOnly(ROEdge* edge,
		const GARDetectorCon& detectors) const {
	assert(myDetectorsOnEdges.find(edge) != myDetectorsOnEdges.end());
	const std::vector<std::string>& detIDs =
			myDetectorsOnEdges.find(edge)->second;
	std::vector<std::string>::const_iterator i;
	for (i = detIDs.begin(); i != detIDs.end(); ++i) {
		const GARDetector& det = detectors.getDetector(*i);
		if (det.getType() != BETWEEN_DETECTOR) {
			return false;
		}
	}
	return true;
}

bool GARNet::hasSourceDetector(ROEdge* edge,
		const GARDetectorCon& detectors) const {
	assert(myDetectorsOnEdges.find(edge) != myDetectorsOnEdges.end());
	const std::vector<std::string>& detIDs =
			myDetectorsOnEdges.find(edge)->second;
	std::vector<std::string>::const_iterator i;
	for (i = detIDs.begin(); i != detIDs.end(); ++i) {
		const GARDetector& det = detectors.getDetector(*i);
		if (det.getType() == SOURCE_DETECTOR) {
			return true;
		}
	}
	return false;
}

void GARNet::computeRoutesFor(ROEdge* edge,
							  GARRouteDesc& base,
							  const GARDetector& det,
							  GARRouteCont& into,
							  GARTripCont& tripCont,
							  const GARDetectorCon& detCont,
							  const GARTazCont& tazCont,
							  int maxFollowingLength,
							  std::vector<ROEdge*>& seen,
							  CLogger* logger) const {
	std::vector<GARRouteDesc> unfoundEnds;
	std::priority_queue<GARRouteDesc, std::vector<GARRouteDesc>, DFRouteDescByTimeComperator> toSolve;
	std::map<ROEdge*, std::vector<ROEdge*>> dets2Follow;
	dets2Follow[edge] = std::vector<ROEdge*>();
	base.passedNo = 0;
	SUMOReal minDist = OptionsCont::getOptions().getFloat("min-route-length");
	toSolve.push(base);

	// Get the TAZ of the source edge
	shared_ptr<const GARTazGroup> pSourceGroup = tazCont.findSourceEdgeGroup(edge->getID());

	while (!toSolve.empty()) {
		GARRouteDesc current = toSolve.top();
		toSolve.pop();
		ROEdge* last = *(current.edges2Pass.end() - 1);
		if (hasDetector(last)) {
			if (dets2Follow.find(last) == dets2Follow.end()) {
				dets2Follow[last] = std::vector<ROEdge*>();
			}
			for (std::vector<ROEdge*>::reverse_iterator i =
					current.edges2Pass.rbegin() + 1;
					i != current.edges2Pass.rend(); ++i) {
				if (hasDetector(*i)) {
					dets2Follow[*i].push_back(last);
					break;
				}
			}
		}

		seen.push_back(last);

		// end if the edge has no further connections
		if (!hasApproached(last)) {
			// ok, no further connections to follow
			current.factor = 1.;
			SUMOReal cdist = current.edges2Pass[0]->getFromNode()->getPosition().
								distanceTo(current.edges2Pass.back()->getToNode()->getPosition());

			// Refuse sink edges in the same group than the source edge
			if (tazCont.findSinkEdgeGroup(last->getID()) == pSourceGroup) {
				//logger->info("Refuse route from source [" + edge->getID()
				//		+ "] to sink [" + last->getID()
				//		+ "] belonging to the same TAZ group [" + pEdgeGroup->getId() + "]");
				continue;
			}

			if (minDist < cdist) {
				std::vector<GARRouteDesc>::iterator it = into.addRouteDesc(current);
				logger->debug("\tAdd route [" + current.routename + "]");
				tripCont.addRouteDesc(dynamic_cast<GAREdge*>(edge),
									  dynamic_cast<GAREdge*>(last),
									  *it);
			}
			continue;
		}

		// check for passing detectors:
		//  if the current last edge is not the one the detector is placed on ...
		bool addNextNoFurther = false;
		if (last != getDetectorEdge(det)) {
			// ... if there is a detector ...
			if (hasDetector(last)) {
				if (!hasInBetweenDetectorsOnly(last, detCont)) {
					// ... and it's not an in-between-detector
					// -> let's add this edge and the following, but not any further
					addNextNoFurther = true;
					current.lastDetectorEdge = last;
					current.duration2Last = (SUMOTime) current.duration_2;
					current.distance2Last = current.distance;
					current.endDetectorEdge = last;
					if (hasSourceDetector(last, detCont)) {
///!!!                        //toDiscard.push_back(current);
					}
					current.factor = 1.;
					SUMOReal cdist =
							current.edges2Pass[0]->getFromNode()->getPosition().distanceTo(
									current.edges2Pass.back()->getToNode()->getPosition());
					if (minDist < cdist) {
						std::vector<GARRouteDesc>::iterator it = into.addRouteDesc(current);
						tripCont.addRouteDesc(dynamic_cast<GAREdge*>(edge),
											  dynamic_cast<GAREdge*>(last),
											  *it);
					}
					continue;
				} else {
					// ... if it's an in-between-detector
					// -> mark the current route as to be continued
					current.passedNo = 0;
					current.duration2Last = (SUMOTime) current.duration_2;
					current.distance2Last = current.distance;
					current.lastDetectorEdge = last;
				}
			}
		}
		// check for highway off-ramps
		if (myAmInHighwayMode) {
			// if it's beside the highway...
			if (last->getSpeed() < 19.4 && last != getDetectorEdge(det)) {
				// ... and has more than one following edge
				if (myApproachedEdges.find(last)->second.size() > 1) {
					// -> let's add this edge and the following, but not any further
					addNextNoFurther = true;
				}

			}
		}
		// check for missing end connections
		if (!addNextNoFurther) {
			// ... if this one would be processed, but already too many edge
			//  without a detector occured
			if (current.passedNo > maxFollowingLength) {
				// mark not to process any further
				WRITE_WARNING("Could not close route for '" + det.getID() + "'");
				unfoundEnds.push_back(current);
				current.factor = 1.;
				SUMOReal cdist = current.edges2Pass[0]->getFromNode()->getPosition().
									distanceTo(current.edges2Pass.back()->getToNode()->getPosition());
				if (minDist < cdist) {
					std::vector<GARRouteDesc>::iterator it = into.addRouteDesc(current);
					tripCont.addRouteDesc(dynamic_cast<GAREdge*>(edge),
										  dynamic_cast<GAREdge*>(last),
										  *it);
				}
				continue;
			}
		}
		// ... else: loop over the next edges
		const std::vector<ROEdge*>& appr = myApproachedEdges.find(last)->second;
		bool hadOne = false;
		for (size_t i = 0; i < appr.size(); i++) {
			if (find(current.edges2Pass.begin(), current.edges2Pass.end(),
					appr[i]) != current.edges2Pass.end()) {
				// do not append an edge twice (do not build loops)
				continue;
			}
			GARRouteDesc t(current);
			t.duration_2 += (appr[i]->getLength() / appr[i]->getSpeed()); //!!!
			t.distance += appr[i]->getLength();
			t.edges2Pass.push_back(appr[i]);
			if (!addNextNoFurther) {
				t.passedNo = t.passedNo + 1;
				toSolve.push(t);
			} else {
				if (!hadOne) {
					t.factor = (SUMOReal) 1. / (SUMOReal) appr.size();
					SUMOReal cdist = current.edges2Pass[0]->getFromNode()->getPosition()
											.distanceTo(current.edges2Pass.back()->getToNode()->getPosition());
					if (minDist < cdist) {
						std::vector<GARRouteDesc>::iterator it = into.addRouteDesc(current);
						tripCont.addRouteDesc(dynamic_cast<GAREdge*>(edge),
											  dynamic_cast<GAREdge*>(last),
											  *it);
					}
					hadOne = true;
				}
			}
		}
	}

	std::vector<GARRouteDesc>::iterator i;
	std::vector<const ROEdge*> lastDetEdges;
	for (i = unfoundEnds.begin(); i != unfoundEnds.end(); ++i) {
		if (find(lastDetEdges.begin(), lastDetEdges.end(),
				(*i).lastDetectorEdge) == lastDetEdges.end()) {
			lastDetEdges.push_back((*i).lastDetectorEdge);
		} else {
			bool ok = into.removeRouteDesc(*i);
			assert(ok);
			UNUSED_PARAMETER(ok); // ony used for assertion
		}
	}

	while (!toSolve.empty()) {
		toSolve.pop();
	}
}


//................................................. Computes routes between source and sink detectors ...
void GARNet::buildRoutes(GARDetectorCon& detCont,
						 const GARTazCont& tazCont,
						 GARRouteCont& rouCont,
						 GARTripCont& tripCont,
						 int maxFollowingLength,
						 CLogger* logger) const {
	// Build needed information first
	this->buildDetectorEdgeDependencies(detCont);

	// Then build the routes
	std::map<ROEdge*, GARRouteCont*> doneEdges;

	// Get the source and sink detectors
	const std::vector<GARDetector*>& sources = detCont.getDetectorsByType(SOURCE_DETECTOR);

	for (GARDetector* pSourceDet : sources) {
		logger->debug("Compute routes from source detector [" + pSourceDet->getID() + "]...");

		// Check if the detector edge has been processed
		ROEdge* edge = getDetectorEdge(*pSourceDet);
		if (doneEdges.find(edge) != doneEdges.end()) {
			// use previously build routes
			pSourceDet->addRoutes(new GARRouteCont(*doneEdges[edge]));
			continue;
		}

		std::vector<ROEdge*> seen;
		GARRouteCont* pDetRouCont = new GARRouteCont();
		doneEdges[edge] = pDetRouCont;

		GARRouteDesc rd;
		rd.edges2Pass.push_back(edge);
		rd.duration_2 = (edge->getLength() / edge->getSpeed()); //!!!;
		rd.endDetectorEdge = nullptr;
		rd.lastDetectorEdge = nullptr;
		rd.distance = edge->getLength();
		rd.distance2Last = 0;
		rd.duration2Last = 0;
		rd.overallProb = 0;

		std::vector<ROEdge*> visited;
		visited.push_back(edge);

		computeRoutesFor(edge,
						 rd,
						 //0,
						 //visited,
						 *pSourceDet,
						 *pDetRouCont,
						 tripCont,
						 detCont,
						 tazCont,
						 maxFollowingLength,
						 seen,
						 logger);
		// Add detector routes
		pSourceDet->addRoutes(pDetRouCont);

		// Add routes to container
		for (GARRouteDesc& rou : pDetRouCont->get()) {
			rouCont.addRouteDesc(rou);
		}
	}
}


void GARNet::revalidateFlows(const GARDetector* detector,
							 GARDetectorFlows& flows,
							 SUMOTime startTime,
							 SUMOTime endTime,
							 SUMOTime stepOffset) {
	{
		if (flows.knows(detector->getID())) {
			const std::vector<FlowDef>& detFlows = flows.getFlowDefs(
					detector->getID());
			for (std::vector<FlowDef>::const_iterator j = detFlows.begin(); j != detFlows.end(); ++j) {
				if ((*j).qPKW > 0 || (*j).qLKW > 0) {
					return;
				}
			}
		}
	}
	// ok, there is no information for the whole time;
	//  lets find preceding detectors and rebuild the flows if possible
	WRITE_WARNING(
			"Detector '" + detector->getID()
					+ "' has no flows.\n Trying to rebuild.");
	// go back and collect flows
	std::vector<ROEdge*> previous;
	{
		std::vector<IterationEdge> missing;
		IterationEdge ie;
		ie.depth = 0;
		ie.edge = getDetectorEdge(*detector);
		missing.push_back(ie);
		bool maxDepthReached = false;
		while (!missing.empty() && !maxDepthReached) {
			IterationEdge last = missing.back();
			missing.pop_back();
			std::vector<ROEdge*> approaching = myApproachingEdges[last.edge];
			for (std::vector<ROEdge*>::const_iterator j = approaching.begin();
					j != approaching.end(); ++j) {
				if (hasDetector(*j)) {
					previous.push_back(*j);
				} else {
					ie.depth = last.depth + 1;
					ie.edge = *j;
					missing.push_back(ie);
					if (ie.depth > 5) {
						maxDepthReached = true;
					}
				}
			}
		}
		if (maxDepthReached) {
			WRITE_WARNING(" Could not build list of previous flows.");
		}
	}
	// Edges with previous detectors are now in "previous";
	//  compute following
	std::vector<ROEdge*> latter;
	{
		std::vector<IterationEdge> missing;
		for (std::vector<ROEdge*>::const_iterator k = previous.begin();
				k != previous.end(); ++k) {
			IterationEdge ie;
			ie.depth = 0;
			ie.edge = *k;
			missing.push_back(ie);
		}
		bool maxDepthReached = false;
		while (!missing.empty() && !maxDepthReached) {
			IterationEdge last = missing.back();
			missing.pop_back();
			std::vector<ROEdge*> approached = myApproachedEdges[last.edge];
			for (std::vector<ROEdge*>::const_iterator j = approached.begin();
					j != approached.end(); ++j) {
				if (*j == getDetectorEdge(*detector)) {
					continue;
				}
				if (hasDetector(*j)) {
					latter.push_back(*j);
				} else {
					IterationEdge ie;
					ie.depth = last.depth + 1;
					ie.edge = *j;
					missing.push_back(ie);
					if (ie.depth > 5) {
						maxDepthReached = true;
					}
				}
			}
		}
		if (maxDepthReached) {
			WRITE_WARNING(" Could not build list of latter flows.");
			return;
		}
	}
	// Edges with latter detectors are now in "latter";

	// lets not validate them by now - surely this should be done
	// for each time step: collect incoming flows; collect outgoing;
	std::vector<FlowDef> mflows;
	int index = 0;
	for (SUMOTime t = startTime; t < endTime; t += stepOffset, index++) {
		FlowDef inFlow;
		inFlow.qLKW = 0;
		inFlow.qPKW = 0;
		inFlow.vLKW = 0;
		inFlow.vPKW = 0;
		// collect incoming
		{
			// !! time difference is missing
			for (std::vector<ROEdge*>::iterator i = previous.begin();
					i != previous.end(); ++i) {
				const std::vector<FlowDef>& flows =
						static_cast<const GAREdge*>(*i)->getFlows();
				if (flows.size() != 0) {
					const FlowDef& srcFD = flows[index];
					inFlow.qLKW += srcFD.qLKW;
					inFlow.qPKW += srcFD.qPKW;
					inFlow.vLKW += srcFD.vLKW;
					inFlow.vPKW += srcFD.vPKW;
				}
			}
		}
		inFlow.vLKW /= (SUMOReal) previous.size();
		inFlow.vPKW /= (SUMOReal) previous.size();
		// collect outgoing
		FlowDef outFlow;
		outFlow.qLKW = 0;
		outFlow.qPKW = 0;
		outFlow.vLKW = 0;
		outFlow.vPKW = 0;
		{
			// !! time difference is missing
			for (std::vector<ROEdge*>::iterator i = latter.begin();
					i != latter.end(); ++i) {
				const std::vector<FlowDef>& flows =
						static_cast<const GAREdge*>(*i)->getFlows();
				if (flows.size() != 0) {
					const FlowDef& srcFD = flows[index];
					outFlow.qLKW += srcFD.qLKW;
					outFlow.qPKW += srcFD.qPKW;
					outFlow.vLKW += srcFD.vLKW;
					outFlow.vPKW += srcFD.vPKW;
				}
			}
		}
		outFlow.vLKW /= (SUMOReal) latter.size();
		outFlow.vPKW /= (SUMOReal) latter.size();
		//
		FlowDef mFlow;
		mFlow.qLKW = inFlow.qLKW - outFlow.qLKW;
		mFlow.qPKW = inFlow.qPKW - outFlow.qPKW;
		mFlow.vLKW = (inFlow.vLKW + outFlow.vLKW) / (SUMOReal) 2.;
		mFlow.vPKW = (inFlow.vPKW + outFlow.vPKW) / (SUMOReal) 2.;
		mflows.push_back(mFlow);
	}
	static_cast<GAREdge*>(getDetectorEdge(*detector))->setFlows(mflows);
	flows.setFlows(detector->getID(), mflows);
}

void GARNet::revalidateFlows(const GARDetectorCon& detectors,
		GARDetectorFlows& flows, SUMOTime startTime, SUMOTime endTime,
		SUMOTime stepOffset) {
	const std::vector<GARDetector*>& dets = detectors.getDetectors();
	for (std::vector<GARDetector*>::const_iterator i = dets.begin();
			i != dets.end(); ++i) {
		// check whether there is at least one entry with a flow larger than zero
		revalidateFlows(*i, flows, startTime, endTime, stepOffset);
	}
}


void GARNet::removeEmptyDetectors(GARDetectorCon& detectors,
								  GARDetectorFlows& flows) {

	for (const GARDetector const* pDet : detectors.getDetectors()) {
		string detId = pDet->getID();
		bool remove = true;

		// Remove detectors having no flows
		if (flows.knows(detId)) {
			for (const FlowDef& fd : flows.getFlowDefs(detId)) {
				// Don't remove if any passenger or heavy duty vehicle has been detected
				if (fd.qPKW > 0  ||  fd.qLKW > 0) {
					remove = false;
					break;
				}
			}
		}

		if (remove &&  (pDet->getType() == SOURCE_DETECTOR || pDet->getType() == SINK_DETECTOR)) {
			WRITE_MESSAGE("Removed empty detector '" + detId + "'");
			flows.removeFlow(detId);
			detectors.removeDetector(detId);
		}
	}
}


void GARNet::reportEmptyDetectors(GARDetectorCon& detectors,
		GARDetectorFlows& flows) {
	const std::vector<GARDetector*>& dets = detectors.getDetectors();
	for (std::vector<GARDetector*>::const_iterator i = dets.begin();
			i != dets.end(); ++i) {
		bool remove = true;
		// check whether there is at least one entry with a flow larger than zero
		if (flows.knows((*i)->getID())) {
			remove = false;
		}
		if (remove) {
			WRITE_MESSAGE("Detector '" + (*i)->getID() + "' has no flow.");
		}
	}
}

ROEdge*
GARNet::getDetectorEdge(const GARDetector& det) const {
	std::string edgeName = det.getLaneID();
	edgeName = edgeName.substr(0, edgeName.rfind('_'));
	ROEdge* ret = getEdge(edgeName);
	if (ret == 0) {
		throw ProcessError(
				"Edge '" + edgeName + "' used by detector '" + det.getID()
						+ "' is not known.");
	}
	return ret;
}

bool GARNet::hasApproaching(ROEdge* edge) const {
	return myApproachingEdges.find(edge) != myApproachingEdges.end()
			&& myApproachingEdges.find(edge)->second.size() != 0;
}

bool GARNet::hasApproached(ROEdge* edge) const {
	return myApproachedEdges.find(edge) != myApproachedEdges.end()
			&& myApproachedEdges.find(edge)->second.size() != 0;
}

bool GARNet::hasDetector(ROEdge* edge) const {
	return myDetectorsOnEdges.find(edge) != myDetectorsOnEdges.end()
			&& myDetectorsOnEdges.find(edge)->second.size() != 0;
}

const std::vector<std::string>&
GARNet::getDetectorList(ROEdge* edge) const {
	return myDetectorsOnEdges.find(edge)->second;
}


const node_set&
GARNet::getNodeList(void) const {
	return this->myNodes;
}


const ROEdge const*
GARNet::getpEdge(const std::string& fromNode,
				 const std::string& toNode) const {
	for (auto edge : myDetectorEdges) {
		const ROEdge const* pEdge = edge.second;
		if (fromNode == pEdge->getFromNode()->getID()  &&  toNode == pEdge->getToNode()->getID()) {
			return pEdge;
		}
	}

	return nullptr;
}


SUMOReal GARNet::getAbsPos(const GARDetector& det) const {
	if (det.getPos() >= 0) {
		return det.getPos();
	}
	return getDetectorEdge(det)->getLength() + det.getPos();
}

bool GARNet::isSource(const GARDetector& det,
		const GARDetectorCon& detectors, bool strict) const {
	std::vector<ROEdge*> seen;
	return isSource(det, getDetectorEdge(det), seen, detectors, strict);
}

bool GARNet::isFalseSource(const GARDetector& det,
		const GARDetectorCon& detectors) const {
	std::vector<ROEdge*> seen;
	return isFalseSource(det, getDetectorEdge(det), seen, detectors);
}

bool GARNet::isDestination(const GARDetector& det,
		const GARDetectorCon& detectors) const {
	std::vector<ROEdge*> seen;
	return isDestination(det, getDetectorEdge(det), seen, detectors);
}

bool GARNet::isSource(const GARDetector& det, ROEdge* edge,
		std::vector<ROEdge*>& seen, const GARDetectorCon& detectors,
		bool strict) const {
	if (seen.size() == 1000) { // !!!
		WRITE_WARNING(
				"Quitting checking for being a source for detector '"
						+ det.getID() + "' due to seen edge limit.");
		return false;
	}
	if (edge == getDetectorEdge(det)) {
		// maybe there is another detector at the same edge
		//  get the list of this/these detector(s)
		const std::vector<std::string>& detsOnEdge = myDetectorsOnEdges.find(
				edge)->second;
		for (std::vector<std::string>::const_iterator i = detsOnEdge.begin();
				i != detsOnEdge.end(); ++i) {
			if ((*i) == det.getID()) {
				continue;
			}
			const GARDetector& sec = detectors.getDetector(*i);
			if (getAbsPos(sec) < getAbsPos(det)) {
				// ok, there is another detector on the same edge and it is
				//  before this one -> no source
				return false;
			}
		}
	}
	// it's a source if no edges are approaching the edge
	if (!hasApproaching(edge)) {
		if (edge != getDetectorEdge(det)) {
			if (hasDetector(edge)) {
				return false;
			}
		}
		return true;
	}
	if (edge != getDetectorEdge(det)) {
		// ok, we are at one of the edges in front
		if (myAmInHighwayMode) {
			if (edge->getSpeed() >= 19.4) {
				if (hasDetector(edge)) {
					// we are still on the highway and there is another detector
					return false;
				}
				// the next is a hack for the A100 scenario...
				//  We have to look into further edges herein edges
				const std::vector<ROEdge*>& appr =
						myApproachingEdges.find(edge)->second;
				size_t noOk = 0;
				size_t noFalse = 0;
				size_t noSkipped = 0;
				for (size_t i = 0; i < appr.size(); i++) {
					if (!hasDetector(appr[i])) {
						noOk++;
					} else {
						noFalse++;
					}
				}
				if ((noFalse + noSkipped) == appr.size()) {
					return false;
				}
			}
		}
	}

	if (myAmInHighwayMode) {
		if (edge->getSpeed() < 19.4 && edge != getDetectorEdge(det)) {
			// we have left the highway already
			//  -> the detector will be a highway source
			if (!hasDetector(edge)) {
				return true;
			}
		}
	}
	if (myDetectorsOnEdges.find(edge) != myDetectorsOnEdges.end()
			&& myDetectorEdges.find(det.getID())->second != edge) {
		return false;
	}

	// let's check the edges in front
	const std::vector<ROEdge*>& appr = myApproachingEdges.find(edge)->second;
	size_t noOk = 0;
	size_t noFalse = 0;
	size_t noSkipped = 0;
	seen.push_back(edge);
	for (size_t i = 0; i < appr.size(); i++) {
		bool had = std::find(seen.begin(), seen.end(), appr[i]) != seen.end();
		if (!had) {
			if (isSource(det, appr[i], seen, detectors, strict)) {
				noOk++;
			} else {
				noFalse++;
			}
		} else {
			noSkipped++;
		}
	}
	if (!strict) {
		return (noFalse + noSkipped) != appr.size();
	} else {
		return (noOk + noSkipped) == appr.size();
	}
}

bool GARNet::isDestination(const GARDetector& det, ROEdge* edge,
		std::vector<ROEdge*>& seen, const GARDetectorCon& detectors) const {
	if (seen.size() == 1000) { // !!!
		WRITE_WARNING(
				"Quitting checking for being a destination for detector '"
						+ det.getID() + "' due to seen edge limit.");
		return false;
	}
	if (edge == getDetectorEdge(det)) {
		// maybe there is another detector at the same edge
		//  get the list of this/these detector(s)
		const std::vector<std::string>& detsOnEdge = myDetectorsOnEdges.find(
				edge)->second;
		for (std::vector<std::string>::const_iterator i = detsOnEdge.begin();
				i != detsOnEdge.end(); ++i) {
			if ((*i) == det.getID()) {
				continue;
			}
			const GARDetector& sec = detectors.getDetector(*i);
			if (getAbsPos(sec) > getAbsPos(det)) {
				// ok, there is another detector on the same edge and it is
				//  after this one -> no destination
				return false;
			}
		}
	}
	if (!hasApproached(edge)) {
		if (edge != getDetectorEdge(det)) {
			if (hasDetector(edge)) {
				return false;
			}
		}
		return true;
	}
	if (edge != getDetectorEdge(det)) {
		// ok, we are at one of the edges coming behind
		if (myAmInHighwayMode) {
			if (edge->getSpeed() >= 19.4) {
				if (hasDetector(edge)) {
					// we are still on the highway and there is another detector
					return false;
				}
			}
		}
	}

	if (myAmInHighwayMode) {
		if (edge->getSpeed() < 19.4 && edge != getDetectorEdge(det)) {
			if (hasDetector(edge)) {
				return true;
			}
			if (myApproachedEdges.find(edge)->second.size() > 1) {
				return true;
			}

		}
	}

	if (myDetectorsOnEdges.find(edge) != myDetectorsOnEdges.end()
			&& myDetectorEdges.find(det.getID())->second != edge) {
		return false;
	}
	const std::vector<ROEdge*>& appr = myApproachedEdges.find(edge)->second;
	bool isall = true;
	size_t no = 0;
	seen.push_back(edge);
	for (size_t i = 0; i < appr.size() && isall; i++) {
		bool had = std::find(seen.begin(), seen.end(), appr[i]) != seen.end();
		if (!had) {
			if (!isDestination(det, appr[i], seen, detectors)) {
				no++;
				isall = false;
			}
		}
	}
	return isall;
}

bool GARNet::isFalseSource(const GARDetector& det, ROEdge* edge,
		std::vector<ROEdge*>& seen, const GARDetectorCon& detectors) const {
	if (seen.size() == 1000) { // !!!
		WRITE_WARNING(
				"Quitting checking for being a false source for detector '"
						+ det.getID() + "' due to seen edge limit.");
		return false;
	}
	seen.push_back(edge);
	if (edge != getDetectorEdge(det)) {
		// ok, we are at one of the edges coming behind
		if (hasDetector(edge)) {
			const std::vector<std::string>& dets =
					myDetectorsOnEdges.find(edge)->second;
			for (std::vector<std::string>::const_iterator i = dets.begin();
					i != dets.end(); ++i) {
				if (detectors.getDetector(*i).getType() == SINK_DETECTOR) {
					return false;
				}
				if (detectors.getDetector(*i).getType() == BETWEEN_DETECTOR) {
					return false;
				}
				if (detectors.getDetector(*i).getType() == SOURCE_DETECTOR) {
					return true;
				}
			}
		} else {
			if (myAmInHighwayMode && edge->getSpeed() < 19.) {
				return false;
			}
		}
	}

	if (myApproachedEdges.find(edge) == myApproachedEdges.end()) {
		return false;
	}

	const std::vector<ROEdge*>& appr = myApproachedEdges.find(edge)->second;
	bool isall = false;
	for (size_t i = 0; i < appr.size() && !isall; i++) {
		//printf("checking %s->\n", appr[i].c_str());
		bool had = std::find(seen.begin(), seen.end(), appr[i]) != seen.end();
		if (!had) {
			if (isFalseSource(det, appr[i], seen, detectors)) {
				isall = true;
			}
		}
	}
	return isall;
}

void GARNet::buildEdgeFlowMap(const GARDetectorFlows& flows,
		const GARDetectorCon& detectors, SUMOTime startTime, SUMOTime endTime,
		SUMOTime stepOffset) {
	std::map<ROEdge*, std::vector<std::string>, idComp>::iterator i;
	for (i = myDetectorsOnEdges.begin(); i != myDetectorsOnEdges.end(); ++i) {
		ROEdge* into = (*i).first;
		const std::vector<std::string>& dets = (*i).second;
		std::map<SUMOReal, std::vector<std::string> > cliques;
		std::vector<std::string>* maxClique = 0;
		for (std::vector<std::string>::const_iterator j = dets.begin();
				j != dets.end(); ++j) {
			if (!flows.knows(*j)) {
				continue;
			}
			const GARDetector& det = detectors.getDetector(*j);
			bool found = false;
			for (std::map<SUMOReal, std::vector<std::string> >::iterator k =
					cliques.begin(); !found && k != cliques.end(); ++k) {
				if (fabs((*k).first - det.getPos()) < 1) {
					(*k).second.push_back(*j);
					if ((*k).second.size() > maxClique->size()) {
						maxClique = &(*k).second;
					}
					found = true;
				}
			}
			if (!found) {
				cliques[det.getPos()].push_back(*j);
				maxClique = &cliques[det.getPos()];
			}
		}
		if (maxClique == 0) {
			continue;
		}
		std::vector<FlowDef> mflows; // !!! reserve
		for (SUMOTime t = startTime; t < endTime; t += stepOffset) {
			FlowDef fd;
			fd.qPKW = 0;
			fd.qLKW = 0;
			fd.vLKW = 0;
			fd.vPKW = 0;
			fd.fLKW = 0;
			fd.isLKW = 0;
			mflows.push_back(fd);
		}
		for (std::vector<std::string>::iterator l = maxClique->begin();
				l != maxClique->end(); ++l) {
			bool didWarn = false;
			const std::vector<FlowDef>& dflows = flows.getFlowDefs(*l);
			int index = 0;
			for (SUMOTime t = startTime; t < endTime;
					t += stepOffset, index++) {
				const FlowDef& srcFD = dflows[index];
				FlowDef& fd = mflows[index];
				fd.qPKW += srcFD.qPKW;
				fd.qLKW += srcFD.qLKW;
				fd.vLKW += (srcFD.vLKW / (SUMOReal) maxClique->size());
				fd.vPKW += (srcFD.vPKW / (SUMOReal) maxClique->size());
				fd.fLKW += (srcFD.fLKW / (SUMOReal) maxClique->size());
				fd.isLKW += (srcFD.isLKW / (SUMOReal) maxClique->size());
				if (!didWarn && srcFD.vPKW > 0 && srcFD.vPKW < 255
						&& srcFD.vPKW / 3.6 > into->getSpeed()) {
					WRITE_MESSAGE(
							"Detected PKW speed higher than allowed speed at '"
									+ (*l) + "' on '" + into->getID() + "'.");
					didWarn = true;
				}
				if (!didWarn && srcFD.vLKW > 0 && srcFD.vLKW < 255
						&& srcFD.vLKW / 3.6 > into->getSpeed()) {
					WRITE_MESSAGE(
							"Detected LKW speed higher than allowed speed at '"
									+ (*l) + "' on '" + into->getID() + "'.");
					didWarn = true;
				}
			}
		}
		static_cast<GAREdge*>(into)->setFlows(mflows);
	}
}

void GARNet::buildDetectorDependencies(GARDetectorCon& detectors) {
	// !!! this will not work when several detectors are lying on the same edge on different positions

	buildDetectorEdgeDependencies(detectors);
	// for each detector, compute the lists of predecessor and following detectors
	std::map<std::string, ROEdge*>::const_iterator i;
	for (i = myDetectorEdges.begin(); i != myDetectorEdges.end(); ++i) {
		const GARDetector& det = detectors.getDetector((*i).first);
		if (!det.hasRoutes()) {
			continue;
		}
		// mark current detectors
		std::vector<GARDetector*> last;
		{
			const std::vector<std::string>& detNames = myDetectorsOnEdges.find(
					(*i).second)->second;
			for (std::vector<std::string>::const_iterator j = detNames.begin();
					j != detNames.end(); ++j) {
				last.push_back(&detectors.getModifiableDetector(*j));
			}
		}
		// iterate over the current detector's routes
		const std::vector<GARRouteDesc>& routes = det.getRouteVector();
		for (std::vector<GARRouteDesc>::const_iterator j = routes.begin();
				j != routes.end(); ++j) {
			const std::vector<ROEdge*>& edges2Pass = (*j).edges2Pass;
			for (std::vector<ROEdge*>::const_iterator k = edges2Pass.begin()
					+ 1; k != edges2Pass.end(); ++k) {
				if (myDetectorsOnEdges.find(*k) != myDetectorsOnEdges.end()) {
					const std::vector<std::string>& detNames =
							myDetectorsOnEdges.find(*k)->second;
					// ok, consecutive detector found
					for (std::vector<GARDetector*>::iterator l = last.begin();
							l != last.end(); ++l) {
						// mark as follower of current
						for (std::vector<std::string>::const_iterator m =
								detNames.begin(); m != detNames.end(); ++m) {
							detectors.getModifiableDetector(*m).addPriorDetector(
									*l);
							(*l)->addFollowingDetector(
									&detectors.getDetector(*m));
						}
					}
					last.clear();
					for (std::vector<std::string>::const_iterator m =
							detNames.begin(); m != detNames.end(); ++m) {
						last.push_back(&detectors.getModifiableDetector(*m));
					}
				}
			}
		}
	}
}

void GARNet::mesoJoin(GARDetectorCon& detectors, GARDetectorFlows& flows) {
	buildDetectorEdgeDependencies(detectors);
	std::map<ROEdge*, std::vector<std::string>, idComp>::iterator i;
	for (i = myDetectorsOnEdges.begin(); i != myDetectorsOnEdges.end(); ++i) {
		const std::vector<std::string>& dets = (*i).second;
		std::map<SUMOReal, std::vector<std::string> > cliques;
		// compute detector cliques
		for (std::vector<std::string>::const_iterator j = dets.begin();
				j != dets.end(); ++j) {
			const GARDetector& det = detectors.getDetector(*j);
			bool found = false;
			for (std::map<SUMOReal, std::vector<std::string> >::iterator k =
					cliques.begin(); !found && k != cliques.end(); ++k) {
				if (fabs((*k).first - det.getPos()) < 10.) {
					(*k).second.push_back(*j);
					found = true;
				}
			}
			if (!found) {
				cliques[det.getPos()] = std::vector<std::string>();
				cliques[det.getPos()].push_back(*j);
			}
		}
		// join detector cliques
		for (std::map<SUMOReal, std::vector<std::string> >::iterator m =
				cliques.begin(); m != cliques.end(); ++m) {
			std::vector<std::string> clique = (*m).second;
			// do not join if only one
			if (clique.size() == 1) {
				continue;
			}
			std::string nid;
			for (std::vector<std::string>::iterator n = clique.begin();
					n != clique.end(); ++n) {
				std::cout << *n << " ";
				if (n != clique.begin()) {
					nid = nid + "_";
				}
				nid = nid + *n;
			}
			std::cout << ":" << nid << std::endl;
			flows.mesoJoin(nid, (*m).second);
			detectors.mesoJoin(nid, (*m).second);
		}
	}
}

/****************************************************************************/

