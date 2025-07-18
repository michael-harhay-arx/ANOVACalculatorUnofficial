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

#define  ANOVAPANEL                       1       /* callback function: ANOVAPanelCB */
#define  ANOVAPANEL_ANOVATABLE            2       /* control type: table, callback function: (none) */
#define  ANOVAPANEL_EDITBUTTON            3       /* control type: command, callback function: ANOVAEditButtonCB */
#define  ANOVAPANEL_SAVEBUTTON            4       /* control type: command, callback function: ANOVASaveButtonCB */
#define  ANOVAPANEL_SAVETEXT              5       /* control type: textMsg, callback function: (none) */


     /* Control Arrays: */

          /* (no control arrays in the resource file) */


     /* Menu Bars, Menus, and Menu Items: */

          /* (no menu bars in the resource file) */


     /* Callback Prototypes: */

int  CVICALLBACK ANOVAEditButtonCB(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK ANOVAPanelCB(int panel, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK ANOVASaveButtonCB(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);


#ifdef __cplusplus
    }
#endif