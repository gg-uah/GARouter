/*
 * GARouter.hpp
 *
 *  Created on: Apr 21, 2015
 *      Author: ctomas
 */

#ifndef GAROUTER_HPP_
#define GAROUTER_HPP_

#include <CLogger.hpp>
#include <GARNet.hpp>
#include <GARDetector.hpp>
#include <GARTazCont.hpp>
#include <GARTripCont.hpp>
#include <GARStaticObjData.hpp>
#include <GARDynObjData.hpp>
#include <GAROdMatrix.hpp>
#include <Graph.hpp>
#include <utils/options/OptionsCont.h>
#include <ga/ga.h>


using common::CLogger;

namespace gar {

typedef GA1DArrayAlleleGenome<const char*> genome_static;
typedef GA1DArrayAlleleGenome<int> genome_dyn;

/**
 * @class GARouter
 * This class implements the disaggregation traffic flow router making use of
 * the genetic algorithms advantages on generating useful solutions to optimization
 * and search problems in complex scenarios.
 * The GARouter reads data from a network file and computes all the feasible routes
 * that can be carried out by the vehicles.
 */
class GARouter {
public:
	/**
	 * Invalidated default constructor.
	 * Prevents to build an instance of this class with no parameters.
	 * The configuration options container is required.
	 */
	GARouter(void) = delete;

	/**
	 * Invalidated copy constructor.
	 * Prevents to create a new GARouter instance from other GARouter instance.
	 * @param[in] other	The other GARouter to copy.
	 */
	GARouter(const GARouter& other) = delete;

	/**
	 * Parameterized constructor
	 * @param[in] options	The option container that stores the ga-router options.
	 * 						These configuration options can be set either through the command line or by means of a XML file.
	 * @param[in] pNet		Reference to the network data.
	 * @param[in] pOdMatrix Reference to the od-matrix data.
	 * @param[in] pTazCont  Reference to the district (TAZ) data container.
	 * @param[in] pDetCont  Reference to the detector data container.
	 * @param[in] pFlows	Reference to the detector flow data.
	 * @param[in] logger	A reference to the application logger.
	 */
	GARouter(const OptionsCont& options,
			 const std::shared_ptr<GARNet>& pNet,
			 const std::shared_ptr<GAROdMatrix>& pOdMatrix,
			 const std::shared_ptr<GARTazCont>& pTazCont,
			 const std::shared_ptr<GARDetectorCon>& pDetCont,
			 const std::shared_ptr<GARDetectorFlows>& pFlows,
			 CLogger* logger);

	/**
	 * Default virtual destructor
	 */
	virtual ~GARouter(void) = default;

	/**
	 * Initializes the ga-router application system (messaging, XML).
	 * @return 	<code>0</code> in case of successful initialization,
	 * 			<code>1</code> otherwise.
	 */
	int init(void);

	/**
	 * Closes the ga-router application system (the XML subsystem).
	 * @return 	<code>0</code> if the application system is successfully closed,
	 * 			<code>1</code> otherwise.
	 */
	int close(void);


	//............................................................. Routing methods ...
	/**
	 * Computes the routes in the network.
	 * @return	<code>0</code> if the routes are successfully computed,
	 * 			<code>1</code> otherwise.
	 */
	int computeRoutes(void);

	/**
	 * Computes the K-shortest paths from source to sink detectors.
	 * @return	The K-shortest paths from source and sink detectors.
	 */
	int computeKShortestRoutes(void);

	/**
	 * Computes routes between the detectors (optionally)
	 * @return	<code>0</code> if the routes between detectors are successfully computed,
	 * 			<code>1</code> otherwise.
	 */
	int computeRoutesBetweenDetectors(void);

	/**
	 * @brief Dump the routes into a data file.
	 * Saves the computed routes into a data file specified int the "routes-output" option.
	 * @return	<code>0</code> if the routes are successfully saved,
	 * 			<code>1</code> otherwise.
	 */
	int dumpRoutes(void);


	//............................................................. Static approach methods ...
	/**
	 * Build the data required in the objective function for the static approach.
	 * @return	<code>0</code> if the objective data for the static approach is successfully built,
	 * 			<code>1</code> otherwise.
	 * @see GARStaticObjData
	 */
	int buildGAStaticObjectiveData(void);

	/**
	 * Build the genome encoding the static approach.
	 * The genome is an allele set array where each gene depicts a route from any origin
	 * to any destination district.
	 * @return	<code>0</code> if the genome is successfully built,
	 * 			<code>1</code> otherwise.
	 */
	int buildGAStaticGenome(void);

