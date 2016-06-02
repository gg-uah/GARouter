/*
 * GARDynObjective.hpp
 *
 *  Created on: Jun 10, 2015
 *      Author: ctomas
 */

#ifndef GARDYNOBJECTIVE_HPP_
#define GARDYNOBJECTIVE_HPP_

#include <GARDetector.hpp>
#include <GARDetectorCon.hpp>
#include <GARDetectorFlow.hpp>
#include <GARRouteCont.hpp>
#include <utils/common/StdDefs.h>
#include <ga/ga.h>

namespace gar {

/**
 * This enumerator is used to identify every line of the od-matrix file.
 */
enum class GARGeneData : unsigned short { RouteName, DepartTime };


typedef GA1DArrayAlleleGenome<int> genome_dyn;

class GARDynObjective {
public:
	/**
	 * Deleted constructor.
	 */
	GARDynObjective() = delete;

	/**
	 * Deleted virtual destructor.
	 */
	virtual ~GARDynObjective() = delete;

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
	 * @param timeStep		 The time step of the flow measures.
	 * @param pDetCont		 The detectors container.
	 * @param pFlows		 The detector flow measures.
	 * @param pRouCont		 A pointer to the route data container.
	 * @param routes		 The routes resulting from the static approach solution.
	 * @param genome		 An individual genome in the genetic algorithm population.
	 * 						 This genome comprises the routes to be performed by the SUMO simulation.
	 * @return				 <code>0</code> if the emitter file is successfully written,
	 * 						 <code>1</code> otherwise.
	 */
	static int writeEmitterFile(const std::string& emittFilename,
						        const SUMOTime& begin,
					 	        const SUMOTime& end,
								const SUMOTime& timeStep,
								const GARDetectorCon const* pDetCont,
								const GARDetectorFlows const* pFlows,
								const GARRouteCont const* pRouCont,
								const std::vector<std::string>& routes,
						        const genome_dyn& genome);

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
	 * Compute the route depart speed for an emitter.
	 * @param startTime		The start time in seconds.
	 * @param endTime		The end time in seconds.
	 * @param timeStep		The time step of the detector flow measures.
	 * @param pDetCont		The detectors container.
	 * @param pFlows		The detector flow measures.
	 * @param pRouCont		The routes container.
	 * @param routeName		The route name.
	 * @return
	 */
	static float computeDepartSpeed(const SUMOTime& startTime,
									const SUMOTime& endTime,
									const SUMOTime& timeStep,
									const GARDetectorCon const* pDetCont,
									const GARDetectorFlows const* pFlows,
									const GARRouteCont const * pRouCont,
									const std::string& routeName);

	//! The loop measurement file generated by the SUMO simulation in the objective function
	static const std::string GAR_LOOP_MEAS_FILE;

	//! The separator between the route and the depart time in the genome allele
	static const std::string GAR_ALLELE_DEPART_SEPARATOR;

};

} /* namespace gar */

#endif /* GARDYNOBJECTIVE_HPP_ */
