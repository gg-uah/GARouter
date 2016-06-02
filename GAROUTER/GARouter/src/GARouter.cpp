/*
 * GARouter.cpp
 *
 *  Created on: Apr 21, 2015
 *      Author: ctomas
 */

#include <GARouter.hpp>
#include <GARConst.hpp>
#include <GARUtils.hpp>
#include <GARGaFactory.hpp>
#include <GARScalingFactory.hpp>
#include <GARSelectionFactory.hpp>
#include <GARStaticObjective.hpp>
#include <GARDynObjective.hpp>
#include <router/ROLane.h>
#include <utils/common/SystemFrame.h>
#include <utils/xml/XMLSubSys.h>
#include <StringUtilities.hpp>
#include <CDump.hpp>
#include <YenTopKShortestPathsAlg.hpp>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/xml_parser.hpp>
#include <boost/filesystem.hpp>
#include <random>
#include <ctime>
#include <algorithm> // std::reverse
#include <typeinfo>  // std::bad_cast

using common::CLogger;
using common::StringUtilities;
using common::CDump;
using std::string;
using std::vector;
using std::map;
using std::unique_ptr;
using std::shared_ptr;
using std::ofstream;
using boost::numeric::ublas::matrix;

namespace boost_pt = boost::property_tree;

namespace gar {

//................................................. Parameterized constructor ...
GARouter::GARouter(const OptionsCont& optCont,
				   const std::shared_ptr<GARNet>& pNet,
				   const std::shared_ptr<GAROdMatrix>& pOdMatrix,
				   const std::shared_ptr<GARTazCont>& pTazCont,
				   const std::shared_ptr<GARDetectorCon>& pDetCont,
				   const std::shared_ptr<GARDetectorFlows>& pFlows,
				   CLogger* logger)
: options         (optCont),
  logger          (logger),
  pNet            (pNet),
  pOdMatrix       (pOdMatrix),
  pTazCont        (pTazCont),
  pDetCont        (pDetCont),
  pFlows          (pFlows),
  pRouCont        (new GARRouteCont()),
  pTripCont       (new GARTripCont(gar::RouteMode::Selected, logger)),
  params	      (GAParameterList()),
  pGAStatic       (nullptr),
  pStaticGenome   (nullptr),
  pStaticObjData  (unique_ptr<GARStaticObjData>(new GARStaticObjData(logger))),
  pStaticSolution (nullptr),
  pGADyn		  (nullptr),
  pDynGenome	  (nullptr),
  pDynObjData     (unique_ptr<GARDynObjData>(new GARDynObjData(logger))),
  pDynSolution    (nullptr),
  alleles         (vector<string>()),
  scoreFilename   ("./gar_stats.dat") {
	// Empty constructor
}


//................................................. Initializes the ga-router application system ...
int GARouter::init(void) {
	try {
		// Enables or disable xml validation
		logger->debug("Set XML validation: [" + options.getString("xml-validation") + "]");
		XMLSubSys::setValidation(options.getString("xml-validation"),
								 options.getString("xml-validation.net"));
		// Initialize the Xerces-parsers
		XMLSubSys::init();

		// Initialize the messaging system
		logger->debug("Initialize messaging system");
		MsgHandler::initOutputOptions();

		// Initialize the random generator
		logger->debug("Initialize the random generator");
		RandHelper::initRandGlobal();

	} catch (std::exception& ex) {
		logger->fatal("Fail to initialize the GA router:" + (string)ex.what());
		SystemFrame::close();
		return 1;
	}

	return 0;
}


//................................................. Closes the ga-router application system ...
int GARouter::close(void) {
	try {
		// Close the XML sub-system
		XMLSubSys::close();

	} catch (std::exception& ex) {
		logger->fatal("Fail to close the GA router:" + (string)ex.what());
		SystemFrame::close();
		return 1;
	}

	return 0;
}

//................................................. Computes the routes ...
int GARouter::computeRoutes(void) {
	// If option 'k-shortest-routes' is set, compute the Yen's k-shortest-path
	if (options.isSet("k-shortest-routes")) {
		computeKShortestRoutes();
		return 0;
	}

	// If option 'k-shortest-routes' is not set, compute the routes between detectors
	computeRoutesBetweenDetectors();

	return 0;
}


//................................................. Computes the K-shortest routes ...
int GARouter::computeKShortestRoutes(void) {
	// Get the number of k-shortest routes
	if (!options.isSet("k-shortest-routes")) {
		logger->error("The 'k-shortest-routes' option is not set");
		return 1;
	}
	const int k = options.getInt("k-shortest-routes");

	// Get the number of selected shortest routes
	const int kSelected = options.isSet("k-shortest-selected") ? options.getInt("k-shortest-selected") : k;

	if (kSelected > k) {
		logger->error("The 'k-shortest-selected' option must be lower than the 'k-shortest-routes' option.");
		return 1;
	}

	logger->error("Compute the k='" + StringUtilities::toString<int>(k) + "' shortest routes");
	try {
		Graph graph = this->buildKShortestGraph();

		// Get the source and sink detectors
		vector<GARDetector*> sources = pDetCont->getDetectorsByType(SOURCE_DETECTOR);
		vector<GARDetector*> sinks = pDetCont->getDetectorsByType(SINK_DETECTOR);

		// Compute shortest paths between from source nodes and to sink nodes
		for (const GARDetector const* pSourceDet : sources) {
			for (const GARDetector const* pSinkDet : sinks) {
				// Leave out source and sink edges in the same district group
				shared_ptr<const GARTazGroup> sourceGroup = pTazCont->findSourceEdgeGroup(pSourceDet->getEdgeID());
				shared_ptr<const GARTazGroup> sinkGroup = pTazCont->findSinkEdgeGroup(pSinkDet->getEdgeID());
				if (sourceGroup->getId() == sinkGroup->getId()) {
					continue;
				}

				vector<GARRouteDesc> shortestRoutes = getKShortestRoutes(graph, pSourceDet, pSinkDet, k, kSelected);

				// Add the shortest routes to the route and trip containers
				addKShortestRoutes(shortestRoutes, pSourceDet, pSinkDet);
			}
		}
	} catch (const std::exception& ex) {
		logger->error("Fail to compute the '" + StringUtilities::toString<int>(k) + "' shortest paths: " + string(ex.what()));
		return 1;
	}

	return 0;
}


//................................................. Computes routes between the detectors ...
int GARouter::computeRoutesBetweenDetectors(void) {
	if (pDetCont->getDetectorsByType(SOURCE_DETECTOR).empty()) {
		throw ProcessError("No source detectors found.");
	}

	try {
		if (!pDetCont->detectorsHaveRoutes()  ||  options.getBool("revalidate-routes")) {
			PROGRESS_BEGIN_MESSAGE("Computing routes");
			pNet->buildRoutes(*pDetCont,
							  *pTazCont,
							  *pRouCont,
							  *pTripCont,
							  options.getInt("max-search-depth"),
							  logger);
			PROGRESS_DONE_MESSAGE();
		}

		// Consider only the selected routes in every trip
		if (pTripCont->getRouteMode() == RouteMode::Selected) {
			float shortRate = options.getFloat("short-dfroutes-rate");
			float longRate  = options.getFloat("long-dfroutes-rate");

			pTripCont->setOdTrips(pTripCont->buildTripsWithGASelectedRoutes(shortRate, longRate));
		}
	} catch (const std::exception& ex) {
		logger->error("Fail to compute routes between the detectors: " + string(ex.what()));
		return 1;
	}

	logger->info(StringUtilities::toString((long)pRouCont->get().size()) + " routes loaded");

	return 0;
}


//................................................. Dump the routes into data files ...
int GARouter::dumpRoutes(void) {
	try {
		// Save the routes file if it was changed or it's wished
		if (pDetCont->detectorsHaveRoutes()  &&  options.isSet("routes-output")) {
			logger->info("Saving routes into [" + options.getString("routes-output") + "]");
			pDetCont->saveRoutes(options.getString("routes-output"));
		} else {
			logger->fatal("The detectors must have routes and the \"routes-output\" option must be set.");
			return 1;
		}
	} catch (const std::exception& ex) {
		logger->error("Fail to save routes: " + string(ex.what()));
		return 1;
	}

	return 0;
}


//................................................. Build the data required in the static approach objective function ...
int GARouter::buildGAStaticObjectiveData(void) {
	// Set the network data
	if (pNet) {
		logger->info("Set the network data to objective data");
		pStaticObjData->setpNet(pNet.get());
	} else {
		logger->error("No network data was loaded.");
		return 1;
	}

	// Set the routes container
	if (pRouCont && !pRouCont->get().empty()) {
		logger->info("Set the route container to objective data");
		pStaticObjData->setpRouCont(pRouCont.get());
	} else {
		logger->error("No routes were loaded.");
		return 1;
	}

	// Set the detectors container
	if (pDetCont && !pDetCont->getDetectors().empty()) {
		logger->info("Set the detector container to objective data");
		pStaticObjData->setpDetCont(pDetCont.get());
	} else {
		logger->error("No detectors were loaded.");
		return 1;
	}

	// Set the detector flow data
	if (!pFlows->getFlows().empty()) {
		logger->info("Set the detector flows to objective data");
		pStaticObjData->setpDetFlows(pFlows.get());
	} else {
		logger->error("No detector flows were loaded.");
		return 1;
	}

	return 0;
}


//................................................. Build the genome for the static approach ...
int GARouter::buildGAStaticGenome(void) {
	if (pStaticObjData == nullptr) {
		logger->fatal("No objective data has been created for the static approach");
		return 1;
	}

	try {
		// Build the allele set array
		GAAlleleSetArray<const char*> alleleArray = buildGAStaticAlleleSetArray();
		if (alleleArray.size() == 0) {
			return 0;
		}

		// Build the genome for the static approach
		pStaticGenome = std::unique_ptr<genome_static>(new genome_static(alleleArray,
														  	   	   	     GARStaticObjective::objective,
																		 pStaticObjData.get()));

		// Set the genetic operators
		setGAGenomeInitializer(pStaticGenome.get());
		setGAGenomeMutator(pStaticGenome.get());
		setGAGenomeCrossover(pStaticGenome.get());

	} catch (const std::exception& ex) {
		logger->error("Fail to build genome for the static approach: " + string(ex.what()));
		return 1;
	}

	logger->info("The genome for the static approach is successfully built");
	return 0;
}


//................................................. Build the genetic algorithm engine for the static approach ...
int GARouter::buildGAStaticGeneticAlgorithm(void) {
	if (pStaticGenome == nullptr) {
		logger->fatal("The GA genome of the static approach hasn't been built.");
		return 1;
	}

	// Get the genetic algorithm type from the input options
	if (!options.isSet("genetic-algorithm")) {
		logger->fatal("Option 'genetic-algorithm' is not specified.");
		return 1;
	}
	string gaType = options.getString("genetic-algorithm");

	// Create the suitable genetic algorithm engine
	GARGaFactory* gaFactory = new GARGaFactory();

	logger->info("Create a [" + gaType + "] genetic algorithm for the static approach");
	pGAStatic = std::unique_ptr<GAGeneticAlgorithm>(gaFactory->createGeneticAlgorithm(gaType,
						 															  pStaticGenome.get(),
																					  params,
																					  logger));
	delete gaFactory;

	if (pGAStatic == nullptr) {
		logger->fatal("Fail to build the static genetic algorithm engine for type [" + gaType + "]");
		return 1;
	}

	int result = 0;

	// Set the scaling scheme
	logger->info(">> Set the Scaling Scheme");
	result = setGAScalingScheme(pGAStatic.get());
	if (result > 0) {
		return result;
	}

	// Set the selection scheme
	logger->info(">> Set the Selection Scheme");
	result = setGASelectionScheme(pGAStatic.get());
	if (result > 0) {
		return result;
	}

	// Set the termination function
	logger->info(">> Set the Termination Function");
	result = setGATerminationFunction(pGAStatic.get());
	if (result > 0) {
		return result;
	}

	// Set the replacement scheme for incremental GA
	if (gaType == "Incremental") {
		logger->info(">> Set the Replacement Scheme");
		result = setGAReplacementScheme(pGAStatic.get());
		if (result > 0) {
			return result;
		}
	}

	return 0;
}


//................................................. Evolve the genetic algorithm for the static approach ...
int GARouter::evolveStaticGA(void) {
	if (pGAStatic == nullptr) {
		logger->fatal("The genetic algorithm engine for the static approach is not created.");
		return 1;
	}

	if (pStaticGenome == nullptr) {
		logger->fatal("The genetic algorithm genome for the static approach is not defined.");
		return 1;
	}

	if (params.size() == 0) {
		logger->fatal("No parameters for the genetic algorithm are defined.");
		return 1;
	}

	try {
		// Set the parameters to the genetic algorithm engine
		pGAStatic->parameters(params);

		// Set the score file name
		string scoreFile = buildScoreFileName("static_");
		pGAStatic->scoreFilename(scoreFile.c_str());

		// Initialize the genetic algorithm using a seed if reported
		logger->info("Initializing the GA for the static approach...");
		if (options.isSet("ga-seed")) {
			pGAStatic->initialize(options.getInt("ga-seed"));
		} else {
			pGAStatic->initialize();
		}

		// Print the initial population
		logger->debug("Initial population:");
		for (int i = 0; i < pGAStatic->populationSize(); i++) {
			string sGenome = "";
			const genome_static& genome = (genome_static&)pGAStatic->population().individual(i);

			for (int j = 0; j < genome.size(); j++) {
				sGenome += string(genome.gene(j)) + " ";
			}
			logger->debug("Genome " + StringUtilities::toString<int>(i) + ": " + sGenome);
		}

		// Evolve the genetic algorithm
		logger->info("Evolving the static genetic algorithm (find the routes)...");
		std::cout << "Evolving..." << std::endl;
		std::cout.flush();

		time_t begin = time(NULL);

		while (!pGAStatic->done()) {
			pGAStatic->step();
			std::cout << ".";
			if (pGAStatic->generation() % 10 == 0) {
				std::cout << pGAStatic->generation() << std::endl;
			}
			std::cout.flush();
		}

		std::cout << std::endl;
		logger->info("Evolution done");

		time_t end = time(NULL);

		std::cout << "Processing time: " << (end - begin) << " seconds" << std::endl;

		// Get the best solution
		const genome_static& best = (genome_static&) pGAStatic->statistics().bestIndividual();

		// Register the GA static solution
		setStaticSolution(best);

	} catch (const std::exception& ex) {
		logger->error("Fail to evolve the genetic algorithm");
		return 1;
	}

	return 0;
}


//................................................. Set the static solution ...
void GARouter::setStaticSolution(const genome_static& best) {
	if (best.size() > 0) {
		pStaticSolution = std::unique_ptr<genome_static>(new genome_static(best));
	}
}


//................................................. Get the static solution ...
genome_static GARouter::getStaticSolution(void) const {
	if (pStaticSolution != nullptr) {
		return *pStaticSolution;
	}

	// Return an empty genome
	return genome_static(0, GAAlleleSet<const char*>());
}


//................................................. Print the static solution ...
void GARouter::printStaticSolution(void) const {
	// Print the best individual resulting from the GA evolution for the static approach
	std::cout << "Best solution:" << std::endl;
	for (int i = 0; i < pStaticSolution->size(); i++) {
		string routename = pStaticSolution->gene(i);

		std::cout << routename << std::endl;

		std::shared_ptr<const GARRouteDesc> pRd = pStaticObjData->getpRouCont()->getRouteDesc(routename);
		if (pRd == nullptr) {
			continue;
		}

		// Print the route edges
		for (const ROEdge const* pEdge : pRd->edges2Pass) {
			std::cout << std::setw(12) << pEdge->getID();
		}
		std::cout << std::endl;
	}

	std::cout << "Score is: " << pStaticSolution->score() << std::endl;
	std::cout << "Statistics:\n" << pGAStatic->statistics() << std::endl;
	std::cout << "Parameters:\n" << pGAStatic->parameters() << std::endl;
}


//................................................. Build the data required in the objective function ...
bool GARouter::triggerDynamicGA(void) const {
	if (!options.isSet("dynamic-trigger")) {
		logger->error("The 'dynamic-trigger' option is not set.");
		return false;
	}

	float trigger = options.getFloat("dynamic-trigger");

	if (trigger < 0.0 || trigger > 1.0) {
		logger->error("Trigger value [" + StringUtilities::toString<float>(trigger) + "] is out of bounds.");
		return false;
	}

	if (pStaticSolution->score() >= trigger * 100.0) {
		return true;
	}

	return false;
}


//................................................. Build the data required in the objective function ...
int GARouter::buildGADynObjectiveData(void) {
	// Set the input files to feed the SUMO simulation
	if (options.isSet("net-file")) {
		logger->info("Set net file [" + options.getString("net-file") + "] to objective data");
		pDynObjData->setNetFile(options.getString("net-file"));
	} else {
		logger->error("Network file is not specified in the 'net-file' option.");
		return 1;
	}

	/*
	if (options.isSet("measure-files")) {
		logger->info("Set measure files [" + options.getString("measure-files") + "] to objective data");
		pDynObjData->setFlowFile(options.getString("measure-files"));
	} else {
		logger->error("Detector flow measurements file is not specified in the 'measure-file' option.");
		return 1;
	}
	*/

	if (options.isSet("routes-output")) {
		logger->info("Set routes output [" + options.getString("routes-output") + "] to objective data");
		pDynObjData->setRouFile(options.getString("routes-output"));
	} else {
		logger->error("Route description file is not specified in the 'routes-output' option.");
		return 1;
	}

	if (options.isSet("emitters-output")) {
		logger->info("Set emitters output [" + options.getString("emitters-output") + "] to objective data");
		pDynObjData->setEmitFile(options.getString("emitters-output"));
	} else {
		logger->error("Emitter file is not specified in the 'emitters-output' option.");
		return 1;
	}

	if (options.isSet("induction-loops-output")) {
		logger->info("Set induction loops output [" + options.getString("induction-loops-output") + "] to objective data");
		pDynObjData->setLoopFile(options.getString("induction-loops-output"));
	} else {
		logger->error("Induction loop file is not specified in the 'induction-loops-output' option.");
		return 1;
	}

	// Set the beginning and end processing times
	if (options.isSet("begin")) {
		logger->info("Set begin time [" + options.getString("begin") + "] to objective data");
		pDynObjData->setBegin(StringUtilities::toInt(options.getString("begin")));
	} else {
		logger->error("Begin time is not specified in the 'begin' option.");
		return 1;
	}

	if (options.isSet("end")) {
		logger->info("Set end time [" + options.getString("end") + "] to objective data");
		pDynObjData->setEnd(StringUtilities::toInt(options.getString("end")));
	} else {
		logger->error("End time is not specified in the 'end' option.");
		return 1;
	}

	// Set the time offset
	if (options.isSet("time-offset")) {
		logger->info("Set offset time [" + options.getString("time-offset") + "] to objective data");
		pDynObjData->setOffset(StringUtilities::toInt(options.getString("time-offset")));
	} else {
		logger->error("Offset time is not specified in the 'time-offset' option.");
		return 1;
	}

	// Set the time step
	if (options.isSet("time-step")) {
		logger->info("Set time step [" + options.getString("time-step") + "] to objective data");
		pDynObjData->setTimeStep(StringUtilities::toInt(options.getString("time-step")));
	} else {
		logger->error("Time step is not specified in the 'time-step' option.");
		return 1;
	}

	// Set the time to teleport
	if (options.isSet("time-to-teleport")) {
		logger->info("Set time to teleport [" + options.getString("time-to-teleport") + "] to objective data");
		pDynObjData->setTimeToTeleport(StringUtilities::toInt(options.getString("time-to-teleport")));
	} else {
		logger->error("Time to teleport is not specified in the 'time-to-teleport' option.");
		return 1;
	}

	// Set the detector flow data
	if (!pFlows->getFlows().empty()) {
		logger->info("Set the detector flows to objective data");
		pDynObjData->setpDetFlows(pFlows.get());
	} else {
		logger->error("No detector flows were loaded.");
		return 1;
	}

	// Set the network data
	if (pNet) {
		logger->info("Set the network data to objective data");
		pDynObjData->setpNet(pNet.get());
	} else {
		logger->error("No network data was loaded.");
		return 1;
	}

	// Set the routes container
	if (pRouCont && !pRouCont->get().empty()) {
		logger->info("Set the route container to objective data");
		pDynObjData->setpRouCont(pRouCont.get());
	} else {
		logger->error("No routes were loaded.");
		return 1;
	}

	// Set the detectors container
	if (pDetCont && !pDetCont->getDetectors().empty()) {
		logger->info("Set the detector container to objective data");
		pDynObjData->setpDetCont(pDetCont.get());
	} else {
		logger->error("No detectors were loaded.");
		return 1;
	}

	// Set the solution of the static approach
	if (pStaticSolution != nullptr) {
		pDynObjData->setRoutesSolution(*pStaticSolution);
	} else {
		logger->error("The static solution is not computed.");
		return 1;
	}

	// Write the induction-loop file
	logger->info("Write the induction loop file [" + pDynObjData->getLoopFile() + "]");
	int status = this->writeInductionLoopFile(pDynObjData->getLoopFile());
	if (status < 0) {
		return 1;
	}
	logger->info("Induction loop file [" + pDynObjData->getLoopFile() + "] successfully written");

	return 0;
}


//................................................. Build the genome for the dynamic approach ...
int GARouter::buildGADynGenome(void) {
	if (pStaticSolution == nullptr) {
		logger->fatal("No solution for the dynamic approach has been calculated");
		return 1;
	}

	if (pDynObjData == nullptr) {
		logger->fatal("No objective data has been created for the dynamic approach");
		return 1;
	}

	try {
		// Build the allele set of the dynamic approach
		GAAlleleSetArray<int> alleleArray = buildGADynAlleleSetArray();
		if (alleleArray.size() == 0) {
			return 0;
		}

		// Build the genome for the static approach
		pDynGenome = std::unique_ptr<genome_dyn>(new genome_dyn(alleleArray,
														  	   	GARDynObjective::objective,
																pDynObjData.get()));

		// Set the genetic operators
		setGAGenomeInitializer(pDynGenome.get());
		setGAGenomeMutator(pDynGenome.get());
		setGAGenomeCrossover(pDynGenome.get());

	} catch (const std::exception& ex) {
		logger->error("Fail to build genome for the dynamic approach: " + string(ex.what()));
		return 1;
	}

	return 0;
}


//................................................. Build the genetic algorithm engine for the dynamic approach ...
int GARouter::buildGADynGeneticAlgorithm(void) {
	if (pDynGenome == nullptr) {
		logger->fatal("The GA genome of the dynamic approach hasn't been built.");
		return 1;
	}

	// Get the genetic algorithm type from the input options
	if (!options.isSet("genetic-algorithm")) {
		logger->fatal("Option 'genetic-algorithm' is not specified.");
		return 1;
	}
	string gaType = options.getString("genetic-algorithm");

	// Create the suitable genetic algorithm engine
	GARGaFactory* gaFactory = new GARGaFactory();

	logger->info("Create a [" + gaType + "] genetic algorithm for the static approach");
	pGADyn = std::unique_ptr<GAGeneticAlgorithm>(gaFactory->createGeneticAlgorithm(gaType,
						 														   pDynGenome.get(),
																				   params,
																				   logger));
	delete gaFactory;

	if (pGADyn == nullptr) {
		logger->fatal("Fail to build the dynamic genetic algorithm engine for type [" + gaType + "]");
		return 1;
	}

	int result = 0;

	// Set the scaling scheme
	logger->info(">> Set the Scaling Scheme");
	result = setGAScalingScheme(pGADyn.get());
	if (result > 0) {
		return result;
	}

	// Set the selection scheme
	logger->info(">> Set the Selection Scheme");
	result = setGASelectionScheme(pGADyn.get());
	if (result > 0) {
		return result;
	}

	// Set the termination function
	logger->info(">> Set the Termination Function");
	result = setGATerminationFunction(pGADyn.get());
	if (result > 0) {
		return result;
	}

	// Set the replacement scheme for incremental GA
	if (gaType == "Incremental") {
		logger->info(">> Set the Replacement Scheme");
		result = setGAReplacementScheme(pGADyn.get());
		if (result > 0) {
			return result;
		}
	}

	return 0;
}


//................................................. Evolve the genetic algorithm for the dynamic approach ...
int GARouter::evolveDynGA(void) {
	if (pGADyn == nullptr) {
		logger->fatal("The genetic algorithm engine for the dynamic approach is not created.");
		return 1;
	}

	if (pDynGenome == nullptr) {
		logger->fatal("The GA genome for the dynamic approach is not defined.");
		return 1;
	}

	if (params.size() == 0) {
		logger->fatal("No parameters for the genetic algorithm are defined.");
		return 1;
	}

	try {
		// Set the parameters to the dynamic genetic algorithm engine
		pGADyn->parameters(params);

		// Set the score file name
		string scoreFile = buildScoreFileName("dyn_");
		pGADyn->scoreFilename(scoreFile.c_str());

		// Initialize the dynamic genetic algorithm using a seed if reported
		logger->info("Initializing the dynamic GA for the static approach...");
		if (options.isSet("ga-seed")) {
			pGADyn->initialize(options.getInt("ga-seed"));
		} else {
			pGADyn->initialize();
		}

		// Print the initial population
		logger->debug("Initial population:");
		for (int i = 0; i < pGADyn->populationSize(); i++) {
			string sGenome = "";
			const genome_dyn& genome = (genome_dyn&)pGADyn->population().individual(i);

			for (int j = 0; j < genome.size(); j++) {
				sGenome += StringUtilities::toString<int>(genome.gene(j)) + " ";
			}
			logger->debug("Genome " + StringUtilities::toString<int>(i) + ": " + sGenome);
		}

		// Evolve the dynamic genetic algorithm
		logger->info("Evolving the dynamic genetic algorithm router (find the depart time)...");
		std::cout << "Evolving..." << std::endl;
		//pGA->evolve(seed);
		std::cout.flush();
		while (!pGADyn->done()) {
			pGADyn->step();
			std::cout << ".";
			if (pGADyn->generation() % 10 == 0) {
				std::cout << pGADyn->generation() << std::endl;
			}
			std::cout.flush();
		}
		std::cout << std::endl;
		logger->info("Evolution done");

		// Get the best solution
		const genome_dyn& best = (genome_dyn&) pGADyn->statistics().bestIndividual();

	} catch (const std::exception& ex) {
		logger->error("Fail to evolve the genetic algorithm: " + string(ex.what()) );
		return 1;
	}

	return 0;
}


//................................................. Set the initialization operator ...
void GARouter::setGAGenomeInitializer(GAGenome* pGenome) {
	string initializer = options.getString("genome-initializer");

	logger->info("Set [" + initializer + "] initialization operator");

	try {
		if (StringUtilities::trim(initializer) == "Uniform") {
			if (typeid(*pGenome) == typeid(genome_static)) {
				static_cast<genome_static*>(pGenome)->initializer(genome_static::UniformInitializer);
			}
			if (typeid(*pGenome) == typeid(genome_dyn)) {
				static_cast<genome_dyn*>(pGenome)->initializer(genome_dyn::UniformInitializer);
			}
			return;
		}

		if (StringUtilities::trim(initializer) == "Ordered") {
			if (typeid(*pGenome) == typeid(genome_static)) {
				static_cast<genome_static*>(pGenome)->initializer(genome_static::OrderedInitializer);
			}
			if (typeid(*pGenome) == typeid(genome_dyn)) {
				static_cast<genome_dyn*>(pGenome)->initializer(genome_dyn::OrderedInitializer);
			}
			return;
		}

		logger->error("Unknown initialization operator [" + initializer + "]");
		if (typeid(*pGenome) == typeid(genome_static)) {
			static_cast<genome_static*>(pGenome)->initializer(genome_static::NoInitializer);
		}
		if (typeid(*pGenome) == typeid(genome_dyn)) {
			static_cast<genome_dyn*>(pGenome)->initializer(genome_dyn::NoInitializer);
		}

	} catch (std::bad_cast &ex) {
		logger->fatal("Trying to set the initializer to an unknown genome type [" + string(typeid(*pGenome).name()) + "]");
	}
}


//................................................. Set the mutation operator ...
void GARouter::setGAGenomeMutator(GAGenome* pGenome) {
	string mutator = options.getString("genome-mutator");

	logger->info("Set [" + mutator + "] mutation operator");

	try {
		if (StringUtilities::trim(mutator) == "Flip") {
			if (typeid(*pGenome) == typeid(genome_static)) {
				static_cast<genome_static*>(pGenome)->mutator(genome_static::FlipMutator);
			}
			if (typeid(*pGenome) == typeid(genome_dyn)) {
				static_cast<genome_dyn*>(pGenome)->mutator(genome_dyn::FlipMutator);
			}
			return;
		}

		if (StringUtilities::trim(mutator) == "Swap") {
			if (typeid(*pGenome) == typeid(genome_static)) {
				static_cast<genome_static*>(pGenome)->mutator(genome_static::SwapMutator);
			}
			if (typeid(*pGenome) == typeid(genome_dyn)) {
				static_cast<genome_dyn*>(pGenome)->mutator(genome_dyn::SwapMutator);
			}
			return;
		}

		logger->error("Unknown mutation operator [" + mutator + "]");
		if (typeid(*pGenome) == typeid(genome_static)) {
			static_cast<genome_static*>(pGenome)->mutator(genome_static::NoMutator);
		}
		if (typeid(*pGenome) == typeid(genome_dyn)) {
			static_cast<genome_dyn*>(pGenome)->mutator(genome_dyn::NoMutator);
		}

	} catch (std::bad_cast &ex) {
		logger->error("Trying to set the mutator to an unknown genome type [" + string(typeid(*pGenome).name()) + "]");
	}
}


//................................................. Set the crossover operator ...
void GARouter::setGAGenomeCrossover(GAGenome* pGenome) {
	string crossover = options.getString("genome-crossover");

	logger->info("Set [" + crossover + "] crossover operator");

	try {
		if (StringUtilities::trim(crossover) == "Cycle") {
			if (typeid(*pGenome) == typeid(genome_static)) {
				static_cast<genome_static*>(pGenome)->crossover(genome_static::CycleCrossover);
			}
			if (typeid(*pGenome) == typeid(genome_dyn)) {
				static_cast<genome_dyn*>(pGenome)->crossover(genome_dyn::CycleCrossover);
			}
			return;
		}

		if (StringUtilities::trim(crossover) == "EvenOdd") {
			if (typeid(*pGenome) == typeid(genome_static)) {
				static_cast<genome_static*>(pGenome)->crossover(genome_static::EvenOddCrossover);
			}
			if (typeid(*pGenome) == typeid(genome_dyn)) {
				static_cast<genome_dyn*>(pGenome)->crossover(genome_dyn::EvenOddCrossover);
			}
			return;
		}

		if (StringUtilities::trim(crossover) == "OnePoint") {
			if (typeid(*pGenome) == typeid(genome_static)) {
				static_cast<genome_static*>(pGenome)->crossover(genome_static::OnePointCrossover);
			}
			if (typeid(*pGenome) == typeid(genome_dyn)) {
				static_cast<genome_dyn*>(pGenome)->crossover(genome_dyn::OnePointCrossover);
			}
			return;
		}

		if (StringUtilities::trim(crossover) == "Order") {
			if (typeid(*pGenome) == typeid(genome_static)) {
				static_cast<genome_static*>(pGenome)->crossover(genome_static::OrderCrossover);
			}
			if (typeid(*pGenome) == typeid(genome_dyn)) {
				static_cast<genome_dyn*>(pGenome)->crossover(genome_dyn::OrderCrossover);
			}
			return;
		}

		if (StringUtilities::trim(crossover) == "PartialMatch") {
			if (typeid(*pGenome) == typeid(genome_static)) {
				static_cast<genome_static*>(pGenome)->crossover(genome_static::PartialMatchCrossover);
			}
			if (typeid(*pGenome) == typeid(genome_dyn)) {
				static_cast<genome_dyn*>(pGenome)->crossover(genome_dyn::PartialMatchCrossover);
			}
			return;
		}

		if (StringUtilities::trim(crossover) == "TwoPoint") {
			if (typeid(*pGenome) == typeid(genome_static)) {
				static_cast<genome_static*>(pGenome)->crossover(genome_static::TwoPointCrossover);
			}
			if (typeid(*pGenome) == typeid(genome_dyn)) {
				static_cast<genome_dyn*>(pGenome)->crossover(genome_dyn::TwoPointCrossover);
			}
			return;
		}

		if (StringUtilities::trim(crossover) == "Uniform") {
			if (typeid(*pGenome) == typeid(genome_static)) {
				static_cast<genome_static*>(pGenome)->crossover(genome_static::UniformCrossover);
			}
			if (typeid(*pGenome) == typeid(genome_dyn)) {
				static_cast<genome_dyn*>(pGenome)->crossover(genome_dyn::UniformCrossover);
			}
			return;
		}

		logger->warning("Unknown crossover operator [" + crossover + "]. Set the 'OnePointCrossover' operator by default.");
		if (typeid(*pGenome) == typeid(genome_static)) {
			static_cast<genome_static*>(pGenome)->crossover(genome_static::OnePointCrossover);
		}
		if (typeid(*pGenome) == typeid(genome_dyn)) {
			static_cast<genome_dyn*>(pGenome)->crossover(genome_dyn::OnePointCrossover);
		}

	} catch (std::bad_cast &ex) {
		logger->error("Trying to set the crossover to an unknown genome type [" + string(typeid(*pGenome).name()) + "]");
	}
}


//................................................. Set the scaling scheme ...
int GARouter::setGAScalingScheme(GAGeneticAlgorithm* pGA) {
	// Get the scaling scheme from the input options
	if (!options.isSet("scaling-scheme")) {
		logger->fatal("Option 'scaling-scheme' is not specified.");
		return 1;
	}
	string scaling = options.getString("scaling-scheme");

	// Create the suitable scaling scheme
	GARScalingFactory* scalingFactory = new GARScalingFactory(options);

	std::unique_ptr<GAScalingScheme> pScaling = std::unique_ptr<GAScalingScheme>(scalingFactory->createScalingScheme(pGA, scaling, logger));

	delete scalingFactory;

	if (pScaling == nullptr) {
		logger->error("Fail to build the scaling scheme.");
		return 1;
	}

	if (pGA == nullptr) {
		logger->error("Fail to set the scaling scheme: The genetic algorithm is not built.");
		return 1;
	}

	logger->info("Set a [" + scaling + "] scaling scheme");
	pGA->scaling(*pScaling);

	return 0;
}


//................................................. Set the selection scheme ...
int GARouter::setGASelectionScheme(GAGeneticAlgorithm* pGA) {
	// Get the selection scheme from the input options
	if (!options.isSet("selection-scheme")) {
		logger->fatal("Option 'selection-scheme' is not specified.");
		return 1;
	}
	string selection = options.getString("selection-scheme");

	// Create the suitable selection scheme
	GARSelectionFactory* selectFactory = new GARSelectionFactory();

	std::unique_ptr<GASelectionScheme> pSelector = std::unique_ptr<GASelectionScheme>(selectFactory->createSelectionScheme(selection, logger));

	delete selectFactory;

	if (pSelector == nullptr) {
		logger->error("Fail to build the selection scheme.");
		return 1;
	}

	if (pGA == nullptr) {
		logger->error("Fail to set the selection scheme: The genetic algorithm is not built.");
		return 1;
	}

	logger->info("Set a [" + selection + "] selection scheme");
	pGA->selector(*pSelector);

	return 0;
}


//................................................. Set the termination function ...
int GARouter::setGATerminationFunction(GAGeneticAlgorithm* pGA) {
	// Get the selection scheme from the input options
	if (!options.isSet("termination-function")) {
		logger->fatal("Option 'termination-function' is not specified.");
		return 1;
	}
	string termination = options.getString("termination-function");

	logger->info("Set [" + termination + "] termination function");

	if (StringUtilities::trim(termination) == "UponGeneration") {
		pGA->terminator(GAGeneticAlgorithm::TerminateUponGeneration);
		return 0;
	}

	if (StringUtilities::trim(termination) == "UponConvergence") {
		pGA->terminator(GAGeneticAlgorithm::TerminateUponConvergence);
		return 0;
	}

	if (StringUtilities::trim(termination) == "UponPopConvergence") {
		pGA->terminator(GAGeneticAlgorithm::TerminateUponPopConvergence);
		return 0;
	}

	logger->warning("Unknown termination function [" + termination + "].");
	return 1;
}


//................................................. Set the replacement scheme ...
int GARouter::setGAReplacementScheme(GAGeneticAlgorithm* pGA) {
	// The replacement scheme only concerns to incremental GA
	if (typeid(*pGA) != typeid(GAIncrementalGA)) {
		logger->warning("Replacement scheme can't be applied to a non-Incremental GA.");
		return 0;
	}

	// Get the selection scheme from the input options
	if (!options.isSet("replacement-scheme")) {
		logger->fatal("Option 'replacement-scheme' is not specified.");
		return 1;
	}
	string replacement = options.getString("replacement-scheme");

	if (StringUtilities::trim(replacement) == "Random") {
		if (typeid(*pGA) == typeid(GAIncrementalGA)) {
			dynamic_cast<GAIncrementalGA*>(pGA)->replacement(GAIncrementalGA::RANDOM);
		}
		return 0;
	}

	if (StringUtilities::trim(replacement) == "Best") {
		if (typeid(*pGA) == typeid(GAIncrementalGA)) {
			dynamic_cast<GAIncrementalGA*>(pGA)->replacement(GAIncrementalGA::BEST);
		}
		return 0;
	}

	if (StringUtilities::trim(replacement) == "Worst") {
		if (typeid(*pGA) == typeid(GAIncrementalGA)) {
			dynamic_cast<GAIncrementalGA*>(pGA)->replacement(GAIncrementalGA::WORST);
		}
		return 0;
	}

	if (StringUtilities::trim(replacement) == "Custom") {
		if (typeid(*pGA) == typeid(GAIncrementalGA)) {
			dynamic_cast<GAIncrementalGA*>(pGA)->replacement(GAIncrementalGA::CUSTOM);
		}
		return 0;
	}

	if (StringUtilities::trim(replacement) == "Crowding") {
		if (typeid(*pGA) == typeid(GAIncrementalGA)) {
			dynamic_cast<GAIncrementalGA*>(pGA)->replacement(GAIncrementalGA::CROWDING);
		}
		return 0;
	}

	if (StringUtilities::trim(replacement) == "Parent") {
		if (typeid(*pGA) == typeid(GAIncrementalGA)) {
			dynamic_cast<GAIncrementalGA*>(pGA)->replacement(GAIncrementalGA::PARENT);
		}
		return 0;
	}

	logger->error("Unknown replacement scheme in 'replacement-scheme' option");
	return 1;
}


//................................................. Set the genetic algorithm parameters ...
int GARouter::setGAParameters(void) {
	try {
		if (options.isSet("number-of-generations")) {
			int numGenerations = options.getInt("number-of-generations");
			logger->info("Set GA parameter 'number-of-generations': [" + StringUtilities::toString<int>(numGenerations) + "]");
			params.set(gaNnGenerations, numGenerations);
		}
		if (options.isSet("population-size")) {
			int populationSize = options.getInt("population-size");
			logger->info("Set GA parameter 'population-size': [" + StringUtilities::toString<int>(populationSize) + "]");
			params.set(gaNpopulationSize, populationSize);
		}
		if (options.isSet("number-of-populations")) {
			int numPopulations = options.getInt("number-of-populations");
			logger->info("Set GA parameter 'number-of-populations': [" + StringUtilities::toString<int>(numPopulations) + "]");
			params.set(gaNnPopulations, numPopulations);
		}
		if (options.isSet("convergence-percentage")) {
			float convergencePercent = options.getFloat("convergence-percentage");
			logger->info("Set GA parameter 'convergence-percentage': [" + StringUtilities::toString<float>(convergencePercent) + "]");
			params.set(gaNpConvergence, convergencePercent);
		}
		if (options.isSet("crossover-probability")) {
			float crossoverProb = options.getFloat("crossover-probability");
			logger->info("Set GA parameter 'crossover-probability': [" + StringUtilities::toString<float>(crossoverProb) + "]");
			params.set(gaNpCrossover, crossoverProb);
		}
		if (options.isSet("mutation-probability")) {
			float mutationProb = options.getFloat("mutation-probability");
			logger->info("Set GA parameter 'mutation-probability': [" + StringUtilities::toString<float>(mutationProb) + "]");
			params.set(gaNpMutation, mutationProb);
		}
		if (options.isSet("replacement-percentage")) {
			float replacementPercent = options.getFloat("replacement-percentage");
			logger->info("Set GA parameter 'replacement-percentage': [" + StringUtilities::toString<float>(replacementPercent) + "]");
			params.set(gaNpReplacement, replacementPercent);
		}
		if (options.isSet("replacement-number")) {
			int replacementNum = options.getInt("replacement-number");
			logger->info("Set GA parameter 'replacement-number': [" + StringUtilities::toString<float>(replacementNum) + "]");
			params.set(gaNnReplacement, replacementNum);
		}
		if (options.isSet("mini-maxi-scaling")) {
			int miniMaxi = options.getInt("mini-maxi-scaling");
			logger->info("Set GA parameter 'mini-maxi-scaling': [" + StringUtilities::toString<int>(miniMaxi) + "]");
			params.set(gaNminimaxi, miniMaxi);
		}
		if (options.isSet("number-of-best")) {
			int numBest = options.getInt("number-of-best");
			logger->info("Set GA parameter 'number-of-best': [" + StringUtilities::toString<int>(numBest) + "]");
			params.set(gaNnBestGenomes, numBest);
		}
		if (options.isSet("score-freq")) {
			int scoreFreq = options.getInt("score-freq");
			logger->info("Set GA parameter 'score-freq': [" + StringUtilities::toString<int>(scoreFreq) + "]");
			params.set(gaNscoreFrequency, scoreFreq);
		}
		if (options.isSet("flush-freq")) {
			int flushFreq = options.getInt("flush-freq");
			logger->info("Set GA parameter 'flush-freq': [" + StringUtilities::toString<int>(flushFreq) + "]");
			params.set(gaNflushFrequency, flushFreq);
		}
		if (options.isSet("score-file")) {
			scoreFilename = options.getString("score-file");
			//logger->info("Set GA parameter 'score-file': [" + string(scoreFilename) + "]");
			//params.set(gaNscoreFilename, scoreFilename.c_str());
		}
		if (options.isSet("select-scores")) {
			string selectScores = options.getString("select-scores");
			logger->info("Set GA parameter 'select-scores': [" + selectScores + "]");
			int stats = getGAStatisticsSelectScores(selectScores);
			params.set(gaNselectScores, stats);
		}
		if (options.isSet("elitism")) {
			bool elitism = options.getBool("elitism");
			logger->info("Set GA parameter 'elitism': [" + StringUtilities::toString<bool>(elitism) + "]");
			params.set(gaNelitism, elitism);
		}
		if (options.isSet("number-of-offspring")) {
			int numOffsprings = options.getInt("number-of-offspring");
			logger->info("Set GA parameter 'number-of-offspring': [" + StringUtilities::toString<int>(numOffsprings) + "]");
			params.set(gaNnOffspring, numOffsprings);
		}
		if (options.isSet("record-diversity")) {
			bool recordDiversity = options.getBool("record-diversity");
			logger->info("Set GA parameter 'record-diversity': [" + StringUtilities::toString<bool>(recordDiversity) + "]");
			params.set(gaNrecordDiversity, recordDiversity);
		}
		if (options.isSet("migration-percentage")) {
			float migrationPercent = options.getFloat("migration-percentage");
			logger->info("Set GA parameter 'migration-percentage': [" + StringUtilities::toString<bool>(migrationPercent) + "]");
			params.set(gaNpMigration, migrationPercent);
		}
		if (options.isSet("migration-number")) {
			int migrationNum = options.getInt("migration-number");
			logger->info("Set GA parameter 'migration-number': [" + StringUtilities::toString<bool>(migrationNum) + "]");
			params.set(gaNnMigration, migrationNum);
		}

	} catch (const std::exception& ex) {
		logger->error("Fail to set the genetic algorithms parameters: " + string(ex.what()));
		return 1;
	}

	logger->info("Parameters are successfully set");
	return 0;
}


//................................................. Logs the route data set ...
void GARouter::logRoutes(void) const {
	logger->info("Route list:");
	for (const GARRouteDesc& rd : pRouCont->get()) {
		this->logRouteDesc(rd);
	}
}


//................................................. Logs the route description ...
void GARouter::logRouteDesc(const GARRouteDesc& rd) const {
	logger->info("\tRoute [" + rd.routename
			+ "], distance [" + StringUtilities::toString(rd.distance)
			+ "], duration [" + StringUtilities::toString(rd.duration_2)
			//+ "], passedNo [" + StringUtilities::toString(rd.passedNo)
			//+ "], endDetEdge [" + rd.endDetectorEdge->getID()
			//+ "], lastDetEdge [" + rd.lastDetectorEdge->getID()
			+ "], distance2last [" + StringUtilities::toString(rd.distance2Last)
			+ "], duration2last [" + StringUtilities::toString(rd.duration2Last)
			+ "], overallProb [" + StringUtilities::toString(rd.overallProb)
			+ "], factor [" + StringUtilities::toString(rd.factor) + "]");
	logger->info("\t\tedges [" + GARUtils::toString(rd.edges2Pass) + "]");
}


//................................................. Logs the route data set ...
void GARouter::logTrips(void) const {
	logger->info("Trip list:");
	for (shared_ptr<const GARTrip> pTrip : pTripCont->getOdTrips()) {
		logger->info("Trip origin [" + pTrip->getpOrig()->getID() + "], destination [" + pTrip->getpDest()->getID() + "]");
		for (shared_ptr<const GARRouteDesc> pRd : pTrip->getOdRoutes()) {
			this->logRouteDesc(*pRd);
		}
	}
}


//................................................. Build the network graph used by the k-shortest-path algorithm ...
Graph GARouter::buildKShortestGraph(void) const {
	Graph graph;
	const int numNodes = pNet->getNodeList().size();

	graph.set_number_vertices(numNodes);

	// Get the network edges
	for (auto edge : pNet->getEdgeMap()) {
		string edgeId = edge.first;
		const ROEdge const* pEdge = edge.second;

		// Refuse internal edges
		if (pEdge->getType() == ROEdge::EdgeType::ET_INTERNAL) {
			continue;
		}

		// Get the from and to nodes and the duration of the edge
		std::string fromNodeId = pEdge->getFromNode()->getID();
		std::string toNodeId = pEdge->getToNode()->getID();
		double duration = pEdge->getLength() / pEdge->getSpeed();

		// Add the link to the graph
		graph.add_link(fromNodeId, toNodeId, duration);
	}

	// Set the number of vertices and edges
	graph.setv();

	return graph;
}


//................................................. Get the shortest paths ...
std::vector<GARRouteDesc> GARouter::getKShortestRoutes(const Graph& graph,
												       const GARDetector const* pSourceDet,
													   const GARDetector const* pSinkDet,
													   int k,
													   int kSelected) const {
	vector<GARRouteDesc> kShortestRoutes;

	string sourceEdgeId = pSourceDet->getEdgeID();
	string sinkEdgeId = pSinkDet->getEdgeID();

	//Get the origin and destination nodes
	const ROEdge const* pSourceEdge = pNet->getEdge(sourceEdgeId);
	const ROEdge const* pSinkEdge = pNet->getEdge(sinkEdgeId);

	string fromNode = pSourceEdge->getFromNode()->getID();
	string toNode = pSinkEdge->getToNode()->getID();

	//Get the K-shortest routes from the origin to the destination nodes
	logger->debug("Shortest paths from [" + fromNode + "] to [" + toNode + "]");
	YenTopKShortestPathsAlg shortestPaths (graph,
										   graph.get_vertex(fromNode),
										   graph.get_vertex(toNode));

	for (int i = 0; i < k  &&  shortestPaths.has_next(); i++) {
		BasePath* path = shortestPaths.next();
		logger->debug("\tPath " + StringUtilities::toString<int>(i) + ": [" + path->toString() + "]");

		//Build the route description
		GARRouteDesc rd = buildKShortestRouteDesc(path, sourceEdgeId, sinkEdgeId, i);
		kShortestRoutes.push_back(rd);
	}

	return getSelectedRoutes(kShortestRoutes, kSelected);
}


//................................................. Get the selected shortest routes randomly ...
std::vector<GARRouteDesc> GARouter::getSelectedRoutes(const std::vector<GARRouteDesc>& kShortestRoutes,
								 	 	 	 	 	  int kSelected) const {

	vector<GARRouteDesc> selectedRoutes = kShortestRoutes;

	// Select the shortest routes randomly
	std::random_device rd;
	std::mt19937 mt(rd());
	while (selectedRoutes.size() > kSelected) {
		std::uniform_real_distribution<double> dist(0, selectedRoutes.size());
		int index = int(dist(mt));
		logger->debug("Erase shortest route at [" + StringUtilities::toString<int>(int(index)) + "]");
		selectedRoutes.erase(selectedRoutes.begin() + index);
	}

	return selectedRoutes;
}


//................................................. Add the shortest routes to the router containers ...
int GARouter::addKShortestRoutes(const std::vector<GARRouteDesc>& kShortestRoutes,
								 const GARDetector const* pSourceDet,
								 const GARDetector const* pSinkDet) {
	//Get the origin and destination nodes
	const ROEdge const* pSourceEdge = pNet->getEdge(pSourceDet->getEdgeID());
	const ROEdge const* pSinkEdge = pNet->getEdge(pSinkDet->getEdgeID());

	try {
		// Register the selected shortest routes
		for (GARRouteDesc rd : kShortestRoutes) {
			//Add the route description to the source detector, the route and trip containers
			const_cast<GARDetector*>(pSourceDet)->addRoute(rd);
			std::vector<GARRouteDesc>::iterator it = pRouCont->addRouteDesc(rd);
			pTripCont->addRouteDesc(dynamic_cast<GAREdge*>(const_cast<ROEdge*>(pSourceEdge)),
									dynamic_cast<GAREdge*>(const_cast<ROEdge*>(pSinkEdge)),
									*it);
		}
	} catch (const std::exception& ex) {
		logger->error("Fail to add k-shortest routes from [" + pSourceDet->getID() + "] to [" + pSinkDet->getID() + "]: " + string(ex.what()));
		return 1;
	}

	return 0;
}


//................................................. Build the shortest route description ...
GARRouteDesc GARouter::buildKShortestRouteDesc(const BasePath const* path,
		  	  	  	  	  	  	  	  	  	   const std::string& sourceEdgeId,
											   const std::string& sinkEdgeId,
											   int routeNum) const {
	GARRouteDesc rd;
	string suffix = (routeNum > 0) ? ("_" + StringUtilities::toString<int>(routeNum)) : "";
	rd.routename = sourceEdgeId + "_to_" + sinkEdgeId + suffix;
	rd.duration_2 = path->Weight();
	rd.distance = 0;
	rd.distance2Last = 0;
	rd.duration2Last = 0;
	rd.endDetectorEdge = nullptr;
	rd.lastDetectorEdge = nullptr;
	rd.overallProb = 0;
	rd.factor = 1.;
	rd.passedNo = 0;

	// Add edges to the route description
	for (int i = 0; i < path->length()-1; i++) {
		const BaseVertex* fromVertex = path->GetVertex(i);
		const BaseVertex* toVertex = path->GetVertex(i+1);
		const ROEdge const* pEdge = pNet->getpEdge(fromVertex->getID(), toVertex->getID());

		rd.edges2Pass.push_back(const_cast<ROEdge*>(pEdge));

		// Compute the number of passed edges and the route distance
		rd.passedNo++;
		rd.distance+=pEdge->getLength();
	}

	return rd;
}


//................................................. Get the number of sink/source detectors with flow data ...
unsigned int GARouter::getNumFlowDetectorsByType(const GARDetectorType& type) const {
	unsigned int numFlowDets = 0;
	vector<GARDetector*> detectors = pDetCont->getDetectorsByType(type);

	for (const GARDetector const* pDet : detectors) {
		string detId = pDet->getID();

		if (!pFlows->getFlowDefs(detId).empty()) {
			numFlowDets ++;
		}
	}

	return numFlowDets;
}


//................................................. Get the accumulative flow amounts from a group of detectors ...
unsigned int GARouter::getFlowAmounts(vector<GARDetector*> detectors) const {
	unsigned int accFlow = 0;

	for (auto pDet : detectors) {
		string detId = pDet->getID();
		for (auto flow : pFlows->getFlowDefs(detId)) {
			accFlow += (flow.qPKW + flow.qLKW);
		}
	}

	return accFlow;
}


//................................................. Write the induction loop file ...
int GARouter::writeInductionLoopFile(const string& loopFileName) {
	int result = 1;

	// Get the measurement frequency from the options
	if (!options.isSet("meas-freq")) {
		logger->error("The measurement frequency is not specified in the 'meas-freq' option.");
		result = 0;
	}
	int freq = options.getInt("meas-freq");

	// Create a new induction loop file
	ofstream loopFile(loopFileName, std::ios_base::trunc);

	if (loopFile.is_open()) {
		// Write header
		loopFile << "<additional xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\" xsi:noNamespaceSchemaLocation=\"http://sumo.dlr.de/xsd/additional_file.xsd\">" << std::endl;

		// Write induction loops
		for (const GARDetector const* pDet : pDetCont->getDetectors()) {
			string loopId = "loop_" + pDet->getLaneID();
			loopFile << "\t<inductionLoop id=\"" << loopId
					 << "\" lane=\"" << pDet->getLaneID()
					 << "\" pos=\"" << pDet->getPos()
					 << "\" freq=\"" << freq
					 << "\" file=\"" << GARDynObjective::GAR_LOOP_MEAS_FILE
					 << "\"/>" << std::endl;

			// Add a loop induction to detector data map element
			logger->debug("Add a map element for induction loop [" + loopId + "] connecting to detector data [" + pDet->getID() + "]");
			pDynObjData->addLoop2Detector(loopId, pDet);
		}

		// Write footer
		loopFile << "</additional>" << std::endl;
	} else {
		logger->error("Fail to open induction loop file [" + loopFileName + "]");
		result = 0;
	}

	// Close the induction loop file
	if (loopFile) {
		loopFile.close();
	}

	return result;
}


//................................................. Get the trips from an origin TAZ to a destination TAZ ...
gar::trip_vector GARouter::getTripsFromTazToTaz(const std::string& fromTaz, const std::string& toTaz) const {
	trip_vector tripVec;

	// Get the origin and destination edges
	edge_set fromEdges = pTazCont->getSourceEdges(fromTaz);
	edge_set toEdges = pTazCont->getSinkEdges(toTaz);

	// Get the trips from every origin to every destination edges
	for (auto fromEdge : fromEdges) {
		for (auto toEdge : toEdges) {
			trip_set::iterator it = pTripCont->getTrip(fromEdge, toEdge);
			if (it == pTripCont->getOdTrips().end()) {
				continue;
			}
			logger->debug("Trip from edge [" + (*it)->getpOrig()->getID()
						+ "] to edge [" + (*it)->getpDest()->getID() + "]");
			for (auto rd : (*it)->getOdRoutes()) {
				logger->debug("\tRoute: [" + rd->routename + "]");
			}
			tripVec.push_back(*it);
		}
	}

	return tripVec;
}


//................................................. Get the value of the 'select-scores' option ...
int GARouter::getGAStatisticsSelectScores(const std::string selectScores) const {
	int stats = GAStatistics::NoScores;

	for (string strScore : StringUtilities::split(selectScores, '|')) {
		int scoreSelection = -1;

		if (StringUtilities::trim(strScore) == "GAStatistics::NoScores") {
			return GAStatistics::NoScores;
		}
		if (StringUtilities::trim(strScore) == "GAStatistics::AllScores") {
			return GAStatistics::AllScores;
		}

		// Bitwise-OR the score selections
		if (StringUtilities::trim(strScore) == "GAStatistics::Mean") {
			scoreSelection = GAStatistics::Mean;
		}
		if (StringUtilities::trim(strScore) == "GAStatistics::Maximum") {
			scoreSelection = GAStatistics::Maximum;
		}
		if (StringUtilities::trim(strScore) == "GAStatistics::Minimum") {
			scoreSelection = GAStatistics::Minimum;
		}
		if (StringUtilities::trim(strScore) == "GAStatistics::Deviation") {
			scoreSelection = GAStatistics::Deviation;
		}
		if (StringUtilities::trim(strScore) == "GAStatistics::Diversity") {
			scoreSelection = GAStatistics::Diversity;
		}
		if (scoreSelection >= 0) {
			stats |= scoreSelection;
		}
	}

	return stats;
}


//................................................. Builds the static allele set array of the genome ...
GAAlleleSetArray<const char*> GARouter::buildGAStaticAlleleSetArray(void) {
	GAAlleleSetArray<const char*> alleleArray;

	try {
		matrix<unsigned int> odMatrix = pOdMatrix->getOdMatrix();
		vector<string> tazs = pOdMatrix->getTazs();

		for (int i = 0; i < odMatrix.size1(); i++) {
			for (int j = 0; j < odMatrix.size2(); j++) {
				string fromTaz = tazs[i];
				string toTaz = tazs[j];
				int numTrips = odMatrix(i,j) * pOdMatrix->getFactor();
				if (numTrips == 0) {
					continue;
				}

				// Get the trips from the origin to the destination TAZ
				logger->debug("Get the trips from TAZ [" + fromTaz + "] to TAZ [" + toTaz + "]");
				trip_vector trips = getTripsFromTazToTaz(fromTaz, toTaz);

				// Build the allele set
				logger->debug("Build allele set containing trips from [" + fromTaz + "] to [" + toTaz + "]");
				GAAlleleSet<const char*> alleleSet = buildGAStaticAlleleSet(trips);

				// Add the allele set to the allele set array
				logger->debug("Add [" + StringUtilities::toString<int>(numTrips) + "] allele sets to the allele array");
				for (int k = 0; k < numTrips; k++) {
					alleleArray.add(alleleSet);
				}
			}
		}

		logger->debug("Allele array size: " + StringUtilities::toString<int>(alleleArray.size()));

	} catch (const std::exception& ex) {
		logger->error("Fail to build allele set array for the static approach: " + string(ex.what()));
		return GAAlleleSetArray<const char*>();
	}

	return alleleArray;
}


//................................................. Builds the static allele set describing a gene in the genome ...
GAAlleleSet<const char*> GARouter::buildGAStaticAlleleSet(const trip_vector& trips) {
	GAAlleleSet<const char*> alleleSet;

	try {
		for (std::shared_ptr<const GARTrip> pTrip : trips) {
			for (std::shared_ptr<const GARRouteDesc> pRou : pTrip->getOdRoutes()) {
				// Add the allele route to the allele set
				alleleSet.add(pRou->routename.c_str());
			}
		}
		logger->debug("Alleles for the static approach:");
		for (int i = 0; i < alleleSet.size(); i++) {
			logger->debug("\tStatic Allele (" + StringUtilities::toString<int>(i) + "): [" + string(alleleSet.allele(i)) + "]");
		}
	} catch (const std::exception& ex) {
		logger->error("Fail to build the allele set for the static approach: " + string(ex.what()));
		return GAAlleleSet<const char*>();
	}

	return alleleSet;
}


//................................................. Builds the allele set array of the genome for the dynamic approach...
GAAlleleSetArray<int> GARouter::buildGADynAlleleSetArray(void) {
	GAAlleleSetArray<int> alleleArray;

	SUMOTime begin;
	SUMOTime timeStep;

	if (options.isSet("begin")) {
		logger->info("Set begin time [" + options.getString("begin") + "] to objective data");
		begin = StringUtilities::toInt(options.getString("begin"));
	} else {
		logger->error("Begin time is not specified in the 'begin' option.");
		GAAlleleSetArray<int>();
	}

	if (options.isSet("time-step")) {
		logger->info("Set time step [" + options.getString("time-step") + "] to objective data");
		timeStep = StringUtilities::toInt(options.getString("time-step"));
	} else {
		logger->error("Time step is not specified in the 'time-step' option.");
		GAAlleleSetArray<int>();
	}

	try {
		// Read the routes from the static solution.
		for (int i = 0; i < pStaticSolution->size(); i++) {
			string routeName = pStaticSolution->gene(i);

			// Get the lower bound of the beginning time from the detector flow data
			SUMOTime lower = getDynBeginLowerBound(routeName, begin, timeStep);
			SUMOTime upper = getDynBeginUpperBound(routeName, begin, timeStep);

			logger->info("lower begin time: [" + StringUtilities::toString<SUMOTime>(lower) + "]");
			logger->info("upper begin time: [" + StringUtilities::toString<SUMOTime>(upper) + "]");

			// Build the allele set
			logger->debug("Build allele set from lower time [" + StringUtilities::toString<SUMOTime>(lower) +
						  "] to upper time [" + StringUtilities::toString<SUMOTime>(upper) + "]");
			GAAlleleSet<int> alleleSet = buildGADynAlleleSet(lower, upper, timeStep);

			// Add the allele set to the allele set array
			alleleArray.add(alleleSet);
		}
	} catch (const std::exception& ex) {
		logger->error("Fail to build the allele set array for the dynamic approach: " + string(ex.what()));
		return GAAlleleSetArray<int>();
	}

	return alleleArray;
}


//................................................. Builds the allele set for the dynamic approach ...
GAAlleleSet<int> GARouter::buildGADynAlleleSet(const SUMOTime& lower,
											   const SUMOTime& upper,
											   const SUMOTime& timeStep) {
	GAAlleleSet<int> alleleSet;

	try {
		for (int i = lower; i <= upper; i+=timeStep) {
			alleleSet.add(i);
		}

		logger->debug("Alleles for the dynamic approach:");
		for (int i = 0; i < alleleSet.size(); i++) {
			logger->debug("\tDynamic Allele (" + StringUtilities::toString<int>(i) + "): [" + StringUtilities::toString<int>(alleleSet.allele(i)) + "]");
		}
	} catch (const std::exception& ex) {
		logger->error("Fail to build the allele set for the dynamic approach: " + string(ex.what()));
		return GAAlleleSet<int>();
	}

	return alleleSet;
}


//................................................. Get the lower bound of the beginning time ...
SUMOTime GARouter::getDynBeginLowerBound(const std::string& routeName,
										 const SUMOTime& begin,
										 const SUMOTime& timeStep) const {
	SUMOTime lowerTime = begin;

	// Get the origin edge of the route
	const GAREdge const* pOrigEdge = GARUtils::getOriginEdge(pRouCont.get(), routeName);

	// Get the detector in the origin Edge
	const GARDetector& origDet = pDetCont->getAnyDetectorForEdge(pOrigEdge);
	logger->debug("Detector at the origin edge: [" + origDet.getID() + "]");

	// Get the flow definitions for the detector at the origin
	const std::vector<FlowDef>& origFlowDefs = pFlows->getFlowDefs(origDet.getID());

	// Get the lower bound of the beginning time
	for (FlowDef fd : origFlowDefs) {
		logger->debug("qPKW: [" + StringUtilities::toString<SUMOReal>(fd.qPKW) + "], "
				     "vPKW: [" + StringUtilities::toString<SUMOReal>(fd.vPKW) + "], "
					 "qLKW: [" + StringUtilities::toString<SUMOReal>(fd.qLKW) + "], "
					 "vLKW: [" + StringUtilities::toString<SUMOReal>(fd.vLKW) + "], "
					 "isLKW: [" + StringUtilities::toString<SUMOReal>(fd.isLKW) + "], "
					 "fLKW: [" + StringUtilities::toString<SUMOReal>(fd.fLKW) + "], "
					 "firstSet: [" + StringUtilities::toString<bool>(fd.firstSet) + "]");
		if (fd.qPKW > 0 || fd.qLKW > 0) {
			return lowerTime;
		}

		lowerTime += timeStep;
	}

	return lowerTime;
}


//................................................. Get the upper bound of the beginning time ...
SUMOTime GARouter::getDynBeginUpperBound(const std::string& routeName,
		 	 	 	 	 	 	 	 	 const SUMOTime& begin,
										 const SUMOTime& timeStep) const {
	SUMOTime fdTime = begin;
	SUMOTime upperTime = begin;

	// Get the origin edge of the route
	const GAREdge const* pOrigEdge = GARUtils::getOriginEdge(pRouCont.get(), routeName);

	// Get the detector in the origin Edge
	const GARDetector& origDet = pDetCont->getAnyDetectorForEdge(pOrigEdge);
	logger->debug("Detector at the origin edge: [" + origDet.getID() + "]");

	// Get the flow definitions for the detector at the origin
	const std::vector<FlowDef>& origFlowDefs = pFlows->getFlowDefs(origDet.getID());

	// Get the upper bound of the beginning time
	for (FlowDef fd : origFlowDefs) {
		logger->debug("qPKW: [" + StringUtilities::toString<SUMOReal>(fd.qPKW) + "], "
				     "vPKW: [" + StringUtilities::toString<SUMOReal>(fd.vPKW) + "], "
					 "qLKW: [" + StringUtilities::toString<SUMOReal>(fd.qLKW) + "], "
					 "vLKW: [" + StringUtilities::toString<SUMOReal>(fd.vLKW) + "], "
					 "isLKW: [" + StringUtilities::toString<SUMOReal>(fd.isLKW) + "], "
					 "fLKW: [" + StringUtilities::toString<SUMOReal>(fd.fLKW) + "], "
					 "firstSet: [" + StringUtilities::toString<bool>(fd.firstSet) + "]");
		if (fd.qPKW > 0 || fd.qLKW > 0) {
			upperTime = fdTime;
		}

		fdTime += timeStep;
	}

	return upperTime + timeStep;
}


//................................................. Get the upper bound of the beginning time ...
string GARouter::buildScoreFileName(const string& prefix) {
	if (!options.isSet("score-file")) {
		logger->fatal("Option 'score-file' is not specified.");
		return "";
	}
	string scoreFile = options.getString("score-file");

	boost::filesystem::path scorePath(scoreFile);

	try {
		// Split the score-file into parent path and file name
		string scoreFilename = scorePath.filename().string();
		string scoreParentPath = scorePath.parent_path().string();

		if (!is_directory(boost::filesystem::path(scoreParentPath))) {
			logger->error("Score file [" + scoreFile + "] can't be created in the directory [" + scoreParentPath + "]");
			return "";
		}

		// Build the score file name by adding the prefix
		scoreFile = scoreParentPath
				+ boost::filesystem::path::preferred_separator
				+ prefix
				+ scoreFilename;

	} catch(const boost::filesystem::filesystem_error &ex) {
		logger->error("Fail to build score file name from score-file [" + scoreFile + "]");
		return "";
	}

	return scoreFile;
}


} /* namespace gar */


