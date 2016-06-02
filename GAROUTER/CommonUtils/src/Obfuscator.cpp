#include <Obfuscator.hpp>

using std::string;
using std::map;
using std::pair;

namespace common {

//Static arrays
const string Obfuscator::ORIGINAL_CHARACTERS = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789";
const string Obfuscator::ENCODED_CHARACTERS  = "ZAY9wX8V7U65TzsyRqxOupvntWmSlrB1C3E2DQ4aFbGkPjoiNhMLgcHIdfK0eJ";

//...................................................... constructor and destructor ...
Obfuscator::Obfuscator() {
   //Fill the maps
   encodingMap.clear();
   for (unsigned int i = 0; i < ORIGINAL_CHARACTERS.length(); i++) {
       encodingMap.insert(pair<char, char> (ORIGINAL_CHARACTERS[i], ENCODED_CHARACTERS[i]));
       decodingMap.insert(pair<char, char> (ENCODED_CHARACTERS[i],  ORIGINAL_CHARACTERS[i]));
   }
}

Obfuscator::~Obfuscator() {
    //Destructor is empty
}



//...................................................... encodeString ...
string Obfuscator::encodeString(const string & originalString) {
    string encodedString;        //The encoded string
    
    encodedString = "";
    for (unsigned int i = 0; i < originalString.length(); i++) {
        encodedString += encodeCharacter(originalString[i]);    
    }
    
    return encodedString;
}



//...................................................... decodeString ...
string Obfuscator::decodeString(const string & encodedString) {
    string decodedString;        //The decoded string
    
    decodedString = "";
    for (unsigned int i = 0; i < encodedString.length(); i++) {
        decodedString += decodeCharacter(encodedString[i]);    
    }
    
    return decodedString;
}



//...................................................... encodeCharacter ...
char Obfuscator::encodeCharacter(char c) {
    map<char, char>::iterator found;   //To look for the character
    char result;                       //The result that will be returned
    
    found = encodingMap.find(c);
    if (found != encodingMap.end()) {
        result = found->second;
    } else {
        result = c;    
    }
    
    return result;
}



//...................................................... decodeCharacter ...
char Obfuscator::decodeCharacter(char c) {
    map<char, char>::iterator found;   //To look for the character
    char result;                       //The result that will be returned
    
    found = decodingMap.find(c);
    if (found != decodingMap.end()) {
        result = found->second;
    } else {
        result = c;    
    }
    
    return result;        
}

} /* namespace common */
