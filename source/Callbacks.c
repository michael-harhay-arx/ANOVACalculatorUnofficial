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
		FileSelectPopup ("", "*.csv", "*.*", "Select a CSV file...", VAL_OK_BUTTON, 0, 1, 1, 0, selected_filepath);
		
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
			InsertTableRows (glbCSVPanelHandle, CSVPANEL_CSVTABLE, -1, 1, VAL_CELL_STRING);

			for (int col = 1; col <= colCount; col++)
			{
				if (row == 1)
				{
					InsertTableColumns (glbCSVPanelHandle, CSVPANEL_CSVTABLE, -1, 1, VAL_CELL_STRING);
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
	if (event == EVENT_SELECTION_CHANGE || event == EVENT_ACTIVE_CELL_CHANGE)
	{
		Rect selection_rect = {0};
		GetTableSelection (panel, CSVPANEL_CSVTABLE, &selection_rect);
		
		// If no selection, print nothing
		if (selection_rect.height == -1 && selection_rect.width == -1)
		{
			SetCtrlVal (panel, CSVPANEL_FACTORSELECT, "");
			SetCtrlVal (panel, CSVPANEL_DATASELECT, "");
			SetCtrlVal (panel, CSVPANEL_LIMITSELECT, "");
			return 0;
		}
		
		// Else print selected cell/range
		char selection_str[32] = {0};
		if (selection_rect.height == 0 && selection_rect.width == 0) // one cell selected, have to use GetActiveTableCell()
		{
			Point active_cell = {0};
			GetActiveTableCell (panel, CSVPANEL_CSVTABLE, &active_cell);
			sprintf (selection_str, "C%dR%d:C%dR%d", active_cell.x, active_cell.y, active_cell.x, active_cell.y);
		}
		else 
		{
			sprintf (selection_str, "C%dR%d:C%dR%d", selection_rect.left, selection_rect.top, selection_rect.left + selection_rect.width - 1, selection_rect.top + selection_rect.height - 1);
		}
		
		SetCtrlVal (panel, CSVPANEL_FACTORSELECT, selection_str);
		SetCtrlVal (panel, CSVPANEL_DATASELECT, selection_str);
		SetCtrlVal (panel, CSVPANEL_LIMITSELECT, selection_str);
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
		int selection_text_box = 0;
		int list_box = 0;
		char selection[32] = {0};
		int list_size = 0;
		
		// Set appropriate text box and list box
		switch (control)
		{
			case CSVPANEL_FACTORSELECTBUTTON:
                selection_text_box = CSVPANEL_FACTORSELECT;
				list_box = CSVPANEL_FACTORLIST;
                break;

            case CSVPANEL_DATASELECTBUTTON:
                selection_text_box = CSVPANEL_DATASELECT;
				list_box = CSVPANEL_DATALIST;
                break;

            case CSVPANEL_LIMITSELECTBUTTON:
                selection_text_box = CSVPANEL_LIMITSELECT;
				list_box = CSVPANEL_LIMITLIST;
                break;
		}
		
		// Get selection from text box
		GetCtrlVal (panel, selection_text_box, selection);
		GetNumListItems (panel, list_box, &list_size);
		
		// Do not insert if duplicate item already exists in list
		char item_label[32];
		for (int i = 0; i < list_size; i++)
		{
			GetLabelFromIndex (panel, list_box, i, item_label);
			if (strcmp (selection, item_label) == 0)
			{
				SetCtrlIndex (panel, list_box, i);
				return 0;
			}
		}
		
		// Insert selection into list
		if (selection != "")
		{
			InsertListItem (panel, list_box, -1, selection, "");
			SetCtrlIndex (panel, list_box, list_size);
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
		int list_box = 0;
		int selection_index = 0;
		
		// Set appropriate list box
		switch (control)
		{
			case CSVPANEL_FACTORDELETEBUTTON:
				list_box = CSVPANEL_FACTORLIST;
                break;

            case CSVPANEL_DATADELETEBUTTON:
				list_box = CSVPANEL_DATALIST;
                break;

            case CSVPANEL_LIMITDELETEBUTTON:
				list_box = CSVPANEL_LIMITLIST;
                break;
		}
		
		// Add selection to list
		GetCtrlIndex (panel, list_box, &selection_index);
		if (selection_index != -1)
		{
			DeleteListItem (panel, list_box, selection_index, 1);
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
		int active_list_index = 0;
		char active_list_label[32] = {0};
		GetCtrlIndex (panel, control, &active_list_index);
		GetLabelFromIndex (panel, control, active_list_index, active_list_label);
		
		// Highlight corresponding selection in table
		Rect active_cell_rect = {0};
		int right = 0;
		int bottom = 0;
		
		sscanf (active_list_label, "C%dR%d:C%dR%d", &active_cell_rect.left, &active_cell_rect.top, &right, &bottom);
		active_cell_rect.width = right - active_cell_rect.left + 1;
		active_cell_rect.height = bottom - active_cell_rect.top + 1;
		
		SetTableSelection (panel, CSVPANEL_CSVTABLE, active_cell_rect);
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