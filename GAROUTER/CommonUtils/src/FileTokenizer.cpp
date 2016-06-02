#include <FileTokenizer.hpp>
#include <StringTokenizer.hpp>
#include <StringUtilities.hpp>
#include <fstream>
#include <iostream>

using std::string;
using std::vector;
using std::ifstream;

namespace common {

//...................................................... constructor and destructor ...
FileTokenizer::FileTokenizer() {
    currentFileName = "";
    currentSeparators = "";
    tokenPos = 0;
    tokens.clear();
}

FileTokenizer::~FileTokenizer() {
    //Destructor is empty
}



//...................................................... set File Name ...
void FileTokenizer::setFileName(const string & fileName) {
    currentFileName = fileName;    
}


//...................................................... set token separator characters ...
void FileTokenizer::setSeparators(const string & separators) {
    //copy the argument into the separators
    if (separators == "\\n") {
        currentSeparators = "";
   } else {
        currentSeparators = separators;
    }
}


//...................................................... tokenize ...
bool FileTokenizer::tokenize() {
    ifstream currentFile;             //file to be tokenized
    string inputLine;                 //line that will be read from the file
    StringTokenizer * tkn;         //String tokenizer
    int i;                            //index variable
    string token;                     //token currently extracted
    bool res = true;
    
    tkn = new StringTokenizer();
    tkn->setSeparators(currentSeparators);
    
	// [1] Initialize position and open the file to be tokenized
	tokenPos = 0;
	tokens.clear();
	currentFile.open(currentFileName.c_str(), std::ifstream::in);

	if (currentFile.fail()) {
		std::cerr << "Can't open file [" << currentFileName << "]" << std::endl;

		res = false;

	} else {
        // [2] Read input while the file has not been finished, and tokenize
        while (currentFile.good()) {
            //Get next line
            std::getline(currentFile, inputLine);

			#ifndef _WIN32
            inputLine = StringUtilities::removeAtEnd(inputLine, __CRLF);
			#endif

            //Tokenize
            tkn->setLine(inputLine);
            tkn->tokenize(true);

            /*
             * Add tokens to the token buffer.
             *
             * Tokens are added while they are "correct" tokens. A correct token is a
             * token that is not empty, does not contain "#", and does not appear
             * after a "#" character in the line.
             */
            i = 0;
            while (i < tkn->getNumTokens()) {
                token = tkn->getToken(i);

                if (token == "") {
                    //The token is blank. Do not add but check next token
                    i = i + 1;
                } else {
                    //The token is not blank, and has no comments inside it
                    tokens.push_back(token);
                    i = i + 1;
                }
            }
        }
    }

    currentFile.close();
    
    //Free memory of the ATMStringTokenizer object
    delete tkn;

    return res;
}



//...................................................... token getter methods ...
int FileTokenizer::getNumTokens() {
    //Returns the number of tokens
    return tokens.size();
}

string FileTokenizer::getNextToken() {
    string res = "";
    if (tokenPos < tokens.size()) {
        res = tokens[tokenPos];
        tokenPos = tokenPos + 1;
    }
    
    return res;
}

string FileTokenizer::getPreviousToken() {
    string res = "";
    if (tokenPos > 0) {
        tokenPos = tokenPos - 1;
        res = tokens[tokenPos];
    }
    
    return res;
}

} /* namespace common */
