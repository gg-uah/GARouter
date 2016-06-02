/****************************************************************************/
/// @file    GARNet.h
/// @author  Daniel Krajzewicz
/// @author  Eric Nicolay
/// @author  Michael Behrisch
/// @date    Thu, 16.03.2006
/// @version $Id: GARNet.h 17235 2014-11-03 10:53:02Z behrisch $
///
// A GAROUTER-network
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.dlr.de/
// Copyright (C) 2001-2014 DLR (http://www.dlr.de/) and contributors
/****************************************************************************/
//
//   This file is part of SUMO.
//   SUMO is free software: you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation, either version 3 of the License, or
//   (at your option) any later version.
//
/****************************************************************************/
#ifndef GARNet_h
#define GARNet_h

// ===========================================================================
// included modules
// ===========================================================================
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

#include <router/RONet.h>
#include <utils/options/OptionsCont.h>
#include "GARDetector.hpp"
#include "GARDetectorCon.hpp"
#include "GARTazCont.hpp"
#include "GARRouteCont.hpp"
#include "GARTripCont.hpp"
#include "GARRouteDesc.hpp"
#include <utils/common/StdDefs.h>
#include <utils/common/SUMOTime.h>
#include <CLogger.hpp>

using gar::GARTazCont;
using gar::GARTripCont;


/**
 * Sorting criterion to keep the node set ordered by the node identifier.
 * It also prevents from allocating two nodes with the same identifier.
 */
struct lex_compare_node {
    bool operator() (const RONode const* lhs,
    				 const RONode const* rhs) const {
    	if (lhs->getID() < rhs->getID()) {
    		return true;
    	}
    	return false;
    }
};

/**
 * A data set of constant RONode pointers.
 * The nodes in the set are sorted regarding the 'lex_compare_node' criterion.
 */
typedef std::set<const RONode const*,lex_compare_node> node_set;


// ===========================================================================
// class definitions
// ===========================================================================
/**
 * @class GARNet
 * @brief A GARROUTER-network
 */
class GARNet: public RONet {
public:
	/** @brief Constructor
	 */
	GARNet(void);

	/// @brief Destructor
	~GARNet(void);

	void buildApproachList();

	void buildNodeList(void);

	void computeTypes(GARDetectorCon& dets, bool sourcesStrict) const;

	void buildRoutes(GARDetectorCon& detCont,
			  	     const GARTazCont& tazCont,
					 GARRouteCont& rouCont,
					 GARTripCont& tripCont,
					 int maxFollowingLength,
					 common::CLogger* logger) const;

	SUMOReal getAbsPos(const GARDetector& det) const;

	void buildEdgeFlowMap(const GARDetectorFlows& flows,
			const GARDetectorCon& detectors, SUMOTime startTime,
			SUMOTime endTime, SUMOTime stepOffset);

	void revalidateFlows(const GARDetectorCon& detectors,
			GARDetectorFlows& flows, SUMOTime startTime, SUMOTime endTime,
			SUMOTime stepOffset);

	void removeEmptyDetectors(GARDetectorCon& detectors,
			GARDetectorFlows& flows);

	void reportEmptyDetectors(GARDetectorCon& detectors,
			GARDetectorFlows& flows);

	void buildDetectorDependencies(GARDetectorCon& detectors);

	void mesoJoin(GARDetectorCon& detectors, GARDetectorFlows& flows);

	bool hasDetector(ROEdge* edge) const;
	const std::vector<std::string>& getDetectorList(ROEdge* edge) const;

	const node_set& getNodeList(void) const;

	const ROEdge const* getpEdge(const std::string& fromNode,
								 const std::string& toNode) const;

protected:
	void revalidateFlows(const GARDetector* detector, GARDetectorFlows& flows,
			SUMOTime startTime, SUMOTime endTime, SUMOTime stepOffset);
	bool isSource(const GARDetector& det, const GARDetectorCon& detectors,
			bool strict) const;
	bool isFalseSource(const GARDetector& det,
			const GARDetectorCon& detectors) const;
	bool isDestination(const GARDetector& det,
			const GARDetectorCon& detectors) const;

	ROEdge* getDetectorEdge(const GARDetector& det) const;
	bool isSource(const GARDetector& det, ROEdge* edge,
			std::vector<ROEdge*>& seen, const GARDetectorCon& detectors,
			bool strict) const;
	bool isFalseSource(const GARDetector& det, ROEdge* edge,
			std::vector<ROEdge*>& seen, const GARDetectorCon& detectors) const;
	bool isDestination(const GARDetector& det, ROEdge* edge,
			std::vector<ROEdge*>& seen, const GARDetectorCon& detectors) const;

	void computeRoutesFor(ROEdge* edge,
						  GARRouteDesc& base,
						  const GARDetector& det,
						  GARRouteCont& into,
						  GARTripCont& tripCont,
						  const GARDetectorCon& detCont,
						  const GARTazCont& tazCont,
						  int maxFollowingLength,
						  std::vector<ROEdge*>& seen,
						  CLogger* logger) const;

	void buildDetectorEdgeDependencies(GARDetectorCon& dets) const;

	bool hasApproaching(ROEdge* edge) const;
	bool hasApproached(ROEdge* edge) const;

	bool hasInBetweenDetectorsOnly(ROEdge* edge,
			const GARDetectorCon& detectors) const;
	bool hasSourceDetector(ROEdge* edge,
			const GARDetectorCon& detectors) const;

	struct IterationEdge {
		int depth;
		ROEdge* edge;
	};

protected:
	class DFRouteDescByTimeComperator {
	public:
		/// Constructor
		explicit DFRouteDescByTimeComperator() {
		}

		/// Destructor
		~DFRouteDescByTimeComperator() {
		}

		/// Comparing method
		bool operator()(const GARRouteDesc& nod1,
				const GARRouteDesc& nod2) const {
			return nod1.duration_2 > nod2.duration_2;
		}
	};

private:
	/// @brief comparator for maps using edges as key, used only in myDetectorsOnEdges to make tests comparable
	struct idComp {
		bool operator()(ROEdge* const lhs, ROEdge* const rhs) const {
			return lhs->getID() < rhs->getID();
		}
	};

	/// @brief Map of edge name->list of names of this edge approaching edges
	std::map<ROEdge*, std::vector<ROEdge*> > myApproachingEdges;

	/// @brief Map of edge name->list of names of edges approached by this edge
	std::map<ROEdge*, std::vector<ROEdge*> > myApproachedEdges;

	mutable std::map<ROEdge*, std::vector<std::string>, idComp> myDetectorsOnEdges;
	mutable std::map<std::string, ROEdge*> myDetectorEdges;

	bool myAmInHighwayMode;
	mutable size_t mySourceNumber, mySinkNumber, myInBetweenNumber,
			myInvalidNumber;

	/// @brief List of ids of edges that shall not be used
	std::vector<std::string> myDisallowedEdges;

	// @brief List of node id's
	node_set myNodes;

	bool myKeepTurnarounds;

};

#endif

/****************************************************************************/

