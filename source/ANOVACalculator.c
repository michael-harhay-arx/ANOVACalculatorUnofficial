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
int numMasks = 0;
int glbNumUniqueFactorElements[MAXFACTORCOLS] = {0};
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
* \brief Parse through CSV data and perform ANOVA calculations
*
* \param [in] Panel					The current panel
* \param [in] FactorRange			The factor range, in format CxRx:CxRx. Factor ranges can be assumed to only be single columns
* \param [in] DataRange				The data range, in format CxRx:CxRx.
* \param [in] LimitRange			The limit range, in format CxRx:CxRx
*******************************************************************************/
void ComputeANOVA (IN int Panel, char FactorRange[][DATALENGTH], char DataRange[][DATALENGTH], char LimitRange[][DATALENGTH])
{
	// Parse ranges
	int factorColNumbers[glbNumFactorCols];
	memset (factorColNumbers, 0, sizeof (factorColNumbers));
	int dataColNumbers[glbNumDataCols];
	memset (dataColNumbers, 0, sizeof (dataColNumbers));
	int limitColNumbers[glbNumDataCols];
	memset (limitColNumbers, 0, sizeof (limitColNumbers));
	
	int startRow = 0;
	int endCol = 0;	
	int endRow = 0;
	int limitStartRow = 0;
	
	for (int i = 0; i < glbNumFactorCols; ++i)
	{
		sscanf (FactorRange[i], "C%dR%d:C%dR%d", factorColNumbers + i, &startRow, &endCol, &endRow);
	}
	
	glbDataColHeight = endRow - startRow + 1;	
	
	for (int i = 0; i < glbNumDataCols; i++)
	{
		sscanf (DataRange[i], "C%d", dataColNumbers + i);
		sscanf (LimitRange[i], "C%dR%d", limitColNumbers + i, &limitStartRow);
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
			if (row == 0)
			{
				strcpy (glbANOVAResult.colLabels[col + 1], currentRow.data[col]);
			}
		}
		dataset[row] = currentRow;
	}
	strcpy (glbANOVAResult.colLabels[0], "Data Type");
	
	// Get limits
	double limitList[glbNumDataCols][2];
	memset (limitList, 0, sizeof (limitList));
	
	for (int col = 0; col < glbNumDataCols; col++)
	{
		for (int row = 0; row < 2; row++)
		{
			limitList[col][row] = atof (glbCSVData[limitStartRow + row - 1][limitColNumbers[col] - 1]);
		}
	}
	
	// Get grand means
	double grandMeans[glbNumDataCols];
	memset (grandMeans, 0, sizeof (grandMeans));
	ComputeGrandMeans (dataset, grandMeans);
		
	// Init results struct
	int numMasks = (1 << glbNumFactorCols) - 1;
	glbANOVAResult.numRows = 3 * (numMasks + 1);
	
	// Iterate through masks (factor combos)
	double groupMeans[MAXFACTORCOMBOS][MAXDATACOLS] = {0};
    for (int mask = 1; mask <= numMasks; mask++) 
	{
		// Get factor combo name
		char factorComboName[256] = {0};
		GetFactorComboName (dataset, mask, factorComboName);
		strcpy (glbANOVAResult.rowLabels[mask - 1], factorComboName);
		
		// Get SS results for specific factor combo
        ComputeSS (dataset, mask, groupMeans, grandMeans, glbANOVAResult.sumSqr[mask - 1], glbANOVAResult.sumSqrRepeat);
    }
	
	// Add equipment to row labels list
	strcpy (glbANOVAResult.rowLabels[numMasks], "Equipment");
	
	// Compute various stats
	ComputeNumUniqueFactorElements (dataset);
	ComputeDegreesFreedom (dataset);
	ComputeMeanSquare ();
	ComputeVariance ();
	ComputeStdDev ();
	ComputePTRatio (limitList);
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
    for (int row = 1; row < glbDataColHeight; row++) 
	{
        for (int col = 0; col < glbNumDataCols; col++) 
		{
            GrandMeans[col] += (double) atof (Dataset[row].data[col]);
        }
    }
	
	// Calculate grand means
    for (int col = 0; col < glbNumDataCols; col++) 
	{
        GrandMeans[col] /= (glbDataColHeight - 1);
    }
}

