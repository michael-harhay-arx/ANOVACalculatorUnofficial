//==============================================================================
//
// Title:		ANOVACalculator
// Purpose:		A short description of the application.
//
// Created on:	2025-07-09 at 11:42:42 AM by .
// Copyright:	. All Rights Reserved.
//
//==============================================================================

//==============================================================================
// Include files

#include "toolbox.h"
#include <ansi_c.h>
#include <cvirte.h>		
#include <userint.h>

#include "ANOVACalculator.h"

//==============================================================================
// Constants

//==============================================================================
// Types

//==============================================================================
// Static global variables

static int panelHandle = 0;

//==============================================================================
// Static functions

//==============================================================================
// Global variables

int glbNumFactors = 0;
int glbNumDataCols = 0;

//==============================================================================
// Global functions

/***************************************************************************//*!
* \brief Main entry point
*******************************************************************************/
int main (int argc, char *argv[])
{
	int error = 0;
	
	/* initialize and load resources */
	nullChk (InitCVIRTE (0, argv, 0));
	errChk (panelHandle = LoadPanel (0, "MainPanel.uir", MAINPANEL));
	
	/* display the panel and run the user interface */
	errChk (DisplayPanel (panelHandle));
	errChk (RunUserInterface ());

Error:
	/* clean up */
	if (panelHandle > 0)
		DiscardPanel (panelHandle);
	return 0;
}

/***************************************************************************//*!
* \brief Parse through CSV table selections and prepare data for calculations
*
* \param [in] Panel					The current panel
* \param [in] FactorRange			The factor range, in format CxRx:CxRx. Factor ranges can be assumed to only be single columns
* \param [in] DataRange				The data range, in format CxRx:CxRx.
* \param [in] LimitRange			The limit range, in format CxRx:CxRx
* \param [out] TreeRoot				Root of tree containing all data nodes, ready for ANOVA
*******************************************************************************/
void ParseCSVSelection (IN int Panel, char FactorRange[][32], char DataRange[][32], char LimitRange[][32], ANOVANode *TreeRoot)
{
	// Get number of factors and data columns
	while (strcmp (FactorRange[glbNumFactors], "0") != 0)
	{
		glbNumFactors++;
	}
	while (strcmp (DataRange[glbNumDataCols], "0") != 0)
	{
		glbNumDataCols++;
	}
	
	// Build ANOVA tree
	BuildANOVATree (Panel, FactorRange, TreeRoot, 0);
					  
	
}

/***************************************************************************//*!
* \brief Recursively builds a tree from ANOVANodes
*
* \param [in] Panel				The current panel
* \param [in] CurrentRoot		The current root at the level the tree being worked on
* \param [in] Level				The current level of the tree (# levels = # factors)
*******************************************************************************/
void BuildANOVATree (IN int Panel, IN char FactorRange[][32], ANOVANode *CurrentRoot, int Level)
{
	// Base case
	if (Level >= glbNumFactors)
	{
		return;
	}
	
	// Load factor col data
	int start_row = 0;
	int start_col = 0;
	int end_row = 0;
	int end_col = 0;
	sscanf (FactorRange[Level], "C%dR%d:C%dR%d", &start_col, &start_row, &end_col, &end_row);
	
	// Add new nodes at current level (for current factor)
	for (int factorElement = start_row + 1; factorElement < end_row + 1; factorElement++)
	{
		char nodeKey[32] = {0};
		Point currentPoint = 
		{
			.x = start_col,
			.y = factorElement
		};
		
		GetTableCellVal (Panel, CSVPANEL_CSVTABLE, currentPoint, nodeKey);
		
		// Debug
        for (int i = 0; i < Level; i++) {
            printf("  ");
        }
        printf("Level %d - Adding node: %s\n", Level, nodeKey);
		
		ANOVANode *newNode = CreateANOVANode (nodeKey);
		AddChildNode (CurrentRoot, newNode);
		
		// Recursive call to populate node children
		BuildANOVATree (Panel, FactorRange, newNode, Level + 1);
	}
}

/***************************************************************************//*!
* \brief Perform ANOVA calculations
*******************************************************************************/
int ComputeANOVA (ANOVANode *TreeRoot)
{
	return 0;
}

/***************************************************************************//*!
* \brief Create new ANOVANode
*
* \param [in] Key				A specific element from a factor (e.g., Factor "SN"
*								contains the labels 1001, 1002, 1003... etc.)
*******************************************************************************/
ANOVANode *CreateANOVANode (IN char Key[32])
{
	ANOVANode *node = malloc (sizeof (ANOVANode));
	strcpy (node->key, Key);
	node->numValues = 0;
	node->values = NULL;
	node->numChildren = 0;
	node->children = NULL;
	
	return node;
}

/***************************************************************************//*!
* \brief Add a value to a node's list of values
*
* \param [in] Node				The target node
* \param [in] Value				The value to be added
*******************************************************************************/
void AddNodeValue (ANOVANode *Node, IN double Value)
{
	Node->numValues++;
	Node->values = realloc (Node->values, sizeof (double) * (Node->numValues));
	Node->values[Node->numValues - 1] = Value;
}

/***************************************************************************//*!
* \brief Assign a node to be a child of another node
*
* \param [in] ParentNode		The target parent node
* \param [in] ChildNode			The target child node
*******************************************************************************/
void AddChildNode (ANOVANode *ParentNode, ANOVANode *ChildNode)
{
	ParentNode->numChildren++;
	ParentNode->children = realloc (ParentNode->children, sizeof (ANOVANode *) * (ParentNode->numChildren));
	ParentNode->children[ParentNode->numChildren - 1] = ChildNode;
}
