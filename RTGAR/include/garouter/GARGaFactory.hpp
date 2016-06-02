/*
 * GARGaFactory.hpp
 *
 *  Created on: Jun 7, 2015
 *      Author: ctomas
 */

#ifndef GARGAFACTORY_HPP_
#define GARGAFACTORY_HPP_

#include <CLogger.hpp>
#include <ga/GABaseGA.h>
#include <ga/GA1DArrayGenome.h>

using common::CLogger;

namespace gar {

/**
 * This class implements the factory method pattern to create the suitable
 * genetic algorithm object determined by the 'genetic-algorithm' input option.
 */
class GARGaFactory {
public:
	/**
	 * Default constructor.
	 */
	GARGaFactory() = default;

	/**
	 * Default virtual destructor.
	 */
	virtual ~GARGaFactory() = default;

	/**
	 * @brief The genetic algorithm factory method.
	 * Creates an instance of the suitable genetic algorithm engine class according to
	 * the given genetic algorithm type argument.
	 * @param gaType	The genetic algorithm type (Simple | SteadyState | Incremental | Deme).
	 * @param pGenome	The genetic algorithm genome definition.
	 * @param params	The genetic algorithm parameters.
	 * @param logger	Referene to the ga-router logger.
	 * @return			A pointer to the suitable genetic algorithm engine object that inherits from the
	 * 					GAGeneticAlgorithm virtual class.
	 */
	GAGeneticAlgorithm* createGeneticAlgorithm(const std::string& gaType,
											   const GAGenome const* pGenome,
											   GAParameterList& params,
											   CLogger* logger);

};

} /* namespace gar */

#endif /* GARGAFACTORY_HPP_ */
