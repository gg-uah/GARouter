#ifndef __EXC_STRLENTOKENIZER_HPP
#define __EXC_STRLENTOKENIZER_HPP

#include <string>

namespace common {

enum EATMStrlTkErrorTypes { STRLTK_UNSPECIFIED,
                            STRLTK_ZERO_FIELD_LENGTH,
                            STRLTK_FIELD_OUT_OF_BOUNDS,
                            STRLTK_UNKNOWN_FIELD};

/**
 * This is the possible exceptions of the class ATMStringLengthTokenizer
 */
class ExcStringLengthTokenizer {

public:
    
    /**
     * Constructors of class
     */
    ExcStringLengthTokenizer();
    
    /**
     * Constructor of the class
     *
     * @param typeException Numeric value indicating the type of the exception
     */
    ExcStringLengthTokenizer(EATMStrlTkErrorTypes typeException);
    
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
    EATMStrlTkErrorTypes type;

};

} /* namespace */
#endif
