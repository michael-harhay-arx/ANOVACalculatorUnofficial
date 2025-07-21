/***************************************************************************//*!
* \file Callbacks.c
* \author 
* \copyright . All Rights Reserved.
* \date 2025-07-09 11:44:39 AM
* \brief A short description.
* 
* A longer description.
*******************************************************************************/

//! \cond
/// REGION START Header
//! \endcond
//==============================================================================
// Include files

/***************************************************************************//*!
* \brief Disables system logging completely.  Needs to be defined before including
* 	ArxtronToolslib.h.  By default, it is defined in each source file to allow
* 	for source file level control for disabling.
*******************************************************************************/
//#define SYSLOGDISABLE
/***************************************************************************//*!
* \brief Overrides config log level.  Needs to be defined before including
* 	ArxtronToolslib.h.  By default, it is defined in each source file to allow
* 	for source file level control for overrides.
*******************************************************************************/
//#define SYSLOGOVERRIDE 3

#include "toolbox.h"
#include <ansi_c.h>
#include <time.h>

#include "Callbacks.h"
#include "CSVParse_LIB.h"

//==============================================================================
// Constants

const char overviewRowLabels[10][DATALENGTH] = {"Variance_reproducibility", "Variance_repeatability", "Variance_total", "Variance_%perFactor",
							 				   "StdDev_reproducibility", "StdDev_repeatability", "StdDev_overall",
											   "PTRatio_reproducibility", "PTRatio_repeatability", "PTRatio_overall"};

//==============================================================================
// Types

//==============================================================================
// Static global variables

//==============================================================================
// Static functions

//==============================================================================
// Global variables

// GUI variables
int glbCSVPanelHandle = 0;
int glbANOVAPanelHandle = 0;
int glbDisplayMode = 0;

// CSV file variables
char glbCSVFilepath[512] = {0};
char ***glbCSVData= NULL;
int glbNumRows = 0;
int glbNumCols = 0;
int glbNumFactorCols = 0;
int glbNumDataCols = 0;

char glbFactorRange[MAXFACTORCOLS][DATALENGTH] = {0};
char glbDataRange[MAXDATACOLS][DATALENGTH] = {0};
char glbLimitRange[MAXDATACOLS][DATALENGTH] = {0};

//==============================================================================
// Global functions

//! \cond
/// REGION END

/// REGION START Code Body
//! \endcond
/***************************************************************************//*!
* \brief Main panel: Callback for closing main panel
*******************************************************************************/
int CVICALLBACK MainPanelCB (int panel, int event, void *callbackData, int eventData1, int eventData2)
{
	if (event == EVENT_CLOSE)
	{
		// Free glbCSVData
		for (int row = 0; row < glbNumRows; row++)
		{
			for (int col = 0; col < glbNumCols; col++)
			{
				free (glbCSVData[row][col]);
			}
			free (glbCSVData[row]);
		}
		free (glbCSVData);
		
		// Quit GUI
		QuitUserInterface (0);
	}
	
	return 0;
}

/***************************************************************************//*!
* \brief Main panel: Open button callback
*******************************************************************************/
int CVICALLBACK OpenButtonCB(int panel, int control, int event, void *callbackData, int eventData1, int eventData2)
{
	if (event == EVENT_LEFT_CLICK)
	{
		// Open file selection dialogue
		int selectionStatus = FileSelectPopup ("", "*.csv", "*.*", "Select a CSV file...", VAL_OK_BUTTON, 0, 1, 1, 0, glbCSVFilepath);
		if (selectionStatus == 0)
		{
			return 0;
		}
		
		// Load CSV panel
		glbCSVPanelHandle = LoadPanel (0, "CSVPanel.uir", CSVPANEL);
		DisplayCSVTable ();
		DisplayPanel (glbCSVPanelHandle);
	}
	
	return 0;
}

