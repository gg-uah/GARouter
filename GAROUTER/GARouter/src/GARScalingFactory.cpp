/*
 * GARScalingFactory.cpp
 *
 *  Created on: Jun 10, 2015
 *      Author: ctomas
 */

#include <GARScalingFactory.hpp>
#include <StringUtilities.hpp>
#include <cstdlib>

using common::StringUtilities;
using std::string;
using std::vector;

namespace gar {

//................................................. Parameterized constructor ...
GARScalingFactory::GARScalingFactory(const OptionsCont& options)
: options (options) {
	// Intentionally left empty
}


//................................................. Factory method ...
GAScalingScheme* GARScalingFactory::createScalingScheme(const GAGeneticAlgorithm const* pGA,
														const std::string& scaling,
														CLogger* logger) {
	try {
		if (StringUtilities::trim(scaling) == "NoScaling") {
			return new GANoScaling();
		}

		if (StringUtilities::trim(scaling) == "Linear") {
			float c = getLinearCoefficient();
			return new GALinearScaling(c);
		}

		if (StringUtilities::trim(scaling) == "SigmaTruncation") {
			float c = getSigmaTruncationCoefficient();
			return new GASigmaTruncationScaling(c);
		}

		if (StringUtilities::trim(scaling) == "PowerLaw") {
			int k = getPowerLawFactor();
			return new GAPowerLawScaling(k);
		}

		if (StringUtilities::trim(scaling) == "Sharing") {
			float cutoff = getSharingCutoff();
			float alpha = getSharingAlpha();
			return new GASharing(GARAlleleDistance, cutoff, alpha);
		}

	} catch (const std::exception& ex) {
		logger->fatal("Fail to create scaling scheme for type [" + scaling + "]");
	}

	return nullptr;
}


//................................................. Get the linear scaling coefficient ...
float GARScalingFactory::getLinearCoefficient(void) const {
	if (!options.isSet("linear-scaling-multiplier")) {
		return gaDefLinearScalingMultiplier;
	}

	return options.getFloat("linear-scaling-multiplier");
}


//................................................. Get the sigma truncation scaling coefficient ...
float GARScalingFactory::getSigmaTruncationCoefficient(void) const {
	if (!options.isSet("sigma-truncation-multiplier")) {
		return gaDefSigmaTruncationMultiplier;
	}

	return options.getFloat("sigma-truncation-multiplier");
}


//................................................. Get the power law scaling coefficient ...
int GARScalingFactory::getPowerLawFactor(void) const {
	if (!options.isSet("power-scaling-factor")) {
		return gaDefPowerScalingFactor;
	}

	return options.getInt("power-scaling-factor");
}


//................................................. Get the sharing cutoff ...
float GARScalingFactory::getSharingCutoff(void) const {
	if (!options.isSet("sharing-cutoff")) {
		return gaDefSharingCutoff;
	}

	return options.getFloat("sharing-cutoff");
}


//................................................. Get the sharing alpha ...
float GARScalingFactory::getSharingAlpha(void) const {
	if (!options.isSet("sharing-alpha")) {
		return 1.0;
	}

	return options.getFloat("sharing-alpha");
}


//................................................. The sharing distance function ...
float GARScalingFactory::GARAlleleDistance(const GAGenome& g1, const GAGenome& g2) {
	if (typeid(g1) == typeid(genome_static)  &&  typeid(g2) == typeid(genome_static)) {
		return computeStaticDistance((const genome_static&)g1, (const genome_static&)g2);
	}

	if (typeid(g1) == typeid(genome_dyn)  &&  typeid(g2) == typeid(genome_dyn)) {
		return computeDynamicDistance((const genome_dyn&)g1, (const genome_dyn&)g2);
	}

	return 0.5;
}


//................................................. The sharing distance for the static approach ...
float GARScalingFactory::computeStaticDistance(const genome_static& genome1, const genome_static& genome2) {
	vector<string> genomevec2;
	for (int i = 0; i < genome2.size(); i++) {
		genomevec2.push_back(string(genome2.gene(i)));
	}

	float dist = 0.0;

	// Compare the genomes gene-to-gene
	for (int i = 0; i < genome1.size(); i++) {
		// Get the gene in genome2
		string gene = genome1.gene(i);

		// Find the gene in genome2
		auto it = std::find(genomevec2.begin(), genomevec2.end(), gene);
		if (it == genomevec2.end()) {
			dist += 1;
		} else {
			genomevec2.erase(it);
		}
	}

	// Return a number between 0 (same) and 1 (entirely different)
	return dist / genome1.length();
}


//................................................. The sharing distance for the dynamic approach ...
float GARScalingFactory::computeDynamicDistance(const genome_dyn& genome1, const genome_dyn& genome2) {
	float dist = 0.0;

	for (int i = 0; i < genome1.size(); i++) {
		if (genome1.gene(i) != genome2.gene(i)) {
			dist += 1.;
		}
	}

	// Return a number between 0 (same) and 1 (entirely different)
	return dist / float(genome1.length());
}



} /* namespace gar */
