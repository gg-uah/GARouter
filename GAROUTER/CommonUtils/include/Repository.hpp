#ifndef __REPOSITORY_HPP
#define __REPOSITORY_HPP

#include <string>
#include <dirent.h>

namespace common {

/** 
 * Provides an abstraction of a file system folder.
 * Calling program may use the methods of this class to return the 
 * names of the files stored in one folder.
 *
 * When no more sources in the file match the specified pattern,
 * or there are no more files in target directory, the values 
 *
 */
class Repository {

public:
    
    /** 
     * Constructor o the Repository class.
     *
     * @param  path   The absolute path of the folder the client is going to access.
     * @param  prefix The prefix of the filenames the repository will retrieve.
     * @param  suffix The suffix of the filenames the repository will retrieve.
     * @throws        std::exception. This exception is thrown when the repository cannot be opened.
     */
    Repository(const std::string & path, const std::string & prefix, const std::string & suffix);
    
    /**
     * Destructor of the Repository class.
     */
    ~Repository();
    
    /**
     * Advances cursor in the directory, moving to the next file.
     *
     * @param  rewindDir True if the dir has to be rewinded, false otherwise
     * @return           True if there is another valid file in the repository, false otherwise.
     */
    bool otherFile(bool rewindDir = true);
    
    /**
     * Tells the object to rewind the directory.
     */
    void rewindDir();
    
    
    
    //------------------------------------------------------ current file info ---
    /**
     * Returns the complete access path (path and file name) of the next source in the repository.
     *
     * @return Complete path and name of the next file in the repository.
     */
    std::string getCurrentPathAndName();
    
    /**
     * Returns the name (only file name) of the next source in the repository.
     *
     * @return The name of the next file in the repository.
     */
    std::string getCurrentName();
    
    
    
    //------------------------------------------------------ repository properties ---
    /**
     * Returns the path of the repository
     * 
     * @return The path of the repository.
     */
    std::string getPath();
    
    /**
     * Returns the prefix of the repository.
     * 
     * @return The prefix of the repository.
     */
    std::string getPrefix();
    
    /**
     * Returns the suffix of the repository.
     * 
     * @return The suffix of the repository.
     */
    std::string getSuffix();
    
private:
    
    /* pointer to a DIR structure. */
    DIR * dir;
        
    /* path, prefix and suffix given in the initialization*/
    std::string repoPath;
    std::string repoPrefix;
    std::string repoSuffix;
    
    /* full path and file name of the current file */
    std::string currentPathAndName;
    std::string currentName;
    
    
            
    /**
     * Returns either true or false depending if current entry in the directory matches the pattern.
     *
     * @param fileName The name of the file to be checked.
     */
    bool isCorrect(const std::string & fileName);
    
};

} /* namespace */
#endif