	/**
	 * @brief Build the genetic algorithm engine for the static approach.
	 * The genetic algorithm can be:
	 * - Simple (non-overlapping populations)
	 * - SteadyState (overlapping populations)
	 * - Incremental (overlapping with custom replacement)
	 * - Deme (parallel populations with migration)
	 * @return	<code>0</code> if the genetic algorithm engine is successfully built,
	 * 			<code>1</code> otherwise.
	 */
	int buildGAStaticGeneticAlgorithm(void);

	/**
	 * @brief Evolve the genetic algorithm for the static approach.
	 * It computes the disaggregated routes from the detector traffic flow amounts.
	 * @return	<code>0</code> if the genetic algorithm for the static approach is successfully evolved,
	 * 			<code>1</code> otherwise.
	 */
	int evolveStaticGA(void);

	/**
	 * @brief Register the static solution.
	 * Copy the best genome solution from the GA evolution in the \link GARouter::pStaticSolution \endlink attribute.
	 * @param best	The best solution of the GA for the static approach.
	 */
	void setStaticSolution(const genome_static& best);

	/**
	 * @brief Get the GA solution for the static approach.
	 * Returns a string vector enclosing the route names defining the traffic flow.
	 * @return A vector containing the route names of the solution for the static approach.
	 */
	gar::genome_static getStaticSolution(void) const;

	/**
	 * @brief Print the static solution.
	 * Print the best genome and the statistics of the genetic evolution
	 * for the static approach.
	 */
	void printStaticSolution(void) const;

	/**
	 * @brief Check whether to trigger the dynamic approach performance.
	 * If the score of the static solution is greater or equal than
	 * the 'trigger-dynamic' option, the dynamic GA is performed.
	 * @return	<code>true</code> if the score of the static solution is greater or
	 * 			equal than the 'trigger-dynamic' option, <code>false</code> otherwise
	 */
	bool triggerDynamicGA(void) const;


	//............................................................. Dynamic approach methods ...
	/**
	 * Build the data required in the objective function for the dynamic approach.
	 * @return	<code>0</code> if the objective data for the dynamic approach is successfully built,
	 * 			<code>1</code> otherwise.
	 * @see GARDynObjData
	 */
	int buildGADynObjectiveData(void);

	/**
	 * Build the genome encoding the dynamic approach.
	 * The genome is an allele set array where each gene depicts a route from any origin
	 * to any destination district.
	 * @return	<code>0</code> if the genome is successfully built,
	 * 			<code>1</code> otherwise.
	 */
	int buildGADynGenome(void);

	/**
	 * @brief Build the genetic algorithm engine for the dynamic approach.
	 * The genetic algorithm can be:
	 * - Simple (non-overlapping populations)
	 * - SteadyState (overlapping populations)
	 * - Incremental (overlapping with custom replacement)
	 * - Deme (parallel populations with migration)
	 * @return	<code>0</code> if the genetic algorithm engine is successfully built,
	 * 			<code>1</code> otherwise.
	 */
	int buildGADynGeneticAlgorithm(void);

	/**
	 * @brief Evolve the genetic algorithm for the dynamic approach.
	 * It computes the depart time for each vehicle route from the static solution.
	 * @return	<code>0</code> if the genetic algorithm for the dynamic approach is successfully evolved,
	 * 			<code>1</code> otherwise.
	 */
	int evolveDynGA(void);


	//............................................................. Set genome operators ...
	/**
	 * @brief Set the initialization operator.
	 * This method is invoked when the population is initialized.
	 * Possible values: [Uniform | Ordered]
	 * @param pGenome	The genome that the initializer method is set on.
	 */
	void setGAGenomeInitializer(GAGenome * pGenome);

	/**
	 * @brief Set the mutation operator.
	 * This method is invoked on each newly generated offspring.
	 * Possible values: [Flip | Swap]
	 * @param pGenome	The genome that the mutation operation is set on.
	 */
	void setGAGenomeMutator(GAGenome * pGenome);

	/**
	 * @brief Set the crossover operator.
	 * This method is used by the genetic algorithm to mate individuals from the population
	 * for form new offspring.
	 * Possible values: [Cycle | EvenOdd | OnePoint | Order | PartialMatch | TwoPoint | Uniform]
	 * @param pGenome	The genome that the crossover operation is set on.
	 */
	void setGAGenomeCrossover(GAGenome * pGenome);

