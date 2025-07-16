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

// ANOVA result data
typedef struct ANOVAResult
{
	int numRows;
	char factorCombos[MAXFACTORCOMBOS][DATALENGTH];
	char dataColumns[MAXDATACOLS][DATALENGTH];
	
	double ssResults[MAXFACTORCOMBOS][MAXDATACOLS];
	double ssResultsRepeat[MAXFACTORCOMBOS][MAXDATACOLS];
	int degFrd[MAXFACTORCOMBOS];
	int degFrdRepeat[MAXFACTORCOMBOS];
	double variance[MAXFACTORCOMBOS][MAXDATACOLS];
	double varianceRepeat[MAXFACTORCOMBOS][MAXDATACOLS];
	double stdDev[MAXFACTORCOMBOS][MAXDATACOLS];
	double stdDevRepeat[MAXFACTORCOMBOS][MAXDATACOLS];
	double stdDevOverall[MAXFACTORCOMBOS][MAXDATACOLS];
	double ptRatio[MAXFACTORCOMBOS][MAXDATACOLS];
	double ptRatioRepeat[MAXFACTORCOMBOS][MAXDATACOLS];
	double ptRatioOverall[MAXFACTORCOMBOS][MAXDATACOLS];
} ANOVAResult;

//==============================================================================
// External variables

extern int glbNumMasks;
extern ANOVAResult glbANOVAResult;

//==============================================================================
// Global functions

// Main calc function
void ComputeANOVA (IN int Panel, char FactorRange[][DATALENGTH], char DataRange[][DATALENGTH], char LimitRange[][DATALENGTH]);

// Helpers
void ComputeGrandMeans (RowStruct Dataset[], double GrandMeans[]);
void ComputeTotalSS (RowStruct Dataset[], double GrandMeans[], double SSTotal[]);
int MatchOnMask (RowStruct RowA, RowStruct RowB, int Mask);
void ComputeSSFactorCombo (RowStruct Dataset[], IN int Mask, double *GrandMeans, double *SSOut, double *SSOutRepeat);
void GetFactorComboName (IN RowStruct Dataset[], IN int Mask, char *FactorComboName);
void ComputeNumUniqueFactorElements(RowStruct Dataset[]);

// Stats calculations
void ComputeDegreesFreedom ();
void ComputeVariance ();
void ComputeStdDev ();
void ComputePTRatio (double LimitList[][2]);

#ifdef __cplusplus
    }
#endif

#endif  /* ndef __ANOVACalculator_H__ */
