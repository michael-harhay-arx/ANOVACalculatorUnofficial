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
	ANOVANode **children;
} ANOVANode;

//==============================================================================
// External variables

//==============================================================================
// Global functions

// Calculation process
int ParseCSVSelection ();
int ComputeANOVA ();

// ANOVANode helpers
ANOVANode *CreateANOVANode (IN char Key[32])
void AddNodeValue (ANOVANode *Node, IN double Value);
void AddChildNode (ANOVANode *ParentNode, ANOVANode *ChildNode);

#ifdef __cplusplus
    }
#endif

#endif  /* ndef __ANOVACalculator_H__ */
