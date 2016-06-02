#ifndef __TIME_UTIL_HPP
#define __TIME_UTIL_HPP

#include <string>
#include <cstring>
#include <time.h>
#include <stdlib.h>

/**
 * This class provides several utilities using static methods to convert a time_t
 * to a date as a formatted string and vice versa
 */
namespace common {

class TimeUtilities {

public:
    /**
     * Time constants
     */
    const static unsigned int SECS_PER_MIN;  /**< Seconds per minute */

    const static unsigned int MINS_PER_HOUR; /**< Minutes per hour */

    const static unsigned int HOURS_PER_DAY; /**< Hours per day */

    const static unsigned int SECS_PER_HOUR; /**< Seconds per hour */

    const static unsigned int MINS_PER_DAY;  /**< Minutes per day */

    const static unsigned int SECS_PER_DAY;  /**< Seconds per day */
    
    /**
     * Method to get a time_t date from a formated string date
     *
     * @deprecated There is an equivalent method in DifUtilities that should be
     *             used instead of this one.
     * @param  date     string date in a specific format
     * @param  format   tell us where is each date part (YYYYMMDDhhmmss)
     * @param  gmthours hours to be added to our date
     * @return          the time_t gotten from a date
     */
     static time_t getDateFromString(const std::string & date,
                                     const std::string & format,
                                     int gmthours);
                                     
    /**
     * Method to get the day of the week, as an int, from the value
     * contained in the date.
     * 
     * @param  date string date in a specific format
     * @return      An integer value that corresponds to the day of the week. 1 is
     *              monday and 7 is sunday.
     */
    static int getWeekDay(const std::string & date);
};

} /* namespace */
#endif
