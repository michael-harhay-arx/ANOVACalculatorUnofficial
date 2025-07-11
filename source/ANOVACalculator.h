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
#include "ArxtronToolslib.h"

//==============================================================================
// Constants

//==============================================================================
// Types

// Node in ANOVA tree
typedef struct ANOVANode
{
	char key[32];
	int numValues;
	double *values;
	
	int numChildren;
	struct ANOVANode **children;
} ANOVANode;

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
void ParseCSVSelection (IN int Panel, char FactorRange[][32], char DataRange[][32], char LimitRange[][32], ANOVANode *TreeRoot);
void BuildANOVATree (IN int Panel, IN char FactorRange[][32], ANOVANode *CurrentRoot, int Level);
int ComputeANOVA (ANOVANode *TreeRoot);

// ANOVANode helpers
ANOVANode *CreateANOVANode (IN char Key[32]);
void AddNodeValue (ANOVANode *Node, IN double Value);
void AddChildNode (ANOVANode *ParentNode, ANOVANode *ChildNode);

#ifdef __cplusplus
    }
#endif

#endif  /* ndef __ANOVACalculator_H__ */
