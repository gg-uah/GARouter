/*
 * GARStaticObjData.cpp
 *
 *  Created on: May 11, 2015
 *      Author: ctomas
 */

#include "GARStaticObjData.hpp"

using std::string;
using std::vector;

namespace gar {

//................................................. Parameterized constructor ...
GARStaticObjData::GARStaticObjData(CLogger* logger)
: pNet 	   (nullptr),
  pRouCont (nullptr),
  pDetCont (nullptr),
  pDetFlows(nullptr),
  logger   (logger) {
	// Intentionally left empty
}

//................................................. Get the network data ...
const GARNet const* GARStaticObjData::getpNet(void) const {
	return this->pNet;
}

//................................................. Get the routes container ...
const GARRouteCont const * GARStaticObjData::getpRouCont(void) const {
	return this->pRouCont;
}

//................................................. Get the detectors container ...
const GARDetectorCon const* GARStaticObjData::getpDetCont(void) const {
	return this->pDetCont;
}

//................................................. Get the detectors flow measurements...
const GARDetectorFlows const* GARStaticObjData::getpDetFlows(void) const {
	return this->pDetFlows;
}

//................................................. Set the network data ...
void GARStaticObjData::setpNet(const GARNet const* pNet) {
	this->pNet = pNet;
}

//................................................. Set the routes container ...
void GARStaticObjData::setpRouCont(const GARRouteCont const* pRouCont) {
	this->pRouCont = pRouCont;
}

//................................................. Set the detectors container ...
void GARStaticObjData::setpDetCont(const GARDetectorCon const* pDetCont) {
	this->pDetCont = pDetCont;
}

//................................................. Set the detector flow data ...
void GARStaticObjData::setpDetFlows(const GARDetectorFlows const* pDetFlows) {
	this->pDetFlows = pDetFlows;
}

} /* namespace gar */
