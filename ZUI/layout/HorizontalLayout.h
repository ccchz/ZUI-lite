#ifndef __HORIZONTALLAYOUT_H__
#define __HORIZONTALLAYOUT_H__
#include <ZUI.h>
/**横向布局结构*/
typedef struct _ZuiHorizontalLayout
{
    ZCtlProc old_call;
    ZVoid old_udata;
}*ZuiHorizontalLayout, ZHorizontalLayout;

ZINT ZCALL ZuiHorizontalLayoutProc(ZINT ProcId, ZuiControl cp, ZuiHorizontalLayout p, ZPARAM Param1, ZPARAM Param2);

#endif // __HORIZONTALLAYOUT_H__
