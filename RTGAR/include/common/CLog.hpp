#ifndef __C_LOG_HPP
#define __C_LOG_HPP

#include <string>
#include <map>
#include <memory>
#include <mutex>

namespace common {
    
/**
 * This class provides a logging facility, allowing classes where
 * the logging facility is used to write traces. The logging class
 * uses the ATMDump class internally, to define where the different
 * traces will be dumped.
 *
 * The format of each entry in the configuration file will be:
 *
 * LOG &lt;log name&gt;
 * LEVEL DEBUG|INFO|WARNING|ERROR|FATAL
 * DUMP SCREEN|&lt;dump name&gt;
 * END_LOG
 *
 * Behavior will be showing traces greater than or equal in severity
 * of the defined level. This means that if, for example, we set the level
 * to WARNING, only the WARNING, ERROR, and FATAL traces will be shown.
 *
 * The DUMP may be on screen (standard output), or to a file on disk.
 *
 */
class CLog {

public:
    /**
     * Returns a singleton of the class.
     * @param logConfig  The log configuration path file name
     * @param dumpConfig The dump configuration path file name
     * @return 			  A unique pointer to the singleton instance of the class.
     */    
    static CLog * GetInstance(const std::string & logConfig,
    						  const std::string & dumpConfig);
    
	/**
	 * Destructor of the CLog class.
	 */
	~CLog(void);

    /**
     * Writes a log at the debug level.
     *
     * @param logName Log configuration where we are going to dump.
     * @param message Text string to be sent to the log.
     */
    void debug(const std::string & logName, const std::string & message);
    
    /**
     * Writes a log at the info level.
     *
     * @param logName Log configuration where we are going to dump.
     * @param message Text string to be sent to the log.
     */
    void info(const std::string & logName, const std::string & message);
    
    /**
     * Writes a log at the warning level.
     *
     * @param logName Log configuration where we are going to dump.
     * @param message Text string to be sent to the log.
     */
    void warning(const std::string & logName, const std::string & message);

    /**
     * Writes a log at the error level.
     *
     * @param logName Log configuration where we are going to dump.
     * @param message Text string to be sent to the log.
     */
    void error(const std::string & logName, const std::string & message);

    /**
     * Writes a log at the fatal level.
     *
     * @param logName Log configuration where we are going to dump.
     * @param message Text string to be sent to the log.
     */
    void fatal(const std::string & logName, const std::string & message);

private:
    //! Debug levels.
    const static int LOG_DEBUG_LEVEL   = 0;
    const static int LOG_INFO_LEVEL    = 1;
    const static int LOG_WARNING_LEVEL = 2;
    const static int LOG_ERROR_LEVEL   = 3;
    const static int LOG_FATAL_LEVEL   = 4;

    //! The log configuration path file name
    static std::string __logConfig;

    //! The dump configuration path file name
    static std::string __dumpConfig;

    //! The unique instance of the CLog class
    static std::unique_ptr<CLog> __instance;

    //! The flag that keeps from calling the instance creation more than once
    static std::once_flag __onceFlag;

    //! Maps a log configuration id with the established trace level
    std::map<std::string, int> logLevels;

    //! Maps a log configuration id with a dump configuration id
    std::map<std::string, std::string> dumpNames;

    
    /**
     * Constructor of the CLog class.
     * @param logConfig 	The logging configuration file.
     * @param dumpConfig	The dumping configuration file.
     */
	CLog(const std::string & logConfig,
		 const std::string & dumpConfig);
	
	/**
	 * Deleted copy constructor.
	 * Disables the copy constructor of the CLog singleton.
	 * Declaring the copy constructor as private keeps from creating another CLog instance
	 * from other CLog singleton instance.
	 * @param other		A CLog singleton instance.
	 */
	CLog(const CLog& other) = delete;

	/**
	 * Assignment operator overload.
	 * Disables the assignment of the CLog singleton.
	 * Overloading the assignment operator as private keeps from copying a CLog singleton instance.
	 * @param rhs	A Clog singleton instance.
	 * @return		A copy of the CLog instance.
	 */
	CLog & operator=(const CLog& rhs) = delete;

	/**
	 * Reads the configuration from the disk file.
	 * Configuration is read using a tokenizer.
	 *
	 * @param configFile path of the configuration file.
	 */
	void getConfiguration(const std::string & configFile);
	
	/**
	 * Writes a log std::string to the target log.
	 *
	 * @param level   The severity level of the log.
     * @param logName Log configuration where we are going to dump.	 
	 * @param message The message we are going to send to the target log. 
	 */
	void logMessage(int level, const std::string & logName, std::string message); 
};

} /* namespace */
#endif
