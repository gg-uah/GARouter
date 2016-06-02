#include <RandomUtilities.hpp>
#include <iostream>
#include <iomanip>
#include <cmath>
#include <stdlib.h>
#include <time.h>
#include <stdlib.h>
#include <limits>

using namespace std;


namespace common {

//........................................... Initialize the random seed ...
void RandomUtilities::initRandomSeed() {
    //Initialize the random seed with a unique number like actual time
    srand((unsigned)time(NULL));
}


//........................................... Return a uniformly distributed random integer ...
int RandomUtilities::getUniformRandomInteger(int minval, int maxval)
    throw(range_error) {

    if (maxval < minval) {
        range_error ex("Ceiling limit is less than floor limit");
        throw ex;
    }

    //Return a random number between floor and ceiling
    return minval + rand() % (maxval + 1 - minval);
}


//........................................... Return a normally distributed random double value ...
double RandomUtilities::getNormalRandomDouble(double mean = 0.0, double sd = 1.0) {
    static bool deviateAvailable = false; // flag
    static float storedDeviate;           // deviate from previous calculation
    double dist, angle;

    //If no deviate has been stored, the standard Box-Muller transformation is
    //performed, producing two independent normally-distributed random
    //deviates.  One is stored for the next round, and one is returned.
    if (!deviateAvailable) {
        //choose a pair of uniformly distributed deviates, one for the
        //distance and one for the angle, and perform transformations
        dist = sqrt(-2.0 * log(double(rand()) / double(RAND_MAX)));
        angle = 2.0 * M_PI * (double(rand()) / double(RAND_MAX));

        //calculate and store first deviate and set flag
        storedDeviate = dist * cos(angle);
        deviateAvailable = true;

        //calculate return second deviate
        return dist * sin(angle) * sd + mean;
    } else {
        //If a deviate is available from a previous call to this function, it is
        //returned, and the flag is set to false.
        deviateAvailable = false;
        return storedDeviate * sd + mean;
    }
}


//........................................... Return a normally distributed random absolute double value ...
double RandomUtilities::getAbsNormalRandomDouble(double mean = 0.0, double sd = 1.0) {
    return abs(RandomUtilities::getNormalRandomDouble(mean, sd));
}


//........................................... Return a normally distributed random integer ...
int RandomUtilities::getNormalRandomInteger(double mean = 0.0, double sd = 1.0) {
    return round(RandomUtilities::getNormalRandomDouble(mean, sd));
}


//........................................... Return a normally distributed random absolute integer ...
unsigned int RandomUtilities::getAbsNormalRandomInteger(double mean = 0.0, double sd = 1.0) {
    return abs(RandomUtilities::getNormalRandomInteger(mean, sd));
}


//........................................... Plot a normal distribution ...
void RandomUtilities::plotNormalDistribution(double mean, double sd) {
    const int nrolls = 100000;  // number of experiments
    const int nstars = 500;    // maximum number of stars to distribute

    const int rng = int(2*mean + sd);
    int * p = new int[rng];

    for(int i = 0; i < rng; i++) {
        p[i] = 0;
    }

    RandomUtilities::initRandomSeed();

    for (int i = 0; i < nrolls ; i++) {
        double number = RandomUtilities::getNormalRandomDouble(mean, sd);
        if(number >= 0.0  && number <= (double(rng) - sd)) {
            ++p[int(number)];
        }
    }

    std::cout << "Normal Distribution (" << mean << "," << sd << "):" << std::endl;
    for (int i = 0; i < rng; ++i) {
        std::cout << std::setiosflags(std::ios::fixed)
                  << std::setw(2)
                  << std::right
                  << i << "-"
                  << std::setw(2)
                  << std::right
                  << (i + 1) << ": ";
        std::cout << std::string(p[i]*nstars/nrolls, '*') << std::endl;
    }

    delete p;
}


//........................................... Creates a normal random number distribution ...
std::normal_distribution<> RandomUtilities::normalDistribution(double mean = 0.0, double sd = 1.0) {
	std::normal_distribution<> dist(mean, sd);
	return dist;
}


//........................................... Gets a random double number from a normal distribution ...
double RandomUtilities::getNormalRandomDouble(std::normal_distribution<>& dist) {
	std::random_device rd;
	std::mt19937 mt(rd());

	return double(dist(mt));
}


//........................................... Gets the absolute value of a random double number from a normal distribution ...
double RandomUtilities::getAbsNormalRandomDouble(std::normal_distribution<>& dist) {
	return fabs(getNormalRandomDouble(dist));
}


//........................................... Gets a random integer number from a normal distribution ...
int RandomUtilities::getNormalRandomInteger(std::normal_distribution<>& dist) {
	return round(getNormalRandomDouble(dist));
}


//........................................... Gets the absolute value of a random integer number from a normal distribution ...
unsigned int RandomUtilities::getAbsNormalRandomInteger(std::normal_distribution<>& dist) {
	return abs(getNormalRandomInteger(dist));
}


//........................................... Gets a random double number between a minimum a maximum values ...
double RandomUtilities::getNormalRandomDoubleBetweenBoundaries(std::normal_distribution<>& dist, double min, double max) {
	double num;

	if (min > max) {
		std::cerr << "Minimum boundary [" << min << "] is greater than the maximum boundary [" << max << "]" << std::endl;
		return std::numeric_limits<double>::quiet_NaN();
	}

	do {
		num = getNormalRandomDouble(dist);
	} while (num < min  ||  num > max);

	return num;
}

//........................................... Gets a random integer number between a minimum a maximum values ...
int RandomUtilities::getNormalRandomIntegerBetweenBoundaries(std::normal_distribution<>& dist, int min, int max) {
	int num;

	if (min > max) {
		std::cerr << "Minimum boundary [" << min << "] is greater than the maximum boundary [" << max << "]" << std::endl;
		return std::numeric_limits<int>::quiet_NaN();
	}

	do {
		num = getNormalRandomInteger(dist);
	} while (num < min  ||  num > max);

	return num;
}

} /* namespace common */
