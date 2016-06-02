#include <MatrixException.hpp>

using std::string;

namespace common {

//...................................................... constructor and destructor ...
MatrixException::MatrixException() {
    type = MATEXC_USPECIFIED;
}

MatrixException::MatrixException(MatrixErrorType typeException) {
    type = typeException;
}


//...................................................... getter methods ...
int MatrixException::getType() {
    return type;
}

string MatrixException::getReason() {
    string reason;       //The reason that will be returned

    switch (type){
        case MATEXC_USPECIFIED:
            reason = "Unknown reason";
            break;
        case MATEXC_BAD_DIMENSIONS:
            reason = "Bad matrix dimensions";
            break;
        case MATEXC_BAD_ACCESS:
            reason = "Matrix accessing indexes out of bounds";
            break;
        case MATEXC_ALLOC_ERROR:
            reason = "Matrix memory allocation error";
            break;
    }
    
    return reason;
}

} /* namespace common */
