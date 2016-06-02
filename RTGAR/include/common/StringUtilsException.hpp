#ifndef __STRING_UTILS_EXCEPTION
#define __STRING_UTILS_EXCEPTION

#include <string>

namespace common {

enum StrUtilErrorType {STRUTIL_UNSPECIFIED,
                       STRUTIL_BAD_INT_CAST,
                       STRUTIL_BAD_LONG_CAST,
                       STRUTIL_BAD_FLOAT_CAST,
                       STRUTIL_BAD_DOUBLE_CAST};

/**
 * This is the possible exceptions of the class StringUtilities
 */
class StringUtilsException {

public:
    
    /**
     * Constructors of class
     */
    StringUtilsException();
    
    /**
     * Constructor of the class
     *
     * @param typeException Numeric value indicating the type of the exception
     */
    StringUtilsException(StrUtilErrorType typeException);
    
    /**
     * Method that return the type of exception
     *
     * @deprecated Only the getReason method should be used
     * @return the type of exception
     */
    int getType();
    
    /**
     * Method that return the reason of the exception
     *
     * @return the reason of the exception
     */
     std::string getReason();

private:
    
    /**
     * Property that indicates the type of exception.
     */
     StrUtilErrorType type;

};

} /* namespace */
#endif
