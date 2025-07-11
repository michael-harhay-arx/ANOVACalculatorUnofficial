/***************************************************************************//*!
* \file Callbacks.h
* \author 
* \copyright . All Rights Reserved.
* \date 2025-07-09 11:44:39 AM
*******************************************************************************/

#ifndef __Callbacks_H__
#define __Callbacks_H__

#ifdef __cplusplus
    extern "C" {
#endif

//==============================================================================
// Include files

#include "cvidef.h"
#include "MainPanel.h"
#include "CSVPanel.h"
#include "ANOVAPanel.h"
#include "ANOVACalculator.h"
#include "ArxtronToolslib.h"

//==============================================================================
// Constants

//==============================================================================
// Types
		
//==============================================================================
// External variables

//==============================================================================
// Global functions

void GetDataFromListBoxes (IN int panel, char **FactorSelection, char **DataSelection, char **LimitSelection);

#ifdef __cplusplus
    }
#endif

#endif  /* ndef __Callbacks_H__ */
