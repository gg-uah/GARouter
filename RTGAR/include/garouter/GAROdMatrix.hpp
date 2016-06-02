/*
 * GAROdMatrix.hpp
 *
 *  Created on: May 14, 2015
 *      Author: ctomas
 */

#ifndef GARODMATRIX_HPP_
#define GARODMATRIX_HPP_

#include <CLogger.hpp>
#include <utils/common/SUMOTime.h>
#include <boost/numeric/ublas/matrix.hpp>

using common::CLogger;

namespace gar {

/**
 * @brief This class comprises the data within an O/D matrix file.
 * An O/D matrix describes a certain time period. The indices within the matrix are names
 * of the origin/destination districts (normally they are equivalent, both lists are the same).
 * The values stored within the matrix are amounts of vehicles driving from the according origin
 * district to the according destination district within the described time period.
 * The O/D matrix file looks like follows:
 * $VMR
 * * Vehicle type
 * pass
 * * From-Time  To-Time
 * 15.00        15.10
 * *
 * * Factor
 * 1.00
 * *
 * * Number of districts
 * 4
 * *
 * * District names
 *         TAZ1        TAZ2        TAZ3        TAZ4
 * * Amount of vehicles that leave the district TAZ1
 *            0          10          10          10
 * * Amount of vehicles that leave the district TAZ2
 *           10           0          10          10
 * * Amount of vehicles that leave the district TAZ3
 *           10          10           0          10
 * * Amount of vehicles that leave the district TAZ4
 *           10          10          10           0
 */
class GAROdMatrix {
public:
	/**
	 * Default empty constructor.
	 */
	GAROdMatrix(void) = default;

	/**
	 * Default copy constructor.
	 * @param other	The GAROdMatrix object the data is copied from when creating a new object.
	 */
	GAROdMatrix(const GAROdMatrix& other) = default;

	/**
	 * The default virtual destructor.
	 */
	virtual ~GAROdMatrix(void) = default;

	/**
	 * Get the indicator meaning that a vehicle type is used.
	 * @return	<code>true</code> if a vehicle type is used in the traffic flow,
	 * 			<code>false</code> otherwise
	 */
	bool getUseVehType(void) const;

	/**
	 * Get the vehicle type used in the traffic flow.
	 * @return	The vehicle type.
	 */
	std::string getVehType(void) const;

	/**
	 * Get the beginning time in seconds of the time period the OD matrix describes.
	 * @return	The beginning time of the day in seconds.
	 */
	SUMOTime getFromTime(void) const;

	/**
	 * Get the end time in seconds of the time period the OD matrix describes.
	 * @return	The end time of the day in seconds.
	 */
	SUMOTime getToTime(void) const;

	/**
	 * Get the multiplier factor applied to the flow amounts in the OD matrix.
	 * @return	The multiplier factor applied to the flow amounts.
	 */
	float getFactor(void) const;

	/**
	 * Get the district (TAZ) number involved in the OD matrix.
	 * @return	The district (TAZ) number.
	 */
	unsigned short getNumTazs(void) const;

	/**
	 * Get the district (TAZ) names involved in the OD matrix.
	 * @return	A vector that comprises the district (TAZ) names.
	 */
	std::vector<std::string> getTazs(void) const;

	/**
	 * @brief Get the OD matrix.
	 * The values stored within the OD matrix are amounts of vehicles driving from the
	 * according origin district (origin TAZ) to the according destination district (destination TAZ)
	 * within the described time period.
	 * @return	The OD matrix.
	 */
	boost::numeric::ublas::matrix<unsigned int> getOdMatrix(void) const;

	/**
	 * Set the indicator meaning that a vehicle type is used.
	 * @param useVehType  <code>true</code> if a vehicle type is used in the traffic flow,
	 * 					  <code>false</code> otherwise
	 */
	void setUseVehType(const bool& useVehType);

	/**
	 * Set the vehicle type used in the traffic flow.
	 * @param vehType	The vehicle type used in the traffic flow.
	 */
	void setVehType(const std::string& vehType);

	/**
	 * Set the beginning time in seconds of the time period the OD matrix describes.
	 * @param begin	 The beginning time of the day in seconds.
	 */
	void setFromTime(const SUMOTime& begin);

	/**
	 * Set the end time in seconds of the time period the OD matrix describes.
	 * @param end	 The end time of the day in seconds.
	 */
	void setToTime(const SUMOTime& end);

