#include <FileMapper.hpp>
#include <fstream>
#include <string>
#include <stdlib.h>
#include <StringTokenizer.hpp>

#include <iostream>

using std::string;
using std::ifstream;
using std::pair;
 
namespace common {

//...................................................... constructor and destructor ...
FileMapper::FileMapper(const string & filePath, const string & separators) {
    
    ifstream file;
    string line;
    string fullPath;
    StringTokenizer tokenizer;   //Tokenizer
    
    //Open the file
    file.open (filePath.c_str());
     
    //Initialize tokenizer
    tokenizer.setSeparators (separators);
    
    //Ensure pairs is empty
    pairs.clear();
    if (file.is_open()) {
        
        //Load pairs from the file
        while (! file.eof() ) {
            getline(file, line);
            if (line != "") {
                tokenizer.setLine(line);
                tokenizer.tokenize();
                pairs.insert(pair<string, string>(tokenizer.getToken(0), 
                                                  tokenizer.getToken(1)));                              
            }  
        }
        file.close();
      
    } else {
        // TODO: add debug msg
    }
}   

FileMapper::~FileMapper() {
    //Destructor is empty      
}

} /* namespace common */
