/*
 * GARDetLoader.hpp
 *
 *  Created on: Jun 4, 2015
 *      Author: ctomas
 */

#ifndef GARDETLOADER_HPP_
#define GARDETLOADER_HPP_

#include <CLogger.hpp>
#include <utils/options/OptionsCont.h>
#include <GARNet.hpp>
#include <GARDetectorCon.hpp>

using common::CLogger;

namespace gar {

/**
 * This class implements the detector loader. It loads the detectors in the network
 * from the detector files designated in the 'detector-files' option.
 * @see GARDetectorCon
 * @see GARDetector
 */
class GARDetLoader {
public:
	/**
	 * Invalidated empty constructor.
	 */
	GARDetLoader() = delete;

	/**
	 * @brief Invalidated copy constructor.
	 * A <code>GARDetLoader</code> object can't be initialized from another object of the same type.
	 * @param other An existing <code>GARDetLoader</code> object to copy in the initialization.
	 */
	GARDetLoader(const GARDetLoader& other) = delete;

	/**
	 * Validated parameterized constructor.
	 * @param options	The ga-router input options.
	 * @param net		The network data.
	 * @param logger	Reference to the logger.
	 */
	GARDetLoader(const OptionsCont& options,
				 const GARNet& net,
				 CLogger* logger);

	/**
	 * Default virtual constructor.
	 */
	virtual ~GARDetLoader() = default;

	/**
	 * Get the detector data container.
	 * @return	A pointer to the detector data container.
	 */
	std::shared_ptr<GARDetectorCon> getpDetCont(void) const;

	/**
	 * Set the pointer to the detector data container.
	 * @param pDetCont	A pointer to the detector data container.
	 */
	void setpDetCont(const std::shared_ptr<GARDetectorCon>& pDetCont);

	/**
	 * @brief Runs the detector loader.
	 * The detector data files are read and the data is loaded into a container.
	 * @return	<code>0</code> if the detector data is successfully loaded,
	 * 			<code>1</code> otherwise.
	 */
	int run(void);

	/**
	 * Computes the detector types (source, sink, between) (optionally)
	 * @return	<code>0</code> if the detector types are successfully computed,
	 * 			<code>1</code> otherwise.
	 * @see RODFDetectorType
	 */
	int computeDetectorTypes(void);

	/**
	 * @brief Check the detector data.
	 * It checks whether the detectors have valid types and have routes
	 * @return	<code>0</code> if the detector data is successfully checked,
	 * 			<code>1</code> otherwise.
	 */
	int checkDetectors(void) const;

	/**
	 * @brief Save the detectors into data files.
	 * It saves the detectors with the computed type and the detectors POI (points of interest)
	 * into data files if specified in the "detector-output" and "detectors-poi-output" options.
	 * @return	<code>0</code> if the detectors are successfully saved,
	 * 			<code>1</code> otherwise.
	 */
	int saveDetectors(void);

	/**
	 * Logs the detector data.
	 */
	void logDetectors(void) const;

	/**
	 * Logs the route description data.
	 * @param rd	The route description.
	 */
	void logRouteDesc(const GARRouteDesc& rd) const;

private:
	//! The detector data container.
	std::shared_ptr<GARDetectorCon> pDetCont;

	//! The ga-router input options.
	const OptionsCont& options;

	//! The transport network.
	const GARNet& net;

	//! The ga-router logger.
	CLogger* logger;
};

} /* namespace gar */

#endif /* GARDETLOADER_HPP_ */