	/**
	 * @brief Set the scaling scheme.
	 * The scaling object is embedded in the population object. The base scaling object is not instantiable.
	 * This object keeps track of the fitness scores (not the objective scores) of each individual in the population.
	 * The genomes that it returns are the genomes in the population to which it is linked; it does not make its own copies.
	 * It is important to note the distinction between fitness and objective scores.
	 * The objective score is the value returned by your objective function; it is the raw performance evaluation of a genome.
	 * The fitness score, on the other hand, is a possibly-transformed rating used by the genetic algorithm to determine the fitness
	 * of individuals for mating. The fitness score is typically obtained by a linear scaling of the raw objective scores
	 * (but you can define any mapping you want, or no transformation at all). For example, if you use linear scaling then
	 * the fitness scores are derived from the objective scores using the fitness proportional scaling technique described
	 * in Goldberg's book. The genetic algorithm uses the fitness scores, not the objective scores, to do selection.
	 * @param pGA	A pointer to either the static or dynamic genetic algorithm.
	 * @return		<code>0</code> if the scaling scheme is successfully set,
	 * 				<code>1</code> otherwise.
	 */
	int setGAScalingScheme(GAGeneticAlgorithm* pGA);

	/**
	 * @brief Set the selection scheme that is used to pick genomes from a population for mating.
	 * A selector may operate on the scaled objective scores or the raw objective scores.
	 * Default behavior is to operate on the scaled (fitness) scores.
	 * Possible values: [Rank | Roulette | Tournament | Uniform | SRS | DS]
	 * @param pGA	A pointer to either the static or dynamic genetic algorithm.
	 * @return		<code>0</code> if the selection scheme is successfully set,
	 * 				<code>1</code> otherwise.
	 */
	int setGASelectionScheme(GAGeneticAlgorithm* pGA);

	/**
	 * @brief Set the terminator function which is used to determine whether or not a genetic algorithm is finished.
	 * The done member function simply calls the completion function to determine whether or not the genetic algorithm should continue.
	 * The predefined completion functions use generation and convergence to determine whether or not the genetic algorithm is finished.
	 * The completion function returns gaTrue when the genetic algorithm should finish, and gaFalse when the genetic algorithm should continue.
	 * In this context, convergence refers to the the similarity of the objective scores, not similarity of underlying genetic structure.
	 * The built-in convergence measures use the best-of-generation scores to determine whether or not the genetic algorithm has plateaued.
	 * Possible values : [UponGeneration | UponConvergence | UponPopConvergence]
	 * @param pGA	A pointer to either the static or dynamic genetic algorithm.
	 * @return		<code>0</code> if the termination function is successfully set,
	 * 				<code>1</code> otherwise.
	 */
	int setGATerminationFunction(GAGeneticAlgorithm* pGA);

	/**
	 * @brief Set the replacement scheme only for the incremental GA.
	 * The replacement scheme is used by the incremental genetic algorithm to determine
	 * how a new individual should be inserted into a population.
	 * Possible values: [Random | Best | Custom]
	 * @return	<code>0</code> if the replacement scheme is successfully set,
	 * 			<code>1</code> otherwise.
	 */
	int setGAReplacementScheme(GAGeneticAlgorithm* pGA);

	/**
	 * @brief Set the parameters that will be used for the genetic algorithm.
	 * The parameter list object contains information about how genetic algorithms should behave.
	 * @return	<code>0</code> if the parameters are successfully set,
	 * 			<code>1</code> otherwise.
	 */
	int setGAParameters(void);

	/**
	 * Logs the routes.
	 */
	void logRoutes(void) const;

	/**
	 * Logs the route description.
	 * @param[in] rd	The route description.
	 */
	void logRouteDesc(const GARRouteDesc& rd) const;

	/**
	 * Logs the trips.
	 */
	void logTrips(void) const;

protected:
	/**
	 * Build the network graph used by the k-shortest-path algorithm.
	 * @return	The network graph used by the k-shortest-path algorithm.
	 */
	Graph buildKShortestGraph(void) const;

	/**
	 * Get the k-shortest routes from a source to a sink detector using the Yen's algorithm.
	 * @param graph			The network graph.
	 * @param pSourceDet	The pointer to a source detector data.
	 * @param pSinkDet		The pointer to a sink detector data.
	 * @param k				The number of k-shortest paths to compute.
	 * @param kSelected 	The number of selected shortest routes.
	 * @return				A vector that contains the top k-shortest route descriptions
	 * 						from a source to a sink detector.
	 */
	std::vector<GARRouteDesc> getKShortestRoutes(const Graph& graph,
			  	  	  	  	  	  	  	  	     const GARDetector const* pSourceDet,
											     const GARDetector const* pSinkDet,
												 int k,
												 int kSelected) const;

