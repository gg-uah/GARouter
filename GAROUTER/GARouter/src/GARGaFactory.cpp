/*
 * GARGaFactory.cpp
 *
 *  Created on: Jun 7, 2015
 *      Author: ctomas
 */

#include <GARGaFactory.hpp>
#include <StringUtilities.hpp>
#include <ga/GASimpleGA.h>
#include <ga/GASStateGA.h>
#include <ga/GAIncGA.h>
#include <ga/GADemeGA.h>

using common::StringUtilities;

namespace gar {

//................................................. Factory method ...
GAGeneticAlgorithm* GARGaFactory::createGeneticAlgorithm(const std::string& gaType,
														 const GAGenome const* pGenome,
														 GAParameterList& params,
														 CLogger* logger) {
	try {
		if (StringUtilities::trim(gaType) == "Simple") {
			GASimpleGA::registerDefaultParameters(params);
			return new GASimpleGA(*pGenome);
		}
		if (StringUtilities::trim(gaType) == "SteadyState") {
			GASteadyStateGA::registerDefaultParameters(params);
			return new GASteadyStateGA(*pGenome);
		}
		if (StringUtilities::trim(gaType) == "Incremental") {
			GAIncrementalGA::registerDefaultParameters(params);
			return new GAIncrementalGA(*pGenome);
		}
		if (StringUtilities::trim(gaType) == "Deme") {
			GADemeGA::registerDefaultParameters(params);
			return new GADemeGA(*pGenome);
		}
	} catch (const std::exception& ex) {
		logger->fatal("Fail to create genetic algorithm for type [" + gaType + "]");
	}

	return nullptr;
}

} /* namespace gar */
