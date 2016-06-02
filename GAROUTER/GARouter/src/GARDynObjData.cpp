/*
 * GARDynObjData.cpp
 *
 *  Created on: May 11, 2015
 *      Author: ctomas
 */

#include "GARDynObjData.hpp"

using std::string;
using std::vector;

namespace gar {

//................................................. Parameterized constructor ...
GARDynObjData::GARDynObjData(CLogger* logger)
: netFile  (""),
  flowFile (""),
  rouFile  (""),
  emitFile (""),
  loopFile (""),
  begin	   (0),
  end	   (86400),
  offset   (0),
  timeStep (60),
  timeToTeleport (-1),
  pNet 	   (nullptr),
  pRouCont (nullptr),
  pDetCont (nullptr),
  loops2Detectors(gar::loop2det_map()),
  pDetFlows(nullptr),
  routesSolution (vector<string>()),
  logger   (logger) {
	// Intentionally left empty
}

//................................................. Get the network file name ...
std::string GARDynObjData::getNetFile(void) const {
	return this->netFile;
}

//................................................. Get the route file name ...
std::string GARDynObjData::getRouFile(void) const {
	return this->rouFile;
}

//................................................. Get the detector flow file name ...
std::string GARDynObjData::getFlowFile(void) const {
	return this->flowFile;
}

//................................................. Get the emitter file name ...
std::string GARDynObjData::getEmitFile(void) const {
	return this->emitFile;
}

//................................................. Get the induction loop file name ...
std::string GARDynObjData::getLoopFile(void) const {
	return this->loopFile;
}

//................................................. Get the beginning time ...
SUMOTime GARDynObjData::getBegin(void) const {
	return this->begin;
}

//................................................. Get the end time ...
SUMOTime GARDynObjData::getEnd(void) const {
	return this->end;
}

//................................................. Get the time step ...
SUMOTime GARDynObjData::getTimeStep(void) const {
	return this->timeStep;
}

//................................................. Get the time offset ...
SUMOTime GARDynObjData::getOffset(void) const {
	return this->offset;
}

//................................................. Get the time to teleport ...
SUMOTime GARDynObjData::getTimeToTeleport(void) const {
	return this->timeToTeleport;
}

//................................................. Get the network data ...
const GARNet const* GARDynObjData::getpNet(void) const {
	return this->pNet;
}

//................................................. Get the routes container ...
const GARRouteCont const * GARDynObjData::getpRouCont(void) const {
	return this->pRouCont;
}

//................................................. Get the detectors container ...
const GARDetectorCon const* GARDynObjData::getpDetCont(void) const {
	return this->pDetCont;
}

//................................................. Get the induction loop to detector id's map ...
gar::loop2det_map GARDynObjData::getLoops2Detectors(void) const {
	return this->loops2Detectors;
}

//................................................. Get the detectors flow measurements...
const GARDetectorFlows const* GARDynObjData::getpDetFlows(void) const {
	return this->pDetFlows;
}

//................................................. Get the static solution ...
const vector<string>& GARDynObjData::getRoutesSolution(void) const {
	return this->routesSolution;
}

//................................................. Set the network file name ...
void GARDynObjData::setNetFile(const std::string& netFile) {
	this->netFile = netFile;
}

//................................................. Set the route file name ...
void GARDynObjData::setRouFile(const std::string& rouFile) {
	this->rouFile = rouFile;
}

//................................................. Set the detector flow file name ...
void GARDynObjData::setFlowFile(const std::string& flowFile) {
	this->flowFile = flowFile;
}

//................................................. Set the emitter file name ...
void GARDynObjData::setEmitFile(const std::string& emitFile) {
	this->emitFile = emitFile;
}

//................................................. Set the induction loop file name ...
void GARDynObjData::setLoopFile(const std::string& loopFile) {
	this->loopFile = loopFile;
}

//................................................. Set the beginning time ...
void GARDynObjData::setBegin(const SUMOTime& begin) {
	this->begin = begin;
}

//................................................. Set the end time ...
void GARDynObjData::setEnd(const SUMOTime& end) {
	this->end = end;
}

//................................................. Set the offset time ...
void GARDynObjData::setOffset(const SUMOTime& offset) {
	this->offset = offset;
}

//................................................. Set the time step ...
void GARDynObjData::setTimeStep(const SUMOTime& timeStep) {
	this->timeStep = timeStep;
}

//................................................. Set the time to teleport ...
void GARDynObjData::setTimeToTeleport(const SUMOTime& timeToTeleport) {
	this->timeToTeleport = timeToTeleport;
}

//................................................. Set the network data ...
void GARDynObjData::setpNet(const GARNet const* pNet) {
	this->pNet = pNet;
}

//................................................. Set the routes container ...
void GARDynObjData::setpRouCont(const GARRouteCont const* pRouCont) {
	this->pRouCont = pRouCont;
}

//................................................. Set the detectors container ...
void GARDynObjData::setpDetCont(const GARDetectorCon const* pDetCont) {
	this->pDetCont = pDetCont;
}

//................................................. Set the induction loops to detector id's map ...
void GARDynObjData::setLoops2Detectors(const gar::loop2det_map& loops2Dets) {
	this->loops2Detectors = loops2Dets;
}

//................................................. Set the detector flow data ...
void GARDynObjData::setpDetFlows(const GARDetectorFlows const* pDetFlows) {
	this->pDetFlows = pDetFlows;
}

//................................................. Set the solution genome from the static GA ...
void GARDynObjData::setRoutesSolution(const gar::genome_static& genomeSolution) {
	for (int i = 0; i < genomeSolution.size(); i++) {
		string routeName = string(genomeSolution.gene(i));
		routesSolution.push_back(routeName);
	}
}

//................................................. Add an entry in the loop2det map ...
void GARDynObjData::addLoop2Detector(const std::string& loopId, const GARDetector const* pDet) {
	if (pDet == nullptr) {
		logger->error("Fail to insert induction loop to detector data map entry: "
				"Null pointer to detector data for induction loop [" + loopId + "]");
		return;
	}

	std::pair<std::string, const GARDetector const*> loop2detPair(loopId, pDet);

	auto ret = this->loops2Detectors.insert(loop2detPair);
	if (ret.second == false) {
		logger->error("Fail to insert map entry from induction loop [" + loopId +
				"] to detector data [" + pDet->getID() + "]");
	}
}


} /* namespace gar */
