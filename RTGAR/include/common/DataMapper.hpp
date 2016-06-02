#ifndef __DATA_MAPPER
#define __DATA_MAPPER

#include <string>
#include <map>
#include <vector>

namespace common {

/**
 * This class is the base class for all the 
 * mappings kept in memory as pairs of values.
 */
class DataMapper {
    
public:
    /**
     * Returns a list of strings containing all the keys of the data mapper.
     *
     * @return  A vector of strings containing the keys of the data mapper.
     */
    std::vector<std::string> getKeys();

     /**
      * Returns the value associated with a parameter. If the 
      * parameter searched is not found, an empty string is
      * returned.
      *
      * @param  parameter The parameter for which we want the value
      * @return           The value associated with a parameter.
      */
     std::string getValue(const std::string & parameter);

protected:
            
    /**
     * Constructor of the class.
     */
    DataMapper();
    
    /**
     * Destructor of the class.
     */
    ~DataMapper();
      
    /**
     * Information of the pairs is kept internally in a map.
     */
    std::map<std::string, std::string> pairs;
        
};

} /* namespace */
#endif
