#ifndef __GRID_H__
#define __GRID_H__
#include <ZUI.h>
/**网格布局结构*/

#define GRIDSIZE 48

typedef struct _ZuiGridLayout
{
    ZSize m_szGridSize;

    ZCtlProc old_call;
    ZVoid old_udata;
}*ZuiGridLayout, ZGridLayout;

ZINT ZCALL ZuiGridLayoutProc(ZINT ProcId, ZuiControl cp, ZuiGridLayout p, ZPARAM Param1, ZPARAM Param2);

#endif // __GRID_H__