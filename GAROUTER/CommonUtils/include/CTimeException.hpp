#ifndef __ATMTIME_EXCEPTION
#define __ATMTIME_EXCEPTION

#include <string>

namespace common {

enum TimeErrorType {TIMEXC_UNSPECIFIED,
                    TIMEXC_BAD_YEAR,
                    TIMEXC_BAD_MONTH,
                    TIMEXC_BAD_DAY,
                    TIMEXC_BAD_HOURS,
                    TIMEXC_BAD_MINUTES,
                    TIMEXC_BAD_SECONDS,
                    TIMEXC_BAD_DATE_TIME};

/**
 * This is the possible exceptions of the class StringUtilities
 */
class CTimeException {

public:
    
    /**
     * Constructors of class
     */
    CTimeException();
    
    /**
     * Constructor of the class
     *
     * @param typeException Numeric value indicating the type of the exception
     */
    CTimeException(TimeErrorType typeException);
    
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
     TimeErrorType type;

};

} /* namespace */
#endif