/***************************************************************************//*!
* \brief Main panel: Load button callback
*******************************************************************************/
int CVICALLBACK LoadButtonCB(int panel, int control, int event, void *callbackData, int eventData1, int eventData2)
{
	if (event == EVENT_LEFT_CLICK)
	{
		// Load ANOVA panel
		glbANOVAPanelHandle = LoadPanel (0, "ANOVAPanel.uir", ANOVAPANEL);
		SetCtrlAttribute (glbANOVAPanelHandle, ANOVAPANEL_SAVETEXT, ATTR_VISIBLE, 0);
		
		// Select .anova file to load
		char selectedFilepath[512];
		int selectionStatus = FileSelectPopup ("..\\saves", "*.anova", "*.*", "Select an ANOVA file...", VAL_OK_BUTTON, 0, 1, 1, 0, selectedFilepath);
		if (selectionStatus == 0)
		{
			return 0;
		}
		
		// Open .anova file
		SaveData data;
		FILE *fp = fopen (selectedFilepath, "rb");
	    if (!fp) 
		{
	        perror ("Couldn't get file to load");
	        return 0;
	    }
		
		// Read SaveData struct
	    size_t readResult = fread (&data, sizeof (SaveData), 1, fp);
		
		// Read glbCSVData
		if (glbCSVData == NULL)
		{
			glbCSVData = malloc (sizeof (char **) * glbNumRows);
			for (int row = 0; row < glbNumRows; row++)
			{
				glbCSVData[row] = malloc (sizeof (char *) * glbNumCols);
				for (int col = 0; col < glbNumCols; col++)
				{
					glbCSVData[row][col] = calloc (DATALENGTH, sizeof (char));
				}
			}
		}
		
		for (int row = 0; row < glbNumRows; row++)
		{
			for (int col = 0; col < glbNumCols; col++)
			{
				fread (glbCSVData[row][col], sizeof (char), DATALENGTH, fp);
			}
		}
			
		if (readResult != 1)
		{
			perror ("Error reading file");
		}
	    fclose (fp);
		
		// Restore data
		glbNumRows = data.numRows;
		glbNumCols = data.numCols;
		glbNumFactorCols = data.numFactorCols;
		glbNumDataCols = data.numDataCols;
		memcpy (glbCSVFilepath, data.csvFilepath, 512);
		memcpy (glbFactorRange, data.factorRange, MAXFACTORCOLS * DATALENGTH);
		memcpy (glbDataRange, data.dataRange, MAXDATACOLS * DATALENGTH);
		memcpy (glbLimitRange, data.limitRange, MAXDATACOLS * DATALENGTH);
		glbANOVAResult = data.anovaResult;
		
		// Load data into panel and display
		DisplayANOVATable ();
		DisplayPanel (glbANOVAPanelHandle);
	}
	
	return 0;
}

/***************************************************************************//*!
* \brief CSV panel: Callback for closing CSV panel
*******************************************************************************/
int CVICALLBACK CSVPanelCB (int panel, int event, void *callbackData, int eventData1, int eventData2)
{
	if (event == EVENT_CLOSE)
	{
		DiscardPanel (glbCSVPanelHandle);
		glbCSVPanelHandle = -1;
	}
	
	return 0;
}

/***************************************************************************//*!
* \brief CSV panel: Callback for making table selections
*******************************************************************************/
int CVICALLBACK CSVTableCB (int panel, int control, int event, void *callbackData, int eventData1, int eventData2)
{
	if (event == EVENT_SELECTION_CHANGE || event == EVENT_ACTIVE_CELL_CHANGE)
	{
		Rect selectionRect = {0};
		GetTableSelection (panel, CSVPANEL_CSVTABLE, &selectionRect);
		
		// If no selection, print nothing
		if (selectionRect.height == -1 && selectionRect.width == -1)
		{
			SetCtrlVal (panel, CSVPANEL_FACTORSELECT, "");
			SetCtrlVal (panel, CSVPANEL_DATASELECT, "");
			SetCtrlVal (panel, CSVPANEL_LIMITSELECT, "");
			return 0;
		}
		
		// Else print selected cell/range
		char selectionStr[DATALENGTH] = {0};
		if (selectionRect.height == 0 && selectionRect.width == 0) // one cell selected, have to use GetActiveTableCell()
		{
			Point activeCell = {0};
			GetActiveTableCell (panel, CSVPANEL_CSVTABLE, &activeCell);
			sprintf (selectionStr, "C%dR%d:C%dR%d", activeCell.x, activeCell.y, activeCell.x, activeCell.y);
		}
		else 
		{
			sprintf (selectionStr, "C%dR%d:C%dR%d", selectionRect.left, selectionRect.top, selectionRect.left + selectionRect.width - 1, selectionRect.top + selectionRect.height - 1);
		}
		
		SetCtrlVal (panel, CSVPANEL_FACTORSELECT, selectionStr);
		SetCtrlVal (panel, CSVPANEL_DATASELECT, selectionStr);
		SetCtrlVal (panel, CSVPANEL_LIMITSELECT, selectionStr);
	}
	
	return 0;
}

