#ifndef __ATM_OBFUSCATOR
#define __ATM_OBFUSCATOR

#include <string>
#include <map>

namespace common {

/**
 * This class provides two methods to encode and decode a string
 * provided as argument.
 */
class Obfuscator {

public:

    /**
     * Constructor of the class.
     */
    Obfuscator();
    
    /**
     * Destructor of the class.
     */
    ~Obfuscator();
    
    /**
     * Encodes the string provided as argument, and returns it
     * 
     * @param  originalString The string that will be encoded
     * @return The encoded string
     */
    std::string encodeString(const std::string & originalString);
    
    /**
     * Decodes the string provided as argument, and returns it
     * 
     * @param  encodedString The string encoded that we want to decode
     * @return The decoded string
     */
    std::string decodeString(const std::string & encodedString);
    
private:

    //--------------------------------------------- character arrays ---
    const static std::string ORIGINAL_CHARACTERS;
    const static std::string ENCODED_CHARACTERS;

    //--------------------------------------------- encoding maps ---
    std::map<char, char> encodingMap;
    std::map<char, char> decodingMap; 
    
    
    
    //--------------------------------------------- methods ---
    /**
     * Returns the encoded character of the character provided as argument. If
     * the character is not found, it is returned as it comes.
     * 
     * @param  c The character that will be encoded
     * @return The encoded character
     */
    char encodeCharacter(char c);
    
    /**
     * Returns the decoded character of the char provided as argument. If the
     * character is not found, it is returned as it comes.
     * 
     * @param  c The character that will be decoded
     * @return The decoded character
     */
    char decodeCharacter(char c);

}; 

} /* namespace */
#endif
