#ifndef __C_LOGGER_HPP
#define __C_LOGGER_HPP

#include <CLog.hpp>

namespace common {

/**
 * Allows a client to write log messages. This class uses internally the 
 * CLog class. The client class instantiates an CLogger class with the key
 * to specify where client program will write the traces.
 *
 * All the methods of the CLog class are still public, any of the two classes
 * may be used.
 */ 
class CLogger {

public:
    
    /**
     * Constructor of the CLogger class.
     *
     * @param logId 	 The key we will use with this class to dump the messages.
     * @param logConfig	 The logging configuration file name.
     * @param dumpConfig The dumping configuration file name.
     */
    CLogger(const std::string & logId,
    		const std::string & logConfig,
			const std::string & dumpConfig);

    
    /**
     * Deleted empty constructor.
     * Prevents creating an instance of the {@link #CLogger} class
     */
    CLogger() = delete;


    /**
     * Destructor of the CLogger class.
     */
    ~CLogger(void);
    
    
    /**
     * Writes a log at the debug level.
     *
     * @param message text std::string to be sent to the log.
     */
    void debug(const std::string & message);
    
    /**
     * Writes a log at the info level.
     *
     * @param message text std::string to be sent to the log.
     */
    void info(const std::string & message);
    
    /**
     * Writes a log at the warning level.
     *
     * @param message text std::string to be sent to the log.
     */
    void warning(const std::string & message);

    /**
     * Writes a log at the error level.
     *
     * @param message text std::string to be sent to the log.
     */
    void error(const std::string & message);

    /**
     * Writes a log at the fatal level.
     *
     * @param message text std::string to be sent to the log.
     */
    void fatal(const std::string & message);
   
private:
    
    //! Pointer to the CLog singleton
    CLog * log;
    
    //! String to store the logName where the messages will be dumped
    std::string logName;
};

} /* namespace */
#endif
