/*
 * GAROdMatrix.cpp
 *
 *  Created on: May 14, 2015
 *      Author: ctomas
 */

#include <GAROdMatrix.hpp>
#include <StringTokenizer.hpp>
#include <StringUtilities.hpp>
#include <TimeUtilities.hpp>

using common::StringTokenizer;
using common::StringUtilities;
using common::TimeUtilities;
using std::string;
using std::vector;
using boost::numeric::ublas::matrix;

namespace gar {

//................................................. Gets the use vehicle type indicator ...
bool GAROdMatrix::getUseVehType(void) const {
	return this->useVehType;
}


//................................................. Gets the vehicle type ...
std::string GAROdMatrix::getVehType(void) const {
	return this->vehType;
}


//................................................. Gets the beginning time ...
SUMOTime GAROdMatrix::getFromTime(void) const {
	return this->fromTime;
}


//................................................. Gets the end time ...
SUMOTime GAROdMatrix::getToTime(void) const {
	return this->toTime;
}


//................................................. Gets the multiplier factor ...
float GAROdMatrix::getFactor(void) const {
	return this->factor;
}


//................................................. Gets the beginning time ...
unsigned short GAROdMatrix::getNumTazs(void) const {
	return this->numTazs;
}


//................................................. Gets the districts (TAZs) ...
vector<string> GAROdMatrix::getTazs(void) const {
	return this->tazs;
}


//................................................. Gets the beginning time ...
boost::numeric::ublas::matrix<unsigned int> GAROdMatrix::getOdMatrix(void) const {
	return this->odMatrix;
}


//................................................. Sets the use vehicle type indicator ...
void GAROdMatrix::setUseVehType(const bool& useVehType) {
	this->useVehType = useVehType;
}


//................................................. Sets the vehicle type ...
void GAROdMatrix::setVehType(const string& vehType) {
	this->vehType = vehType;
}


//................................................. Sets the begin time ...
void GAROdMatrix::setFromTime(const SUMOTime& fromTime) {
	this->fromTime = fromTime;
}


//................................................. Sets the end time ...
void GAROdMatrix::setToTime(const SUMOTime& toTime) {
	this->toTime = toTime;
}


//................................................. Sets the multiplier factor ...
void GAROdMatrix::setFactor(const float& factor) {
	this->factor = factor;
}


//................................................. Sets the number of districts ...
void GAROdMatrix::setNumTazs(const unsigned short& numTazs) {
	this->numTazs = numTazs;
}


//................................................. Sets the district (TAZ) names ...
void GAROdMatrix::setTazs(const vector<string>& tazs) {
	this->tazs = tazs;
}


//................................................. Sets the OD matrix ...
void GAROdMatrix::setOdMatrix(const matrix<unsigned int>& odMatrix) {
	this->odMatrix = odMatrix;
}


//................................................. Sets the use vehicle type indicator ...
int GAROdMatrix::setUseVehType(const string& vHeader) {
	try {
		this->useVehType = (vHeader[2] == 'M') ? true : false;
	} catch (const std::exception& ex) {
		return 0;
	}
	return 1;
}


//................................................. Sets the from and to times ...
int GAROdMatrix::setFromToTimes(string fromTo) {
	auto findDuplicateSpaces = [] (const char& lhs, const char& rhs) { return (lhs == rhs)  &&  (lhs == ' '); };
	fromTo.erase(std::unique(fromTo.begin(), fromTo.end(), findDuplicateSpaces), fromTo.end());

	try {
		// Tokenize the line '<HH.MM>  <HH.MM>'
		StringTokenizer stkn;
		stkn.setLine(fromTo);
		stkn.setSeparators(" ");
		stkn.tokenize();

		fromTime = getSecondsTime(stkn.getNextToken());
		toTime   = getSecondsTime(stkn.getNextToken());
	} catch (const std::exception& ex) {
		return 0;
	}

	return 1;
}


//................................................. Sets the multiplier factor ...
int GAROdMatrix::setFactor(const string& factor) {
	try {
		this->factor = StringUtilities::toFloat(StringUtilities::trim(factor));
	} catch (const std::exception& ex) {
		return 0;
	}
	return 1;
}


//................................................. Sets the number of districts ...
int GAROdMatrix::setNumTazs(const std::string& numTazs) {
	try {
		this->numTazs = StringUtilities::toInt(StringUtilities::trim(numTazs));
	} catch(const std::exception& ex) {
		return 0;
	}
	return 1;
}


//................................................. Sets the district (TAZ) names ...
int GAROdMatrix::setTazs(std::string strTazs) {
	auto findDuplicateSpaces = [] (const char& lhs, const char& rhs) { return (lhs == rhs)  &&  (lhs == ' '); };
	strTazs.erase(std::unique(strTazs.begin(), strTazs.end(), findDuplicateSpaces), strTazs.end());

	try {
		StringTokenizer stkn;
		stkn.setLine(strTazs);
		stkn.setSeparators(" ");
		stkn.tokenize();

		for (int i = 0; i < stkn.getNumTokens(); i++) {
			string taz = StringUtilities::trim(stkn.getNextToken());
			tazs.push_back(taz);
		}
	} catch (const std::exception& ex) {
		return 0;
	}
	return 1;
}


//................................................. Sets the OD matrix dimensions ...
int GAROdMatrix::setOdMatrixDimensions(const unsigned short& dim) {
	try {
		odMatrix = matrix<unsigned int>(dim, dim);
	} catch (const std::exception& ex) {
		return 0;
	}
	return 1;
}


//................................................. Sets the OD matrix ...
int GAROdMatrix::setOdMatrixRow(const unsigned short& numRow, std::string odRow) {
	auto findDuplicateSpaces = [] (const char& lhs, const char& rhs) { return (lhs == rhs)  &&  (lhs == ' '); };
	odRow.erase(std::unique(odRow.begin(), odRow.end(), findDuplicateSpaces), odRow.end());

	// Check the odMatrix dimensions
	if (odMatrix.size1() == 0  &&  odMatrix.size2() == 0) {
		setOdMatrixDimensions(this->numTazs);
	}

	try {
		StringTokenizer stkn;
		stkn.setLine(odRow);
		stkn.setSeparators(" ");
		stkn.tokenize();

		for (int i = 0; i < stkn.getNumTokens(); i++) {
			unsigned int odFlow = StringUtilities::toInt(StringUtilities::trim(stkn.getNextToken()));
			odMatrix(numRow, i) = odFlow;
		}
	} catch (const std::exception& ex) {
		return 0;
	}
	return 1;
}


//................................................. Get the stringfied O/D matrix data ...
std::string GAROdMatrix::toString() const {
	return "useVehType : [" + StringUtilities::toString(useVehType)
			+ "], vehType : [" + vehType
			+ "], fromTime: [" + StringUtilities::toString<SUMOTime>(fromTime)
			+ "], toTime: [" + StringUtilities::toString<SUMOTime>(toTime)
			+ "], factor: [" + StringUtilities::toString(factor)
			+ "], numTazs: [" + StringUtilities::toString(numTazs)
			+ "], tazs: [" + StringUtilities::toString(tazs)
			+ "], odMatrix : [" + StringUtilities::toString<unsigned int>(odMatrix) + "]";
}


//................................................. Gets the time of the day in seconds ...
SUMOTime GAROdMatrix::getSecondsTime(const string& strTime) const {
	try {
		StringTokenizer stkn;
		stkn.setLine(strTime);
		stkn.setSeparators(".");
		stkn.tokenize();

		unsigned short hh = StringUtilities::toInt(StringUtilities::trim(stkn.getNextToken()));
		unsigned short mm = StringUtilities::toInt(StringUtilities::trim(stkn.getNextToken()));

		return hh * TimeUtilities::SECS_PER_HOUR + mm * TimeUtilities::SECS_PER_MIN;
	} catch (const std::exception& ex) {
		return 0;
	}
}

} /* namespace gar */
