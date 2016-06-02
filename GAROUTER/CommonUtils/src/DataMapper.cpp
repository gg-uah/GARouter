#include <DataMapper.hpp>

using std::string;
using std::map;
using std::pair;
using std::vector;

namespace common {

//...................................................... getValue ...
vector<string> DataMapper::getKeys() {
    map<string, string>::const_iterator it;
    vector<string> vkeys;

    for (it = this->pairs.begin(); it != this->pairs.end(); it++) {
        vkeys.push_back(it->first);
    }

    return vkeys;
}


//...................................................... getValue ...
string DataMapper::getValue(const string & parameter) {
    //Look for the item in the map
    map<string, string>::iterator found;
        
    found = pairs.find(parameter);
    if (found != pairs.end()) {
        return found->second;    
    } else {
        return "";
    }
}


//...................................................... constructor and destructor ...
DataMapper::DataMapper() {
    //Constructor is empty
}

DataMapper::~DataMapper() {
    //Destructor is empty    
}

} /* namespace common */
