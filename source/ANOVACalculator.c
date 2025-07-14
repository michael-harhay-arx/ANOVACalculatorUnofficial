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
void GetSSDataset (IN int Panel, char FactorRange[][DATALENGTH], char DataRange[][DATALENGTH], char LimitRange[][DATALENGTH], ANOVANode *TreeRoot)
{
	// Parse ranges
	int factorColNumbers[glbNumFactorCols];
	int dataColNumbers[glbNumDataCols];
	memset (factorColNumbers, 0, sizeof (factorColNumbers));
	memset (dataColNumbers, 0, sizeof (dataColNumbers));
	
	int startRow = 0;
	int endCol = 0;	
	int endRow = 0;
	int colHeight = 0;
	
	for (int i = 0; i < glbNumFactorCols; ++i)
	{
		sscanf (FactorRange[i], "C%dR%d:C%dR%d", factorColNumbers + i, &startRow, &endCol, &endRow);
	}
	
	colHeight = endRow - startRow + 1;
	
	for (int i = 0; i < glbNumDataCols; i++)
	{
		sscanf (DataRange[i], "C%d", dataColNumbers + i);
	}
	
	// Build list of RowStructs
	RowStruct dataset[glbNumRows + 1];
	memset (dataset, 0, sizeof (dataset));
	
	RowStruct currentRow = {0};
	for (int row = 0; row < colHeight; row++)
	{
		for (int col = 0; col < glbNumFactorCols; col++)
		{
			strcpy (currentRow.factors[col], glbCSVData[startRow + row - 1][factorColNumbers[col]]);
		}
		
		for (int col = 0; col < glbNumDataCols; col++)
		{
			strcpy (currentRow.data[col], glbCSVData[startRow + row - 1][dataColNumbers[col]]);
		}
		
		dataset[row] = currentRow;
	}
	
	// Get grand means
	double grandMeans[glbNumFactorCols + glbNumDataCols];
	memset (grandMeans, 0, sizeof (grandMeans));
	ComputeGrandMeans (dataset, grandMeans);
	
	
	
	//// Get list of unique factor elements
	//int numRows = end_row - start_row + 1;
	//char *existing = calloc (glbNumFactorCols * numRows * 32, 1);
	//char glbUniqueFactors[glbNumFactorCols][numRows][32] = {0};
	//int glbUniqueFactorCount[glbNumFactorCols] = {0};
	//
	//for (int i = 0; i < numRows; ++i)
	//{
	//	for (int j = 0; j < glbNumFactorCols; ++j)
	//	{
	//		if (strstr (existing + j * numRows * 32, glbCSVData[i][colNumbers[j]]))
	//		{
	//			continue;
	//		}
	//		strcat (existing + j * numRows * 32, glbCSVData[i][colNumbers[j]]);
	//		strcpy (glbUniqueFactors[glbUniqueFactorCount[j]++], glbCSVData[i][colNumbers[j]]);
	//	}
	//}
	
	// Group duplicate factor elements into list
	//int numUniqueFactorElements = 0;
	//FactorElement factorElementList[1] = {0};
	
	/*
	int product = 1;
	for (int j = 0; j < glbNumFactorCols; ++j)
	{
		product *= glbUniqueFactorCount[j];
	}
	*/
	
	//// Get SS values 
	//char names[buckets][pow (2, glbNumFactorCols)][64];
	//double sums[buckets][pow (2, glbNumFactorCols)] = {0};
	//int sumCount[buckets][pow (2, glbNumFactorCols)] = {0};
	//int buckets = 7;
	//
	//for (int i = 0; i < numRows; ++i)
	//{
	//	for (int j = 0; j < buckets; ++j)
	//	{
	//		int m = 0;
	//		for (int k = 0; k < glbNumFactorCols; ++k)
	//		{
	//			for (m; m < glbUniqueFactorCount[k]; ++m)
	//			{
	//				if (0 == strcmp (data[i][k], glbUniqueFactors[k][m])) break;
	//			}
	//		}
	//		sums[j][m] += val;
	//		++sumCount[j][m];
	//	}
	//}
			
	// Build ANOVA tree
	BuildANOVATree (Panel, FactorRange, TreeRoot, 0);
}

/***************************************************************************//*!
* \brief Calculate grand mean for each column
*
* \param [in] Dataset				A dataset of RowStructs
* \param [in] GrandMeans			A list of the grand means for each column
*******************************************************************************/
void ComputeGrandMeans (RowStruct Dataset[], double GrandMeans[])
{
	int numCols = glbNumFactorCols + glbNumDataCols;
	
	// Sum data from each column
    for (int row = 0; row < glbNumRows; row++) 
	{
        for (int col = 0; col < numCols; col++) 
		{
            GrandMeans[col] += (double) atoi (Dataset[row].data[col]);
        }
    }
	
	// Calculate grand means
    for (int col = 0; col < numCols; col++) 
	{
        GrandMeans[col] /= glbNumRows;
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
* \brief Recursively builds a tree from ANOVANodes
*
* \param [in] Panel				The current panel
* \param [in] CurrentRoot		The current root at the level the tree being worked on
* \param [in] Level				The current level of the tree (# levels = # factors)
*******************************************************************************/
void BuildANOVATree (IN int Panel, IN char FactorRange[][DATALENGTH], ANOVANode *CurrentRoot, int Level)
{
	// Base case
	if (Level >= glbNumFactorCols)
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
		char nodeKey[DATALENGTH] = {0};
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
* \brief Create new ANOVANode
*
* \param [in] Key				A specific element from a factor (e.g., Factor "SN"
*								contains the labels 1001, 1002, 1003... etc.)
*******************************************************************************/
ANOVANode *CreateANOVANode (IN char Key[DATALENGTH])
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
