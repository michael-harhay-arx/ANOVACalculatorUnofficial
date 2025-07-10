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
#define  CSVPANEL_LIMITLIST               3       /* control type: listBox, callback function: CSVListCB */
#define  CSVPANEL_LIMITDELETEBUTTON       4       /* control type: command, callback function: CSVDeleteButtonCB */
#define  CSVPANEL_LIMITSELECTBUTTON       5       /* control type: command, callback function: CSVSelectButtonCB */
#define  CSVPANEL_LIMITSELECT             6       /* control type: string, callback function: (none) */
#define  CSVPANEL_DATALIST                7       /* control type: listBox, callback function: CSVListCB */
#define  CSVPANEL_DATADELETEBUTTON        8       /* control type: command, callback function: CSVDeleteButtonCB */
#define  CSVPANEL_DATASELECTBUTTON        9       /* control type: command, callback function: CSVSelectButtonCB */
#define  CSVPANEL_DATASELECT              10      /* control type: string, callback function: (none) */
#define  CSVPANEL_FACTORLIST              11      /* control type: listBox, callback function: CSVListCB */
#define  CSVPANEL_FACTORDELETEBUTTON      12      /* control type: command, callback function: CSVDeleteButtonCB */
#define  CSVPANEL_FACTORSELECTBUTTON      13      /* control type: command, callback function: CSVSelectButtonCB */
#define  CSVPANEL_FACTORSELECT            14      /* control type: string, callback function: (none) */
#define  CSVPANEL_FACTORTEXT              15      /* control type: textMsg, callback function: (none) */
#define  CSVPANEL_DATATEXT                16      /* control type: textMsg, callback function: (none) */
#define  CSVPANEL_LIMITTEXT               17      /* control type: textMsg, callback function: (none) */


     /* Control Arrays: */

          /* (no control arrays in the resource file) */


     /* Menu Bars, Menus, and Menu Items: */

          /* (no menu bars in the resource file) */


     /* Callback Prototypes: */

int  CVICALLBACK CSVDeleteButtonCB(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK CSVListCB(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK CSVPanelCB(int panel, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK CSVSelectButtonCB(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK CSVTableCB(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);


#ifdef __cplusplus
    }
#endif