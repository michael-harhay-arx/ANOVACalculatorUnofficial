/***************************************************************************//*!
* \file ANOVACalculator.h
* \author 
* \copyright . All Rights Reserved.
* \date 2025-07-09 4:25:52 PM
*******************************************************************************/

#ifndef __ANOVACalculator_H__
#define __ANOVACalculator_H__

#ifdef __cplusplus
    extern "C" {
#endif

//==============================================================================
// Include files

#include "cvidef.h"
		
#include "Callbacks.h"
#include "ANOVACalculatorDefinitions.h"
		
#include "ArxtronToolslib.h"

//==============================================================================
// Constants

//==============================================================================
// Types

// Row in data table
typedef struct RowStruct
{
	char factors[MAXFACTORCOLS][DATALENGTH];
	char data[MAXDATACOLS][DATALENGTH];
} RowStruct;

//==============================================================================
// External variables

extern glbNumFactorCols;
extern glbNumDataCols;
extern ANOVAResult glbANOVAResult;

//==============================================================================
// Global functions

// Main calc function
void ComputeANOVA (IN int Panel, char FactorRange[][DATALENGTH], char DataRange[][DATALENGTH], char LimitRange[][DATALENGTH]);

// Helpers
void ComputeGrandMeans (RowStruct Dataset[], double GrandMeans[]);
int MatchOnMask (RowStruct RowA, RowStruct RowB, int Mask);
void ComputeSS (RowStruct Dataset[], IN int Mask, double GroupMeans[MAXFACTORCOMBOS][MAXDATACOLS], double *GrandMeans, double *SSOut, double *SSOutRepeat);
void GetFactorComboName (IN RowStruct Dataset[], IN int Mask, char *FactorComboName);
void ComputeNumUniqueFactorElements(RowStruct Dataset[]);

// Stats calculations
void ComputeDegreesFreedom (RowStruct Dataset[]);
void ComputeMeanSquare ();
int ComputeVarianceDenom (int Mask);
void ComputeVariance ();
void ComputeStdDev ();
void ComputePTRatio (double LimitList[][2]);

#ifdef __cplusplus
    }
#endif

#endif  /* ndef __ANOVACalculator_H__ */
