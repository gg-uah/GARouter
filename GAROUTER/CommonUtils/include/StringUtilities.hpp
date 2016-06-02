#ifndef STRING_UTILITIES_H
#define STRING_UTILITIES_H

#include <StringUtilsException.hpp>

#include <iostream>
#include <sstream>
#include <string>
#include <vector>
#include <set>
#include <boost/tr1/unordered_set.hpp>
#include <boost/tr1/unordered_map.hpp>
#include <boost/numeric/ublas/matrix.hpp>


namespace common {

/**
 * This class provides several common utilities for managing strings.
 */
class StringUtilities {

public:
    
    /**
     * Method to remove white spaces in the left side of the string
     *
     * @param  oldString string with white spaces 
     * @return the string passed as argument to the method without white spaces in the left side
     */
    static std::string lTrim(const std::string & oldString);
     
    /**
     * Method to remove white spaces in the rigth side of the string
     *
     * @param  oldString string with white spaces
     * @return the string passed as argument to the method without white spaces in the rigth side     
     */
    static std::string rTrim(const std::string & oldString);
     
    /**
     * Method to remove white spaces in the rigth side and the left side of the string
     *
     * @param  oldString string with white spaces
     * @return the string passed as argument to the method without white spaces 
     *         in the rigth side and the left side
     */
    static std::string trim(const std::string & oldString);

    /**
     * Template method that converts a value of any type 'T' to a string.
     * @param value	A value of type 'T'.
     * @return		A string containing the value of type 'T'.
     */
    template <typename T>
    		static std::string toString(const T& value);

    /** 
     * Method to convert a value of type integer to string
     *
     * @param  value a variable of integer type
     * @return the value of the param converted to string type
     */
    static std::string toString(int value);
      
    /** 
     * Method to convert a unsigned integer type value to string
     *
     * @param value  A unsigned integer value
     * @return 		 The unsigned integer value converted to a string
     */
    static std::string toString(unsigned int value);

    /**
     * Method to convert a value of type float to string
     *
     * @param  value a variable of float type
     * @return the value of the param converted to string type
     */
    static std::string toString(float value);
      
    /** 
     * Method to convert a value of type double to string
     *
     * @param  value a variable of double type
     * @return the value of the param converted to string type
     */
    static std::string toString(double value);
        
    /** 
     * Method to convert a value of type long to string
     *
     * @param  value a variable of long type
     * @return the value of the param converted to string type
     */
    static std::string toString(long value);
     
    /** 
     * Method to convert a value of type *char to string
     *
     * @param  value a variable of *char type
     * @return the value of the param converted to string type
     */
    static std::string toString(const char * value);
    
    /**
     * Method to convert a boolean value to string
     *
     * @param value Value of boolean type
     * @return      "true" if value is true, "false" otherwise
     */
    static std::string toString(const bool & value);

    /**
     * @brief Converts the vector data into a string.
     * @param vec	A vector of elements of any type 'T'.
     * @return		A string containing the data within the vector.
     */
    template <typename T>
    		static std::string toString(const std::vector<T>& vec);

    /**
     * Method to return a string containing the elements in a vector of strings
     *
     * @param vstr  a vector containing strings
     * @return      a string containing all the elements in a vector of strings
     */
    static std::string toString(const std::vector<std::string> & vstr);

    /**
     * Method to return a string containing the elements in a vector of integers
     *
     * @param vstr  a vector containing integers
     * @return      a string containing all the elements in a vector of integers
     */
    static std::string toString(const std::vector<int> & vstr);

    /**
     * Method to return a string containing the elements in a set of strings
     *
     * @param aset  a set containing strings
     * @return      a string containing all the elements in a set of strings
     */
    static std::string toString(const std::set<std::string> & aset);

    /**
     * Method to return a string containing the elements in a set of integers
     *
     * @param aset  a set containing integers
     * @return      a string containing all the elements in a set of integers
     */
    static std::string toString(const std::set<int> & aset);

    /**
     * Method to return a string containing the elements in an unordered set of strings
     *
     * @param aset  an unordered set containing strings
     * @return      a string containing all the elements in an unordered set of strings
     */
    static std::string toString(const std::tr1::unordered_set<std::string> & aset);

    /**
     * Method to return a string containing the elements in an unordered set of integers
     *
     * @param aset  an unordered set containing integers
     * @return      a string containing all the elements in an unordered set of integers
     */
    static std::string toString(const std::tr1::unordered_set<int> & aset);

    /**
     * Method to return a string containing the pair elements in an unordered map with string key and integer value
     *
     * @param amap  An unordered map containing pairs of strings and integers
     * @return      A string containing all the pairs of strings and integers in an unordered map
     */
    static std::string toString(const std::tr1::unordered_map<std::string,int> & amap);

