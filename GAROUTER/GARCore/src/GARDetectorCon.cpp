/*
 * GARDetectorCon.cpp
 *
 *  Created on: Apr 30, 2015
 *      Author: ctomas
 */

#include "GARDetectorCon.hpp"
#include <utils/iodevices/OutputDevice.h>
#include <utils/common/StringUtils.h>

GARDetectorCon::GARDetectorCon() {}


GARDetectorCon::~GARDetectorCon() {
    for (std::vector<GARDetector*>::iterator i = myDetectors.begin(); i != myDetectors.end(); ++i) {
        delete *i;
    }
}


bool
GARDetectorCon::addDetector(GARDetector* dfd) {
    if (myDetectorMap.find(dfd->getID()) != myDetectorMap.end()) {
        return false;
    }
    myDetectorMap[dfd->getID()] = dfd;
    myDetectors.push_back(dfd);
    std::string edgeid = dfd->getLaneID().substr(0, dfd->getLaneID().rfind('_'));
    if (myDetectorEdgeMap.find(edgeid) == myDetectorEdgeMap.end()) {
        myDetectorEdgeMap[edgeid] = std::vector<GARDetector*>();
    }
    myDetectorEdgeMap[edgeid].push_back(dfd);
    return true; // !!!
}


bool
GARDetectorCon::detectorsHaveCompleteTypes() const {
    for (std::vector<GARDetector*>::const_iterator i = myDetectors.begin(); i != myDetectors.end(); ++i) {
        if ((*i)->getType() == TYPE_NOT_DEFINED) {
            return false;
        }
    }
    return true;
}


bool
GARDetectorCon::detectorsHaveRoutes() const {
    for (std::vector<GARDetector*>::const_iterator i = myDetectors.begin(); i != myDetectors.end(); ++i) {
        if ((*i)->hasRoutes()) {
            return true;
        }
    }
    return false;
}


const std::vector< GARDetector*>&
GARDetectorCon::getDetectors() const {
    return myDetectors;
}


void
GARDetectorCon::save(const std::string& file) const {
    OutputDevice& out = OutputDevice::getDevice(file);
    out.writeXMLHeader("detectors");
    for (std::vector<GARDetector*>::const_iterator i = myDetectors.begin(); i != myDetectors.end(); ++i) {
        out.openTag(SUMO_TAG_DETECTOR_DEFINITION).writeAttr(SUMO_ATTR_ID, StringUtils::escapeXML((*i)->getID())).writeAttr(SUMO_ATTR_LANE, (*i)->getLaneID()).writeAttr(SUMO_ATTR_POSITION, (*i)->getPos());
        switch ((*i)->getType()) {
            case BETWEEN_DETECTOR:
                out.writeAttr(SUMO_ATTR_TYPE, "between");
                break;
            case SOURCE_DETECTOR:
                out.writeAttr(SUMO_ATTR_TYPE, "source");
                break;
            case SINK_DETECTOR:
                out.writeAttr(SUMO_ATTR_TYPE, "sink");
                break;
            case DISCARDED_DETECTOR:
                out.writeAttr(SUMO_ATTR_TYPE, "discarded");
                break;
            default:
                throw 1;
        }
        out.closeTag();
    }
    out.close();
}


void
GARDetectorCon::saveAsPOIs(const std::string& file) const {
    OutputDevice& out = OutputDevice::getDevice(file);
    out.writeXMLHeader("pois");
    for (std::vector<GARDetector*>::const_iterator i = myDetectors.begin(); i != myDetectors.end(); ++i) {
        out.openTag(SUMO_TAG_POI).writeAttr(SUMO_ATTR_ID, StringUtils::escapeXML((*i)->getID()));
        switch ((*i)->getType()) {
            case BETWEEN_DETECTOR:
                out.writeAttr(SUMO_ATTR_TYPE, "between_detector_position").writeAttr(SUMO_ATTR_COLOR, RGBColor::BLUE);
                break;
            case SOURCE_DETECTOR:
                out.writeAttr(SUMO_ATTR_TYPE, "source_detector_position").writeAttr(SUMO_ATTR_COLOR, RGBColor::GREEN);
                break;
            case SINK_DETECTOR:
                out.writeAttr(SUMO_ATTR_TYPE, "sink_detector_position").writeAttr(SUMO_ATTR_COLOR, RGBColor::RED);
                break;
            case DISCARDED_DETECTOR:
                out.writeAttr(SUMO_ATTR_TYPE, "discarded_detector_position").writeAttr(SUMO_ATTR_COLOR, RGBColor(51, 51, 51, 255));
                break;
            default:
                throw 1;
        }
        out.writeAttr(SUMO_ATTR_LANE, (*i)->getLaneID()).writeAttr(SUMO_ATTR_POSITION, (*i)->getPos()).closeTag();
    }
    out.close();
}


