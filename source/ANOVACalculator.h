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
	char factorCombos[10][DATALENGTH]; // Maximum 8 actual factor combos + equipment + total
	char dataColumns[MAXDATACOLS][DATALENGTH];
	
	double ssResults[10][MAXDATACOLS];
	int degFrd[10][MAXDATACOLS];
	double variance[10][MAXDATACOLS];
	double stdDev[10][MAXDATACOLS];
	double ptRatio[10][MAXDATACOLS];
} ANOVAResult;

//==============================================================================
// External variables

extern int glbNumMasks;
extern ANOVAResult glbANOVAResult;

//==============================================================================
// Global functions

// Calculation process
void GetSSDataset (IN int Panel, char FactorRange[][DATALENGTH], char DataRange[][DATALENGTH], char LimitRange[][DATALENGTH]);

// Helpers
void ComputeGrandMeans (RowStruct Dataset[], double GrandMeans[]);
void ComputeTotalSS (RowStruct Dataset[], double GrandMeans[], double SSTotal[]);
int matchOnMask (RowStruct RowA, RowStruct RowB, int Mask);
void ComputeSSFactorCombo (RowStruct Dataset[], IN int Mask, double *GrandMeans, double *ssOut);
void getFactorComboName (IN RowStruct Dataset[], IN int Mask, char *FactorComboName);

int ComputeANOVA ();

#ifdef __cplusplus
    }
#endif

#endif  /* ndef __ANOVACalculator_H__ */
