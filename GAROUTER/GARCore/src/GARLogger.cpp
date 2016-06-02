/*
 * GARLogger.cpp
 *
 *  Created on: Apr 21, 2015
 *      Author: ctomas
 */

#include <iostream>
#include <cstdlib>
#include <GARLogger.hpp>
#include <FileUtils.hpp>

using namespace std;
using common::CLogger;
using common::FileUtils;

namespace gar {

//................................................. Default constructor ...
GARLogger::GARLogger(void) {
	if (getenv("RTGAR") == nullptr) {
		std::cerr << "The environment variable \'RTGAR\' is not declared" << std::endl;
		this->logger = nullptr;
		return;
	}

	const string logId = "GARLog";
	const string logConfig  = string(getenv("RTGAR")) + string("/config/log_config.cfg");
	const string dumpConfig = string(getenv("RTGAR")) + string("/config/dump_config.cfg");

	if (!FileUtils::isFile(logConfig)) {
		std::cerr << "Log configuration file [" << logConfig << "] doesn't exist" << std::endl;
		this->logger = nullptr;
		return;
	}

	if (!FileUtils::isFile(dumpConfig)) {
		std::cerr << "Dump configuration file [" << dumpConfig << "] doesn't exist" << std::endl;
		this->logger = nullptr;
		return;
	}

	this->logger = new CLogger(logId, logConfig, dumpConfig);
}


//................................................. Parameterized constructor (mandatory parameters) ...
GARLogger::GARLogger(const string& logId,
				     const string& logConfig,
				     const string& dumpConfig) {
	if (!FileUtils::isFile(logConfig)) {
		std::cerr << "Log configuration file [" << logConfig << "] doesn't exist" << std::endl;
		this->logger = nullptr;
		return;
	}

	if (!FileUtils::isFile(dumpConfig)) {
		std::cerr << "Dump configuration file [" << dumpConfig << "] doesn't exist" << std::endl;
		this->logger = nullptr;
		return;
	}

	this->logger = new CLogger(logId, logConfig, dumpConfig);
}


//................................................. Virtual destructor ...
GARLogger::~GARLogger(void) {
	//Deallocate the memory block pointed by the logger
	delete this->logger;
}

//................................................. Getter methods ...
CLogger* GARLogger::getLogger(void) const {
	return this->logger;
}

} /* namespace gar */

