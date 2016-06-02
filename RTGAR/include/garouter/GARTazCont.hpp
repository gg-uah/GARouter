/*
 * GARTazCont.hpp
 *
 *  Created on: Apr 26, 2015
 *      Author: ctomas
 */

#ifndef GARTAZCONT_HPP_
#define GARTAZCONT_HPP_

#include <GARTaz.hpp>
#include <GARTazGroup.hpp>
#include <CLogger.hpp>
#include <set>

using common::CLogger;

namespace gar {

/**
 * This class implements the TAZ data container.
 * This container holds two data sets:
 * - A TAZ definition set
 * - An OD-group set, each of them encloses a TAZ set belonging to the group.
 */
class GARTazCont {
public:
	/**
	 * Deleted default constructor.
	 * Prevents from creating an empty instance of this class.
	 */
	GARTazCont(void) = delete;

	/**
	 * Deleted copy constructor.
	 * Prevents from creating an instance of this class as a copy of other class.
	 * @param other  The instance to be copied.
	 */
	GARTazCont(const GARTazCont& other) = delete;

	/**
	 * Parameterized constructor.
	 * @param logger	Reference to the ga-router logger.
	 */
	GARTazCont(CLogger* logger);

	/**
	 * Default virtual destructor.
	 */
	virtual ~GARTazCont(void) = default;

	/**
	 * Gets the TAZ groups reported in the file specified in the 'od-groups-file' option.
	 * @return	A set containing the O/D TAZ groups.
	 */
	const gar::grp_set& getOdGroups(void) const;

	/**
	 * Gets the TAZs reported in the file specified in the 'taz-file' option.
	 * @return	A set containing the TAZ data.
	 */
	const gar::taz_set& getTazs(void) const;

	/**
	 * Gets a TAZ element in the TAZ set searching by the TAZ identifier.
	 * @param id	The TAZ identfier
	 * @return		A pointer to the TAZ data that matches the TAZ identifier.
	 */
	std::shared_ptr<const GARTaz> getTaz(const std::string& id) const;

	/**
	 * Gets a TAZ group element in the O/D groups set searching by the group identifier.
	 * @param id	The O/D group identifier.
	 * @return		A pointer to the O/D group data that matches the group identifer.
	 */
	std::shared_ptr<const GARTazGroup> getOdGroup(const std::string& id) const;


	/**
	 * Gets the source edges concerning to the specified district (TAZ).
	 * @param taz	The district (TAZ) identifier where searching the source edges.
	 * @return		A data set containing the source edges in the specified district.
	 */
	const gar::edge_set& getSourceEdges(const std::string& taz) const;

	/**
	 * Gets the sink edges concerning to the specified district (TAZ).
	 * @param taz	The district (TAZ) identifier where searching the sink edges.
	 * @return		A data set containing the sink edges in the specified district.
	 */
	const gar::edge_set& getSinkEdges(const std::string& taz) const;

	/**
	 * Gets the undefined edges concerning to the specified district (TAZ).
	 * @param taz	The district (TAZ) identifier where searching the undefined edges.
	 * @return		A data set containing the undefined edges in the specified district.
	 */
	const gar::edge_set& getUndefEdges(const std::string& taz) const;

	/**
	 * Sets the TAZ data set.
	 * @param tazs	The TAZ data set.
	 */
	void setTazs(const gar::taz_set& tazs);

	/**
	 * Sets the O/D group data set.
	 * @param odgroups	The O/D group data set.
	 */
	void setOdGroups(const gar::grp_set& odgroups);

	/**
	 * Inserts a pointer to a TAZ element into the container TAZ data set.
	 * @param pTaz	A pointer to a TAZ element.
	 * @return
	 */
	gar::taz_set::iterator insertTaz(const std::shared_ptr<const GARTaz>& pTaz);

	/**
	 * Inserts a pointer to a TAZ group into the container O/D group data set.
	 * @param pTaz	A pointer to a TAZ group element.
	 * @return
	 */
	gar::grp_set::iterator insertOdGroup(const std::shared_ptr<const GARTazGroup>& pGroup);

	/**
	 * Searches for a source edge among the TAZs established in the network.
	 * @param pEdge	The source edge identifier for which we want to determine the
	 * 				TAZ it belongs to.
	 * @return		A pointer to the TAZ the source edge belongs to or <code>nullptr</code>
	 * 				if the source edge is not found among the defined TAZs.
	 */
	std::shared_ptr<const GARTaz> findSourceEdgeTaz(const std::string& id) const;

	/**
	 * Searches for a sink edge among the TAZs established in the network.
	 * @param id	The sink edge identifier for which we want to determine the
	 * 				TAZ it belongs to.
	 * @return		A pointer to the TAZ the sink edge belongs to or <code>nullptr</code>
	 * 				if the sink edge is not found among the defined TAZs.
	 */
	std::shared_ptr<const GARTaz> findSinkEdgeTaz(const std::string& id) const;

	/**
	 * Searches for a source edge among the TAZ groups established in the network.
	 * @param id	The source edge identifier for which we want to determine the O/D TAZ group
	 * 				it belongs to.
	 * @return		A pointer to the TAZ group the source edge belongs to or <code>nullptr</code>
	 * 				if the source edge is not found among the defined TAZ groups.
	 */
	std::shared_ptr<const GARTazGroup> findSourceEdgeGroup(const std::string& id) const;

	/**
	 * Searches for a sink edge among the TAZ groups established in the network.
	 * @param id	The sink edge identifier for which we want to determine the O/D TAZ group
	 * 				it belongs to.
	 * @return		A pointer to the TAZ group the sink edge belongs to or <code>nullptr</code>
	 * 				if the sink edge is not found among the defined TAZ groups.
	 */
	std::shared_ptr<const GARTazGroup> findSinkEdgeGroup(const std::string& id) const;

private:
	//! The O/D TAZ groups as depicted in the group file specified in the 'od-group-file' option
	gar::grp_set odGroups;

	//! The TAZ's reported in the file specified in the 'taz-file' option
	gar::taz_set tazs;

	//! The ga-router logger
	CLogger* logger;
};

} /* namespace gar */

#endif /* GARTAZCONT_HPP_ */
