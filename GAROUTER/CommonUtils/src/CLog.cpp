#include <EnvironmentUtil.hpp>
#include <stdlib.h>
#include <iostream>
#include <sstream>
#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/date_time/posix_time/posix_time_io.hpp>
#include <CDump.hpp>
#include <CLog.hpp>
#include <FileTokenizer.hpp>

#ifdef _WIN32
#define _ISWIN32  true
#else
#define _ISWIN32 false
#endif

using std::string;
using std::map;
using std::pair;
using std::cout;
using std::endl;
using std::unique_ptr;
using std::once_flag;
using std::ostringstream;
using boost::posix_time::ptime;
using boost::posix_time::time_facet;


namespace common {

//...................................................... static class atributes ...
string CLog::__logConfig;
string CLog::__dumpConfig;

unique_ptr<CLog> CLog::__instance;
once_flag CLog::__onceFlag;


//...................................................... get the singleton ...
CLog * CLog::GetInstance(const string & logConfig,
						 const string & dumpConfig) {
	// Replace the instance exactly once, even if called from several threads
	auto newlog = [&] () { __instance.reset(new CLog(logConfig, dumpConfig)); };
	std::call_once(__onceFlag, newlog);

	// Return a pointer to the instance managed object (CLog)
	return __instance.get();
}



//...................................................... logging methods ...
void CLog::debug(const string & logName, const string & message) {
    logMessage(LOG_DEBUG_LEVEL, logName, message);
}


void CLog::info(const string & logName, const string & message) {
    logMessage(LOG_INFO_LEVEL, logName, message);
}


void CLog::warning(const string & logName, const string & message) {
    logMessage(LOG_WARNING_LEVEL, logName, message);
}


void CLog::error(const string & logName, const string & message) {
    logMessage(LOG_ERROR_LEVEL, logName, message);
}


void CLog::fatal(const string & logName, const string & message) {
    logMessage(LOG_FATAL_LEVEL, logName, message);
}



//...................................................... constructor and destructor ...
CLog::CLog(const string & logConfig,
		   const string & dumpConfig) {
	// Resolve the log and dump configuration path names
	__logConfig  = EnvironmentUtil::transformString(logConfig, _ISWIN32);
	__dumpConfig = EnvironmentUtil::transformString(dumpConfig, _ISWIN32);

    //Read the configuration file
    getConfiguration(__logConfig);
}

CLog::~CLog() {
    //Destructor is empty
}



//...................................................... getConfiguration ...
void CLog::getConfiguration(const string & configFile) {
    FileTokenizer * ftkn;            //pointer to an FileTokenizer class
    string token;                       //token extracted from the file
    bool parseMore;                     //true while we have to parse more tokens
    int logLevel;                   	//level of debugging

    //Variables to configure the log
    string logName;                     //Name of the log
    string level;                       //Level of the log. Could be DEBUG|INFO|WARNING|ERROR|FATAL
    string dump;                        //Name of the dump

    //Assign the pointer to the file tokenizer
    ftkn = new FileTokenizer();

    //Tokenize configuration file
    ftkn->setFileName(configFile);
    ftkn->setSeparators(" \t");
    ftkn->tokenize();

    //Parse tokenized file
    parseMore = true;
    while (parseMore) {

        //[1] Read the current configuration file from the input
        //LOG
        if (parseMore) {
            token = ftkn->getNextToken();
            if (token != "LOG") {
                cout << endl << "CLog::getConfiguration error: LOG expected but [" << token << "] found";
                parseMore = false;
            }
        }
        //LOG value
        if (parseMore) {
            logName = ftkn->getNextToken();
        }


        //LEVEL
        if (parseMore) {
            token = ftkn->getNextToken();
            if (token != "LEVEL") {
                cout << endl << "CLog::getConfiguration error: LEVEL expected but [" << token << "] found";
                parseMore = false;
            }
        }
        //LEVEL value
        if (parseMore) {
            level = ftkn->getNextToken();

            //Check that the level is one of these tokens: DEBUG|INFO|WARNING|ERROR|FATAL
            if ((level != "DEBUG") && (level != "INFO" ) && (level != "WARNING") && (level != "ERROR") && (level != "FATAL")) {
                cout << endl << "CLog::getConfiguration error: DEBUG|INFO|WARNING|ERROR|FATAL expected but [" << token << "] found";
                parseMore = false;
            }
        }


        //DUMP
        if (parseMore) {
            token = ftkn->getNextToken();
            if (token != "DUMP") {
                cout << endl << "CLog::getConfiguration error: DUMP expected but [" << token << "] found";
                parseMore = false;
            }
        }
        //DUMP value
        if (parseMore) {
            dump = ftkn->getNextToken();
        }


        //END_LOG
        if (parseMore) {
            token = ftkn->getNextToken();
            if (token != "END_LOG") {
                cout << endl << "CLog::getConfiguration error: END_LOG expected but [" << token << "] found";
                parseMore = false;
            }
        }

        //[2] Use configuration just read to add a new logging entry
        if (parseMore) {

            //Assign the log level as an integer
            if (level == "DEBUG")   logLevel = LOG_DEBUG_LEVEL;
            if (level == "INFO")    logLevel = LOG_INFO_LEVEL;
            if (level == "WARNING") logLevel = LOG_WARNING_LEVEL;
            if (level == "ERROR")   logLevel = LOG_ERROR_LEVEL;
            if (level == "FATAL")   logLevel = LOG_FATAL_LEVEL;

            logLevels.insert(pair<string, int>(logName, logLevel));
            dumpNames.insert(pair<string, string>(logName, dump));

            //[3] Check if there are more tokens or not
            token = ftkn->getNextToken();
            if (token == "")
                //End loop
                parseMore = false;
            else
                ftkn->getPreviousToken();
        }
    }

    //Release allocated memory
    delete ftkn;

}



//...................................................... logMessage ...
void CLog::logMessage(int level, const string & logName, string message) {
    map<string, int>::iterator    itrLevel; //To find the level defined for the log
    map<string, string>::iterator itrDump;  //To find the dump associated with the log
    int currentLevel;                       //Current level of trace defined
    string notification;                    //Time and severity text
    string fullMessage;                     //Full message

    time_facet * facet;						// Facet for date-time format

    ostringstream oss;                      //Output string stream to convert the date and time


    //Find the level defined for the current log
    itrLevel = logLevels.find(logName);
    if (itrLevel == logLevels.end()) {
        cout << endl << "CLog::logMessage: no log level defined for " << logName << ". Check log configuration file.";
        return;
    }

    //Get the level for the logName
    currentLevel = itrLevel->second;

    if (currentLevel <= level) {
        //Build the notification line
        notification = "";
        switch(level) {
            case LOG_DEBUG_LEVEL  :notification.append("[DEBUG|");break;
            case LOG_INFO_LEVEL   :notification.append("[INFO |");break;
            case LOG_WARNING_LEVEL:notification.append("[WARN |");break;
            case LOG_ERROR_LEVEL  :notification.append("[ERROR|");break;
            case LOG_FATAL_LEVEL  :notification.append("[FATAL|");break;
        }
        
        //Append the date-time the log is generated
        // Set a facet format for date-time in microseconds precision and imbue to output stream
        facet = new time_facet("%Y%m%d %H%M%S.%f");
        oss.imbue(std::locale(oss.getloc(), facet));
        
        // Append local time in microseconds precision to output string in the facet format
        oss << boost::posix_time::microsec_clock::local_time() << "]: ";

        notification.append(oss.str());        
                
        //Remove final EOL to the message
        if (message.size()>0) {
            if (message[message.size()-1] == '\n')
                message = message.substr(0, message.size()-1);
        }
            
        //Build final string
        fullMessage = notification + message;    
            
        //Find the dump
        itrDump = dumpNames.find(logName);

        //If the second value is SCREEN, do not call CDump
        if (itrDump == dumpNames.end()) {
            cout << endl << "CLog::logMessage: no dump defined for [" << logName << "]";
        }
        else if (itrDump->second == "SCREEN") {
            cout << fullMessage << endl;
        }
        else {
            //Dump fullMessage
            CDump::GetInstance(__dumpConfig)->dumpInstant(itrDump->second, fullMessage);
        }
    }
}

} /* namespace common */
