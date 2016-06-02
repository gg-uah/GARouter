#ifndef __FILE_TOKENIZER_HPP
#define __FILE_TOKENIZER_HPP

#include <string>
#include <vector>

#define __CRLF "\r"

namespace common {

/**
 * Given a file, converts it into a list of tokens.
 *
 * Typicall usage should be to tokenize a file, and then call to the getNextToken()
 * and getPreviousToken() methods, to parse the grammar or configuration file.
 *
 */
class FileTokenizer {
    
public:

    /**
     * Constructor of the FileTokenizer class.
     */    
    FileTokenizer();
    
    /**
     * Destructor of the FileTokenizer class.
     */
    ~FileTokenizer();
        
    /**
     * Sets the complete path of the file to be tokenized.
     *
     * @param fileName The name and path of the file that will be tokenized.
     */
    void setFileName(const std::string & fileName);
        
    /**
     * Sets the characters that we will consider as separators.
     *
     * @param separators The set of characters that will be considered
     *                   as separators. Each character will be treated
     *                   as a separator.
     */
    void setSeparators(const std::string & separators);

    /**
     * Tokenizes the current file.
     *
     * @return 'true' if the file was straightly tokenized,
     * 		   'false' if an error raised while tokenizing the file.
     */
    bool tokenize();
    
    /**
     * Returns the number of tokens in the std::string.
     *
     * @return the number of tokens in the tokenized line.
     */
    int getNumTokens();    
    
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
    
    /* Current file to be tokenized */
    std::string currentFileName;
    
    /* current set of characters that will be considered as separators */
    std::string currentSeparators;

    /* Position of the tokens cursor */
    unsigned int tokenPos;
    
    /* set of tokens */
    std::vector<std::string> tokens;
        
};

} /* namespace */
#endif
