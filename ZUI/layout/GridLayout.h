#ifndef __GRID_H__
#define __GRID_H__
#include <ZUI.h>
/**网格布局结构*/

#define GRIDSIZE 48

typedef struct _ZuiGridLayout
{
    ZSize m_szGridSize;

    ZCtlProc old_call;
    ZuiAny old_udata;
}*ZuiGridLayout, ZGridLayout;

ZuiAny ZCALL ZuiGridLayoutProc(int ProcId, ZuiControl cp, ZuiGridLayout p, ZuiAny Param1, ZuiAny Param2);

#endif // __GRID_H__