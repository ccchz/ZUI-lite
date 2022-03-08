#ifndef __UIWINDOW_H__
#define __UIWINDOW_H__
#include <ZUI.h>
#include <platform/platform.h>

/**窗口控件结构*/
typedef struct _ZWindow
{    
    ZuiOsWindow m_osWindow;    
    
    ZCtlProc old_call;
    ZVoid old_udata;
}*ZuiWindow, ZWindow;
ZEXPORT ZINT ZCALL ZuiWindowProc(ZINT ProcId, ZuiControl cp, ZuiWindow p, ZPARAM Param1, ZPARAM Param2);
//查找窗口
ZEXPORT ZuiControl ZCALL ZuiWindowFindName(ZuiText Name);

#endif	//__UIWINDOW_H__