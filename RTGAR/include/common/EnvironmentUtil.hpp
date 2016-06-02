#ifndef __ENVIRONMENT_UTIL
#define __ENVIRONMENT_UTIL

#include <stdlib.h>
#include <string>

namespace common {

/**
 * This class provides a method that parses a string and makes a replacement
 * of the environment variables that may be found in the string with the value
 * those variables have in the environment. The class provides a single static
 * public method to do this substitution.
 * 
 * For the substitution to work, the environment variables must be defined using
 * parenthesees in the string. For example, if the value of RTFMS is "/detection",
 * then "$(RTFMS)/rtfit/logs" will be replaced by "/detection/rtfit/logs". 
 * 
 * The string may contain more than an environment variable.
 */
class EnvironmentUtil {

public:  
    
    /**
     * Static method that does the substitution.
     * 
     * @param value The value in which we will perform substitution of the
     *              environment variables
     * @param path2Dos If true, converts &#47; into &#92;.
     * @return      The value of the string, transformed.
     */
    static std::string transformString(const std::string & value, bool path2Dos = false);
};

} /* namespace */
#endif
