/*
 * GARObjective.hpp
 *
 *  Created on: Jun 10, 2015
 *      Author: ctomas
 */

#ifndef GARSTATICOBJECTIVE_HPP_
#define GARSTATICOBJECTIVE_HPP_

#include <GARDetector.hpp>
#include <GARDetectorFlow.hpp>
#include <GARRouteCont.hpp>
#include <utils/common/StdDefs.h>
#include <ga/ga.h>

namespace gar {

typedef GA1DArrayAlleleGenome<const char*> genome_static;

class GARStaticObjective {
public:
	/**
	 * Deleted constructor.
	 */
	GARStaticObjective() = delete;

	/**
	 * Deleted virtual destructor.
	 */
	virtual ~GARStaticObjective() = delete;

	/**
	 * The GA objective function for the static solution.
	 * @param __genome	The genome of the genetic algorithm.
	 * @return			The score assigned to the genome.
	 */
	static float objective(GAGenome& __genome);

	/**
	 * Get the accumulative flow data per edge from the route data in the genome.
	 * @param genome	An individual genome in the genetic algorithm population.
	 * 					This genome comprises the routes to be performed by the SUMO simulation.
	 * @return
	 */
	static std::map<std::string,std::shared_ptr<int>> getEdgeFlowCounts(const GARRouteCont const* pRouCont,
													  	  	  	  	  	const genome_static& genome);

	/**
	 * Compute the score by matching the flow amounts on every edge against the detector flows measurements.
	 * @param edgeFlows	The flow amounts per edge.
	 * @param pDetCont	The detectors container.
	 * @param pDetFlows	The detector flows measurements.
	 * @return			The score as result of matching the flow amounts on every edge against the detector
	 * 					flows measurements.
	 */
	static float computeScore(const GARNet const* pNet,
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
};

} /* namespace gar */

#endif /* GARSTATICOBJECTIVE_HPP_ */
