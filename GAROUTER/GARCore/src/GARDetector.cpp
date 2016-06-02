/****************************************************************************/
/// @file    GARDetector.cpp
/// @author  Daniel Krajzewicz
/// @author  Eric Nicolay
/// @author  Jakob Erdmann
/// @author  Sascha Krieg
/// @author  Michael Behrisch
/// @author  Laura Bieker
/// @author  Melanie Knocke
/// @date    Thu, 16.03.2006
/// @version $Id: GARDetector.cpp 17235 2014-11-03 10:53:02Z behrisch $
///
// Class representing a detector within the DFROUTER
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

#include <cassert>
#include <utils/common/FileHelpers.h>
#include <utils/common/MsgHandler.h>
#include <utils/common/UtilExceptions.h>
#include <utils/common/ToString.h>
#include <router/ROEdge.h>
#include "GARDetector.hpp"
#include "GAREdge.hpp"
#include "GARRouteDesc.hpp"
#include "GARRouteCont.hpp"
#include "GARDetectorFlow.hpp"
#include "GARDetectorCon.hpp"
#include "GARNet.hpp"
#include <utils/common/StdDefs.h>
#include <utils/common/TplConvert.h>
#include <utils/geom/GeomHelper.h>
#include <utils/iodevices/OutputDevice.h>
#include <utils/options/OptionsCont.h>

#ifdef CHECK_MEMORY_LEAKS
#include <foreign/nvwa/debug_new.h>
#endif // CHECK_MEMORY_LEAKS

// ===========================================================================
// method definitions
// ===========================================================================
GARDetector::GARDetector(const std::string& id, const std::string& laneID,
                           SUMOReal pos, const GARDetectorType type)
    : Named(id), myLaneID(laneID), myPosition(pos), myType(type), myRoutes(new GARRouteCont()) {
	// Empty constructor
}


GARDetector::GARDetector(const std::string& id, const GARDetector& f)
    : Named(id), myLaneID(f.myLaneID), myPosition(f.myPosition),
      myType(f.myType), myRoutes(0) {
    if (f.myRoutes != 0) {
        myRoutes = new GARRouteCont(*(f.myRoutes));
    }
}


GARDetector::~GARDetector() {
    delete myRoutes;
}


void
GARDetector::setType(GARDetectorType type) {
    myType = type;
}


SUMOReal
GARDetector::computeDistanceFactor(const GARRouteDesc& rd) const {
    SUMOReal distance = rd.edges2Pass[0]->getFromNode()->getPosition().distanceTo(rd.edges2Pass.back()->getToNode()->getPosition());
    SUMOReal length = 0;
    for (std::vector<ROEdge*>::const_iterator i = rd.edges2Pass.begin(); i != rd.edges2Pass.end(); ++i) {
        length += (*i)->getLength();
    }
    return (distance / length);
}


