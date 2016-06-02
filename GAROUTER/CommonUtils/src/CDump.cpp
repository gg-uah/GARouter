#include <CDump.hpp>
#include <EnvironmentUtil.hpp>
#include <FileTokenizer.hpp>
#include <stdlib.h>

#ifdef _WIN32
#define _ISWIN32  true
#else
#define _ISWIN32 false
#endif

using std::string;
using std::map;
using std::pair;
using std::unique_ptr;
using std::once_flag;


namespace common {

//...................................................... Static class attributes ...
const long CDump::__defaultMaxLines(5000);

string CDump::__configFile;
unique_ptr<CDump> CDump::__instance;
once_flag CDump::__onceFlag;


//...................................................... singleton ...
CDump * CDump::GetInstance(const string & configFile) {
	// Replace the instance exactly once, even if called from several threads
	auto newdump = [&] () { __instance.reset(new CDump(configFile)); };
	std::call_once(__onceFlag, newdump);

	// Return a pointer to the instance managed object (CDump)
	return __instance.get();
}



//...................................................... setter methods ...
void CDump::setDump(const string & dumpKey) {
    currentDumpKey=dumpKey;
}

void CDump::setEvent(const string & dumpLine) {
    currentDumpLine=dumpLine;
}



//...................................................... dump ...
void CDump::dump() {
    dumpInstant(currentDumpKey, currentDumpLine);
}



//...................................................... dumpInstant ...
void CDump::dumpInstant(const string & dumpKey, const string & dumpLine) {
    map<string, CDrain>::iterator itr;
    
    //Find the key
    itr = dumpConfig.find(dumpKey);
    
    //If it is found, dump to its CDrain associated object
    if (itr!=dumpConfig.end()) {
        itr->second.addLine(dumpLine);
    }
    else {
        printf("\nDump::dumpInstant: Error trying to dump line <%s> into dump <%s>.", dumpLine.c_str(), dumpKey.c_str());
    }    
}



//...................................................... finishCurrentFiles ...
void CDump::finishCurrentFiles() {
    map<string, CDrain>::iterator itr;

    for(itr=dumpConfig.begin();itr!=dumpConfig.end();itr++) {
        itr->second.finishCurrentFile();    
    }
}



//...................................................... finishConfigFile ...
void CDump::finishFile(const string & dumpKey) {
    map<string, CDrain>::iterator found;
    
    found = dumpConfig.find(dumpKey);
    if (found != dumpConfig.end()) {
        found->second.finishCurrentFile();    
    }
}



//...................................................... constructor and destructor ...
CDump::CDump(const string & configFile) {
	__configFile = EnvironmentUtil::transformString(configFile, _ISWIN32);

    //We read the dumping configuration from the file.
    getConfiguration(__configFile);
}

CDump::~CDump() {
    //nothing to do, destructor of each CDrain object implicitly called
}



//...................................................... getConfiguration ...
void CDump::getConfiguration(const string & configFile) {
    FileTokenizer * ftkn;        //pointer to an FileTokenizer class
    string token;                   //token extracted from the file
    bool parseMore;                 //true while we may parse more tokens
    int maxDumpLines;               //maximum number of lines of the dump file                   
    
    //Variables to configure the dump
    string dumpName;                //Name of the configuration
    string dumpDir;                 //Path
    string dumpPrefix;              //Prefix
    string dumpSuffix;              //Suffix
    
    //Assign the pointer to the file tokenizer
    ftkn = new FileTokenizer();
        
    //Tokenize configuration file
    ftkn->setFileName(configFile);
    ftkn->setSeparators(" \t");
    ftkn->tokenize();
    
    //Parse tokenized file
    parseMore = true;    
    while (parseMore) {
        
        //[1] Read the current configuration from the input file        
        //DUMP
        if (parseMore) {
            token = ftkn->getNextToken();
            if (token!="DUMP") {
                printf("\nDump::getConfiguration error: DUMP expected but <%s> found\n", token.c_str());
                parseMore = false;
            }
        }
        //DUMP value        
        if (parseMore) {
            dumpName = ftkn->getNextToken();
        }
        
        //DIR
        if (parseMore) {
            token = ftkn->getNextToken();
            if (token!="DIR") {
                printf("\nDump::getConfiguration error: DIR expected but <%s> found\n", token.c_str());
                parseMore = false;
            }
        }
        //DIR value
        if (parseMore) {
            dumpDir = ftkn->getNextToken();
        }
        
        //PREFIX
        if (parseMore) {
             token = ftkn->getNextToken();
             if (token!="PREFIX") {
                printf("\nDump::getConfiguration error: PREFIX expected but <%s> found\n", token.c_str());
                parseMore = false;                
             }
        }
        //PREFIX value
        if (parseMore) {
            dumpPrefix = ftkn->getNextToken();
        }
        
        //SUFFIX
        if (parseMore) {
             token = ftkn->getNextToken();
             if (token!="SUFFIX") {
                printf("\nDump::getConfiguration error: SUFFIX expected but <%s> found\n", token.c_str());
                parseMore = false;                
             }
        }
        //SUFFIX value
        if (parseMore) {
            dumpSuffix = ftkn->getNextToken();
        }
        
        //MAX_LINES. This token is optional but, if present, must have an associated value
        if (parseMore) {
            token = ftkn->getNextToken();
            if (token!="MAX_LINES") {
                //Go back and set the number of lines to __defaultMaxLines
                token = ftkn->getPreviousToken();    
                maxDumpLines = __defaultMaxLines;
            } else {
                //Read next token and set the maximum number of lines to the value read
                token = ftkn->getNextToken();
                maxDumpLines = atoi(token.c_str());
                
                //If the number of lines equals 0, initialize to the default value
                if (maxDumpLines == 0) {
                    maxDumpLines = __defaultMaxLines;
                }                
            }
        }
        
        //END_DUMP
        if (parseMore) {
            token = ftkn->getNextToken();
            if (token!="END_DUMP") {
                printf("\nDump::getConfiguration error: END_DUMP expected but <%s> found\n", token.c_str());
                parseMore = false;
            }
        }
        
        //[2] Use configuration just read to add a new CDrain object
        if (parseMore) {
            dumpConfig.insert(pair<string, CDrain>(dumpName, CDrain(dumpDir,
                                                                    dumpPrefix,
                                                                    dumpSuffix,
                                                                    maxDumpLines)));
        
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

} /* namespace common */