void
GARDetectorCon::saveRoutes(const std::string& file) const {
    OutputDevice& out = OutputDevice::getDevice(file);
    out.writeXMLHeader("routes");
    std::vector<std::string> saved;
    // write for source detectors
    bool lastWasSaved = true;
    for (std::vector<GARDetector*>::const_iterator i = myDetectors.begin(); i != myDetectors.end(); ++i) {
        if ((*i)->getType() != SOURCE_DETECTOR) {
            // do not build routes for other than sources
            continue;
        }
        if (lastWasSaved) {
            out << "\n";
        }
        lastWasSaved = (*i)->writeRoutes(saved, out);
    }
    out << "\n";
    out.close();
}


const GARDetector&
GARDetectorCon::getDetector(const std::string& id) const {
    return *(myDetectorMap.find(id)->second);
}


GARDetector&
GARDetectorCon::getModifiableDetector(const std::string& id) const {
    return *(myDetectorMap.find(id)->second);
}


bool
GARDetectorCon::knows(const std::string& id) const {
    return myDetectorMap.find(id) != myDetectorMap.end();
}


void
GARDetectorCon::writeEmitters(const std::string& file,
                               const GARDetectorFlows& flows,
                               SUMOTime startTime, SUMOTime endTime,
                               SUMOTime stepOffset, const GARNet& net,
                               bool writeCalibrators,
                               bool includeUnusedRoutes,
                               SUMOReal scale,
                               bool insertionsOnly) {
    // compute turn probabilities at detector
    for (std::vector<GARDetector*>::const_iterator i = myDetectors.begin(); i != myDetectors.end(); ++i) {
        (*i)->computeSplitProbabilities(&net, *this, flows, startTime, endTime, stepOffset);
    }
    //
    OutputDevice& out = OutputDevice::getDevice(file);
    out.writeXMLHeader("additional", "xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\" xsi:noNamespaceSchemaLocation=\"http://sumo.dlr.de/xsd/additional_file.xsd\"");
    for (std::vector<GARDetector*>::const_iterator i = myDetectors.begin(); i != myDetectors.end(); ++i) {
        GARDetector* det = *i;
        // get file name for values (emitter/calibrator definition)
        std::string escapedID = StringUtils::escapeXML(det->getID());
        std::string defFileName;
        if (det->getType() == SOURCE_DETECTOR) {
            defFileName = file;
        } else if (writeCalibrators && det->getType() == BETWEEN_DETECTOR) {
            defFileName = FileHelpers::getFilePath(file) + "calibrator_" + escapedID + ".def.xml";
        } else {
            defFileName = FileHelpers::getFilePath(file) + "other_" + escapedID + ".def.xml";
            continue;
        }
        // try to write the definition
        SUMOReal defaultSpeed = net.getEdge(det->getEdgeID())->getSpeed();
        //  ... compute routes' distribution over time
        std::map<size_t, RandomDistributor<size_t>* > dists;
        if (!insertionsOnly && flows.knows(det->getID())) {
            det->buildDestinationDistribution(*this, startTime, endTime, stepOffset, net, dists);
        }
        //  ... write the definition
        if (!det->writeEmitterDefinition(defFileName, dists, flows, startTime, endTime, stepOffset, includeUnusedRoutes, scale, insertionsOnly, defaultSpeed)) {
            // skip if something failed... (!!!)
            continue;
        }
        //  ... clear temporary values
        clearDists(dists);
        // write the declaration into the file
        if (writeCalibrators && det->getType() == BETWEEN_DETECTOR) {
            out.openTag(SUMO_TAG_CALIBRATOR).writeAttr(SUMO_ATTR_ID, "calibrator_" + escapedID).writeAttr(SUMO_ATTR_POSITION, det->getPos());
            out.writeAttr(SUMO_ATTR_LANE, det->getLaneID()).writeAttr(SUMO_ATTR_FRIENDLY_POS, true).writeAttr(SUMO_ATTR_FILE, defFileName).closeTag();
        }
    }
    out.close();
}


