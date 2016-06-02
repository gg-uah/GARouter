#include <CDrain.hpp>
#include <EnvironmentUtil.hpp>
#include <sys/time.h>
#include <sstream> 

#ifdef _WIN32
#define _ISWIN32 true
#else
#define _ISWIN32 false
#endif

using std::string;
using std::endl;
using std::ostringstream;
using std::ofstream;
    
namespace common {

//...................................................... constructor and destructor ...
CDrain::CDrain (const string & directory,
                    const string & prefix,
                    const string & suffix,
                    int lines) {
    initialize(directory, prefix, suffix, lines);
}

CDrain::CDrain(const CDrain & fdr) {
    initialize(fdr.directory, fdr.prefix, fdr.suffix, fdr.numRecords);
}

CDrain::~CDrain() {
    if (currentRecord > 0) {
        //Close current file
        finishCurrentFile();  
    }
} 



//...................................................... addLine ...
void CDrain::addLine (const string & line) {
    
    //If the line is empty, do not insert it
    if(line.length() == 0) {
        return;
    }
    
    //If the maximum number of records is reached, close current file
    //and open a new one
    if(currentRecord >= numRecords) {
        finishCurrentFile();
        openNewFile();
    } else if (currentRecord == 0) {
        openNewFile();    
    }
    currentFile << line << endl; 
    currentRecord++;
}



//...................................................... buildFileName ...
string CDrain::buildFileName(const string & append) {
    time_t   now;         //time_t variale
    tm     * localt;      //pointer to a tm structure
    ostringstream oss;    //output string stream object to build the file name
    
    now = time(0);
    localt = localtime(& now);    
    
    oss.str("");
        
    oss.fill('0');
    
    oss << this->directory.c_str();
    
    //The "append" is added in front of the name of the file
    oss << append;
    oss << this->prefix.c_str();
    
    //Append the date
    oss.width(4); oss << 1900 + localt->tm_year;        
    oss.width(2); oss << localt->tm_mon + 1;    
    oss.width(2); oss << localt->tm_mday;    
    oss.width(2); oss << localt->tm_hour;    
    oss.width(2); oss << localt->tm_min;    
    oss.width(2); oss << localt->tm_sec;    
    
    oss.width(3);
    oss << "_" << this->currentEnd;
    
    //oss << "_" << getpid();
    oss << "_" << this->suffix.c_str();
    
    return oss.str();     
}



//...................................................... initialize ...
void CDrain::initialize(const string & directory,
                        const string & prefix,
                        const string & suffix,
                        int lines) {
    
    //set the currentRecord and currentEnd
    currentRecord = 0;
    currentEnd = 0;
    
    //set the directory
    this->directory = directory;

    if (this->directory[(this->directory).length()-1] != '/' &&
        this->directory[(this->directory).length()-1] != '\\') {
    	this->directory += "/";
    }

    this->directory = EnvironmentUtil::transformString(this->directory, _ISWIN32);
    
    //set the prefix of the file name
    this->prefix = EnvironmentUtil::transformString(prefix);
    
    //set the suffix of the file name
    this->suffix = EnvironmentUtil::transformString(suffix);
    
    //set the number of records to the number of lines
    numRecords = lines;
}



//...................................................... openNewFile ...
void CDrain::openNewFile() {

    //increase the current end
    currentEnd++;
    if(currentEnd > 999) {
        currentEnd = 1;
    }

    //build the name of the file
    currentFileName = buildFileName("temp.");
                      
    //open the current archive
    currentFile.open(currentFileName.c_str());
}



//...................................................... finishCurrentFile ...
void CDrain::finishCurrentFile() {
    string newName;
    string moveCommand;
    
    //The file is closed only if the current record is greater than 0
    if (currentRecord > 0) {
      
        //Flush content of current output buffer
        currentFile.flush();
        currentFile.close();
        
        //build the name of the new file
        newName = buildFileName("");
        
        //Rename temp file
        int result = rename(currentFileName.c_str(), newName.c_str());
        if (result == 0) {
        	std::cout << "Rename [" << currentFileName << "] to [" << newName << "]" << std::endl;
        } else {
        	std::cerr << "Fail to rename file [" << currentFileName << "] to [" << newName << "]" << std::endl;
        }
        
        //reset current record
        currentRecord = 0;    
    }
}



//...................................................... deleteCurrentFile ...
void CDrain::deleteCurrentFile() {

    //The file is closed only if the current record is greater than 0
    if (currentRecord > 0) {
      
        //Flush content of current output buffer
        currentFile.flush();
        currentFile.close();
        
        //Remove the target file        
        remove(currentFileName.c_str());
        
        //reset current record
        currentRecord = 0;    
    }        
}

} /* namespace common */