	/**
	 * Set the multiplier factor applied to the flow amounts in the OD matrix.
	 * @param factor	The multiplier factor applied to the flow amounts.
	 */
	void setFactor(const float& factor);

	/**
	 * Set the number of districts (TAZs).
	 * @param numTazs	The number of districts.
	 */
	void setNumTazs(const unsigned short& numTazs);

	/**
	 * Set the district (TAZ) names involved in the OD matrix.
	 * @param tazs  A vector that contains the district (TAZ) names involved in the OD matrix.
	 */
	void setTazs(const std::vector<std::string>& tazs);

	/**
	 * @brief Set the OD matrix.
	 * The values stored within the OD matrix are amounts of vehicles driving from the
	 * according origin district (origin TAZ) to the according destination district (destination TAZ)
	 * within the described time period.
	 * @param odMatrix	The OD matrix.
	 */
	void setOdMatrix(const boost::numeric::ublas::matrix<unsigned int>& odMatrix);

	/**
	 * Set the indicator meaning that a vehicle type is used.
	 * @param vHeader  The header of the OD matrix data file.
	 * @return			<code>1</code> if the use vehicle type indicator is successfully set,
	 * 					<code>0</code> otherwise
	 */
	int setUseVehType(const std::string& vHeader);

	/**
	 * Sets the from and to times from the "From-Time To-Time" line in the OD matrix data file.
	 * @param fromTo	The "From-Time To-Time" line in the OD matrix data file.
	 * @return			<code>1</code> if the from and to times are successfully set,
	 * 					<code>0</code> otherwise
	 */
	int setFromToTimes(std::string fromTo);

	/**
	 * Sets the multiplier factor applied to the flow amounts in the OD matrix.
	 * @param factor	A string that reports the multiplier factor applied
	 * 					to the flow amounts.
	 * @return			<code>1</code> if the multiplier factor is successfully set,
	 * 					<code>0</code> otherwise
	 */
	int setFactor(const std::string& factor);

	/**
	 * Sets the number of districts (TAZs).
	 * @param numTazs	The number of districts.
	 * @return			<code>1</code> if the number of districts is successfully set,
	 * 					<code>0</code> otherwise
	 */
	int setNumTazs(const std::string& numTazs);

	/**
	 * Sets the district (TAZ) names involved in the OD matrix.
	 * @param tazs  A string that contains the district (TAZ) names involved in the OD matrix.
	 * @return		<code>1</code> if the district names are successfully set,
	 * 				<code>0</code> otherwise
	 */
	int setTazs(std::string strTazs);

	/**
	 * Sets the dimensions of the O/D matrix.
	 * @param dim	The dimension of the O/D matrix.
	 * @return		<code>1</code> if the O/D matrix dimensions are successfully set,
	 * 				<code>0</code> otherwise
	 */
	int setOdMatrixDimensions(const unsigned short& dim);

	/**
	 * Sets an O/D matrix row.
	 * @param numRow	The row number.
	 * @param odRow		The O/D matrix row data.
	 * @return			<code>1</code> if the O/D matrix row is successfully set,
	 * 					<code>0</code> otherwise
	 */
	int setOdMatrixRow(const unsigned short& numRow, std::string odRow);

	/**
	 * Converts the O/D matrix data into a string.
	 * @return	A string describing the O/D matrix data.
	 */
	std::string toString() const;

protected:
	/**
	 * Get the time of the day in seconds from a time string in format <HH.MM>.
	 * @param strTime	A string that reports the time of the day in format <HH.MM>.
	 * @return			The time of the day in seconds.
	 */
	SUMOTime getSecondsTime(const std::string& strTime) const;

private:
	//! Indicates that a vehicle type is used
	bool useVehType;

	//! The vehicle type
	std::string vehType;

	//! The beginning time in seconds
	SUMOTime fromTime;

	//! The end time in seconds
	SUMOTime toTime;

	//! The vehicle amount multiplier
	float factor;

	//! The district number
	unsigned short numTazs;

	//! The traffic analysis zones (districts) in the network
	std::vector<std::string> tazs;

	//! The O/D matrix
	boost::numeric::ublas::matrix<unsigned int> odMatrix;

};

} /* namespace gar */

#endif /* GARODMATRIX_HPP_ */
