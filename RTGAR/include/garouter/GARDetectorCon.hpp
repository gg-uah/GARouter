/*
 * GARDetectorCon.hpp
 *
 *  Created on: Apr 30, 2015
 *      Author: ctomas
 */

#ifndef GARDETECTORCON_HPP_
#define GARDETECTORCON_HPP_

#include "GARDetector.hpp"
#include "GARDetectorFlow.hpp"
#include "GAREdge.hpp"
#include "GARNet.hpp"
#include <utils/common/RandomDistributor.h>
#include <utils/common/StdDefs.h>

/**
 * @class GARDetectorCon
 * @brief A container for GARDetectors
 */
class GARDetectorCon {
public:
	GARDetectorCon();
	virtual ~GARDetectorCon();
	bool addDetector(GARDetector* dfd);
	void removeDetector(const std::string& id);
	bool detectorsHaveCompleteTypes() const;
	bool detectorsHaveRoutes() const;
	const std::vector<GARDetector*>& getDetectors() const;
	void save(const std::string& file) const;
	void saveAsPOIs(const std::string& file) const;
	void saveRoutes(const std::string& file) const;

	const GARDetector& getDetector(const std::string& id) const;
	GARDetector& getModifiableDetector(const std::string& id) const;
	const GARDetector& getAnyDetectorForEdge(const GAREdge* const edge) const;

	bool knows(const std::string& id) const;
	void writeEmitters(const std::string& file, const GARDetectorFlows& flows,
			SUMOTime startTime, SUMOTime endTime, SUMOTime stepOffset,
			const GARNet& net, bool writeCalibrators, bool includeUnusedRoutes,
			SUMOReal scale, bool insertionsOnly);

	void writeEmitterPOIs(const std::string& file,
			const GARDetectorFlows& flows);

	void writeSpeedTrigger(const GARNet* const net, const std::string& file,
			const GARDetectorFlows& flows, SUMOTime startTime,
			SUMOTime endTime, SUMOTime stepOffset);

	void writeValidationDetectors(const std::string& file, bool includeSources,
			bool singleFile, bool friendly);
	void writeEndRerouterDetectors(const std::string& file);

	int getAggFlowFor(const ROEdge* edge, SUMOTime time, SUMOTime period,
			const GARDetectorFlows& flows) const;

	void guessEmptyFlows(GARDetectorFlows& flows);

	void mesoJoin(const std::string& nid,
			const std::vector<std::string>& oldids);

	/**
	 * Finds the detectors that matches a given type.
	 * @param type	The detector type (source, sink, between)
	 * @return		A vector that comprises the detectors that matches the given type.
	 * @see GARDetectorType
	 */
	std::vector<GARDetector*> getDetectorsByType(const GARDetectorType& type) const;

	/**
	 * Get the detector located on the specified edge.
	 * @param edgeId	The edge identifier.
	 * @return			A pointer to the detector located on the specified edge,
	 * 					or <code>nullptr</code> if the edge has no detector.
	 */
	//const GARDetector const* getDetectorOnEdge(const std::string& edgeId) const;

	/**
	 * Get the detectors located on the specified edge.
	 * @param edgeId	The edge identifier.
	 * @return			A list of pointer to the detector data located on the specified edge,
	 * 					or an empty list if the edge has no detector.
	 */
	std::vector<GARDetector*> getEdgeDetectors(const std::string& edgeId) const;


protected:
	/** @brief Clears the given distributions map, deleting the timed distributions
	 * @param[in] dists The distribution map to clear
	 */
	void clearDists(std::map<size_t, RandomDistributor<size_t>*>& dists) const;

protected:
	std::vector<GARDetector*> myDetectors;
	std::map<std::string, GARDetector*> myDetectorMap;
	std::map<std::string, std::vector<GARDetector*> > myDetectorEdgeMap;

private:
	/// @brief Invalidated copy constructor
	GARDetectorCon(const GARDetectorCon& src);

	/// @brief Invalidated assignment operator
	GARDetectorCon& operator=(const GARDetectorCon& src);

};

#endif /* GARDETECTORCON_HPP_ */
