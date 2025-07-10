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

int glbCSVPanelHandle = 0;
int glbANOVAPanelHandle = 0;

//==============================================================================
// Global functions

//! \cond
/// REGION END

/// REGION START Code Body
//! \endcond
/***************************************************************************//*!
* \brief Example callback
*******************************************************************************/
int Define_Your_Functions_Here (int x)
{
	return x;
}

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
		char selected_filepath[512];
		int status = 0;
		status = FileSelectPopup ("", "*.csv", "*.*", "Select a CSV file...", VAL_OK_BUTTON, 0, 1, 1, 0, selected_filepath);
		
		// Load CSV panel
		glbCSVPanelHandle = LoadPanel (0, "CSVPanel.uir", CSVPANEL);
		
		// Parse selected CSV, load into buffer
		int colCount = 0;
		int rowCount = 0;
		int buffSize = 32;
		char headerBuffer[2048]; // Can account for 64 x 32-byte column headers
		char dataBuffer[131072]; // Can account for 64 x 64 x 32-byte pieces of data
		
		tsErrChk (Initialize_CSVParse_LIB (selected_filepath, 1, NULL, 0, buffSize, &colCount, &rowCount, headerBuffer, errmsg), errmsg);
		tsErrChk (CSVParse_GetDataByIndex (0, 0, rowCount - 1, colCount - 1, dataBuffer, errmsg), errmsg);

		// Populate main table
		for (int row = 1; row <= rowCount + 1; row++)
		{
			status = InsertTableRows (glbCSVPanelHandle, CSVPANEL_CSVTABLE, -1, 1, VAL_CELL_STRING);

			for (int col = 1; col <= colCount; col++)
			{
				if (row == 1)
				{
					status = InsertTableColumns (glbCSVPanelHandle, CSVPANEL_CSVTABLE, -1, 1, VAL_CELL_STRING);
					SetTableCellVal (glbCSVPanelHandle, CSVPANEL_CSVTABLE, MakePoint (col, row), headerBuffer + (col - 1) * buffSize);
				}
				else
				{	
					SetTableCellVal (glbCSVPanelHandle, CSVPANEL_CSVTABLE, MakePoint (col, row), dataBuffer + (row - 2) * buffSize * colCount + (col - 1) * buffSize);
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
	if (event == EVENT_LEFT_CLICK_UP)
	{
		Rect selection_rect = {0};
		int status = 0;
		status = GetTableSelection (glbCSVPanelHandle, CSVPANEL_CSVTABLE, &selection_rect);
		
		// Display selection
		if (selection_rect.height == -1 && selection_rect.width == -1)
		{
			SetCtrlVal (glbCSVPanelHandle, CSVPANEL_FACTORSELECT, "");
		}
		else {
			char selection_str[32] = {0};
			status = sprintf (selection_str, "C%dR%d:C%dR%d", selection_rect.left, selection_rect.top, selection_rect.left + selection_rect.width - 1, selection_rect.top + selection_rect.height - 1);
			SetCtrlVal (glbCSVPanelHandle, CSVPANEL_FACTORSELECT, selection_str);
		}
	}
	
	return 0;
}

/***************************************************************************//*!
* \brief Callback for factor select button
*******************************************************************************/
int CVICALLBACK FactorSelectButtonCB(int panel, int control, int event, void *callbackData, int eventData1, int eventData2)
{
	if (event == EVENT_LEFT_CLICK)
	{
		int status = 0;
		char factor_range[32] = {0};
		int list_size = 0;
		
		// Add selection to list
		GetCtrlVal (glbCSVPanelHandle, CSVPANEL_FACTORSELECT, factor_range);
		if (factor_range != "")
		{
			GetNumListItems (glbCSVPanelHandle, CSVPANEL_FACTORLIST, &list_size);
			char list_size_str[16] = {0};
			sprintf (list_size_str, "%d", list_size);
			status = InsertListItem (glbCSVPanelHandle, CSVPANEL_FACTORLIST, -1, factor_range, "");
		}
	}
	
	return 0;
}

/***************************************************************************//*!
* \brief Callback for factor delete button
*******************************************************************************/
int CVICALLBACK FactorDeleteButtonCB(int panel, int control, int event, void *callbackData, int eventData1, int eventData2)
{
	if (event == EVENT_LEFT_CLICK)
	{
		int status = 0; 
		int selection_index = 0;
		
		// Add selection to list
		GetCtrlIndex (glbCSVPanelHandle, CSVPANEL_FACTORLIST, &selection_index);
		if (selection_index != -1)
		{
			status = DeleteListItem (glbCSVPanelHandle, CSVPANEL_FACTORLIST, selection_index, 1);
		}
	}
	
	return 0;
}

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