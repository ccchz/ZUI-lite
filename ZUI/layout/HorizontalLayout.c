#include "HorizontalLayout.h"
#include "Layout.h"
#include <core/control.h>
#include <stdlib.h>

void* ZCALL ZuiHorizontalLayoutProc(int ProcId, ZuiControl cp, ZuiHorizontalLayout p, void* Param1, void* Param2) {
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

        if (darray_len(op->m_items) == 0) {
            ZCCALL(ZM_Layout_ProcessScrollBar, cp, &rc, 0);
            return 0;
        }

        // Determine the minimum size
        ZSize szAvailable = { rc.right - rc.left, rc.bottom - rc.top };
        if (op->m_pHorizontalScrollBar && op->m_pHorizontalScrollBar->m_bVisible)
            szAvailable.cx += (int)ZCCALL(ZM_ScrollBar_GetScrollRange, op->m_pHorizontalScrollBar, NULL, NULL);
        if (op->m_pVerticalScrollBar && op->m_pVerticalScrollBar->m_bVisible)
            szAvailable.cy += (int)ZCCALL(ZM_ScrollBar_GetScrollRange, op->m_pVerticalScrollBar, NULL, NULL);

        int cyNeeded = 0;
        int nAdjustables = 0;
        int cxFixed = 0;
        int nEstimateNum = 0;
        ZSize szControlAvailable;
        int iControlMaxWidth = 0;
        int iControlMaxHeight = 0;
        //第一轮计算得到各种信息，不做实际布局处理 
        for (int it1 = 0; it1 < darray_len(op->m_items); it1++) {
            ZuiControl pControl = (ZuiControl)(op->m_items->data[it1]);
            if (!pControl->m_bVisible) continue;
            if (pControl->m_bFloat) continue;
            szControlAvailable = szAvailable;
            ZRect rcPadding = *(ZRect *)(ZCCALL(ZM_GetPadding, pControl, 0, 0));
            szControlAvailable.cy -= rcPadding.top + rcPadding.bottom;
            iControlMaxWidth = (int)ZCCALL(ZM_GetFixedWidth, pControl, 0, 0);
            iControlMaxHeight = (int)ZCCALL(ZM_GetFixedHeight, pControl, 0, 0);
            if (iControlMaxWidth <= 0) iControlMaxWidth = (int)ZCCALL(ZM_GetMaxWidth, pControl, 0, 0);
            if (iControlMaxHeight <= 0) iControlMaxHeight = (int)ZCCALL(ZM_GetMaxHeight, pControl, 0, 0);
            if (szControlAvailable.cx > iControlMaxWidth) szControlAvailable.cx = iControlMaxWidth;
            if (szControlAvailable.cy > iControlMaxHeight) szControlAvailable.cy = iControlMaxHeight;
            ZSize sz = *(ZSize *)ZCCALL(ZM_EstimateSize, pControl, (void *)&szControlAvailable, 0);
            if (sz.cx == 0) {
                //记录需要自动计算宽度的子控件的数量
                nAdjustables++;
            }
            else {
                if (sz.cx < (int)ZCCALL(ZM_GetMinWidth, pControl, 0, 0)) sz.cx = (int)ZCCALL(ZM_GetMinWidth, pControl, 0, 0);
                if (sz.cx > (int)ZCCALL(ZM_GetMaxWidth, pControl, 0, 0)) sz.cx = (int)ZCCALL(ZM_GetMaxWidth, pControl, 0, 0);
            }
            cxFixed += sz.cx + ((ZRect *)(ZCCALL(ZM_GetPadding, pControl, 0, 0)))->left + ((ZRect *)(ZCCALL(ZM_GetPadding, pControl, 0, 0)))->right;

            sz.cy = MAX(sz.cy, 0);
            if (sz.cy < (int)ZCCALL(ZM_GetMinHeight, pControl, 0, 0)) sz.cy = (int)ZCCALL(ZM_GetMinHeight, pControl, 0, 0);
            if (sz.cy > (int)ZCCALL(ZM_GetMaxHeight, pControl, 0, 0)) sz.cy = (int)ZCCALL(ZM_GetMaxHeight, pControl, 0, 0);
            cyNeeded = MAX(cyNeeded, sz.cy + rcPadding.top + rcPadding.bottom);
            //记录需要做相对布局的子控件的数量
            nEstimateNum++;
        }
        //cxFixed保存了所有相对布局的控件占用的宽度（包括了padding属性好childpadding属性占用的宽度）
        cxFixed += (nEstimateNum - 1) * op->m_iChildPadding;

        // Place elements
        int cxNeeded = 0;
        //cxExpand保存需要自动计算宽度的子控件的宽度
        int cxExpand = 0;
        if (nAdjustables > 0) cxExpand = MAX(0, (szAvailable.cx - cxFixed) / nAdjustables);
        // Position the elements
        //szRemaining保存除已被布局的子控件以外的剩余空间
        ZSize szRemaining = szAvailable;
        int iPosX = rc.left;
        if (op->m_pHorizontalScrollBar && op->m_pHorizontalScrollBar->m_bVisible) {
            iPosX -= (int)ZCCALL(ZM_ScrollBar_GetScrollPos, op->m_pHorizontalScrollBar, NULL, NULL);
        }
        int iEstimate = 0;
        int iAdjustable = 0;
        //cxFixedRemaining记录当前还未被布局过的所有子控件的总宽度
        int cxFixedRemaining = cxFixed;
        for (int it2 = 0; it2 < darray_len(op->m_items); it2++) {
            ZuiControl pControl = (ZuiControl)(op->m_items->data[it2]);
            if (!pControl->m_bVisible)
                continue;
            if (pControl->m_bFloat) {
                ZCCALL(ZM_Layout_SetFloatPos, cp, (void *)it2, 0);
                continue;
            }

            iEstimate += 1;
            ZRect rcPadding = *(ZRect *)(ZCCALL(ZM_GetPadding, pControl, 0, 0));
            szRemaining.cx -= rcPadding.left;

            szControlAvailable = szRemaining;
            szControlAvailable.cy -= rcPadding.top + rcPadding.bottom;
            iControlMaxWidth = (int)ZCCALL(ZM_GetFixedWidth, pControl, 0, 0);
            iControlMaxHeight = (int)ZCCALL(ZM_GetFixedHeight, pControl, 0, 0);
            if (iControlMaxWidth <= 0) iControlMaxWidth = (int)ZCCALL(ZM_GetMaxWidth, pControl, 0, 0);
            if (iControlMaxHeight <= 0) iControlMaxHeight = (int)ZCCALL(ZM_GetMaxHeight, pControl, 0, 0);
            if (szControlAvailable.cx > iControlMaxWidth) szControlAvailable.cx = iControlMaxWidth;
            if (szControlAvailable.cy > iControlMaxHeight) szControlAvailable.cy = iControlMaxHeight;
            cxFixedRemaining = cxFixedRemaining - (rcPadding.left + rcPadding.right);
            if (iEstimate > 1) cxFixedRemaining = cxFixedRemaining - op->m_iChildPadding;
            ZSize sz = *(ZSize *)ZCCALL(ZM_EstimateSize, pControl, (void *)&szControlAvailable, 0);
            if (sz.cx == 0) {
                iAdjustable++;
                sz.cx = cxExpand;
                // Distribute remaining to last element (usually round-off left-overs)
                // 这里判断如果是最后一个需要自动计算宽度的元素则做出不同的处理
                if (iAdjustable == nAdjustables) {
                    sz.cx = MAX(0, szRemaining.cx - rcPadding.right - cxFixedRemaining);
                }
                if (sz.cx < (int)ZCCALL(ZM_GetMinWidth, pControl, 0, 0)) sz.cx = (int)ZCCALL(ZM_GetMinWidth, pControl, 0, 0);
                if (sz.cx >(int)ZCCALL(ZM_GetMaxWidth, pControl, 0, 0)) sz.cx = (int)ZCCALL(ZM_GetMaxWidth, pControl, 0, 0);
            }
            else {
                if (sz.cx < (int)ZCCALL(ZM_GetMinWidth, pControl, 0, 0)) sz.cx = (int)ZCCALL(ZM_GetMinWidth, pControl, 0, 0);
                if (sz.cx > (int)ZCCALL(ZM_GetMaxWidth, pControl, 0, 0)) sz.cx = (int)ZCCALL(ZM_GetMaxWidth, pControl, 0, 0);
                cxFixedRemaining -= sz.cx;
            }

            sz.cy = (int)ZCCALL(ZM_GetMaxHeight, pControl, 0, 0);
            if (sz.cy == 0) sz.cy = szAvailable.cy - rcPadding.top - rcPadding.bottom;
            if (sz.cy < 0) sz.cy = 0;
            if (sz.cy > szControlAvailable.cy) sz.cy = szControlAvailable.cy;
            if (sz.cy < (int)ZCCALL(ZM_GetMinHeight, pControl, 0, 0)) sz.cy = (int)ZCCALL(ZM_GetMinHeight, pControl, 0, 0);

            unsigned int iChildAlign = (unsigned int)ZCCALL(ZM_Layout_GetChildVAlign, cp, NULL, NULL);
            if (iChildAlign == ZDT_VCENTER) {
                int iPosY = (rc.bottom + rc.top) / 2;
                if (op->m_pVerticalScrollBar && op->m_pVerticalScrollBar->m_bVisible) {
                    iPosY += cyNeeded - rc.bottom + rc.top / 2;
                    iPosY -= (int)ZCCALL(ZM_ScrollBar_GetScrollPos, op->m_pVerticalScrollBar, 0, NULL);
                }
                ZRect rcCtrl = { iPosX + rcPadding.left, iPosY - sz.cy / 2, iPosX + sz.cx + rcPadding.left, iPosY + sz.cy - sz.cy / 2 };
                ZCCALL(ZM_SetPos, pControl, &rcCtrl, FALSE);
            }
            else if (iChildAlign == ZDT_BOTTOM) {
                int iPosY = rc.bottom;
                if (op->m_pVerticalScrollBar && op->m_pVerticalScrollBar->m_bVisible) {
                    iPosY += cyNeeded - rc.bottom + rc.top;
                    iPosY -= (int)ZCCALL(ZM_ScrollBar_GetScrollPos, op->m_pVerticalScrollBar, 0, NULL);
                }
                ZRect rcCtrl = { iPosX + rcPadding.left, iPosY - rcPadding.bottom - sz.cy, iPosX + sz.cx + rcPadding.left, iPosY - rcPadding.bottom };
                ZCCALL(ZM_SetPos, pControl, &rcCtrl, FALSE);
            }
            else {
                int iPosY = rc.top;
                if (op->m_pVerticalScrollBar && op->m_pVerticalScrollBar->m_bVisible) {
                    iPosY -= (int)ZCCALL(ZM_ScrollBar_GetScrollPos, op->m_pVerticalScrollBar, 0, NULL);
                }
                ZRect rcCtrl = { iPosX + rcPadding.left, iPosY + rcPadding.top, iPosX + sz.cx + rcPadding.left, iPosY + sz.cy + rcPadding.top };
                ZCCALL(ZM_SetPos, pControl, &rcCtrl, FALSE);
            }

            iPosX += sz.cx + op->m_iChildPadding + rcPadding.left + rcPadding.right;
            cxNeeded += sz.cx + rcPadding.left + rcPadding.right;
            szRemaining.cx -= sz.cx + op->m_iChildPadding + rcPadding.right;
        }
        cxNeeded += (nEstimateNum - 1) * op->m_iChildPadding;

        // Process the scrollbar
        ZCCALL(ZM_Layout_ProcessScrollBar, cp, (ZuiAny)&rc, (ZuiAny)(MAKEPARAM(cxNeeded, cyNeeded)));
        return 0;
    }
    case ZM_OnCreate: {
        p = (ZuiHorizontalLayout)malloc(sizeof(ZHorizontalLayout));
        memset(p, 0, sizeof(ZHorizontalLayout));
        //创建继承的控件 保存数据指针
        p->old_udata = ZuiLayoutProc(ZM_OnCreate, cp, 0, 0, 0);
        p->old_call = (ZCtlProc)&ZuiLayoutProc;

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
        if (_tcsicmp(Param1, (ZuiAny)ZC_HorizontalLayout) == 0)
            return (ZuiAny)p;
        break;
    case ZM_GetType:
        return (ZuiAny)ZC_HorizontalLayout;
    case ZM_CoreInit:
        return (ZuiAny)TRUE;
    case ZM_CoreUnInit:
        return NULL;
    default:
        break;
    }
    return p->old_call(ProcId, cp, p->old_udata, Param1, Param2);
}