/***************************************************************************//*!
* \brief CSV panel: Callback for factor/data/limit select button
*******************************************************************************/
int CVICALLBACK CSVSelectButtonCB(int panel, int control, int event, void *callbackData, int eventData1, int eventData2)
{
	if (event == EVENT_LEFT_CLICK)
	{
		int selectionTextBox = 0;
		int listBox = 0;
		char selection[DATALENGTH] = {0};
		int listSize = 0;
		
		// Set appropriate text box and list box
		switch (control)
		{
			case CSVPANEL_FACTORSELECTBUTTON:
                selectionTextBox = CSVPANEL_FACTORSELECT;
				listBox = CSVPANEL_FACTORLIST;
                break;

            case CSVPANEL_DATASELECTBUTTON:
                selectionTextBox = CSVPANEL_DATASELECT;
				listBox = CSVPANEL_DATALIST;
                break;

            case CSVPANEL_LIMITSELECTBUTTON:
                selectionTextBox = CSVPANEL_LIMITSELECT;
				listBox = CSVPANEL_LIMITLIST;
                break;
		}
		
		// Get selection from text box
		GetCtrlVal (panel, selectionTextBox, selection);
		GetNumListItems (panel, listBox, &listSize);
		
		// Do not insert if duplicate item already exists in list
		char itemLabel[DATALENGTH];
		for (int i = 0; i < listSize; i++)
		{
			GetLabelFromIndex (panel, listBox, i, itemLabel);
			if (strcmp (selection, itemLabel) == 0)
			{
				SetCtrlIndex (panel, listBox, i);
				return 0;
			}
		}
		
		// Insert selection into list
		if (selection != "")
		{
			InsertListItem (panel, listBox, -1, selection, "");
			SetCtrlIndex (panel, listBox, listSize);
		}
	}
	
	return 0;
}

/***************************************************************************//*!
* \brief CSV panel: Callback for factor/data/limit delete button
*******************************************************************************/
int CVICALLBACK CSVDeleteButtonCB(int panel, int control, int event, void *callbackData, int eventData1, int eventData2)
{
	if (event == EVENT_LEFT_CLICK)
	{
		int listBox = 0;
		int selectionIndex = 0;
		
		// Set appropriate list box
		switch (control)
		{
			case CSVPANEL_FACTORDELETEBUTTON:
				listBox = CSVPANEL_FACTORLIST;
                break;

            case CSVPANEL_DATADELETEBUTTON:
				listBox = CSVPANEL_DATALIST;
                break;

            case CSVPANEL_LIMITDELETEBUTTON:
				listBox = CSVPANEL_LIMITLIST;
                break;
		}
		
		// Add selection to list
		GetCtrlIndex (panel, listBox, &selectionIndex);
		if (selectionIndex != -1)
		{
			DeleteListItem (panel, listBox, selectionIndex, 1);
		}
	}
	
	return 0;
}

/***************************************************************************//*!
* \brief CSV panel: Callback for factor/data/limit list boxes
*******************************************************************************/
int CVICALLBACK CSVListCB(int panel, int control, int event, void *callbackData, int eventData1, int eventData2)
{
	if (event == EVENT_VAL_CHANGED || event == EVENT_GOT_FOCUS)
	{
		// Get active list index
		int activeListIndex = 0;
		char activeListLabel[DATALENGTH] = {0};
		GetCtrlIndex (panel, control, &activeListIndex);
		GetLabelFromIndex (panel, control, activeListIndex, activeListLabel);
		
		// Highlight corresponding selection in table
		Rect activeCellRect = {0};
		int right = 0;
		int bottom = 0;
		
		sscanf (activeListLabel, "C%dR%d:C%dR%d", &activeCellRect.left, &activeCellRect.top, &right, &bottom);
		activeCellRect.width = right - activeCellRect.left + 1;
		activeCellRect.height = bottom - activeCellRect.top + 1;
		
		SetTableSelection (panel, CSVPANEL_CSVTABLE, activeCellRect);
	}
	
	return 0;
}