/***************************************************************************//*!
* \brief Check if two rows match on selected factor mask
*
* \param [in] RowA					First RowStruct to be compared
* \param [in] RowB					Second RowStruct to be compared
* \param [in] Mask					Effect mask
*******************************************************************************/
int MatchOnMask (RowStruct RowA, RowStruct RowB, int Mask) 
{
    for (int factor = 0; factor < glbNumFactorCols; factor++) 
	{
        if ((Mask & (1 << factor)) && (strcmp (RowA.factors[factor], RowB.factors[factor]) != 0)) 
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
* \param [out] SSOut				A list containing the SS for the chosen factor combo
* \param [out] SSOutRepeat			A list containing the SS for equipment
*******************************************************************************/
void ComputeSS (RowStruct Dataset[], IN int Mask, double GroupMeans[MAXFACTORCOMBOS][MAXDATACOLS], double *GrandMeans, double *SSOut, double *SSOutRepeat) 
{
	// Factor combo SS calculation
    int visited[glbDataColHeight];
	memset (visited, 0, sizeof (visited));

    for (int row = 1; row < glbDataColHeight; row++) 
	{
		// Check if mask has already been used
		if (visited[row])
		{
			continue;
		}
		
        // Accumulate all rows matching this group
        double sum[MAXDATACOLS] = {0};
        int count = 0;
		int matchedRows[glbDataColHeight];
    	int matchedCount = 0;

        for (int subRow = 1; subRow < glbDataColHeight; subRow++) 
		{
            if (MatchOnMask (Dataset[row], Dataset[subRow], Mask)) 
			{				
                for (int col = 0; col < glbNumDataCols; col++) 
				{
                    sum[col] += atof (Dataset[subRow].data[col]);
                }
				matchedRows[matchedCount++] = subRow;
                visited[subRow] = 1;
				count++;
            }
        }
		
		// Calculate SSOut
        for (int col = 0; col < glbNumDataCols; col++) 
		{
            GroupMeans[Mask][col] = sum[col] / count;
            double diff = GroupMeans[Mask][col] - GrandMeans[col];
            SSOut[col] += count * diff * diff;
        }
		
		// Calculate SS for equipment
		if (Mask == ((1 << glbNumFactorCols) - 1))
		{
			for (int i = 0; i < matchedCount; i++) 
			{
		        int rowIdx = matchedRows[i];
		        for (int col = 0; col < glbNumDataCols; col++) 
				{
		            double val = atof (Dataset[rowIdx].data[col]);
		            double diff = val - GroupMeans[Mask][col];
		            SSOutRepeat[col] += diff * diff;
		        }
			}
		}
	}
}

/***************************************************************************//*!
* \brief Get factor combo name
*
* \param [in] Dataset				A dataset of RowStructs
* \param [in] Mask					Factor combo mask
* \param [in] FactorComboName		Factor combo name
*******************************************************************************/
void GetFactorComboName (IN RowStruct Dataset[], IN int Mask, char *FactorComboName)
{
    if (Mask == 0) 
	{ 
		strcpy (FactorComboName, "Residual/Error"); 
		return; 
	}
	
	int first = 1;
    for (int col = 0; col < glbNumFactorCols; col++) 
	{
        if (Mask & (1 << col))
		{
			if (!first)
			{
				strcat (FactorComboName, " & ");
			}
			strcat (FactorComboName, Dataset[0].factors[col]);
			first = 0;
		}
    }
}

/***************************************************************************//*!
* \brief Get the number of unique factor elements per factor
*
* \param [in] Dataset				A dataset of RowStructs
*******************************************************************************/
void ComputeNumUniqueFactorElements(RowStruct Dataset[]) 
{
	char seen[glbNumFactorCols][glbDataColHeight][DATALENGTH];
	memset (seen, 0, sizeof (seen));
	
	// Iterate through factors
    for (int f = 0; f < glbNumFactorCols; f++) 
	{
		
		// For each factor, get number of unique values
		for (int row = 1; row < glbDataColHeight; row++) 
		{
			int alreadySeen = 0;
			for (int seenVal = 0; seenVal < glbNumUniqueFactorElements[f]; seenVal++)
			{
	            if (strcmp (seen[f][seenVal], Dataset[row].factors[f]) == 0) 
				{
	                alreadySeen = 1; 
	            }
			}
			
			if (alreadySeen == 0)
			{
				strcpy (seen[f][glbNumUniqueFactorElements[f]], Dataset[row].factors[f]);
				glbNumUniqueFactorElements[f]++;
			}
        }
    }
}

/***************************************************************************//*!
* \brief Compute degrees of freedom for each factor combo
*
* \param [in] Dataset				A dataset of RowStructs
*******************************************************************************/
void ComputeDegreesFreedom (RowStruct Dataset[])
{
	for (int fc = 1; fc <= glbANOVAResult.numRows / NUMINTERMEDROWS; fc++)
	{		
		// Otherwise perform standard calculation
	    int df = 1;
		
		for (int f = 0; f < glbNumFactorCols; f++) 
		{
			if (fc & (1 << f)) 
			{
				df *= glbNumUniqueFactorElements[f] - 1;
			}
		}
	
		glbANOVAResult.degFrd[fc - 1] = df;
	}
	
	// Calculate equipment degrees of freedom
	int counted[glbDataColHeight];
	memset (counted, 0, sizeof (counted));
	int fullMask = (1 << glbNumFactorCols) - 1; 
	
    for (int row = 0; row < glbDataColHeight; row++)
    {
        if (counted[row])
		{
            continue;
		}

        int matchCount = 1;
        counted[row] = 1;

        for (int cmp = row + 1; cmp < glbDataColHeight; cmp++)
        {
            if (!counted[cmp] && MatchOnMask (Dataset[row], Dataset[cmp], fullMask))
            {
                matchCount++;
                counted[cmp] = 1;
            }
        }

        // Each group adds (r - 1) to DoF
        glbANOVAResult.degFrdRepeat += (matchCount - 1);
    }	
}

/***************************************************************************//*!
* \brief Compute MS for each factor combo
*******************************************************************************/
void ComputeMeanSquare ()
{		
    for (int col = 0; col < glbNumDataCols; col++)
	{	
		for (int fc = 0; fc < glbANOVAResult.numRows / NUMINTERMEDROWS; fc++)
		{	
			glbANOVAResult.meanSqr[fc][col] = glbANOVAResult.sumSqr[fc][col] / glbANOVAResult.degFrd[fc];
		}
		
		glbANOVAResult.meanSqrRepeat[col] = glbANOVAResult.sumSqrRepeat[col] / glbANOVAResult.degFrdRepeat;
	}
}

/***************************************************************************//*!
* \brief Compute the denominator for calculating variance
*
* \param [in] Mask					Factor combo mask
*******************************************************************************/
int ComputeVarianceDenom (int Mask)
{
	int n = glbDataColHeight - 1;
	int numCombos = 1;
	
	// Get total number of combinations
    for (int factor = 0; factor < glbNumFactorCols; factor++)
	{
		if (Mask & (1 << factor)) // Get correct bit from mask
		{	
			numCombos *= glbNumUniqueFactorElements[factor];
		}
	}
	
	return n / numCombos;
}

/***************************************************************************//*!
* \brief Get variance indices that involve operator
*
* \param [out] OperatorIndices		A list of which factor combo indices involve the operator
*******************************************************************************/
void GetOperatorVariances (int OperatorIndices[MAXFACTORCOMBOS])
{
	// Only one factor, factor A is automatically the operator	
	if (glbNumFactorCols == 1)
	{
		OperatorIndices[0] = 1;
	}
	
	// Otherwise factor B is the operator, set active indices accordingly
	else if (glbNumFactorCols == 2)
	{
		OperatorIndices[1] = 1;
		OperatorIndices[2] = 1;
	}
	
	else if (glbNumFactorCols == 3)
	{	
		OperatorIndices[1] = 1;
		OperatorIndices[2] = 1;
		OperatorIndices[5] = 1;
	}
}

/***************************************************************************//*!
* \brief Compute variance for each factor combo
*******************************************************************************/
void ComputeVariance (RowStruct Dataset[])
{	
	// Get variance for each factor combo, and equipment
    for (int col = 0; col < glbNumDataCols; col++)
	{
		for (int fc = 1; fc <= glbANOVAResult.numRows / NUMINTERMEDROWS - 1; fc++)
		{	
			// Get denom
			int denom = ComputeVarianceDenom (fc);
			glbANOVAResult.variance[fc - 1][col] = (glbANOVAResult.meanSqr[fc - 1][col] - glbANOVAResult.meanSqrRepeat[col]) / denom;
			glbANOVAResult.varianceTotal[col] += glbANOVAResult.variance[fc - 1][col];
		}
		glbANOVAResult.varianceRepeat[col] = glbANOVAResult.meanSqrRepeat[col];	
	}
										 
	// Get reprod variance
	int operatorVarianceIndices[glbANOVAResult.numRows / NUMINTERMEDROWS - 1];
	memset (operatorVarianceIndices, 0, sizeof (operatorVarianceIndices));
	GetOperatorVariances (operatorVarianceIndices);
	
	for (int col = 0; col < glbNumDataCols; col++)
	{
		glbANOVAResult.varianceReprod[col] = 0;
		
		for (int fc = 0; fc < glbANOVAResult.numRows / NUMINTERMEDROWS - 1; fc++)
		{
			if (operatorVarianceIndices[fc])
			{
				glbANOVAResult.varianceReprod[col] += glbANOVAResult.variance[fc][col];
			}
		}
	}
	
	// Get variance as %
	for (int col = 0; col < glbNumDataCols; col++)
	{
		glbANOVAResult.varianceReprodPct[col] = 100 * glbANOVAResult.varianceReprod[col] / glbANOVAResult.varianceTotal[col];
		glbANOVAResult.varianceRepeatPct[col] = 100 * glbANOVAResult.varianceRepeat[col] / glbANOVAResult.varianceTotal[col];
	}
}

/***************************************************************************//*!
* \brief Compute std dev for each factor combo
*******************************************************************************/
void ComputeStdDev ()
{		
    for (int col = 0; col < glbNumDataCols; col++)
	{
		glbANOVAResult.stdDevReprod[col] = sqrt (glbANOVAResult.varianceReprod[col]);
		glbANOVAResult.stdDevRepeat[col] = sqrt (glbANOVAResult.varianceRepeat[col]);
		glbANOVAResult.stdDevTotal[col] = sqrt (glbANOVAResult.varianceReprod[col] + glbANOVAResult.varianceRepeat[col]);
	}
}

/***************************************************************************//*!
* \brief Compute PT Ratio for each factor combo
*
* \param [in] LimitList				A list containing limits for each data column
*******************************************************************************/
void ComputePTRatio (double LimitList[][2])
{
    for (int col = 0; col < glbNumDataCols; col++)
	{
		double limitDiff = abs (LimitList[col][1] - LimitList[col][0]);
		
		glbANOVAResult.ptRatioReprod[col] = 6 * glbANOVAResult.stdDevReprod[col] / limitDiff;
		glbANOVAResult.ptRatioRepeat[col] = 6 * glbANOVAResult.stdDevRepeat[col] / limitDiff;
		glbANOVAResult.ptRatioTotal[col] = 6 * glbANOVAResult.stdDevTotal[col] / limitDiff;
	}
}
