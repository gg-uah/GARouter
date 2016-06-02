#include <EnvironmentUtil.hpp>
#include <StringTokenizer.hpp>

#ifdef WIN32
#define OS_SEP '\\'
#else
#define OS_SEP '/'
#endif

using std::string;

namespace common {

//...................................................... string transformation ...
string EnvironmentUtil::transformString(const std::string & value, bool path2Dos) {
    
    /*
     * The method used will consist in tokenizing the input string, and
     * checking if the first value of each token is a "("
     * 
     * [...]  $  [(...)...(...)...]  $  [...(...)...]
     */    
    string newValue;                     //Transformed value
    StringTokenizer dollarTokenizer;  //String tokenizer, with the "$"
    StringTokenizer parTokenizer;     //String tokenizer, with the "(" and ")" characters

#ifdef _WIN32
    dollarTokenizer.setSeparators("$");
    parTokenizer.setSeparators("()");
#else
    dollarTokenizer.setSeparators("$");
    parTokenizer.setSeparators("{}");
#endif
    
    dollarTokenizer.setLine(value);
    dollarTokenizer.tokenize();
    
    newValue = dollarTokenizer.getToken(0);
    for (int i = 1; i < dollarTokenizer.getNumTokens(); i++) {
        parTokenizer.setLine(dollarTokenizer.getToken(i));
        parTokenizer.tokenize();
        
        //If the first token has 0 length, we have an env variable
        if (parTokenizer.getToken(0).length() == 0) {
            newValue = newValue + string(getenv(parTokenizer.getToken(1).c_str()));
            //Pad remaining tokens
            for (int j = 2; j < parTokenizer.getNumTokens(); j++) {
                newValue = newValue + parTokenizer.getToken(j);    
            }
        } else {
            newValue = newValue + "$" + dollarTokenizer.getToken(i);
        }
    }

    /*
     * If value is a DOS path, transform '/' into '\'
     */
    if (path2Dos == true) {
    	for (unsigned int i = 0; i < newValue.length() ; i++) {
    		if (newValue[i] == '/') {
    			newValue[i] = '\\';
    		}
    	}
    }

    return newValue;
}

} /* namespace common */
