/*
 * XMLUtilities.hpp
 *
 *  Created on: Apr 27, 2015
 *      Author: ctomas
 */

#ifndef INCLUDE_XMLUTILITIES_HPP_
#define INCLUDE_XMLUTILITIES_HPP_

#include <CLogger.hpp>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/xml_parser.hpp>

namespace boost_pt = boost::property_tree;

namespace common {

class XMLUtilities {
public:
	XMLUtilities() = delete;

	virtual ~XMLUtilities() = delete;

	static boost_pt::ptree parseXML(const std::string& xmlFile, CLogger* logger);

	static std::vector<std::string> getNodeAttrValues(const boost_pt::basic_ptree<std::string, std::string>& btree,
											  	      const std::string& elementTag,
													  const std::string& attrName,
													  CLogger* logger);
};

} /* namespace common */

#endif /* INCLUDE_XMLUTILITIES_HPP_ */
