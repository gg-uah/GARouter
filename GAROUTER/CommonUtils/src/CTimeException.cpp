#include <CTimeException.hpp>

using std::string;

namespace common {

//...................................................... constructor and destructor ...
CTimeException::CTimeException() {
    type = TIMEXC_UNSPECIFIED;
}

CTimeException::CTimeException(TimeErrorType typeException) {
    type = typeException;
}


//...................................................... getter methods ...
int CTimeException::getType() {
    return type;
}

string CTimeException::getReason() {
    string reason;       //The reason that will be returned

    switch (type){
        case TIMEXC_UNSPECIFIED:
            reason = "Unknown reason";
            break;
        case TIMEXC_BAD_YEAR:
            reason = "Unsuitable year value";
            break;
        case TIMEXC_BAD_MONTH:
            reason = "Unsuitable month value";
            break;
        case TIMEXC_BAD_DAY:
            reason = "Unsuitable day value";
            break;
        case TIMEXC_BAD_HOURS:
            reason = "Unsuitable hours value";
            break;
        case TIMEXC_BAD_MINUTES:
            reason = "Unsuitable minutes value";
            break;
        case TIMEXC_BAD_SECONDS:
            reason = "Unsuitable seconds value";
            break;
        case TIMEXC_BAD_DATE_TIME:
            reason = "Unsuitable date-time value";
            break;
    }
    
    return reason;
}

} /* namespace common */
