/*
 * GAROdLoader.hpp
 *
 *  Created on: Jun 3, 2015
 *      Author: ctomas
 */

#ifndef GARODLOADER_HPP_
#define GARODLOADER_HPP_

#include <GAROdMatrix.hpp>
#include <CLogger.hpp>
#include <utils/options/OptionsCont.h>

using common::CLogger;

namespace gar {

/**
 * This enumerator is used to identify every line of the od-matrix file.
 */
enum class LineId : short { Header, VehType, FromTo, Factor, NumTazs, Tazs, MatrixRow, End };


/**
 * This class loads the OD-matrix data.
 * @see GAROdMatrix
 */
class GAROdLoader {
public:
	/**
	 * Invalidated default constructor.
	 */
	GAROdLoader(void) = delete;

	/**
	 * @brief Invalidated copy constructor.
	 * A <code>GAROdLoader</code> object can't be initialized from another object of the same type.
	 * @param other An existing <code>GAROdLoader</code> object to copy in the initialization.
	 */
	GAROdLoader(const GAROdLoader& other) = delete;

	/**
	 * Parameterized constructor.
	 * @param options	The ga-router input options.
	 * @param logger	Reference to the ga-router logger.
	 */
	GAROdLoader(const OptionsCont& options, CLogger *logger);

	/**
	 * Default virtual destructor.
	 */
	virtual ~GAROdLoader(void) = default;

	/**
	 * Get the pointer to the od-matrix data.
	 * @return	A pointer to the od-matrix data.
	 */
	std::shared_ptr<GAROdMatrix> getpOdMatrix(void) const;

	/**
	 * Set the pointer to the od-matrix data.
	 * @param pNet	A pointer to the od-matrix data.
	 */
	void setpOdMatrix(const std::shared_ptr<GAROdMatrix>& pOdMatrix);

	/**
	 * Load the od-matrix data from the OD Matrix file designated in the 'od-matrix-file' option.
	 * @return  <code>0</code> in case of successful network loading,
	 * 		 	<code>1</code> otherwise.
	 * @see GAROdMatrix
	 */
	int run(void);

	/**
	 * Log the od-matrix data.
	 */
	void logOdMatrix(void) const;

protected:
	/**
	 * Process a line within the O/D matrix data file.
	 * @param lineId	The line identifier that qualifies the type of data within the line (@see LineId).
	 * @param line		A data line within the O/D matrix data file.
	 * @return			The identifier of the next line to process.
	 */
	LineId processOdMatrixLine(const gar::LineId& lineId, const std::string& line);

	/**
	 * Checks whether the number of districts in the OD matrix data file matches
	 * the dimensions of the district name vector and the OD matrix.
	 * @return	<code>true</code> if the O/D matrix data is successfully loaded,
	 * 			<code>false</code> otherwise.
	 */
	bool checkNumDistrictsData(void) const;

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
	//! A pointer to the od-matrix data
	std::shared_ptr<GAROdMatrix> pOdMatrix;

	//! The ga-router input options
	const OptionsCont& options;

	//! Reference to the ga-router logger
	CLogger* logger;

	//! The comment character used in the OD matrix data file.
	static const std::string COMMENT_CHARACTER;
};

} /* namespace gar */

#endif /* GARODLOADER_HPP_ */