    /**
     * Template method that returns a string reporting the content of a two-dimensional matrix.
     * @param matrx	The two-dimensional matrix.
     * @return		A string containing the elements within a two-dimensional matrix.
     */
    template <typename T>
    static std::string toString(const boost::numeric::ublas::matrix<T>& matrx);

    /**
     * Method to convert a string to LowerCase
     * 
     * @param  oldString a string variable
     * @return the string passed as argument convert to lower case
     */
    static std::string toLower(const std::string & oldString);
    
    /**
     * Method to convert a string to UpperCase
     * 
     * @param  oldString a string variable
     * @return the string passed as argument convert to upper case
     */
    static std::string toUpper(const std::string & oldString);
       
    /**
     * Method to convert a string into an integer value
     *
     * @param str   string to be converted to integer type
     * @return      the integer value corresponding to the string parameter
     */
    static int toInt(const std::string & str) throw (StringUtilsException);

    /**
     * Method to convert a string into a long value
     *
     * @param str   string to be converted to long type
     * @return      the long value corresponding to the string parameter
     */
    static long toLong(const std::string & str) throw (StringUtilsException);

    /**
     * Method to convert a string into a float value
     *
     * @param str   string to be converted to float type
     * @return      the float value corresponding to the string parameter
     */
    static float toFloat(const std::string & str) throw (StringUtilsException);

    /**
     * Method to convert a string into a double value
     *
     * @param str   string to be converted to double type
     * @return      the double value corresponding to the string parameter
     */
    static double toDouble(const std::string & str) throw (StringUtilsException);

    /**
     * Method to remove characters at the beginning of a string
     *
     * @param  oldString a string variable
     * @param  charset   set of characters to be removed at the beginning of the string
     * @return the string passed as argument without beginning character found
     */
    static std::string removeAtBegin(const std::string & oldString, const std::string & charset);

    /**
     * Method to remove characters at the beginning of a string
     *
     * @param  oldString a string variable
     * @param  charset   set of characters to be removed at the end of the string
     * @return the string passed as argument without ending character found
     */
    static std::string removeAtEnd(const std::string & oldString, const std::string & charset);

    /**
      * Method to remove characters at the end of a string
      *
      * @param  oldString a string variable
      * @param  charset   set of characters to be removed at both endings of the string
      * @return the string passed as argument without characters found at beginning and ending
      */
    static std::string removeEndings(const std::string & oldString, const std::string & charset);

    /**
     * Checks whether a string starts with a pattern.
     * @param str		The string to check.
     * @param pattern	The searching pattern.
     * @return			<code>true</code> if the string starts with the pattern,
     * 					<code>false</code> otherwise.
     */
    static bool startsWith(const std::string& str, const std::string& pattern);

    /**
     * Checks whether a string ends with a pattern.
     * @param str		The string to check.
     * @param pattern	The searching pattern.
     * @return			<code>true</code> if the string ends with the pattern,
     * 					<code>false</code> otherwise.
     */
    static bool endsWith(const std::string& str, const std::string& pattern);

    /**
     * Splits a string by the specified delimiter.
     * @param str	The string to be splitted.
     * @param delim	The delimiter character of the string tokens.
     * @return		A vector containing the tokens in the string delimited
     * 				by the specified delimiter character.
     */
    static std::vector<std::string> split(const std::string &str, char delim);


    /**
     * Left pad a string to a given width with a specified character.
     * @param input			The input string.
     * @param num			The number of padding characters to add at the left of the string.
     * @param paddingChar	The padding character, by default whitespace character is considered.
     * @return				The left padded string with the specified character.
     */
    static std::string leftPadding(const std::string &input, const size_t num, const char& paddingChar = ' ');

};


template <typename T>
std::string StringUtilities::toString(const T& value) {
    std::ostringstream oss;
    oss.str("");
    oss << value;
    return oss.str();
}


template <typename T>
std::string StringUtilities::toString(const std::vector<T>& vec) {
    std::string str = "{";

    for (auto elem : vec) {
        str += (StringUtilities::toString<T>(elem) + ",");
    }
    if (!vec.empty()) {
        //Remove last comma
        str = StringUtilities::removeAtEnd(str, ",");
    }
    str += "}";

    return str;
}


template <typename T>
std::string StringUtilities::toString(const boost::numeric::ublas::matrix<T>& matrx) {
	std::string str = "";

	for (int i = 0; i < matrx.size1(); i++) {
		str += "(";
		for (int j = 0; j < matrx.size2(); j++) {
			str += (StringUtilities::toString<T>(matrx(i,j)) + ",");
		}
		str = StringUtilities::removeAtEnd(str, ",");
		str += "),";
	}
	if (matrx.size1() > 0  &&  matrx.size2() > 0) {
		//Remove last comma
		str = StringUtilities::removeAtEnd(str, ",");
	}

	return str;
}


} /* namespace */
#endif
