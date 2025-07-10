/**************************************************************************/
/* LabWindows/CVI User Interface Resource (UIR) Include File              */
/*                                                                        */
/* WARNING: Do not add to, delete from, or otherwise modify the contents  */
/*          of this include file.                                         */
/**************************************************************************/

#include <userint.h>

#ifdef __cplusplus
    extern "C" {
#endif

     /* Panels and Controls: */

#define  CSVPANEL                         1       /* callback function: CSVPanelCB */
#define  CSVPANEL_CSVTABLE                2       /* control type: table, callback function: CSVTableCB */
#define  CSVPANEL_FACTORLIST              3       /* control type: listBox, callback function: (none) */
#define  CSVPANEL_FACTORDELETEBUTTON      4       /* control type: command, callback function: FactorDeleteButtonCB */
#define  CSVPANEL_FACTORSELECTBUTTON      5       /* control type: command, callback function: FactorSelectButtonCB */
#define  CSVPANEL_FACTORSELECT            6       /* control type: string, callback function: (none) */
#define  CSVPANEL_FACTORTEXT              7       /* control type: textMsg, callback function: (none) */


     /* Control Arrays: */

          /* (no control arrays in the resource file) */


     /* Menu Bars, Menus, and Menu Items: */

          /* (no menu bars in the resource file) */


     /* Callback Prototypes: */

int  CVICALLBACK CSVPanelCB(int panel, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK CSVTableCB(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK FactorDeleteButtonCB(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK FactorSelectButtonCB(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);


#ifdef __cplusplus
    }
#endif