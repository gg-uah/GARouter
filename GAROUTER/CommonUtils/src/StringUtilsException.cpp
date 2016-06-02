#include <StringUtilsException.hpp>

using std::string;

namespace common {

//...................................................... constructor and destructor ...
StringUtilsException::StringUtilsException() {
    type = STRUTIL_UNSPECIFIED;
}

StringUtilsException::StringUtilsException(StrUtilErrorType typeException) {
    type = typeException;
}


//...................................................... getter methods ...
int StringUtilsException::getType() {
    return type;
}

string StringUtilsException::getReason() {
    string reason;       //The reason that will be returned
    
    switch (type){
        case STRUTIL_UNSPECIFIED:
            reason = "Unknown reason";
            break;
        case STRUTIL_BAD_INT_CAST:
            reason = "Bad conversion from string characters to integer value";
            break;
        case STRUTIL_BAD_LONG_CAST:
            reason = "Bad conversion from string characters to long integer value";
            break;
        case STRUTIL_BAD_FLOAT_CAST:
            reason = "Bad conversion from string characters to float value";
            break;
        case STRUTIL_BAD_DOUBLE_CAST:
            reason = "Bad conversion from string characters to double value";
            break;
    }
    
    return reason;
}

} /* namespace common */
