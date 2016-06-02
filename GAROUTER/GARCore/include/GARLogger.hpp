/*
 * GARLogger.hpp
 *
 *  Created on: Apr 21, 2015
 *      Author: ctomas
 */

#ifndef __GARLOGGER_HPP_
#define __GARLOGGER_HPP_

#include <CLogger.hpp>

using common::CLogger;

namespace gar {

/**
 * @class GARLogger
 * This class implements the logger singleton used to dump information from the GARouter process
 * into a log file.
 * The log configuration (log level, dump configuration) is described in a configuration file
 * usually located in a 'config' directory.
 * Alongside to the log configuration, a dump configuration has to be provided to settle the log file
 * path, prefix and suffix.
 */
class GARLogger {
public:
    //!...................................................... Constructors and destructor ...
    /**
     * Default constructor
     */
	GARLogger(void);

	/**
	 * Parameterized constructor
	 * @param[in] logId			The log configuration identifier in the log configuration file
	 * @param[in] logConfig		The log configuration path file name
	 * @param[in] dumpConfig	The log configuration path file name
	 */
	GARLogger(const std::string& logId,
			  const std::string& logConfig,
			  const std::string& dumpConfig);

	/**
	 * Virtual destructor.
	 * Deallocates the memory block pointed by the {@link gar.CLogger#logger} singleton
	 */
	virtual ~GARLogger(void);

	/**
	 * Getter method for the {@link gar.CLogger#logger} attribute
	 */
	CLogger* getLogger(void) const;

private:
	//! A singleton for dumping information into a file or the standard output
	CLogger* logger;
};

} /* namespace gar */

#endif /* __GARLOGGER_HPP_ */
