#include "GridLayout.h"
#include "Layout.h"
#include <core/control.h>
#include <stdlib.h>

void* ZCALL ZuiGridLayoutProc(int ProcId, ZuiControl cp, ZuiGridLayout p, void* Param1, void* Param2) {
    switch (ProcId)
    {
    case ZM_SetPos: {
        ZuiDefaultControlProc(ProcId, cp, 0, Param1, Param2);
        ZRect rc = cp->m_rcItem;
        ZuiLayout op = (ZuiLayout)p->old_udata;

        // Adjust for inset
        rc.left += op->m_rcInset.left;
        rc.top += op->m_rcInset.top;
        rc.right -= op->m_rcInset.right;
        rc.bottom -= op->m_rcInset.bottom;
        rc.left += cp->m_dwBorderWidth;
        rc.top += cp->m_dwBorderWidth;
        rc.right -= cp->m_dwBorderWidth;
        rc.bottom -= cp->m_dwBorderWidth;

        int iposx, iposy;  //滚动条位置。
        iposx = iposy = 0;
        if (op->m_pVerticalScrollBar && op->m_pVerticalScrollBar->m_bVisible)
        {
            rc.right -= (int)ZCCALL(ZM_GetFixedWidth, op->m_pVerticalScrollBar, NULL, NULL);
            iposy = (int)ZCCALL(ZM_ScrollBar_GetScrollPos, op->m_pVerticalScrollBar, 0, NULL);
        }
        if (op->m_pHorizontalScrollBar && op->m_pHorizontalScrollBar->m_bVisible)
        {
            rc.bottom -= (int)ZCCALL(ZM_GetFixedHeight, op->m_pHorizontalScrollBar, NULL, NULL);
            iposx = (int)ZCCALL(ZM_ScrollBar_GetScrollPos, op->m_pHorizontalScrollBar, 0, NULL);
        }

        if (darray_len(op->m_items) == 0) {
            ZCCALL(ZM_Layout_ProcessScrollBar, cp, &rc, 0);
            return 0;
        }
        int col = (rc.right - rc.left) / p->m_szGridSize.cx;
        if (col < 1) {
            p->m_szGridSize.cx = rc.right - rc.left;
            col = 1;
        }
        int i , j, rows;
        j = 0;
        rows = -1;
        ZRect rcCtrl;
        for (i = 0; i < darray_len(op->m_items); i++) {
            j = i % col;
            if (j == 0)
                rows++;
            rcCtrl.left = rc.left + j * p->m_szGridSize.cx - iposx;
            rcCtrl.top = rc.top + rows * p->m_szGridSize.cy - iposy;
            rcCtrl.right = rcCtrl.left + p->m_szGridSize.cx;
            rcCtrl.bottom = rcCtrl.top + p->m_szGridSize.cy;
            ZuiControl pControl = (ZuiControl)(op->m_items->data[i]);
            ZCCALL(ZM_SetPos, pControl, &rcCtrl, FALSE);
        }
        // Process the scrollbar
        ZCCALL(ZM_Layout_ProcessScrollBar, cp, (ZuiAny)&rc, (ZuiAny)(MAKEPARAM(col * p->m_szGridSize.cx, (rows+1) * p->m_szGridSize.cy)));
        return 0;
    }
    case ZM_SetAttribute: {
        if (_tcsicmp(Param1, _T("gridsize")) == 0) {
            ZuiText pstr = NULL;
            int cx = _tcstol(Param2, &pstr, 10);  ASSERT(pstr);
            int cy = _tcstol(pstr + 1, &pstr, 10);    ASSERT(pstr);
            ZCCALL(ZM_GridSetSize, cp, (ZuiAny)cx, (ZuiAny)cy);
        }
        break;
    }
    case ZM_GridSetSize: {
        p->m_szGridSize.cx = (int)Param1;
        p->m_szGridSize.cy = (int)Param2;
        ZuiControlNeedParentUpdate(cp);
        break;
    }
    case ZM_OnCreate: {
        p = (ZuiGridLayout)malloc(sizeof(ZGridLayout));
        memset(p, 0, sizeof(ZGridLayout));
        //创建继承的控件 保存数据指针
        p->old_udata = ZuiLayoutProc(ZM_OnCreate, cp, 0, 0, 0);
        p->old_call = (ZCtlProc)&ZuiLayoutProc;
        p->m_szGridSize.cx = GRIDSIZE;
        p->m_szGridSize.cy = GRIDSIZE;
        return p;
    }
    case ZM_OnDestroy: {
        ZCtlProc old_call = p->old_call;
        ZuiAny old_udata = p->old_udata;

        old_call(ProcId, cp, old_udata, Param1, Param2);

        free(p);

        return 0;
    }
    case ZM_GetObject:
        if (_tcsicmp(Param1, (ZuiAny)ZC_GridLayout) == 0)
            return (ZuiAny)p;
        break;
    case ZM_GetType:
        return (ZuiAny)ZC_GridLayout;
    case ZM_CoreInit:
        return (ZuiAny)TRUE;
    case ZM_CoreUnInit:
        return NULL;
    default:
        break;
    }
    return p->old_call(ProcId, cp, p->old_udata, Param1, Param2);
}