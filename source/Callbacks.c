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

//#include "Callbacks.h"

#include "toolbox.h"
#include <ansi_c.h>

#include "Callbacks.h"
#include "CSVParse_LIB.h"

#include "SystemLog_LIB.h"

//==============================================================================
// Constants

//==============================================================================
// Types

//==============================================================================
// Static global variables

/***************************************************************************//*!
* \brief Stores the log level used for SYSLOG macro
*******************************************************************************/
static int glbSysLogLevel = 0;

//==============================================================================
// Static functions

//==============================================================================
// Global variables

// GUI variables
int glbCSVPanelHandle = 0;
int glbANOVAPanelHandle = 0;

// CSV file variables
int glbRowCount = 0;
char ***glbCSVData= NULL;

//==============================================================================
// Global functions

//! \cond
/// REGION END

/// REGION START Code Body
//! \endcond
/***************************************************************************//*!
* \brief Callback for closing main panel
*******************************************************************************/
int CVICALLBACK MainPanelCB (int panel, int event, void *callbackData, int eventData1, int eventData2)
{
	if (event == EVENT_CLOSE)
	{
		QuitUserInterface (0);
	}
	
	return 0;
}

/***************************************************************************//*!
* \brief Open button callback
*******************************************************************************/
int CVICALLBACK OpenButtonCB(int panel, int control, int event, void *callbackData, int eventData1, int eventData2)
{
	char errmsg[ERRLEN] = {0};
	fnInit;
	
	if (event == EVENT_LEFT_CLICK)
	{
		// Open file selection dialogue
		char selectedFilepath[512];
		FileSelectPopup ("", "*.csv", "*.*", "Select a CSV file...", VAL_OK_BUTTON, 0, 1, 1, 0, selectedFilepath);
		
		// Load CSV panel
		glbCSVPanelHandle = LoadPanel (0, "CSVPanel.uir", CSVPANEL);
		
		// Parse selected CSV, load into buffer
		int colCount = 0;
		int buffSize = 32;
		char headerBuffer[2048]; // Can account for 64 x 32-byte column headers
		char dataBuffer[131072]; // Can account for 64 x 64 x 32-byte pieces of data
		
		tsErrChk (Initialize_CSVParse_LIB (selectedFilepath, 1, NULL, 0, buffSize, &colCount, &glbRowCount, headerBuffer, errmsg), errmsg);
		tsErrChk (CSVParse_GetDataByIndex (0, 0, glbRowCount - 1, colCount - 1, dataBuffer, errmsg), errmsg);

		// Populate main table, as well as glbCSVData
		glbCSVData = malloc (sizeof (char **) * (glbRowCount + 1));
		for (int row = 1; row <= glbRowCount + 1; row++)
		{
			glbCSVData[row - 1] = malloc (sizeof (char *) * colCount);
			InsertTableRows (glbCSVPanelHandle, CSVPANEL_CSVTABLE, -1, 1, VAL_CELL_STRING);

			for (int col = 1; col <= colCount; col++)
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
					SetTableCellVal (glbCSVPanelHandle, CSVPANEL_CSVTABLE, MakePoint (col, row), dataBuffer + (row - 2) * buffSize * colCount + (col - 1) * buffSize);
					memcpy (glbCSVData[row - 1][col - 1], dataBuffer + (row - 2) * buffSize * colCount + (col - 1) * buffSize, buffSize);
				}
			}
		}

		DisplayPanel (glbCSVPanelHandle);
	}
	
Error:
	return error;
}

/***************************************************************************//*!
* \brief Load button callback
*******************************************************************************/
int CVICALLBACK LoadButtonCB(int panel, int control, int event, void *callbackData, int eventData1, int eventData2)
{
	if (event == EVENT_LEFT_CLICK)
	{
		// Load ANOVA panel
		glbANOVAPanelHandle = LoadPanel (0, "ANOVAPanel.uir", ANOVAPANEL);
		DisplayPanel (glbANOVAPanelHandle);
		// TODO add "loading" sign?
	}
	
	return 0;
}

/***************************************************************************//*!
* \brief Callback for closing CSV panel
*******************************************************************************/
int CVICALLBACK CSVPanelCB (int panel, int event, void *callbackData, int eventData1, int eventData2)
{
	if (event == EVENT_CLOSE)
	{
		DiscardPanel (glbCSVPanelHandle);
	}
	
	return 0;
}

