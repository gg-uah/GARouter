/*
 * XMLUtilities.cpp
 *
 *  Created on: Apr 27, 2015
 *      Author: ctomas
 */

#include <XMLUtilities.hpp>

using std::vector;
using std::string;

namespace common {

//................................................. Parse a XML file ...
boost_pt::ptree XMLUtilities::parseXML(const std::string& xmlFile, CLogger* logger) {
	// Create empty property tree object
	boost_pt::ptree tree;

	try {
		// Parse the XML into the property tree.
		boost_pt::read_xml(xmlFile, tree);
	} catch (std::exception& ex) {
		logger->error("Fail to parse XML file [" + xmlFile + "]" );

		// Return an empty property tree
		return boost_pt::ptree();
	}

	return tree;
}


//................................................. Get the attribute value of a specified node ...
std::vector<std::string> XMLUtilities::getNodeAttrValues(const boost_pt::basic_ptree<std::string, std::string>& btree,
														 const std::string& nodeTag,
														 const std::string& attrName,
														 CLogger* logger) {
	vector<string> attribs;

	for (const auto& node : btree) {
		if (node.first != nodeTag) {
			continue;
		}
		// Get the attribute value
		string attrValue = node.second.get<string>(attrName, "");

		if (attrValue.empty()) {
			logger->warning("Not found the value for attribute [" + attrName + "] in node [" + nodeTag + "]");
			continue;
		}

		logger->debug("Attribute value: [" + attrValue + "]");
		attribs.push_back(attrValue);
	}

	return attribs;
}


} /* namespace common */