/***************************************************************************//*!
* \brief CSV panel: Callback for ANOVA calculation button
*******************************************************************************/
int CVICALLBACK CSVCalcButtonCB(int panel, int control, int event, void *callbackData, int eventData1, int eventData2)
{
	char errmsg[ERRLEN] = {0};
	fnInit;
	error = 0;
	
	if (event == EVENT_LEFT_CLICK)
	{
		// Load ANOVA panel
		HidePanel (glbCSVPanelHandle);
		glbANOVAPanelHandle = LoadPanel (0, "ANOVAPanel.uir", ANOVAPANEL);
		SetCtrlAttribute (glbANOVAPanelHandle, ANOVAPANEL_SAVETEXT, ATTR_VISIBLE, 0);
		
		// Get number of cols for each (assume all selections are columns)
		// TODO: allow for non-column selections
		GetNumListItems (panel, CSVPANEL_FACTORLIST, &glbNumFactorCols);
		GetNumListItems (panel, CSVPANEL_DATALIST, &glbNumDataCols);
		
		// Get factors/data/limits from UI list boxes
		GetDataFromListBoxes (panel, glbFactorRange, glbDataRange, glbLimitRange);

		// Parse selected factors/data/limits
		ComputeANOVA (panel, glbFactorRange, glbDataRange, glbLimitRange); // TODO add support for multi-col data selections... but can assume factors are one col
		
		// Display ANOVA table
		glbDisplayMode = 0;
		DisplayANOVATable ();
		DisplayPanel (glbANOVAPanelHandle);
	}

Error:
	return error;
}

/***************************************************************************//*!
* \brief ANOVA panel: Callback for closing ANOVA panel
*******************************************************************************/
int CVICALLBACK ANOVAPanelCB (int panel, int event, void *callbackData, int eventData1, int eventData2)
{
	if (event == EVENT_CLOSE)
	{
		DiscardPanel (glbANOVAPanelHandle);
	}
	
	return 0;
}

/***************************************************************************//*!
* \brief ANOVA panel: for mode selection dropdown
*******************************************************************************/
int CVICALLBACK ANOVAModeSelectCB (int panel, int control, int event, void *callbackData, int eventData1, int eventData2)
{
	if (event == EVENT_VAL_CHANGED)
	{
		// Get ANOVA table display mode
		GetCtrlVal (glbANOVAPanelHandle, ANOVAPANEL_MODESELECT, &glbDisplayMode);
		
		// Call display function
		DisplayANOVATable ();
	}
	
	return 0;
}

/***************************************************************************//*!
* \brief ANOVA panel: Callback for save button
*******************************************************************************/
int CVICALLBACK ANOVASaveButtonCB (int panel, int control, int event, void *callbackData, int eventData1, int eventData2)
{
	if (event == EVENT_LEFT_CLICK)
	{
		// Save current data to struct
		SaveData data;
		memset (&data, 0, sizeof (data)); 
	
		data.numRows = glbNumRows;
		data.numCols = glbNumCols;
		data.numFactorCols = glbNumFactorCols;
		data.numDataCols = glbNumDataCols;
		memcpy (data.csvFilepath, glbCSVFilepath, 512);
		memcpy (data.factorRange, glbFactorRange, MAXFACTORCOLS * DATALENGTH);
		memcpy (data.dataRange, glbDataRange, MAXDATACOLS * DATALENGTH);
		memcpy (data.limitRange, glbLimitRange, MAXDATACOLS * DATALENGTH);
		data.anovaResult = glbANOVAResult;
		
		// Create .anova file
		char filename[256] = {0};
		time_t now = time(NULL);
		struct tm *t = localtime(&now);
		strftime(filename, 256, "..\\saves\\%Y-%m-%d_%H-%M-%S.anova", t);
		
		FILE *fp = fopen(filename, "wb");
	    if (!fp) 
		{
	        perror("Error opening file");
	        return 1;
	    }
		
		// Write SaveData struct and glbCSVData to file
	    fwrite (&data, sizeof(SaveData), 1, fp);
		for (int row = 0; row < glbNumRows; row++)
		{
			for (int col = 0; col < glbNumCols; col++)
			{
				fwrite (glbCSVData[row][col], sizeof (char), DATALENGTH, fp);
			}
		}
		
	    fclose (fp);	
		
		// Display message
		SetCtrlAttribute (glbANOVAPanelHandle, ANOVAPANEL_SAVETEXT, ATTR_VISIBLE, 1);
	}
	
	return 0;
}

