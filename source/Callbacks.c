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
		int selection_status = FileSelectPopup ("", "*.csv", "*.*", "Select a CSV file...", VAL_OK_BUTTON, 0, 1, 1, 0, selected_filepath);
		
		// Parse CSV, load into buffer
		int colCount = 0;
		int rowCount = 0;
		int buffSize = 64;
		char headerBuffer[2048]; // TODO fix sizing
		char dataBuffer[500000]; // TODO fix sizing
		
		tsErrChk (Initialize_CSVParse_LIB (selected_filepath, 1, NULL, 0, buffSize, &colCount, &rowCount, headerBuffer, errmsg), errmsg);
		tsErrChk (CSVParse_GetDataByIndex (0, 0, rowCount - 1, colCount - 1, dataBuffer, errmsg), errmsg);
		
		// Load CSV panel
		glbCSVPanelHandle = LoadPanel (0, "CSVPanel.uir", CSVPANEL);
		// TODO add "loading" sign?

		// Add data to CSV table
		for (int row = 1; row <= rowCount + 1; row++)
		{
			int status = 0;
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
		NULL;
	}
	
	return 0;
}
//! \cond
/// REGION END
//! \endcond