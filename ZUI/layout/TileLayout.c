#include "TileLayout.h"
#include "Layout.h"
#include <core/control.h>
#include <stdlib.h>

ZINT ZCALL ZuiTileLayoutProc(ZINT ProcId, ZuiControl cp, ZuiTileLayout p, ZPARAM Param1, ZPARAM Param2) {
    switch (ProcId)
    {
    case ZM_SetPos: {
        ZuiDefaultControlProc(ProcId, cp, 0, Param1, Param2);
        ZRect rc = cp->m_rcItem;

        // Adjust for inset
        ZuiLayout op = (ZuiLayout)p->old_udata;
        rc.left += op->m_rcInset.left;
        rc.top += op->m_rcInset.top;
        rc.right -= op->m_rcInset.right;
        rc.bottom -= op->m_rcInset.bottom;

        if (darray_len(op->m_items) == 0) {
            return 0;
        }

        // Position the elements
        if (p->m_szItem.cx > 0) p->m_nColumns = (rc.right - rc.left) / p->m_szItem.cx;
        if (p->m_nColumns == 0) p->m_nColumns = 1;

        int cyNeeded = 0;
        int cxWidth = (rc.right - rc.left) / p->m_nColumns;


        int cyHeight = 0;
        int iCount = 0;
        ZPoint ptTile = { rc.left, rc.top };

        int iPosX = rc.left;

        for (int it1 = 0; it1 < darray_len(op->m_items); it1++) {
            ZuiControl pControl = (ZuiControl)(op->m_items->data[it1]);
            if (!pControl->m_bVisible) continue;
            if (pControl->m_bFloat) {
                ZCCALL(ZM_Layout_SetFloatPos, cp, (ZPARAM)it1, 0);
                continue;
            }

            // Determine size
            ZRect rcTile = { ptTile.x, ptTile.y, ptTile.x + cxWidth, ptTile.y };
            if ((iCount % p->m_nColumns) == 0)
            {
                int iIndex = iCount;
                for (int it2 = it1; it2 < darray_len(op->m_items); it2++) {
                    ZuiControl pLineControl = (ZuiControl)(op->m_items->data[it2]);
                    if (!pLineControl->m_bVisible) continue;
                    if (pLineControl->m_bFloat) continue;

                    ZRect *rcMargin = (ZRect *)(ZCCALL(ZM_GetMargin, pLineControl, 0, 0));
                    ZSize szAvailable = { rcTile.right - rcTile.left - rcMargin->left - rcMargin->right, 9999 };

                    if (szAvailable.cx < (int)ZCCALL(ZM_GetMinWidth, pControl, 0, 0)) { szAvailable.cx = (int)ZCCALL(ZM_GetMinWidth, pControl, 0, 0); }
                    if (szAvailable.cx > (int)ZCCALL(ZM_GetMaxWidth, pControl, 0, 0)) { szAvailable.cx = (int)ZCCALL(ZM_GetMaxWidth, pControl, 0, 0); }


                    ZSize *pszTile = (ZSize *)ZCCALL(ZM_EstimateSize, pLineControl, (void *)&szAvailable, 0);
                    ZSize szTile;
                    szTile.cx = pszTile->cx;
                    szTile.cy = pszTile->cy;
                    if (szTile.cx == 0) szTile.cx = p->m_szItem.cx;
                    if (szTile.cy == 0) szTile.cy = p->m_szItem.cy;
                    if (szTile.cx < (int)ZCCALL(ZM_GetMinWidth, pControl, 0, 0)) szTile.cx = (int)ZCCALL(ZM_GetMinWidth, pControl, 0, 0);
                    if (szTile.cx > (int)ZCCALL(ZM_GetMaxWidth, pControl, 0, 0)) szTile.cx = (int)ZCCALL(ZM_GetMaxWidth, pControl, 0, 0);
                    if (szTile.cy < (int)ZCCALL(ZM_GetMinHeight, pControl, 0, 0)) szTile.cy = (int)ZCCALL(ZM_GetMinHeight, pControl, 0, 0);
                    if (szTile.cy > (int)ZCCALL(ZM_GetMaxHeight, pControl, 0, 0)) szTile.cy = (int)ZCCALL(ZM_GetMaxHeight, pControl, 0, 0);

                    cyHeight = MAX(cyHeight, szTile.cy + rcMargin->top + rcMargin->bottom);
                    if ((++iIndex % p->m_nColumns) == 0) break;
                }
            }

            ZRect *rcMargin = (ZRect *)(ZCCALL(ZM_GetMargin, pControl, 0, 0));

            rcTile.left += rcMargin->left;
            rcTile.right -= rcMargin->right;

            // Set position
            rcTile.top = ptTile.y + rcMargin->top;
            rcTile.bottom = ptTile.y + cyHeight;

            ZSize szAvailable = { rcTile.right - rcTile.left, rcTile.bottom - rcTile.top };

            ZSize *pszTile = (ZSize *)ZCCALL(ZM_EstimateSize, pControl, (void *)&szAvailable, 0);
            ZSize szTile;
            szTile.cx = pszTile->cx;
            szTile.cy = pszTile->cy;
            if (szTile.cx == 0) szTile.cx = p->m_szItem.cx;
            if (szTile.cy == 0) szTile.cy = p->m_szItem.cy;
            if (szTile.cx == 0) szTile.cx = szAvailable.cx;
            if (szTile.cy == 0) szTile.cy = szAvailable.cy;
            if (szTile.cx < (int)ZCCALL(ZM_GetMinWidth, pControl, 0, 0)) { szTile.cx = (int)ZCCALL(ZM_GetMinWidth, pControl, 0, 0); }
            if (szTile.cx > (int)ZCCALL(ZM_GetMaxWidth, pControl, 0, 0)) { szTile.cx = (int)ZCCALL(ZM_GetMaxWidth, pControl, 0, 0); }
            if (szTile.cy < (int)ZCCALL(ZM_GetMinHeight, pControl, 0, 0)) { szTile.cy = (int)ZCCALL(ZM_GetMinHeight, pControl, 0, 0); }
            if (szTile.cy > (int)ZCCALL(ZM_GetMaxHeight, pControl, 0, 0)) { szTile.cy = (int)ZCCALL(ZM_GetMaxHeight, pControl, 0, 0); }
            ZRect rcPos = { (rcTile.left + rcTile.right - szTile.cx) / 2, (rcTile.top + rcTile.bottom - szTile.cy) / 2,
                (rcTile.left + rcTile.right - szTile.cx) / 2 + szTile.cx, (rcTile.top + rcTile.bottom - szTile.cy) / 2 + szTile.cy };
            ZCCALL(ZM_SetPos, pControl, &rcPos, (ZPARAM)ZuiOnSize);
            if ((++iCount % p->m_nColumns) == 0) {
                ptTile.x = iPosX;
                ptTile.y += cyHeight + op->m_iChildPadding;
                cyHeight = 0;
            }
            else {
                ptTile.x += cxWidth;
            }
            cyNeeded = rcTile.bottom - rc.top;
        }
        return 0;
    }
    case ZM_SetAttribute: {
        ZuiAttribute zAttr = (ZuiAttribute)Param1;
        if (_tcsicmp(zAttr->name, _T("itemsize")) == 0) {
            ZuiText pstr = NULL;
            ZSize sz = { 0 };
            sz.cx = _tcstol(zAttr->value, &pstr, 10);  ASSERT(pstr);
            sz.cy = _tcstol(pstr + 1, &pstr, 10);   ASSERT(pstr);
            ZCCALL(ZM_TileLayout_SetItemSize, cp, &sz, Param2);
        }
        else if (_tcsicmp(zAttr->name, _T("columns")) == 0) 
            ZCCALL(ZM_TileLayout_SetColumns, cp, (ZPARAM)_ttoi(zAttr->value), Param2);
        break;
    }
    case ZM_TileLayout_SetColumns: {
        if (Param1 <= 0) return 0;
        p->m_nColumns = (int)Param1;
        if (!Param2)
            ZuiControlNeedUpdate(cp);
        break;
    }
    case ZM_TileLayout_SetItemSize: {
        if (p->m_szItem.cx != ((ZuiSize)Param1)->cx || p->m_szItem.cy != ((ZuiSize)Param1)->cy) {
            p->m_szItem.cx = ((ZuiSize)Param1)->cx;
            p->m_szItem.cy = ((ZuiSize)Param1)->cy;
            if (!Param2)
                ZuiControlNeedUpdate(cp);
        }
        break;
    }
    case ZM_OnCreate: {
        p = (ZuiTileLayout)malloc(sizeof(ZTileLayout));
        memset(p, 0, sizeof(ZTileLayout));
        //创建继承的控件 保存数据指针
        p->old_udata = ZuiLayoutProc(ZM_OnCreate, cp, 0, 0, 0);
        p->old_call = (ZCtlProc)&ZuiLayoutProc;

        return p;
    }
    case ZM_OnDestroy: {
        ZCtlProc old_call = p->old_call;
        ZVoid old_udata = p->old_udata;

        free(p);

        return old_call(ProcId, cp, old_udata, Param1, Param2);
    }
    case ZM_GetType:
        return (ZPARAM)ZC_TileLayout;
    case ZM_CoreInit:
        return (ZPARAM)TRUE;
    case ZM_CoreUnInit:
        return NULL;
    default:
        break;
    }
    return p->old_call(ProcId, cp, p->old_udata, Param1, Param2);
}