void
GARDetector::computeSplitProbabilities(const GARNet* net, const GARDetectorCon& detectors,
                                        const GARDetectorFlows& flows,
                                        SUMOTime startTime, SUMOTime endTime, SUMOTime stepOffset) {
    if (myRoutes == 0) {
        return;
    }
    // compute edges to determine split probabilities
    const std::vector<GARRouteDesc>& routes = myRoutes->get();
    std::vector<GAREdge*> nextDetEdges;
    std::set<ROEdge*> preSplitEdges;
    for (std::vector<GARRouteDesc>::const_iterator i = routes.begin(); i != routes.end(); ++i) {
        const GARRouteDesc& rd = *i;
        bool hadSplit = false;
        for (std::vector<ROEdge*>::const_iterator j = rd.edges2Pass.begin(); j != rd.edges2Pass.end(); ++j) {
            if (hadSplit && net->hasDetector(*j)) {
                if (find(nextDetEdges.begin(), nextDetEdges.end(), *j) == nextDetEdges.end()) {
                    nextDetEdges.push_back(static_cast<GAREdge*>(*j));
                }
                myRoute2Edge[rd.routename] = static_cast<GAREdge*>(*j);
                break;
            }
            if (!hadSplit) {
                preSplitEdges.insert(*j);
            }
            if ((*j)->getNumSuccessors() > 1) {
                hadSplit = true;
            }
        }
    }
    std::map<ROEdge*, SUMOReal> inFlows;
    if (OptionsCont::getOptions().getBool("respect-concurrent-inflows")) {
        for (std::vector<GAREdge*>::const_iterator i = nextDetEdges.begin(); i != nextDetEdges.end(); ++i) {
            std::set<ROEdge*> seen(preSplitEdges);
            std::vector<ROEdge*> pending;
            pending.push_back(*i);
            seen.insert(*i);
            while (!pending.empty()) {
                ROEdge* e = pending.back();
                pending.pop_back();
                const unsigned int numAppr = e->getNumPredecessors();
                for (unsigned int j = 0; j < numAppr; j++) {
                    ROEdge* e2 = e->getPredecessor(j);
                    if (e2->getNumSuccessors() == 1 && seen.count(e2) == 0) {
                        if (net->hasDetector(e2)) {
                            inFlows[*i] += detectors.getAggFlowFor(e2, 0, 0, flows);
                        } else {
                            pending.push_back(e2);
                        }
                        seen.insert(e2);
                    }
                }
            }
        }
    }
    // compute the probabilities to use a certain direction
    int index = 0;
    for (SUMOTime time = startTime; time < endTime; time += stepOffset, ++index) {
        mySplitProbabilities.push_back(std::map<GAREdge*, SUMOReal>());
        SUMOReal overallProb = 0;
        // retrieve the probabilities
        for (std::vector<GAREdge*>::const_iterator i = nextDetEdges.begin(); i != nextDetEdges.end(); ++i) {
            SUMOReal flow = detectors.getAggFlowFor(*i, time, 60, flows) - inFlows[*i];
            overallProb += flow;
            mySplitProbabilities[index][*i] = flow;
        }
        // norm probabilities
        if (overallProb > 0) {
            for (std::vector<GAREdge*>::const_iterator i = nextDetEdges.begin(); i != nextDetEdges.end(); ++i) {
                mySplitProbabilities[index][*i] = mySplitProbabilities[index][*i] / overallProb;
            }
        }
    }
}


void
GARDetector::buildDestinationDistribution(const GARDetectorCon& detectors,
        SUMOTime startTime, SUMOTime endTime, SUMOTime stepOffset,
        const GARNet& net,
        std::map<size_t, RandomDistributor<size_t>* >& into) const {
    if (myRoutes == 0) {
        if (myType != DISCARDED_DETECTOR && myType != BETWEEN_DETECTOR) {
            WRITE_ERROR("Missing routes for detector '" + myID + "'.");
        }
        return;
    }
    std::vector<GARRouteDesc>& descs = myRoutes->get();
    // iterate through time (in output interval steps)
    for (SUMOTime time = startTime; time < endTime; time += stepOffset) {
        into[time] = new RandomDistributor<size_t>();
        std::map<ROEdge*, SUMOReal> flowMap;
        // iterate through the routes
        size_t index = 0;
        for (std::vector<GARRouteDesc>::iterator ri = descs.begin(); ri != descs.end(); ++ri, index++) {
            SUMOReal prob = 1.;
            for (std::vector<ROEdge*>::iterator j = (*ri).edges2Pass.begin(); j != (*ri).edges2Pass.end() && prob > 0;) {
                if (!net.hasDetector(*j)) {
                    ++j;
                    continue;
                }
                const GARDetector& det = detectors.getAnyDetectorForEdge(static_cast<GAREdge*>(*j));
                const std::vector<std::map<GAREdge*, SUMOReal> >& probs = det.getSplitProbabilities();
                if (probs.size() == 0) {
                    prob = 0;
                    ++j;
                    continue;
                }
                const std::map<GAREdge*, SUMOReal>& tprobs = probs[(time - startTime) / stepOffset];
                GAREdge* splitEdge = 0;
                for (std::map<GAREdge*, SUMOReal>::const_iterator k = tprobs.begin(); k != tprobs.end(); ++k) {
                    if (find(j, (*ri).edges2Pass.end(), (*k).first) != (*ri).edges2Pass.end()) {
                        prob *= (*k).second;
                        splitEdge = (*k).first;
                        break;
                    }
                }
                if (splitEdge != 0) {
                    j = find(j, (*ri).edges2Pass.end(), splitEdge);
                } else {
                    ++j;
                }
            }
            into[time]->add(prob, index);
            (*ri).overallProb = prob;
        }
    }
}


const std::vector<GARRouteDesc>&
GARDetector::getRouteVector() const {
	return myRoutes->get();
}


void
GARDetector::addPriorDetector(const GARDetector* det) {
    myPriorDetectors.insert(det);
}


void
GARDetector::addFollowingDetector(const GARDetector* det) {
    myFollowingDetectors.insert(det);
}


