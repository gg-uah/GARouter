/****************************************************************************/
/// @file    GARDetector.h
/// @author  Daniel Krajzewicz
/// @author  Eric Nicolay
/// @author  Michael Behrisch
/// @date    Thu, 16.03.2006
/// @version $Id: GARDetector.h 17235 2014-11-03 10:53:02Z behrisch $
///
// Class representing a detector within the DFROUTER
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
#ifndef GARDetector_h
#define GARDetector_h

// ===========================================================================
// included modules
// ===========================================================================
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

#include <map>
#include <string>
#include <vector>
#include <set>
#include <utils/common/SUMOTime.h>
#include <utils/common/RandomDistributor.h>
#include <utils/common/Named.h>
#include "GARRouteCont.hpp"


// ===========================================================================
// class declarations
// ===========================================================================
class GARRouteCont;
class GARDetectorFlows;
class ROEdge;
class GAREdge;
class GARDetectorCon;
class GARNet;
struct GARRouteDesc;
class OutputDevice;

// ===========================================================================
// enumerations
// ===========================================================================
/**
 * @enum GARDetectorType
 * @brief Numerical representation of different detector types
 */
enum GARDetectorType {
	/// A not yet defined detector
	TYPE_NOT_DEFINED = 0,

	/// A detector which had to be discarded (!!!)
	DISCARDED_DETECTOR,

	/// An in-between detector
	BETWEEN_DETECTOR,

	/// A source detector
	SOURCE_DETECTOR,
	SINK_DETECTOR
};

// ===========================================================================
// class definitions
// ===========================================================================
/**
 * @class GARDetector
 * @brief Class representing a detector within the DFROUTER
 */
class GARDetector: public Named {
public:
	/** @brief Constructor
	 *
	 * This constructor is used when detectors are read from a file
	 *
	 * @param[in] id The id of the detector
	 * @param[in] laneID The id of the lane the detector is placed at
	 * @param[in] pos The position of the detector at the lane
	 * @param[in] type The df-router type of the detector
	 * @see GARDetectorType
	 */
	GARDetector(const std::string& id,
				 const std::string& laneID,
				 SUMOReal pos,
				 const GARDetectorType type);

	/** @brief Constructor
	 *
	 * This constructor is used when detectors shall be joined
	 *
	 * @param[in] id The id of the detector
	 * @param[in] f A detector from which routes shall be copied
	 */
	GARDetector(const std::string& id,
				 const GARDetector& f);

	/// @brief Destructor
	~GARDetector();

	/// @name Atomar getter methods
	/// @{

	/** @brief Returns the id of the lane this detector is placed on
	 * @return The id of the lane this detector is placed on
	 */
	const std::string& getLaneID() const {
		return myLaneID;
	}
	;

	/** @brief Returns the id of the edge this detector is placed on
	 * @return The id of the edge this detector is placed on
	 */
	std::string getEdgeID() const {
		return myLaneID.substr(0, myLaneID.rfind('_'));
	}

	/** @brief Returns the position at which the detector lies
	 * @return The position of the detector at the lane
	 */
	SUMOReal getPos() const {
		return myPosition;
	}
	;

	/** @brief Returns the type of the detector
	 * @return This detector's type
	 * @see GARDetectorType
	 */
	GARDetectorType getType() const {
		return myType;
	}
	;
	/// @}

	void setType(GARDetectorType type);
	void addRoute(GARRouteDesc& nrd);
	void addRoutes(GARRouteCont* routes);
	bool hasRoutes() const;
	const std::vector<GARRouteDesc>& getRouteVector() const;
	void addPriorDetector(const GARDetector* det);
	void addFollowingDetector(const GARDetector* det);
	const std::set<const GARDetector*>& getPriorDetectors() const;
	const std::set<const GARDetector*>& getFollowerDetectors() const;

	/// @name Writing methods
	/// @{

	bool writeEmitterDefinition(const std::string& file,
			const std::map<size_t, RandomDistributor<size_t>*>& dists,
			const GARDetectorFlows& flows, SUMOTime startTime,
			SUMOTime endTime, SUMOTime stepOffset, bool includeUnusedRoutes,
			SUMOReal scale, bool insertionsOnly, SUMOReal defaultSpeed) const;
	bool writeRoutes(std::vector<std::string>& saved, OutputDevice& out);
	void writeSingleSpeedTrigger(const std::string& file,
			const GARDetectorFlows& flows, SUMOTime startTime,
			SUMOTime endTime, SUMOTime stepOffset,
			SUMOReal defaultSpeed);
	void writeEndRerouterDetectors(const std::string& file);
	/// @}

	void buildDestinationDistribution(const GARDetectorCon& detectors,
			SUMOTime startTime, SUMOTime endTime, SUMOTime stepOffset,
			const GARNet& net,
			std::map<size_t, RandomDistributor<size_t>*>& into) const;

	void computeSplitProbabilities(const GARNet* net,
			const GARDetectorCon& detectors, const GARDetectorFlows& flows,
			SUMOTime startTime, SUMOTime endTime, SUMOTime stepOffset);

	const std::vector<std::map<GAREdge*, SUMOReal> >& getSplitProbabilities() const {
		return mySplitProbabilities;
	}

protected:
	int getFlowFor(const ROEdge* edge, SUMOTime time) const;
	SUMOReal computeDistanceFactor(const GARRouteDesc& rd) const;

protected:
	std::string myLaneID;SUMOReal myPosition;
	GARDetectorType myType;
	GARRouteCont* myRoutes;
	std::set<const GARDetector*> myPriorDetectors, myFollowingDetectors;
	std::vector<std::map<GAREdge*, SUMOReal> > mySplitProbabilities;
	std::map<std::string, GAREdge*> myRoute2Edge;

private:
	/// @brief Invalidated copy constructor
	GARDetector(const GARDetector& src);

	/// @brief Invalidated assignment operator
	GARDetector& operator=(const GARDetector& src);

};


namespace gar {

/**
 * A map that connects induction loop identifiers with detector data.
 */
typedef std::map<std::string, const GARDetector const*> loop2det_map;

} /* namespace gar */

#endif

/****************************************************************************/

