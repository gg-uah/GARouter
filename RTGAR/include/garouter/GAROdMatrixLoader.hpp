/*
 * GAROdMatrixLoader.hpp
 *
 *  Created on: May 15, 2015
 *      Author: ctomas
 */

#ifndef GARODMATRIXLOADER_HPP_
#define GARODMATRIXLOADER_HPP_

#include <GAROdMatrix.hpp>
#include <GARNet.hpp>
#include <CLogger.hpp>

using common::CLogger;

namespace gar {

enum class LineId : short { Header, VehType, FromTo, Factor, NumTazs, Tazs, MatrixRow, End };

/**
 * This class implements the process of loading the O/D matrix
 * data from a V-format data file into a GAROdMatrix object.
 * @see GAROdMatrix
 */
class GAROdMatrixLoader {
public:
	/**
	 * Unavailable default constructor.
	 */
	GAROdMatrixLoader(void) = delete;

	/**
	 * Parameterized constructor.
	 * @param logger	Reference to the ga-router logger.
	 */
	GAROdMatrixLoader(const std::string& odmFile,
					  CLogger* logger);

	/**
	 * Default virtual constructor.
	 */
	virtual ~GAROdMatrixLoader(void) = default;

	/**
	 * Get the O/D matrix data file name.
	 * @return	The O/D matrix data file name.
	 */
	std::string getOdmFile(void) const;

	/**
	 * Get the O/D matrix data.
	 * @return	A GAROdMatrix object that comprises the O/D matrix data.
	 */
	GAROdMatrix getOdMatrix(void) const;

	/**
	 * Set the O/D matrix data file name.
	 * @param odmFile	The O/D matrix data file name.
	 */
	void setOdmFile(const std::string& odmFile);

	/**
	 * Set the O/D matrix data.
	 * @param odMatrix	A GAROdMatrix object that comprises the O/D matrix data.
	 */
	void setOdMatrix(const gar::GAROdMatrix& odMatrix);

	/**
	 * Run the loader that loads the O/D matrix data from a OD matrix data file
	 * in V format.
	 * @return	<code>1</code> if the O/D matrix data is successfully loaded,
	 * 			<code>0</code> otherwise.
	 */
	int run(void);

	/**
	 * Checks whether the number of districts in the OD matrix data file matches
	 * the dimensions of the district name vector and the OD matrix.
	 * @return	<code>true</code> if the O/D matrix data is successfully loaded,
	 * 			<code>false</code> otherwise.
	 */
	bool checkNumDistrictsData(void) const;

protected:
	/**
	 * Process a line within the O/D matrix data file.
	 * @param lineId	The line identifier that qualifies the type of data within the line (@see LineId).
	 * @param line		A data line within the O/D matrix data file.
	 * @return			The identifier of the next line to process.
	 */
	LineId processOdMatrixLine(const gar::LineId& lineId, const std::string& line);

	/**
	 * @brief Validate the OD matrix header.
	 * The OD matrix header has to match one of the next values: $V-, $VR-, $VMR and $VM-.
	 * The 'M' in the type name indicates that a vehicle type is used,
	 * the "R" that the values shall be rounded randomly (this information is not processed).
	 * @param header	The OD matrix header.
	 * @return			<code>true</code> if the header is successfully validated,
	 * 				    <code>false</code> otherwise.
	 */
	bool validateHeader(const std::string& header) const;

	/**
	 * @brief Validate the vehicle type information.
	 * @param vehType	The vehicle type information.
	 * @return			<code>true</code> if the vehicle type is successfully validated,
	 * 				    <code>false</code> otherwise.
	 */
	bool validateVehType(const std::string& vehType) const;

	/**
	 * @brief Validate the From-Time and To-Time information.
	 * @param fromTo	The From-Time and To-Time information.
	 * @return			<code>true</code> if the From-Time and To-Time information is successfully validated,
	 * 				    <code>false</code> otherwise.
	 */
	bool validateFromTo(const std::string& fromTo) const;

	/**
	 * @brief Validate the multiplier factor information.
	 * @param factor	The multiplier factor information.
	 * @return			<code>true</code> if the factor information is successfully validated,
	 * 				    <code>false</code> otherwise.
	 */
	bool validateFactor(const std::string& factor) const;

	/**
	 * Validate the number of districts (TAZs) information.
	 * @param numTazs	The number of districts information.
	 * @return			<code>true</code> if the district number information is successfully validated,
	 * 				    <code>false</code> otherwise.
	 */
	bool validateNumTazs(const std::string& numTazs) const;

	/**
	 * Validate the district (TAZs) names.
	 * @param tazs	A string enclosing the district names separated by space characters.
	 * @return		<code>true</code> if the district names are successfully validated,
	 * 				<code>false</code> otherwise.
	 */
	bool validateTazs(const std::string& tazs) const;

	/**
	 * Validate an OD matrix row.
	 * @param row	A row from the OD matrix.
	 * @return		<code>true</code> if the OD matrix row is successfully validated,
	 * 				<code>false</code> otherwise.
	 */
	bool validateMatrixRow(const std::string& row) const;

private:
	//! The O/D matrix data file name.
	std::string odmFile;

	//! The O/D matrix data.
	gar::GAROdMatrix odMatrix;

	//! Reference to the ga-router logger.
	CLogger* logger;

	//! The comment character used in the OD matrix data file.
	static const std::string COMMENT_CHARACTER;
};

} /* namespace gar */

#endif /* GARODMATRIXLOADER_HPP_ */
