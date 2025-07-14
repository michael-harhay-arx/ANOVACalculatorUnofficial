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

int glbDataColHeight = 0;
int glbNumMasks = 0;
ANOVAResult glbANOVAResult = {0};

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
	memset (factorColNumbers, 0, sizeof (factorColNumbers));
	int dataColNumbers[glbNumDataCols];
	memset (dataColNumbers, 0, sizeof (dataColNumbers));
	
	int startRow = 0;
	int endCol = 0;	
	int endRow = 0;
	
	for (int i = 0; i < glbNumFactorCols; ++i)
	{
		sscanf (FactorRange[i], "C%dR%d:C%dR%d", factorColNumbers + i, &startRow, &endCol, &endRow);
	}
	
	glbDataColHeight = endRow - startRow + 1;
	
	for (int i = 0; i < glbNumDataCols; i++)
	{
		sscanf (DataRange[i], "C%d", dataColNumbers + i);
	}
	
	// Build list of RowStructs
	RowStruct dataset[glbNumRows];
	memset (dataset, 0, sizeof (dataset));
	
	RowStruct currentRow = {0};
	for (int row = 0; row < glbDataColHeight; row++)
	{
		for (int col = 0; col < glbNumFactorCols; col++)
		{
			strcpy (currentRow.factors[col], glbCSVData[startRow + row - 1][factorColNumbers[col] - 1]);
		}
		for (int col = 0; col < glbNumDataCols; col++)
		{
			strcpy (currentRow.data[col], glbCSVData[startRow + row - 1][dataColNumbers[col] - 1]);
		}
		dataset[row] = currentRow;
	}
	
	// Get grand means
	double grandMeans[glbNumDataCols];
	memset (grandMeans, 0, sizeof (grandMeans));
	ComputeGrandMeans (dataset, grandMeans);
	
	// Get total SS vales (per data column)
	double ssTotal[glbNumDataCols];
	memset (ssTotal, 0, sizeof (ssTotal));
	ComputeTotalSS (dataset, grandMeans, ssTotal);
	
	// Init other SS arrays
	double ssFactorCombos[8][glbNumDataCols];
	memset (ssFactorCombos, 0, sizeof (ssFactorCombos));
    double ssSum[glbNumDataCols];
	memset (ssSum, 0, sizeof (ssSum));
		
	// Init results struct
	int glbNumMasks = 1 << glbNumFactorCols;
	glbANOVAResult.numRows = glbNumMasks + 2;
	
	// Iterate through masks (factor combos), get SS results
    for (int mask = 1; mask < glbNumMasks; mask++) 
	{
        ComputeSSFactorCombo (dataset, mask, grandMeans, ssFactorCombos[mask]);
		
		sprintf (glbANOVAResult.factorCombos[mask], "%d", mask); // TODO: Fix mask naming for table
		
        printf ("SS["); 
		printFactorComboName (mask); 
		printf ("]:");
        
		for (int col = 0; col < glbNumDataCols; col++) 
		{
			glbANOVAResult.ssResults[mask][col] = ssFactorCombos[mask][col];
            printf (" %.3f", ssFactorCombos[mask][col]);
            ssSum[col] += ssFactorCombos[mask][col];
        }
        printf("\n");
    }
	
	// Error calculation
    printf ("SS[Residual/Error]:");
    for (int col = 0; col < glbNumDataCols; col++) 
	{
        ssFactorCombos[0][col] = ssTotal[col] - ssSum[col];
        printf (" %.3f", ssFactorCombos[0][col]);
    }
    printf("\n");
			
	// Build ANOVA tree
	//BuildANOVATree (Panel, FactorRange, TreeRoot, 0);
}

/***************************************************************************//*!
* \brief Calculate grand mean for each column
*
* \param [in] Dataset				A dataset of RowStructs
* \param [in] GrandMeans			A list containing the grand mean for each column
*******************************************************************************/
void ComputeGrandMeans (RowStruct Dataset[], double GrandMeans[])
{	
	// Sum data from each column
    for (int row = 0; row < glbDataColHeight; row++) 
	{
        for (int col = 0; col < glbNumDataCols; col++) 
		{
            GrandMeans[col] += (double) atof (Dataset[row].data[col]);
        }
    }
	
	// Calculate grand means
    for (int col = 0; col < glbNumDataCols; col++) 
	{
        GrandMeans[col] /= glbDataColHeight;
    }
}

/***************************************************************************//*!
* \brief Calculate total SS for each column
*
* \param [in] Dataset				A dataset of RowStructs
* \param [in] GrandMeans			A list containing the grand mean for each column
* \param [in] SSTotal				A list containing the SS total for each column
*******************************************************************************/
void ComputeTotalSS (RowStruct Dataset[], double GrandMeans[], double SSTotal[])
{	
	// Get total SS for each column
    for (int row = 0; row < glbDataColHeight; row++) 
	{
        for (int col = 0; col < glbNumDataCols; col++) 
		{
            double diff = (double) atof (Dataset[row].data[col]) - GrandMeans[col];
			SSTotal[col] += pow (diff, 2);
        }
    }
}

/***************************************************************************//*!
* \brief Check if two rows match on selected factor mask
*
* \param [in] RowA					First RowStruct to be compared
* \param [in] RowB					Second RowStruct to be compared
* \param [in] Mask					Effect mask
*******************************************************************************/
int matchOnMask (RowStruct *RowA, RowStruct *RowB, int Mask) 
{
    for (int factor = 0; factor < glbNumFactorCols; factor++) 
	{
        if ((Mask & (1 << factor)) && (RowA->factors[factor] != RowB->factors[factor])) 
		{
            return 0;
        }
    }
    return 1;
}

/***************************************************************************//*!
* \brief Compute SS for the factor combo defined by mask
*
* \param [in] Dataset				A dataset of RowStructs
* \param [in] Mask					Effect mask
* \param [in] GrandMeans			A list containing the grand mean for each column
* \param [in] SSOut					A list containing the SS for the chosen factor combo
*******************************************************************************/
void ComputeSSFactorCombo (RowStruct Dataset[], int Mask, double *GrandMeans, double *SSOut) 
{
    int visited[glbDataColHeight];
	memset (visited, 0, sizeof (visited));

    for (int row = 0; row < glbDataColHeight; row++) 
	{
		// Check if mask has already been used
		if (visited[row])
		{
			continue;
		}
		
        // Accumulate all rows matching this group
        double sum[MAXDATACOLS] = {0.0};
        int count = 0;

        for (int subRow = 0; subRow < glbDataColHeight; subRow++) 
		{
            if (!visited[subRow] && matchOnMask (&Dataset[row], &Dataset[subRow], Mask)) 
			{				
                for (int col = 0; col < glbNumDataCols; col++) 
				{
                    sum[col] += (double) atof (Dataset[subRow].data[col]);
                }
                count++;
                visited[subRow] = 1;
            }
        }

        for (int col = 0; col < glbNumDataCols; col++) 
		{
            double groupMean = sum[col] / count;
            double diff = groupMean - GrandMeans[col];
            SSOut[col] += count * diff * diff;
        }
    }
}

/***************************************************************************//*!
* \brief Print factor combo
*
* \param [in] Mask					Factor combo mask
*******************************************************************************/
void printFactorComboName (int Mask) 
{
    if (Mask == 0) 
	{ 
		printf ("Residual/Error"); 
		return; 
	}
    for (int i = 0; i < glbNumFactorCols; i++) 
	{
        if (Mask & (1 << i))
		{
			printf ("%c", 'A' + i);
		}
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
