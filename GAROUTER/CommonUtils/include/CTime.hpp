#ifndef __CTIME_H_
#define __CTIME_H_

#include <CTimeException.hpp>
#include <locale>

namespace common {

class CTime {
public:
    //...................................................... Constructors and destructor ...
    /**
     * Default constructor of the class (sets to current time)
     */
    CTime();

    /**
     * Parameterized constructor of the class
     *
     * @param year      CTime year
     * @param month     CTime month
     * @param day       CTime day
     * @param hour   	CTime hour
     * @param minutes   CTime minutes
     * @param seconds   CTime seconds
     * @throw           CTimeException
     */
    CTime(const unsigned short & year,
          const unsigned short & month,
          const unsigned short & day,
          const unsigned short & hour,
          const unsigned short & minutes,
          const unsigned short & seconds)
        throw (CTimeException);

    /**
     * Parameterized constructor of the class
     *
     * @param year      String time year
     * @param month     String time month
     * @param day       String time day
     * @param hour      String time hour
     * @param minutes   String time minutes
     * @param seconds   String time seconds
     * @throw           CTimeException
     */
    CTime(const std::string & year,
          const std::string & month,
          const std::string & day,
          const std::string & hour,
          const std::string & minutes,
          const std::string & seconds)
        throw (CTimeException);

    /**
     * String parameterized constructor
     *
     * @param date  string date in one of these possible formats:
     *              - *YYMMDDhhmmss*
     *              - *YY-MM-DD hh:mm:ss*
     *              - *YY/MM/DD hh:mm:ss*
     *              - *YY.MM.DD hh:mm:ss*
     *              - *YY-MM-DD*
     */
    CTime(const std::string & date)
        throw (CTimeException);

    /**
     * Epoch time parameterized constructor
     *
     * @param timet number of seconds since epoch time (01/01/1970 00:00:00 UTC)
     */
    CTime(const time_t & timet)
        throw (CTimeException);

    /**
     * Copy constructor
     *
     * @param other another CTime object to copy
     */
    CTime(const CTime & other);

    /**
     * Destructor
     */
    virtual ~CTime();

    //...................................................... Getter methods ...
    unsigned short getYear() const { return this->year; }

    unsigned short getMonth() const { return this->month; }

    unsigned short getDay() const { return this->day; }

    unsigned short getHour() const { return this->hour; }

    unsigned short getMinutes() const { return this->minutes; }

    unsigned short getSeconds() const { return this->seconds; }

    time_t getTimet() const;

    std::string getTimeString() const;

    //...................................................... Setter methods ...
    void setYear(const unsigned short & year) { this->year = year; }

    void setMonth(const unsigned short & month) { this->month = month; }

    void setDay(const unsigned short & day) { this->day = day; }

    void setHour(const unsigned short & hour) { this->hour = hour; }

    void setMinutes(const unsigned short & minutes) { this->minutes = minutes; }

    void setSeconds(const unsigned short & seconds) { this->seconds = seconds; }

    void setCurrentTime();

    //...................................................... Operator overloading ...
    CTime operator= (const CTime & other);

    bool operator== (const CTime & other) const;

    bool operator!= (const CTime & other) const {
        return !(*this == other);
    }

    bool operator< (const CTime & other) const {
        return (this->getTimet() < other.getTimet());
    }

    bool operator> (const CTime & other) const {
        return (this->getTimet() > other.getTimet());
    }

    bool operator<= (const CTime & other) const {
        return (this->getTimet() <= other.getTimet());
    }

    bool operator>= (const CTime & other) const {
        return (this->getTimet() >= other.getTimet());
    }

    const CTime operator+ (const int & interval) const {
        return CTime(this->getTimet() + interval);
    }

    const CTime operator- (const int & interval) const {
        return CTime(this->getTimet() - interval);
    }

    const int operator- (const CTime & other) const {
        return int(this->getTimet() - other.getTimet());
    }

    CTime & operator+= (const int & interval) {
        *this = *this + interval;
        return *this;
    }

    bool isEmpty() const {
        if ((*this) == CTime()) {
            return true;
        }
        return false;
    }

private:
    static const std::locale __STR_TIME_FORMATS[];
    static const size_t __NUM_TIME_FORMATS;

    unsigned short year;    /**< Time year */
    unsigned short month;   /**< Time month */
    unsigned short day;     /**< Time day */
    unsigned short hour;    /**< Time hour */
    unsigned short minutes; /**< Time minutes */
    unsigned short seconds; /**< Time seconds */
};

} /* namespace */
#endif /* __CTIME_H_ */
