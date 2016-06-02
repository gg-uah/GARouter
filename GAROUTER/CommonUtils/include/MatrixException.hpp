#ifndef __MATRIX_EXCEPTION
#define __MATRIX_EXCEPTION

#include <string>

namespace common {

enum MatrixErrorType {MATEXC_USPECIFIED,
                      MATEXC_BAD_DIMENSIONS,
                      MATEXC_BAD_ACCESS,
                      MATEXC_ALLOC_ERROR};

/**
 * This is the possible exceptions of the class StringUtilities
 */
class MatrixException {

public:
    
    /**
     * Constructors of class
     */
    MatrixException();
    
    /**
     * Constructor of the class
     *
     * @param typeException Numeric value indicating the type of the exception
     */
    MatrixException(MatrixErrorType typeException);
    
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
     MatrixErrorType type;

};

} /* namespace */
#endif
