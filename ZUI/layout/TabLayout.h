#ifndef __TABLAYOUT_H__
#define __TABLAYOUT_H__
#include <ZUI.h>

/**选择夹布局结构*/
typedef struct _ZuiTabLayout
{
    int m_iCurSel;//当前选择夹
    ZCtlProc old_call;
    ZVoid old_udata;
}*ZuiTabLayout, ZTabLayout;

ZINT ZCALL ZuiTabLayoutProc(ZINT ProcId, ZuiControl cp, ZuiTabLayout p, ZPARAM Param1, ZPARAM Param2);

#endif // __UITABLAYOUT_H__
