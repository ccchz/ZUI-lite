#ifndef __SplitterBar_H__
#define __SplitterBar_H__
#include <ZUI.h>
/**分割条结构*/
typedef struct _ZSplitterBar
{
    ZuiBool m_bMouseDown;
    int m_dwType;
    ZCtlProc old_call;
}*ZuiSplitterBar, ZSplitterBar;
ZEXPORT ZINT ZCALL ZuiSplitterBarProc(ZINT ProcId, ZuiControl cp, ZuiSplitterBar p, ZPARAM Param1, ZPARAM Param2);


#endif	//__SplitterBar_H__