void
GARDetectorCon::writeEmitterPOIs(const std::string& file,
                                  const GARDetectorFlows& flows) {
    OutputDevice& out = OutputDevice::getDevice(file);
    out.writeXMLHeader("additional", "xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\" xsi:noNamespaceSchemaLocation=\"http://sumo.dlr.de/xsd/additional_file.xsd\"");
    for (std::vector<GARDetector*>::const_iterator i = myDetectors.begin(); i != myDetectors.end(); ++i) {
        GARDetector* det = *i;
        SUMOReal flow = flows.getFlowSumSecure(det->getID());
        const unsigned char col = static_cast<unsigned char>(128 * flow / flows.getMaxDetectorFlow() + 128);
        out.openTag(SUMO_TAG_POI).writeAttr(SUMO_ATTR_ID, StringUtils::escapeXML((*i)->getID()) + ":" + toString(flow));
        switch ((*i)->getType()) {
            case BETWEEN_DETECTOR:
                out.writeAttr(SUMO_ATTR_TYPE, "between_detector_position").writeAttr(SUMO_ATTR_COLOR, RGBColor(0, 0, col, 255));
                break;
            case SOURCE_DETECTOR:
                out.writeAttr(SUMO_ATTR_TYPE, "source_detector_position").writeAttr(SUMO_ATTR_COLOR, RGBColor(0, col, 0, 255));
                break;
            case SINK_DETECTOR:
                out.writeAttr(SUMO_ATTR_TYPE, "sink_detector_position").writeAttr(SUMO_ATTR_COLOR, RGBColor(col, 0, 0, 255));
                break;
            case DISCARDED_DETECTOR:
                out.writeAttr(SUMO_ATTR_TYPE, "discarded_detector_position").writeAttr(SUMO_ATTR_COLOR, RGBColor(51, 51, 51, 255));
                break;
            default:
                throw 1;
        }
        out.writeAttr(SUMO_ATTR_LANE, (*i)->getLaneID()).writeAttr(SUMO_ATTR_POSITION, (*i)->getPos()).closeTag();
    }
    out.close();
}


int
GARDetectorCon::getAggFlowFor(const ROEdge* edge, SUMOTime time, SUMOTime period,
                               const GARDetectorFlows&) const {
    UNUSED_PARAMETER(period);
    UNUSED_PARAMETER(time);
    if (edge == 0) {
        return 0;
    }
//    SUMOReal stepOffset = 60; // !!!
//    SUMOReal startTime = 0; // !!!
//    cout << edge->getID() << endl;
    assert(myDetectorEdgeMap.find(edge->getID()) != myDetectorEdgeMap.end());
    const std::vector<FlowDef>& flows = static_cast<const GAREdge*>(edge)->getFlows();
    SUMOReal agg = 0;
    for (std::vector<FlowDef>::const_iterator i = flows.begin(); i != flows.end(); ++i) {
        const FlowDef& srcFD = *i;
        if (srcFD.qLKW >= 0) {
            agg += srcFD.qLKW;
        }
        if (srcFD.qPKW >= 0) {
            agg += srcFD.qPKW;
        }
    }
    return (int) agg;
    /* !!! make this time variable
    if (flows.size()!=0) {
        SUMOReal agg = 0;
        size_t beginIndex = (int)((time/stepOffset) - startTime);  // !!! falsch!!!
        for (SUMOTime t=0; t<period&&beginIndex<flows.size(); t+=(SUMOTime) stepOffset) {
            const FlowDef &srcFD = flows[beginIndex++];
            if (srcFD.qLKW>=0) {
                agg += srcFD.qLKW;
            }
            if (srcFD.qPKW>=0) {
                agg += srcFD.qPKW;
            }
        }
        return (int) agg;
    }
    */
//    return -1;
}


