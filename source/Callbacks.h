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
#include "ANOVACalculatorDefinitions.h"
		
#include "ArxtronToolslib.h"

//==============================================================================
// Constants

//==============================================================================
// Types
		
typedef struct SaveData
{	
	int numRows;
	int numCols;
	int numFactorCols;
	int numDataCols;
	char csvFilepath[512];
	char factorRange[MAXFACTORCOLS][DATALENGTH];
	char dataRange[MAXDATACOLS][DATALENGTH];
	char limitRange[MAXDATACOLS][DATALENGTH];
	
	ANOVAResult anovaResult;
} SaveData;
		
//==============================================================================
// External variables
		
extern char ***glbCSVData;
extern int glbNumRows;
extern int glbNumFactorCols;
extern int glbNumDataCols;

//==============================================================================
// Global functions

void GetDataFromListBoxes (IN int panel, char FactorSelection[][DATALENGTH], char DataSelection[][DATALENGTH], char LimitSelection[][DATALENGTH]);
void GetANOVATableRowName (IN int RowNum, char *RowName);
int DisplayCSVTable ();
void DisplayANOVATable ();

#ifdef __cplusplus
    }
#endif

#endif  /* ndef __Callbacks_H__ */
