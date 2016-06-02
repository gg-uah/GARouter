/*
 * GARObjective.cpp
 *
 *  Created on: Jun 10, 2015
 *      Author: ctomas
 */

#include <GARDynObjective.hpp>
#include <GARDynObjData.hpp>
#include <GARUtils.hpp>
#include <router/ROEdge.h>
#include <StringUtilities.hpp>
#include <FileUtils.hpp>
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

//................................................. Class constants ...
const std::string GARDynObjective::GAR_LOOP_MEAS_FILE ("__gar_loop_measurements.xml");
const std::string GARDynObjective::GAR_ALLELE_DEPART_SEPARATOR ("__");


//................................................. The genome objective function ...
float GARDynObjective::objective(GAGenome& __genome) {
	float score = 0.0;

	// Cast to the suitable genome
	genome_dyn& genome = (genome_dyn&)__genome;

	// Get the data required by the objective from the genome user data
	const GARDynObjData const* pData = (GARDynObjData*)genome.userData();

	// Write the emitter file
	int status = writeEmitterFile(pData->getEmitFile(),
					 	 	 	  pData->getBegin(),
								  pData->getEnd(),
								  pData->getTimeStep(),
								  pData->getpDetCont(),
								  pData->getpDetFlows(),
								  pData->getpRouCont(),
								  pData->getRoutesSolution(),
								  genome);

	// If writeEmitterFile fails, return 0.0 score
	if (status > 0) {
		return 0.0;
	}

	SUMOTime endTime = runSUMO(pData->getNetFile(),
							   pData->getLoopFile(),
							   pData->getRouFile(),
							   pData->getEmitFile(),
							   pData->getBegin(),
							   pData->getTimeToTeleport());

	if (endTime < 0) {
		std::cerr << "SUMO simulation failed" << std::endl;
		return 0.0;
	}


	// Read measurements from induction loop measure file
	shared_ptr<GARDetectorFlows> pSimFlowData = readLoopMeasures(pData->getLoops2Detectors(),
															 	 pData->getBegin(),
																 pData->getEnd(),
																 pData->getTimeStep());

	// Evaluate the score of the SUMO simulation
	score = computeScore(pData->getpDetCont()->getDetectors(),
						 pData->getpDetFlows(),
						 pSimFlowData.get());

	return score;
}


//................................................. Write the emitter file ...
int GARDynObjective::writeEmitterFile(const string& emittFilename,
							       	  const SUMOTime& begin,
									  const SUMOTime& end,
									  const SUMOTime& timeStep,
									  const GARDetectorCon const* pDetCont,
									  const GARDetectorFlows const* pFlows,
									  const GARRouteCont const * pRouCont,
									  const vector<string>& routes,
									  const genome_dyn& genome) {
	int result = 0;

	// Create a new emitter file
	ofstream emitterFile(emittFilename, std::ios_base::trunc);

	if (!emitterFile.is_open()) {
		std::cerr << "Fail to open emitter file [" << emittFilename << "]" << std::endl;
		return 1;
	}

	// Write header
	emitterFile << "<additional xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\" xsi:noNamespaceSchemaLocation=\"http://sumo.dlr.de/xsd/additional_file.xsd\">" << std::endl;

	// Write vehicle emitters
	for (int i = 0; i < genome.size(); i++) {
		string routeName = routes[i];
		string departTime = StringUtilities::toString<int>(genome.gene(i));

		if (routeName.empty() || departTime.empty()) {
			continue;
		}

		std::shared_ptr<const GARRouteDesc> pRd = pRouCont->getRouteDesc(routeName);
		if (pRd == nullptr) {
			std::cerr << "Warning: Unable to find route [" << routeName << "] in the routes container" << std::endl;
			return 1;
		}
		const ROEdge const* pDepartEdge = pRd->edges2Pass.at(0);
		const GARDetector& departDet = pDetCont->getAnyDetectorForEdge(static_cast<const GAREdge*>(pDepartEdge));

		// Compute the depart speed and position
		double speed = computeDepartSpeed(begin, end, timeStep, pDetCont, pFlows, pRouCont, routeName);
		double departPos = departDet.getPos();

		//Write the vehicle emitter
		emitterFile << "\t<vehicle id=" << boost::format("'emitter_%04i'") % i
					<< " depart='" << departTime << "' departLane='" << 0 << "' departPos='" << departPos
					<< "' departSpeed='" << speed << "' route='" << routeName << "'/>" << std::endl;
	}

	// Write footer
	emitterFile << "</additional>" << std::endl;

	// Close the induction loop file
	if (emitterFile) {
		emitterFile.close();
	}

	return result;
}


