#ifndef __C_DRAIN_HPP
#define __C_DRAIN_HPP

#include <string>
#include <iostream>
#include <fstream>

namespace common {

/** 
 * Provides an easy way to dump information to a file or several files on disk.
 * The name of the file is specified at the constructor, but the date and pid of
 * the process will be added between the prefix and the suffix. Each file will have 
 * no more lines than the "lines" parameter of the constructor.
 */
class CDrain {
    
public:

    /** 
     * Constructor of the CDrain class.
     *
     * @param directory Folder where we are going to do the drain
     * @param prefix    Prefix of the file
     * @param suffix    Suffix of the file
     * @param lines     Number of lines after which the current file will
     *                  be closed
     */
    CDrain(const std::string & directory, const std::string & prefix, const std::string & suffix, int lines=10000);
    
    /**
     * Copy constructor of the CDrain class
     *
     * @param fdr the object we are going to copy
     */
    CDrain(const CDrain & fdr);

    /**
     * Destructor free the resources of the object this.
     */
    ~CDrain();

    /**
     * Adds a line to the current file
     *
     * @param line line to be added to the current file
     */
    void addLine(const std::string & line);
    
    /**
     * Finishes the current file
     */
    void finishCurrentFile();  
    
    /**
     * Deletes the current file. This method may be invoked if, for some reason,
     * we want to stop the "drain" processing in target file and discard it from
     * the file system.
     */
    void deleteCurrentFile();  
        
private:

    /* Name of the current archive */
    std::string currentFileName;

    /* Output stream of the current archive */
    std::ofstream currentFile;

    /* tag to differenciate between two or more files generated in the same second. */
    int currentEnd;                         

    /* configuration of the dump file */
    std::string directory;
    std::string prefix;
    std::string suffix;
    int  numRecords;
    int  currentRecord;
    
    /**
     * Builds the name of the file (currentFileName)
     *
     * @param  append The final string to be appended to the name of the file
     * @return        Name of the file
     */
    std::string buildFileName(const std::string & append);    
    
    /**
     * Initializes the values of the class, invoked from the constructors of the object.
     *
     * @param directory Folder where we are going to do the drain
     * @param prefix    Prefix of the file
     * @param suffix    Suffix of the file
     * @param lines     Number of lines after which the current file will
     *                  be closed 
     */
    void initialize(const std::string & directory, const std::string & prefix, const std::string & suffix, int lines);
    
    /* Opens a new file for dumping. */
    void openNewFile();    
        
};

} /* namespace */
#endif
