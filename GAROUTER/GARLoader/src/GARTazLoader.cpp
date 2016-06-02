/*
 * GARTazLoader.cpp
 *
 *  Created on: Apr 26, 2015
 *      Author: ctomas
 */

#include <GARTazLoader.hpp>
#include <GARTaz.hpp>
#include <memory>
#include <StringUtilities.hpp>

using std::string;
using std::vector;
using std::set;
using std::shared_ptr;
using common::CLogger;
using common::StringUtilities;

namespace gar {

//................................................. Parameterized constructor ...
const std::string GARTazLoader::OD_GROUP_ROOT_TAG = "odgroups";

const std::string GARTazLoader::OD_GROUP_TAG = "odgroup";

const std::string GARTazLoader::TAZ_ROOT_TAG = "tazs";

const std::string GARTazLoader::TAZ_TAG = "taz";

const std::string GARTazLoader::TAZ_SOURCE_TAG = "tazSource";

const std::string GARTazLoader::TAZ_SINK_TAG = "tazSink";

const std::string GARTazLoader::ID_ATTRIB = "<xmlattr>.id";

const std::string GARTazLoader::EDGES_ATTRIB = "<xmlattr>.edges";


//................................................. Parameterized constructor ...
GARTazLoader::GARTazLoader(const OptionsCont& options,
		 	 	 	 	   const GARNet& net,
						   CLogger* logger)
: pTazCont (new GARTazCont(logger)),
  options  (options),
  net      (net),
  logger   (logger) {

	// Intentionally left empty
}


//................................................. Gets the TAZ data container ...
std::shared_ptr<GARTazCont> GARTazLoader::getpTazCont(void) const {
	return this->pTazCont;
}


//................................................. Sets the TAZ data container ...
void GARTazLoader::setpTazCont(const std::shared_ptr<GARTazCont>& pTazCont) {
	this->pTazCont = pTazCont;
}


//................................................. Runs the TAZ data loader ...
int GARTazLoader::run(void) {
	int status = 0;

	// Get the TAZ file from the options
	if (!options.isSet("taz-file")) {
		logger->error("District file is not specified in the 'taz-file' option");
		return 1;
	}
	string tazFile = options.getString("taz-file");

	// Get the TAZ group file from the options
	if (!options.isSet("od-groups-file")) {
		logger->error("O/D group file is not specified in the 'od-groups-file' option");
		return 1;
	}
	string grpFile = options.getString("od-groups-file");

	logger->debug("Load TAZs from file: [" + tazFile + "]");
	logger->debug("Load O/D groups from file: [" + grpFile + "]");


	logger->info(">> Loading tazs");
	status = this->loadTazs(tazFile);
	if (status > 0) {
		return status;
	}

	logger->info(">> Loading O/D taz groups");
	status = this->loadGroups(grpFile);
	if (status > 0) {
		return status;
	}

    return 0;
}


//................................................. Logs the TAZ data ...
void GARTazLoader::logTazData(void) const {
	logger->info("............... TAZs ...");
	for (const shared_ptr<const GARTaz>& pTaz : pTazCont->getTazs()) {
		logger->info("TAZ [" + pTaz->getId() + "]");
		for (const GAREdge const* pEdge : pTaz->getSinks()) {
			logger->info("\tSink edge: [" + pEdge->getID() + "]");
		}
		for (const GAREdge const* pEdge : pTaz->getSources()) {
			logger->info("\tSource edge: [" + pEdge->getID() + "]");
		}
		for (const GAREdge const* pEdge : pTaz->getUndefs()) {
			logger->info("\tUndefined edge: [" + pEdge->getID() + "]");
		}
	}

	logger->info("............... O/D groups ...");
	for (const shared_ptr<const GARTazGroup>& pGroup : pTazCont->getOdGroups()) {
		logger->info("Group [" + pGroup->getId() + "]");
		for (const shared_ptr<const GARTaz>& pTaz : pGroup->getTazs()) {
			logger->info("\tTaz: [" + pTaz->getId() + "]");
		}
	}
}


//................................................. Loads the TAZ data ...
int GARTazLoader::loadTazs(const std::string& tazFile) {
    using boost::property_tree::ptree;

    try {
 		// Create empty property tree object
		boost_pt::ptree tree;

		// Parse the XML into the property tree.
		boost_pt::read_xml(tazFile, tree);

		// Traverse the property tree to get the taz set
		taz_set tazs = this->parseTazs(tree);

		// Fix the TAZ data set into the container
		pTazCont->setTazs(tazs);

    } catch (boost_pt::ptree_bad_data& ex) {
    	logger->error("Bad data exception caught while loading TAZ districts from file '" + tazFile + "':" + string(ex.what()));
    	return 1;
    } catch (boost_pt::ptree_bad_path& ex) {
    	logger->error("Bad path exception caught while loading TAZ districts from file '" + tazFile + "':" + string(ex.what()));
    	return 1;
    } catch (boost_pt::ptree_error& ex) {
    	logger->error("Unknown exception caught while loading TAZ districts from file '" + tazFile + "':" + string(ex.what()));
    	return 1;
    }

    return 0;
}


//................................................. Parses the TAZ nodes from the TAZ file ...
taz_set GARTazLoader::parseTazs(const boost_pt::ptree& tree)
		throw(boost_pt::ptree_bad_data,
			  boost_pt::ptree_bad_path,
			  boost_pt::ptree_error) {
	using boost::property_tree::ptree;

	taz_set tazs;
	string idAttr;
	string edgesAttr;
	edge_set sources;
	edge_set sinks;
	edge_set undefs;

	for (const ptree::value_type& taz : tree.get_child(GARTazLoader::TAZ_ROOT_TAG)) {
		if (taz.first != GARTazLoader::TAZ_TAG) {
			continue;
		}
		// Get the TAZ id attribute
		idAttr = taz.second.get<string>(GARTazLoader::ID_ATTRIB, "");

		if (idAttr.empty()) {
			logger->warning("Not found the id attribute for taz tag [" + taz.first + "]");
			continue;
		}

		// Get the TAZ edges attribute (undefined type)
		edgesAttr = taz.second.get<string>(GARTazLoader::EDGES_ATTRIB, "");
		logger->debug("Parsing the edges attribute for TAZ [" + idAttr + "]: [" + edgesAttr + "]");
		undefs = this->parseEdgesAttribute(edgesAttr);

		// Get the source edges
		logger->debug("Parsing source edges for TAZ [" + idAttr + "]");
		sources = this->parseTazEdges(taz, GARTazLoader::TAZ_SOURCE_TAG);

		// Get the sink edges
		logger->debug("Parsing sink edges for TAZ [" + idAttr + "]");
		sinks   = this->parseTazEdges(taz, GARTazLoader::TAZ_SINK_TAG);

		// Insert the TAZ data into the TAZ set
		std::shared_ptr<const GARTaz> pTaz = std::make_shared<const GARTaz>(GARTaz(idAttr, sources, sinks, undefs));
		logger->info("Insert TAZ " + pTaz->toString());
		if (tazs.insert(pTaz).second == false) {
			logger->warning("Fail to insert existing TAZ [" + idAttr + "]");
		}

	}

	return tazs;
}


//................................................. Parses the undefined edges from the TAZ 'edges' attribute ...
edge_set GARTazLoader::parseEdgesAttribute(const std::string & edgesAttr)
		throw(boost::property_tree::ptree_bad_data,
			  boost::property_tree::ptree_bad_path,
			  boost::property_tree::ptree_error) {
    edge_set tazEdges;

	vector<string> edges = StringUtilities::split(edgesAttr, ' ');
	for (string edgeId : edges) {
		try {
			logger->debug("\tInsert edge [" + edgeId + "]");
			const GAREdge const* pEdge = dynamic_cast<GAREdge*>(this->net.getEdge(edgeId));
			tazEdges.insert(pEdge);
		} catch (std::bad_cast& ex) {
			logger->error("Fail to convert base class ROEdge* to derived class GAREdge*");
		}
	}

	return tazEdges;
}

//................................................. Parses the source/sink edges from the TAZ file ...
edge_set GARTazLoader::parseTazEdges(const boost_pt::ptree::value_type& taz,
									 const string& type)
		throw(boost_pt::ptree_bad_data,
			  boost_pt::ptree_bad_path,
			  boost_pt::ptree_error) {
    using boost::property_tree::ptree;

    edge_set tazEdges;

	for (const boost_pt::ptree::value_type& taz : taz.second) {
		if (taz.first != type) {
			continue;
		}
		// Get the TAZ identifier
		string edgeId = taz.second.get<string>(GARTazLoader::ID_ATTRIB, "");

		if (edgeId.empty()) {
			logger->warning("Not found the id attribute for taz tag [" + taz.first + "]");
			continue;
		}
		try {
			logger->debug("\tInsert edge [" + edgeId + "]");
			const GAREdge const* pEdge = dynamic_cast<GAREdge*>(this->net.getEdge(edgeId));
			tazEdges.insert(pEdge);
		} catch (std::bad_cast& ex) {
			logger->error("Fail to convert base class ROEdge* to derived class GAREdge*");
		}
	}

	return tazEdges;
}


//................................................. Loads the O/D group data ...
int GARTazLoader::loadGroups(const std::string& grpFile) {
    using boost::property_tree::ptree;

    try {
 		// Create empty property tree object
		boost_pt::ptree tree;

		// Parse the XML into the property tree.
		boost_pt::read_xml(grpFile, tree);

		// Traverse the property tree
		grp_set odgroups = this->parseODGroups(tree);

		// Fix the O/D group data set into the container
		pTazCont->setOdGroups(odgroups);

    } catch (boost_pt::ptree_bad_data& ex) {
    	logger->error("Bad data exception caught while loading groups from file '" + grpFile + "':" + string(ex.what()));
    	return 1;
    } catch (boost_pt::ptree_bad_path& ex) {
    	logger->error("Bad path exception caught while loading groups from file '" + grpFile + "':" + string(ex.what()));
    	return 1;
    } catch (boost_pt::ptree_error& ex) {
    	logger->error("Unknown exception caught while loading groups from file '" + grpFile + "':" + string(ex.what()));
    	return 1;
    }

    return 0;
}


//................................................. Parses the O/D group nodes from the OD-group file ...
grp_set GARTazLoader::parseODGroups(const boost_pt::ptree& tree)
		throw(boost_pt::ptree_bad_data,
			  boost_pt::ptree_bad_path,
			  boost_pt::ptree_error) {
	using boost::property_tree::ptree;

	grp_set odgroups;
	taz_set odtazs;

	for (const ptree::value_type& odgrp : tree.get_child(GARTazLoader::OD_GROUP_ROOT_TAG)) {
		if (odgrp.first != GARTazLoader::OD_GROUP_TAG) {
			continue;
		}
		// Get the O/D group identifier
		string grpId = odgrp.second.get<string>(GARTazLoader::ID_ATTRIB, "");

		if (grpId.empty()) {
			logger->warning("Not found the id attribute for group tag [" + odgrp.first + "]");
			continue;
		}

		logger->debug("Parsing tazs for OD group [" + grpId + "]");
		odtazs = this->parseODTazs(odgrp);

		// Insert the OD group data into the OD-group set
		std::shared_ptr<const GARTazGroup> pGroup = std::make_shared<const GARTazGroup>(GARTazGroup(grpId, odtazs));
		logger->info("Insert O/D group " + pGroup->toString());
		if (odgroups.insert(pGroup).second == false) {
			logger->warning("Fail to insert existing O/D group [" + grpId + "]");
		}
	}

	return odgroups;
}


//................................................. Parses the TAZ nodes from the OD-group file ...
taz_set GARTazLoader::parseODTazs(const boost_pt::ptree::value_type& odgroup)
		throw(boost_pt::ptree_bad_data,
			  boost_pt::ptree_bad_path,
			  boost_pt::ptree_error) {
    using boost::property_tree::ptree;

    taz_set odtazs;

	for (const boost_pt::ptree::value_type& taz : odgroup.second) {
		if (taz.first != GARTazLoader::TAZ_TAG) {
			continue;
		}
		// Get the TAZ identifier
		string tazId = taz.second.get<string>(GARTazLoader::ID_ATTRIB, "");

		if (tazId.empty()) {
			logger->warning("Not found the id attribute for taz tag [" + taz.first + "]");
			continue;
		}

		std::shared_ptr<const GARTaz> pTaz = pTazCont->getTaz(tazId);
		if (pTaz == nullptr) {
			continue;
		}

		logger->debug("\tInsert TAZ [" + pTaz->getId() + "]");
		if (odtazs.insert(pTaz).second == false) {
			logger->warning("Trying to insert existing taz [" + tazId + "]");
		}
	}

	return odtazs;
}

} /* namespace gar */
