/*
 * GARNetLoader.hpp
 *
 *  Created on: May 25, 2015
 *      Author: ctomas
 */

#ifndef GARNETLOADER_HPP_
#define GARNETLOADER_HPP_

#include <GARNet.hpp>
#include <CLogger.hpp>
#include <utils/options/OptionsCont.h>

using common::CLogger;

namespace gar {

/**
 * This class loads the network data (edges, nodes).
 */
class GARNetLoader {
public:
	/**
	 * Invalidated default constructor.
	 */
	GARNetLoader(void) = delete;

	/**
	 * @brief Invalidated copy constructor.
	 * A <code>GARNetLoader</code> object can't be initialized from another object of the same type.
	 * @param other An existing <code>GARNetLoader</code> object to copy in the initialization.
	 */
	GARNetLoader(const GARNetLoader& other) = delete;

	/**
	 * Parameterized constructor.
	 * @param options	The ga-router input options.
	 * @param logger	Reference to the ga-router logger.
	 */
	GARNetLoader(const OptionsCont& options, CLogger *logger);

	/**
	 * Default virtual destructor.
	 */
	virtual ~GARNetLoader(void) = default;

	/**
	 * Get the pointer to the network data.
	 * @return	A pointer to the network data.
	 */
	std::shared_ptr<GARNet> getpNet(void) const;

	/**
	 * Set the pointer to the network data.
	 * @param pNet	A pointer to the network data.
	 */
	void setpNet(const std::shared_ptr<GARNet>& pNet);

	/**
	 * Load the network data from the network file designated in the 'net-file' option.
	 * @return  <code>0</code> in case of successful network loading,
	 * 		 	<code>1</code> otherwise.
	 * @see GARNet
	 */
	int run(void);

	/**
	 * Log the network data.
	 */
	void logNet(void) const;

private:
	//! A pointer to the network data (edges, nodes)
	std::shared_ptr<GARNet> pNet;

	//! The ga-router input options
	const OptionsCont& options;

	//! Reference to the ga-router logger
	CLogger* logger;
};

} /* namespace gar */

#endif /* GARNETLOADER_HPP_ */