/***************************************************************************//*!
* \brief Callback for making table selections
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
		char selectionStr[32] = {0};
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
* \brief Callback for factor/data/limit select button
*******************************************************************************/
int CVICALLBACK CSVSelectButtonCB(int panel, int control, int event, void *callbackData, int eventData1, int eventData2)
{
	if (event == EVENT_LEFT_CLICK)
	{
		int selectionTextBox = 0;
		int listBox = 0;
		char selection[32] = {0};
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
		char itemLabel[32];
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
* \brief Callback for factor/data/limit delete button
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
* \brief Callback for factor/data/limit list boxes
*******************************************************************************/
int CVICALLBACK CSVListCB(int panel, int control, int event, void *callbackData, int eventData1, int eventData2)
{
	if (event == EVENT_VAL_CHANGED || event == EVENT_GOT_FOCUS)
	{
		// Get active list index
		int activeListIndex = 0;
		char activeListLabel[32] = {0};
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
* \brief Callback for ANOVA calculation button
*******************************************************************************/
int CVICALLBACK CSVCalcButtonCB(int panel, int control, int event, void *callbackData, int eventData1, int eventData2)
{
	char errmsg[ERRLEN] = {0};
	fnInit;
	error = 0;
	
	if (event == EVENT_LEFT_CLICK)
	{
		// Load ANOVA panel
		glbANOVAPanelHandle = LoadPanel (0, "ANOVAPanel.uir", ANOVAPANEL);

		// TODO add "loading" sign?
		
		// Get factors/data/limits from UI
		// TODO FIX DATA LOADING INTO PARSE FUNCTION
		//GetDataFromListBoxes (panel, factorRange, dataRange, limitRange);

		// Parse selected factors/data/limits
		
		// MANUAL INPUT FOR NOW
		ANOVANode *treeRoot = CreateANOVANode ("Root");
		char factorRange[][32] = {
		    "C1R9:C1R39",
		    "C7R9:C7R39",
			"0"
		};

		char dataRange[][32] = {
		    "C11R9:C11R39",
		    "C12R9:C12R39",
			"0"
		};

		char limitRange[][32] = {
		    "C11R6:C11R7",
		    "C12R6:C12R7",
			"0"
		};

		ParseCSVSelection (panel, factorRange, dataRange, limitRange, treeRoot); // TODO add support for multi-col data selections... but can assume factors are one col
		
		// Perform calculations
		//tsErrChk (ComputeANOVA (treeRoot), errmsg);
		
		// Load calculations into ANOVA table
		
		
		//DisplayPanel (glbANOVAPanelHandle);
	}

Error:
	return error;
}

/***************************************************************************//*!
* \brief Helper function that gets 
*******************************************************************************/
/*
void GetDataFromListBoxes (IN int panel, char **FactorSelection, char **DataSelection, char **LimitSelection)
{
	// Get length of each list box
	int factorListLen = 0;
	int dataListLen = 0;
	int limitListLen = 0;
	
	GetNumListItems (panel, CSVPANEL_FACTORLIST, &factorListLen);
	GetNumListItems (panel, CSVPANEL_DATALIST, &dataListLen);
	GetNumListItems (panel, CSVPANEL_LIMITLIST, &limitListLen);
	
	// Initialize string arrays
	char factorRange[factorListLen][32];
	char dataRange[dataListLen][32];;
	char limitRange[limitListLen][32];
	
	// Loop through each list box's indices, add to appropriate string array
	for (int i = 0; i < 3; i++)
	{
		int listBox = 0;
		int listLen = 0;
		char **outputArr = NULL;
		
		switch (i)
		{
			case 0:
				listBox = CSVPANEL_FACTORLIST;
				listLen = factorListLen;
				outputArr = FactorSelection;
				break;
			case 1:
				listBox = CSVPANEL_DATALIST;
				listLen = dataListLen;
				outputArr = DataSelection;
				break;
			case 2:
				listBox = CSVPANEL_LIMITLIST;
				listLen = limitListLen;
				outputArr = LimitSelection;
				break;
		}
		
		for (int listIndex = 0; listIndex < listLen; listIndex++)
		{
			GetLabelFromIndex (panel, listBox, listIndex, outputArr[listIndex]);
		}
	}
}
*/

/***************************************************************************//*!
* \brief Callback for closing ANOVA panel
*******************************************************************************/
int CVICALLBACK ANOVAPanelCB (int panel, int event, void *callbackData, int eventData1, int eventData2)
{
	if (event == EVENT_CLOSE)
	{
		DiscardPanel (glbANOVAPanelHandle);
	}
	
	return 0;
}
//! \cond
/// REGION END
//! \endcond