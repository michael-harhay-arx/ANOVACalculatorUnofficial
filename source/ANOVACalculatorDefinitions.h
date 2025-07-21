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
		
#define DATALENGTH 			(32)
#define MAXFACTORCOLS		(3)
#define MAXDATACOLS			(16)
#define NUMOVERVIEWROWS		(10) // overview display
#define NUMINTERMEDROWS		(3) // SS, Df, MS (intermediate display)
#define MAXFACTORCOMBOS 	((1 << MAXFACTORCOLS) + 1) // Maximum 8 actual factor combos + equipment + total
		
//==============================================================================
// Types
		
// ANOVA result data
typedef struct ANOVAResult
{
	int numRows;
	char rowLabels[MAXFACTORCOMBOS][DATALENGTH];
	char colLabels[MAXDATACOLS][DATALENGTH];
	
	// Intermediate results
	double sumSqr[MAXFACTORCOMBOS][MAXDATACOLS];
	int degFrd[MAXFACTORCOMBOS];
	double meanSqr[MAXFACTORCOMBOS][MAXDATACOLS];
	double variance[MAXFACTORCOMBOS][MAXDATACOLS];
	
	double sumSqrRepeat[MAXDATACOLS];
	int degFrdRepeat;
	double meanSqrRepeat[MAXDATACOLS];
	
	// ANOVA overview results
	double varianceReprod[MAXDATACOLS];
	double varianceRepeat[MAXDATACOLS];
	double varianceTotal[MAXDATACOLS];
	double stdDevReprod[MAXDATACOLS];
	double stdDevRepeat[MAXDATACOLS];
	double stdDevTotal[MAXDATACOLS];
	double ptRatioReprod[MAXDATACOLS];
	double ptRatioRepeat[MAXDATACOLS];
	double ptRatioTotal[MAXDATACOLS];
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
