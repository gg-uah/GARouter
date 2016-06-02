//============================================================================
// Name        : GARouter.cpp
// Author      : ctomas
// Version     :
// Copyright   : Your copyright notice
// Description : Hello World in C++, Ansi-style
//============================================================================

#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

#ifdef HAVE_VERSION_H
#include <version.h>
#endif

#include <ga/ga.h>

#include <iostream>
#include <CLogger.hpp>
#include <GARLogger.hpp>
#include <GAROptions.hpp>
#include <GARouter.hpp>
#include <GAROdMatrix.hpp>
#include <GARNetLoader.hpp>
#include <GAROdLoader.hpp>
#include <GARTazLoader.hpp>
#include <GARDetLoader.hpp>
#include <GARDetFlowLoader.hpp>
#include <GARDetector.hpp>
#include <GARTrip.hpp>
#include <GARUtils.hpp>
#include <utils/options/Option.h>
#include <utils/common/StdDefs.h>
#include <StringUtilities.hpp>

#include <random>

using std::string;
using std::shared_ptr;
using common::StringUtilities;
using gar::GARLogger;
using gar::GAROptions;
using gar::GARouter;
using gar::GARUtils;
using gar::trip_set;
using gar::GARNetLoader;
using gar::GAROdLoader;
using gar::GARTazLoader;
using gar::GARDetLoader;
using gar::GARDetFlowLoader;
using gar::GAROdMatrix;


/**
 * This is the global function which is the designated start of the garouter.
 * @param argc    Non-negative value representing the number of arguments passed to the garouter
 *                 from the environment in which the program is run.
 * @param argv     Pointer to the first element of an array of pointers to null-terminated multibyte
 *                 strings that represent the arguments passed to the program from the execution environment
 *                 (argv[0] through argv[argc-1]). The value of argv[argc] is guaranteed to be ​0​.
 *                 The first element argv[0] is the name used to invoke the program itself (garouter in this case)
 * @return
 */
