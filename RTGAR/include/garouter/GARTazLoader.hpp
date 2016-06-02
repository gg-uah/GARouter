/*
 * GARTazLoader.h
 *
 *  Created on: Apr 26, 2015
 *      Author: ctomas
 */

#ifndef GARTAZSLOADER_HPP_
#define GARTAZSLOADER_HPP_

#include <CLogger.hpp>
#include <utils/options/OptionsCont.h>
#include <GARNet.hpp>
#include <GARTazCont.hpp>
#include <string>
#include <set>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/xml_parser.hpp>

using common::CLogger;
namespace boost_pt = boost::property_tree;

namespace gar {

/**
 * This class implements the TAZ data loader. It reads the TAZ group and TAZ definition
 * data from XML files and stores in a container.
 * A file containing a mapping from traffic assignment zones (TAZ) to edges looks as following:
 * <tazs>
 *   <taz id="<TAZ_ID>">
 *     <tazSource id="<EDGE_ID>" weight="<PROBABILITY_TO_USE>"/>
 *     ... further source edges ...
 *
 *     <tazSink id="<EDGE_ID>" weight="<PROBABILITY_TO_USE>"/>
 *     ... further destination edges ...
 *   </taz>
 *
 * ... further traffic assignment zones (districts) ...
 *
 * </tazs>
 *
 * If you do not want to distinguish between source and sink edges and give all edges the same probability
 * you may as well use the following abbreviated form (it is also posible to mix both styles):
 * <tazs>
 *    <taz id="<TAZ_ID>" edges="<EDGE_ID> <EDGE_ID> ..."/>
 *
 *    ... further traffic assignment zones (districts) ...
 *
 * </tazs>
 *
 * @see GARTazCont
 * @see GARTaz
 * @see GARTazGroup
 */
class GARTazLoader {
public:
	//!< The O/D group root tag name
    const static std::string OD_GROUP_ROOT_TAG;

    //!< The O/D group tag name
    const static std::string OD_GROUP_TAG;

    //!< The TAZ root tag name
    const static std::string TAZ_ROOT_TAG;

    //!< The TAZ tag name
    const static std::string TAZ_TAG;

    //!< The source tag name
    const static std::string TAZ_SOURCE_TAG;

    //!< The sink tag name
    const static std::string TAZ_SINK_TAG;

    //!< The id attribute name
    const static std::string ID_ATTRIB;

    //!< The edges attribute name
    const static std::string EDGES_ATTRIB;


	/**
	 * Invalidated empty constructor.
	 */
	GARTazLoader(void) = delete;

	/**
	 * @brief Invalidated copy constructor.
	 * A <code>GARTazLoader</code> object can't be initialized from another object of the same type.
	 * @param other An existing <code>GARTazLoader</code> object to copy in the initialization.
	 */
	GARTazLoader(const GARTazLoader& other) = delete;

	/**
	 * Validated parameterized constructor.
	 * @param options	The ga-router input options.
	 * @param net		The network data.
	 * @param logger	Reference to the logger.
	 */
	GARTazLoader(const OptionsCont& options,
				 const GARNet& net,
				 CLogger* logger);

	/**
	 * Default virtual destructor.
	 */
	virtual ~GARTazLoader(void) = default;

	/**
	 * Get the TAZ data container.
	 * @return	A pointer to the TAZ data container.
	 */
	std::shared_ptr<GARTazCont> getpTazCont(void) const;

	/**
	 * Set the pointer to the TAZ data container.
	 * @param pTazCont	A pointer to the TAZ data container.
	 */
	void setpTazCont(const std::shared_ptr<GARTazCont>& pTazCont);

	/**
	 * Runs the loader.The TAZ group and TAZ definition data files
	 * are read and the data is loaded into a container.
	 * @return	<code>0</code> if TAZ data is successfully loaded,
	 * 			<code>1</code> otherwise.
	 */
	int run(void);

	/**
	 * Logs the districts (TAZ) and the district groups data.
	 */
	void logTazData(void) const;

protected:
	/**
	 * Loads the TAZ definition from a XML data file.
	 * @param tazFile	The district data file.
	 * @return 			<code>1</code> if the TAZ definitions are successfully loaded,
	 * 		   			<code>0</code> otherwise.
	 */
	int loadTazs(const std::string& tazFile);

	/**
	 * Parses the TAZ nodes in the TAZ file.
	 * @param tree	The root tree of the XML TAZ document file.
	 * @return		A set containing pointers to the loaded TAZ data elements.
	 */
	gar::taz_set parseTazs(const boost::property_tree::ptree& tree)
			throw(boost::property_tree::ptree_bad_data,
				  boost::property_tree::ptree_bad_path,
				  boost::property_tree::ptree_error);

	/**
	 * @brief Parses the edges attribute in the TAZ tag.
	 * The edges reported in this attribute are neither source nor sink specified.
	 * @param edgesAttr	A string containing a list of edges identifiers separated
	 * 					by whitespace characters.
	 * @return			A set containing pointers to the edges with undefined type.
	 */
	gar::edge_set parseEdgesAttribute(const std::string & edgesAttr)
			throw(boost::property_tree::ptree_bad_data,
				  boost::property_tree::ptree_bad_path,
				  boost::property_tree::ptree_error);

	/**
	 * Parses the TAZ edges in the TAZ file.
	 * @param taz	The 'taz' node property tree.
	 * @param type	The sink/source tag identifier.
	 * @return		A set containing pointers to the sink/source edges related to a 'taz' node.
	 */
	gar::edge_set parseTazEdges(const boost::property_tree::ptree::value_type& taz,
						        const std::string& type)
			throw(boost::property_tree::ptree_bad_data,
				  boost::property_tree::ptree_bad_path,
				  boost::property_tree::ptree_error);

	/**
	 * Loads the O/D TAZ groups from a XML data file.
	 * @param grpFile  The district group data file.
	 * @return 		   <code>1</code> if the TAZ groups are successfully loaded,
	 * 		   		   <code>0</code> otherwise.
	 */
	int loadGroups(const std::string& grpFile);

	/**
	 * Parses the O/D group nodes in the OD-group file.
	 * @param tree	The root tree of the XML OD-group document file.
	 * @return		A set containing pointers to the loaded OD-group data elements.
	 */
	gar::grp_set parseODGroups(const boost::property_tree::ptree& tree)
			throw(boost::property_tree::ptree_bad_data,
				  boost::property_tree::ptree_bad_path,
				  boost::property_tree::ptree_error);

	/**
	 * Parse the TAZs related to an O/D group in the OD-group file.
	 * @param odgroup	The 'odgroup' node property tree.
	 * @return			A set containing the TAZs related to an 'odgroup' node.
	 */
	gar::taz_set parseODTazs(const boost::property_tree::ptree::value_type& odgroup)
			throw(boost::property_tree::ptree_bad_data,
				  boost::property_tree::ptree_bad_path,
				  boost::property_tree::ptree_error);

private:
	//! The TAZ data container, which holds the OD groups and the TAZs in each group.
	std::shared_ptr<GARTazCont> pTazCont;

	//! The ga-router input options.
	const OptionsCont& options;

	//! The transport network.
	const GARNet& net;

	//! The ga-router logger.
	CLogger* logger;
};

} /* namespace gar */

#endif /* GARTAZSLOADER_H_ */
