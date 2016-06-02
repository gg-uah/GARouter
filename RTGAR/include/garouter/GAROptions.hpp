/*
 * GAROptions.h
 *
 *  Created on: Apr 24, 2015
 *      Author: ctomas
 */

#ifndef GAROPTIONS_HPP_
#define GAROPTIONS_HPP_

#include <CLogger.hpp>
#include <utils/options/OptionsCont.h>

using common::CLogger;

namespace gar {

/**
 * @class GAROptions
 * This class implements the options used to configure the ga-router
 * when it is created.
 * The options can be set either through the command line or by means
 * of a configuration XML file.
 */
class GAROptions {
public:
	/**
	 * Default constructor.
	 */
	GAROptions(void);

	/**
	 * Default virtual constructor.
	 */
	virtual ~GAROptions(void) = default;

	/**
	 * Initializes the ga-router configuration options.
	 * @param argc	  The number of command line arguments.
	 * @param argv	  An array containing the command line arguments.
	 * @param logger  Reference to the application logger.
	 * @return		  <code>0</code> if the ga-router options has successfully been initiated
	 * 				  <code>1</code> otherwise.
	 */
	int init(const int& argc, char ** argv, CLogger* logger);

	/**
	 * Returns a reference to the ga-router configuration options.
	 * @return		A reference to the ga-router configuration options.
	 */
	const OptionsCont& getOptions(void) const;

	/**
	 * Log the ga-router configuration options.
	 * @param logger	Reference to the application logger.
	 */
	void logOptions(CLogger* logger) const;

private:
	//! A reference to the options container
	OptionsCont& optCont;
};

} /* namespace gar */

#endif /* GAROPTIONS_HPP_ */
