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
#define  CSVPANEL_CALCBUTTON              3       /* control type: command, callback function: CSVCalcButtonCB */
#define  CSVPANEL_LIMITLIST               4       /* control type: listBox, callback function: CSVListCB */
#define  CSVPANEL_LIMITDELETEBUTTON       5       /* control type: command, callback function: CSVDeleteButtonCB */
#define  CSVPANEL_LIMITSELECTBUTTON       6       /* control type: command, callback function: CSVSelectButtonCB */
#define  CSVPANEL_LIMITSELECT             7       /* control type: string, callback function: (none) */
#define  CSVPANEL_DATALIST                8       /* control type: listBox, callback function: CSVListCB */
#define  CSVPANEL_DATADELETEBUTTON        9       /* control type: command, callback function: CSVDeleteButtonCB */
#define  CSVPANEL_DATASELECTBUTTON        10      /* control type: command, callback function: CSVSelectButtonCB */
#define  CSVPANEL_DATASELECT              11      /* control type: string, callback function: (none) */
#define  CSVPANEL_FACTORLIST              12      /* control type: listBox, callback function: CSVListCB */
#define  CSVPANEL_FACTORDELETEBUTTON      13      /* control type: command, callback function: CSVDeleteButtonCB */
#define  CSVPANEL_FACTORSELECTBUTTON      14      /* control type: command, callback function: CSVSelectButtonCB */
#define  CSVPANEL_FACTORSELECT            15      /* control type: string, callback function: (none) */
#define  CSVPANEL_FACTORTEXT              16      /* control type: textMsg, callback function: (none) */
#define  CSVPANEL_DATATEXT                17      /* control type: textMsg, callback function: (none) */
#define  CSVPANEL_LIMITTEXT               18      /* control type: textMsg, callback function: (none) */


     /* Control Arrays: */

          /* (no control arrays in the resource file) */


     /* Menu Bars, Menus, and Menu Items: */

          /* (no menu bars in the resource file) */


     /* Callback Prototypes: */

int  CVICALLBACK CSVCalcButtonCB(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK CSVDeleteButtonCB(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK CSVListCB(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK CSVPanelCB(int panel, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK CSVSelectButtonCB(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK CSVTableCB(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);


#ifdef __cplusplus
    }
#endif