/***************************************************************************//*!
* \brief ANOVA panel: Callback for export button
*******************************************************************************/
int CVICALLBACK ANOVAExportButtonCB (int panel, int control, int event, void *callbackData, int eventData1, int eventData2)
{
	if (event == EVENT_LEFT_CLICK)
	{
		// Get new CSV file
		char filePath[MAX_PATHNAME_LEN] = "";
		FileSelectPopup ("..\\", "*.csv", "*.csv", "Save Table As CSV", VAL_SAVE_BUTTON, 0, 1, 1, 1, filePath);
		if (strlen (filePath) == 0) return 0;
		
		FILE *fp = fopen(filePath, "w");
		if (!fp) 
		{
		    perror ("Failed to open file for writing.");
		    return 0;
		}

		// Iterate through table, export each cell value to CSV
		for (int row = 0; row < glbANOVAResult.numRows; row++) 
		{
		    for (int col = 0; col < glbNumDataCols + 1; col++) 
			{
		        char cellText[256] = "";
				double cellData = 0;
				void *dataPtr = NULL;
				
				if (col == 0)
				{
					dataPtr = cellText;
				}
				else 
				{
					dataPtr = &cellData;
				}
				
		        GetTableCellVal (glbANOVAPanelHandle, ANOVAPANEL_ANOVATABLE, MakePoint (col + 1, row + 1), dataPtr);
				
				if (col != 0)
				{
					sprintf (cellText, "%f", *(double *)dataPtr);
				}
		        
		        // Add quotes if necessary
		        if (strchr (cellText, ',') || strchr (cellText, '"')) 
				{
		            char temp[256];
		            sprintf (temp, "\"%s\"", cellText);
		            fprintf (fp, "%s", temp);
		        } 
				else 
				{
		            fprintf (fp, "%s", cellText);
		        }

		        if (col < glbNumDataCols)
				{
		            fprintf (fp, ",");
				}
		    }
		    fprintf (fp, "\n");
		}

		fclose(fp);
		
		// Display message
		SetCtrlAttribute (glbANOVAPanelHandle, ANOVAPANEL_EXPORTTEXT, ATTR_VISIBLE, 1);
	}
	
	return 0;
}

/***************************************************************************//*!
* \brief ANOVA panel: Callback for edit button
*******************************************************************************/
int CVICALLBACK ANOVAEditButtonCB (int panel, int control, int event, void *callbackData, int eventData1, int eventData2)
{
	if (event == EVENT_LEFT_CLICK)
	{
		DiscardPanel (glbANOVAPanelHandle);
		
		// Load CSV panel if not already loaded
		if (glbCSVPanelHandle <= 0)
		{
			glbCSVPanelHandle = LoadPanel (0, "CSVPanel.uir", CSVPANEL);
			DisplayCSVTable ();
			
			// Populate list boxes
			for (int i = 0; i < 3; i++)
			{
				int listBox = 0;
				int listDataLength = 0;
				char (*listData)[DATALENGTH] = NULL;
			
				switch (i)
				{
					case 0:
						listBox = CSVPANEL_FACTORLIST;
						listDataLength = glbNumFactorCols;
						listData = glbFactorRange;
						break;
						
					case 1:
						listBox = CSVPANEL_DATALIST;
						listDataLength = glbNumDataCols;
						listData = glbDataRange;
						break;
						
					case 2:
						listBox = CSVPANEL_LIMITLIST;
						listDataLength = glbNumDataCols;
						listData = glbLimitRange;
						break;
				}
				
				// Insert data into list
				for (int j = 0; j < listDataLength; j++)
				{
					InsertListItem (glbCSVPanelHandle, listBox, -1, listData[j], "");
					SetCtrlIndex (glbCSVPanelHandle, listBox, j);
				}
			}
		}
		
		DisplayPanel (glbCSVPanelHandle);
	}
	
	return 0;
}