const std::set<const GARDetector*>&
GARDetector::getPriorDetectors() const {
    return myPriorDetectors;
}


const std::set<const GARDetector*>&
GARDetector::getFollowerDetectors() const {
    return myFollowingDetectors;
}



void
GARDetector::addRoutes(GARRouteCont* routes) {
    delete myRoutes;
    myRoutes = routes;
}


void
GARDetector::addRoute(GARRouteDesc& nrd) {
    if (myRoutes == 0) {
        myRoutes = new GARRouteCont();
    }
    myRoutes->addRouteDesc(nrd);
}


bool
GARDetector::hasRoutes() const {
    return myRoutes != 0 && myRoutes->get().size() != 0;
}


bool
GARDetector::writeEmitterDefinition(const std::string& file,
                                     const std::map<size_t, RandomDistributor<size_t>* >& dists,
                                     const GARDetectorFlows& flows,
                                     SUMOTime startTime, SUMOTime endTime,
                                     SUMOTime stepOffset,
                                     bool includeUnusedRoutes,
                                     SUMOReal scale,
                                     bool insertionsOnly,
                                     SUMOReal defaultSpeed) const {
    OutputDevice& out = OutputDevice::getDevice(file);
    OptionsCont& oc = OptionsCont::getOptions();
    if (getType() != SOURCE_DETECTOR) {
        out.writeXMLHeader("calibrator");
    }
    // routes
    if (myRoutes != 0 && myRoutes->get().size() != 0) {
        const std::vector<GARRouteDesc>& routes = myRoutes->get();
        out.openTag(SUMO_TAG_ROUTE_DISTRIBUTION).writeAttr(SUMO_ATTR_ID, myID);
        bool isEmptyDist = true;
        for (std::vector<GARRouteDesc>::const_iterator i = routes.begin(); i != routes.end(); ++i) {
            if ((*i).overallProb > 0) {
                isEmptyDist = false;
            }
        }
        for (std::vector<GARRouteDesc>::const_iterator i = routes.begin(); i != routes.end(); ++i) {
            if ((*i).overallProb > 0 || includeUnusedRoutes) {
                out.openTag(SUMO_TAG_ROUTE).writeAttr(SUMO_ATTR_REFID, (*i).routename).writeAttr(SUMO_ATTR_PROB, (*i).overallProb).closeTag();
            }
            if (isEmptyDist) {
                out.openTag(SUMO_TAG_ROUTE).writeAttr(SUMO_ATTR_REFID, (*i).routename).writeAttr(SUMO_ATTR_PROB, SUMOReal(1)).closeTag();
            }
        }
        out.closeTag(); // routeDistribution
    } else {
        WRITE_ERROR("Detector '" + getID() + "' has no routes!?");
        return false;
    }
    // insertions
    if (insertionsOnly || flows.knows(myID)) {
        // get the flows for this detector
        const std::vector<FlowDef>& mflows = flows.getFlowDefs(myID);
        // go through the simulation seconds
        unsigned int index = 0;
        for (SUMOTime time = startTime; time < endTime; time += stepOffset, index++) {
            // get own (departure flow)
            assert(index < mflows.size());
            const FlowDef& srcFD = mflows[index];  // !!! check stepOffset
            // get flows at end
            RandomDistributor<size_t>* destDist = dists.find(time) != dists.end() ? dists.find(time)->second : 0;
            // go through the cars
            size_t carNo = (size_t)((srcFD.qPKW + srcFD.qLKW) * scale);
            for (size_t car = 0; car < carNo; ++car) {
                // get the vehicle parameter
                SUMOReal v = -1;
                std::string vtype;
                int destIndex = destDist != 0 && destDist->getOverallProb() > 0 ? (int) destDist->get() : -1;
                if (srcFD.isLKW >= 1) {
                    srcFD.isLKW = srcFD.isLKW - (SUMOReal) 1.;
                    v = srcFD.vLKW;
                    vtype = "LKW";
                } else {
                    v = srcFD.vPKW;
                    vtype = "PKW";
                }
                // compute insertion speed
                if (v <= 0 || v > 250) {
                    v = defaultSpeed;
                } else {
                    v = (SUMOReal)(v / 3.6);
                }
                // compute the departure time
                SUMOTime ctime = (SUMOTime)(time + ((SUMOReal) stepOffset * (SUMOReal) car / (SUMOReal) carNo));

                // write
                out.openTag(SUMO_TAG_VEHICLE);
                if (getType() == SOURCE_DETECTOR) {
                    out.writeAttr(SUMO_ATTR_ID, "emitter_" + myID + "_" + toString(ctime));
                } else {
                    out.writeAttr(SUMO_ATTR_ID, "calibrator_" + myID + "_" + toString(ctime));
                }
                if (oc.getBool("vtype")) {
                    out.writeAttr(SUMO_ATTR_TYPE, vtype);
                }
                out.writeAttr(SUMO_ATTR_DEPART, time2string(ctime));
                if (oc.isSet("departlane")) {
                    out.writeNonEmptyAttr(SUMO_ATTR_DEPARTLANE, oc.getString("departlane"));
                } else {
                    out.writeAttr(SUMO_ATTR_DEPARTLANE, TplConvert::_2int(myLaneID.substr(myLaneID.rfind("_") + 1).c_str()));
                }
                if (oc.isSet("departpos")) {
                    std::string posDesc = oc.getString("departpos");
                    if (posDesc.substr(0, 8) == "detector") {
                        SUMOReal position = myPosition;
                        if (posDesc.length() > 8) {
                            if (posDesc[8] == '+') {
                                position += TplConvert::_2SUMOReal(posDesc.substr(9).c_str());
                            } else if (posDesc[8] == '-') {
                                position -= TplConvert::_2SUMOReal(posDesc.substr(9).c_str());
                            } else {
                                throw NumberFormatException();
                            }
                        }
                        out.writeAttr(SUMO_ATTR_DEPARTPOS, position);
                    } else {
                        out.writeNonEmptyAttr(SUMO_ATTR_DEPARTPOS, posDesc);
                    }
                } else {
                    out.writeAttr(SUMO_ATTR_DEPARTPOS, myPosition);
                }
                if (oc.isSet("departspeed")) {
                    out.writeNonEmptyAttr(SUMO_ATTR_DEPARTSPEED, oc.getString("departspeed"));
                } else {
                    if (v > defaultSpeed) {
                        out.writeAttr(SUMO_ATTR_DEPARTSPEED, "max");
                    } else {
                        out.writeAttr(SUMO_ATTR_DEPARTSPEED, v);
                    }
                }
                if (oc.isSet("arrivallane")) {
                    out.writeNonEmptyAttr(SUMO_ATTR_ARRIVALLANE, oc.getString("arrivallane"));
                }
                if (oc.isSet("arrivalpos")) {
                    out.writeNonEmptyAttr(SUMO_ATTR_ARRIVALPOS, oc.getString("arrivalpos"));
                }
                if (oc.isSet("arrivalspeed")) {
                    out.writeNonEmptyAttr(SUMO_ATTR_ARRIVALSPEED, oc.getString("arrivalspeed"));
                }
                if (destIndex >= 0) {
                    out.writeAttr(SUMO_ATTR_ROUTE, myRoutes->get()[destIndex].routename);
                } else {
                    out.writeAttr(SUMO_ATTR_ROUTE, myID);
                }
                out.closeTag();
                srcFD.isLKW += srcFD.fLKW;
            }
        }
    }
    if (getType() != SOURCE_DETECTOR) {
        out.close();
    }
    return true;
}


