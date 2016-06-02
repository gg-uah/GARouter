#ifndef __CONFIG_PARAMS_HPP
#define __CONFIG_PARAMS_HPP

#include <GARConst.hpp>
#include <CLogger.hpp>
#include <Obfuscator.hpp>
#include <map>
#include <string>

using common::CLogger;
using common::Obfuscator;

	/**
 * This class contains the configuration parameters that are read
 * from a text file on disk. A class that uses configuration parameters
 * should inherit from this one and may be used as a singleton.
 */
class ConfigParams {
    
public:

    /**
     * Returns a singleton of the class.
     *
     * @return pointer to a singleton of the class.
     */
    static ConfigParams * Instance();


    /**
     * Returns the name of the log key that must be used.
     *
     * @return the log key that will be used
     */
    std::string getLogKey() const {
        return this->logKey;
    }

    /**
     * Return the geographic zones file
     *
     * @return The geographic zones file
     */
    std::string getGeoZonesFile() const {
        return this->geoZonesFile;
    }

    /**
     * Return the behaviour for traffic involving non-catalogued airports
     *
     * @return KEEP: Traffic involving non-catalogued airports is kept
     *         FILT: Traffic involving non-catalogued airports is filtered
     *         \<NODE_NAME\>: Non-catalogued airports in traffic data are grouped in a \<NODE_NAME\> node, e.g.: OTHE
     */
    std::string getNonCatAirpts() const {
        return this->nonCatAirpts;
    }

    /**
     * Return the network traffic percentile managed
     *
     * @return The network traffic percentile managed
     */
    float getTrafficPercent() const {
        return this->trafficPercent;
    }

    /**
     * Return the default number of seats in an aircraft
     *
     * @return  The default number of seats in an aircraft
     */
    short getAircftSeats() const {
        return this->aircftSeats;
    }

    float getSeatsSdPercent() const {
        return this->aircftSeatsSd;
    }

    /**
     * Return the turn-around time for an aircraft
     *
     * @return  The turn-around time for an aircraft in seconds
     */
    short getRotationSecs() const {
        return this->rotationMins * GARConst::SECS_PER_MIN;
    }

    /**
     * Return the standard deviation for the rotation time
     *
     * @return  The standard deviation for the rotation time
     */
    float getRotationSdPercent() const {
        return this->rotationSd;
    }

    /**
     * Return the default number of gates in an airport
     *
     * @return  The default number of gates in an airport
     */
    short getAirptGates() const {
        return this->airptGates;
    }

    /**
     * Return the time interval in seconds for the hourly traffic histogram
     *
     * @return  The time interval in seconds for the hourly traffic histogram
     */
    short getHistIntervalSecs() const {
        return this->histIntervalMins * GARConst::SECS_PER_MIN;
    }

    /**
     * Return the maximum percentage of capacity managed by an airport in the hourly traffic histogram
     *
     * @return  The maximum percentage of capacity managed by an airport in the hourly traffic histogram
     */
    float getCapHistPercent() const {
        return this->histPercentCap;
    }

    /**
     * Return the minimum nominal capacity managed by an airport
     *
     * @return  The minimum nominal capacity managed by an airport
     */
    short getMinNominalCap() const {
        return this->minNominalCap;
    }

    int getTaxiSecs() const {
        return this->taxiMins * GARConst::SECS_PER_MIN;
    }

    /**
     * Return the standard deviation for the stochastic component of the taxi-in time
     *
     * @return  The standard deviation for the stochastic component of the taxi-in time
     */
    float getTxiSdPercent() const {
        return this->txiSd;
    }

    /**
     * Return the standard deviation for the stochastic component of the taxi-out time
     *
     * @return  The standard deviation for the stochastic component of the taxi-out time
     */
    float getTxoSdPercent() const {
        return this->txoSd;
    }

    /**
     * Return the standard deviation for the stochastic component of the flight duration
     * 
     * @return  The standard deviation for the stochastic component of the flight duration
     */
    float getFltDurSdPercent() const {
        return this->fltDurSd;
    }
    
    /**
     * Return the minimum connecting time between gates in seconds
     *
     * @return  The minimum connecting time between gates in seconds
     */
    short getMctSecs() const {
        return this->mctMins * GARConst::SECS_PER_MIN;
    }

    /**
     * Return the standard deviation for the stochastic component of the minimum connecting time
     *
     * @return  The standard deviation for the stochastic component of the minimum connecting time
     */
    float getMctSdPercent() const {
        return this->mctSd;
    }

    /**
     * Return the passing time for IAF
     *
     * @return  The passing time for IAF in seconds
     */
    short getPtiSecs() const {
        return this->ptiMins * GARConst::SECS_PER_MIN;
    }

    /**
     * Return the runway queue policy
     *
     * @return  The runway queue policy
     */
    std::string getRwPolicy() const {
        return this->rwPolicy;
    }

    /**
     * Return the route length for determining long-haul flights
     *
     * @return  The length in kilometers for determining long-haul routes
     */
    int getLongHaulKm() const {
        return this->longHaulKm;
    }

    /**
     * Return the maximum deviation for the random component of the actual time of arrival
     *
     * @return  The maximum deviation in seconds for the random component of the
     *          actual time of arrival
     */
    short getAtaMdSecs() const {
        return this->ataMdMins * GARConst::SECS_PER_MIN;
    }

    /**
     * This private method returns the value associated with the
     * token parameter specified. If the value to be returned is not
     * found, the method returns an empty string.
     * 
     * @param  paramName The name of the parameter requested
     * @return The value associated with that parameter, or an empty string
     *         if no such parameter exists.
     */
    std::string getParamValue(const std::string & paramName) const;
    
    /**
     * Log the configuration parameters of the model
     *
     */
    void logConfiguration(CLogger * logger) const;

private:
    /** Path of the configuration file. */
    const static std::string __GAR_PARAMS_CONFIG_FILE;
    
    /**
     * Pointer to the Obfuscator class
     */
    Obfuscator * obfuscator;

    /**
     * The class uses a map to store the different parameters
     */
    std::map<std::string, std::string> paramValues;

    /**
     * Configurable parameters
     */
    std::string logKey;
    std::string geoZonesFile;
    std::string nonCatAirpts;
    float       trafficPercent;
    short       aircftSeats;
    float       aircftSeatsSd;
    short       rotationMins;
    float       rotationSd;
    short       airptGates;
    short       histIntervalMins;
    float       histPercentCap;
    short       minNominalCap;
    short       taxiMins;
    float       txiSd;
    float       txoSd;
    float       fltDurSd;
    short       mctMins;
    float       mctSd;
    short       ptiMins;
    std::string rwPolicy;
    int         longHaulKm;
    short       ataMdMins;

    /**
     * Constructor of the class. Receives as parameter the full path of
     * the file on disk from which the parameters will be read.
     *
     * @param configFilePath The path of the file from which the parameters
     *                       will be read.
     */
    ConfigParams(const std::string & configFilePath);

    /**
     * Destructor of the class.
     */
    ~ConfigParams();
};

#endif
