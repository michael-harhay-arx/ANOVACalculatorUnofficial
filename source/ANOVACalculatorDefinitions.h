/***************************************************************************//*!
* \file ANOVACalculatorDefinitions.h
* \author 
* \copyright . All Rights Reserved.
* \date 2025-07-14 11:54:19 AM
*******************************************************************************/

#ifndef __ANOVACalculatorDefinitions_H__
#define __ANOVACalculatorDefinitions_H__

#ifdef __cplusplus
    extern "C" {
#endif

//==============================================================================
// Include files

#include "cvidef.h"

//==============================================================================
// Constants
		
#define DATALENGTH 		(32)
#define MAXFACTORCOLS	(3)
#define MAXDATACOLS		(16)
#define NUMDISPLAYROWS	(12)
#define MAXFACTORCOMBOS ((1 << MAXFACTORCOLS) + 1) // Maximum 8 actual factor combos + equipment + total
		
//==============================================================================
// Types
		
// ANOVA result data
typedef struct ANOVAResult
{
	int numRows;
	char factorCombos[MAXFACTORCOMBOS][DATALENGTH];
	char dataColumns[MAXDATACOLS][DATALENGTH];
	
	// Main results
	double sumSqr[MAXFACTORCOMBOS][MAXDATACOLS];
	int degFrd[MAXFACTORCOMBOS];
	double variance[MAXFACTORCOMBOS][MAXDATACOLS];
	double stdDev[MAXFACTORCOMBOS][MAXDATACOLS];
	double ptRatio[MAXFACTORCOMBOS][MAXDATACOLS];
	
	// Equipment
	double sumSqrRepeat[MAXFACTORCOMBOS][MAXDATACOLS];
	int degFrdRepeat[MAXFACTORCOMBOS];
	double varianceRepeat[MAXFACTORCOMBOS][MAXDATACOLS];
	double stdDevRepeat[MAXFACTORCOMBOS][MAXDATACOLS];
	double ptRatioRepeat[MAXFACTORCOMBOS][MAXDATACOLS];
	
	// Total
	double varianceTotal[MAXFACTORCOMBOS][MAXDATACOLS];
	double stdDevTotal[MAXFACTORCOMBOS][MAXDATACOLS];
	double ptRatioTotal[MAXFACTORCOMBOS][MAXDATACOLS];
} ANOVAResult;

//==============================================================================
// External variables

//==============================================================================
// Global functions

int Declare_Your_Functions_Here (int x);

#ifdef __cplusplus
    }
#endif

#endif  /* ndef __ANOVACalculatorDefinitions_H__ */
