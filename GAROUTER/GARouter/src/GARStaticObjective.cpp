/*
 * GARStaticObjective.cpp
 *
 *  Created on: Jun 10, 2015
 *      Author: ctomas
 */

#include <GARStaticObjective.hpp>
#include <GARStaticObjData.hpp>
#include <router/ROEdge.h>
#include <StringUtilities.hpp>
#include <boost/format.hpp>
#include <boost/regex.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/xml_parser.hpp>
#include <vector>
#include <iostream>
#include <algorithm>


using std::string;
using std::ofstream;
using std::vector;
using std::map;
using std::pair;
using std::shared_ptr;
using std::unique_ptr;
using common::StringUtilities;

namespace boost_pt = boost::property_tree;


namespace gar {

//................................................. The genome objective function ...
float GARStaticObjective::objective(GAGenome& __genome) {
	float score = 0.0;

	// Cast to the suitable genome
	genome_static& genome = (genome_static&)__genome;

	// Get the data required by the objective from the genome user data
	const GARStaticObjData const* pData = (GARStaticObjData*)genome.userData();

	// Compute the flow amounts per edge corresponding to the genome routes
	map<string,shared_ptr<int>> edgeFlows = getEdgeFlowCounts(pData->getpRouCont(),
															  genome);

	score = computeScore(pData->getpNet(),
						 pData->getpDetCont(),
						 pData->getpDetFlows(),
						 edgeFlows);

	return score;
}


//................................................. Get the flow amounts per edge from the routes in the genome ...
std::map<std::string, std::shared_ptr<int>> GARStaticObjective::getEdgeFlowCounts(const GARRouteCont const* pRouCont,
																		   	      const genome_static& genome) {
	std::map<std::string,std::shared_ptr<int>> edgeFlows;

	for (int i = 0; i < genome.size(); i++) {
		string routeName = genome.gene(i);

		// Get the route description corresponding to the route name in the genome
		std::shared_ptr<const GARRouteDesc> pRd = pRouCont->getRouteDesc(routeName);
		if (pRd == nullptr) {
			continue;
		}

		// Increment flow per edges counter
		for (const ROEdge const* pEdge : pRd->edges2Pass) {
			string edgeId = pEdge->getID();
			std::map<std::string,std::shared_ptr<int>>::iterator it = edgeFlows.find(edgeId);
			if (it == edgeFlows.end()) {
				auto ret = edgeFlows.insert(std::pair<string,shared_ptr<int>>(edgeId,std::make_shared<int>(0)));
				if (ret.second == false) {
					continue;
				}
				it = ret.first;
			}
			(*(it->second))++;
		}
	}

	/*
	std::cout << "Flows per edge: " << std::endl;
	for (map<string,shared_ptr<int>>::iterator it = edgeFlows.begin(); it != edgeFlows.end(); it++) {
		string edgeId = it->first;
		int accumFlow = *(it->second);
		std::cout << "edge [" << edgeId << "]: " << accumFlow << std::endl;
	}
	*/

	return edgeFlows;
}


//................................................. Compute the flow amount score for the static approach ...
float GARStaticObjective::computeScore(const GARNet const* pNet,
									   const GARDetectorCon const* pDetCont,
									   const GARDetectorFlows const* pDetFlows,
									   const std::map<std::string,std::shared_ptr<int>>& edgeFlows) {

	double sum = 0.0;
	double num = 0.0;

	for (auto edgePair : pNet->getEdgeMap()) {
		string edgeId = edgePair.first;
		const ROEdge const* pEdg = edgePair.second;
		if (pEdg->getType() == ROEdge::EdgeType::ET_INTERNAL) {
			continue;
		}

		// Get the edge occurrences from the genome routes
		map<string,shared_ptr<int>>::const_iterator it;
		it = edgeFlows.find(edgeId);
		int edgSum = (it != edgeFlows.end()) ? *(it->second) : 0;

		// Get the detector flow amount in the edge
		int detSum = computeDetFlowSumPerEdge(edgeId, pDetCont, pDetFlows);

		// Compute the square root difference
		double sqdiff = std::pow(double(edgSum - detSum), 2);
		sum += sqdiff;
		num++;
	}

	// The RMSE
	float rmse = std::sqrt(sum / num);
	float score = 100.0 / (1.0 + rmse);

	// Return the objective score
	return score;
}


//................................................. Compute the flow amount score ...
int GARStaticObjective::computeDetFlowSumPerEdge(const std::string& edgeId,
									 	   	   	 const GARDetectorCon const* pDetCont,
												 const GARDetectorFlows const* pDetFlows) {
	vector<GARDetector*> edgeDetectors = pDetCont->getEdgeDetectors(edgeId);

	int flowSum = 0;

	for (const GARDetector const* pDet : edgeDetectors) {
		flowSum += pDetFlows->getFlowSumSecure(pDet->getID());
	}

	return flowSum;
}


} /* namespace gar */
