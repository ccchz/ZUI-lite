#include "VerticalLayout.h"
#include "Layout.h"
#include <core/control.h>
#include <stdlib.h>

ZINT ZCALL ZuiVerticalLayoutProc(ZINT ProcId, ZuiControl cp, ZuiVerticalLayout p, ZPARAM Param1, ZPARAM Param2) {
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

        if (op->m_pVerticalScrollBar && op->m_pVerticalScrollBar->m_bVisible)
        {
            rc.right -= (int)ZCCALL(ZM_GetFixedWidth, op->m_pVerticalScrollBar, NULL, NULL);
        }
        if (op->m_pHorizontalScrollBar && op->m_pHorizontalScrollBar->m_bVisible)
        {
            rc.bottom -= (int)ZCCALL(ZM_GetFixedHeight, op->m_pHorizontalScrollBar, NULL, NULL);
        }

        ZSize SBarSize = {0};
        if (darray_len(op->m_items) == 0) {
            ZCCALL(ZM_Layout_ProcessScrollBar, cp, &rc, &SBarSize);
            return 0;
        }

        // Determine the minimum size
        ZSize szAvailable = { rc.right - rc.left, rc.bottom - rc.top };
        if (op->m_pHorizontalScrollBar && op->m_pHorizontalScrollBar->m_bVisible)
            szAvailable.cx += (int)ZCCALL(ZM_ScrollBar_GetScrollRange, op->m_pHorizontalScrollBar, NULL, NULL);
        if (op->m_pVerticalScrollBar && op->m_pVerticalScrollBar->m_bVisible)
            szAvailable.cy += (int)ZCCALL(ZM_ScrollBar_GetScrollRange, op->m_pVerticalScrollBar, NULL, NULL);

        int cxNeeded = 0;
        int nAdjustables = 0;
        int cyFixed = 0;
        int nEstimateNum = 0;
        ZSize szControlAvailable;
        int iControlMaxWidth = 0;
        int iControlMaxHeight = 0;
        for (int it1 = 0; it1 < darray_len(op->m_items); it1++) {
            ZuiControl pControl = (ZuiControl)(op->m_items->data[it1]);
            if (!pControl->m_bVisible) continue;
            if (pControl->m_bFloat) continue;
            szControlAvailable = szAvailable;
            ZRect rcMargin = *(ZRect *)(ZCCALL(ZM_GetMargin, pControl, 0, 0));
            szControlAvailable.cx -= rcMargin.left + rcMargin.right;
            iControlMaxWidth = (int)ZCCALL(ZM_GetFixedWidth, pControl, 0, 0);
            iControlMaxHeight = (int)ZCCALL(ZM_GetFixedHeight, pControl, 0, 0);
            if (iControlMaxWidth <= 0) iControlMaxWidth = (int)ZCCALL(ZM_GetMaxWidth, pControl, 0, 0);
            if (iControlMaxHeight <= 0) iControlMaxHeight = (int)ZCCALL(ZM_GetMaxHeight, pControl, 0, 0);
            if (szControlAvailable.cx > iControlMaxWidth) szControlAvailable.cx = iControlMaxWidth;
            if (szControlAvailable.cy > iControlMaxHeight) szControlAvailable.cy = iControlMaxHeight;
            ZSize sz = *(ZSize *)ZCCALL(ZM_EstimateSize, pControl, (void *)&szControlAvailable, 0);
            if (sz.cy == 0) {
                nAdjustables++;
            }
            else {
                if (sz.cy < (int)ZCCALL(ZM_GetMinHeight, pControl, 0, 0)) sz.cy = (int)ZCCALL(ZM_GetMinHeight, pControl, 0, 0);
                if (sz.cy > (int)ZCCALL(ZM_GetMaxHeight, pControl, 0, 0)) sz.cy = (int)ZCCALL(ZM_GetMaxHeight, pControl, 0, 0);
            }
            cyFixed += sz.cy + rcMargin.top + rcMargin.bottom;

            sz.cx = MAX(sz.cx, 0);
            if (sz.cx < (int)ZCCALL(ZM_GetMinWidth, pControl, 0, 0)) sz.cx = (int)ZCCALL(ZM_GetMinWidth, pControl, 0, 0);
            if (sz.cx > (int)ZCCALL(ZM_GetMaxWidth, pControl, 0, 0)) sz.cx = (int)ZCCALL(ZM_GetMaxWidth, pControl, 0, 0);
            cxNeeded = MAX(cxNeeded, sz.cx + rcMargin.left + rcMargin.right);
            nEstimateNum++;
        }
        cyFixed += (nEstimateNum - 1) * op->m_iChildPadding;

        // Place elements
        int cyNeeded = 0;
        int cyExpand = 0;
        if (nAdjustables > 0) cyExpand = MAX(0, (szAvailable.cy - cyFixed) / nAdjustables);
        // Position the elements
        ZSize szRemaining = szAvailable;
        int iPosY = rc.top;
        if (op->m_pVerticalScrollBar && op->m_pVerticalScrollBar->m_bVisible) {
            iPosY -= (int)ZCCALL(ZM_ScrollBar_GetScrollPos, op->m_pVerticalScrollBar, NULL, NULL);
        }

        int iEstimate = 0;
        int iAdjustable = 0;
        int cyFixedRemaining = cyFixed;
        for (int it2 = 0; it2 < darray_len(op->m_items); it2++) {
            ZuiControl pControl = (ZuiControl)(op->m_items->data[it2]);
            if (!pControl->m_bVisible) continue;
            if (pControl->m_bFloat) {
                ZCCALL(ZM_Layout_SetFloatPos, cp, (ZPARAM)it2, 0);
                continue;
            }

            iEstimate += 1;
            ZRect rcMargin = *(ZRect *)(ZCCALL(ZM_GetMargin, pControl, 0, 0));
            szRemaining.cy -= rcMargin.top;

            szControlAvailable = szRemaining;
            szControlAvailable.cx -= rcMargin.left + rcMargin.right;
            iControlMaxWidth = (int)ZCCALL(ZM_GetFixedWidth, pControl, 0, 0);
            iControlMaxHeight = (int)ZCCALL(ZM_GetFixedHeight, pControl, 0, 0);
            if (iControlMaxWidth <= 0) iControlMaxWidth = (int)ZCCALL(ZM_GetMaxWidth, pControl, 0, 0);
            if (iControlMaxHeight <= 0) iControlMaxHeight = (int)ZCCALL(ZM_GetMaxHeight, pControl, 0, 0);
            if (szControlAvailable.cx > iControlMaxWidth) szControlAvailable.cx = iControlMaxWidth;
            if (szControlAvailable.cy > iControlMaxHeight) szControlAvailable.cy = iControlMaxHeight;
            cyFixedRemaining = cyFixedRemaining - (rcMargin.top + rcMargin.bottom);
            if (iEstimate > 1) cyFixedRemaining = cyFixedRemaining - op->m_iChildPadding;
            ZSize sz = *(ZSize *)ZCCALL(ZM_EstimateSize, pControl, (void *)&szControlAvailable, 0);
            if (sz.cy == 0) {
                iAdjustable++;
                sz.cy = cyExpand;
                // Distribute remaining to last element (usually round-off left-overs)
                if (iAdjustable == nAdjustables) {
                    sz.cy = MAX(0, szRemaining.cy - rcMargin.bottom - cyFixedRemaining);
                }
                if (sz.cy < (int)ZCCALL(ZM_GetMinHeight, pControl, 0, 0)) sz.cy = (int)ZCCALL(ZM_GetMinHeight, pControl, 0, 0);
                if (sz.cy > (int)ZCCALL(ZM_GetMaxHeight, pControl, 0, 0)) sz.cy = (int)ZCCALL(ZM_GetMaxHeight, pControl, 0, 0);
            }
            else {
                if (sz.cy < (int)ZCCALL(ZM_GetMinHeight, pControl, 0, 0)) sz.cy = (int)ZCCALL(ZM_GetMinHeight, pControl, 0, 0);
                if (sz.cy > (int)ZCCALL(ZM_GetMaxHeight, pControl, 0, 0)) sz.cy = (int)ZCCALL(ZM_GetMaxHeight, pControl, 0, 0);
                cyFixedRemaining -= sz.cy;
            }

            sz.cx = (int)ZCCALL(ZM_GetMaxWidth, pControl, 0, 0);
            if (sz.cx == 0) sz.cx = szAvailable.cx - rcMargin.left - rcMargin.right;
            if (sz.cx < 0) sz.cx = 0;
            if (sz.cx > szControlAvailable.cx) sz.cx = szControlAvailable.cx;
            if (sz.cx < (int)ZCCALL(ZM_GetMinWidth, pControl, 0, 0)) sz.cx = (int)ZCCALL(ZM_GetMinWidth, pControl, 0, 0);

            unsigned int iChildAlign = (unsigned int)ZCCALL(ZM_Layout_GetChildAlign, cp, NULL, NULL);
            if (iChildAlign == ZDT_CENTER) {
                int iPosX = (rc.right + rc.left) / 2;
                if (op->m_pHorizontalScrollBar && op->m_pHorizontalScrollBar->m_bVisible) {
                    iPosX += (cxNeeded - rc.right + rc.left) / 2;
                    iPosX -= (int)ZCCALL(ZM_ScrollBar_GetScrollPos, op->m_pHorizontalScrollBar, 0, NULL);
                }
                RECT rcCtrl = { iPosX - sz.cx / 2, iPosY + rcMargin.top, iPosX + sz.cx - sz.cx / 2, iPosY + sz.cy + rcMargin.top };
                ZCCALL(ZM_SetPos, pControl, &rcCtrl, (ZPARAM)ZuiOnSize);
            }
            else if (iChildAlign == ZDT_RIGHT) {
                int iPosX = rc.right;
                if (op->m_pHorizontalScrollBar && op->m_pHorizontalScrollBar->m_bVisible) {
                    iPosX += cxNeeded - rc.right + rc.left;
                    iPosX -= (int)ZCCALL(ZM_ScrollBar_GetScrollPos, op->m_pHorizontalScrollBar, 0, NULL);
                }
                RECT rcCtrl = { iPosX - rcMargin.right - sz.cx, iPosY + rcMargin.top, iPosX - rcMargin.right, iPosY + sz.cy + rcMargin.top };
                ZCCALL(ZM_SetPos, pControl, &rcCtrl, (ZPARAM)ZuiOnSize);
            }
            else {
                int iPosX = rc.left;
                if (op->m_pHorizontalScrollBar && op->m_pHorizontalScrollBar->m_bVisible) {
                    iPosX -= (int)ZCCALL(ZM_ScrollBar_GetScrollPos, op->m_pHorizontalScrollBar, 0, NULL);
                }
                RECT rcCtrl = { iPosX + rcMargin.left, iPosY + rcMargin.top, iPosX + rcMargin.left + sz.cx, iPosY + sz.cy + rcMargin.top };
                ZCCALL(ZM_SetPos, pControl, &rcCtrl, (ZPARAM)ZuiOnSize);
            }

            iPosY += sz.cy + op->m_iChildPadding + rcMargin.top + rcMargin.bottom;
            cyNeeded += sz.cy + rcMargin.top + rcMargin.bottom;
            szRemaining.cy -= sz.cy + op->m_iChildPadding + rcMargin.bottom;
        }
        cyNeeded += (nEstimateNum - 1) * op->m_iChildPadding;

        SBarSize.cx = cxNeeded;
        SBarSize.cy = cyNeeded;
        // Process the scrollbar
        ZCCALL(ZM_Layout_ProcessScrollBar, cp, (ZPARAM)&rc, (ZPARAM)&SBarSize);
        return 0;
    }
    case ZM_OnCreate: {
        p = (ZuiVerticalLayout)malloc(sizeof(ZVerticalLayout));
        memset(p, 0, sizeof(ZVerticalLayout));
        //创建继承的控件 保存数据指针
        p->old_udata = ZuiLayoutProc(ZM_OnCreate, cp, 0, 0, 0);
        p->old_call = (ZCtlProc)&ZuiLayoutProc;

        return p;
    }
    case ZM_OnDestroy: {
        ZCtlProc old_call = p->old_call;
        ZVoid old_udata = p->old_udata;
        old_call(ProcId, cp, old_udata, Param1, Param2);
        
        free(p);
        return 0;
    }
    case ZM_GetType:
        return ZC_VerticalLayout;
    case ZM_CoreInit:
        return TRUE;
    case ZM_CoreUnInit:
        return NULL;
    default:
        break;
    }
    return p->old_call(ProcId, cp, p->old_udata, Param1, Param2);
}




