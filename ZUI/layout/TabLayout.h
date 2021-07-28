﻿#ifndef __TABLAYOUT_H__
#define __TABLAYOUT_H__
#include <ZUI.h>

/**选择夹布局结构*/
typedef struct _ZuiTabLayout
{
    int m_iCurSel;//当前选择夹
    ZCtlProc old_call;
    ZuiAny old_udata;
}*ZuiTabLayout, ZTabLayout;

ZuiAny ZCALL ZuiTabLayoutProc(int ProcId, ZuiControl cp, ZuiTabLayout p, ZuiAny Param1, ZuiAny Param2);

#endif // __UITABLAYOUT_H__
