#ifndef __FILE_MAPPER_HPP
#define __FILE_MAPPER_HPP

#include <DataMapper.hpp>

namespace common {

/**
 * This class implements the mapping where the data
 * is read from a file on disk.
 */
class FileMapper : public DataMapper {

public:
    
    /**
     * Constructor of the class, reads the pairs from a file on disk. 
     *
     * @param filePath   The path of the file to be read.
     * @param separators Separator character set used to tokenize the file into pairs to insert into a map
     */
    FileMapper(const std::string & filePath,
                  const std::string & separators = "|");
        
    /**
     * Destructor of the class.
     */
    ~FileMapper();
    

};

} /* namespace */
#endif
