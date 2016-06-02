/*
 * GARSelectionFactory.hpp
 *
 *  Created on: Jun 9, 2015
 *      Author: ctomas
 */

#ifndef GARSELECTIONFACTORY_HPP_
#define GARSELECTIONFACTORY_HPP_

#include <CLogger.hpp>
#include <ga/ga.h>

using common::CLogger;

namespace gar {

/**
 * This class implements the factory method pattern to create the suitable
 * selection object determined by the 'selection-scheme' input option.
 */
class GARSelectionFactory {
public:
	/**
	 * Default constructor.
	 */
	GARSelectionFactory() = default;

	/**
	 * Default virtual constructor.
	 */
	virtual ~GARSelectionFactory() = default;

	/**
	 * @brief The selection scheme factory method.
	 * Creates an instance of the suitable selector class according to the given
	 * selection scheme type argument.
	 * @param selection	 The selection scheme.
	 * @param logger	 Reference to the ga-router logger.
	 * @return			 A pointer to the suitable selector object that inherits from the
	 * 					 GASelectionScheme virtual class.
	 */
	GASelectionScheme* createSelectionScheme(const std::string& selection,
										     CLogger* logger);

};

} /* namespace gar */

#endif /* GARSELECTIONFACTORY_HPP_ */
