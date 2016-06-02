/****************************************************************************/
/// @file    GARFrame.cpp
/// @author  Daniel Krajzewicz
/// @author  Eric Nicolay
/// @author  Jakob Erdmann
/// @author  Michael Behrisch
/// @author  Melanie Knocke
/// @date    Thu, 16.03.2006
/// @version $Id: GARFrame.cpp 17235 2014-11-03 10:53:02Z behrisch $
///
// Sets and checks options for df-routing
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.dlr.de/
// Copyright (C) 2006-2014 DLR (http://www.dlr.de/) and contributors
/****************************************************************************/
//
//   This file is part of SUMO.
//   SUMO is free software: you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation, either version 3 of the License, or
//   (at your option) any later version.
//
/****************************************************************************/

// ===========================================================================
// included modules
// ===========================================================================
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

#include <GARFrame.hpp>
#include <utils/options/OptionsCont.h>
#include <utils/options/Option.h>
#include <utils/common/MsgHandler.h>
#include <utils/common/UtilExceptions.h>
#include <utils/common/ToString.h>
#include <utils/common/SystemFrame.h>
#include <utils/common/RandHelper.h>
#include <utils/common/SUMOTime.h>
#include <iostream>
#include <fstream>
#include <ctime>
#include <ga/GAStatistics.h>

#ifdef CHECK_MEMORY_LEAKS
#include <foreign/nvwa/debug_new.h>
#endif // CHECK_MEMORY_LEAKS