	/**
	 * Get the selected k-shortest routes randomly.
	 * @param kshortestRoutes	The k-shortest routes from a source to a sink detector.
	 * @param kSelected			The number of randomly selected k-shortest routes.
	 * @return
	 */
	std::vector<GARRouteDesc> getSelectedRoutes(const std::vector<GARRouteDesc>& kShortestRoutes,
											    int kSelected) const;

	/**
	 * Add the shortest routes to the route and the trip container and the detector data.
	 * @param shortestPaths	The object containing the k-shortest routes from the source to the
	 * 						sink detector.
	 * @param pSourceDet	The source detector at the origin edge of the routes.
	 * @param pSinkDet		The sink detector at the destination edge of the routes.
   	 * @return				<code>0</code> if the routes are successfully added,
	 * 						<code>1</code> otherwise.
	 */
	int addKShortestRoutes(const std::vector<GARRouteDesc>& kShortestRoutes,
						   const GARDetector const* pSourceDet,
						   const GARDetector const* pSinkDet);

	/**
	 * Build the description of a shortest route.
	 * @param path			The shortest path data.
	 * @param sourceEdgeId	The source edge identifier.
	 * @param sinkEdgeId	The sink edge identifier.
	 * @param routeNum		The shortest route number from the source to the sink edge.
	 * @return				An object containing the route data.
	 */
	GARRouteDesc buildKShortestRouteDesc(const BasePath const* path,
	  	  	  	  	   	   	   	   	   	 const std::string& sourceEdgeId,
										 const std::string& sinkEdgeId,
										 int routeNum) const;

	/**
	 * Get the number of detectors with the specified type (source/sink) having flow definitions.
	 * @param type	The detector type (source / sink)
	 * @return		The number of detectors with the specified type having flow definitions.
	 * @see GARDetectorType
	 */
	unsigned int getNumFlowDetectorsByType(const GARDetectorType& type) const;

	/**
	 * Get the accumulative flow amounts collected from a group of detectors.
	 * @param detectors	The detectors from which the flow amounts are collected.
	 * @return			The accumulative flow amounts from the group of detectors.
	 */
	unsigned int getFlowAmounts(std::vector<GARDetector*> detectors) const;

	/**
	 * @brief Write the induction loop file.
	 * For every detector in the network, define an induction loop to measure the
	 * traffic flow.
	 * @param loopFileName	The induction loop file name.
	 * @return	<code>0</code> if the induction loop file is successfully written.
	 * 			<code>1</code> otherwise.
	 */
	int writeInductionLoopFile(const std::string& loopFilename);

	/**
	 * @brief Get the vehicle trips between the given districts (TAZs).
	 * Returns a trip data vector containing the vehicle trips leaving from the specified origin district
	 * and arriving to the specified destination district.
	 * @param fromTaz	The origin district of the vehicle trips.
	 * @param toTaz		The destination district of the vehicle trips.
	 * @return			A data vector containing the vehicle trips that leave an edge residing in the origin
	 * 					district and arrive to an edge residing in the destination district.
	 */
	gar::trip_vector getTripsFromTazToTaz(const std::string& fromTaz, const std::string& toTaz) const;

	/**
	 * Get the gaNselectScores parameter value from the option string.
	 * @param selectScores	The option string containing the definition of the gaNselectScores parameter.
	 * @return				The gaNselectScores value.
	 */
	int getGAStatisticsSelectScores(const std::string selectScores) const;

	/**
	 * @brief Build the genome allele set array for the static approach.
	 * @return	The allele set array that comprises the allele sets for every gene in the genome.
	 */
	GAAlleleSetArray<const char*> buildGAStaticAlleleSetArray(void);

	/**
	 * @brief Build the genome allele set for the static approach.
	 * @param trips	The trips from a origin to a destination district (TAZ).
	 * @return		An allele set that comprises all the selected (shortest and longest) routes
	 * 				from an edge in the origin district to an edge in the destination district.
	 */
	GAAlleleSet<const char*> buildGAStaticAlleleSet(const trip_vector& trips);

	/**
	 * @brief Build the genome allele set array for the dynamic approach.
	 * @return	The allele set array that comprises the allele sets for every gene in the genome.
	 */
	GAAlleleSetArray<int> buildGADynAlleleSetArray(void);