/***************************************************************************//*!
* \brief Helper function that displays CSV panel and table
*******************************************************************************/
int DisplayCSVTable ()
{
	char errmsg[ERRLEN] = {0};
	fnInit;
	
	// Parse selected CSV, load into buffer
	int buffSize = DATALENGTH;
	char headerBuffer[64 * buffSize]; // Can account for 64 x 32-byte column headers
	char dataBuffer[64 * 64 * buffSize]; // Can account for 64 x 64 x 32-byte pieces of data
	
	tsErrChk (Initialize_CSVParse_LIB (glbCSVFilepath, 1, NULL, 0, buffSize, &glbNumCols, &glbNumRows, headerBuffer, errmsg), errmsg);
	tsErrChk (CSVParse_GetDataByIndex (0, 0, glbNumRows - 1, glbNumCols - 1, dataBuffer, errmsg), errmsg);

	// Populate main table, as well as glbCSVData
	glbNumRows++; // increment to account for header
	glbCSVData = malloc (sizeof (char **) * glbNumRows);
	for (int row = 1; row <= glbNumRows; row++)
	{
		glbCSVData[row - 1] = malloc (sizeof (char *) * glbNumCols);
		InsertTableRows (glbCSVPanelHandle, CSVPANEL_CSVTABLE, -1, 1, VAL_CELL_STRING);

		for (int col = 1; col <= glbNumCols; col++)
		{
			glbCSVData[row - 1][col - 1] = calloc (buffSize, sizeof (char));
			
			if (row == 1)
			{
				InsertTableColumns (glbCSVPanelHandle, CSVPANEL_CSVTABLE, -1, 1, VAL_CELL_STRING);
				SetTableCellVal (glbCSVPanelHandle, CSVPANEL_CSVTABLE, MakePoint (col, row), headerBuffer + (col - 1) * buffSize);
				memcpy (glbCSVData[row - 1][col - 1], headerBuffer + (col - 1) * buffSize, buffSize);
			}
			else
			{	
				SetTableCellVal (glbCSVPanelHandle, CSVPANEL_CSVTABLE, MakePoint (col, row), dataBuffer + (row - 2) * buffSize * glbNumCols + (col - 1) * buffSize);
				memcpy (glbCSVData[row - 1][col - 1], dataBuffer + (row - 2) * buffSize * glbNumCols + (col - 1) * buffSize, buffSize);
			}
		}
	}

Error:
	return error;
}

/***************************************************************************//*!
* \brief Helper function that gets ranges from list boxes
*******************************************************************************/
void GetDataFromListBoxes (IN int panel, char FactorSelection[][DATALENGTH], char DataSelection[][DATALENGTH], char LimitSelection[][DATALENGTH])
{	
	// Loop through each list box's indices, add to appropriate string array
	for (int i = 0; i < 3; i++)
	{
		int listBox = 0;
		int listLen = 0;
		char (*outputArr)[DATALENGTH] = NULL;
		
		switch (i)
		{
			case 0:
				listBox = CSVPANEL_FACTORLIST;
				listLen = glbNumFactorCols;
				outputArr = FactorSelection;
				break;
			case 1:
				listBox = CSVPANEL_DATALIST;
				listLen = glbNumDataCols;
				outputArr = DataSelection;
				break;
			case 2:
				listBox = CSVPANEL_LIMITLIST;
				listLen = glbNumDataCols;
				outputArr = LimitSelection;
				break;
		}
		
		for (int listIndex = 0; listIndex < listLen; listIndex++)
		{
			int numListItems = 0;
			GetNumListItems (panel, listBox, &numListItems);
			
			if (numListItems > 0)
			{
				GetLabelFromIndex (panel, listBox, listIndex, outputArr[listIndex]);
			}
		}
	}
}

