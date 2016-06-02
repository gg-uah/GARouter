#include <StringLengthTokenizer.hpp>
#include <ExcStringLengthTokenizer.hpp>

using std::string;
using std::map;
using std::pair;

namespace common {

//...................................................... Constructor and destructor ...
StringLengthTokenizer::StringLengthTokenizer() {
    line = "";
}

StringLengthTokenizer::~StringLengthTokenizer() {
    posIni.clear();
    length.clear();
}



//...................................................... setLine ...
void StringLengthTokenizer::setLine (const string & lineToAssig) {
    
    line = lineToAssig;
}



//...................................................... addField (configuration) ...
void StringLengthTokenizer::addField (const string & fieldName, int ini, int lengthField) {

    if (lengthField > 0) {
        posIni.insert(pair<string, int>(fieldName, ini));
        length.insert(pair<string, int>(fieldName, lengthField));
    } else {
        throw ExcStringLengthTokenizer(STRLTK_ZERO_FIELD_LENGTH);
    }
}



//...................................................... getField (extract token) ...
string StringLengthTokenizer::getField (const string & field){
    int initialPosition = 0;
    int fieldLength = 0;
    string value = "";
        
    map<string, int>::iterator fieldPosItr;  //Position of a field
    map<string, int>::iterator fieldLenItr;  //Size of a field
    
    fieldPosItr = posIni.find(field);
    
    if (fieldPosItr != posIni.end()) {
        initialPosition = fieldPosItr->second;
        fieldLenItr = length.find(field);        
        fieldLength = fieldLenItr->second;
        if ((fieldLength + initialPosition) > line.length()) {
           throw ExcStringLengthTokenizer(STRLTK_FIELD_OUT_OF_BOUNDS);
        }
        value = line.substr(initialPosition, fieldLength);            
    } else {
        throw ExcStringLengthTokenizer(STRLTK_UNKNOWN_FIELD);
    }
    
    return value;    
}

} /* namespace common */
