#ifndef __C_DUMP_H
#define __C_DUMP_H

#include <CDrain.hpp>
#include <string>
#include <map>

namespace common {

/**
 * Provides a mechanism to dump information to files at disk.
 * The class uses a configuration file that defines the different types 
 * of dumps we can use. Each "dump target" specifies a directory and the
 * prefix and suffix the files will have.
 *
 * This class is used as a singleton.
 *
 * The format of each entry in the configuration file is: 
 *
 * DUMP &lt;dump_name&gt;
 * DIR  &lt;directory&gt;
 * PREFIX &ltprefix of the files&gt;. Final "/" is optional.
 * SUFFIX &ltsuffix of the files&gt;. Must include the "."
 * END_DUMP
 *
 */
class CDump {

public:
    /**
     * Returns a singleton of the class
     * @param configFile 	the dumping data configuration file
     * @return 				a pointer to a singleton of the class
     */
    static CDump * GetInstance(const std::string & configFile);
    
    /**
     * Destructor of the Dump class.
     */
    ~CDump();

    /**
     * Sets the key to do a dump
     *
     * @param dumpKey  Key to choose the drain object that will do the dump.
     */
    void setDump(const std::string & dumpKey);
    
    /**
     * Sets the line of text that will be dumped to disk.
     *
     * @param dumpLine Line of text that will be dumped to disk.
     */
    void setEvent(const std::string & dumpLine);
    
    /**
     * Dumps a line to be dumped to the defined drain object according to the
     * key.
     * 
     * The order of invocation to the methods of the class is:
     * [1] Call setDump() and setEvent() to add a pair of <key,value> for the
     * dump (at the initEvent method of a raw class).
     * [2] Call dump() to add the line to the file (at the matrix operator).
     */ 
    void dump();  
    
    /**
     * Dumps a line to disk the moment the method is called.
     *
     * @param dumpKey  Key to choose the drain object that will do the dump.
     * @param dumpLine Line of text that will be dumped to disk.
     */
    void dumpInstant(const std::string & dumpKey, const std::string & dumpLine);
        
    /**
     * Finishes the files currently open.
     */
    void finishCurrentFiles();
    
    /**
     * This method closes the file associated with the configuration name
     * provided as argument. If the configuration is not found, then the
     * method does nothing.
     * 
     * @param dumpKey The dumping key of the configuration that will be closed.
     */
    void finishFile(const std::string & dumpKey);


private:
    //! Default max lines for dumping
    const static long __defaultMaxLines;

    //! Path name for the configuration file
    static std::string __configFile;
    
    //! The unique instance of the CLog class
    static std::unique_ptr<CDump> __instance;

    //! The flag that keeps from calling the instance creation more than once
    static std::once_flag __onceFlag;

    //! Line we are going to dump to disk
    std::string currentDumpLine;
    
    //! Key we are going to use to do the dump
    std::string currentDumpKey;    
            
    //! Map with the configuration of the dumps
    std::map<std::string, CDrain> dumpConfig;


    /**
     * Constructor of the Dump class.
     *
     * @param dumpConfig  the configuration file from which the configuration
     *        			  for the class will be taken.
     */
    CDump(const std::string & configFile);
    
	/**
	 * Deleted copy constructor.
	 * Disables the copy constructor of the CDump singleton.
	 * Declaring the copy constructor as private keeps from creating another CDump instance
	 * from other CDump singleton instance.
	 * @param other		A CDump singleton instance.
	 */
	CDump(const CDump& other) = delete;

	/**
	 * Assignment operator overload.
	 * Disables the assignment of the CDump singleton.
	 * Overloading the assignment operator as private keeps from copying a CDump singleton instance.
	 * @param rhs	A CDump singleton instance.
	 * @return		A copy of the CDump instance.
	 */
	CDump & operator=(const CDump& rhs) = delete;

    /**
     * Reads the configuration from the disk file.
     * Configuration is read using a tokenizer.
     *
     * @param configFile path of the configuration file
     */
    void getConfiguration(const std::string & configFile);   
};

} /* namespace */
#endif