/***************************************************************************//*!
* \brief Helper function that gets appropriate row name
*******************************************************************************/
void GetANOVATableRowName (IN int RowNum, char *RowName)
{	
	switch (RowNum)
	{
		case 0:		
			strcpy (RowName, "SumSqr_Parts_");
			break;
		case 1:		
			strcpy (RowName, "SumSqr_Repeat_");
			break;
		case 2:		
			strcpy (RowName, "DegFrd_Parts_");
			break;
		case 3:		
			strcpy (RowName, "DegFrd_Repeat_");
			break;
		case 4:		
			strcpy (RowName, "Variance_Parts_");
			break;
		case 5:		
			strcpy (RowName, "Variance_Repeat_");
			break;
		case 6:		
			strcpy (RowName, "StdDev_Parts_");
			break;
		case 7:		
			strcpy (RowName, "StdDev_Repeat_");
			break;
		case 8:		
			strcpy (RowName, "StdDev_Overall_");
			break;
		case 9:		
			strcpy (RowName, "PTRatio_Parts_");
			break;
		case 10:	
			strcpy (RowName, "PTRatio_Repeat_");
			break;
		case 11:	
			strcpy (RowName, "PTRatio_Overall_");
			break;
		default:	
			strcpy (RowName, "Error!!");
			break;
	}
}

