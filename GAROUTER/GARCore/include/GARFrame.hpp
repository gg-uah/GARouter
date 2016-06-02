/****************************************************************************/
/// @file    GARFrame.h
/// @author  Daniel Krajzewicz
/// @author  Eric Nicolay
/// @author  Michael Behrisch
/// @date    Thu, 16.03.2006
/// @version $Id: GARFrame.h 17235 2014-11-03 10:53:02Z behrisch $
///
// Sets and checks options for df-routing
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.dlr.de/
// Copyright (C) 2001-2014 DLR (http://www.dlr.de/) and contributors
/****************************************************************************/
//
//   This file is part of SUMO.
//   SUMO is free software: you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation, either version 3 of the License, or
//   (at your option) any later version.
//
/****************************************************************************/
#ifndef GARFrame_h
#define GARFrame_h

// ===========================================================================
// included modules
// ===========================================================================
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif


// ===========================================================================
// class definitions
// ===========================================================================
/**
 * @class GARFrame
 * @brief Sets and checks options for df-routing
 */
class GARFrame {
public:
	/** @brief Inserts options used by dfrouter into the OptionsCont-singleton
	 */
	static void fillOptions();

	/** @brief Checks set options from the OptionsCont-singleton for being valid for usage within dfrouter
	 *
	 * @return Whether all needed options are set
	 * @todo Unused currently; repair/fill
	 */
	static bool checkOptions();

};

#endif

/****************************************************************************/

