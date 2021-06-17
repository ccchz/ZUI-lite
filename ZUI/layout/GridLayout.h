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

void* ZCALL ZuiGridLayoutProc(int ProcId, ZuiControl cp, ZuiGridLayout p, void* Param1, void* Param2);

#endif // __GRID_H__