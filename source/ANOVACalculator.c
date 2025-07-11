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
	
	// Group duplicate factor elements into list
	//int numUniqueFactorElements = 0;
	//FactorElement factorElementList[1] = {0};
	
	// Parse ranges
	int colNumbers[100] = {0};
	int start_row = 0;
	int end_row = 0;
	int end_col = 0;	
	
	for (int i = 0; i < glbNumFactors; ++i)
	{
		sscanf (FactorRange[i], "C%dR%d:C%dR%d", colNumbers + i, &start_row, &end_col, &end_row);
	}
	
	
	/*
	// Get list of unique factor elements
	int numRows = end_row - start_row + 1;
	char *existing = calloc (glbNumFactors * numRows * 32, 1);
	char glbUniqueFactors[glbNumFactors][numRows][32] = {0};
	int glbUniqueFactorCount[glbNumFactors] = {0};
	
	for (int i = 0; i < numRows; ++i)
	{
		for (int j = 0; j < glbNumFactors; ++j)
		{
			if (strstr (existing + j * numRows * 32, glbCSVData[i][colNumbers[j]]))
			{
				continue;
			}
			strcat (existing + j * numRows * 32, glbCSVData[i][colNumbers[j]]);
			strcpy (glbUniqueFactors[glbUniqueFactorCount[j]++], glbCSVData[i][colNumbers[j]]);
		}
	}
	
	int product = 1;
	for (int j = 0; j < glbNumFactors; ++j)
	{
		product *= glbUniqueFactorCount[j];
	}
	
	// Get SS values 
	char names[buckets][pow (2, glbNumFactors)][64];
	double sums[buckets][pow (2, glbNumFactors)] = {0};
	int sumCount[buckets][pow (2, glbNumFactors)] = {0};
	int buckets = 7;
	
	for (int i = 0; i < numRows; ++i)
	{
		for (int j = 0; j < buckets; ++j)
		{
			int m = 0;
			for (int k = 0; k < glbNumFactors; ++k)
			{
				for (m; m < glbUniqueFactorCount[k]; ++m)
				{
					if (0 == strcmp (data[i][k], glbUniqueFactors[k][m])) break;
				}
			}
			sums[j][m] += val;
			++sumCount[j][m];
		}
	}
	*/
	
	
	/*
	// Iterate through factors
	for (int i = 0; i < glbNumFactors; i++)
	{
		
		
		
		
		// Load factor col data
		
		// Iterate through factor elements
		for (int factorElement = start_row; factorElement <= end_row; factorElement++)
		{
			// Get factor element key
			char factorElementKey[32] = {0};
			Point currentPoint = 
			{
				.x = start_col,
				.y = factorElement
			};
			GetTableCellVal (Panel, CSVPANEL_CSVTABLE, currentPoint, factorElementKey);
			
			// If factor element already in list, just add data, otherwise add key and data
			int match = 0;
			for (int k = 0; k < numUniqueFactorElements; k++)
			{
				if (strcmp (factorElementKey, factorElementList[k]) == 0)
				{
					match = 1;
				}
				
				// Add data
				if (match)
				{
					
				}
				
				else
				{
					numUniqueFactorElements++;
					
					// Add key and data, then realloc more space
				}
			}
		}
	}
	*/
			
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
	for (int factorElement = start_row + 1; factorElement <= end_row; factorElement++)
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
