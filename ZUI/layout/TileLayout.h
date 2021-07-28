#ifndef __TILELAYOUT_H__
#define __TILELAYOUT_H__
#include <ZUI.h>
/**瓦片布局结构*/
typedef struct _ZuiTileLayout
{
    ZSize m_szItem;
    int m_nColumns;
    ZCtlProc old_call;
    void *old_udata;
}*ZuiTileLayout, ZTileLayout;

ZuiAny ZCALL ZuiTileLayoutProc(int ProcId, ZuiControl cp, ZuiTileLayout p, ZuiAny Param1, ZuiAny Param2);


#endif // __UITILELAYOUT_H__
