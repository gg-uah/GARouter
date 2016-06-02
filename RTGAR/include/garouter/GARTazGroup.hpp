/*
 * GARTazGroup.hpp
 *
 *  Created on: Apr 26, 2015
 *      Author: ctomas
 */

#ifndef GARTAZGROUP_HPP_
#define GARTAZGROUP_HPP_

#include <GARTaz.hpp>

namespace gar {

/**
 * This class implements the O/D district (TAZ) group defined in the network.
 * The district group comprises the districts (TAZ) to be considered as a source/destination
 * zone in the traffic simulation. The traffic flow will be performed among the defined
 * TAZ groups; e.g. having two TAZ groups:
 * - "TAZ_GROUP_1" groups the "TAZ1", "TAZ2", "TAZ3" districts.
 * - "TAZ_GROUP_2" groups the "TAZ4", "TAZ5" districts
 * the traffic simulation will be carried out between the edges comprised in the districts bound to
 * "TAZ_GROUP_1" ("TAZ1", "TAZ2", "TAZ3") and those bound to "TAZ_GROUP_2" ("TAZ4", "TAZ5")
 */
class GARTazGroup {
public:
	/**
	 * Invalidated default constructor.
	 */
	GARTazGroup(void) = delete;

	/**
	 * Default copy constructor
	 * @param other	The taz group instance to be copied.
	 */
	GARTazGroup(const GARTazGroup& other) = default;

	/**
	 * Entirely parameterized constructor.
	 * @param id	  The O/D group identifier.
	 * @param odtazs  A set containing pointers to the TAZs belonging
	 * 				  to the group.
	 */
	GARTazGroup(const std::string& id,
			    const gar::taz_set& odtazs);

	/**
	 * Partially parameterized constructor.
	 * @param id	The O/D group identifier.
	 */
	GARTazGroup(const std::string& id);

	/**
	 * Default virutal destructor.
	 */
	virtual ~GARTazGroup(void) = default;

	/**
	 * Gets the TAZ group identifier.
	 * @return	The TAZ group identifier.
	 */
	const std::string& getId(void) const;


	/**
	 * Gets the TAZ data set regarding to the O/D group.
	 * @return	The OD-group TAZ data set.
	 */
	const gar::taz_set& getTazs(void) const;

	/**
	 * Checks whether the O/D TAZ group has the specified source edge.
	 * @param id	The identifier of the edge we want to search.
	 * @return		<code>true</code> if the source edge belongs to the O/D TAZ group,
	 * 				<code>false</code> otherwise.
	 */
	bool hasSourceEdge(const std::string& id) const;

	/**
	 * Checks whether the O/D TAZ group has the specified sink edge.
	 * @param id	The identifier of the sink edge we want to search.
	 * @return		<code>true</code> if the sink edge belongs to the O/D TAZ group,
	 * 				<code>false</code> otherwise.
	 */
	bool hasSinkEdge(const std::string& id) const;

	/**
	 * Gets the O/D group stringfied data.
	 * Builds a string containing the OD-group identifier and the TAZs
	 * concerning to a group.
	 * @return	A string reporting the O/D group data.
	 */
	std::string toString(void) const;

	/**
	 * Overloads the 'is equal to' comparison operator.
	 * @param rhs	The right-hand side taz group in an 'is equal to' operation.
	 * @return
	 */
	bool operator==(const GARTazGroup& rhs) const;

	/**
	 * Overloads the 'is less than' comparison operator.
	 * @param rhs	The right-hand side taz group in an 'is less than' operation.
	 * @return
	 */
	bool operator<(const GARTazGroup& rhs) const ;


private:
	//! The O/D group identifier
	std::string id;

	//! The TAZs concerning to the group
	gar::taz_set odtazs;
};


/**
 * Internal comparison operator used by the O/D TAZ group set for keeping itself sorted
 * by the group identifier and preventing from allocating two TAZ groups with the same
 * identifier.
 */
struct lex_compare_grp {
    bool operator() (const std::shared_ptr<const GARTazGroup>& lhs,
    				 const std::shared_ptr<const GARTazGroup>& rhs) const {
        return lhs->getId() < rhs->getId();
    }
};

/**
 * A set of constant GARTazGroup pointers to constant GARTaz data.
 * The groups in a set are sorted regarding the 'lex_compare_grp' criterion.
 */
typedef std::set<std::shared_ptr<const GARTazGroup>, lex_compare_grp> grp_set;


} /* namespace gar */

#endif /* GARTAZGROUP_HPP_ */
