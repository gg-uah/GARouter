/*
 * GARScalingFactory.hpp
 *
 *  Created on: Jun 10, 2015
 *      Author: ctomas
 */

#ifndef GARSCALINGFACTORY_HPP_
#define GARSCALINGFACTORY_HPP_

#include <CLogger.hpp>
#include <utils/options/OptionsCont.h>
#include <ga/ga.h>

using common::CLogger;

namespace gar {

typedef GA1DArrayAlleleGenome<const char*> genome_static;
typedef GA1DArrayAlleleGenome<int> genome_dyn;

/**
 * This class implements the factory method pattern to create the suitable
 * scaling object determined by the 'scaling-scheme' input option.
 */
class GARScalingFactory {
public:
	/**
	 * Default constructor.
	 */
	GARScalingFactory() = delete;

	/**
	 * Parameterized constructor.
	 * @param options	The option container that stores the ga-router options.
	 * 					These configuration options can be set either through the command line or by means of a XML file.
	 */
	GARScalingFactory(const OptionsCont& options);

	/**
	 * Default virtual destructor.
	 */
	virtual ~GARScalingFactory() = default;

	/**
	 * @brief The scaling scheme factory method.
	 * Creates an instance of the suitable scaling class according to the given
	 * scaling scheme type argument.
	 * @param pGA		A pointer to the genetic algorithm engine.
	 * @param scaling	The scaling scheme.
	 * @param logger	A reference to the ga-router logger.
	 * @return			A pointer to the suitable selector object that inherits from the
	 * 					GAScalingScheme virtual class.
	 */
	GAScalingScheme* createScalingScheme(const GAGeneticAlgorithm const* pGA,
										 const std::string& scaling,
										 CLogger* logger);

protected:
	/**
	 * Get the coefficient used in the linear scaling from the input options.
	 * If not specified, the default value 'gaDefLinearScalingMultiplier' is returned.
	 * @return	The linear scaling coefficient.
	 */
	float getLinearCoefficient(void) const;

	/**
	 * Get the coefficient used in the sigma truncation scaling from the input options.
	 * If not specified, the default value 'gaDefSigmaTruncationMultiplier' is returned.
	 * @return	The sigma truncation scaling coefficient.
	 */
	float getSigmaTruncationCoefficient(void) const;

	/**
	 * Get the exponential factor used in the power law scaling from the input options.
	 * If not specified, the default value 'gaDefPowerScalingFactor' is returned.
	 * @return	The power lqw scaling exponential factor.
	 */
	int getPowerLawFactor(void) const;

	/**
	 * Get the cutoff value used in the sharing from the input options.
	 * If not specified, the default value 'gaDefSharingCutoff' is returned.
	 * @return	The sharing cutoff value.
	 */
	float getSharingCutoff(void) const;

	/**
	 * Get the alpha value used in the sharing from the input options.
	 * If not specified, the default value '1.0' is returned.
	 * @return	The sharing alpha value.
	 */
	float getSharingAlpha(void) const;

private:
	/**
	 * @brief The distance function used in the GASharing scaling.
	 * The distance function is used to specify how similar to each other two individuals are.
	 * This distance function uses the genes to determine same-ness.
	 * @param g1	An individual in the population.
	 * @param g2	Another individual in the population.
	 * @return		A float number between 0 and 1. If the genomes are the same, then we return a 0.
	 * 				If they are completely different then we return a 1.
	 */
	static float GARAlleleDistance(const GAGenome& g1, const GAGenome& g2);

	/**
	 * @brief Computes the distance between two genomes defined in the static aproach.
	 * A genome defined in the static approach is an array of allele sets. An allele is
	 * a string containing the name of a vehicle route.
	 * All we do is check to see if the allele arrays are identical regardless the position of the genes.
	 * @param genome1	An individual in the static approach population.
	 * @param genome2	Another individual in the static approach population.
	 * @return			A float number between 0 and 1. If the genomes are the same, then we return a 0.
	 * 					If they are completely different then we return a 1.
	 */
	static float computeStaticDistance(const genome_static& genome1, const genome_static& genome2);

	/**
	 * @brief Computes the distance between two genomes defined in the dynamic aproach.
	 * A genome defined in the dynamic approach is an array of allele sets. An allele is
	 * an integer value which refers to the beginning time in seconds of a vehicle route.
	 * @param genome1	An individual in the dynamic approach population.
	 * @param genome2	Another individual in the dynamic approach population.
	 * @return			A float number between 0 and 1. If the genomes are the same, then we return a 0.
	 * 					If they are completely different then we return a 1.
	 */
	static float computeDynamicDistance(const genome_dyn& genome1, const genome_dyn& genome2);


	//! The ga-router option container
	const OptionsCont& options;
};

} /* namespace gar */

#endif /* GARSCALINGFACTORY_HPP_ */
