/*
 * FileUtils.hpp
 *
 *  Created on: Apr 21, 2015
 *      Author: ctomas
 */

#ifndef FILEUTILS_HPP_
#define FILEUTILS_HPP_

#include <iostream>
#include <boost/filesystem.hpp>

namespace common {

class FileUtils {

public:

	/**
	 * Deleted default constructor.
	 * Prevents to make an instance of this class.
	 */
	FileUtils(void) = delete;

	/**
	 * Deleted virtual destructor.
	 * Prevents to destroy this class.
	 */
	virtual ~FileUtils(void) = delete;

	/**
	 * Check if the file referenced by filename exists .
	 * @param filename	The path file name
	 * @return	<code>true</code> if filename refers to an existing file,
	 * 			<code>false</code> otherwise
	 */
	static bool isFile(const std::string& filename);

	/**
	 * Search file in directory and subdirectory.
	 * @param dir		Search in this directory.
	 * @param filename	Search for this file name.
	 * @param found		Place path file name if found.
	 * @return			<code>true</code> if the file is found,
	 * 					<code>false</code> otherwise.
	 */
	static bool findFile(const std::string& dir,
			 	 	   	 const std::string& filename,
						 std::string& found);

};

} /* namespace common */

#endif /* FILEUTILS_HPP_ */
