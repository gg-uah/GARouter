/*
 * GARStaticObjData.hpp
 *
 *  Created on: May 11, 2015
 *      Author: ctomas
 */

#ifndef GARSTATICOBJDATA_HPP_
#define GARSTATICOBJDATA_HPP_

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
 * @brief This class implements the data required in the objective function of the genome for the static approach.
 * This is the data required to compute the flow amounts in the detectors.
 * The following data is enclosed:
 * - The network file name
 * - The detector flow file name
 * - The route file name
 * - The emitters file name
 * - The induction-loop file name
 * - The route description data set
 */
class GARStaticObjData {
public:
	/**
	 * Empty constructor.
	 */
	GARStaticObjData() = delete;

	/**
	 * Parameterized constructor.
	 * @param logger	Reference to the ga-router logger.
	 */
	GARStaticObjData(CLogger* logger);

	/**
	 * Default virtual constructor.
	 */
	virtual ~GARStaticObjData() = default;

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
	 * Get the pointer to the detector flow data.
	 * @return	The pointer to the detector flow data.
	 */
	const GARDetectorFlows const* getpDetFlows(void) const;

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
	 * Set the pointer to the detector flow data.
	 * @param pDetFlows	A pointer to the detector flow data.
	 */
	void setpDetFlows(const GARDetectorFlows const* pDetFlows);

private:
	//! The network data
	const GARNet const* pNet;

	//! The route container
	const GARRouteCont const* pRouCont;

	//! The detector container
	const GARDetectorCon const* pDetCont;

	//! The detector flow measurements
	const GARDetectorFlows const* pDetFlows;

	//! The application logger
	CLogger* logger;

};

} /* namespace gar */

#endif /* GARSTATICOBJDATA_HPP_ */