// ===========================================================================
// method definitions
// ===========================================================================
void GARFrame::fillOptions() {
	OptionsCont& oc = OptionsCont::getOptions();
	oc.addCallExample("-c <CONFIGURATION>",
			"run routing with options from file");

	// Insert options sub-topics
	SystemFrame::addConfigurationOptions(oc); // fill this subtopic, too
	oc.addOptionSubTopic("Input");
	oc.addOptionSubTopic("Output");
	oc.addOptionSubTopic("Processing");
	oc.addOptionSubTopic("GAParameters");
	oc.addOptionSubTopic("Defaults");
	oc.addOptionSubTopic("Time");
	SystemFrame::addReportOptions(oc); // fill this subtopic, too

	//....................................................... Register input options ...
	oc.doRegister("net-file", 'n', new Option_FileName());
	oc.addSynonyme("net-file", "net");
	oc.addDescription("net-file", "Input",
			"Loads the network description from FILE");

	oc.doRegister("detector-files", 'd', new Option_FileName());
	oc.addSynonyme("detector-files", "detectors");
	oc.addDescription("detector-files", "Input",
			"Loads detector descriptions from FILE(s)");

	oc.doRegister("measure-files", 'f', new Option_FileName());
	oc.addSynonyme("measure-files", "detflows");
	oc.addSynonyme("measure-files", "detector-flow-files", true);
	oc.addDescription("measure-files", "Input",
			"Loads detector flows from FILE(s)");

	oc.doRegister("od-matrix-file", 'm', new Option_FileName());
	oc.addSynonyme("od-matrix-file", "odmatrix");
	oc.addDescription("od-matrix-file", "Input",
			"Loads the O/D matrix flows among districts from FILE(s)");

	oc.doRegister("taz-file", 't', new Option_FileName());
	oc.addSynonyme("taz-file", "taz");
	oc.addDescription("taz-file", "Input",
			"Loads the district (TAZ) in the network from FILE");

	oc.doRegister("od-groups-file", 'g', new Option_FileName());
	oc.addSynonyme("od-groups-file", "odgroups");
	oc.addDescription("od-groups-file", "Input",
			"Loads the origin/destination district (TAZ) groups from FILE");


	//....................................................... Register output options ...
	oc.doRegister("routes-output", 'o', new Option_FileName());
	oc.addDescription("routes-output", "Output",
			"Saves computed routes to FILE");

	oc.doRegister("detector-output", new Option_FileName());
	oc.addSynonyme("detector-output", "detectors-output", true);
	oc.addDescription("detector-output", "Output",
			"Saves typed detectors to FILE");

	oc.doRegister("detectors-poi-output", new Option_FileName());
	oc.addDescription("detectors-poi-output", "Output",
			"Saves detector positions as pois to FILE");

	oc.doRegister("emitters-output", new Option_FileName());
	oc.addDescription("emitters-output", "Output",
			"Saves emitter definitions for source detectors to FILE");

	oc.doRegister("induction-loops-output", new Option_FileName());
	oc.addDescription("induction-loops-output", "Output",
			"Saves induction loop definitions for detectors to FILE");

	oc.doRegister("vtype", new Option_Bool(false));
	oc.addDescription("vtype", "Output",
			"Add vehicle types to the emitters file (PKW, LKW)");

	oc.doRegister("emitters-poi-output", new Option_FileName()); // !!! describe
	oc.addDescription("emitters-poi-output", "Output",
			"Saves emitter positions as pois to FILE");

	oc.doRegister("validation-output", new Option_FileName());
	oc.addDescription("validation-output", "Output", "");

	oc.doRegister("validation-output.add-sources", new Option_Bool(false));
	oc.addDescription("validation-output.add-sources", "Output", "");


	//....................................................... Register processing options ...
	// To get the short routes between detectors being part of the chromosome of the genetic algorithm
	oc.doRegister("short-dfroutes-rate", new Option_Float(1.));
	oc.addDescription("short-dfroutes-rate", "Processing",
			"The share of short routes between detectors being part of the chromosomes in the genetic algorithm population. "
			"Possible values range from 0 (no short routes selected) to 1 (all short routes selected)");

	// To get the long routes between detectors being part of the chromosome of the genetic algorithm
	oc.doRegister("long-det-routes-rate", new Option_Float(0.));
	oc.addDescription("long-det-routes-rate", "Processing",
			"The share of long routes between detectors being part of the chromosomes in the genetic algorithm population. "
			"Possible values range from 0 (no long routes selected) to 1 (all long routes selected)");

	// To get the shortest routes going from a source to a sink edge
	oc.doRegister("k-shortest-routes", new Option_Integer());
	oc.addDescription("k-shortest-routes", "Processing",
			"The number of shortest routes to be computed from a source to a sink edge of the network.");

	// To get the selected shortest routes being part of the chromosome of the genetic algorithm
	oc.doRegister("k-shortest-selected", new Option_Integer());
	oc.addDescription("k-shortest-selected", "Processing",
			"The number of shortest routes to be considered as part of the chromosome alleles "
			"of the genetic algorithm population.");

	// For detector reading
	oc.doRegister("ignore-invalid-detectors", new Option_Bool(false));
	oc.addDescription("ignore-invalid-detectors", "Processing",
			"Only warn about unparseable detectors");

	// For detector type computation
	oc.doRegister("revalidate-detectors", new Option_Bool(false));
	oc.addDescription("revalidate-detectors", "Processing",
			"Recomputes detector types even if given");

	// For route computation
	oc.doRegister("revalidate-routes", new Option_Bool(false));
	oc.addDescription("revalidate-routes", "Processing",
			"Recomputes routes even if given");

	oc.doRegister("keep-unfinished-routes", new Option_Bool(false));
	oc.addSynonyme("keep-unfinished-routes", "keep-unfound-ends", true);
	oc.addDescription("keep-unfinished-routes", "Processing",
			"Keeps routes even if they have exhausted max-search-depth");

	oc.doRegister("keep-longer-routes", new Option_Bool(false));
	oc.addDescription("keep-longer-routes", "Processing",
			"Keeps routes even if a shorter one exists");

	oc.doRegister("max-search-depth", new Option_Integer(30));
	oc.addSynonyme("max-search-depth", "max-nodet-follower", true);
	oc.addDescription("max-search-depth", "Processing",
			"Number of edges to follow a route without passing a detector");

	oc.doRegister("emissions-only", new Option_Bool(false));
	oc.addDescription("emissions-only", "Processing",
			"Writes only emission times");

	oc.doRegister("disallowed-edges", new Option_String(""));
	oc.addDescription("disallowed-edges", "Processing",
			"Do not route on these edges");

	oc.doRegister("keep-turnarounds", new Option_Bool(false));
	oc.addDescription("keep-turnarounds", "Processing",
			"Allow turnarounds as route continuations");

	oc.doRegister("min-route-length", new Option_Float(-1));
	oc.addSynonyme("min-route-length", "min-dist", true);
	oc.addSynonyme("min-route-length", "min-distance", true);
	oc.addDescription("min-route-length", "Processing",
			"Minimum distance in meters between start and end node of every route");

	// flow reading
	oc.doRegister("time-factor", new Option_String("60", "TIME"));
	oc.addDescription("time-factor", "Processing",
			"Multiply flow times with TIME to get seconds");

	oc.doRegister("time-offset", new Option_String("0", "TIME"));
	oc.addDescription("time-offset", "Processing",
			"Subtracts TIME seconds from (scaled) flow times");

	oc.doRegister("time-step", new Option_String("60", "TIME"));
	oc.addDescription("time-step", "Processing",
			"Expected distance between two successive data sets");

	oc.doRegister("time-to-teleport", new Option_String("300", "TIME"));
	oc.addDescription("time-to-teleport", "Processing",
			"Specify how long a vehicle may wait until being teleported in a SUMO simulation,"
			" defaults to 300, non-positive values disable teleporting");

	// trigger the dynamic approach
	oc.doRegister("dynamic-trigger", new Option_Float(0.7));
	oc.addDescription("dynamic-trigger", "Processing",
			"If the score of the static solution is greater or equal than the trigger-dynamic "
			"value, the dynamic genetic algorithm is performed. The triggering value ranges from "
			"0 (always trigger dynamic approach) to 1 (the dynamic approach is triggered if the static"
			"solution has been found)."
			"Default value: 0.7");

	// saving further structures
	oc.doRegister("include-unused-routes", new Option_Bool(false)); // !!!undescribed
	oc.addDescription("include-unused-routes", "Processing", "");

	oc.doRegister("revalidate-flows", new Option_Bool(false));
	oc.addDescription("revalidate-flows", "Processing", "");

	oc.doRegister("remove-empty-detectors", new Option_Bool(false));
	oc.addDescription("remove-empty-detectors", "Processing",
			"Removes empty detectors from the list");

	oc.doRegister("strict-sources", new Option_Bool(false)); // !!!undescribed
	oc.addDescription("strict-sources", "Processing", "");

	oc.doRegister("respect-concurrent-inflows", new Option_Bool(false));
	oc.addDescription("respect-concurrent-inflows", "Processing",
			"Try to determine further inflows to an inbetween detector when computing split probabilities");

	oc.doRegister("scale", new Option_Float(1.));
	oc.addDescription("scale", "Processing", "Scale factor for flows");

	oc.doRegister("meas-freq", new Option_Integer(10));
	oc.addDescription("meas-freq", "Processing", "The measurement frequency in seconds");


	//....................................................... Register the GA parameter options ...
	// genetic algorithm type
	// number of generations
	oc.doRegister("genetic-algorithm", new Option_String("Simple"));
	oc.addDescription("genetic-algorithm", "GAParameters",
			"The type of the genetic algorithm to be used. "
			"Possible values: [Simple | SteadyState | Incremental | Deme]. "
			"Simple (non-overlapping populations), "
			"SteadyState (overlapping populations), "
			"Incremental (overlapping with custom replacement), "
			"Deme (parallel populations with migration)."
			"Default value: Simple");
	// seed
	oc.doRegister("ga-seed", new Option_Integer());
	oc.addDescription("ga-seed", "GAParameters",
			"The random seed to use in the genetic algorithm. "
			"When  a random seed is specified, the evolution will be exactly the same each time "
			"that seed number is used.");
	// number of generations
	oc.doRegister("number-of-generations", new Option_Integer(250));
	oc.addDescription("number-of-generations", "GAParameters",
			"The number of generations to evolve the population. Default value: 250");
	// population size
	oc.doRegister("population-size", new Option_Integer(30));
	oc.addDescription("population-size", "GAParameters",
			"The number of individuals in the population. Default value: 30");
	// number of populations
	oc.doRegister("number-of-populations", new Option_Integer(10));
	oc.addDescription("number-of-populations", "GAParameters",
			"The number of populations in the evolution algorithm. Default value: 10");
	// convergence percentage
	oc.doRegister("convergence-percentage", new Option_Float(0.99));
	oc.addDescription("convergence-percentage", "GAParameters",
			"The convergence percentage."
			"The convergence is defined as the ratio of the Nth previous best-of-generation score to the current best-of-generation score. "
			"Default value: 0.99");
	// crossover probability
	oc.doRegister("crossover-probability", new Option_Float(0.9));
	oc.addDescription("crossover-probability", "GAParameters",
			"The crossover probability in the population. Default value: 0.9");
	// mutation probability
	oc.doRegister("mutation-probability", new Option_Float(0.01));
	oc.addDescription("mutation-probability", "GAParameters",
			"The mutation probability in an individual of the population. Default value: 0.01");
	// mutation probability
	oc.doRegister("replacement-percentage", new Option_Float(0.25));
	oc.addDescription("replacement-percentage", "GAParameters",
			"The percentage of the population to replace each generation (the amount of overlapping). "
			"When given, the replacement number is set to 0. Default value: 0.25");
	// replacement number
	oc.doRegister("replacement-number", new Option_Integer(5));
	oc.addDescription("replacement-number", "GAParameters",
			"The number of individuals to replace each generation. "
			"When given, the replacement percentage is set to 0. Default value: 5");
	// mimimize or maximize based sharing scaling
	oc.doRegister("mini-maxi-scaling", new Option_Integer(1));
	oc.addDescription("mini-maxi-scaling", "GAParameters",
			"The sharing scaling differs depending on whether the objective is to maximized or minimized. "
			"If the goal is to maximize the objective score, the raw scores will be divided by the sharing factor. "
			"If the goal is to minimize the objective score, the raw scores will be multiplied by the sharing factor. "
			"You can explicitly tell the sharing object to do minimize- or maximize-based scaling by using the minimaxi "
			"member function. Default value: 1");
	// number of best genomes
	oc.doRegister("number-of-best", new Option_Integer(1));
	oc.addDescription("number-of-best", "GAParameters",
			"How many best individuals to record. Default value: 1");
	// score frequency
	oc.doRegister("score-freq", new Option_Integer(1));
	oc.addDescription("score-freq", "GAParameters",
			"How often to record the score. Default value: 1");
	// flush frequency
	oc.doRegister("flush-freq", new Option_Integer(0));
	oc.addDescription("flush-freq", "GAParameters",
			"How often to dump scores to file. Default value: 0");
	// score file
	oc.doRegister("score-file", new Option_FileName("gar_stats.dat"));
	oc.addDescription("score-file", "GAParameters",
			"The file name where the scores are dumped. Default value: 'gar_stats.dat'");
	// select scores
	oc.doRegister("select-scores", new Option_String("GAStatistics::Maximum"));
	oc.addDescription("select-scores", "GAParameters",
			"Which statistical data of the scores are dumped to the score file. "
			"The argument is the logical OR of the following values: Mean, Maximum, Minimum, Deviation, Diversity "
			"(all defined in the scope of the GAStatistics object)."
			"Possible values: [GAStatistics::AllScores | GAStatistics::NoScores | GAStatistics::Mean | GAStatistics::Maximum | "
			"GAStatistics::Minimum | GAStatistics::Deviation | GAStatistics::Diversity."
			"Default value: GAStatistics::Maximum");
	// elitism
	oc.doRegister("elitism", new Option_Bool(gaTrue));
	oc.addDescription("elitism", "GAParameters",
			"Specify if the elitism is on or off. If the elitism is on, it means that the best individual"
			"from each generation is carried over to the next generation. Possible values: true and false. "
			"Default value: true");
	// number of offspring
	oc.doRegister("number-of-offspring", new Option_Integer(2));
	oc.addDescription("number-of-offspring", "GAParameters",
			"The number of children that are generated in each 'generation'. "
			"Since this genetic algorithm is based on a two-parent crossover model, "
			"the number of offspring must be either 1 or 2. "
			"Default value: 2");
	// record diversity
	oc.doRegister("record-diversity", new Option_Bool(gaFalse));
	oc.addDescription("record-diversity", "GAParameters",
			"Specify whether or not to record diversity statistics. Since diversity calculations require comparison of each"
			"individual with every other, recording this statistic can be expensive and the default is to not record diversity. "
			"Possible values: true and false. "
			"Default value: false");
	// migration percentage
	oc.doRegister("migration-percentage", new Option_Float(0.1));
	oc.addDescription("migration-percentage", "GAParameters",
			"The share of individuals which are migrated from one population to another "
			"in each generation when using a GADemeGA algorithm. The migration algorithm is deterministic stepping-stone; "
			"each population migrates a fixed number of its best individuals to its neighbor. The master population is updated "
			"each generation with best individual from each population. "
			"Default value: 0.1");
	// migration number
	oc.doRegister("migration-number", new Option_Integer(5));
	oc.addDescription("migration-number", "GAParameters",
			"The number of individuals migrating from one population to a neighbor in each generation. "
			"Each population will migrate this many of its best individuals to the neighbor population "
			"(the stepping-stone migration model). The worst individuals in the neighbor population are replaced. "
			"Default value: 5");
	// initialization operator
	oc.doRegister("genome-initializer", new Option_String("Uniform"));
	oc.addDescription("genome-initializer", "GAParameters",
			"Defines the initialization operator of the genetic algorithm. "
			"The initialization operator defines how the genome is initialized. "
			"This operator works when a population is created. This operator does not actually create new genomes, "
			"rather it 'stuffs' the genomes with the primordial genetic material from which all solutions will evolve."
			"Valid initialization values: [Uniform | Ordered]. "
			"Uniform: the genes in each genome element are randomly chosen,"
			"Ordered: Run the genome from the beginning to the end, and the next gene value from the set of possible values is assigned in every position. "
			"When the genome is initialized, scramble the contents by swapping the elements. "
			"This assumes that there is only one allele set in the genome array. "
			"Default value: Uniform");
	// mutation operator
	oc.doRegister("genome-mutator", new Option_String("Flip"));
	oc.addDescription("genome-mutator", "GAParameters",
			"The mutation operator. The mutation operator defines the procedure for mutating each genome. "
			"In general, you should define a mutation that can do both exploration and exploitation; "
			"mutation should be able to introduce new genetic material as well as modify existing material. "
			"Valid mutation operators: [Flip | Swap]. "
			"Flip: randomly pick elements in the array then set the element to any of the alleles in the allele set for this genome, "
			"Swap: swaps the value of one element of the genome for the value of other element with a given probability. "
			"Default value: Flip");
	// crossover operator
	oc.doRegister("genome-crossover", new Option_String("OnePoint"));
	oc.addDescription("genome-crossover", "GAParameters",
			"The crossover operator. The crossover operator defines the procedure for generating a child "
			"from two parent genomes. Like the mutation operator, crossover is specific to the data type. Unlike mutation, "
			"however, crossover involves multiple genomes. In GAlib, each genome 'knows' its preferred method of mating "
			"(the default crossover method) but it is incapable of performing crossover itself. Each genetic algorithm 'knows' "
			"how to get the default crossover method from its genomes then use that method to peform the mating."
			"Valid crossover operators: [Cycle | EvenOdd | OnePoint | Order | PartialMatch | TwoPoint | Uniform]. "
			"Cycle: cycle crossover. This is implemented as described in Goldberg's book.  The first is picked from mom, then cycle using dad."
			" Finally, fill in the gaps with the elements from dad. We allocate space for a temporary array in this routine."
			" It never frees the memory that it uses, so you might want to re-think this if you're really memory-constrained"
			" (similar to what we do with the uniform crossover when the children are resizeable)."
			" Allocate space for an array of flags.  We use this to keep track of whether the child's contents came from the mother"
			" or the father.  We don't free the space here, but it is not a memory leak. The first step is to cycle through mom & dad"
			" to get the cyclic part of The crossover. Then fill in the rest of the sister with dad's contents that we didn't use in the cycle."
			" Finally, do the same thing for the other child, "
			"EvenOdd: even and odd crossover. For even crossover we take the 0th element and every other one after that from the mother."
			" The 1st and every other come from the father. For odd crossover, we do just the opposite, "
			"OnePoint: one point crossover. This crossover picks a single point in the parents then generates one or two children from the two halves of each parent, "
			"Order: order crossover. This uses the order crossover described in Goldberg's book. This implementation isn't terribly smart."
			" For example, a linear search is done rather than caching and doing binary search or smarter hash tables."
			" First we copy the mother into the sister. Then move the 'holes' into the crossover section and maintain the ordering of the non-hole elements."
			" Finally, put the 'holes' in the proper order within the crossover section. After we have done the sister, we do the brother."
			"PartialMatch: partial match crossover. This uses the partial matching algorithm described in Goldberg's book. "
			"Parents and children must be the same size for this crossover to work, "
			"TwoPoint: two point crossover. Similar to the single point crossover, but here we pick two points then grab the sections based upon those two points, "
			"Uniform: uniform crossover. Randomly take elements from each parent. For each element we flip a coin to see if element bit should come from the mother or the father. "
			"Default value: OnePoint");
	// scaling scheme
	oc.doRegister("scaling-scheme", new Option_String("Linear"));
	oc.addDescription("scaling-scheme", "GAParameters",
			"Defines the scaling scheme the genetic algorithm uses for selection. Each population object has a scaling scheme object "
			"associated with it. The scaling scheme object converts the objective score of each genome to a fitness score that "
			"the genetic algorithm uses for selection. It also caches fitness information for use later on by the selection schemes."
			"Valid scaling schemes: [NoScaling | Linear | SigmaTruncation | PowerLaw | Sharing]. "
			"NoScaling: The fitness scores are identical to the objective scores. No scaling takes place, "
			"Linear: The fitness scores are derived from the objective scores using the linear scaling method described in Goldberg's book."
			" Objective scores are converted to fitness scores using the relation 'f = a • obj + b' where 'a' and 'b' are calculated based"
			" upon the objective scores of the individuals in the population as described in Goldberg's book, "
			"SigmaTruncation: Use this scaling method if your objective scores will be negative. It scales based on the variation"
			" from the population average and truncates arbitrarily at 0. The mapping from objective to fitness score for each individual"
			" is given by 'f = obj - (obj_ave - c • obj_dev)', "
			"PowerLaw: Power law scaling maps objective scores to fitness scores using an exponential relationship defined as 'f = obj^k', "
			"Sharing: This scaling method is used to do speciation. The fitness score is derived from its objective score by comparing the individual"
			" against the other individuals in the population. If there are other similar individuals then the fitness is derated."
			" The distance function is used to specify how similar to each other two individuals are. A distance function must return a value of 0 or higher,"
			" where 0 means that the two individuals are identical (no diversity). "
			"Default value: Linear");
	// linear scaling coefficient
	oc.doRegister("linear-scaling-multiplier", new Option_Float(1.2));
	oc.addDescription("linear-scaling-multiplier", "GAParameters",
			"The multiplier used in the linear scaling. "
			"Default value: 1.2");
	// sigma truncation scaling coefficient
	oc.doRegister("sigma-truncation-multiplier", new Option_Float(2.0));
	oc.addDescription("sigma-truncation-multiplier", "GAParameters",
			"The multiplier used in the sigma truncation scaling. "
			"Default value: 2.0");
	// power law scaling coefficient
	oc.doRegister("power-scaling-factor", new Option_Integer(1));
	oc.addDescription("power-scaling-factor", "GAParameters",
			"The exponential factor used in the power law scaling. "
			"Default value: 1");
	// sharing cutoff
	oc.doRegister("sharing-cutoff", new Option_Float(1.0));
	oc.addDescription("sharing-cutoff", "GAParameters",
			"The cutoff value used in the sharing scaling. "
			"Default value: 1.0");
	// sharing alpha
	oc.doRegister("sharing-alpha", new Option_Float(1.0));
	oc.addDescription("sharing-alpha", "GAParameters",
			"The curvature of the sharing function.When alpha is 1.0 the sharing function is a straight line (triangular sharing). "
			"Default value: 1.0");
	// selection scheme
	oc.doRegister("selection-scheme", new Option_String("Roulette"));
	oc.addDescription("selection-scheme", "GAParameters",
			"Specify the selection scheme used to pick individuals from the populations. A selector may make its selections based "
			"either on the scaled (fitness) scores or on the raw (objective) scores of the individuals in the population. "
			"Valid selection schemes: [Rank | Roulette | Tournament | Uniform | SRS | DS]. "
			"Rank: the rank selector simply picks the best individual in the population, "
			"Roulette: roulette wheel uses a fitness-proportional algorithm for selecting individuals, "
			"Tournament: this version of the tournament selector does two roulette wheel selections then picks the better of the two, "
			"Uniform: stochastic uniform sampling selection. This is just a fancy name for random sampling."
			" Any individual in the population has as much chance of being selected as any other, "
			"SRS: stochastic remainder sampling selection, "
			"DS:  deterministic sampling selection. "
			"Default value: Roulette");
	// termination function
	oc.doRegister("termination-function", new Option_String("UponGeneration"));
	oc.addDescription("termination-function", "GAParameters",
			"Specify the termination function. Completion functions are used to determine whether or not a genetic algorithm is finished. "
			"Valid termination functions: [UponGeneration | UponConvergence | UponPopConvergence]"
			"UponGeneration: This function compares the current generation to the specified number of generations."
			" If the current generation is less than the requested number of generations, it returns gaFalse. Otherwise, it returns gaTrue, "
			"UponConvergence: This function compares the current convergence to the specified convergence value."
			" If the current convergence is less than the requested convergence, it returns gaFalse. Otherwise, it returns gaTrue."
			" Convergence is a number between 0 and 1. A convergence of 1 means that the nth previous best-of- generation is equal to the current best-of-generation."
			" When you use convergence as a stopping criterion you must specify the convergence percentage and you may specify the number of previous generations against which to compare."
			" The genetic algorithm will always run at least this many generations."
			"UponPopConvergence: This function compares the population average to the score of the best individual in the population."
			" If the population average is within pConvergence of the best individual's score, it returns gaTrue. Otherwise, it returns gaFalse. "
			"Possible values: [UponGeneration | UponConvergence | UponPopConvergence]. "
			"Default value: UponGeneration");
	// replacement scheme
	oc.doRegister("replacement-scheme", new Option_String("Random"));
	oc.addDescription("replacement-scheme", "GAParameters",
			"Specify the replacement scheme only in incremental GA. The replacement scheme is used by the incremental genetic "
			"algorithm to determine how a new individual should be inserted into a population. "
			"Valid replacement schemes: [Random | Best | Worst | Custom | Crowding | Parent]. "
			"In general, replace worst produces the best results. "
			"Replace parent is useful for basic speciation, and custom replacement can be used when you want to do your own type of speciation. "
			"If you specify 'Custom' or 'Crowding' replacement then you must also specify a replacement function. "
			"The replacement function takes as arguments an individual and the population into which the individual should be placed. "
			"It returns a reference to the genome that the individual should replace. If the individual should not be inserted into the population, "
			"the function should return a reference to the individual. "
			"Default value: Random");

	//....................................................... Register the default options ...
	// register default options
	oc.doRegister("departlane", new Option_String());
	oc.addDescription("departlane", "Defaults",
			"Assigns a default depart lane");

	oc.doRegister("departpos", new Option_String());
	oc.addDescription("departpos", "Defaults",
			"Assigns a default depart position");

	oc.doRegister("departspeed", new Option_String());
	oc.addDescription("departspeed", "Defaults",
			"Assigns a default depart speed");

	oc.doRegister("arrivallane", new Option_String());
	oc.addDescription("arrivallane", "Defaults",
			"Assigns a default arrival lane");

	oc.doRegister("arrivalpos", new Option_String());
	oc.addDescription("arrivalpos", "Defaults",
			"Assigns a default arrival position");

	oc.doRegister("arrivalspeed", new Option_String());
	oc.addDescription("arrivalspeed", "Defaults",
			"Assigns a default arrival speed");

	//....................................................... Register the time options ...
	// Register the simulation settings
	oc.doRegister("begin", 'b', new Option_String("0", "TIME"));
	oc.addDescription("begin", "Time",
			"Defines the begin time; Previous definitions will be discarded");

	oc.doRegister("end", 'e', new Option_String("86400", "TIME"));
	oc.addDescription("end", "Time",
			"Defines the end time; Later definitions will be discarded; Defaults to one day");

	//....................................................... Register the report options ...
	oc.doRegister("report-empty-detectors", new Option_Bool(false));
	oc.addDescription("report-empty-detectors", "Report",
			"Lists detectors with no flow (enable -v)");

	oc.doRegister("print-absolute-flows", new Option_Bool(false));
	oc.addDescription("print-absolute-flows", "Report",
			"Prints aggregated detector flows");

	// register report options
	oc.doRegister("no-step-log", new Option_Bool(false));
	oc.addDescription("no-step-log", "Report",
			"Disable console output of route parsing step");

	//....................................................... Register the random options ...
	RandHelper::insertRandOptions();
}

bool GARFrame::checkOptions() {
	return true;
}

/****************************************************************************/

