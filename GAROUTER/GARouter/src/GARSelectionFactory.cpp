/*
 * GARSelectionFactory.cpp
 *
 *  Created on: Jun 9, 2015
 *      Author: ctomas
 */

#include <GARSelectionFactory.hpp>
#include <StringUtilities.hpp>

using common::StringUtilities;

namespace gar {

//................................................. Factory method ...
GASelectionScheme* GARSelectionFactory::createSelectionScheme(const std::string& selection,
														 	  CLogger* logger) {
	try {
		if (StringUtilities::trim(selection) == "Rank") {
			return new GARankSelector();
		}
		if (StringUtilities::trim(selection) == "Roulette") {
			return new GARouletteWheelSelector();
		}
		if (StringUtilities::trim(selection) == "Tournament") {
			return new GATournamentSelector();
		}
		if (StringUtilities::trim(selection) == "Uniform") {
			return new GAUniformSelector();
		}
		if (StringUtilities::trim(selection) == "SRS") {
			return new GASRSSelector();
		}
		if (StringUtilities::trim(selection) == "DS") {
			return new GADSSelector();
		}
	} catch (const std::exception& ex) {
		logger->fatal("Fail to create selection scheme for type [" + selection + "]");
	}

	return nullptr;
}

} /* namespace gar */