//................................................. Run the SUMO simulation ...
int GARDynObjective::runSUMO(const std::string& netFile,
		   	   	   	  	  	 const std::string& loopFile,
							 const std::string& rouFile,
							 const std::string& emittFile,
							 const SUMOTime& begin,
							 const SUMOTime& teleport) {
	SUMOTime endTime = 0;
	FILE *fp;
	char buff[512];

	string sumoCommand = string("sumo") +
						 " --net-file " + netFile +
						 " --additional-files " + loopFile + "," + rouFile + "," + emittFile +
						 " --begin " + StringUtilities::toString<SUMOTime>(begin) +
						 " --time-to-teleport " + StringUtilities::toString<SUMOTime>(teleport);

	// Open a pipe stream to execute the SUMO command
	if (!(fp = popen(sumoCommand.c_str(), "r"))) {
		return -1;
	}

	// Read the output of the command execution from the pipe
	while (fgets(buff, sizeof(buff), fp) != NULL);
	//endTime = getSimulationEndTime(fp);

	// Close the pipe stream when the command finishes
	if (pclose(fp) == -1) {
	    std::cerr << "Fail to close the pipe stream reading the output of the SUMO simulation" << std::endl;
	    return -1;
	}

	return endTime;
}


//................................................. Get the end time of the SUMO simulation ...
SUMOTime GARDynObjective::getSimulationEndTime(FILE* fp) {
	SUMOTime endTime;
	char buff[512];

	boost::regex re ("^Step #([0-9]{1,4}|[1-7][0-9]{4}|8[0-5][0-9]{3}|86[0-3][0-9]{2}|86400)\\.");
	while (fgets(buff, sizeof(buff), fp) != NULL) {
		string output (buff);

		vector<string> strs;
		boost::split(strs, output, boost::is_any_of("\r\n"));

		for (string line : strs) {
			const int subs[] = {1};  // we just want to see group 1
			boost::sregex_token_iterator it (line.begin(), line.end(), re, subs);
			boost::sregex_token_iterator end;

			// Get the simulation step time from group 1
			if (it == end) {
				continue;
			}
			endTime = StringUtilities::toInt(*it);
		}
	}

	return endTime;
}


//................................................. Read the induction loop measurements ...
std::shared_ptr<GARDetectorFlows> GARDynObjective::readLoopMeasures(const gar::loop2det_map& loops2Dets,
							   	   	   	    			     	    const SUMOTime& beginTime,
																    const SUMOTime& endTime,
																    const SUMOTime& timeStep) {
	// Create the detector flows data
	std::shared_ptr<GARDetectorFlows> pDetFlows = std::make_shared<GARDetectorFlows>(beginTime, endTime, timeStep);

	// Find the measures file
	string measFile;
	common::FileUtils::findFile("RTGAR",
								GAR_LOOP_MEAS_FILE,
								measFile);

    using boost::property_tree::ptree;
    try {
 		// Create empty property tree object
		boost_pt::ptree tree;

		// Parse the XML into the property tree.
		boost_pt::read_xml(measFile, tree);

		// Traverse the property tree to get the induction loop measurements
		for (const ptree::value_type& interval : tree.get_child("detector")) {
			if (interval.first != "interval") {
				continue;
			}

			// Get the measurement attributes
			string id 		= interval.second.get<string>("<xmlattr>.id", "");
			SUMOTime begin 	= (int)StringUtilities::toFloat(interval.second.get<string>("<xmlattr>.begin", ""));
			int nVehContrib = StringUtilities::toInt(interval.second.get<string>("<xmlattr>.nVehContrib", "0"));
			float speed 	= StringUtilities::toFloat(interval.second.get<string>("<xmlattr>.speed", "-1.00"));

			// Discard measurements later than the end time
			if (begin > endTime) {
				continue;
			}

			// Get the detector data corresponding to the induction loop
			auto it = loops2Dets.find(id);
			if (it == loops2Dets.end()) {
				std::cerr << "Can't find detector for induction loop [" << id << "]" << std::endl;
				continue;
			}
			string detectorId = (it->second)->getID();

			FlowDef fd;
			fd.qPKW  = nVehContrib;
			fd.vPKW  = speed;
			fd.qLKW  = 0.0;
			fd.vLKW  = 0.0;
			fd.isLKW = 0.0;
			fd.fLKW  = 0.0;
			fd.firstSet = false;

			pDetFlows->addFlow(detectorId, begin, fd);
		}
    } catch (boost_pt::ptree_bad_data& ex) {
    	std::cerr << "Bad data exception caught while loading flow measures from file '" << GAR_LOOP_MEAS_FILE << "':" << string(ex.what()) << std::endl;
    	return nullptr;
    } catch (boost_pt::ptree_bad_path& ex) {
    	std::cerr << "Bad path exception caught while loading flow measures from file '" << GAR_LOOP_MEAS_FILE << "':" << string(ex.what()) << std::endl;
    	return nullptr;
    } catch (boost_pt::ptree_error& ex) {
    	std::cerr << "Unknown exception caught while loading flow measures from file '" << GAR_LOOP_MEAS_FILE << "':" << string(ex.what()) << std::endl;
    	return nullptr;
    } catch (common::StringUtilsException& ex) {
    	std::cerr << "Fail to read induction loop measurements from file '" << GAR_LOOP_MEAS_FILE << "':" << ex.getReason() << std::endl;
    	return nullptr;
    }

    return pDetFlows;
}