	/**
	 * @brief Build the genome allele set for the dynamic approach.
	 * @param lower		The lower bound of the begin time.
	 * @param upper		The upper bound of the upper time.
	 * @param timeStep	The time step of the simulation.
	 * @return			An allele set that comprises the begin times
	 * 					for the vehicles to take the route in the flow.
	 */
	GAAlleleSet<int> buildGADynAlleleSet(const SUMOTime& lower,
			   	   	   	   	   	   	   	 const SUMOTime& upper,
										 const SUMOTime& timeStep);

	/**
	 * @brief Get the lower bound time of the beginning time used in the dynamic approach.
	 * For a given route, the detector in the origin edge is retrieved.
	 * The lower bound of the beginning time is the first time in the detector flow data
	 * with a non-zero value in qPKW or qLKW.
	 * @param routeName	The name of the route for which the lower bound time of the
	 * 					beginning time lower bound is being calculated.
	 * @param begin		The begin time of the traffic flow.
	 * @param timeStep	The time interval the data is collected in.
	 * @return			The lower bound time of the beginning time in seconds.
	 */
	SUMOTime getDynBeginLowerBound(const std::string& routeName,
								   const SUMOTime& begin,
								   const SUMOTime& timeStep) const;

	/**
	 * @brief Get the upper bound time of the beginning time used in the dynamic approach.
	 * For a given route, the detector in the origin edge is retrieved.
	 * The upper bound of the beginning time is the last time in the detector flow data
	 * with a non-zero value in qPKW or qLKW.
	 * @param routeName	The name of the route for which the upper bound time of the
	 * 					beginning time lower bound is being calculated.
	 * @param begin		The begin time of the traffic flow.
	 * @param timeStep	The time interval the data is collected in.
	 * @return			The upper bound time of the beginning time in seconds.
	 */
	SUMOTime getDynBeginUpperBound(const std::string& routeName,
			   	   	   	   	   	   const SUMOTime& begin,
								   const SUMOTime& timeStep) const;

	/**
	 * @brief Builds the score file name.
	 * The score file name is made up from the value in the --score-file option.
	 * This value is split into the path name and the file name. Then the prefix
	 * is added at the beginning of the file name, and all together is joined back.
	 * So: score file name = path name + prefix + file name;
	 * @param prefix The prefix to be added at the beginning of the score file name.
	 * @return		 A score file name (path name + prefix + file name)
	 */
	std::string buildScoreFileName(const std::string& prefix);


private:
	//! The ga-router option container
	const OptionsCont& options;

	//! The application logger
	CLogger* logger;

	//! The network as used by the ga-router
	std::shared_ptr<GARNet> pNet;

	//! The O/D matrix data
	std::shared_ptr<const GAROdMatrix> pOdMatrix;

	//! The districts (TAZs) and O/D groups in the network
	std::shared_ptr<GARTazCont> pTazCont;

	//! The detector container ascribed to the network
	std::shared_ptr<GARDetectorCon> pDetCont;

	//! The detector flow measurements
	std::shared_ptr<GARDetectorFlows> pFlows;

	//! The computed route container
	std::unique_ptr<GARRouteCont> pRouCont;

	//! The trip container
	std::unique_ptr<GARTripCont> pTripCont;

	//! The genetic algorithm parameters
	GAParameterList params;

	//! The genetic algorithm for the static approach
	std::unique_ptr<GAGeneticAlgorithm> pGAStatic;

	//! The genome of the GA for the static approach
	std::unique_ptr<genome_static> pStaticGenome;

	//! The data required in the GA objective function for the static approach
	std::unique_ptr<GARStaticObjData> pStaticObjData;

	//! The solution of the static approach. A list of route names defining the traffic flow.
	std::unique_ptr<genome_static> pStaticSolution;

	//! The genetic algorithm for the dynamic approach
	std::unique_ptr<GAGeneticAlgorithm> pGADyn;

	//! The genome of the GA for the dynamic approach
	std::unique_ptr<genome_dyn> pDynGenome;

	//! The data required in the GA objective function for the dynamic approach
	std::unique_ptr<GARDynObjData> pDynObjData;

	//! The solution of the dynamic approach. A list of route names defining the traffic flow.
	std::unique_ptr<genome_dyn> pDynSolution;

	//! The allele data set (the allele value pattern is <route_name>__<depart_time>)
	std::vector<std::string> alleles;

	//! The score file name
	std::string scoreFilename;
};

} /* namespace gar */

#endif /* GAROUTER_HPP_ */