bool
GARDetector::writeRoutes(std::vector<std::string>& saved,
                          OutputDevice& out) {
    if (myRoutes != 0) {
        return myRoutes->save(saved, "", out);
    }
    return false;
}


void
GARDetector::writeSingleSpeedTrigger(const std::string& file,
                                      const GARDetectorFlows& flows,
                                      SUMOTime startTime, SUMOTime endTime,
                                      SUMOTime stepOffset, SUMOReal defaultSpeed) {
    OutputDevice& out = OutputDevice::getDevice(file);
    out.writeXMLHeader("vss");
    const std::vector<FlowDef>& mflows = flows.getFlowDefs(myID);
    unsigned int index = 0;
    for (SUMOTime t = startTime; t < endTime; t += stepOffset, index++) {
        assert(index < mflows.size());
        const FlowDef& srcFD = mflows[index];
        SUMOReal speed = MAX2(srcFD.vLKW, srcFD.vPKW);
        if (speed <= 0 || speed > 250) {
            speed = defaultSpeed;
        } else {
            speed = (SUMOReal)(speed / 3.6);
        }
        out.openTag(SUMO_TAG_STEP).writeAttr(SUMO_ATTR_TIME, time2string(t)).writeAttr(SUMO_ATTR_SPEED, speed).closeTag();
    }
    out.close();
}

/****************************************************************************/
