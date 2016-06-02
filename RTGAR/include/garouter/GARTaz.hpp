/*
 * GARTaz.hpp
 *
 *  Created on: Apr 26, 2015
 *      Author: ctomas
 */

#ifndef GARTAZ_HPP_
#define GARTAZ_HPP_

#include <GAREdge.hpp>
#include <set>


namespace gar {

/**
 * This class implements the district (TAZ) in the network.
 * A TAZ depicts an edge list belonging to the same district.
 */
class GARTaz {
public:
	/**
	 * Deleted default constructor.
	 * Prevents from creating an instance of this class with no parameters.
	 */
	GARTaz(void) = delete;

	/**
	 * Default copy constructor.
	 * @param other	 The GARTaz instance to be copied.
	 */
	GARTaz(const GARTaz& other) = default;

	/**
	 * Parameterized constructor.
	 * @param id[in]  		The district (TAZ) identifier
	 * @param sources[in]	The source edges
	 * @param sinks[in]		The sink edges
	 */
	GARTaz(const std::string& id,
		   const gar::edge_set& sources,
		   const gar::edge_set& sinks,
		   const gar::edge_set& undefs);

	/**
	 * Parameterized constructor.
	 * @param id[in]	The TAZ identifier.
	 */
	GARTaz(const std::string& id);

	/**
	 * Default virtual destructor.
	 */
	virtual ~GARTaz(void) = default;

	/**
	 * Gets the district (TAZ) identifier.
	 * @return	The district (TAZ) identifier
	 */
	std::string getId(void) const;

	/**
	 * Gets the source edges in the district.
	 * @return	A data set containing the source edges.
	 */
	const gar::edge_set& getSources(void) const;

	/**
	 * Gets the sink edges in the district.
	 * @return	A data set containing the sink edges.
	 */
	const gar::edge_set& getSinks(void) const;

	/**
	 * Gets the undefined edges in the district.
	 * @return	A data set containing the undefined edges.
	 */
	const gar::edge_set& getUndefs(void) const;

	/**
	 * Gets all the edges in the district.
	 * @return	A data set containing all the edges.
	 */
	const gar::edge_set getEdges(void) const;

	/**
	 * Checks whether the TAZ has the specified source edge.
	 * @param id	The identifier of the edge we want to search.
	 * @return		<code>true</code> if the source edge belongs to the TAZ,
	 * 				<code>false</code> otherwise.
	 */
	bool hasSourceEdge(const std::string& id) const;

	/**
	 * Checks whether the TAZ has the specified sink edge.
	 * @param id	The identifier of the sink edge we want to search.
	 * @return		<code>true</code> if the sink edge belongs to the TAZ,
	 * 				<code>false</code> otherwise.
	 */
	bool hasSinkEdge(const std::string& id) const;

	/**
	 * Checks whether the TAZ has the specified undefined edge.
	 * @param id	The identifier of the undefined edge we want to search.
	 * @return		<code>true</code> if the undefined edge belongs to the TAZ,
	 * 				<code>false</code> otherwise.
	 */
	bool hasUndefEdge(const std::string& id) const;

	/**
	 * Checks whether the TAZ has the specified edge.
	 * @param id	The identifier of the edge we want to search.
	 * @return		<code>true</code> if the edge belongs to the TAZ,
	 * 				<code>false</code> otherwise.
	 */
	bool hasEdge(const std::string& id) const;

	/**
	 * Gets the stringfied TAZ data.
	 * Builds a string containing the TAZ identifier and the source
	 * and sink edges in the TAZ.
	 * @return	A string reporting the TAZ data.
	 */
	std::string toString(void) const;

	/**
	 * Overloads the 'is equal to' comparison operator.
	 * @param rhs	The right-hand side TAZ in an 'is equal to' operation.
	 * @return		<code>true</code> if this TAZ and the rhs TAZ have the same identifier,
	 * 				<code>false</code> otherwise.
	 */
	bool operator==(const GARTaz& rhs) const;

private:
	//! The district (TAZ) unique identifier
	std::string id;

	//! The source edges in the TAZ
	gar::edge_set sources;

	//! The sink edges in the TAZ
	gar::edge_set sinks;

	//! The undefined edges in the TAZ
	gar::edge_set undefs;
};


/**
 * Sorting criterion to keep the TAZ set ordered by the TAZ id.
 * It also prevents from allocating two TAZ's with the same id.
 */
struct lex_compare_taz {
    bool operator() (const std::shared_ptr<const GARTaz>& lhs,
    				 const std::shared_ptr<const GARTaz>& rhs) const {
        return lhs->getId() < rhs->getId();
    }
};

/**
 * A set of constant GARTaz pointers to constant GARTaz data.
 * The TAZs in a set are sorted regarding the 'lex_compare_taz' criterion.
 */
typedef std::set<std::shared_ptr<const GARTaz>,lex_compare_taz> taz_set;


} /* namespace gar */

#endif /* GARTAZ_HPP_ */