//................................................. Write the detector flows file ...
float GARDynObjective::computeScore(const vector<GARDetector*>& detectors,
							 	    const GARDetectorFlows const* pGoalFlowData,
								    const GARDetectorFlows const* pSimFlowData) {
	double sum = 0.0;
	double num = 0.0;

	// Compute the root mean squared error (RMSE) of the flow data
	for (const GARDetector const* pDet : detectors) {
		string detectorId = pDet->getID();
		const std::vector<FlowDef> goalFlows = pGoalFlowData->getFlowDefs(detectorId);
		const std::vector<FlowDef> simFlows  = pSimFlowData->getFlowDefs(detectorId);


		if (goalFlows.size() != simFlows.size()) {
			return 0.0;
		}

		for (int i = 0; i < goalFlows.size(); i++) {
			double sqdiff = std::pow(double(simFlows[i].qPKW - goalFlows[i].qPKW), 2);
			sum += sqdiff;
			num++;
		}
	}

	// The RMSE
	float rmse = std::sqrt(sum/num);

	// Return the objective score
	return 100.0/(1.0 + rmse);
}


//................................................. Compute the emitter depart speed ...
float GARDynObjective::computeDepartSpeed(const SUMOTime& startTime,
										  const SUMOTime& endTime,
										  const SUMOTime& timeStep,
										  const GARDetectorCon const* pDetCont,
										  const GARDetectorFlows const* pFlows,
										  const GARRouteCont const * pRouCont,
										  const std::string& routeName) {
    SUMOReal speed = 0;
    SUMOReal departSpeed = 0;
    unsigned int index = 0;

    try {
		// Get the detector id at the origin edge of the route
		const GAREdge const* pEdge = GARUtils::getOriginEdge(pRouCont, routeName);
		if (pEdge == nullptr) {
			return (SUMOReal) 0.;
		}
		const GARDetector& det = pDetCont->getAnyDetectorForEdge(pEdge);

		// Get the speed allowed on the edge at the origin as the default speed
		SUMOReal defaultSpeed = pEdge->getSpeed();

		// Get the detector flows at the origin edge
		const std::vector<FlowDef>& flows = pFlows->getFlowDefs(det.getID());

		for (SUMOTime t = startTime; t < endTime; t += timeStep, index++) {
			assert(index < flows.size());

			// Get the speed from vLKW or vPKW
			const FlowDef& fd = flows[index];
			speed = MAX2(fd.vLKW, fd.vPKW);

			if (speed <= 0 || speed > 250) {
				speed += defaultSpeed;
			}

			departSpeed += (speed / 3.6);
		}

		departSpeed /= SUMOReal(index + 1.0);

    } catch(const std::exception& ex) {
    	return (SUMOReal) 0.;
    }

    return departSpeed;
}

} /* namespace gar */
