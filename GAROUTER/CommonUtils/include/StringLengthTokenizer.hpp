#ifndef __STRING_LENGTH_TOKENIZER
#define __STRING_LENGTH_TOKENIZER

#include <string>
#include <map>

namespace common {

/** 
 * This class is used to extract the tokens of a string, that begin and end at a specific position
 * It is necessary to to indicate the beginning and the length wherefrom they begin the words, 
 * which correspond with fields.
 */
class StringLengthTokenizer {

private:
    
    /**
     * Property where is stored the initial positio of field
     */
    std::map <std::string, int> posIni;
    
    /**
     * Property where is stored the initial positio of field
     */
    std::map <std::string, int> length;
    
    /**
     * Property where is stored the line that contains the fields
     */
    std::string line;
    
public:
    
    /**
     * Contructor of the class
     */
    StringLengthTokenizer();
    
    /**
     * Destructor of the class
     */
    ~StringLengthTokenizer();
    
    /**
     * Method that adds fields to the class the value to be extracted the line
     *
     * @param fieldName   Name of the field
     * @param ini         Initial position of the field value
     * @param fieldLength Length of the value of field
     * @throws            EStringLengthTokenizer
     */
    void addField (const std::string & fieldName, int ini, int fieldLength);
    
    /**
     * Method that a string assigns to the property line of the 
     * class, to extract the values of the fields
     *
     * @param  lineToAssig Line that is assigned to the property line 
     */
    void setLine (const std::string & lineToAssig);
    
    /**
     * Method to extract the value of a field
     *
     * @param  fieldName The name of the field to extract value
     * @return       	 The value of the field
     * @throws       	 EStringLengthTokenizer
     */
    std::string getField (const std::string & fieldName);

};

} /* namespace */
#endif
