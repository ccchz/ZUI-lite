#include "GridLayout.h"
#include "Layout.h"
#include <core/control.h>
#include <stdlib.h>

ZINT ZCALL ZuiGridLayoutProc(ZINT ProcId, ZuiControl cp, ZuiGridLayout p, ZPARAM Param1, ZPARAM Param2) {
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

        ZSize SBarSize = { 0 };
        if (darray_len(op->m_items) == 0) {
            ZCCALL(ZM_Layout_ProcessScrollBar, cp, &rc, &SBarSize);
            return 0;
        }
        //计算每行控件数量。
        if (rc.right - rc.left < 1) //控件宽度为零时，不进行布局计算。
            return 0;
        int maxcols = (rc.right - rc.left) / (p->m_szGridSize.cx + op->m_iChildPadding); //最大列数
        if (maxcols < 1) {
            maxcols = 1;
        }
        int index ,count, cols, rows;
        cols = 0;
        rows = -1;
        ZRect rcCtrl;
        //计算控件布局尺寸。
        for (index = count = 0; index < darray_len(op->m_items); index++) {
            ZuiControl pControl = (ZuiControl)(op->m_items->data[index]);
            if (!pControl->m_bVisible)
                continue;
            if (pControl->m_bFloat) {
                ZCCALL(ZM_Layout_SetFloatPos, cp, (ZPARAM)index, 0);
                continue;
            }

            cols = count % maxcols;
            count++;
            if (cols == 0)
                rows++;
            rcCtrl.left = rc.left + cols * (p->m_szGridSize.cx + op->m_iChildPadding)- iposx;
            rcCtrl.top = rc.top + rows * (p->m_szGridSize.cy + op->m_iChildPadding) - iposy;
            rcCtrl.right = rcCtrl.left + p->m_szGridSize.cx;
            rcCtrl.bottom = rcCtrl.top + p->m_szGridSize.cy;
           
            //处理控件 Margin 尺寸。
            ZRect rcMargin = *(ZRect*)(ZCCALL(ZM_GetMargin, pControl, 0, 0));
            rcCtrl.left += rcMargin.left;
            rcCtrl.top += rcMargin.top;
            rcCtrl.right -= rcMargin.right;
            rcCtrl.bottom -= rcMargin.bottom;
            ZCCALL(ZM_SetPos, pControl, &rcCtrl, (ZPARAM)ZuiOnSize);
        }

        SBarSize.cx = maxcols * p->m_szGridSize.cx;
        SBarSize.cy = (rows + 1) * p->m_szGridSize.cy;
        // Process the scrollbar
        ZCCALL(ZM_Layout_ProcessScrollBar, cp, &rc, &SBarSize);
        return 0;
    }
    //设置控件单元尺寸。
    case ZM_SetAttribute: {
        ZuiAttribute zAttr = (ZuiAttribute)Param1;
        if (_tcsicmp(zAttr->name, _T("gridsize")) == 0) {
            ZuiText pstr = NULL;
            ZSize sz = { 0 };
            sz.cx = _tcstol(zAttr->value, &pstr, 10);  ASSERT(pstr);
            sz.cy = _tcstol(pstr + 1, &pstr, 10);    ASSERT(pstr);
            ZCCALL(ZM_GridSetSize, cp, &sz, Param2);
        }
        break;
    }
    case ZM_GridSetSize: {
        p->m_szGridSize.cx = ((ZuiSize)Param1)->cx;
        p->m_szGridSize.cy = ((ZuiSize)Param1)->cy;
        if (!Param2)
            ZuiControlNeedUpdate(cp);
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
        ZVoid old_udata = p->old_udata;

        old_call(ProcId, cp, old_udata, Param1, Param2);

        free(p);

        return 0;
    }
    case ZM_GetObject:
        if (_tcsicmp(Param1, (ZPARAM)ZC_GridLayout) == 0)
            return (ZPARAM)p;
        break;
    case ZM_GetType:
        return (ZPARAM)ZC_GridLayout;
    case ZM_CoreInit:
        return (ZPARAM)TRUE;
    case ZM_CoreUnInit:
        return NULL;
    default:
        break;
    }
    return p->old_call(ProcId, cp, p->old_udata, Param1, Param2);
}