#include <CLogger.hpp>
#include <iostream>

using std::string;

namespace common {

//...................................................... constructor ...
CLogger::CLogger(const string & logId,
				 const string & logConfig,
				 const string & dumpConfig) {

    //Initialize CLog singleton
    log = CLog::GetInstance(logConfig, dumpConfig);

    //Assign the parameter log name
    logName = logId;
}


//...................................................... destructor ...
CLogger::~CLogger() {
    //Nothing to do in the destructor
}



//...................................................... logging methods ...
void CLogger::debug(const string & message) {
    log->debug(logName, message);
}

void CLogger::info(const string & message) {
    log->info(logName, message);
}

void CLogger::warning(const string & message) {
    log->warning(logName, message);
}

void CLogger::error(const string & message) {
    log->error(logName, message);
}

void CLogger::fatal(const string & message) {
    log->fatal(logName, message);
}

} /* namespace common */