void
GARDetectorCon::writeSpeedTrigger(const GARNet* const net,
                                   const std::string& file,
                                   const GARDetectorFlows& flows,
                                   SUMOTime startTime, SUMOTime endTime,
                                   SUMOTime stepOffset) {
    OutputDevice& out = OutputDevice::getDevice(file);
    out.writeXMLHeader("additional", "xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\" xsi:noNamespaceSchemaLocation=\"http://sumo.dlr.de/xsd/additional_file.xsd\"");
    for (std::vector<GARDetector*>::const_iterator i = myDetectors.begin(); i != myDetectors.end(); ++i) {
        GARDetector* det = *i;
        // write the declaration into the file
        if (det->getType() == SINK_DETECTOR && flows.knows(det->getID())) {
            std::string filename = FileHelpers::getFilePath(file) + "vss_" + det->getID() + ".def.xml";
            out.openTag(SUMO_TAG_VSS).writeAttr(SUMO_ATTR_ID, StringUtils::escapeXML(det->getID())).writeAttr(SUMO_ATTR_LANES, det->getLaneID()).writeAttr(SUMO_ATTR_FILE, filename).closeTag();
            SUMOReal defaultSpeed = net != 0 ? net->getEdge(det->getEdgeID())->getSpeed() : (SUMOReal) 200.;
            det->writeSingleSpeedTrigger(filename, flows, startTime, endTime, stepOffset, defaultSpeed);
        }
    }
    out.close();
}


void
GARDetectorCon::writeEndRerouterDetectors(const std::string& file) {
    OutputDevice& out = OutputDevice::getDevice(file);
    out.writeXMLHeader("additional", "xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\" xsi:noNamespaceSchemaLocation=\"http://sumo.dlr.de/xsd/additional_file.xsd\"");
    for (std::vector<GARDetector*>::const_iterator i = myDetectors.begin(); i != myDetectors.end(); ++i) {
        GARDetector* det = *i;
        // write the declaration into the file
        if (det->getType() == SINK_DETECTOR) {
            out.openTag(SUMO_TAG_REROUTER).writeAttr(SUMO_ATTR_ID, "endrerouter_" + StringUtils::escapeXML(det->getID())).writeAttr(SUMO_ATTR_EDGES, det->getLaneID());
            out.writeAttr(SUMO_ATTR_POSITION, SUMOReal(0)).writeAttr(SUMO_ATTR_FILE, "endrerouter_" + det->getID() + ".def.xml").closeTag();
        }
    }
    out.close();
}


void
GARDetectorCon::writeValidationDetectors(const std::string& file,
        bool includeSources,
        bool singleFile, bool friendly) {
    OutputDevice& out = OutputDevice::getDevice(file);
    out.writeXMLHeader("additional");
    for (std::vector<GARDetector*>::const_iterator i = myDetectors.begin(); i != myDetectors.end(); ++i) {
        GARDetector* det = *i;
        // write the declaration into the file
        if (det->getType() != SOURCE_DETECTOR || includeSources) {
            SUMOReal pos = det->getPos();
            if (det->getType() == SOURCE_DETECTOR) {
                pos += 1;
            }
            out.openTag(SUMO_TAG_E1DETECTOR).writeAttr(SUMO_ATTR_ID, "validation_" + StringUtils::escapeXML(det->getID())).writeAttr(SUMO_ATTR_LANE, det->getLaneID());
            out.writeAttr(SUMO_ATTR_POSITION, pos).writeAttr(SUMO_ATTR_FREQUENCY, 60);
            if (friendly) {
                out.writeAttr(SUMO_ATTR_FRIENDLY_POS, true);
            }
            if (!singleFile) {
                out.writeAttr(SUMO_ATTR_FILE, "validation_det_" + StringUtils::escapeXML(det->getID()) + ".xml");
            } else {
                out.writeAttr(SUMO_ATTR_FILE, "validation_dets.xml");
            }
            out.closeTag();
        }
    }
    out.close();
}


