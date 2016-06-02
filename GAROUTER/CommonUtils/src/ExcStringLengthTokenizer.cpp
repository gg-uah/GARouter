#include <ExcStringLengthTokenizer.hpp>

using std::string;

namespace common {

//...................................................... constructor and destructor ...
ExcStringLengthTokenizer::ExcStringLengthTokenizer() {
    type = STRLTK_UNSPECIFIED;
}

ExcStringLengthTokenizer::ExcStringLengthTokenizer(EATMStrlTkErrorTypes typeException) {
    type = typeException;
}


//...................................................... getter methods ...
int ExcStringLengthTokenizer::getType() {
    return type;
}

string ExcStringLengthTokenizer::getReason() {
    string reason;       //The reason that will be returned
    
    
    switch (type){
        case STRLTK_UNSPECIFIED:
            reason = "Unknown reason";
            break;
        case STRLTK_ZERO_FIELD_LENGTH:
            reason = "Length 0";
            break;
        case STRLTK_FIELD_OUT_OF_BOUNDS:
            reason = "The field that is retrieved exceeds the length of the line";
            break;
        case STRLTK_UNKNOWN_FIELD:
            reason = "Field not found";
            break;
    }
    
    return reason;
}

} /* namespace common */
