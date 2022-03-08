#ifndef __DrawPanel_H__
#define __DrawPanel_H__
#include <ZUI.h>

/**按钮控件结构*/
typedef struct _ZDrawPanel
{
    ZCtlProc old_call;
}*ZuiDrawPanel, ZDrawPanel;
ZEXPORT ZINT ZCALL ZuiDrawPanelProc(ZINT ProcId, ZuiControl cp, ZuiDrawPanel p, ZPARAM Param1, ZPARAM Param2);


#endif	//__LABEL_H__