void
GARDetectorCon::removeDetector(const std::string& id) {
    //
    std::map<std::string, GARDetector*>::iterator ri1 = myDetectorMap.find(id);
    GARDetector* oldDet = (*ri1).second;
    myDetectorMap.erase(ri1);
    //
    std::vector<GARDetector*>::iterator ri2 =
        find(myDetectors.begin(), myDetectors.end(), oldDet);
    myDetectors.erase(ri2);
    //
    bool found = false;
    for (std::map<std::string, std::vector<GARDetector*> >::iterator rr3 = myDetectorEdgeMap.begin(); !found && rr3 != myDetectorEdgeMap.end(); ++rr3) {
        std::vector<GARDetector*>& dets = (*rr3).second;
        for (std::vector<GARDetector*>::iterator ri3 = dets.begin(); !found && ri3 != dets.end();) {
            if (*ri3 == oldDet) {
                found = true;
                ri3 = dets.erase(ri3);
            } else {
                ++ri3;
            }
        }
    }
    delete oldDet;
}


void
GARDetectorCon::guessEmptyFlows(GARDetectorFlows& flows) {
    // routes must be built (we have ensured this in main)
    // detector followers/prior must be build (we have ensured this in main)
    //
    for (std::vector<GARDetector*>::const_iterator i = myDetectors.begin(); i != myDetectors.end(); ++i) {
        GARDetector* det = *i;
        const std::set<const GARDetector*>& prior = det->getPriorDetectors();
        const std::set<const GARDetector*>& follower = det->getFollowerDetectors();
        size_t noFollowerWithRoutes = 0;
        size_t noPriorWithRoutes = 0;
        // count occurences of detectors with/without routes
        std::set<const GARDetector*>::const_iterator j;
        for (j = prior.begin(); j != prior.end(); ++j) {
            if (flows.knows((*j)->getID())) {
                ++noPriorWithRoutes;
            }
        }
        for (j = follower.begin(); j != follower.end(); ++j) {
            if (flows.knows((*j)->getID())) {
                ++noFollowerWithRoutes;
            }
        }

        // do not process detectors which have no routes
        if (!flows.knows(det->getID())) {
            continue;
        }

        // plain case: all of the prior detectors have routes
        if (noPriorWithRoutes == prior.size()) {
            // the number of vehicles is the sum of all vehicles on prior
            continue;
        }

        // plain case: all of the follower detectors have routes
        if (noFollowerWithRoutes == follower.size()) {
            // the number of vehicles is the sum of all vehicles on follower
            continue;
        }

    }
}


const GARDetector&
GARDetectorCon::getAnyDetectorForEdge(const GAREdge* const edge) const {
    for (std::vector<GARDetector*>::const_iterator i = myDetectors.begin(); i != myDetectors.end(); ++i) {
        if ((*i)->getEdgeID() == edge->getID()) {
            return **i;
        }
    }
    throw 1;
}


void
GARDetectorCon::clearDists(std::map<size_t, RandomDistributor<size_t>* >& dists) const {
    for (std::map<size_t, RandomDistributor<size_t>* >::iterator i = dists.begin(); i != dists.end(); ++i) {
        delete(*i).second;
    }
}


void
GARDetectorCon::mesoJoin(const std::string& nid,
                         const std::vector<std::string>& oldids) {
    // build the new detector
    const GARDetector& first = getDetector(*(oldids.begin()));
    GARDetector* newDet = new GARDetector(nid, first);
    addDetector(newDet);
    // delete previous
    for (std::vector<std::string>::const_iterator i = oldids.begin(); i != oldids.end(); ++i) {
        removeDetector(*i);
    }
}


std::vector<GARDetector*>
GARDetectorCon::getDetectorsByType(const GARDetectorType& type) const {
	std::vector<GARDetector*> detectors;
	auto searchByType = [&type, &detectors] (GARDetector* pDet) mutable {
			if (pDet->getType() == type) {
				detectors.push_back(pDet);
			}
	};

	std::for_each(myDetectors.begin(), myDetectors.end(), searchByType);

	return detectors;
}

/*
const GARDetector const*
GARDetectorCon::getDetectorOnEdge(const std::string& edgeId) const {
	auto it = myDetectorEdgeMap.find(edgeId);

	if (it == myDetectorEdgeMap.end()) {
		return nullptr;
	}

	return it->second;
}
*/

std::vector<GARDetector*>
GARDetectorCon::getEdgeDetectors(const std::string& edgeId) const {
	std::map<std::string, std::vector<GARDetector*>>::const_iterator it;

	it = myDetectorEdgeMap.find(edgeId);
	if (it == myDetectorEdgeMap.end()) {
		return std::vector<GARDetector*>();
	}

	return it->second;
}
