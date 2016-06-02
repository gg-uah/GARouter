#include <TimeUtilities.hpp>

using std::string;

namespace common {

//...................................................... Time constants ...
const unsigned int TimeUtilities::SECS_PER_MIN  = 60;

const unsigned int TimeUtilities::MINS_PER_HOUR = 60;

const unsigned int TimeUtilities::HOURS_PER_DAY = 24;

const unsigned int TimeUtilities::SECS_PER_HOUR = TimeUtilities::MINS_PER_HOUR * TimeUtilities::SECS_PER_MIN;

const unsigned int TimeUtilities::MINS_PER_DAY  = TimeUtilities::HOURS_PER_DAY * TimeUtilities::MINS_PER_HOUR;

const unsigned int TimeUtilities::SECS_PER_DAY  = TimeUtilities::MINS_PER_DAY  * TimeUtilities::SECS_PER_MIN;


//...................................................... gets a time_t from a string ...
time_t TimeUtilities::getDateFromString (const string & date,
                                    const string & format,
                                    int gmthours) {
    bool separator;
    time_t errorValue = 0;
    string numbers = "0123456789";
    string year = "";
    string month = "";
    string day = "";
    string hh = "";
    string mm = "";
    string ss = "";
    struct tm timeDate;

    if( date.size() != format.size() ) {
        return errorValue;
    }

    //Initialize the tm structure to nulls
    std::memset(&timeDate,0,sizeof(struct tm));


    //Assign char by char to the proper variable
    for ( unsigned int i = 0; i < format.size(); i++){
        separator = false;
        switch (format[i]){
            case 'Y':
                year += date[i];
                break;
            case 'M':
                month += date[i];
                break;
            case 'D':
                day += date[i];
                break;
            case 'h':
                hh += date[i];
                break;
            case 'm':
                mm += date[i];
                break;
            case 's':
                ss += date[i];
                break;
            default:  //Separator chars
                separator = true;
                break;
        }

        if (!separator){ // If it has been assigned
            if( numbers.find_first_of( date[i] ) == string::npos){ //did it not be a number?
                return errorValue;  // It was not a number
            }
        }
    }

    //Check sizes
    if ( year.size() != 4  ||  month.size() != 2 || day.size() != 2   ||
         hh.size() > 2     ||  mm.size() > 2     || ss.size() > 2 ) {
            return errorValue;
    }

    // set the colect values to the variables of tm estructure
    timeDate.tm_year  = atoi( year.c_str() )-1900;
    timeDate.tm_mon   = atoi( month.c_str() )-1;
    timeDate.tm_mday  = atoi( day.c_str() );
    timeDate.tm_hour  = atoi( hh.c_str() ) + gmthours;
    timeDate.tm_min   = atoi( mm.c_str() );
    timeDate.tm_sec   = atoi( ss.c_str() );
    timeDate.tm_isdst = -1;

    // make the time
    time_t temp = mktime(& timeDate);

    // In summer we must add two hour to the GMT
    // In winter one hour

    return temp;
}



//...................................................... get day of the week ...
int TimeUtilities::getWeekDay(const std::string & date) {
    struct tm timeInfo;      //tm structure
    int       weekDay;       //The day of the week

    //[1] Initialize timeInfo
    timeInfo.tm_year = atoi(date.substr(0, 4).c_str()) - 1900;
    timeInfo.tm_mon  = atoi(date.substr(4, 2).c_str()) - 1;
    timeInfo.tm_mday = atoi(date.substr(6, 2).c_str());
    timeInfo.tm_hour = 0;
    timeInfo.tm_min = 0;
    timeInfo.tm_sec = 1;
    timeInfo.tm_isdst = -1;

    //[2] Call mktime
    mktime(& timeInfo);

    //[3] Get result and return it
    if (timeInfo.tm_wday == 0) {
        //sunday, return 7
        weekDay = 7;
    } else {
        //the day matches the value, return it
        weekDay = timeInfo.tm_wday;
    }
    return weekDay;
}

} /* namespace common */
