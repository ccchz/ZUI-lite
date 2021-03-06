#include "List.h"
#include <core/control.h>
#include <core/resdb.h>
#include <core/function.h>
#include <layout/Layout.h>
#include <layout/VerticalLayout.h>
#include <layout/HorizontalLayout.h>
#include <core/Register.h>
#include <control/Label.h>
ZEXPORT ZINT ZCALL ZuiListProc(ZINT ProcId, ZuiControl cp, ZuiList p, ZPARAM Param1, ZPARAM Param2) {
    switch (ProcId)
    {
    case ZM_SetAttribute: {
        ZuiAttribute zAttr = (ZuiAttribute)Param1;
        if (_tcsicmp(zAttr->name, _T("header")) == 0)
            ZCCALL(ZM_SetVisible, p->m_pHeader, (ZPARAM)(_tcsicmp(zAttr->value, _T("hidden")) == 0 ? FALSE : TRUE), Param2);
        else if (_tcsicmp(zAttr->name, _T("linecolor")) == 0) {
            p->m_ListInfo.m_cLineColor = ZuiStr2Color(zAttr->value);
            p->m_ListInfo.m_bShowRowLine = TRUE;
        }
        else if (_tcsicmp(zAttr->name, _T("columncolor")) == 0) {
            p->m_ListInfo.m_cColumnColor = ZuiStr2Color(zAttr->value);
            p->m_ListInfo.m_bShowColumnLine = TRUE;
        }
        else if (_tcsicmp(zAttr->name, _T("hotbkcolor")) == 0) {
            p->m_ListInfo.m_cHotBkColor = ZuiStr2Color(zAttr->value);
        }
        else if (_tcsicmp(zAttr->name, _T("selectedbkcolor")) == 0) {
            p->m_ListInfo.m_cSelectedBkColor = ZuiStr2Color(zAttr->value);
        }


        break;
    }
    case ZM_List_SetScrollPos: {
        return ZCCALL(ZM_ListBody_SetScrollPos, p->m_pList, Param1, NULL);
    }
    case ZM_List_GetHeader: {
        return p->m_pHeader;
    }
    case ZM_List_GetListInfo: {
        return &p->m_ListInfo;
    }
    case ZM_List_GetVerticalScrollBar: {
        return ZCCALL(ZM_Layout_GetVerticalScrollBar, p->m_pList, NULL, NULL);
    }
    case ZM_List_GetHorizontalScrollBar: {
        return ZCCALL(ZM_Layout_GetHorizontalScrollBar, p->m_pList, NULL, NULL);
    }
    case ZM_List_EnableScrollBar: {
        return ZCCALL(ZM_Layout_EnableScrollBar, p->m_pList, Param1, Param2);
    }
    case ZM_List_Add: {
        //判断添加的元素是否是表头控件
        if (!_tcsicmp(ZCCALL(ZM_GetType, Param1, NULL, NULL) ,(ZPARAM)ZC_ListHeader)) {
            if (p->m_pHeader != Param1 && ZCCALL(ZM_Layout_GetCount, p->m_pHeader, NULL, NULL) == 0) {
                //删除原来的头元素
                FreeZuiControl(p->m_pHeader, FALSE);
                p->m_pHeader = Param1;
                if (cp->m_pOs != NULL)
                    ZCCALL(ZM_SetOs, (ZuiControl)p->m_pHeader, cp, (ZPARAM)TRUE);
                p->m_ListInfo.m_iColumns = MIN((int)ZCCALL(ZM_Layout_GetCount, p->m_pHeader, NULL,  NULL), ZLIST_MAX_COLUMNS);
                return 0;
            }
            return FALSE;
        }
        if (!_tcsicmp(ZCCALL(ZM_GetType, Param1, NULL, NULL), (ZPARAM)ZC_ListBody)) {
            if (p->m_pList != Param1 && ZCCALL(ZM_Layout_GetCount, p->m_pList, NULL, NULL) == 0) {
                //删除原来的表体
                FreeZuiControl(p->m_pList, FALSE);
                p->m_pList = Param1;
                if (cp->m_pOs != NULL)
                    ZCCALL(ZM_SetOs, (ZuiControl)p->m_pList, cp, (ZPARAM)TRUE);
                ZuiVerticalLayoutProc(ZM_Layout_Add, cp, p->old_udata, p->m_pList, Param2);
                ZCCALL(ZM_ListBody_SetOwner, p->m_pList, cp, NULL);
                return 0;
            }
            return FALSE;
        }
        //判断元素是否是表头元素
        if (!_tcsicmp(ZCCALL(ZM_GetType, Param1, NULL, NULL), (ZPARAM)ZC_ListHeaderItem)) {
            //插入到头容器
            ZuiBool ret = (ZuiBool)ZCCALL(ZM_Layout_Add, p->m_pHeader, Param1, NULL);
            //计算列数量
            p->m_ListInfo.m_iColumns = MIN((int)ZCCALL(ZM_Layout_GetCount, p->m_pHeader, NULL, NULL), ZLIST_MAX_COLUMNS);
            return (ZPARAM)ret;
        }
        // 插入的元素是行数据
        if (ZCCALL(ZM_GetObject, Param1, (ZPARAM)ZC_ListElement, NULL)) {
            return ZCCALL(ZM_Layout_Add, p->m_pList, Param1, NULL);
        }

        return (ZPARAM)TRUE;
    }
    case ZM_List_AddAt: {
        //判断添加的元素是否是表头控件
        if (!_tcsicmp(ZCCALL(ZM_GetType, Param1, NULL, NULL), (ZPARAM)ZC_ListHeader)) {
            if (p->m_pHeader != Param1 && ZCCALL(ZM_Layout_GetCount, p->m_pHeader, NULL, NULL) == 0) {
                //删除原来的头元素
                FreeZuiControl(p->m_pHeader, FALSE);
                p->m_pHeader = Param1;
                if (cp->m_pOs != NULL)
                    ZCCALL(ZM_SetOs, (ZuiControl)p->m_pHeader, cp, (ZPARAM)TRUE);
                p->m_ListInfo.m_iColumns = MIN((int)ZCCALL(ZM_Layout_GetCount, p->m_pHeader, NULL, NULL), ZLIST_MAX_COLUMNS);
                //表头永远在第一个位置
                return 0;
            }
            return FALSE;
        }
        //判断元素是否是表头元素
        if (!_tcsicmp(ZCCALL(ZM_GetType, Param1, NULL, NULL), (ZPARAM)ZC_ListHeaderItem)) {
            //插入到头容器指定位置
            ZuiBool ret = (ZuiBool)ZCCALL(ZM_Layout_AddAt, p->m_pHeader, Param1, Param2);
            //计算列数量
            p->m_ListInfo.m_iColumns = MIN((int)ZCCALL(ZM_Layout_GetCount, p->m_pHeader, NULL, NULL), ZLIST_MAX_COLUMNS);
            return (ZPARAM)ret;
        }
        // 插入的元素是行数据
        if (ZCCALL(ZM_GetObject, Param1, (ZPARAM)ZC_ListElement, NULL)) {
            return ZCCALL(ZM_Layout_Add, p->m_pList, Param1, Param2);
        }
        return (ZPARAM)TRUE;
    }
    case ZM_List_Remove: {
        //判断删除的元素是否是表头控件
        //判断删除的元素是否是表头元素
        if (!_tcsicmp(ZCCALL(ZM_GetType, Param1, NULL, NULL), (ZPARAM)ZC_ListHeaderItem))
            return ZCCALL(ZM_Layout_Remove, p->m_pHeader, Param1, Param2);
        //判断删除的元素是否是表体控件
        // 删除的元素是行数据
        int iIndex = -1;
        if (!_tcsicmp(ZCCALL(ZM_GetType, Param1, NULL, NULL), (ZPARAM)ZC_ListElement)) {
            iIndex = (int)ZCCALL(ZM_Layout_GetItemIndex, p->m_pList, Param1, 0);
            if (iIndex == -1) return FALSE;
            return ZCCALL(ZM_Layout_RemoveAt, p->m_pList, Param1, (ZPARAM)iIndex);
        }
        break;
    }
    case ZM_List_RemoveAt: {
        //此调用只删除表数据,不影响表头
        if (!ZCCALL(ZM_Layout_RemoveAt, p->m_pList, Param1, Param2))
            return FALSE;

        return (ZPARAM)TRUE;
    }
    case ZM_List_RemoveAll: {
        p->m_iCurSel = -1;
        p->m_iExpandedItem = -1;
        return ZCCALL(ZM_Layout_RemoveAll, p->m_pList, NULL, NULL);
    }
    case ZM_List_SelectItem: {
        // 取消所有选择项
        ZCCALL(ZM_List_UnSelectAllItems, cp, NULL, NULL);
        // 判断是否合法列表项
        if (Param1 < 0) return FALSE;
        ZuiControl pControl = ZCCALL(ZM_List_GetItemAt, cp, Param1, NULL);
        if (pControl == NULL) return FALSE;
        if (!ZCCALL(ZM_ListElement_Select, pControl, (ZPARAM)TRUE, NULL)) {
            return FALSE;
        }
        int iLastSel = p->m_iCurSel;
        p->m_iCurSel = (int)Param1;
        darray_append(p->m_aSelItems, Param1);
//定位滚动条
//设置焦点
        if (cp->m_pOs != NULL && iLastSel != p->m_iCurSel) {
            //事件通知

        }

        return (ZPARAM)TRUE;
    }
    case ZM_List_SelectMultiItem: {

    }
    case ZM_List_SetMultiSelect: {

    }
    case ZM_List_IsMultiSelect: {

    }
    case ZM_List_UnSelectItem: {
        if (!p->m_bMultiSel)
            return FALSE;
        if (Param2) {

        }
        else {
            if (Param1 < 0)
                return FALSE;
            ZuiControl pControl = ZCCALL(ZM_List_GetItemAt, cp, Param1, NULL);
            if (pControl == NULL) return FALSE;
            if (!pControl->m_bEnabled) return FALSE;

            int aIndex = darray_find(p->m_aSelItems, Param1);
            if (aIndex < 0) return FALSE;
            if (!ZCCALL(ZM_ListElement_SelectMulti, pControl, FALSE, NULL)) return FALSE;
            if (p->m_iCurSel == (int)Param1) p->m_iCurSel = -1;
            darray_delete(p->m_aSelItems, aIndex);
        }
        return (ZPARAM)TRUE;
    }
    case ZM_List_SelectAllItems: {

    }
    case ZM_List_UnSelectAllItems: {
        for (int i = 0; i < p->m_aSelItems->count; ++i) {
            int iSelIndex = (int)darray_getat(p->m_aSelItems, i);
            ZuiControl pControl = ZCCALL(ZM_List_GetItemAt, cp, (ZPARAM)iSelIndex,NULL);;
            if (pControl == NULL) continue;
            if (!pControl->m_bEnabled) continue;
            if (!ZCCALL(ZM_ListElement_SelectMulti, pControl, FALSE, NULL)) continue;
        }
        darray_empty(p->m_aSelItems);
        p->m_iCurSel = -1;
        return (ZPARAM)TRUE;
    }
    //直接转发到列表体的消息
    case ZM_List_GetCount:
    case ZM_List_GetItemAt:
        return ZCCALL(ProcId, p->m_pList, Param1, Param2);
    case ZM_OnCreate: {
        p = (ZuiList)malloc(sizeof(ZList));
        memset(p, 0, sizeof(ZList));
        //保存原来的回调地址,创建成功后回调地址指向当前函数
        //创建继承的控件 保存数据指针
        p->old_udata = ZuiVerticalLayoutProc(ZM_OnCreate, cp, 0, 0, 0);
        p->old_call = (ZCtlProc)&ZuiVerticalLayoutProc;

        p->m_aSelItems = darray_create();

        p->m_ListInfo.m_cLineColor = 0xFF686868;
        p->m_ListInfo.m_cHotBkColor = 0xFF888888;
        p->m_ListInfo.m_cSelectedBkColor = 0xFFa8a8a8;
        return p;
    }
    case ZM_OnDestroy: {
        ZCtlProc old_call = p->old_call;
        ZVoid old_udata = p->old_udata;
        if (p->m_pHeader) {
            ZCCALL(ProcId, p->m_pHeader, (ZPARAM)FALSE, Param2);
        }
        old_call(ProcId, cp, old_udata, Param1, Param2);

        free(p);
        return 0;
    }
    case ZM_GetObject: {
        if (_tcsicmp(Param1, (ZPARAM)ZC_List) == 0)
            return (ZPARAM)p;
        break;
    }
    case ZM_GetType:
        return (ZPARAM)ZC_List;
    case ZM_CoreInit:
        //将辅助控件注册到系统
        ZuiControlRegisterAdd(ZC_ListBody, (ZCtlProc)&ZuiListBodyProc);
        ZuiControlRegisterAdd(ZC_ListHeader, (ZCtlProc)&ZuiListHeaderProc);
        return (ZPARAM)TRUE;
    case ZM_CoreUnInit:
        return (ZPARAM)TRUE;
    default:
        break;
    }
    return p->old_call(ProcId, cp, p->old_udata, Param1, Param2);
}
ZEXPORT ZINT ZCALL ZuiListBodyProc(ZINT ProcId, ZuiControl cp, ZuiListBody p, ZPARAM Param1, ZPARAM Param2) {
    switch (ProcId)
    {
    case ZM_Layout_Add:
    {
        ZCCALL(ZM_ListElement_SetOwner, Param1, p->m_pOwner, NULL);
        ZCCALL(ZM_ListElement_SetIndex, Param1, ZCCALL(ZM_List_GetCount, cp, NULL, NULL), NULL);
        break;
    }
    case ZM_Layout_AddAt:
    {
        if (!ZuiVerticalLayoutProc(ZM_Layout_AddAt, cp, p->old_udata, Param1, Param2))
            return FALSE;
        ZCCALL(ZM_ListElement_SetOwner, Param1, cp, NULL);
        ZCCALL(ZM_ListElement_SetIndex, Param1, Param2, NULL);
        int j = (int)ZCCALL(ZM_List_GetCount, cp, NULL, NULL);
        for (int index = (int)Param2 + 1; index < j; index++) {
            ZuiControl pControl = ZCCALL(ZM_Layout_GetItemAt, cp, (ZPARAM)index, NULL);
            if (pControl)
                ZCCALL(ZM_ListElement_SetIndex, pControl, (ZPARAM)index, NULL);
        }
        return 0;
    }
    case ZM_Layout_RemoveAt:
    {
        if (!ZuiVerticalLayoutProc(ZM_Layout_RemoveAt, cp, p->old_udata, Param1, Param2))
            return FALSE;
        int j = (int)ZCCALL(ZM_List_GetCount, cp, NULL, NULL);
        for (int index = (int)Param2; index < j; index++) {
            ZuiControl pControl = ZCCALL(ZM_Layout_GetItemAt, cp, (ZPARAM)index, NULL);
            if (pControl)
                ZCCALL(ZM_ListElement_SetIndex, pControl, (ZPARAM)index, NULL);
        }
        return 0;
    }
    case ZM_OnPaint: {
        p->old_call(ProcId, cp, p->old_udata, Param1, Param2);
        ZuiRect rrc = &cp->m_rcItem;
        ZuiRect rc = Param2;
        ZuiLayout op = ZCCALL(ZM_GetObject, cp, (ZPARAM)ZC_Layout, NULL);
        if (op->m_pVerticalScrollBar && op->m_pVerticalScrollBar->m_bVisible) {
            int sw = (int)ZCCALL(ZM_GetFixedWidth, op->m_pVerticalScrollBar, NULL, NULL);
            if (rc->right > (rrc->right - sw)) {
                rc->right -= sw;
            }
        }
        ZuiControl pHeader = ((ZuiList)ZCCALL(ZM_GetObject, p->m_pOwner, (ZPARAM)ZC_List, NULL))->m_pHeader;
        if (pHeader->m_bVisible) {
            ZCCALL(ProcId, pHeader, Param1, rc);
        }
        return 0;
    }
    case ZM_FindControl: {
        ZuiControl pResult = NULL;
        pResult = ZCCALL(ProcId, ((ZuiList)ZCCALL(ZM_GetObject, p->m_pOwner, (ZPARAM)ZC_List, NULL))->m_pHeader, Param1, Param2);
        if (pResult)
            return pResult;
        return p->old_call(ProcId, cp, p->old_udata, Param1, Param2);
    }
    case ZM_SetPos: {
        ZuiDefaultControlProc(ProcId, cp, 0, Param1, Param2);
        ZRect rc = *(ZRect*)Param1;
        ZuiLayout op = ZCCALL(ZM_GetObject, cp, (ZPARAM)ZC_Layout, NULL);
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
            rc.right -= (int)ZCCALL(ZM_GetFixedWidth, op->m_pVerticalScrollBar, NULL, NULL);
        if (op->m_pHorizontalScrollBar && op->m_pHorizontalScrollBar->m_bVisible)
            rc.bottom -= (int)ZCCALL(ZM_GetFixedHeight, op->m_pHorizontalScrollBar, NULL, NULL);
       
        // Determine the minimum size
        SIZE szAvailable = { rc.right - rc.left, rc.bottom - rc.top };
        if (op->m_pHorizontalScrollBar && op->m_pHorizontalScrollBar->m_bVisible)
            szAvailable.cx += (int)ZCCALL(ZM_ScrollBar_GetScrollRange, op->m_pHorizontalScrollBar, NULL, NULL);


        int cxNeeded = 0;
        int nAdjustables = 0;
        int cyFixed = 0;
        int nEstimateNum = 0;
        for (int it1 = 0; it1 < darray_len(op->m_items); it1++) {
            ZuiControl pControl = (ZuiControl)(op->m_items->data[it1]);
            if (!pControl->m_bVisible) continue;
            if (pControl->m_bFloat) continue;
            SIZE sz;
            SIZE * psz = (SIZE *)ZCCALL(ZM_EstimateSize, pControl, (void *)&szAvailable, 0);
            sz.cx = psz->cx;
            sz.cy = psz->cy;
            if (sz.cy == 0) {
                nAdjustables++;
            }
            else {
                if (sz.cy < (LONG)ZCCALL(ZM_GetMinHeight, pControl, 0, 0))
                    sz.cy = (LONG)ZCCALL(ZM_GetMinHeight, pControl, 0, 0);
                if (sz.cy > (LONG)ZCCALL(ZM_GetMaxHeight, pControl, 0, 0))
                    sz.cy = (LONG)ZCCALL(ZM_GetMaxHeight, pControl, 0, 0);
            }
            cyFixed += sz.cy + ((ZRect *)(ZCCALL(ZM_GetPadding, pControl, 0, 0)))->top + ((ZRect *)(ZCCALL(ZM_GetPadding, pControl, 0, 0)))->bottom;

            ZRect *rcPadding = (ZRect *)(ZCCALL(ZM_GetPadding, pControl, 0, 0));
            sz.cx = MAX(sz.cx, 0);
            if (sz.cx < (LONG)ZCCALL(ZM_GetMinWidth, pControl, 0, 0)) { sz.cx = (LONG)ZCCALL(ZM_GetMinWidth, pControl, 0, 0); }
            if (sz.cx > (LONG)ZCCALL(ZM_GetMaxWidth, pControl, 0, 0)) { sz.cx = (LONG)ZCCALL(ZM_GetMaxWidth, pControl, 0, 0); }

            cxNeeded = MAX(cxNeeded, sz.cx);
            nEstimateNum++;
        }
        cyFixed += (nEstimateNum - 1) * op->m_iChildPadding;

        int ph = 0;
        if (p->m_pOwner) {
            //得到表头控件
            ZuiControl pHeader = ZCCALL(ZM_List_GetHeader, p->m_pOwner, cp, NULL);
            if (pHeader != NULL && ZCCALL(ZM_Layout_GetCount, pHeader, NULL, NULL) > 0) {
                SIZE sz = { rc.right - rc.left, rc.bottom - rc.top };
                cxNeeded = MAX(0, ((SIZE *)ZCCALL(ZM_EstimateSize, pHeader, (void *)&sz, 0))->cx);
                if (pHeader->m_bVisible) {
                    ph = ((SIZE *)ZCCALL(ZM_EstimateSize, pHeader, (void *)&sz, 0))->cy;
                }
                ZRect rcHeader = *(ZRect *)ZCCALL(ZM_GetPos, pHeader, NULL, NULL);
                if (op->m_pHorizontalScrollBar && op->m_pHorizontalScrollBar->m_bVisible)
                {
                    int nOffset = (LONG)ZCCALL(ZM_ScrollBar_GetScrollPos, op->m_pHorizontalScrollBar, NULL, NULL);
                    rcHeader.left = rc.left - nOffset; rcHeader.top = rc.top; rcHeader.right = rc.right; rcHeader.bottom = rc.top + ph;
                    ZCCALL(ZM_SetPos, pHeader, &rcHeader, (ZPARAM)ZuiOnSize);
                }
                else {
                    rcHeader.left = rc.left; rcHeader.top = rc.top; rcHeader.right = rc.right; rcHeader.bottom = rc.top + ph;
                    ZCCALL(ZM_SetPos, pHeader, &rcHeader, (ZPARAM)ZuiOnSize);
                }
            }
        }
        rc.top += ph;
        // Place elements
        int cyNeeded = 0;
        int cyExpand = 0;
        if (nAdjustables > 0)
            cyExpand = MAX(0, (szAvailable.cy - cyFixed) / nAdjustables);
        // Position the elements
        SIZE szRemaining = szAvailable;
        int iPosY = rc.top;
        if (op->m_pVerticalScrollBar && op->m_pVerticalScrollBar->m_bVisible) {
            iPosY -= (LONG)ZCCALL(ZM_ScrollBar_GetScrollPos, op->m_pVerticalScrollBar, NULL, NULL);
        }
        int iPosX = rc.left;
        if (op->m_pHorizontalScrollBar && op->m_pHorizontalScrollBar->m_bVisible) {
            iPosX -= (LONG)ZCCALL(ZM_ScrollBar_GetScrollPos, op->m_pHorizontalScrollBar, NULL, NULL);
        }
        int iAdjustable = 0;
        int cyFixedRemaining = cyFixed;
        for (int it2 = 0; it2 < darray_len(op->m_items); it2++) {
            ZuiControl pControl = (ZuiControl)(op->m_items->data[it2]);
            if (!pControl->m_bVisible) continue;
            if (pControl->m_bFloat) {
                ZCCALL(ZM_Layout_SetFloatPos, cp, (ZPARAM)it2, 0);
                continue;
            }

            ZRect *rcPadding = (ZRect *)(ZCCALL(ZM_GetPadding, pControl, 0, 0));
            szRemaining.cy -= rcPadding->top;
            SIZE sz;
            SIZE * psz = (SIZE *)ZCCALL(ZM_EstimateSize, pControl, (void *)&szRemaining, 0);
            sz.cx = psz->cx;
            sz.cy = psz->cy;
            if (sz.cy == 0) {
                iAdjustable++;
                sz.cy = cyExpand;
                // Distribute remaining to last element (usually round-off left-overs)
                if (iAdjustable == nAdjustables) {
                    sz.cy = MAX(0, szRemaining.cy - rcPadding->bottom - cyFixedRemaining);
                }
                if (sz.cy < (LONG)ZCCALL(ZM_GetMinHeight, pControl, 0, 0)) {
                    sz.cy = (LONG)ZCCALL(ZM_GetMinHeight, pControl, 0, 0); 
                }
                if (sz.cy > (LONG)ZCCALL(ZM_GetMaxHeight, pControl, 0, 0)) {
                    sz.cy = (LONG)ZCCALL(ZM_GetMaxHeight, pControl, 0, 0);
                }

            }
            else {
                if (sz.cy < (LONG)ZCCALL(ZM_GetMinHeight, pControl, 0, 0)) {
                    sz.cy = (LONG)ZCCALL(ZM_GetMinHeight, pControl, 0, 0); 
                }
                if (sz.cy > (LONG)ZCCALL(ZM_GetMaxHeight, pControl, 0, 0)) {
                    sz.cy = (LONG)ZCCALL(ZM_GetMaxHeight, pControl, 0, 0); 
                }
                cyFixedRemaining -= sz.cy;
            }

            sz.cx = MAX(cxNeeded, szAvailable.cx - rcPadding->left - rcPadding->right);

            if (sz.cx < (LONG)ZCCALL(ZM_GetMinWidth, pControl, 0, 0))
                sz.cx = (LONG)ZCCALL(ZM_GetMinWidth, pControl, 0, 0);
            if (sz.cx > (LONG)ZCCALL(ZM_GetMaxWidth, pControl, 0, 0))
                sz.cx = (LONG)ZCCALL(ZM_GetMaxWidth, pControl, 0, 0);

            ZRect rcCtrl = { iPosX + rcPadding->left, iPosY + rcPadding->top, iPosX + rcPadding->left + sz.cx, iPosY + sz.cy + rcPadding->top + rcPadding->bottom };
            ZCCALL(ZM_SetPos, pControl, &rcCtrl, (ZPARAM)ZuiOnSize);

            iPosY += sz.cy + op->m_iChildPadding + rcPadding->top + rcPadding->bottom;
            cyNeeded += sz.cy + rcPadding->top + rcPadding->bottom;
            szRemaining.cy -= sz.cy + op->m_iChildPadding + rcPadding->bottom;
        }
        cyNeeded += (nEstimateNum - 1) * op->m_iChildPadding;
        // Process the scrollbar
        rc.top -= ph; cyNeeded += ph;
        
        ZSize SBarSize;
        SBarSize.cx = cxNeeded;
        SBarSize.cy = cyNeeded;
        ZCCALL(ZM_Layout_ProcessScrollBar, cp, (ZPARAM)&rc, (ZPARAM)&SBarSize);
        return 0;
    }
    case ZM_ListBody_SetScrollPos: {
        int cx = 0;
        int cy = 0;
        SIZE *szPos = Param1;
        ZuiLayout op = ZCCALL(ZM_GetObject, cp, (ZPARAM)ZC_Layout, NULL);

        if (op->m_pVerticalScrollBar && op->m_pVerticalScrollBar->m_bVisible) {
            int iLastScrollPos = (int)ZCCALL(ZM_ScrollBar_GetScrollPos, op->m_pVerticalScrollBar, NULL, NULL);
            ZCCALL(ZM_ScrollBar_SetScrollPos, op->m_pVerticalScrollBar, (ZPARAM)szPos->cy, NULL);
            cy = (int)ZCCALL(ZM_ScrollBar_GetScrollPos, op->m_pVerticalScrollBar, NULL, NULL) - iLastScrollPos;
        }

        if (op->m_pHorizontalScrollBar && op->m_pHorizontalScrollBar->m_bVisible) {
            int iLastScrollPos = (int)ZCCALL(ZM_ScrollBar_GetScrollPos, op->m_pHorizontalScrollBar, NULL, NULL);
            ZCCALL(ZM_ScrollBar_SetScrollPos, op->m_pHorizontalScrollBar, (ZPARAM)szPos->cx, NULL);
            cx = (int)ZCCALL(ZM_ScrollBar_GetScrollPos, op->m_pHorizontalScrollBar, NULL, NULL) - iLastScrollPos;
        }

        if (cx == 0 && cy == 0)
            return 0;
        ZRect rcPos;

        if (p->m_pOwner) {
            //得到表头控件
            ZuiControl pHeader = ZCCALL(ZM_List_GetHeader, p->m_pOwner, cp, NULL);
            if (pHeader == NULL)
                return 0;
            ZuiListInfo pInfo = ZCCALL(ZM_List_GetListInfo, p->m_pOwner, cp, NULL);
            pInfo->m_iColumns = MIN((int)ZCCALL(ZM_Layout_GetCount, pHeader, NULL, NULL), ZLIST_MAX_COLUMNS);
            for (int i = 0; i < pInfo->m_iColumns; i++) {
                ZuiControl pControl = ZCCALL(ZM_Layout_GetItemAt, pHeader, (ZPARAM)i, NULL);
                if (!pControl->m_bVisible) continue;
                if (pControl->m_bFloat) continue;

                rcPos = *(ZRect *)ZCCALL(ZM_GetPos, pControl, NULL, NULL);
                rcPos.left -= cx;
                rcPos.right -= cx;
                ZCCALL(ZM_SetPos, pControl, &rcPos, (ZPARAM)ZuiOnSize);
                pInfo->m_rcColumn[i] = *(ZRect *)ZCCALL(ZM_GetPos, pControl, NULL, NULL);
            }
            //将列表元素的移动调整到头移动完成后
            for (int it2 = 0; it2 < op->m_items->count; it2++) {
                ZuiControl pControl = (ZuiControl)(op->m_items->data[it2]);
                if (!pControl->m_bVisible) continue;
                if (pControl->m_bFloat) continue;

                rcPos = *(ZRect *)ZCCALL(ZM_GetPos, pControl, NULL, NULL);
                rcPos.left -= cx;
                rcPos.right -= cx;
                rcPos.top -= cy;
                rcPos.bottom -= cy;
                ZCCALL(ZM_SetPos, pControl, &rcPos, (ZPARAM)ZuiOnSize);
            }
        }
        ZuiControlInvalidate(cp, TRUE);
        return 0;
    }
    case ZM_ListBody_SetOwner: {
        if (Param1)
            p->m_pOwner = Param1;
        break;
    }
    case ZM_OnCreate: {
        p = (ZuiListBody)malloc(sizeof(ZListBody));
        memset(p, 0, sizeof(ZListBody));
        //保存原来的回调地址,创建成功后回调地址指向当前函数
        //创建继承的控件 保存数据指针
        p->old_udata = ZuiVerticalLayoutProc(ZM_OnCreate, cp, 0, 0, 0);
        p->old_call = (ZCtlProc)&ZuiVerticalLayoutProc;



        return p;
    }
    case ZM_OnDestroy: {
        ZCtlProc old_call = p->old_call;
        ZVoid old_udata = p->old_udata;
        old_call(ProcId, cp, old_udata, Param1, Param2);
        free(p);

        return 0;
    }
    case ZM_GetObject: {
        if (_tcsicmp(Param1, (ZPARAM)ZC_ListBody) == 0)
            return (ZPARAM)p;
        break;
    }
    case ZM_GetType:
        return (ZPARAM)ZC_ListBody;
    case ZM_CoreInit:
        //将辅助控件注册到系统
        ZuiControlRegisterAdd(ZC_ListElement, (ZCtlProc)&ZuiListElementProc);
        return (ZPARAM)TRUE;
    case ZM_CoreUnInit:
        return (ZPARAM)NULL;
    default:
        break;
    }
    return p->old_call(ProcId, cp, p->old_udata, Param1, Param2);
}
ZEXPORT ZINT ZCALL ZuiListElementProc(ZINT ProcId, ZuiControl cp, ZuiListElement p, ZPARAM Param1, ZPARAM Param2) {
    switch (ProcId)
    {
    case ZM_Layout_Add:
    case ZM_Layout_AddAt: {
        ZuiLayoutProc(ProcId, cp, p->old_udata, Param1, Param2);
        if (p->m_pOwner == NULL)
            return FALSE;
        ZuiControl pHeader = ZCCALL(ZM_List_GetHeader, p->m_pOwner, cp, NULL);
        if (pHeader == NULL)
            return FALSE;
        ZuiLayout op = ZCCALL(ZM_GetObject, cp, (ZPARAM)ZC_Layout, NULL);
        int i = darray_find(op->m_items, Param1);

        if (i >= 0) {
            ZuiControl pHeaderItem = ZCCALL(ZM_Layout_GetItemAt, pHeader, (ZPARAM)i, NULL);
            if (pHeaderItem == NULL)
                break;

            ZuiListHeaderItem lhi = ZCCALL(ZM_GetObject, pHeaderItem, (ZPARAM)ZC_ListHeaderItem, NULL);
            ZuiLabel lop = ZCCALL(ZM_GetObject, op->m_items->data[i], (ZPARAM)ZC_Label, NULL);
            lop->m_uTextStyle |= lhi->m_uListTextStyle;
            lop->m_cTextColor = lhi->m_cTextColor;
            lop->m_rFont = lhi->m_rListFont;

        }
        return (ZPARAM)TRUE;
    }
    case ZM_OnEvent: {
        TEventUI *event = (TEventUI *)Param1;
        if (!cp->m_bMouseEnabled && event->Type > ZEVENT__MOUSEBEGIN && event->Type < ZEVENT__MOUSEEND) {
            if (p->m_pOwner != NULL)
                ZCCALL(ZM_OnEvent, p->m_pOwner, Param1, NULL);
            else
                ZuiLayoutProc(ZM_OnEvent, cp, 0, Param1, NULL);
            return 0;
        }

        if (event->Type == ZEVENT_LDBLCLICK)
        {
            if (cp->m_bEnabled) {
                //Activate();
                ZuiControlInvalidate(cp, TRUE);
            }
            return 0;
        }
        if (event->Type == ZEVENT_KEYDOWN && cp->m_bEnabled)
        {
            if (event->chKey == VK_RETURN) {
                ZuiControlInvalidate(cp, TRUE);
                return 0;
            }
        }
        if (event->Type == ZEVENT_LBUTTONDOWN)
        {
            if (cp->m_bEnabled) {
                if ((GetKeyState(VK_CONTROL) & 0x8000)) {
                }
                else
                    ZCCALL(ZM_ListElement_Select, cp, (ZPARAM)TRUE, NULL);
            }
            return 0;
        }
        if (event->Type == ZEVENT_LBUTTONUP)
        {
            if (cp->m_bEnabled) {
            }
            return 0;
        }
        if (event->Type == ZEVENT_MOUSEMOVE)
        {
            return 0;
        }
        if (event->Type == ZEVENT_MOUSEENTER)
        {
            if (cp->m_bEnabled) {
                p->m_uButtonState |= ZSTATE_HOT;
                ZuiControlInvalidate(cp, TRUE);
            }
            return 0;
        }
        if (event->Type == ZEVENT_MOUSELEAVE)
        {
            if ((p->m_uButtonState & ZSTATE_HOT) != 0) {
                p->m_uButtonState &= ~ZSTATE_HOT;
                ZuiControlInvalidate(cp, TRUE);
            }
            return 0;
        }
        if (event->Type == ZEVENT_TIMER)
        {
            return 0;
        }

        if (event->Type == ZEVENT_CONTEXTMENU)
        {
        }
        if (p->m_pOwner != NULL) ZCCALL(ZM_OnEvent, p->m_pOwner, Param1, NULL);
        else ZuiLayoutProc(ZM_OnEvent, cp, 0, Param1, NULL);
        break;
    }
    case ZM_Invalidate: {
        if (!cp->m_bVisible)
            return 0;

        if (cp->m_pParent) {
            ZuiLayout pParentContainer = ZCCALL(ZM_GetObject, cp->m_pParent, (ZPARAM)ZC_Layout, NULL);
            if (pParentContainer) {
                ZRect rc = *(ZRect *)ZCCALL(ZM_GetPos, cp->m_pParent, NULL, NULL);
                ZRect rcInset = pParentContainer->m_rcInset;
                rc.left += rcInset.left;
                rc.top += rcInset.top;
                rc.right -= rcInset.right;
                rc.bottom -= rcInset.bottom;
                if (pParentContainer->m_pVerticalScrollBar && pParentContainer->m_pVerticalScrollBar->m_bVisible)
                    rc.right -= (int)ZCCALL(ZM_GetFixedWidth, pParentContainer->m_pVerticalScrollBar, NULL, NULL);
                if (pParentContainer->m_pHorizontalScrollBar && pParentContainer->m_pHorizontalScrollBar->m_bVisible)
                    rc.bottom -= (int)ZCCALL(ZM_GetFixedHeight, pParentContainer->m_pHorizontalScrollBar, NULL, NULL);

                ZRect invalidateRc = cp->m_rcItem;
                if (!IntersectRect((LPRECT)&invalidateRc, (const RECT *)&cp->m_rcItem, (const RECT *)&rc))
                {
                    return 0;
                }

                ZuiControl pParent = cp->m_pParent;
                ZRect rcTemp;
                ZRect *rcParent;
                while (pParent = pParent->m_pParent)
                {
                    rcTemp = invalidateRc;
                    rcParent = (ZRect *)ZCCALL(ZM_GetPos, pParent, NULL, NULL);
                    if (!IntersectRect((LPRECT)&invalidateRc, (const RECT *)&rcTemp, (const RECT *)rcParent))
                    {
                        return 0;
                    }
                }
                //重置动画
                if (Param1 && cp->m_aAnime)
                    cp->m_aAnime->steup = 0;
                if (cp->m_pOs != NULL) ZuiOsInvalidateRect(cp->m_pOs, &invalidateRc);
            }
            else {
                ZuiLayoutProc(ZM_Invalidate, cp, p->old_udata, Param1, NULL);
            }
        }
        else {
            ZuiLayoutProc(ZM_Invalidate, cp, p->old_udata, Param1, NULL);
        }
        return 0;
    }
    case ZM_SetPos: {
        ZuiLayout op = ZCCALL(ZM_GetObject, cp, (ZPARAM)ZC_Layout, NULL);
        //通知父容器调整布局
        ZuiLayoutProc(ProcId, cp, p->old_udata, Param1, Param2);
        if (p->m_pOwner == NULL)
            return 0;

        ZuiControl pHeader = ZCCALL(ZM_List_GetHeader, p->m_pOwner, cp, NULL);
        if (pHeader == NULL)
            return 0;
        int nCount = op->m_items->count;
        for (int i = 0; i < nCount; i++)
        {
            ZuiControl pListItem = (ZuiControl)(op->m_items->data[i]);
            ZuiControl pHeaderItem = ZCCALL(ZM_Layout_GetItemAt, pHeader, (ZPARAM)i, NULL);
            if (pHeaderItem == NULL)
                return 0;
            ZRect *rcHeaderItem = ZCCALL(ZM_GetPos, pHeaderItem, NULL, NULL);
            if (pListItem != NULL && !(rcHeaderItem->left == 0 && rcHeaderItem->right == 0))
            {
                ZRect rt = *(ZRect *)ZCCALL(ZM_GetPos, pListItem, NULL, NULL);
                rt.left = rcHeaderItem->left;
                rt.right = rcHeaderItem->right;

                ZCCALL(ZM_SetPos, pListItem, &rt, (ZPARAM)ZuiOnSize);
            }
        }
        ZuiControl tmpHeaderItem = ZCCALL(ZM_Layout_GetItemAt, pHeader, (ZPARAM)((int)ZCCALL(ZM_Layout_GetCount, pHeader, NULL, NULL) - 1), NULL);
        if (tmpHeaderItem) {
            cp->m_rcItem.right = tmpHeaderItem->m_rcItem.right;
        }
        return 0;
    }
    case ZM_OnPaintBkColor: {
        ZRect *rc = (ZRect*)&cp->m_rcItem;
        if (p->m_pOwner == NULL)
            return 0;
        ZuiListInfo pInfo = ZCCALL(ZM_List_GetListInfo, p->m_pOwner, cp, NULL);



        DWORD iBackColor = 0;
        if (!pInfo->m_bAlternateBk || p->m_iIndex % 2 == 0)
            iBackColor = pInfo->m_cBkColor;

        if ((p->m_uButtonState & ZSTATE_HOT) != 0) {
            iBackColor = pInfo->m_cHotBkColor;
        }
        if (p->m_bSelected) {
            iBackColor = pInfo->m_cSelectedBkColor;
        }
        if (!cp->m_bEnabled) {
            iBackColor = pInfo->m_cDisabledBkColor;
        }
        if (iBackColor != 0) {
            ZuiDrawFillRoundRect(cp, iBackColor, rc, &cp->m_rRound);
        }

        ZRect rcc;
        if (pInfo->m_cLineColor != 0) {
            if (pInfo->m_bShowRowLine) {
                MAKEZRECT(rcc, rc->left, rc->bottom - 1, rc->right, rc->bottom - 1);
                ZuiDrawLine(cp, pInfo->m_cLineColor, &rcc, 1);
            }
        }
        if (pInfo->m_cColumnColor) {
            if (pInfo->m_bShowColumnLine) {
                for (int i = 0; i < pInfo->m_iColumns; i++) {
                    MAKEZRECT(rcc, pInfo->m_rcColumn[i].right - 1, rc->top, pInfo->m_rcColumn[i].right  - 1, rc->bottom);
                    ZuiDrawLine(cp, pInfo->m_cColumnColor, &rcc, 1);
                }
            }
        }
        break;
    }
    case ZM_ListElement_SetOwner: {
        if (Param1)
            p->m_pOwner = Param1;
        break;
    }
    case ZM_ListElement_GetIndex: {
        return (ZPARAM)p->m_iIndex;
    }
    case ZM_ListElement_SetIndex: {
        p->m_iIndex = (int)Param1;
        break;
    }
    case ZM_ListElement_Select: {
        if (!cp->m_bEnabled)
            return FALSE;
        if (p->m_pOwner != NULL && p->m_bSelected)
            ZCCALL(ZM_List_UnSelectItem, p->m_pOwner, (ZPARAM)p->m_iIndex, (ZPARAM)TRUE);//如果被选中就先反选,然后重新选取
        if (Param1 == (ZPARAM)p->m_bSelected)
            return (ZPARAM)TRUE;
        p->m_bSelected = (ZuiBool)Param1;
        if (Param1 && p->m_pOwner != NULL)
            ZCCALL(ZM_List_SelectItem, p->m_pOwner, (ZPARAM)p->m_iIndex, FALSE);
        ZuiControlInvalidate(cp, TRUE);

        return (ZPARAM)TRUE;
    }
    case ZM_ListElement_SelectMulti: {
        if (!cp->m_bEnabled)
            return FALSE;
        if (Param1 == (ZPARAM)p->m_bSelected)
            return (ZPARAM)TRUE;

        p->m_bSelected = (ZuiBool)Param1;
        if (Param1 && p->m_pOwner != NULL)
            ZCCALL(ZM_List_SelectMultiItem, p->m_pOwner, (ZPARAM)p->m_iIndex, FALSE);
        ZuiControlInvalidate(cp, TRUE);
        return (ZPARAM)TRUE;
    }
    case ZM_OnCreate: {
        p = (ZuiListElement)malloc(sizeof(ZListElement));
        memset(p, 0, sizeof(ZListElement));
        //保存原来的回调地址,创建成功后回调地址指向当前函数
        //创建继承的控件 保存数据指针
        p->old_udata = ZuiLayoutProc(ZM_OnCreate, cp, 0, 0, 0);
        p->old_call = (ZCtlProc)&ZuiLayoutProc;

        p->m_iIndex = -1;
        return p;
    }
    case ZM_OnDestroy: {
        ZCtlProc old_call = p->old_call;
        ZVoid old_udata = p->old_udata;
        old_call(ProcId, cp, old_udata, (ZPARAM)TRUE, Param2); //设置Param1 让子控件不处理字体资源。
        free(p);

        return 0;
    }
    case ZM_GetObject: {
        if (_tcsicmp(Param1, (ZPARAM)ZC_ListElement) == 0)
            return (ZPARAM)p;
        break;
    }
    case ZM_GetType:
        return (ZPARAM)ZC_ListElement;
    case ZM_CoreInit:
        return (ZPARAM)TRUE;
    case ZM_CoreUnInit:
        return (ZPARAM)TRUE;
    default:
        break;
    }
    return p->old_call(ProcId, cp, p->old_udata, Param1, Param2);
}
ZEXPORT ZINT ZCALL ZuiListHeaderProc(ZINT ProcId, ZuiControl cp, ZuiListHeader p, ZPARAM Param1, ZPARAM Param2) {
    switch (ProcId)
    {
    case ZM_EstimateSize: {
        ZuiLayout op = ZCCALL(ZM_GetObject, cp, (ZPARAM)ZC_Layout, NULL);
        p->m_szXY.cy = cp->m_cxyFixed.cy;
        p->m_szXY.cx = 0;
        if (p->m_szXY.cy == 0 && cp->m_pOs != NULL) {
            for (int it = 0; it < op->m_items->count; it++) {
                SIZE * psz = (SIZE *)ZCCALL(ZM_EstimateSize, op->m_items->data[it], Param1, 0);
                p->m_szXY.cy = MAX(p->m_szXY.cy, psz->cy);
            }
        }

        for (int it = 0; it < op->m_items->count; it++) {
            SIZE * psz = (SIZE *)ZCCALL(ZM_EstimateSize, op->m_items->data[it], Param1, 0);
            p->m_szXY.cx += psz->cx;
        }

        return &p->m_szXY;
    }
    case ZM_OnCreate: {
        p = (ZuiListHeader)malloc(sizeof(ZListHeader));
        memset(p, 0, sizeof(ZListHeader));
        //保存原来的回调地址,创建成功后回调地址指向当前函数
        //创建继承的控件 保存数据指针
        p->old_udata = ZuiHorizontalLayoutProc(ZM_OnCreate, cp, 0, 0, 0);
        p->old_call = (ZCtlProc)&ZuiHorizontalLayoutProc;

        return p;
    }
    case ZM_OnDestroy: {
        ZCtlProc old_call = p->old_call;
        ZVoid old_udata = p->old_udata;
        old_call(ProcId, cp, old_udata, Param1, Param2);
        free(p);

        return 0;
    }
    case ZM_GetObject: {
        if (_tcsicmp(Param1, (ZPARAM)ZC_ListHeader) == 0)
            return (ZPARAM)p;
        break;
    }
    case ZM_GetType:
        return (ZPARAM)ZC_ListHeader;
    case ZM_CoreInit:
        //将辅助控件注册到系统
        ZuiControlRegisterAdd(ZC_ListHeaderItem, (ZCtlProc)&ZuiListHeaderItemProc);
        return (ZPARAM)TRUE;
    case ZM_CoreUnInit: {
        return (ZPARAM)TRUE;
    }
    default:
        break;
    }
    return p->old_call(ProcId, cp, p->old_udata, Param1, Param2);
}
ZEXPORT ZINT ZCALL ZuiListHeaderItemProc(ZINT ProcId, ZuiControl cp, ZuiListHeaderItem p, ZPARAM Param1, ZPARAM Param2) {
    switch (ProcId)
    {
    case ZM_GetControlFlags: {
        return (ZPARAM)ZFLAG_SETCURSOR;//需要设置鼠标
    }
    case ZM_OnEvent: {
        TEventUI *event = (TEventUI *)Param1;
        //不响应鼠标消息
        if (!cp->m_bMouseEnabled && event->Type > ZEVENT__MOUSEBEGIN && event->Type < ZEVENT__MOUSEEND) {
            if (cp->m_pParent != NULL)
                ZCCALL(ZM_OnEvent, cp->m_pParent, Param1, NULL);
            else
                ZuiLayoutProc(ZM_OnEvent, cp, 0, Param1, NULL);
            return 0;
        }

        //if (event->Type == ZEVENT_SETFOCUS)
        //{
        //    ZuiControlInvalidate(cp, TRUE);
        //}
        //if (event->Type == ZEVENT_KILLFOCUS)
        //{
        //    ZuiControlInvalidate(cp, TRUE);
        //}
        if (event->Type == ZEVENT_LBUTTONDOWN || event->Type == ZEVENT_LDBLCLICK)
        {
            if (!cp->m_bEnabled)
                return 0;
            ZRect *rcSeparator = ZCCALL(ZM_ListHeaderItem_GetThumbRect, cp, NULL, NULL);
            if (p->m_iSepWidth >= 0)
                rcSeparator->left -= 4;
            else
                rcSeparator->right += 4;
            if (PtInRect((LPRECT)rcSeparator, *(POINT *)&event->ptMouse)) {
                if (p->m_bDragable) {
                    p->m_uButtonState |= ZSTATE_CAPTURED;
                    p->ptLastMouse = event->ptMouse;
                }
            }
            else {
                p->m_uButtonState |= ZSTATE_PUSHED;
                //表头单击事件
                ZuiControlInvalidate(cp, TRUE);
            }
            return 0;
        }
        if (event->Type == ZEVENT_LBUTTONUP)
        {
            if ((p->m_uButtonState & ZSTATE_CAPTURED) != 0) {
                p->m_uButtonState &= ~ZSTATE_CAPTURED;
                if (cp->m_pParent)
                    ZuiControlNeedParentUpdate(cp->m_pParent);
            }
            else if ((p->m_uButtonState & ZSTATE_PUSHED) != 0) {
                p->m_uButtonState &= ~ZSTATE_PUSHED;
                ZuiControlInvalidate(cp, TRUE);
            }
            return 0;
        }
        if (event->Type == ZEVENT_MOUSEMOVE)
        {
            if ((p->m_uButtonState & ZSTATE_CAPTURED) != 0) {
                ZRect rc = cp->m_rcItem;
                if (p->m_iSepWidth >= 0) {
                    rc.right -= p->ptLastMouse.x - event->ptMouse.x;
                }
                else {
                    rc.left -= p->ptLastMouse.x - event->ptMouse.x;
                }

                if (rc.right - rc.left > (int)(ZCCALL(ZM_GetMinWidth, cp, 0, 0)) + 3) {
                    cp->m_cxyFixed.cx = rc.right - rc.left;
                    p->ptLastMouse = event->ptMouse;
                    if (cp->m_pParent)
                        ZuiControlNeedParentUpdate(cp->m_pParent);
                }
            }
            return 0;
        }
        if (event->Type == ZEVENT_SETCURSOR)
        {
            ZRect *rcSeparator = ZCCALL(ZM_ListHeaderItem_GetThumbRect, cp, NULL, NULL);
            if (p->m_iSepWidth >= 0)
                rcSeparator->left -= 4;
            else
                rcSeparator->right += 4;
            if (cp->m_bEnabled && p->m_bDragable && PtInRect((LPRECT)rcSeparator, *(POINT *)&event->ptMouse)) {
                ZuiOsSetCursor((unsigned int)ZIDC_SIZEWE);
                return 0;
            }
            ZuiOsSetCursor((unsigned int)ZIDC_ARROW);
        }
        if (event->Type == ZEVENT_MOUSEENTER)
        {
            if (cp->m_bEnabled) {
                p->m_uButtonState |= ZSTATE_HOT;
                ZuiControlInvalidate(cp, TRUE);
            }
            return 0;
        }
        if (event->Type == ZEVENT_MOUSELEAVE)
        {
            if (cp->m_bEnabled) {
                p->m_uButtonState &= ~ZSTATE_HOT;
                ZuiControlInvalidate(cp, TRUE);
            }
            return 0;
        }
        return 0;
    }
    case ZM_OnPaintText: {
        if (!cp->m_sText)
            return 0;
        ZRect *rc = &cp->m_rcItem;
        ZRect pt;
        pt.left = rc->left + p->m_rcPadding.left;
        pt.top = rc->top + p->m_rcPadding.top;
        pt.right = rc->right - p->m_rcPadding.right - p->m_iSepWidth;
        pt.bottom = rc->bottom - p->m_rcPadding.bottom;
        ZuiDrawString(cp, p->m_rFont ? p->m_rFont->p : Global_Font->p, cp->m_sText, _tcslen(cp->m_sText), &pt, p->m_cTextColor, p->m_uTextStyle);
        return 0;
    }
    case ZM_OnPaintStatusImage: {
        ZRect *rc = (ZRect *)&cp->m_rcItem;
        ZuiImage img;
        if (cp->m_bFocused)
            p->m_uButtonState |= ZSTATE_FOCUSED;
        else p->m_uButtonState &= ~ZSTATE_FOCUSED;

        if ((p->m_uButtonState & ZSTATE_PUSHED) != 0) {
            if (p->m_ResPushed) {
                img = p->m_ResPushed->p;
                ZuiDrawImageEx(cp, img, rc->left, rc->top, rc->right - rc->left, rc->bottom - rc->top, 255);
            }
            else if (p->m_ResNormal) {
                img = p->m_ResNormal->p;
                ZuiDrawImageEx(cp, img, rc->left, rc->top, rc->right - rc->left, rc->bottom - rc->top, 255);
            }
            else
                ZuiDrawFillRoundRect(cp, p->m_cColorPushed, rc, 0);
        }
        else if ((p->m_uButtonState & ZSTATE_HOT) != 0) {
            if (p->m_ResHot) {
                img = p->m_ResHot->p;
                ZuiDrawImageEx(cp, img, rc->left, rc->top, rc->right, rc->bottom, 255);
            }
            else if (p->m_ResNormal) {
                img = p->m_ResNormal->p;
                ZuiDrawImageEx(cp, img, rc->left, rc->top, rc->right, rc->bottom, 255);
            }
            else
                ZuiDrawFillRoundRect(cp, p->m_cColorHot, rc, 0);
        }
        else if ((p->m_uButtonState & ZSTATE_FOCUSED) != 0) {
            if (p->m_ResFocused) {
                img = p->m_ResFocused->p;
                ZuiDrawImageEx(cp, img, rc->left, rc->top, rc->right, rc->bottom, 255);
            }
            else if (p->m_ResNormal) {
                img = p->m_ResNormal->p;
                ZuiDrawImageEx(cp, img, rc->left, rc->top, rc->right, rc->bottom, 255);
            }
            else
                ZuiDrawFillRoundRect(cp, p->m_cColorFocused, rc, 0);
        }
        else {
            if (p->m_ResNormal) {
                img = p->m_ResNormal->p;
                ZuiDrawImageEx(cp, img, rc->left, rc->top, rc->right, rc->bottom, 255);
            }
            else
                ZuiDrawFillRoundRect(cp, p->m_cColorNormal, rc, 0);
        }

        ZRect *rcThumb = ZCCALL(ZM_ListHeaderItem_GetThumbRect, cp, NULL, NULL);
        if (p->m_ResSep) {
            rc = rcThumb;
            img = p->m_ResSep->p;
            ZuiDrawImageEx(cp, img, rc->left, rc->top, rc->right, rc->bottom, 255);
        }
        else {
            rc = rcThumb;
            ZuiDrawFillRoundRect(cp, p->m_cColorSep, rc, 0);
        }
        return 0;
    }
    case ZM_ListHeaderItem_GetThumbRect: {
        if (p->m_iSepWidth >= 0) {
            p->m_rcThumb.left = cp->m_rcItem.right - p->m_iSepWidth;
            p->m_rcThumb.top = cp->m_rcItem.top;
            p->m_rcThumb.right = cp->m_rcItem.right;
            p->m_rcThumb.bottom = cp->m_rcItem.bottom;
        }
        else {
            p->m_rcThumb.left = cp->m_rcItem.left;
            p->m_rcThumb.top = cp->m_rcItem.top;
            p->m_rcThumb.right = cp->m_rcItem.left - p->m_iSepWidth;
            p->m_rcThumb.bottom = cp->m_rcItem.bottom;
        }

        return &p->m_rcThumb;
    }
    case ZM_ListHeaderItem_EstimateSize: {
        //调整预设大小为字体高度
        break;
    }
    case ZM_OnCreate: {
        p = (ZuiListHeaderItem)malloc(sizeof(ZListHeaderItem));
        memset(p, 0, sizeof(ZListHeaderItem));
        //保存原来的回调地址,创建成功后回调地址指向当前函数
        //创建继承的控件 保存数据指针
        //p->old_udata = ZuiLayoutProc(ZM_OnCreate, cp, 0, 0, 0);
        p->old_call = cp->call;
        //初始参数
        p->m_bDragable = TRUE;
        p->m_iSepWidth = 4;
        p->ptLastMouse.x = p->ptLastMouse.y = 0;
        p->m_uTextStyle = ZDT_VCENTER | ZDT_SINGLELINE;
        p->m_cTextColor = 0xFFd8d8d8;
        ZRect rctmp = { 2,1,2,1 };
        p->m_rcPadding = rctmp;
        return p;
    }
    case ZM_OnDestroy: {
        ZCtlProc old_call = p->old_call;
        ZVoid old_udata = p->old_udata;
        old_call(ProcId, cp, old_udata, Param1, Param2);
        if (p->m_ResNormal)
            ZuiResDBDelRes(p->m_ResNormal);
        if (p->m_ResHot)
            ZuiResDBDelRes(p->m_ResHot);
        if (p->m_ResPushed)
            ZuiResDBDelRes(p->m_ResPushed);
        if (p->m_ResFocused)
            ZuiResDBDelRes(p->m_ResFocused);
        if (p->m_ResSep)
            ZuiResDBDelRes(p->m_ResSep);
        if (p->m_rFont)
            ZuiResDBDelRes(p->m_rFont);
        if (p->m_rListFont)
            ZuiResDBDelRes(p->m_rListFont);
        free(p);

        return 0;
    }
    case ZM_GetObject: {
        if (_tcsicmp(Param1, (ZPARAM)ZC_ListHeaderItem) == 0)
            return (ZPARAM)p;
        break;
    }
    case ZM_SetAttribute: {
        ZuiAttribute zAttr = (ZuiAttribute)Param1;
        if (_tcsicmp(zAttr->name, _T("dragable")) == 0)
            ZCCALL(ZM_ListHeaderItem_SetDragable, cp, (ZPARAM)(_tcsicmp(zAttr->value, _T("true")) == 0 ? TRUE : FALSE), Param2);
        else if (_tcsicmp(zAttr->name, _T("sepwidth")) == 0)
            ZCCALL(ZM_ListHeaderItem_SetSepWidth, cp, (ZPARAM)_ttoi(zAttr->value), Param2);
        else if (_tcsicmp(zAttr->name, _T("normalimage")) == 0)
            ZCCALL(ZM_ListHeaderItem_SetNormalImage, cp, ZuiResDBGetRes(zAttr->value, ZREST_IMG), Param2);
        else if (_tcsicmp(zAttr->name, _T("hotimage")) == 0)
            ZCCALL(ZM_ListHeaderItem_SetHotImage, cp, ZuiResDBGetRes(zAttr->value, ZREST_IMG), Param2);
        else if (_tcsicmp(zAttr->name, _T("pushedimage")) == 0)
            ZCCALL(ZM_ListHeaderItem_SetPushedImage, cp, ZuiResDBGetRes(zAttr->value, ZREST_IMG), Param2);
        else if (_tcsicmp(zAttr->name, _T("focusedimage")) == 0)
            ZCCALL(ZM_ListHeaderItem_SetFocusedImage, cp, ZuiResDBGetRes(zAttr->value, ZREST_IMG), Param2);
        else if (_tcsicmp(zAttr->name, _T("sepimage")) == 0)
            ZCCALL(ZM_ListHeaderItem_SetSepImage, cp, ZuiResDBGetRes(zAttr->value, ZREST_IMG), Param2);
        else if (_tcsicmp(zAttr->name, _T("normalcolor")) == 0)
            p->m_cColorNormal = ZuiStr2Color(zAttr->value);
        else if (_tcsicmp(zAttr->name, _T("hotcolor")) == 0)
            p->m_cColorHot = ZuiStr2Color(zAttr->value);
        else if (_tcsicmp(zAttr->name, _T("pushedcolor")) == 0)
            p->m_cColorPushed = ZuiStr2Color(zAttr->value);
        else if (_tcsicmp(zAttr->name, _T("focusedcolor")) == 0)
            p->m_cColorFocused = ZuiStr2Color(zAttr->value);
        else if (_tcsicmp(zAttr->name, _T("sepcolor")) == 0)
            p->m_cColorSep = ZuiStr2Color(zAttr->value);
        else if (_tcsicmp(zAttr->name, _T("font")) == 0) {
            ZCCALL(ZM_ListHeaderItem_SetFont, cp, ZuiResDBGetRes(zAttr->value, ZREST_FONT), Param2);
        }
        else if (_tcsicmp(zAttr->name, _T("listfont")) == 0) {
            ZCCALL(ZM_ListHeaderItem_SetListFont, cp, ZuiResDBGetRes(zAttr->value, ZREST_FONT), Param2);
        }
        else if (_tcsicmp(zAttr->name, _T("align")) == 0) {
            //横向对齐方式
            unsigned int tstyle = (unsigned int)ZCCALL(ZM_ListHeaderItem_GetTextStyle, cp, 0, 0);
            if (_tcsicmp(zAttr->value, _T("left")) == 0) {
                tstyle &= ~(ZDT_CENTER | ZDT_RIGHT);
                tstyle |= ZDT_LEFT;
            }
            if (_tcsicmp(zAttr->value, _T("center")) == 0) {
                tstyle &= ~(ZDT_LEFT | ZDT_RIGHT | ZDT_WORDBREAK | ZDT_EDITCONTROL);
                tstyle |= ZDT_CENTER;
            }
            if (_tcsicmp(zAttr->value, _T("right")) == 0) {
                tstyle &= ~(ZDT_LEFT | ZDT_CENTER | ZDT_WORDBREAK | ZDT_EDITCONTROL);
                tstyle |= ZDT_RIGHT;
            }
            ZCCALL(ZM_ListHeaderItem_SetTextStyle, cp, (ZPARAM)tstyle, Param2);
        }
        else if (_tcsicmp(zAttr->name, _T("valign")) == 0) {
            //纵向对齐方式
            unsigned int tstyle = (unsigned int)ZCCALL(ZM_ListHeaderItem_GetTextStyle, cp, 0, 0);
            if (_tcsicmp(zAttr->value, _T("top")) == 0) {
                tstyle &= ~(ZDT_BOTTOM | ZDT_VCENTER);
                tstyle |= (ZDT_TOP | ZDT_SINGLELINE);
            }
            if (_tcsicmp(zAttr->value, _T("vcenter")) == 0) {
                tstyle &= ~(ZDT_TOP | ZDT_BOTTOM | ZDT_WORDBREAK | ZDT_EDITCONTROL);
                tstyle |= (ZDT_VCENTER | ZDT_SINGLELINE);
            }
            if (_tcsicmp(zAttr->value, _T("bottom")) == 0) {
                tstyle &= ~(ZDT_TOP | ZDT_VCENTER | ZDT_WORDBREAK | ZDT_EDITCONTROL);
                tstyle |= (ZDT_BOTTOM | ZDT_SINGLELINE);
            }
            ZCCALL(ZM_ListHeaderItem_SetTextStyle, cp, (ZPARAM)tstyle, Param2);
        }
        else if (_tcsicmp(zAttr->name, _T("listalign")) == 0) {
            //横向对齐方式
            unsigned int tstyle = (unsigned int)ZCCALL(ZM_ListHeaderItem_GetListTextStyle, cp, 0, 0);
            if (_tcsicmp(zAttr->value, _T("left")) == 0) {
                tstyle &= ~(ZDT_CENTER | ZDT_RIGHT);
                tstyle |= ZDT_LEFT;
            }
            if (_tcsicmp(zAttr->value, _T("center")) == 0) {
                tstyle &= ~(ZDT_LEFT | ZDT_RIGHT | ZDT_WORDBREAK | ZDT_EDITCONTROL);
                tstyle |= ZDT_CENTER;
            }
            if (_tcsicmp(zAttr->value, _T("right")) == 0) {
                tstyle &= ~(ZDT_LEFT | ZDT_CENTER | ZDT_WORDBREAK | ZDT_EDITCONTROL);
                tstyle |= ZDT_RIGHT;
            }
            ZCCALL(ZM_ListHeaderItem_SetListTextStyle, cp, (ZPARAM)tstyle, Param2);
        }
        else if (_tcsicmp(zAttr->name, _T("listvalign")) == 0) {
            //纵向对齐方式
            unsigned int tstyle = (unsigned int)ZCCALL(ZM_ListHeaderItem_GetListTextStyle, cp, 0, 0);
            if (_tcsicmp(zAttr->value, _T("top")) == 0) {
                tstyle &= ~(ZDT_BOTTOM | ZDT_VCENTER);
                tstyle |= (ZDT_TOP | ZDT_SINGLELINE);
            }
            if (_tcsicmp(zAttr->value, _T("vcenter")) == 0) {
                tstyle &= ~(ZDT_TOP | ZDT_BOTTOM | ZDT_WORDBREAK | ZDT_EDITCONTROL);
                tstyle |= (ZDT_VCENTER | ZDT_SINGLELINE);
            }
            if (_tcsicmp(zAttr->value, _T("bottom")) == 0) {
                tstyle &= ~(ZDT_TOP | ZDT_VCENTER | ZDT_WORDBREAK | ZDT_EDITCONTROL);
                tstyle |= (ZDT_BOTTOM | ZDT_SINGLELINE);
            }
            ZCCALL(ZM_ListHeaderItem_SetListTextStyle, cp, (ZPARAM)tstyle, Param2);
        }
        else if (_tcsicmp(zAttr->name, _T("textcolor")) == 0) {
            //字体颜色
            ZuiColor clrColor = ZuiStr2Color(zAttr->value);
            ZCCALL(ZM_ListHeaderItem_SetTextColor, cp, (ZPARAM)clrColor, Param2);
        }
        else if (_tcsicmp(zAttr->name, _T("textpadding")) == 0) {
            //字体边距
            ZRect rcPadding = { 0 };
            ZuiText pstr = NULL;
            rcPadding.left = _tcstol(zAttr->value, &pstr, 10);  ASSERT(pstr);
            rcPadding.top = _tcstol(pstr + 1, &pstr, 10);    ASSERT(pstr);
            rcPadding.right = _tcstol(pstr + 1, &pstr, 10);  ASSERT(pstr);
            rcPadding.bottom = _tcstol(pstr + 1, &pstr, 10); ASSERT(pstr);
            ZCCALL(ZM_ListHeaderItem_SetTextPadding, cp, &rcPadding, Param2);
        }
        break;
    }
     case ZM_ListHeaderItem_SetTextPadding: {
         memcpy(&p->m_rcPadding, Param1, sizeof(ZRect));
         if (!Param2)
             ZuiControlNeedUpdate(cp);
         return 0;
     }
    case ZM_ListHeaderItem_SetTextColor: {
        p->m_cTextColor = (ZuiColor)Param1;
        if (!Param2)
            ZuiControlInvalidate(cp, TRUE);
        return 0;
    }
    case ZM_ListHeaderItem_SetFont: {
        if (p->m_rFont)
            ZuiResDBDelRes(p->m_rFont);
        p->m_rFont = (ZuiRes)Param1;
        if (!Param2)
            ZuiControlInvalidate(cp, TRUE);
    }
    case ZM_ListHeaderItem_SetListFont: {
        if (p->m_rListFont)
            ZuiResDBDelRes(p->m_rListFont);
        p->m_rListFont = (ZuiRes)Param1;
        if (!Param2 && cp->m_pParent)
            ZuiControlInvalidate(cp->m_pParent->m_pParent, TRUE);
    }
    case ZM_ListHeaderItem_SetListTextStyle: {
        p->m_uListTextStyle = (unsigned int)Param1;
        if (!Param2 && cp->m_pParent)
            ZuiControlInvalidate(cp->m_pParent->m_pParent, TRUE);
        return 0;
    }
    case ZM_ListHeaderItem_GetListTextStyle: {
        return (ZPARAM)p->m_uListTextStyle;
    }
    case ZM_ListHeaderItem_SetTextStyle: {
         p->m_uTextStyle = (unsigned int)Param1;
         if (!Param2)
             ZuiControlInvalidate(cp, TRUE);
        return 0;
    }
    case ZM_ListHeaderItem_GetTextStyle: {
        return (ZPARAM)p->m_uTextStyle;
    }
    case ZM_ListHeaderItem_SetDragable: {
        p->m_bDragable = (ZuiBool)Param1;
        if (!Param1) p->m_uButtonState &= ~ZSTATE_CAPTURED;
        return 0;
    }
    case ZM_ListHeaderItem_SetSepWidth: {
        p->m_iSepWidth = (int)Param1;
        return 0;
    }
    case ZM_ListHeaderItem_SetNormalImage: {
        if (p->m_ResNormal)
            ZuiResDBDelRes(p->m_ResNormal);
        p->m_ResNormal = Param1;
        break;
    }
    case ZM_ListHeaderItem_SetHotImage: {
        if (p->m_ResHot)
            ZuiResDBDelRes(p->m_ResHot);
        p->m_ResHot = Param1;
        break;
    }
    case ZM_ListHeaderItem_SetPushedImage: {
        if (p->m_ResPushed)
            ZuiResDBDelRes(p->m_ResPushed);
        p->m_ResPushed = Param1;
        break;
    }
    case ZM_ListHeaderItem_SetFocusedImage: {
        if (p->m_ResFocused)
            ZuiResDBDelRes(p->m_ResFocused);
        p->m_ResFocused = Param1;
        break;
    }
    case ZM_ListHeaderItem_SetSepImage: {
        if (p->m_ResSep)
            ZuiResDBDelRes(p->m_ResSep);
        p->m_ResSep = Param1;
        break;
    }
    case ZM_GetType:
        return (ZPARAM)ZC_ListHeaderItem;
    case ZM_CoreInit:
        return (ZPARAM)TRUE;
    case ZM_CoreUnInit:
        return (ZPARAM)NULL;
    default:
        break;
    }
    return p->old_call(ProcId, cp, p->old_udata, Param1, Param2);
}

