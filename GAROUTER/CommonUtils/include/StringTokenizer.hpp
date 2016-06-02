#ifndef __STRING_TOKENIZER_HPP
#define __STRING_TOKENIZER_HPP

#include <string>
#include <vector>

namespace common {

/**
 * Provides functions to turn a string into tokens.
 * Client class specifies the string to be tokenized and the characters
 * that must be used as separators.
 * Once tokenized, client class may access the tokens separately
 */
class StringTokenizer {

public:
    
    /**
     * Constructor of the StringTokenizer class.
     */    
    StringTokenizer();
    
    /**
     * Destructor of the StringTokenizer class.
     */
    ~StringTokenizer();
        
    /**
     * Sets the line that we want to parse.
     *
     * @param line The line that we are going to tokenize.
     */
    void setLine(const std::string & line);
    
    /**
     * Sets the characters that we will consider as separators.
     *
     * @param separators The set of characters that will be considered
     *                   as separators. Each character will be treated
     *                   as a separator.
     */
    void setSeparators(const std::string & separators);
    
    /**
     * Tokenizes the string setted, leaving the result in the tokens vector
     *
     * @param useCommentMode if set to true, the "#" character will be used as
     *                       the beggining of a comment.
     */
    void tokenize(bool useCommentMode = false);
    
    /**
     * This method removes the empty tokens, once the string has been tokenized. 
     * Note that empty tokens are not removed by default, because they be meaningless
     * in some circumstances, but have some meaning in other cases.
     */
    void removeEmptyTokens();
    
    /**
     * Returns the number of tokens in the string.
     *
     * @return the number of tokens in the tokenized line.
     */
    int getNumTokens();
    
    /**
     * Returns the token at position pos.
     *
     * @param  pos the position of the token we want to get. Should be
     *             between 0 and getNumTokens()-1. 
     * @return     the token at the specified position. If the token requested
     *             is out of the range of tokens, an empty string is returned.
     */
    std::string getToken(unsigned int pos);
        
    /**
     * Retuns the next token in the file, and updates the token cursor.
     *
     * @return The next token in the file, as a string object. If there are
     *         no more tokens in the file, an empty string is returned.
     */
    std::string getNextToken();
    
    /**
     * Returns the previous token, and updates the token cursor.
     *
     * @return The previous token in the file, as a string object. If we try 
     *         to get the previous token of the first one, an empty string will
     *         be returned.
     */
    std::string getPreviousToken();
    
private:    
    
    /* current line that we want to tokenize */
    std::string currentLine;
    
    /* current set of characters that will be considered as separators */
    std::string currentSeparators;
    
    /* set of tokens */
    std::vector<std::string> tokens;
        
    /* position of current token */
    unsigned int tokenPos;
        
};

} /* namespace */
#endif
