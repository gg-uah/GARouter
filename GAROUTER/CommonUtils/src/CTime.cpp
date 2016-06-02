#include <CTime.hpp>
#include <StringUtilities.hpp>
#include <iostream>
#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/date_time.hpp>

using namespace std;
using boost::posix_time::ptime;
using boost::posix_time::time_input_facet;
using boost::posix_time::time_duration;

namespace common {

const std::locale CTime::__STR_TIME_FORMATS[] = {
    std::locale(std::locale::classic(), new time_input_facet("%Y%m%d%H%M%S")),
    std::locale(std::locale::classic(), new time_input_facet("%Y-%m-%d %H:%M:%S")),
    std::locale(std::locale::classic(), new time_input_facet("%Y/%m/%d %H:%M:%S")),
    std::locale(std::locale::classic(), new time_input_facet("%d.%m.%Y %H:%M:%S")),
    std::locale(std::locale::classic(), new time_input_facet("%d/%m/%Y %H:%M:%S")),
    std::locale(std::locale::classic(), new time_input_facet("%Y-%m-%d")),
    std::locale(std::locale::classic(), new time_input_facet("%d%b%y"))};

const size_t CTime::__NUM_TIME_FORMATS = sizeof(CTime::__STR_TIME_FORMATS)/sizeof(CTime::__STR_TIME_FORMATS[0]);

//...................................................... Default constructor ...
CTime::CTime() {
    this->year    = 0;
    this->month   = 0;
    this->day     = 0;
    this->hour   = 0;
    this->minutes = 0;
    this->seconds = 0;
}


//...................................................... Integer parameterized constructor ...
CTime::CTime(const unsigned short & year,
             const unsigned short & month,
             const unsigned short & day,
             const unsigned short & hour,
             const unsigned short & minutes,
             const unsigned short & seconds)
    throw (CTimeException) {

    this->year    = year;
    this->month   = month;
    this->day     = day;
    this->hour   = hour;
    this->minutes = minutes;
    this->seconds = seconds;

    // Check for right date-time
    if(getTimet() == -1) {
        throw CTimeException(TIMEXC_BAD_DATE_TIME);
    }
}


//...................................................... String parameterized constructor ...
CTime::CTime(const string & year,
             const string & month,
             const string & day,
             const string & hour,
             const string & minutes,
             const string & seconds)
    throw (CTimeException) {

    try {
        this->year = (unsigned short)StringUtilities::toInt(year);
    } catch (StringUtilsException & ex) {
        throw CTimeException(TIMEXC_BAD_YEAR);
    }

    try {
        this->month = (unsigned short)StringUtilities::toInt(month);
    } catch (StringUtilsException & ex) {
        throw CTimeException(TIMEXC_BAD_MONTH);
    }

    try {
        this->day = (unsigned short)StringUtilities::toInt(day);
    } catch (StringUtilsException & ex) {
        throw CTimeException(TIMEXC_BAD_DAY);
    }

    try {
        this->hour = (unsigned short)StringUtilities::toInt(hour);
    } catch (StringUtilsException & ex) {
        throw CTimeException(TIMEXC_BAD_HOURS);
    }

    try {
        this->minutes = (unsigned short)StringUtilities::toInt(minutes);
    } catch (StringUtilsException & ex) {
        throw CTimeException(TIMEXC_BAD_MINUTES);
    }

    try {
        this->seconds = (unsigned short)StringUtilities::toInt(seconds);
    } catch (StringUtilsException & ex) {
        throw CTimeException(TIMEXC_BAD_SECONDS);
    }

    // Check for right date-time
    if(this->getTimet() == -1) {
        throw CTimeException(TIMEXC_BAD_DATE_TIME);
    }
}


//...................................................... String parameterized constructor ...
CTime::CTime(const string & date)
    throw(CTimeException) {

    ptime pt;
    std::tm tmtime;

    for(size_t i = 0; i < CTime::__NUM_TIME_FORMATS; i++) {
        std::istringstream iss(date);
        iss.imbue(CTime::__STR_TIME_FORMATS[i]);
        iss >> pt;
        if(pt != ptime()) {
            break;
        }
    }

    if(pt == ptime()) {
        throw CTimeException(TIMEXC_BAD_DATE_TIME);
    }

    tmtime = boost::posix_time::to_tm(pt);

    this->year    = tmtime.tm_year + 1900;
    this->month   = tmtime.tm_mon + 1;
    this->day     = tmtime.tm_mday;
    this->hour    = tmtime.tm_hour;
    this->minutes = tmtime.tm_min;
    this->seconds = tmtime.tm_sec;

    // Check for right date-time
    if(this->getTimet() == -1) {
        throw CTimeException(TIMEXC_BAD_DATE_TIME);
    }
}


//...................................................... Epoch time parameterized constructor ...
CTime::CTime(const time_t & timet)
    throw (CTimeException) {

    ptime pt;
    std::tm tmtime;

    pt = boost::posix_time::from_time_t(timet);

    tmtime = boost::posix_time::to_tm(pt);

    this->year    = tmtime.tm_year + 1900;
    this->month   = tmtime.tm_mon + 1;
    this->day     = tmtime.tm_mday;
    this->hour   = tmtime.tm_hour;
    this->minutes = tmtime.tm_min;
    this->seconds = tmtime.tm_sec;

    // Check for right date-time
    if(this->getTimet() == -1) {
        throw CTimeException(TIMEXC_BAD_DATE_TIME);
    }
}


//...................................................... Copy constructor ...
CTime::CTime(const CTime & other) {
    this->year    = other.year;
    this->month   = other.month;
    this->day     = other.day;
    this->hour   = other.hour;
    this->minutes = other.minutes;
    this->seconds = other.seconds;
}


//...................................................... Destructor ...
CTime::~CTime() {
    // Empty destructor
}


//...................................................... Get number of seconds since epoch time ...
time_t CTime::getTimet() const {
    tm when;
    ptime atime;
    ptime epoch;
    time_duration::sec_type secs;

    when.tm_year = this->year - 1900;
    when.tm_mon  = this->month - 1;
    when.tm_mday = this->day;
    when.tm_hour = this->hour;
    when.tm_min  = this->minutes;
    when.tm_sec  = this->seconds;

    atime = boost::posix_time::ptime_from_tm(when);
    epoch = ptime(boost::gregorian::date(1970,1,1));

    secs = (atime - epoch).total_seconds();

    return time_t(secs);
}


//...................................................... Get time in string format ...
string CTime::getTimeString() const {
    ostringstream oss;

    if(*this == CTime()) {
        return "";
    }

    oss.flags(ios::right);
    oss.fill('0');
    oss.width(4); oss << this->year;
    oss.width(1); oss << "/";
    oss.width(2); oss << this->month;
    oss.width(1); oss << "/";
    oss.width(2); oss << this->day;
    oss.width(1); oss << " ";
    oss.width(2); oss << this->hour;
    oss.width(1); oss << ":";
    oss.width(2); oss << this->minutes;
    oss.width(1); oss << ":";
    oss.width(2); oss << this->seconds;

    return oss.str();
}

//...................................................... Set current time ...
void CTime::setCurrentTime() {
    //Get current UTC time
    ptime now = boost::posix_time::second_clock::universal_time();
    tm now_tm = to_tm(now);

    this->year    = now_tm.tm_year + 1900;
    this->month   = now_tm.tm_mon + 1;
    this->day     = now_tm.tm_mday;
    this->hour   = now_tm.tm_hour;
    this->minutes = now_tm.tm_min;
    this->seconds = now_tm.tm_sec;
}


//...................................................... Assignment operator overloading ...
CTime CTime::operator=(const CTime & other) {
    //Check for self-assignment
    if(this == &other) {
        return *this;
    }

    this->year    = other.year;
    this->month   = other.month;
    this->day     = other.day;
    this->hour   = other.hour;
    this->minutes = other.minutes;
    this->seconds = other.seconds;

    return *this;
}

//...................................................... Comparison operator overloading ...
bool CTime::operator==(const CTime & other) const {
    //Check for self-assignment
    if(this == &other) {
        return true;
    }

    if (this->year == other.year &&
        this->month == other.month &&
        this->day == other.day &&
        this->hour == other.hour &&
        this->minutes == other.minutes &&
        this->seconds == other.seconds) {
        return true;
    }

    return false;
}

} /* namespace common */
