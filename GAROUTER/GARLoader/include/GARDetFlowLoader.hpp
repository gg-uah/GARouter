/*
 * GARDetFlowLoader.hpp
 *
 *  Created on: Jun 4, 2015
 *      Author: ctomas
 */

#ifndef GARDETFLOWLOADER_HPP_
#define GARDETFLOWLOADER_HPP_

#include <CLogger.hpp>
#include <GARNet.hpp>
#include <GARDetectorFlow.hpp>
#include <utils/options/OptionsCont.h>

using common::CLogger;

namespace gar {

/**
 * @brief This class implements a loader for detector flows.
 * It loads the detector flows from the measurement files designated in the 'measure-files' option.
 * @see GARDetectorFlow
 */
class GARDetFlowLoader {
public:
	/**
	 * Invalidated empty constructor.
	 */
	GARDetFlowLoader() = delete;

	/**
	 * @brief Invalidated copy constructor.
	 * A <code>GARDetFlowLoader</code> object can't be initialized from another object of the same type.
	 * @param other An existing <code>GARDetFlowLoader</code> object to copy in the initialization.
	 */
	GARDetFlowLoader(const GARDetFlowLoader& other) = delete;

	/**
	 * Validated parameterized constructor.
	 * @param options	The ga-router input options.
	 * @param detCont	The detector data container.
	 * @param logger	Reference to the logger.
	 */
	GARDetFlowLoader(const OptionsCont& options,
			 	 	 const GARDetectorCon& detCont,
				     CLogger* logger);

	/**
	 * Default virtual destructor.
	 */
	virtual ~GARDetFlowLoader() = default;

	/**
	 * Invalidated assignment operator
	 */
	GARDetFlowLoader& operator=(const GARDetFlowLoader& rhs) = delete;

	/**
	 * Get the detector flows container.
	 * @return	A pointer to the detector flows container.
	 */
	std::shared_ptr<GARDetectorFlows> getpFlows(void) const;

	/**
	 * Set the pointer to the detector flows container.
	 * @param pFlows	A pointer to the detector flows container.
	 */
	void setpFlows(const std::shared_ptr<GARDetectorFlows>& pFlows);

	/**
	 * @brief Runs the detector flows loader.
	 * It loads the detector flows from the measurement files designated in the 'measure-files' option.
	 * @return	<code>0</code> if the detector flows are successfully loaded,
	 * 			<code>1</code> otherwise.
	 */
	int run(void);

	/**
	 * Logs the detector flows.
	 */
	void logDetectorFlows(void) const;

protected:
	/** @brief Reads the given file assuming it contains detector values
	 *
	 * Reads the first line, first, and parses it assuming it contains
	 *  the names of the columns that follow within the next lines.
	 *
	 * Then, the rest of the file is read and the read values for vehicle/heavy vehicle
	 *  amounts and speeds are stored into "myStorage". Values that lie
	 *  before "myStartTime" and behind "myEndTime" as well as values
	 *  which refer to an unknown detector are omitted.
	 *
	 * @param[in] file 			The name of the file to read
	 * @param[in] startTime		The first time step to read
	 * @param[in] endTime		The last time step to read
	 * @param[in] timeOffset	The time offset to apply to read time values
	 * @param[in] timeScale		The time scale to apply to read time values
	 * @param[in] warnedOverridingBoundaries  Whether a warning about overriding boundaries was already written
	 * @param[in] warnedPartialDefs  Whether a warning about partial definitions was already written
	 * @exception IOError Not yet implemented!
	 * @exception ProcessError Thrown if a value could not be parsed properly or a needed value is missing
	 */
	void read(const std::string& file,
			  SUMOTime startTime,
			  SUMOTime endTime,
			  SUMOTime timeOffset,
			  SUMOTime timeScale,
			  bool& warnedOverridingBoundaries,
			  bool& warnedPartialDefs);

private:
	//! The detector flow measurements
	std::shared_ptr<GARDetectorFlows> pFlows;

	//! The ga-router input options.
	const OptionsCont& options;

	//! The detector data container.
	const GARDetectorCon& detCont;

	//! The ga-router logger.
	CLogger* logger;
};

} /* namespace gar */

#endif /* GARDETFLOWLOADER_HPP_ */
