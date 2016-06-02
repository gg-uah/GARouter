#include <StringUtilities.hpp>
#include <iostream>
#include <sstream>
#include <cctype>
#include <boost/lexical_cast.hpp>
#include <boost/algorithm/string/predicate.hpp>


using std::string;
using std::ostringstream;
using boost::lexical_cast;
using boost::bad_lexical_cast;
using std::tr1::unordered_set;
using std::tr1::unordered_map;

namespace common {

string StringUtilities::lTrim(const string & oldString) {
    string aux = oldString;
    aux.erase(0, aux.find_first_not_of(" \n\t"));
    return aux;
}

string StringUtilities::rTrim(const string & oldString) {
    string aux = oldString;
    aux.erase(aux.find_last_not_of(" \n\t") + 1);
    return aux;
}

string StringUtilities::trim(const string & oldString){
    string aux = oldString;
    aux.erase(0, aux.find_first_not_of(" \n\t"));
    aux.erase(aux.find_last_not_of(" \n\t") + 1);
    if ((aux.find_first_not_of(" \n\t") == aux.find_last_not_of(" \n\t")) 
        && (aux.size() != 1)) {
    	aux = "";
    }
    return aux;
}

string StringUtilities::toString(int value){
     ostringstream oss;
     oss.str("");
     oss << value;
     return oss.str();
}

string StringUtilities::toString(unsigned int value) {
    ostringstream oss;
    oss.str("");
    oss << value;
    return oss.str();
}

string StringUtilities::toString(float value){
     ostringstream oss;
     oss.str("");
     oss << value;
     return oss.str();
}

string StringUtilities::toString(double value){
     ostringstream oss;
     oss.str("");
     oss << value;
     return oss.str();
}

string StringUtilities::toString(long value){
     ostringstream oss;
     oss.str("");
     oss << value;
     return oss.str();
}

string StringUtilities::toString(const char * value){
     ostringstream oss;
     oss.str("");
     oss << value;
     return oss.str();
}

string StringUtilities::toString(const bool & value) {
    if (value == true) {
        return "true";
    }
    return "false";
}

string StringUtilities::toString(const std::vector<string> & vstr) {
    string str = "{";
    std::vector<string>::const_iterator it;

    for(it = vstr.begin(); it != vstr.end(); it++) {
        str = str + *it + ",";
    }

    if(it != vstr.begin()) {
        //Erase last comma
        str = str.substr(0, str.size()-1);
    }

    str = str + "}";

    return str;
}

string StringUtilities::toString(const std::vector<int> & vstr) {
    string str = "{";
    std::vector<int>::const_iterator it;

    for(it = vstr.begin(); it != vstr.end(); it++) {
        str = str + StringUtilities::toString(*it) + ",";
    }

    if(it != vstr.begin()) {
        //Erase last comma
        str = str.substr(0, str.size()-1);
    }

    str = str + "}";

    return str;
}

string StringUtilities::toString(const std::set<string> & aset) {
    string str = "{";
    std::set<string>::const_iterator it;

    for(it = aset.begin(); it != aset.end(); it++) {
        str = str + *it + ",";
    }

    if(it != aset.begin()) {
        //Erase last comma
        str = str.substr(0, str.size()-1);
    }

    str = str + "}";

    return str;
}

string StringUtilities::toString(const std::set<int> & aset) {
    string str = "{";
    std::set<int>::const_iterator it;

    for(it = aset.begin(); it != aset.end(); it++) {
        str = str + StringUtilities::toString(*it) + ",";
    }

    if(it != aset.begin()) {
        //Erase last comma
        str = str.substr(0, str.size()-1);
    }

    str = str + "}";

    return str;
}

string StringUtilities::toString(const unordered_set<string> & aset) {
    string str = "{";
    unordered_set<string>::const_iterator it;

    for(it = aset.begin(); it != aset.end(); it++) {
        str = str + *it + ",";
    }

    if(it != aset.begin()) {
        //Erase last comma
        str = str.substr(0, str.size()-1);
    }

    str = str + "}";

    return str;
}

string StringUtilities::toString(const unordered_set<int> & aset) {
    string str = "{";
    unordered_set<int>::const_iterator it;

    for(it = aset.begin(); it != aset.end(); it++) {
        str = str + StringUtilities::toString(*it) + ",";
    }

    if(it != aset.begin()) {
        //Erase last comma
        str = str.substr(0, str.size()-1);
    }

    str = str + "}";

    return str;
}

string StringUtilities::toString(const unordered_map<string,int> & amap) {
    string str = "{";
    unordered_map<string,int>::const_iterator it;

    for (it = amap.begin(); it != amap.end(); it++) {
        str = str + "(" + it->first + "#" + StringUtilities::toString(it->second) + ")/";
    }

    if (!amap.empty()) {
        //Erase last slash character
        str = str.substr(0, str.size() - 1);
    }

    str = str + "}";

    return str;
}

string StringUtilities::toLower(const string & oldString) {
	string aux = oldString;
	for (string::iterator p = aux.begin(); p != aux.end(); ++p) {
		*p = tolower(*p);
	}
	return aux;
}

string StringUtilities::toUpper(const string & oldString) {
	string aux = oldString;
	for (string::iterator p = aux.begin(); p != aux.end(); ++p) {
		*p = toupper(*p);
	}
	return aux;
}

int StringUtilities::toInt(const string & str)
throw (StringUtilsException) {
    int iValue;

    try {
        iValue = boost::lexical_cast<int>(str);

    } catch( boost::bad_lexical_cast const & ex) {
        throw StringUtilsException(STRUTIL_BAD_INT_CAST);
    }

    return iValue;
}

long StringUtilities::toLong(const string & str)
throw (StringUtilsException){
    int lValue;

    try {
        lValue = boost::lexical_cast<long>(str);

    } catch( boost::bad_lexical_cast const & ex) {
        throw StringUtilsException(STRUTIL_BAD_LONG_CAST);
    }

    return lValue;
}

float StringUtilities::toFloat(const string & str)
throw (StringUtilsException){
    float fValue;

    try {
        fValue = boost::lexical_cast<float>(str);

    } catch( boost::bad_lexical_cast const & ex) {
        throw StringUtilsException(STRUTIL_BAD_FLOAT_CAST);
    }

    return fValue;
}

double StringUtilities::toDouble(const string & str)
throw (StringUtilsException){
    double dValue;

    try {
        dValue = boost::lexical_cast<double>(str);

    } catch( boost::bad_lexical_cast const & ex) {
        throw StringUtilsException(STRUTIL_BAD_DOUBLE_CAST);
    }

    return dValue;
}

string StringUtilities::removeAtBegin(const std::string & oldString, const std::string & charset) {
    size_t pos;

    pos = oldString.find_first_of(charset);

    //If found at the beginning, return substring after found character position
    if(pos == 0) {
        return oldString.substr(pos + 1);
    }

    return oldString;
}

string StringUtilities::removeAtEnd(const std::string & oldString, const std::string & charset) {
    size_t pos;

    pos = oldString.find_last_of(charset);

    //If found at the end, return substring until found character position
    if(pos == oldString.length() - 1) {
        return oldString.substr(0, pos);
    }

    return oldString;
}

string StringUtilities::removeEndings(const std::string & oldString, const std::string & charset) {
    string str;

    str = removeAtBegin(oldString, charset);
    return removeAtEnd(str, charset);
}


bool StringUtilities::startsWith(const std::string& str, const std::string& pattern) {
	return boost::starts_with(str, pattern);
}


bool StringUtilities::endsWith(const std::string& str, const std::string& pattern) {
	return boost::ends_with(str, pattern);
}


std::vector<std::string> StringUtilities::split(const std::string &str, char delim) {
    std::vector<std::string> tokens;
    std::string tkn;

    std::stringstream ss(StringUtilities::trim(str));
    while (std::getline(ss, tkn, delim)) {
    	if (!tkn.empty()) {
    		tokens.push_back(tkn);
    	}
    }
    return tokens;
}


string StringUtilities::leftPadding(const std::string &input, const size_t num, const char& paddingChar) {
    string str = input;

	if (num > input.size()) {
		str.insert(0, num-str.size(), paddingChar);
	}

	return str;
}


} /* namespace common */
