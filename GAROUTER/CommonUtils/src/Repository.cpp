#include <EnvironmentUtil.hpp>
#include <Repository.hpp>
#include <sys/types.h>
#include <sys/stat.h>
#include <exception>
#include <iostream>

using std::string;
using std::endl;
using std::exception;
    
namespace common {

//...................................................... constructor and destructor ...
Repository::Repository(const string & path, const string & prefix, const string & suffix) {
    repoPath = EnvironmentUtil::transformString(path);

    //Force the path to end with '/'
	if (repoPath[repoPath.size()-1] != '/') {
        repoPath = repoPath + "/";
    }

#ifdef _WIN32
    repoPath = EnvironmentUtil::transformString(repoPath, true);
#endif

    //Try to open the directory
    dir = opendir(repoPath.c_str());
    if (dir == NULL) {
    	std::cerr << "Not found repository directory [" << repoPath << "]" << std::endl;
    }
    
   	//Initialize repository and current data information
	repoPrefix = EnvironmentUtil::transformString(prefix);
	repoSuffix = EnvironmentUtil::transformString(suffix);

	currentPathAndName = "";
	currentName = "";
}

Repository::~Repository() {
    //Close dir if it is not NULL
    if (dir != NULL) {
        closedir(dir);
    }
}



//...................................................... otherFile ...
bool Repository::otherFile(bool rewindDir) {
    bool     found;              //True if a file matching the pattern is found
    dirent * dp;                 //Pointer to an entry in the directory
    
    //Current file and full path are initialized to an empty string
    currentPathAndName = "";
    currentName = "";    

    if (dir == NULL) {
    	return false;
    }

    //Read the contents of the directory
    if (rewindDir) {
        rewinddir(dir);     
    }

    found = false;    
    dp = readdir(dir);

    while ((dp != NULL) && (!found)) {
        if (isCorrect(dp->d_name)) {
            //Update current full path and file name
            currentPathAndName = repoPath + dp->d_name;
            currentName = dp->d_name;

            //Exit loop
            found = true;            
        } else {
            dp = readdir(dir);
        }                
    }
    
    //Return found
    return found;
}



//...................................................... rewindDir ...
void Repository::rewindDir() {
    rewinddir(dir);    
}



//...................................................... current file info ...
string Repository::getCurrentPathAndName() {
    return currentPathAndName;
}

string Repository::getCurrentName() {
    return currentName;
}



//...................................................... repository properties ...
string Repository::getPath() {
    return repoPath;    
}

string Repository::getPrefix() {
    return repoPrefix;    
}

string Repository::getSuffix() {
    return repoSuffix;    
}



//...................................................... isCorrect ...
bool Repository::isCorrect(const string & fileName) {
    string::size_type pos;         //position of the substring searched in the fileName
    string            fullPath;    //full access path to the file
    struct stat       buf;         //to store the information of the new file
    
    /* 
     * fileName must begin with repoPrefix, and end with repoSuffix.
     * Besides, it cannot be neither "." nor "..", and it cannot be a 
     * directory itself.
     */        
              
    //[1] fileName cannot be either "." or ".."
    if ((fileName == ".")||(fileName == "..")) {
        return false;    
    }
    
    //[2] prefix must not be empty, and be in the first position
    if (repoPrefix != "") {
        pos = fileName.find(repoPrefix, 0);
        if (pos!=0) {
            return false;
        } //prefix is either not found or found at a wrong position
    } //prefix is not empty
    
    //[3] sufix must not be empty, and be in the last position
    if (repoSuffix != "") {
        pos = fileName.rfind(repoSuffix, fileName.size()-1);
        if (pos!=string::npos) {
            if ((pos + repoSuffix.size())!=fileName.size()) {
                return false;
            } 
        } else {
            return false;    
        } //suffix is found in the file name
    } //suffix is not empty
    
    //[4] the name of the file cannot be a directory itself
    fullPath = repoPath + fileName;
    if (stat(fullPath.c_str(), &buf)==0) {
        //check wether this file is itself a folder or not
        if ((buf.st_mode & S_IFMT) == S_IFDIR) {
            //This file is a directory
            return false;    
        } else {
            //All checks passed, file is correct
            return true;    
        }
    } else {
        //stat failed
        return false;    
    }
}

} /* namespace common */
