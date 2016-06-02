/*
 * GARObjData.hpp
 *
 *  Created on: May 11, 2015
 *      Author: ctomas
 */

#ifndef GAROBJDATA_HPP_
#define GAROBJDATA_HPP_

#include <iostream>
#include <CLogger.hpp>
#include <GARNet.hpp>
#include <GARRouteDesc.hpp>
#include <GARDetector.hpp>
#include <GARDetectorFlow.hpp>
#include <GARRouteCont.hpp>
#include <GARDetectorCon.hpp>
#include <utils/common/SUMOTime.h>

using common::CLogger;

namespace gar {

/**
 * @brief This class implements the data required in the objective function of the genome.
 * This is the data required to perform a SUMO simulation.
 * The following data is enclosed:
 * - The network file name
 * - The detector flow file name
 * - The route file name
 * - The emitters file name
 * - The induction-loop file name
 * - The begin time in seconds
 * - The end time in seconds
 * - The route description data set
 */
class GARObjData {
public:
	/**
	 * Empty constructor.
	 */
	GARObjData() = delete;

	/**
	 * Parameterized constructor.
	 * @param logger	Reference to the ga-router logger.
	 */
	GARObjData(CLogger* logger);

	/**
	 * Default virtual constructor.
	 */
	virtual ~GARObjData() = default;

	/**
	 * Get the network file name.
	 * @return	The network file name.
	 */
	std::string getNetFile(void) const;

	/**
	 * Get the route file name.
	 * @return	The route file name.
	 */
	std::string getRouFile(void) const;

	/**
	 * Get the detector flow file name.
	 * @return	The detector flow file name.
	 */
	std::string getFlowFile(void) const;

	/**
	 * Get the emitter file name.
	 * @return	The emitter file name.
	 */
	std::string getEmitFile(void) const;

	/**
	 * Get the induction-loop file name.
	 * @return	The induction-loop file name.
	 */
	std::string getLoopFile(void) const;

	/**
	 * Get the simulation beginning time in seconds.
	 * @return	The beginning time in seconds.
	 */
	SUMOTime getBegin(void) const;

	/**
	 * Get the simulation end time in seconds.
	 * @return	The end time in seconds.
	 */
	SUMOTime getEnd(void) const;

	/**
	 * Get the time interval in seconds between two successive flow data sets.
	 * @return	The time interval in seconds between tow successive
	 * 			flow data sets.
	 */
	SUMOTime getTimeStep(void) const;

	/**
	 * Get the time to teleport in the SUMO simulation.
	 * @return	The time to teleport in seconds.
	 */
	SUMOTime getTimeToTeleport(void) const;

	/**
	 * Get the network data;
	 * @return	A pointer to the network data.
	 */
	const GARNet const* getpNet(void) const;

	/**
	 * Get the routes container.
	 * @return	The routes container.
	 */
	const GARRouteCont const * getpRouCont(void) const;

	/**
	 * Get the detectors container.
	 * @return	The detectors container.
	 */
	const GARDetectorCon const * getpDetCont(void) const;

	/**
	 * Get the induction loop to detector data map.
	 * @return	The induction loop to detector data map.
	 */
	gar::loop2det_map getLoops2Detectors(void) const;

	/**
	 * Get the pointer to the detector flow data.
	 * @return	The pointer to the detector flow data.
	 */
	const GARDetectorFlows const* getpDetFlows(void) const;

	/**
	 * Set the network file name.
	 * @param netFile	The network file name.
	 */
	void setNetFile(const std::string& netFile);

	/**
	 * Set the route file name.
	 * @param rouFile	The route file name.
	 */
	void setRouFile(const std::string& rouFile);

	/**
	 * Set the detector flow file name.
	 * @param flowFile	The detector flow file name.
	 */
	void setFlowFile(const std::string& flowFile);

	/**
	 * Set the emitter file name.
	 * @param emitFile	The emitter file name.
	 */
	void setEmitFile(const std::string& emitFile);

	/**
	 * Set the induction-loop file name.
	 * @param loopFile	The induction-loop file name.
	 */
	void setLoopFile(const std::string& loopFile);

	/**
	 * Set the simulation beginning time in seconds.
	 * @param begin	The beginning time in seconds.
	 */
	void setBegin(const SUMOTime& begin);

	/**
	 * Set the simulation end time in seconds.
	 * @param end	The end time in seconds.
	 */
	void setEnd(const SUMOTime& end);

	/**
	 * Set the time interval in seconds between two successive flow data sets.
	 * @param timeStep	The time in seconds between two successive flow
	 * 					data sets.
	 */
	void setTimeStep(const SUMOTime& timeStep);

	/**
	 * Set the time to teleport in the SUMO simulation.
	 * @param timeToTeleport	The time to teleport in seconds.
	 */
	void setTimeToTeleport(const SUMOTime& timeToTeleport);

	/**
	 * Set the network data;
	 * @param pNet	A pointer to the network data.
	 */
	void setpNet(const GARNet const* pNet);

	/**
	 * Set the routes container.
	 * @param pRouCont	A pointer to the routes container.
	 */
	void setpRouCont(const GARRouteCont const* pRouCont);

	/**
	 * Set the detectors container.
	 * @param pDetCont	A pointer to the detectors container.
	 */
	void setpDetCont(const GARDetectorCon const* pDetCont);

	/**
	 * Set the induction loop to detector data map.
	 * @param loops2Dets	The induction loop to detector data map.
	 */
	void setLoops2Detectors(const gar::loop2det_map& loops2Dets);

	/**
	 * Set the pointer to the detector flow data.
	 * @param pDetFlows	A pointer to the detector flow data.
	 */
	void setpDetFlows(const GARDetectorFlows const* pDetFlows);

	/**
	 * Add an entry that maps an induction loop identifier to a detector data.
	 * @param loopId	The induction loop identifier.
	 * @param pDet		A pointer to the detector data.
	 */
	void addLoop2Detector(const std::string& loopId, const GARDetector const* pDet);

private:
	//! The network file name (input)
	std::string netFile;

	//! The detector flow measurement file (input)
	std::string flowFile;

	//! The route file name (output)
	std::string rouFile;

	//! The emitter file name (output)
	std::string emitFile;

	//! The induction-loop file name (output)
	std::string loopFile;

	//! The simulation begin time in seconds
	SUMOTime begin;

	//! The simulation end time in seconds
	SUMOTime end;

	//! The time interval between two successive flow data sets
	SUMOTime timeStep;

	//! How long a vehicle may wait until being teleported
	SUMOTime timeToTeleport;

	//! The network data
	const GARNet const* pNet;

	//! The route container
	const GARRouteCont const* pRouCont;

	//! The detector container
	const GARDetectorCon const* pDetCont;

	//! The induction loop to detector data mapping
	gar::loop2det_map loops2Detectors;

	//! The detector flow measurements
	const GARDetectorFlows const* pDetFlows;

	//! The application logger
	CLogger* logger;

};

} /* namespace gar */

#endif /* GAROBJDATA_HPP_ */
