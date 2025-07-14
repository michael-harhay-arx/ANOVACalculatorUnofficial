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

// Node in ANOVA tree
typedef struct ANOVANode
{
	char key[DATALENGTH];
	int numValues;
	double *values;
	
	int numChildren;
	struct ANOVANode **children;
} ANOVANode;

// Row in data table
typedef struct RowStruct
{
	char factors[MAXFACTORCOLS][DATALENGTH];
	char data[MAXDATACOLS][DATALENGTH];
} RowStruct;

// Factor element grouping struct
/*
typedef struct FactorElement
{
	char factorElement[32];
	double dataValues[];
} FactorMapping;
*/

//==============================================================================
// External variables

//==============================================================================
// Global functions

// Calculation process
void GetSSDataset (IN int Panel, char FactorRange[][DATALENGTH], char DataRange[][DATALENGTH], char LimitRange[][DATALENGTH], ANOVANode *TreeRoot);
void ComputeGrandMeans (RowStruct Dataset[], double GrandMeans[]);

int ComputeANOVA (ANOVANode *TreeRoot);

// ANOVANode helpers
void BuildANOVATree (IN int Panel, IN char FactorRange[][DATALENGTH], ANOVANode *CurrentRoot, int Level);
ANOVANode *CreateANOVANode (IN char Key[DATALENGTH]);
void AddNodeValue (ANOVANode *Node, IN double Value);
void AddChildNode (ANOVANode *ParentNode, ANOVANode *ChildNode);

#ifdef __cplusplus
    }
#endif

#endif  /* ndef __ANOVACalculator_H__ */
