/*
 * FileUtils.cpp
 *
 *  Created on: Apr 21, 2015
 *      Author: ctomas
 */

#include <FileUtils.hpp>
#include <algorithm>
#include <sys/stat.h>

using std::string;
//using namespace boost::filesystem;
using boost::filesystem::path;
using boost::filesystem::recursive_directory_iterator;
using boost::filesystem::directory_entry;

namespace common {

//...................................................... Checks if a file exists ...
bool FileUtils::isFile(const string& filename) {
	struct stat buffer;

	return (stat(filename.c_str(), &buffer) == 0);
}


//...................................................... Find a file in a directory ...
bool FileUtils::findFile(const string& dir,
						 const string& filename,
						 string& found) {
	string baseDir = "";

	try {
		// Is 'dir' the name of a directory or an environment variable?
		if (boost::filesystem::exists(boost::filesystem::path(dir))) {
			baseDir = dir;
		} else {
			//Perhaps dir contains the name of an environment variable
			if (std::getenv(dir.c_str()) == nullptr) {
				std::cerr << "System environment variable \'RTGAR\' is not defined" << std::endl;
				return false;
			}
			baseDir = getenv(dir.c_str());
		}
	} catch(boost::filesystem::filesystem_error & ex) {
		//Perhaps dir contains the name of an environment variable
		if (std::getenv(dir.c_str()) == nullptr) {
			std::cerr << "System environment variable \'RTGAR\' is not defined" << std::endl;
			return false;
		}
		baseDir = getenv(dir.c_str());
	}

	path dir_path = boost::filesystem::path(baseDir);
	path filename_path = boost::filesystem::path(filename);

	const recursive_directory_iterator end;
	const auto it = std::find_if(recursive_directory_iterator(dir_path),
		  	  	  	  	  	   	 end,
								 [&filename_path] (const directory_entry& entry) {
                            	 	 return entry.path().filename() == filename_path;
  	  	  	  	  	  	  	   	 });
	if (it == end) {
		return false;
	} else {
		found = it->path().string();
		return true;
	}
}

} /* namespace common */