/***************************************************************************//*!
* \brief Helper function that displays ANOVA data in table
*******************************************************************************/
void DisplayANOVATable ()
{	
	// Hide table and clear it if necessary
	SetCtrlAttribute (glbANOVAPanelHandle, ANOVAPANEL_ANOVATABLE, ATTR_VISIBLE, 0);
	
	int currentNumCols = 0;
	int currentNumRows = 0;
	GetNumTableColumns (glbANOVAPanelHandle, ANOVAPANEL_ANOVATABLE, &currentNumCols);
	GetNumTableRows (glbANOVAPanelHandle, ANOVAPANEL_ANOVATABLE, &currentNumRows);
	if (currentNumCols > 0)
	{
		DeleteTableColumns (glbANOVAPanelHandle, ANOVAPANEL_ANOVATABLE, 1, currentNumCols);
		DeleteTableRows (glbANOVAPanelHandle, ANOVAPANEL_ANOVATABLE, 1, currentNumRows);
	}

	// ANOVA overview mode
	if (glbDisplayMode == 0)
	{
		for (int col = 0; col < glbNumDataCols + 1; col++)
		{
			// Insert column
			int cellType = VAL_CELL_NUMERIC;
			if (col == 0)
			{
				cellType = VAL_CELL_STRING;
			}
			
			InsertTableColumns (glbANOVAPanelHandle, ANOVAPANEL_ANOVATABLE, -1, 1, cellType); 
			SetTableColumnAttribute (glbANOVAPanelHandle, ANOVAPANEL_ANOVATABLE, -1, ATTR_USE_LABEL_TEXT, 1);
			SetTableColumnAttribute (glbANOVAPanelHandle, ANOVAPANEL_ANOVATABLE, -1, ATTR_LABEL_TEXT, glbANOVAResult.colLabels[col]);
				
			for (int row = 0; row < NUMOVERVIEWROWS; row++)
			{				
				// If first column, insert rows and populate row labels
				if (col == 0)
				{
					InsertTableRows (glbANOVAPanelHandle, ANOVAPANEL_ANOVATABLE, -1, 1, VAL_CELL_STRING);
					SetTableCellVal (glbANOVAPanelHandle, ANOVAPANEL_ANOVATABLE, MakePoint (col + 1, row + 1), overviewRowLabels[row]);
				}
				
				// If not first column, insert data
				else
				{						
					SetTableCellAttribute (glbANOVAPanelHandle, ANOVAPANEL_ANOVATABLE, MakePoint (col + 1, row + 1), ATTR_FORMAT, VAL_SCIENTIFIC_FORMAT);
					SetTableCellAttribute (glbANOVAPanelHandle, ANOVAPANEL_ANOVATABLE, MakePoint (col + 1, row + 1), ATTR_PRECISION, 3);
					double cellData = 0;
					switch (row)
					{
						case 0:
							cellData = glbANOVAResult.varianceReprod[row][col - 1];
							break;
							
						case 1:
							cellData = glbANOVAResult.varianceRepeat[row][col - 1];
							break;
							
						case 2:
							cellData = glbANOVAResult.varianceTotal[row][col - 1];
							break;
							
						case 3:
							cellData = 0; // TODO: Variance %
							break;
							
						case 4:
							cellData = glbANOVAResult.stdDevReprod[row][col - 1];
							break;
							
						case 5:
							cellData = glbANOVAResult.stdDevRepeat[row][col - 1];
							break;
							
						case 6:
							cellData = glbANOVAResult.stdDevTotal[row][col - 1];
							break;
							
						case 7:
							cellData = glbANOVAResult.ptRatioReprod[row][col - 1];
							break;
							
						case 8:
							cellData = glbANOVAResult.ptRatioRepeat[row][col - 1];
							break;
							
						case 9:
							cellData = glbANOVAResult.ptRatioTotal[row][col - 1];
							break;
							
						default:
							cellData = 0;
					}
					SetTableCellVal (glbANOVAPanelHandle, ANOVAPANEL_ANOVATABLE, MakePoint (col + 1, row + 1), cellData);	
				}
			}
			
			SetColumnWidthToWidestCellContents (glbANOVAPanelHandle, ANOVAPANEL_ANOVATABLE, col + 1);
		}
	}
	
	// Intermediate value view mode
	else 
	{
		for (int col = 0; col < glbNumDataCols + 1; col++)
		{
			// Insert column
			int cellType = VAL_CELL_NUMERIC;
			if (col == 0)
			{
				cellType = VAL_CELL_STRING;
			}
			
			InsertTableColumns (glbANOVAPanelHandle, ANOVAPANEL_ANOVATABLE, -1, 1, cellType); 
			SetTableColumnAttribute (glbANOVAPanelHandle, ANOVAPANEL_ANOVATABLE, -1, ATTR_USE_LABEL_TEXT, 1);
			SetTableColumnAttribute (glbANOVAPanelHandle, ANOVAPANEL_ANOVATABLE, -1, ATTR_LABEL_TEXT, glbANOVAResult.colLabels[col]);
				
			for (int row = 0; row < glbANOVAResult.numRows; row++)
			{				
				// If first column, insert rows and populate row labels
				if (col == 0)
				{
					InsertTableRows (glbANOVAPanelHandle, ANOVAPANEL_ANOVATABLE, -1, 1, VAL_CELL_STRING);
					
					char rowLabel[256] = {0};
					GetANOVATableRowName (row % NUMINTERMEDROWS, rowLabel);
					strcat (rowLabel, glbANOVAResult.rowLabels[row / NUMINTERMEDROWS]);

					SetTableCellVal (glbANOVAPanelHandle, ANOVAPANEL_ANOVATABLE, MakePoint (col + 1, row + 1), rowLabel);
				}
				
				// If not first column, insert data
				else
				{						
					SetTableCellAttribute (glbANOVAPanelHandle, ANOVAPANEL_ANOVATABLE, MakePoint (col + 1, row + 1), ATTR_FORMAT, VAL_SCIENTIFIC_FORMAT);
					SetTableCellAttribute (glbANOVAPanelHandle, ANOVAPANEL_ANOVATABLE, MakePoint (col + 1, row + 1), ATTR_PRECISION, 3);
					double cellData = 0;
					switch (row % NUMINTERMEDROWS)
					{
						case 0:
							cellData = glbANOVAResult.sumSqr[row / NUMINTERMEDROWS][col - 1];
							break;
							
						case 1:
							cellData = glbANOVAResult.degFrd[row / NUMINTERMEDROWS];
							break;
							
						case 2:
							cellData = glbANOVAResult.meanSqr[row / NUMINTERMEDROWS][col - 1];
							break;
							
						// TODO: add more cases for equipment
							
						default:
							cellData = 0;
					}
					SetTableCellVal (glbANOVAPanelHandle, ANOVAPANEL_ANOVATABLE, MakePoint (col + 1, row + 1), cellData);
						
				}
			}
			
			SetColumnWidthToWidestCellContents (glbANOVAPanelHandle, ANOVAPANEL_ANOVATABLE, col + 1);
		}
	}
	
	// Show table
	SetCtrlAttribute (glbANOVAPanelHandle, ANOVAPANEL_ANOVATABLE, ATTR_VISIBLE, 1);
}
//! \cond
/// REGION END
//! \endcond