/*
 * GAROdLoader.cpp
 *
 *  Created on: Jun 3, 2015
 *      Author: ctomas
 */

#include "GAROdLoader.hpp"
#include <StringUtilities.hpp>
#include <FileTokenizer.hpp>
#include <boost/regex.hpp>

using common::FileTokenizer;
using common::StringUtilities;
using gar::GAROdMatrix;
using gar::LineId;
using std::string;
using std::unique_ptr;


namespace gar {

const string GAROdLoader::COMMENT_CHARACTER ("*");

//................................................. Parameterized constructor ...
GAROdLoader::GAROdLoader(const OptionsCont& options, CLogger *logger)
: pOdMatrix (new GAROdMatrix()),
  options (options),
  logger (logger) {

	// Intentionally left empty
}


//................................................. Gets the pointer to the od-matrix data ...
std::shared_ptr<GAROdMatrix> GAROdLoader::getpOdMatrix(void) const {
	return this->pOdMatrix;
}


//................................................. Sets the pointer to the od-matrix data ...
void GAROdLoader::setpOdMatrix(const std::shared_ptr<GAROdMatrix>& pOdMatrix) {
	this->pOdMatrix = pOdMatrix;
}


//................................................. Loads the od-matrix data ...
int GAROdLoader::run(void) {
	if (!options.isSet("od-matrix-file")) {
		logger->error("O/D matrix file is not specified in the 'od-matrix-file' option");
		return 1;
	}

	LineId id = LineId::Header;

	try {
		string odmFile = options.getString("od-matrix-file");

		logger->debug("Load OD matrix data from file: [" + odmFile + "]");

		// Tokenize the OD matrix data file
		FileTokenizer ftkn;
		ftkn.setFileName(odmFile);
		ftkn.setSeparators("\n");
		ftkn.tokenize();

		// Get the lines within the OD matrix data file
		for (int i = 0; i < ftkn.getNumTokens() && id != LineId::End; i++) {
			string line = StringUtilities::trim(ftkn.getNextToken());
			if (StringUtilities::startsWith(line, COMMENT_CHARACTER)) {
				continue;
			}
			logger->debug("Load OD matrix data from line: [" + line + "]");

			// Process the OD matrix data file line
			id = processOdMatrixLine(id, line);
		}

		// Validate the number of districts data
		if (!checkNumDistrictsData()) {
			logger->error("The number of districts doesn't match the OD-Matrix data");
			return 1;
		}

		//pOdMatrix = unique_ptr<const GAROdMatrix>(new GAROdMatrix(odmLoader.getOdMatrix()));

		logger->info("O/D Matrix: " + pOdMatrix->toString());
	} catch (std::exception& ex) {
		logger->error("Fail to load OD matrix data: " + string(ex.what()));
		return 1;
	}

	return 0;
}


//................................................. Logs the od-matrix data ...
void GAROdLoader::logOdMatrix(void) const {
	logger->info("............... OD-Matrix data ...");
	logger->info("Use Vehicle Type: [" + StringUtilities::toString<bool>(pOdMatrix->getUseVehType()) + "]");
	logger->info("Vehicle type: [" + pOdMatrix->getVehType() +"]");
	logger->info("From Time - To Time: [" + StringUtilities::toString<SUMOTime>(pOdMatrix->getFromTime()) + "] - [" + StringUtilities::toString<SUMOTime>(pOdMatrix->getToTime()) + "]");
	logger->info("Factor: [" + StringUtilities::toString<float>(pOdMatrix->getFactor()) + "]");
	logger->info("Number of districts: [" + StringUtilities::toString<unsigned short>(pOdMatrix->getNumTazs()) + "]");
	logger->info("District names: [" + StringUtilities::toString(pOdMatrix->getTazs()) + "]");
	logger->info("OD Matrix:");

	boost::numeric::ublas::matrix<unsigned int> odmatrix = pOdMatrix->getOdMatrix();
	for (int i= 0; i < odmatrix.size1(); i++) {
		string file = "";
		for (int j = 0; j < odmatrix.size2(); j++) {
			string odElem = StringUtilities::toString<unsigned int>(odmatrix(i,j));
			file += StringUtilities::leftPadding(odElem, 5) + "  ";
		}
		logger->info(file);
	}
}


//................................................. Process the OD matrix line ...
LineId GAROdLoader::processOdMatrixLine(const LineId& id, const string& line) {
	static unsigned short rowNum = 0;

	if (id == LineId::Header) {
		if (!this->validateHeader(line)) {
			logger->error("Header [" + line + "] doesn't match a valid value");
			return LineId::End;
		}
		pOdMatrix->setUseVehType(line);
		return LineId::VehType;
	}

	if (id == LineId::VehType) {
		if (!this->validateVehType(line)) {
			logger->error("Vehicle type [" + line + "] doesn't match a valid value");
			return LineId::End;
		}
		pOdMatrix->setVehType(line);
		return LineId::FromTo;
	}

	if (id == LineId::FromTo) {
		if (!this->validateFromTo(line)) {
			logger->error("From and To times [" + line + "] doesn't match a valid value");
			return LineId::End;
		}
		pOdMatrix->setFromToTimes(line);
		return LineId::Factor;
	}

	if (id == LineId::Factor) {
		if (!this->validateFactor(line)) {
			logger->error("Factor [" + line + "] doesn't match a valid value");
			return LineId::End;
		}
		pOdMatrix->setFactor(line);
		return LineId::NumTazs;
	}

	if (id == LineId::NumTazs) {
		if (!this->validateNumTazs(line)) {
			logger->error("Number of districts [" + line + "] doesn't match a valid value. "
						  "Maximum number of districts is 99.");
			return LineId::End;
		}
		pOdMatrix->setNumTazs(line);
		pOdMatrix->setOdMatrixDimensions(pOdMatrix->getNumTazs());
		return LineId::Tazs;
	}

	if (id == LineId::Tazs) {
		if (!this->validateTazs(line)) {
			logger->error("District names [" + line + "] doesn't match a valid value.");
			return LineId::End;
		}
		pOdMatrix->setTazs(line);
		return LineId::MatrixRow;
	}

	if (id == LineId::MatrixRow) {
		if (!this->validateMatrixRow(line)) {
			logger->error("Matrix row [" + line + "] doesn't match a valid value.");
			return LineId::End;
		}
		pOdMatrix->setOdMatrixRow(rowNum++, line);
		return LineId::MatrixRow;
	}

	return LineId::End;
}


//................................................. Check the number of districts data ...
bool GAROdLoader::checkNumDistrictsData(void) const {
	// Check the number of districts along with the taz names size
	if (pOdMatrix->getNumTazs() != pOdMatrix->getTazs().size()) {
		logger->error("The number of districts [" + StringUtilities::toString(pOdMatrix->getNumTazs())
				+ "] doesn't match the number of district names [" + StringUtilities::toString(pOdMatrix->getTazs()) + "]");
		return false;
	}

	// Check the number of districts along with the OD matrix dimensions
	if (pOdMatrix->getNumTazs() != pOdMatrix->getOdMatrix().size1()  ||  pOdMatrix->getNumTazs() != pOdMatrix->getOdMatrix().size2()) {
		logger->error("The number of districts [" + StringUtilities::toString(pOdMatrix->getNumTazs())
				+ "] doesn't match the dimensions of the OD matrix [" + StringUtilities::toString<unsigned int>(pOdMatrix->getOdMatrix()) + "]");
		return false;
	}

	return true;
}


//................................................. Validate the vehicle type ...
bool GAROdLoader::validateHeader(const std::string& header) const {
	static const boost::regex regEx("\\$V-|\\$VR-|\\$VMR|\\$VM-");
	return boost::regex_match(header, regEx);
}


//................................................. Validate the vehicle type ...
bool GAROdLoader::validateVehType(const std::string& vehType) const {
	static const boost::regex regEx("^[\\w,\\s-]+$");
	return boost::regex_match(vehType, regEx);
}


//................................................. Validate the from and to times ...
bool GAROdLoader::validateFromTo(const std::string& fromTo) const {
	static const boost::regex regEx("^([01]?[0-9]|2[0-3])\\.[0-5][0-9]\\s+([01]?[0-9]|2[0-3])\\.[0-5][0-9]$");
	return boost::regex_match(fromTo, regEx);
}


//................................................. Validate the factor ...
bool GAROdLoader::validateFactor(const std::string& factor) const {
	static const boost::regex regEx("^[0-9]*\.?[0-9]*$");
	return boost::regex_match(factor, regEx);
}


//................................................. Validate the number of districts ...
bool GAROdLoader::validateNumTazs(const std::string& numTazs) const {
	static const boost::regex regEx("^[0-9]{1,2}$");
	return boost::regex_match(numTazs, regEx);
}


//................................................. Validate the district names ...
bool GAROdLoader::validateTazs(const std::string& tazs) const {
	static const boost::regex regEx("^[\\w\\s]+$");
	return boost::regex_match(tazs, regEx);
}


//................................................. Validate a row from the OD matrix ...
bool GAROdLoader::validateMatrixRow(const std::string& row) const {
	static const boost::regex regEx("^[\\d\\s]+$");
	return boost::regex_match(row, regEx);
}

} /* namespace gar */
