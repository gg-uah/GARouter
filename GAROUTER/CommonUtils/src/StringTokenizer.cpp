#include <StringTokenizer.hpp>

using std::string;
using std::vector;


namespace common {

//...................................................... constructor and destructor ...
StringTokenizer::StringTokenizer() {
    //Initialize line, separators and clear vector
    currentLine = "";
    currentSeparators = "";
    tokens.clear();
    tokenPos = 0;
}

StringTokenizer::~StringTokenizer() {
    //Destructor is empty
}



//...................................................... setter methods ...
void StringTokenizer::setLine(const string & line) {
    //copy the argument into the line
    currentLine = line;    
}

void StringTokenizer::setSeparators(const string & separators) {
    //copy the argument into the separators
    currentSeparators = separators;    
}



//...................................................... tokenize ...
void StringTokenizer::tokenize(bool useCommentMode) {
    string::size_type currPos;        //current position of the string to be tokenized
    string::size_type sepPos;         //final position of the next separator
    string::size_type commentPos;     //position of a "#" character inside current token
    string token;                     //current token extracted from the line
    bool tokenized;                   //true if the string is totally tokenized
    
    //Erase comments
    if (useCommentMode) {
        commentPos = currentLine.find("#", 0);
        if (commentPos != string::npos) {
            currentLine.erase(commentPos, currentLine.length() - commentPos);
        }
    }
       
    tokens.clear();
    tokenized = false;
    currPos = 0;
    while (!tokenized) {
        //[1] Find the minimum position of the next separator
        sepPos = currentLine.find_first_of(currentSeparators, currPos);    
                
        //[2] Get current token
        if (sepPos == string::npos) {
            //The remaining of the line is the current token
            token = currentLine.substr(currPos, currentLine.size() - currPos);           
            
            //The line has been totally tokenized
            tokenized = true;
        } else {            
            //We extract the next token from the line
            token = currentLine.substr(currPos, sepPos - currPos);
            currPos = sepPos + 1;
        }
                
        //[3] Add the token to the tokens vector
        tokens.push_back(token);           
    }   
}



//...................................................... removeEmptyTokens ...
void StringTokenizer::removeEmptyTokens() {
    vector<string>::iterator itr;
    vector<string> newVector;
        
    newVector.clear();
    for (unsigned int i = 0; i < tokens.size(); i++) {
        if (tokens[i] != "") {
            newVector.push_back(tokens[i]);    
        }
    }
    
    tokens = newVector;
}



//...................................................... getNumTokens ...
int StringTokenizer::getNumTokens() {
    //Returns the number of tokens
    return tokens.size();
}



//...................................................... token getter methods ...
string StringTokenizer::getToken(unsigned int pos) {
    string res;               //result string
    
    if (pos < tokens.size()) {
        res = tokens[pos];
    }
    else {
        res = "";   
    }
    
    return res;
}

string StringTokenizer::getNextToken() {
    string res;
    
    res = "";
    if (tokenPos < tokens.size()) {
        res = tokens[tokenPos];
        tokenPos = tokenPos + 1;
    }
    
    return res;
}

string StringTokenizer::getPreviousToken() {
    string res;
    
    res = "";
    if (tokenPos > 0) {
        tokenPos = tokenPos - 1;
        res = tokens[tokenPos];
    }
    
    return res;
}

} /* namespace common */
