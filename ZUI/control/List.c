#include "List.h"
#include <core/control.h>
#include <core/resdb.h>
#include <core/function.h>
#include <layout/Layout.h>
#include <layout/VerticalLayout.h>
#include <layout/HorizontalLayout.h>
#include <control/Register.h>
#include <control/Label.h>
ZEXPORT ZuiAny ZCALL ZuiListProc(int ProcId, ZuiControl cp, ZuiList p, ZuiAny Param1, ZuiAny Param2) {
    switch (ProcId)
    {
    case ZM_SetPos: {
        //通知父容器调整布局
        ZuiVerticalLayoutProc(ProcId, cp, p->old_udata, Param1, Param2);
        if (p->m_pHeader == NULL) return 0;
        //计算列数量
        p->m_ListInfo.nColumns = (INT)MIN(ZCCALL(ZM_Layout_GetCount, p->m_pHeader, NULL, NULL), (ZuiAny)ZLIST_MAX_COLUMNS);

        if (!p->m_pHeader->m_bVisible) {

        }

        for (int i = 0; i < p->m_ListInfo.nColumns; i++) {
            ZuiControl pControl = ZCCALL(ZM_Layout_GetItemAt, p->m_pHeader, (ZuiAny)i, NULL);
            if (!pControl->m_bVisible) continue;
            if (pControl->m_bFloat) continue;
            p->m_ListInfo.rcColumn[i] = *(ZRect *)ZCCALL(ZM_GetPos, pControl, NULL, NULL);
        }
        if (!p->m_pHeader->m_bVisible) {

        }
        return 0;
    }
    case ZM_SetAttribute: {
        if (_tcsicmp(Param1, _T("header")) == 0)
            ZCCALL(ZM_SetVisible, p->m_pHeader, (ZuiAny)(_tcsicmp(Param2, _T("hidden")) == 0 ? FALSE : TRUE), NULL);
        else if (_tcsicmp(Param1, _T("linecolor")) == 0) {
            p->m_ListInfo.dwLineColor = ZuiStr2Color(Param2);
            p->m_ListInfo.bShowRowLine = TRUE;
        }
        else if (_tcsicmp(Param1, _T("columncolor")) == 0) {
            p->m_ListInfo.dwColumnColor = ZuiStr2Color(Param2);
            p->m_ListInfo.bShowColumnLine = TRUE;
        }
        else if (_tcsicmp(Param1, _T("hotbkcolor")) == 0) {
            p->m_ListInfo.dwHotBkColor = ZuiStr2Color(Param2);
        }
        else if (_tcsicmp(Param1, _T("selectedbkcolor")) == 0) {
            p->m_ListInfo.dwSelectedBkColor = ZuiStr2Color(Param2);
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
        if (!_tcsicmp(ZCCALL(ZM_GetType, Param1, NULL, NULL) ,(ZuiAny)ZC_ListHeader)) {
            if (p->m_pHeader != Param1 && ZCCALL(ZM_Layout_GetCount, p->m_pHeader, NULL, NULL) == 0) {
                //删除原来的头元素
                FreeZuiControl(p->m_pHeader, FALSE);
                p->m_pHeader = Param1;
                if (cp->m_pOs != NULL)
                    ZCCALL(ZM_SetOs, (ZuiControl)p->m_pHeader, cp, (ZuiAny)TRUE);
                p->m_ListInfo.nColumns = MIN((int)ZCCALL(ZM_Layout_GetCount, p->m_pHeader, NULL,  NULL), ZLIST_MAX_COLUMNS);
                return 0;
            }
            return FALSE;
        }
        if (!_tcsicmp(ZCCALL(ZM_GetType, Param1, NULL, NULL), (ZuiAny)ZC_ListBody)) {
            if (p->m_pList != Param1 && ZCCALL(ZM_Layout_GetCount, p->m_pList, NULL, NULL) == 0) {
                //删除原来的表体
                FreeZuiControl(p->m_pList, FALSE);
                p->m_pList = Param1;
                if (cp->m_pOs != NULL)
                    ZCCALL(ZM_SetOs, (ZuiControl)p->m_pList, cp, (ZuiAny)TRUE);
                ZuiVerticalLayoutProc(ZM_Layout_Add, cp, p->old_udata, p->m_pList, NULL);
                ZCCALL(ZM_ListBody_SetOwner, p->m_pList, cp, NULL);
                return 0;
            }
            return FALSE;
        }
        //判断元素是否是表头元素
        if (!_tcsicmp(ZCCALL(ZM_GetType, Param1, NULL, NULL), (ZuiAny)ZC_ListHeaderItem)) {
            //插入到头容器
            ZuiBool ret = (ZuiBool)ZCCALL(ZM_Layout_Add, p->m_pHeader, Param1, NULL);
            //计算列数量
            p->m_ListInfo.nColumns = MIN((int)ZCCALL(ZM_Layout_GetCount, p->m_pHeader, NULL, NULL), ZLIST_MAX_COLUMNS);
            return (ZuiAny)ret;
        }
        // 插入的元素是行数据
        if (ZCCALL(ZM_GetObject, Param1, (ZuiAny)ZC_ListElement, NULL)) {
            return ZCCALL(ZM_Layout_Add, p->m_pList, Param1, NULL);
        }

        return (ZuiAny)TRUE;
    }
    case ZM_List_AddAt: {
        //判断添加的元素是否是表头控件
        if (!_tcsicmp(ZCCALL(ZM_GetType, Param1, NULL, NULL), (ZuiAny)ZC_ListHeader)) {
            if (p->m_pHeader != Param1 && ZCCALL(ZM_Layout_GetCount, p->m_pHeader, NULL, NULL) == 0) {
                //删除原来的头元素
                FreeZuiControl(p->m_pHeader, FALSE);
                p->m_pHeader = Param1;
                if (cp->m_pOs != NULL)
                    ZCCALL(ZM_SetOs, (ZuiControl)p->m_pHeader, cp, (void*)TRUE);
                p->m_ListInfo.nColumns = MIN((int)ZCCALL(ZM_Layout_GetCount, p->m_pHeader, NULL, NULL), ZLIST_MAX_COLUMNS);
                //表头永远在第一个位置
                return 0;
            }
            return FALSE;
        }
        //判断元素是否是表头元素
        if (!_tcsicmp(ZCCALL(ZM_GetType, Param1, NULL, NULL), (ZuiAny)ZC_ListHeaderItem)) {
            //插入到头容器指定位置
            ZuiBool ret = (ZuiBool)ZCCALL(ZM_Layout_AddAt, p->m_pHeader, Param1, Param2);
            //计算列数量
            p->m_ListInfo.nColumns = MIN((int)ZCCALL(ZM_Layout_GetCount, p->m_pHeader, NULL, NULL), ZLIST_MAX_COLUMNS);
            return (ZuiAny)ret;
        }
        // 插入的元素是行数据
        if (ZCCALL(ZM_GetObject, Param1, (ZuiAny)ZC_ListElement, NULL)) {
            return ZCCALL(ZM_Layout_Add, p->m_pList, Param1, Param2);
        }
        return (ZuiAny)TRUE;
    }
    case ZM_List_Remove: {
        //判断删除的元素是否是表头控件
        //判断删除的元素是否是表头元素
        if (!_tcsicmp(ZCCALL(ZM_GetType, Param1, NULL, NULL), (ZuiAny)ZC_ListHeaderItem))
            return ZCCALL(ZM_Layout_Remove, p->m_pHeader, Param1, Param2);
        //判断删除的元素是否是表体控件
        // 删除的元素是行数据
        int iIndex = -1;
        if (!_tcsicmp(ZCCALL(ZM_GetType, Param1, NULL, NULL), (ZuiAny)ZC_ListElement)) {
            iIndex = (int)ZCCALL(ZM_Layout_GetItemIndex, p->m_pList, Param1, 0);
            if (iIndex == -1) return FALSE;
            return ZCCALL(ZM_Layout_RemoveAt, p->m_pList, Param1, (ZuiAny)iIndex);
        }
        break;
    }
    case ZM_List_RemoveAt: {
        //此调用只删除表数据,不影响表头
        if (!ZCCALL(ZM_Layout_RemoveAt, p->m_pList, Param1, Param2))
            return FALSE;

        return (ZuiAny)TRUE;
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
        if (!ZCCALL(ZM_ListElement_Select, pControl, (ZuiAny)TRUE, NULL)) {
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

        return (ZuiAny)TRUE;
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
        return (ZuiAny)TRUE;
    }
    case ZM_List_SelectAllItems: {

    }
    case ZM_List_UnSelectAllItems: {
        for (int i = 0; i < p->m_aSelItems->count; ++i) {
            int iSelIndex = (int)darray_getat(p->m_aSelItems, i);
            ZuiControl pControl = ZCCALL(ZM_List_GetItemAt, cp, (ZuiAny)iSelIndex,NULL);;
            if (pControl == NULL) continue;
            if (!pControl->m_bEnabled) continue;
            if (!ZCCALL(ZM_ListElement_SelectMulti, pControl, FALSE, NULL)) continue;
        }
        darray_empty(p->m_aSelItems);
        p->m_iCurSel = -1;
        return (ZuiAny)TRUE;
    }
    //直接转发到列表体的消息
    case ZM_Layout_LineUp:
    case ZM_Layout_LineDown:
    case ZM_Layout_PageUp:
    case ZM_Layout_PageDown:
    case ZM_Layout_HomeUp:
    case ZM_Layout_EndDown:
    case ZM_Layout_LineLeft:
    case ZM_Layout_LineRight:
    case ZM_Layout_PageLeft:
    case ZM_Layout_PageRight:
    case ZM_Layout_HomeLeft:
    case ZM_Layout_EndRight:
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

        //创建表头
        p->m_pHeader = NewZuiControl(_T("ListHeader"), NULL, NULL);
        ZuiVerticalLayoutProc(ZM_Layout_Add, cp, p->old_udata, p->m_pHeader, NULL);
        ZCCALL(ZM_SetBkColor, p->m_pHeader, (ZuiAny)BK_Color, (ZuiAny)ARGB(255, 111, 222, 200));
        //创建表主体
        p->m_pList = NewZuiControl(_T("ListBody"), NULL, NULL);
        ZuiVerticalLayoutProc(ZM_Layout_Add, cp, p->old_udata, p->m_pList, NULL);
        ZCCALL(ZM_ListBody_SetOwner, p->m_pList, cp, NULL);

        p->m_ListInfo.dwLineColor = 0xFF686868;
        p->m_ListInfo.dwHotBkColor = 0xFF888888;
        p->m_ListInfo.dwSelectedBkColor = 0xFFa8a8a8;
        return p;
    }
    case ZM_OnDestroy: {
        ZCtlProc old_call = p->old_call;
        ZuiAny old_udata = p->old_udata;
        if (p->m_pHeader) {
            ZCCALL(ProcId, p->m_pHeader, (ZuiAny)FALSE, Param2);
        }
        old_call(ProcId, cp, old_udata, Param1, Param2);

        free(p);
        return 0;
    }
    case ZM_GetObject: {
        if (_tcsicmp(Param1, (ZuiAny)ZC_List) == 0)
            return (ZuiAny)p;
        break;
    }
    case ZM_GetType:
        return (ZuiAny)ZC_List;
    case ZM_CoreInit:
        //将辅助控件注册到系统
        ZuiControlRegisterAdd(ZC_ListBody, (ZCtlProc)&ZuiListBodyProc);
        ZuiControlRegisterAdd(ZC_ListHeader, (ZCtlProc)&ZuiListHeaderProc);
        return (ZuiAny)TRUE;
    case ZM_CoreUnInit:
        return (ZuiAny)TRUE;
    default:
        break;
    }
    return p->old_call(ProcId, cp, p->old_udata, Param1, Param2);
}
ZEXPORT ZuiAny ZCALL ZuiListBodyProc(int ProcId, ZuiControl cp, ZuiListBody p, ZuiAny Param1, ZuiAny Param2) {
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
            ZuiControl pControl = ZCCALL(ZM_Layout_GetItemAt, cp, (ZuiAny)index, NULL);
            if (pControl)
                ZCCALL(ZM_ListElement_SetIndex, pControl, (ZuiAny)index, NULL);
        }
        return 0;
    }
    case ZM_Layout_RemoveAt:
    {
        if (!ZuiVerticalLayoutProc(ZM_Layout_RemoveAt, cp, p->old_udata, Param1, Param2))
            return FALSE;
        int j = (int)ZCCALL(ZM_List_GetCount, cp, NULL, NULL);
        for (int index = (int)Param2; index < j; index++) {
            ZuiControl pControl = ZCCALL(ZM_Layout_GetItemAt, cp, (ZuiAny)index, NULL);
            if (pControl)
                ZCCALL(ZM_ListElement_SetIndex, pControl, (ZuiAny)index, NULL);
        }
        return 0;
    }
    case ZM_OnPaint: {
        p->old_call(ProcId, cp, p->old_udata, Param1, Param2);
        ZuiRect rrc = &cp->m_rcItem;
        ZuiRect rc = Param2;
        ZuiLayout op = ZCCALL(ZM_GetObject, cp, (ZuiAny)ZC_Layout, NULL);
        if (op->m_pVerticalScrollBar && op->m_pVerticalScrollBar->m_bVisible) {
            int sw = (int)ZCCALL(ZM_GetFixedWidth, op->m_pVerticalScrollBar, NULL, NULL);
            if (rc->right > (rrc->right - sw)) {
                rc->right -= sw;
            }
        }
        ZuiControl pHeader = ((ZuiList)ZCCALL(ZM_GetObject, p->m_pOwner, (ZuiAny)ZC_List, NULL))->m_pHeader;
        if (pHeader->m_bVisible) {
            ZCCALL(ProcId, pHeader, Param1, rc);
        }
        return 0;
    }
    case ZM_FindControl: {
        ZuiControl pResult = NULL;
        pResult = ZCCALL(ProcId, ((ZuiList)ZCCALL(ZM_GetObject, p->m_pOwner, (ZuiAny)ZC_List, NULL))->m_pHeader, Param1, Param2);
        if (pResult)
            return pResult;
        return p->old_call(ProcId, cp, p->old_udata, Param1, Param2);
    }
    case ZM_SetPos: {
        ZuiDefaultControlProc(ProcId, cp, 0, Param1, Param2);
        ZRect rc = *(ZRect*)Param1;
        ZuiLayout op = ZCCALL(ZM_GetObject, cp, (ZuiAny)ZC_Layout, NULL);
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
                    ZCCALL(ZM_SetPos, pHeader, &rcHeader, FALSE);
                }
                else {
                    rcHeader.left = rc.left; rcHeader.top = rc.top; rcHeader.right = rc.right; rcHeader.bottom = rc.top + ph;
                    ZCCALL(ZM_SetPos, pHeader, &rcHeader, FALSE);
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
                ZCCALL(ZM_Layout_SetFloatPos, cp, (ZuiAny)it2, 0);
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
            ZCCALL(ZM_SetPos, pControl, &rcCtrl, FALSE);

            iPosY += sz.cy + op->m_iChildPadding + rcPadding->top + rcPadding->bottom;
            cyNeeded += sz.cy + rcPadding->top + rcPadding->bottom;
            szRemaining.cy -= sz.cy + op->m_iChildPadding + rcPadding->bottom;
        }
        cyNeeded += (nEstimateNum - 1) * op->m_iChildPadding;
        // Process the scrollbar
        rc.top -= ph; cyNeeded += ph;
        ZCCALL(ZM_Layout_ProcessScrollBar, cp, (ZuiAny)&rc, (ZuiAny)(MAKEPARAM(cxNeeded, cyNeeded)));
        return 0;
    }
    case ZM_ListBody_SetScrollPos: {
        int cx = 0;
        int cy = 0;
        SIZE *szPos = Param1;
        ZuiLayout op = ZCCALL(ZM_GetObject, cp, (ZuiAny)ZC_Layout, NULL);

        if (op->m_pVerticalScrollBar && op->m_pVerticalScrollBar->m_bVisible) {
            int iLastScrollPos = (int)ZCCALL(ZM_ScrollBar_GetScrollPos, op->m_pVerticalScrollBar, NULL, NULL);
            ZCCALL(ZM_ScrollBar_SetScrollPos, op->m_pVerticalScrollBar, (ZuiAny)szPos->cy, NULL);
            cy = (int)ZCCALL(ZM_ScrollBar_GetScrollPos, op->m_pVerticalScrollBar, NULL, NULL) - iLastScrollPos;
        }

        if (op->m_pHorizontalScrollBar && op->m_pHorizontalScrollBar->m_bVisible) {
            int iLastScrollPos = (int)ZCCALL(ZM_ScrollBar_GetScrollPos, op->m_pHorizontalScrollBar, NULL, NULL);
            ZCCALL(ZM_ScrollBar_SetScrollPos, op->m_pHorizontalScrollBar, (ZuiAny)szPos->cx, NULL);
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
            pInfo->nColumns = MIN((int)ZCCALL(ZM_Layout_GetCount, pHeader, NULL, NULL), ZLIST_MAX_COLUMNS);
            for (int i = 0; i < pInfo->nColumns; i++) {
                ZuiControl pControl = ZCCALL(ZM_Layout_GetItemAt, pHeader, (ZuiAny)i, NULL);
                if (!pControl->m_bVisible) continue;
                if (pControl->m_bFloat) continue;

                rcPos = *(ZRect *)ZCCALL(ZM_GetPos, pControl, NULL, NULL);
                rcPos.left -= cx;
                rcPos.right -= cx;
                ZCCALL(ZM_SetPos, pControl, &rcPos, (ZuiAny)TRUE);
                pInfo->rcColumn[i] = *(ZRect *)ZCCALL(ZM_GetPos, pControl, NULL, NULL);
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
                ZCCALL(ZM_SetPos, pControl, &rcPos, (ZuiAny)TRUE);
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
        ZuiAny old_udata = p->old_udata;
        old_call(ProcId, cp, old_udata, Param1, Param2);
        free(p);

        return 0;
    }
    case ZM_GetObject: {
        if (_tcsicmp(Param1, (ZuiAny)ZC_ListBody) == 0)
            return (ZuiAny)p;
        break;
    }
    case ZM_GetType:
        return (ZuiAny)ZC_ListBody;
    case ZM_CoreInit:
        //将辅助控件注册到系统
        ZuiControlRegisterAdd(ZC_ListElement, (ZCtlProc)&ZuiListElementProc);
        return (ZuiAny)TRUE;
    case ZM_CoreUnInit:
        return (ZuiAny)NULL;
    default:
        break;
    }
    return p->old_call(ProcId, cp, p->old_udata, Param1, Param2);
}
ZEXPORT ZuiAny ZCALL ZuiListElementProc(int ProcId, ZuiControl cp, ZuiListElement p, ZuiAny Param1, ZuiAny Param2) {
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
        ZuiLayout op = ZCCALL(ZM_GetObject, cp, (ZuiAny)ZC_Layout, NULL);
        int i = darray_find(op->m_items, Param1);

        if (i >= 0) {
            ZuiControl pHeaderItem = ZCCALL(ZM_Layout_GetItemAt, pHeader, (ZuiAny)i, NULL);
            if (pHeaderItem == NULL)
                break;

            ZuiListHeaderItem lhi = ZCCALL(ZM_GetObject, pHeaderItem, (ZuiAny)ZC_ListHeaderItem, NULL);
            ZuiLabel lop = ZCCALL(ZM_GetObject, op->m_items->data[i], (ZuiAny)ZC_Label, NULL);
            lop->m_uTextStyle |= lhi->m_uListTextStyle;
            lop->m_cTextColor = lhi->m_cTextColor;
            lop->m_rFont = lhi->m_rListFont;

        }
        return (ZuiAny)TRUE;
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
                    ZCCALL(ZM_ListElement_Select, cp, (ZuiAny)TRUE, NULL);
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
            ZuiLayout pParentContainer = ZCCALL(ZM_GetObject, cp->m_pParent, (ZuiAny)ZC_Layout, NULL);
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
        ZuiLayout op = ZCCALL(ZM_GetObject, cp, (ZuiAny)ZC_Layout, NULL);
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
            ZuiControl pHeaderItem = ZCCALL(ZM_Layout_GetItemAt, pHeader, (ZuiAny)i, NULL);
            if (pHeaderItem == NULL)
                return 0;
            ZRect *rcHeaderItem = ZCCALL(ZM_GetPos, pHeaderItem, NULL, NULL);
            if (pListItem != NULL && !(rcHeaderItem->left == 0 && rcHeaderItem->right == 0))
            {
                ZRect rt = *(ZRect *)ZCCALL(ZM_GetPos, pListItem, NULL, NULL);
                rt.left = rcHeaderItem->left;
                rt.right = rcHeaderItem->right;

                ZCCALL(ZM_SetPos, pListItem, &rt, FALSE);
            }
        }
        ZuiControl tmpHeaderItem = ZCCALL(ZM_Layout_GetItemAt, pHeader, (ZuiAny)((int)ZCCALL(ZM_Layout_GetCount, pHeader, NULL, NULL) - 1), NULL);
        if (tmpHeaderItem) {
            cp->m_rcItem.right = tmpHeaderItem->m_rcItem.right;
        }
        return 0;
    }
    case ZM_OnPaintBkColor: {
        ZuiGraphics gp = (ZuiGraphics)Param1;
        ZRect *rc = (ZRect*)&cp->m_rcItem;
        if (p->m_pOwner == NULL)
            return 0;
        ZuiListInfo pInfo = ZCCALL(ZM_List_GetListInfo, p->m_pOwner, cp, NULL);



        DWORD iBackColor = 0;
        if (!pInfo->bAlternateBk || p->m_iIndex % 2 == 0)
            iBackColor = pInfo->dwBkColor;

        if ((p->m_uButtonState & ZSTATE_HOT) != 0) {
            iBackColor = pInfo->dwHotBkColor;
        }
        if (p->m_bSelected) {
            iBackColor = pInfo->dwSelectedBkColor;
        }
        if (!cp->m_bEnabled) {
            iBackColor = pInfo->dwDisabledBkColor;
        }
        if (iBackColor != 0) {
            ZuiDrawFillRect(gp, iBackColor, rc);
        }

        ZRect rcc;
        if (pInfo->dwLineColor != 0) {
            if (pInfo->bShowRowLine) {
                MAKEZRECT(rcc, rc->left, rc->bottom - 1, rc->right, rc->bottom - 1);
                ZuiDrawLine(gp, pInfo->dwLineColor, &rcc, 1);
            }
        }
        if (pInfo->dwColumnColor) {
            if (pInfo->bShowColumnLine) {
                for (int i = 0; i < pInfo->nColumns; i++) {
                    MAKEZRECT(rcc, pInfo->rcColumn[i].right - 1, rc->top, pInfo->rcColumn[i].right  - 1, rc->bottom);
                    ZuiDrawLine(gp, pInfo->dwColumnColor, &rcc, 1);
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
        return (ZuiAny)p->m_iIndex;
    }
    case ZM_ListElement_SetIndex: {
        p->m_iIndex = (int)Param1;
        break;
    }
    case ZM_ListElement_Select: {
        if (!cp->m_bEnabled)
            return FALSE;
        if (p->m_pOwner != NULL && p->m_bSelected)
            ZCCALL(ZM_List_UnSelectItem, p->m_pOwner, (ZuiAny)p->m_iIndex, (ZuiAny)TRUE);//如果被选中就先反选,然后重新选取
        if (Param1 == (ZuiAny)p->m_bSelected)
            return (ZuiAny)TRUE;
        p->m_bSelected = (ZuiBool)Param1;
        if (Param1 && p->m_pOwner != NULL)
            ZCCALL(ZM_List_SelectItem, p->m_pOwner, (ZuiAny)p->m_iIndex, FALSE);
        ZuiControlInvalidate(cp, TRUE);

        return (ZuiAny)TRUE;
    }
    case ZM_ListElement_SelectMulti: {
        if (!cp->m_bEnabled)
            return FALSE;
        if (Param1 == (ZuiAny)p->m_bSelected)
            return (ZuiAny)TRUE;

        p->m_bSelected = (ZuiBool)Param1;
        if (Param1 && p->m_pOwner != NULL)
            ZCCALL(ZM_List_SelectMultiItem, p->m_pOwner, (ZuiAny)p->m_iIndex, FALSE);
        ZuiControlInvalidate(cp, TRUE);
        return (ZuiAny)TRUE;
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
        ZuiAny old_udata = p->old_udata;
        old_call(ProcId, cp, old_udata, (ZuiAny)TRUE, Param2); //设置Param1 让子控件不处理字体资源。
        free(p);

        return 0;
    }
    case ZM_GetObject: {
        if (_tcsicmp(Param1, (ZuiAny)ZC_ListElement) == 0)
            return (ZuiAny)p;
        break;
    }
    case ZM_GetType:
        return (ZuiAny)ZC_ListElement;
    case ZM_CoreInit:
        return (ZuiAny)TRUE;
    case ZM_CoreUnInit:
        return (ZuiAny)TRUE;
    default:
        break;
    }
    return p->old_call(ProcId, cp, p->old_udata, Param1, Param2);
}
ZEXPORT ZuiAny ZCALL ZuiListHeaderProc(int ProcId, ZuiControl cp, ZuiListHeader p, ZuiAny Param1, ZuiAny Param2) {
    switch (ProcId)
    {
    case ZM_EstimateSize: {
        ZuiLayout op = ZCCALL(ZM_GetObject, cp, (ZuiAny)ZC_Layout, NULL);
        p->cXY.cy = cp->m_cxyFixed.cy;
        p->cXY.cx = 0;
        if (p->cXY.cy == 0 && cp->m_pOs != NULL) {
            for (int it = 0; it < op->m_items->count; it++) {
                SIZE * psz = (SIZE *)ZCCALL(ZM_EstimateSize, op->m_items->data[it], Param1, 0);
                p->cXY.cy = MAX(p->cXY.cy, psz->cy);
            }
        }

        for (int it = 0; it < op->m_items->count; it++) {
            SIZE * psz = (SIZE *)ZCCALL(ZM_EstimateSize, op->m_items->data[it], Param1, 0);
            p->cXY.cx += psz->cx;
        }

        return &p->cXY;
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
        ZuiAny old_udata = p->old_udata;
        old_call(ProcId, cp, old_udata, Param1, Param2);
        free(p);

        return 0;
    }
    case ZM_GetObject: {
        if (_tcsicmp(Param1, (ZuiAny)ZC_ListHeader) == 0)
            return (ZuiAny)p;
        break;
    }
    case ZM_GetType:
        return (ZuiAny)ZC_ListHeader;
    case ZM_CoreInit:
        //将辅助控件注册到系统
        ZuiControlRegisterAdd(ZC_ListHeaderItem, (ZCtlProc)&ZuiListHeaderItemProc);
        return (ZuiAny)TRUE;
    case ZM_CoreUnInit: {
        return (ZuiAny)TRUE;
    }
    default:
        break;
    }
    return p->old_call(ProcId, cp, p->old_udata, Param1, Param2);
}
ZEXPORT ZuiAny ZCALL ZuiListHeaderItemProc(int ProcId, ZuiControl cp, ZuiListHeaderItem p, ZuiAny Param1, ZuiAny Param2) {
    switch (ProcId)
    {
    case ZM_GetControlFlags: {
        return (ZuiAny)ZFLAG_SETCURSOR;//需要设置鼠标
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

        if (event->Type == ZEVENT_SETFOCUS)
        {
            ZuiControlInvalidate(cp, TRUE);
        }
        if (event->Type == ZEVENT_KILLFOCUS)
        {
            ZuiControlInvalidate(cp, TRUE);
        }
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
                ZuiOsSetCursor((unsigned int)IDC_SIZEWE);
                return 0;
            }
            ZuiOsSetCursor((unsigned int)IDC_ARROW);
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
        ZuiGraphics gp = (ZuiGraphics)Param1;
        ZRect *rc = &cp->m_rcItem;
        ZRect pt;
        pt.left = rc->left + p->m_rcPadding.left;
        pt.top = rc->top + p->m_rcPadding.top;
        pt.right = rc->right - p->m_rcPadding.right - p->m_iSepWidth;
        pt.bottom = rc->bottom - p->m_rcPadding.bottom;
        ZuiDrawString(gp, p->m_rFont ? p->m_rFont->p : Global_Font, cp->m_sText, _tcslen(cp->m_sText), &pt, p->m_cTextColor, p->m_uTextStyle);
        return 0;
    }
    case ZM_OnPaintStatusImage: {
        ZuiGraphics gp = (ZuiGraphics)Param1;
        ZRect *rc = (ZRect *)&cp->m_rcItem;
        ZuiImage img;
        if (cp->m_bFocused)
            p->m_uButtonState |= ZSTATE_FOCUSED;
        else p->m_uButtonState &= ~ZSTATE_FOCUSED;

        if ((p->m_uButtonState & ZSTATE_PUSHED) != 0) {
            if (p->m_diPushed) {
                img = p->m_diPushed->p;
                ZuiDrawImageEx(gp, img, rc->left, rc->top, rc->right - rc->left, rc->bottom - rc->top, 0, 0, 0, 0, 255);
            }
            else if (p->m_diNormal) {
                img = p->m_diNormal->p;
                ZuiDrawImageEx(gp, img, rc->left, rc->top, rc->right - rc->left, rc->bottom - rc->top, 0, 0, 0, 0, 255);
            }
            else
                ZuiDrawFillRect(gp, p->m_ColorPushed, rc);
        }
        else if ((p->m_uButtonState & ZSTATE_HOT) != 0) {
            if (p->m_diHot) {
                img = p->m_diHot->p;
                ZuiDrawImageEx(gp, img, rc->left, rc->top, rc->right, rc->bottom, 0, 0, 0, 0, 255);
            }
            else if (p->m_diNormal) {
                img = p->m_diNormal->p;
                ZuiDrawImageEx(gp, img, rc->left, rc->top, rc->right, rc->bottom, 0, 0, 0, 0, 255);
            }
            else
                ZuiDrawFillRect(gp, p->m_ColorHot, rc);
        }
        else if ((p->m_uButtonState & ZSTATE_FOCUSED) != 0) {
            if (p->m_diFocused) {
                img = p->m_diFocused->p;
                ZuiDrawImageEx(gp, img, rc->left, rc->top, rc->right, rc->bottom, 0, 0, 0, 0, 255);
            }
            else if (p->m_diNormal) {
                img = p->m_diNormal->p;
                ZuiDrawImageEx(gp, img, rc->left, rc->top, rc->right, rc->bottom, 0, 0, 0, 0, 255);
            }
            else
                ZuiDrawFillRect(gp, p->m_ColorFocused, rc);
        }
        else {
            if (p->m_diNormal) {
                img = p->m_diNormal->p;
                ZuiDrawImageEx(gp, img, rc->left, rc->top, rc->right, rc->bottom, 0, 0, 0, 0, 255);
            }
            else
                ZuiDrawFillRect(gp, p->m_ColorNormal, rc);
        }

        ZRect *rcThumb = ZCCALL(ZM_ListHeaderItem_GetThumbRect, cp, NULL, NULL);
        if (p->m_diSep) {
            rc = rcThumb;
            img = p->m_diSep->p;
            ZuiDrawImageEx(gp, img, rc->left, rc->top, rc->right, rc->bottom, 0, 0, 0, 0, 255);
        }
        else {
            rc = rcThumb;
            ZuiDrawFillRect(gp, p->m_ColorSep, rc);
        }
        return 0;
    }
    case ZM_ListHeaderItem_GetThumbRect: {
        if (p->m_iSepWidth >= 0) {
            p->Thumb.left = cp->m_rcItem.right - p->m_iSepWidth;
            p->Thumb.top = cp->m_rcItem.top;
            p->Thumb.right = cp->m_rcItem.right;
            p->Thumb.bottom = cp->m_rcItem.bottom;
        }
        else {
            p->Thumb.left = cp->m_rcItem.left;
            p->Thumb.top = cp->m_rcItem.top;
            p->Thumb.right = cp->m_rcItem.left - p->m_iSepWidth;
            p->Thumb.bottom = cp->m_rcItem.bottom;
        }

        return &p->Thumb;
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
        p->old_udata = ZuiLayoutProc(ZM_OnCreate, cp, 0, 0, 0);
        p->old_call = (ZCtlProc)&ZuiLayoutProc;
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
        ZuiAny old_udata = p->old_udata;
        old_call(ProcId, cp, old_udata, Param1, Param2);
        if (p->m_diNormal)
            ZuiResDBDelRes(p->m_diNormal);
        if (p->m_diHot)
            ZuiResDBDelRes(p->m_diHot);
        if (p->m_diPushed)
            ZuiResDBDelRes(p->m_diPushed);
        if (p->m_diFocused)
            ZuiResDBDelRes(p->m_diFocused);
        if (p->m_diSep)
            ZuiResDBDelRes(p->m_diSep);
        if (p->m_rFont)
            ZuiResDBDelRes(p->m_rFont);
        if (p->m_rListFont)
            ZuiResDBDelRes(p->m_rListFont);
        free(p);

        return 0;
    }
    case ZM_GetObject: {
        if (_tcsicmp(Param1, (ZuiAny)ZC_ListHeaderItem) == 0)
            return (ZuiAny)p;
        break;
    }
    case ZM_SetAttribute: {
        if (_tcsicmp(Param1, _T("dragable")) == 0)
            ZCCALL(ZM_ListHeaderItem_SetDragable, cp, (ZuiAny)(_tcsicmp(Param2, _T("true")) == 0 ? TRUE : FALSE), NULL);
        else if (_tcsicmp(Param1, _T("sepwidth")) == 0)
            ZCCALL(ZM_ListHeaderItem_SetSepWidth, cp, (ZuiAny)_ttoi(Param2), NULL);
        else if (_tcsicmp(Param1, _T("normalimage")) == 0)
            ZCCALL(ZM_ListHeaderItem_SetNormalImage, cp, ZuiResDBGetRes(Param2, ZREST_IMG), NULL);
        else if (_tcsicmp(Param1, _T("hotimage")) == 0)
            ZCCALL(ZM_ListHeaderItem_SetHotImage, cp, ZuiResDBGetRes(Param2, ZREST_IMG), NULL);
        else if (_tcsicmp(Param1, _T("pushedimage")) == 0)
            ZCCALL(ZM_ListHeaderItem_SetPushedImage, cp, ZuiResDBGetRes(Param2, ZREST_IMG), NULL);
        else if (_tcsicmp(Param1, _T("focusedimage")) == 0)
            ZCCALL(ZM_ListHeaderItem_SetFocusedImage, cp, ZuiResDBGetRes(Param2, ZREST_IMG), NULL);
        else if (_tcsicmp(Param1, _T("sepimage")) == 0)
            ZCCALL(ZM_ListHeaderItem_SetSepImage, cp, ZuiResDBGetRes(Param2, ZREST_IMG), NULL);
        else if (_tcsicmp(Param1, _T("normalcolor")) == 0)
            p->m_ColorNormal = ZuiStr2Color(Param2);
        else if (_tcsicmp(Param1, _T("hotcolor")) == 0)
            p->m_ColorHot = ZuiStr2Color(Param2);
        else if (_tcsicmp(Param1, _T("pushedcolor")) == 0)
            p->m_ColorPushed = ZuiStr2Color(Param2);
        else if (_tcsicmp(Param1, _T("focusedcolor")) == 0)
            p->m_ColorFocused = ZuiStr2Color(Param2);
        else if (_tcsicmp(Param1, _T("sepcolor")) == 0)
            p->m_ColorSep = ZuiStr2Color(Param2);
        else if (_tcsicmp(Param1, _T("font")) == 0) {
            if (p->m_rFont)
                ZuiResDBDelRes(p->m_rFont);
            p->m_rFont = ZuiResDBGetRes(Param2, ZREST_FONT);
            ZuiControlNeedUpdate(cp);
        }
        else if (_tcsicmp(Param1, _T("listfont")) == 0) {
            if (p->m_rListFont)
                ZuiResDBDelRes(p->m_rListFont);
            p->m_rListFont = ZuiResDBGetRes(Param2, ZREST_FONT);
            ZuiControlNeedUpdate(cp);
        }
        else if (_tcsicmp(Param1, _T("align")) == 0) {
            //横向对齐方式
            if (_tcsicmp(Param2, _T("left")) == 0) {
                p->m_uTextStyle &= ~(ZDT_CENTER | ZDT_RIGHT);
                p->m_uTextStyle |= ZDT_LEFT;
            }
            if (_tcsicmp(Param2, _T("center")) == 0) {
                p->m_uTextStyle &= ~(ZDT_LEFT | ZDT_RIGHT);
                p->m_uTextStyle |= ZDT_CENTER;
            }
            if (_tcsicmp(Param2, _T("right")) == 0) {
                p->m_uTextStyle &= ~(ZDT_LEFT | ZDT_CENTER);
                p->m_uTextStyle |= ZDT_RIGHT;
            }
            ZuiControlNeedUpdate(cp);
        }
        else if (_tcsicmp(Param1, _T("valign")) == 0) {
            //纵向对齐方式
            if (_tcsicmp(Param2, _T("top")) == 0) {
                p->m_uTextStyle &= ~(ZDT_BOTTOM | ZDT_VCENTER | ZDT_WORDBREAK);
                p->m_uTextStyle |= (ZDT_TOP | ZDT_SINGLELINE);
            }
            if (_tcsicmp(Param2, _T("vcenter")) == 0) {
                p->m_uTextStyle &= ~(ZDT_TOP | ZDT_BOTTOM | ZDT_WORDBREAK);
                p->m_uTextStyle |= (ZDT_VCENTER | ZDT_SINGLELINE);
            }
            if (_tcsicmp(Param2, _T("bottom")) == 0) {
                p->m_uTextStyle &= ~(ZDT_TOP | ZDT_VCENTER | ZDT_WORDBREAK);
                p->m_uTextStyle |= (ZDT_BOTTOM | ZDT_SINGLELINE);
            }
            ZuiControlNeedUpdate(cp);
        }
        else if (_tcsicmp(Param1, _T("listalign")) == 0) {
            //横向对齐方式
            if (_tcsicmp(Param2, _T("left")) == 0) {
                p->m_uListTextStyle &= ~(ZDT_CENTER | ZDT_RIGHT);
                p->m_uListTextStyle |= ZDT_LEFT;
            }
            if (_tcsicmp(Param2, _T("center")) == 0) {
                p->m_uListTextStyle &= ~(ZDT_LEFT | ZDT_RIGHT);
                p->m_uListTextStyle |= ZDT_CENTER;
            }
            if (_tcsicmp(Param2, _T("right")) == 0) {
                p->m_uListTextStyle &= ~(ZDT_LEFT | ZDT_CENTER);
                p->m_uListTextStyle |= ZDT_RIGHT;
            }
            ZuiControlNeedUpdate(cp);
        }
        else if (_tcsicmp(Param1, _T("listvalign")) == 0) {
            //纵向对齐方式
            if (_tcsicmp(Param2, _T("top")) == 0) {
                p->m_uListTextStyle &= ~(ZDT_BOTTOM | ZDT_VCENTER | ZDT_WORDBREAK);
                p->m_uListTextStyle |= (ZDT_TOP | ZDT_SINGLELINE);
            }
            if (_tcsicmp(Param2, _T("vcenter")) == 0) {
                p->m_uListTextStyle &= ~(ZDT_TOP | ZDT_BOTTOM | ZDT_WORDBREAK);
                p->m_uListTextStyle |= (ZDT_VCENTER | ZDT_SINGLELINE);
            }
            if (_tcsicmp(Param2, _T("bottom")) == 0) {
                p->m_uListTextStyle &= ~(ZDT_TOP | ZDT_VCENTER | ZDT_WORDBREAK);
                p->m_uListTextStyle |= (ZDT_BOTTOM | ZDT_SINGLELINE);
            }
        }
        else if (_tcsicmp(Param1, _T("textcolor")) == 0) {
            //字体颜色
            p->m_cTextColor = ZuiStr2Color(Param2);
            ZuiControlNeedUpdate(cp);
        }
        else if (_tcsicmp(Param1, _T("textpadding")) == 0) {
            //字体边距
            ZRect rcPadding = { 0 };
            ZuiText pstr = NULL;
            rcPadding.left = _tcstol(Param2, &pstr, 10);  ASSERT(pstr);
            rcPadding.top = _tcstol(pstr + 1, &pstr, 10);    ASSERT(pstr);
            rcPadding.right = _tcstol(pstr + 1, &pstr, 10);  ASSERT(pstr);
            rcPadding.bottom = _tcstol(pstr + 1, &pstr, 10); ASSERT(pstr);
            memcpy(&p->m_rcPadding, &rcPadding, sizeof(ZRect));
            ZuiControlNeedUpdate(cp);
        }
        break;
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
        if (p->m_diNormal)
            ZuiResDBDelRes(p->m_diNormal);
        p->m_diNormal = Param1;
        break;
    }
    case ZM_ListHeaderItem_SetHotImage: {
        if (p->m_diHot)
            ZuiResDBDelRes(p->m_diHot);
        p->m_diHot = Param1;
        break;
    }
    case ZM_ListHeaderItem_SetPushedImage: {
        if (p->m_diPushed)
            ZuiResDBDelRes(p->m_diPushed);
        p->m_diPushed = Param1;
        break;
    }
    case ZM_ListHeaderItem_SetFocusedImage: {
        if (p->m_diFocused)
            ZuiResDBDelRes(p->m_diFocused);
        p->m_diFocused = Param1;
        break;
    }
    case ZM_ListHeaderItem_SetSepImage: {
        if (p->m_diSep)
            ZuiResDBDelRes(p->m_diSep);
        p->m_diSep = Param1;
        break;
    }
    case ZM_GetType:
        return (ZuiAny)ZC_ListHeaderItem;
    case ZM_CoreInit:
        return (ZuiAny)TRUE;
    case ZM_CoreUnInit:
        return (ZuiAny)NULL;
    default:
        break;
    }
    return p->old_call(ProcId, cp, p->old_udata, Param1, Param2);
}