int main(int argc, char * argv[]) {
    /*
     * Handle signals throughout the ga-router performance
     */
    signal(SIGINT,  [] (int signum) {
    		std::cerr << "Interrupt signal received [" << signum << "] : Interruption" << std::endl;
    		exit(signum); });
    signal(SIGILL,  [] (int signum) {
    		std::cerr << "Interrupt signal received [" << signum << "] : Illegal instruction" << std::endl;
    		exit(signum); });
    signal(SIGFPE,  [] (int signum) {
    		std::cerr << "Interrupt signal received [" << signum << "] : Floating point error" << std::endl;
    		exit(signum); });
    signal(SIGSEGV, [] (int signum) {
    		std::cerr << "Interrupt signal received [" << signum << "] : Segmentation violation" << std::endl;
    		exit(signum); });
    signal(SIGTERM, [] (int signum) {
    		std::cerr << "Interrupt signal received [" << signum << "] : Termination request" << std::endl;
    		exit(signum); });
    signal(SIGABRT, [] (int signum) {
    		std::cerr << "Interrupt signal received [" << signum << "] : Abnormal termination (abort)" << std::endl;
    		exit(signum); });
    #ifdef SIGBREAK
    signal(SIGBREAK,[] (int signum) {
    		std::cerr << "Interrupt signal received [" << signum << "] : Control-break" << std::endl;
    		exit(signum); });
    #endif
    #ifdef SIGSTOP
    signal(SIGSTOP, [] (int signum) {
    		std::cerr << "Interrupt signal received [" << signum << "] : Unblockable stop" << std::endl;
    		exit(signum); });
	#endif
    #ifdef SIGTSTP
    signal(SIGTSTP, [] (int signum) {
    		std::cerr << "Interrupt signal received [" << signum << "] : Keyboard stop" << std::endl;
    		exit(signum); });
    #endif

    int status = 0;

	// Get the application logger
    GARLogger garLogger;
    CLogger* logger = garLogger.getLogger();
    if (logger == nullptr) {
    	std::cerr << "Fail to get logger. Check for the right log and dump configuration files and the log identifier" << std::endl;
    	return EXIT_FAILURE;
    }

    logger->info("..... Starting ga-router: the traffic flow disaggregation router based on genetic algorithms .....");

    // Initialize the ga-router options
    GAROptions garOpt;
    status = garOpt.init(argc, argv, logger);
	if (status > 0) {
		return EXIT_FAILURE;
	}

    // Load the network data
	logger->info(">> Loading network");
	GARNetLoader netLoader (garOpt.getOptions(), logger);
	status = netLoader.run();
	if (status > 0) {
		return EXIT_FAILURE;
	}
	netLoader.logNet();
	std::shared_ptr<GARNet> pNet = netLoader.getpNet();

	//Load the OD matrix data
	logger->info(">> Loading the OD matrix");
	GAROdLoader odLoader(garOpt.getOptions(), logger);
	status = odLoader.run();
	if (status > 0) {
		return EXIT_FAILURE;
	}
	odLoader.logOdMatrix();
	std::shared_ptr<GAROdMatrix> pOdMatrix = odLoader.getpOdMatrix();

	// Load the districts (TAZs)
	logger->info(">> Loading TAZ data");
	GARTazLoader tazLoader (garOpt.getOptions(), *pNet, logger);
	status = tazLoader.run();
	if (status > 0) {
		return EXIT_FAILURE;
	}
	tazLoader.logTazData();
	std::shared_ptr<GARTazCont> pTazCont = tazLoader.getpTazCont();

	// Load the detectors
	logger->info(">> Loading detectors");
	GARDetLoader detLoader (garOpt.getOptions(), *pNet, logger);
	status = detLoader.run();
	if (status > 0) {
		return EXIT_FAILURE;
	}
	detLoader.logDetectors();
	std::shared_ptr<GARDetectorCon> pDetCont = detLoader.getpDetCont();

	// Load the detector flows
	logger->info(">> Load detector flows");
	GARDetFlowLoader flowLoader (garOpt.getOptions(), *pDetCont, logger);
	status = flowLoader.run();
	if (status > 0) {
		return EXIT_FAILURE;
	}
	flowLoader.logDetectorFlows();
	std::shared_ptr<GARDetectorFlows> pFlows = flowLoader.getpFlows();

	// Initialize the ga-router application system (messaging, XML, options)
	GARouter gaRouter(garOpt.getOptions(),
					  pNet,
					  pOdMatrix,
					  pTazCont,
					  pDetCont,
					  pFlows,
					  logger);
	status = gaRouter.init();
	if (status > 0) {
		return EXIT_FAILURE;
	}

	logger->info(">> Compute the routes");
	gaRouter.computeRoutes();
	if (status > 0) {
		return EXIT_FAILURE;
	}

	// Log the trip data
	gaRouter.logTrips();

	// Check whether the detectors are valid
	logger->info(">> Check the detector routes");
	status = detLoader.checkDetectors();
	if (status > 0) {
		return EXIT_FAILURE;
	}
	logger->info("Detectors successfully checked");

	// Save the detectors if wished
	logger->info(">> Save the detectors");
	status = detLoader.saveDetectors();
	if (status > 0) {
		return EXIT_FAILURE;
	}

	// Dump the routes into a data file
	logger->info(">> Dump the routes");
	status = gaRouter.dumpRoutes();
	if (status > 0) {
		return EXIT_FAILURE;
	}

	//Build the objective data for the static approach
	logger->info(">> Build the objective data for the static approach");
	status = gaRouter.buildGAStaticObjectiveData();
	if (status > 0) {
		return EXIT_FAILURE;
	}

	//Build the genome for the static approach
	logger->info(">> Build the genome for the static approach");
	status = gaRouter.buildGAStaticGenome();
	if (status > 0) {
		return EXIT_FAILURE;
	}

	// Build the genetic algorithm engine for the static approach
	logger->info(">> Build the genetic algorithm engine for the static approach");
	status = gaRouter.buildGAStaticGeneticAlgorithm();
	if (status > 0) {
		return EXIT_FAILURE;
	}

	// Set the genetic algorithm parameters from the input options
	logger->info(">> Set the genetic algorithm parameters");
	status = gaRouter.setGAParameters();
	if (status > 0) {
		return EXIT_FAILURE;
	}

	// Evolve the genetic algorithm of the static approach
	logger->info(">> Evolve the genetic algorithm of the static approach");
	status = gaRouter.evolveStaticGA();
	if (status > 0) {
		return EXIT_FAILURE;
	}

	// Print the solution for the static approach
	logger->info("Get the solution of the static approach");
	gaRouter.printStaticSolution();

	// Check whether to run the dynamic approach
	logger->info("Check the triggering of the dynamic approach");
	if (!gaRouter.triggerDynamicGA()) {
		logger->info("The dynamic approach can't be triggered");
		return EXIT_SUCCESS;
	}

	//Build the objective data for the dynamic approach
	logger->info(">> Build the objective data for the dynamic approach");
	status = gaRouter.buildGADynObjectiveData();
	if (status > 0) {
		return EXIT_FAILURE;
	}

	//Build the genome for the dynamic approach
	logger->info(">> Build the genome for the dynamic approach");
	status = gaRouter.buildGADynGenome();
	if (status > 0) {
		return EXIT_FAILURE;
	}

	// Build the genetic algorithm engine for the dynamic approach
	logger->info(">> Build the genetic algorithm engine for the dynamic approach");
	status = gaRouter.buildGADynGeneticAlgorithm();
	if (status > 0) {
		return EXIT_FAILURE;
	}

	// Set the genetic algorithm parameters from the input options
	logger->info(">> Set the genetic algorithm parameters");
	status = gaRouter.setGAParameters();
	if (status > 0) {
		return EXIT_FAILURE;
	}

	// Evolve the genetic algorithm of the dynamic approach
	logger->info(">> Evolve the genetic algorithm of the dynamic approach");
	status = gaRouter.evolveDynGA();
	if (status > 0) {
		return EXIT_FAILURE;
	}

	return EXIT_SUCCESS;

	// Check the output of the algorithm with the detector flow amounts
	logger->info(">> Check the evolution output");
	//status = gaRouter.checkOutput();
	//if (status > 0) {
	//	return EXIT_FAILURE;
	//}

	return EXIT_SUCCESS;
}

