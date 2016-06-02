#ifndef __RANDOM_UTILITIES_HPP
#define __RANDOM_UTILITIES_HPP

#include <stdexcept>
#include <vector>
#include <random>

namespace common {

/**
 * This class provides several common utilities for generating random numbers.
 */
class RandomUtilities {

public:
    
    /**
     * Initialize the random seed with a unique number
     */
    static void initRandomSeed();

    /**
     * Return a uniformly distribuited random integer in a range
     * between a floor and a ceiling value
     *
     * @param floor     Minimum value of the random range
     * @param ceiling   Maximum value of the random range
     * @return          A random integer between floor and ceiling
     */
    static int getUniformRandomInteger(int floor, int ceiling)
        throw(std::range_error);

    /**
     * Return a normally distributed random double value using a gaussian distribution
     *
     * @param mean  Mean of the normal distribution
     * @param sd    Standard deviation of the normal distribution
     * @return      A random double value following the normal distribution
     */
    static double getNormalRandomDouble(double mean, double sd);

    /**
     * Return a normally distributed random absolute double value using a gaussian distribution
     *
     * @param mean  Mean of the normal distribution
     * @param sd    Standard deviation of the normal distribution
     * @return      A random absolute double value following the normal distribution
     */
    static double getAbsNormalRandomDouble(double mean, double sd);

    /**
     * Return a normally distributed random integer value using a gaussian distribution
     *
     * @param mean  Mean of the normal distribution
     * @param sd    Standard deviation of the normal distribution
     * @return      A random integer value following the normal distribution
     */
    static int getNormalRandomInteger(double mean, double sd);

    /**
     * Return a normally distributed random absolute integer value using a gaussian distribution
     *
     * @param mean  Mean of the normal distribution
     * @param sd    Standard deviation of the normal distribution
     * @return      A random absolute integer value following the normal distribution
     */
    static unsigned int getAbsNormalRandomInteger(double mean, double sd);

    /**
     * Plot a normal distribution
     *
     * @param mean  Mean of the normal distribution
     * @param sd    Standard deviation of the normal distribution
     */
    static void plotNormalDistribution(double mean, double sd);

    /**
     * Creates a random number distribution that produces double values according to a normal (gaussian) distribution.
     * This distribution produces random numbers around the distribution mean with a specific standard deviaiton.
     * @param mean	The mean of the normal distribution.
     * @param sd	The standard deviation of the normal distribution.
     * @return		A normal distribution according to the specified mean and standard deviation.
     */
    static std::normal_distribution<> normalDistribution(double mean, double sd);

    /**
     * Generates a random double number that follows the distribution's parameters associated
     * to the normal distribution.
     * @param dist	The normal distribution from which the random numbers are generated.
     * @return		A random double number.
     */
    static double getNormalRandomDouble(std::normal_distribution<>& dist);

    /**
     * Gets the absolute value of a generated random double number following a normal distribution.
     * @param dist	A normal distribution from which the random double numbers are generated.
     * @return		The absolute value of a random double number.
     */
    static double getAbsNormalRandomDouble(std::normal_distribution<>& dist);

    /**
     * Gets a random integer number than follows the distribution's parameters associated
     * to the normal distribution.
     * @param dist	The normal distribution from which the random integer numbers are generated.
     * @return		A random integer number.
     */
    static int getNormalRandomInteger(std::normal_distribution<>& dist);

    /**
     * Gets the absolute value of a generated random integer number following a normal distribution.
     * @param dist	A normal distribution from which the random integer numbers are generated.
     * @return		The absolute value of a random integer number.
     */
    static unsigned int getAbsNormalRandomInteger(std::normal_distribution<>& dist);

    /**
     * Get a random double number between a minimum and maximum boundaries that follows the distribution's
     * parameters associated to the normal distribution.
     * @param dist	The normal distribution from which the random double numbers are generated.
     * @param min	The minimum value of the random number generation.
     * @param max	The maximum value of the random number generation.
     * @return		A random double number.
     */
    static double getNormalRandomDoubleBetweenBoundaries(std::normal_distribution<>& dist, double min, double max);

    /**
     * Get a random integer number between a minimum and maximum boundaries that follows the distribution's
     * parameters associated to the normal distribution.
     * @param dist	The normal distribution from which the random integer numbers are generated.
     * @param min	The minimum value of the random number generation.
     * @param max	The maximum value of the random number generation.
     * @return		A random integer number.
     */
    static int getNormalRandomIntegerBetweenBoundaries(std::normal_distribution<>& dist, int min, int max);

};
 
} /* namespace */
#endif
