/*
 * GARObjective.hpp
 *
 *  Created on: Jun 10, 2015
 *      Author: ctomas
 */

#ifndef GAROBJECTIVE_HPP_
#define GAROBJECTIVE_HPP_

#include <GARDetector.hpp>
#include <GARDetectorFlow.hpp>
#include <GARRouteCont.hpp>
#include <utils/common/StdDefs.h>
#include <ga/ga.h>

namespace gar {

/**
 * This enumerator is used to identify every line of the od-matrix file.
 */
enum class GARGeneData : unsigned short { RouteName, DepartTime };


typedef GA1DArrayAlleleGenome<const char*> genome_t;
typedef std::unique_ptr<GA1DArrayAlleleGenome<const char*>> genome_uniq_ptr;

class GARObjective {
public:
	/**
	 * Deleted constructor.
	 */
	GARObjective() = delete;

	/**
	 * Deleted virtual destructor.
	 */
	virtual ~GARObjective() = delete;

	/**
	 * The objective function of the genetic algorithm.
	 * @param __genome	The genome of the genetic algorithm.
	 * @return			The score assigned to the genome.
	 */
	static float objective(GAGenome& __genome);

	/**
	 * Write the emitter file containing the vehicle trips to be performed
	 * by the SUMO simulation.
	 * @param emittFilename  The emitter file name.
	 * @param begin			 The beginning processing file.
	 * @param end			 The end processing file.
	 * @param pRouCont		 A pointer to the route data container.
	 * @param genome		 An individual genome in the genetic algorithm population.
	 * 						 This genome comprises the routes to be performed by the SUMO simulation.
	 * @return				 <code>0</code> if the emitter file is successfully written,
	 * 						 <code>1</code> otherwise.
	 */
	static int writeEmitterFile(const std::string& emittFilename,
						        const SUMOTime& begin,
					 	        const SUMOTime& end,
								const GARRouteCont const* pRouCont,
						        const genome_t& genome);

	/**
	 * Run the SUMO simulation.
	 * @param netFile	The network file.
	 * @param loopFile	The induction loop file.
	 * @param rouFile	The route file.
	 * @param emittFile	The emitter file.
	 * @param begin		The simulation beginning time in seconds.
	 * @param teleport	The time to teleport in seconds.
	 * @return			<code>-1</code> if the SUMO simulation if the SUMO simulation can't be executed,
	 * 					<code>status</code> the return status of the SUMO simulation executed by popen().
	 */
	static int runSUMO(const std::string& netFile,
					   const std::string& loopFile,
					   const std::string& rouFile,
					   const std::string& emittFile,
					   const SUMOTime& begin,
					   const SUMOTime& teleport);

	/**
	 * Read the end time in seconds of the SUMO simulation from the pipe stream to the executed 'sumo' command.
	 * @param fp	A pointer to the stream used to read from the pipe between the calling program and
	 * 				the 'sumo' command.
	 * @return		The end time in seconds of the SUMO simulation or -1 if an error raises during the
	 * 				SUMO simulation.
	 */
	static SUMOTime getSimulationEndTime(FILE* fp);


	/**
	 * Read the induction loop measurements and build the detector flow data.
	 * @param loops2Dets	A map that connects induction loop identifiers with detector data.
	 * @param beginTime	 	The simulation beginning time in seconds.
	 * @param endTime		The simulation end time in seconds.
	 * @param timeStep		The interval of time in seconds between two successive detector data.
	 * @return
	 */
	static std::shared_ptr<GARDetectorFlows> readLoopMeasures(const gar::loop2det_map& loops2Dets,
														      const SUMOTime& beginTime,
													          const SUMOTime& endTime,
														      const SUMOTime& timeStep);

	/**
	 * Compute the Root Mean Squared Error (RMSE) of the detector flow data.
	 * @param detectors		 A vector containing the detector data in the network.
	 * @param pGoalFlowData  The detector flow data to be aimed for.
	 * @param pSimFlowData	 The detector flow data as a result of the SUMO simulation.
	 * @return				 The RMSE of the detector flow data of the SUMO simulation.
	 */
	static float computeScore(const std::vector<GARDetector*>& detectors,
							  const GARDetectorFlows const* pGoalFlowData,
							  const GARDetectorFlows const* pSimFlowData);

	/**
	 * Get the route name from a gene in the genome.
	 * @param gene	The gene data in the genome.
	 * @return		The emitter route name.
	 */
	static std::string getRouteNameFromGene(const std::string& gene);

	/**
	 * Get the depart time from a gene in the genome.
	 * @param gene	The gene data in the genome.
	 * @return		The emitter depart time.
	 */
	static std::string getDepartTimeFromGene(const std::string& gene);

	/**
	 * Get the specified data from a gene in the genome.
	 * @param gene		The gene data in the genome.
	 * @param geneData	The type of data to be extracted from the gene.
	 * @return			The specified data extracted from the gene.
	 */
	static std::string getDataFromGene(const std::string& gene, const gar::GARGeneData& geneData);

	/**
	 * Get the accumulative flow data per edge from the route data in the genome.
	 * @param genome	An individual genome in the genetic algorithm population.
	 * 					This genome comprises the routes to be performed by the SUMO simulation.
	 * @return
	 */
	static std::map<std::string,std::shared_ptr<int>> getEdgeFlowCounts(const GARRouteCont const* pRouCont,
													  	  	  	  	  	const genome_t& genome);

	/**
	 * Compute the score by matching the flow amounts on every edge against the detector flows measurements.
	 * @param edgeFlows	The flow amounts per edge.
	 * @param pDetCont	The detectors container.
	 * @param pDetFlows	The detector flows measurements.
	 * @return			The score as result of matching the flow amounts on every edge against the detector
	 * 					flows measurements.
	 */
	static float computeFlowAmountScore(const GARNet const* pNet,
										const GARDetectorCon const* pDetCont,
										const GARDetectorFlows const* pDetFlows,
										const std::map<std::string,std::shared_ptr<int>>& edgeFlows);

	/**
	 * Compute the flow amount sum of the detectors in the specified edge.
	 * @param edgeId	The edge identifier.
	 * @param pDetCont	The detectors container.
	 * @param pDetFlows	The detector flows.
	 * @return			The flow amount sum of the detectors in the specified edge.
	 */
	static int computeDetFlowSumPerEdge(const std::string& edgeId,
					   	   	      	    const GARDetectorCon const* pDetCont,
										const GARDetectorFlows const* pDetFlows);


	//! The loop measurement file generated by the SUMO simulation in the objective function
	static const std::string GAR_LOOP_MEAS_FILE;

	//! The detector flow measurement file generated by the SUMO simulation in the objective function
	static const std::string GAR_DET_FLOW_FILE;

	//! The separator between the route and the depart time in the genome allele
	static const std::string GAR_ALLELE_DEPART_SEPARATOR;

};

} /* namespace gar */

#endif /* GAROBJECTIVE_HPP_ */
