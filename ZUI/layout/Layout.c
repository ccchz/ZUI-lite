﻿#include "Layout.h"
#include <core/control.h>
#include <core/function.h>
#include <core/builder.h>
#include <stdlib.h>

void* ZCALL ZuiLayoutProc(int ProcId, ZuiControl cp, ZuiLayout p, ZuiAny Param1, ZuiAny Param2) {
    ZSize sz = { 0, 0 };
    ZSize sz1 = { 0,0 };
    switch (ProcId)
    {
    case ZM_FindControl: {
        ZuiControl pResult = NULL;
        FINDCONTROLPROC findProc = __FindControlsFromUpdate;

        if ((int)Param2 & ZFIND_FROM_UPDATE) findProc = __FindControlsFromUpdate;
        else if ((int)Param2 & ZFIND_FROM_TAB) findProc = __FindControlFromTab;
        else if ((int)Param2 & ZFIND_FROM_POINT) findProc = __FindControlFromPoint;
        else if ((int)Param2 & ZFIND_FROM_NAME) findProc = __FindControlFromName;

        // Check if this guy is valid
        if (((unsigned int)Param2 & ZFIND_VISIBLE) != 0 && !cp->m_bVisible) return NULL;
        if (((unsigned int)Param2 & ZFIND_ENABLED) != 0 && !cp->m_bEnabled) return NULL;
        if (((unsigned int)Param2 & ZFIND_HITTEST) != 0 && !ZuiIsPointInRect(&cp->m_rcItem, Param1)) return NULL;
        if (((unsigned int)Param2 & ZFIND_UPDATETEST) != 0 && ((FINDCONTROLPROC)findProc)(cp, Param1) != NULL) return NULL;


        if (((unsigned int)Param2 & ZFIND_ME_FIRST) != 0) {
            if (((unsigned int)Param2 & ZFIND_HITTEST) == 0 || cp->m_bMouseEnabled) pResult = ((FINDCONTROLPROC)findProc)(cp, Param1);
        }
        if (pResult != NULL) return pResult;

        if (((unsigned int)Param1 & ZFIND_HITTEST) == 0 || p->m_bMouseChildEnabled) {
            ZRect rc = cp->m_rcItem;
            rc.left += p->m_rcInset.left;
            rc.top += p->m_rcInset.top;
            rc.right -= p->m_rcInset.right;
            rc.bottom -= p->m_rcInset.bottom;
            if (p->m_pVerticalScrollBar && p->m_pVerticalScrollBar->m_bVisible) rc.right -= (int)ZCCALL(ZM_GetFixedWidth, p->m_pVerticalScrollBar, NULL, NULL);
            if (p->m_pHorizontalScrollBar && p->m_pHorizontalScrollBar->m_bVisible) rc.bottom -= (int)ZCCALL(ZM_GetFixedHeight, p->m_pHorizontalScrollBar, NULL, NULL);

            if (((unsigned int)Param2 & ZFIND_TOP_FIRST) != 0) {
                for (int it = darray_len(p->m_items) - 1; it >= 0; it--) {
                    pResult = (ZuiControl)ZCCALL(ZM_FindControl, (ZuiControl)(p->m_items->data[it]), Param1, Param2);
                    if (pResult != NULL) {
                        if (((unsigned int)Param2 & ZFIND_HITTEST) != 0 && !pResult->m_bFloat && !ZuiIsPointInRect(&rc, Param1))
                            continue;
                        else
                            return pResult;
                    }
                }
            }
            else {
                for (int it = 0; it < darray_len(p->m_items); it++) {
                    pResult = (ZuiControl)ZCCALL(ZM_FindControl, (ZuiControl)(p->m_items->data[it]), Param1, Param2);
                    if (pResult != NULL) {
                        if (((unsigned int)Param2 & ZFIND_HITTEST) != 0 && !pResult->m_bFloat && !ZuiIsPointInRect(&rc, Param1))
                            continue;
                        else
                            return pResult;
                    }
                }
            }
        }

        if (p->m_pVerticalScrollBar != NULL) pResult = (ZuiControl)ZCCALL(ZM_FindControl, p->m_pVerticalScrollBar, Param1, Param2);
        if (pResult == NULL && p->m_pHorizontalScrollBar != NULL) pResult = (ZuiControl)ZCCALL(ZM_FindControl, p->m_pHorizontalScrollBar, Param1, Param2);
        if (pResult != NULL)
            return pResult;

        pResult = NULL;
        if (pResult == NULL && ((unsigned int)Param2 & ZFIND_ME_FIRST) == 0) {
            if (((unsigned int)Param2 & ZFIND_HITTEST) == 0 || cp->m_bMouseEnabled) pResult = ((FINDCONTROLPROC)findProc)(cp, Param1);
        }
        return pResult;
    }
    case ZM_OnPaint: {
        //这里是所有绘制的调度中心
		
		//绘制区域
        ZRect rcTemp = { 0 };
		//求绘制区域交集
        if (!IntersectRect((LPRECT)&rcTemp, (const RECT *)Param2, (const RECT *)&cp->m_rcItem))
            //不在绘制区域
            return 0;

		//设置新剪裁区
		ZRect rcClip;
		MAKEZRECT(rcClip, rcTemp.left, rcTemp.top, rcTemp.right, rcTemp.bottom);
		ZuiGraphicsPushClipRect((ZuiGraphics)Param1, &rcClip, 0);

        //获取当前剪裁区
        ZRect CurBox;
        ZuiGraphicsGetClipBox((ZuiGraphics)Param1, &CurBox);

		//通知当前容器绘制
        p->old_call(ZM_OnPaint, cp, 0, Param1, Param2);
		//绘制子控件
        if (darray_len(p->m_items) > 0) {
			//当前控件区域
            ZRect rc = cp->m_rcItem;
			//求出内边距
            rc.left += p->m_rcInset.left;
            rc.top += p->m_rcInset.top;
            rc.right -= p->m_rcInset.right;
            rc.bottom -= p->m_rcInset.bottom;
			//获取滚动条区域,排除滚动条绘制
            if (p->m_pVerticalScrollBar && p->m_pVerticalScrollBar->m_bVisible) rc.right -= (int)ZCCALL(ZM_GetFixedWidth, p->m_pVerticalScrollBar, NULL, NULL);
            if (p->m_pHorizontalScrollBar && p->m_pHorizontalScrollBar->m_bVisible) rc.bottom -= (int)ZCCALL(ZM_GetFixedHeight, p->m_pHorizontalScrollBar, NULL, NULL);
			//求当前控件的具体交集
            if (!IntersectRect((LPRECT)&rcTemp, (const RECT *)Param2, (const RECT *)&rc)) {
                for (int it = 0; it < darray_len(p->m_items); it++) {
                    ZuiControl pControl = (ZuiControl)(p->m_items->data[it]);
                    if (!pControl->m_bVisible) continue;
                    if (!IntersectRect((LPRECT)&rcTemp, (const RECT *)Param2, (const RECT *)ZCCALL(ZM_GetPos, pControl, 0, 0))) continue;
                    if (pControl->m_bFloat) {
                        if (!IntersectRect((LPRECT)&rcTemp, (const RECT *)&cp->m_rcItem, (const RECT *)ZCCALL(ZM_GetPos, pControl, 0, 0))) continue;
                        IntersectRect((LPRECT)&rcTemp, (const RECT *)Param2, (const RECT *)&pControl->m_rcItem);
                        //MAKEZRECT(rcClip, rcTemp.left, rcTemp.top, rcTemp.right, rcTemp.bottom);
                        //ZuiGraphicsPushClipRect((ZuiGraphics)Param1, &rcClip, 0);
                        ZCCALL(ZM_OnPaint, pControl, Param1, &rcTemp);
						//ZuiGraphicsPopClip((ZuiGraphics)Param1);
                    }
                }
            }
            else {
                //设置子剪裁区
                //MAKEZRECT(rcClip, rcTemp.left, rcTemp.top, rcTemp.right, rcTemp.bottom);
                //ZuiGraphicsPushClipRect((ZuiGraphics)Param1, &rcClip, 0);


                for (int it = 0; it < darray_len(p->m_items); it++) {
                    ZuiControl pControl = (ZuiControl)(p->m_items->data[it]);
                    if (!pControl->m_bVisible) continue;
                    if (!IntersectRect((LPRECT)&rcTemp, (const RECT *)Param2, (const RECT *)ZCCALL(ZM_GetPos, pControl, 0, 0))) continue;
                    if (pControl->m_bFloat) {
                        if (!IntersectRect((LPRECT)&rcTemp, (const RECT *)Param2, (const RECT *)ZCCALL(ZM_GetPos, pControl, 0, 0)))
                            continue;
                        IntersectRect((LPRECT)&rcTemp, (const RECT *)Param2, (const RECT *)&pControl->m_rcItem);
                        //MAKEZRECT(rcClip, rcTemp.left, rcTemp.top, rcTemp.right, rcTemp.bottom);
						//ZuiGraphicsPushClipRect((ZuiGraphics)Param1, &rcClip, 0);
                        if (pControl->m_aAnime)
                            pControl->m_aAnime->OnPaint(pControl, Param1, Param2);
                        else
                            ZCCALL(ZM_OnPaint, pControl, Param1, &rcTemp);
						//ZuiGraphicsPopClip((ZuiGraphics)Param1);
                    }
                    else {
                        if (!IntersectRect((LPRECT)&rcTemp, (const RECT *)&rc, (const RECT *)ZCCALL(ZM_GetPos, pControl, 0, 0))) continue;
                        if (pControl->m_aAnime)
                            pControl->m_aAnime->OnPaint(pControl, Param1, Param2);
                        else {
                            IntersectRect((LPRECT)&rcTemp, (const RECT *)Param2, (const RECT *)&rcTemp);
                           // MAKEZRECT(rcClip, rcTemp.left, rcTemp.top, rcTemp.right, rcTemp.bottom);
							//ZuiGraphicsPushClipRect((ZuiGraphics)Param1, &rcClip, 0);
                            ZCCALL(ZM_OnPaint, pControl, Param1, &rcTemp);
							//ZuiGraphicsPopClip((ZuiGraphics)Param1);
                        }
                    }
                }

                //恢复剪裁区
				//ZuiGraphicsPopClip((ZuiGraphics)Param1);
            }
        }
        //绘制滚动条
        if (p->m_pVerticalScrollBar != NULL && p->m_pVerticalScrollBar->m_bVisible) {
            if (IntersectRect((LPRECT)&rcTemp, (const RECT *)Param2, (const RECT *)ZCCALL(ZM_GetPos, p->m_pVerticalScrollBar, 0, 0))) {
                ZCCALL(ZM_OnPaint, p->m_pVerticalScrollBar, Param1, Param2);
            }
        }
        if (p->m_pHorizontalScrollBar != NULL && p->m_pHorizontalScrollBar->m_bVisible) {
            if (IntersectRect((LPRECT)&rcTemp, (const RECT *)Param2, (const RECT *)ZCCALL(ZM_GetPos, p->m_pHorizontalScrollBar, 0, 0))) {
                ZCCALL(ZM_OnPaint, p->m_pHorizontalScrollBar, Param1, Param2);
            }
        }
        //通知绘制完毕
		ZCCALL(ZM_EndPaint, cp, Param1, Param2);

        //恢复剪裁区
		ZuiGraphicsPopClip((ZuiGraphics)Param1);
        return 0;//绘图完毕,不需要默认
    }
    case ZM_SetPos: {
        ZRect rc;
        p->old_call(ProcId, cp, 0, Param1, Param2);
        if (darray_isempty(p->m_items))
            return 0;

        rc = cp->m_rcItem;
		rc.left += p->m_rcInset.left;
		rc.top += p->m_rcInset.top;
		rc.right -= p->m_rcInset.right;
		rc.bottom -= p->m_rcInset.bottom;
		rc.left += cp->m_dwBorderWidth;
		rc.top += cp->m_dwBorderWidth;
		rc.right -= cp->m_dwBorderWidth;
		rc.bottom -= cp->m_dwBorderWidth;

        for (int it = 0; it < darray_len(p->m_items); it++) {
            ZuiControl pControl = (ZuiControl)(p->m_items->data[it]);
            if (!pControl->m_bVisible)
                continue;
            if (pControl->m_bFloat) {
                ZCCALL(ZM_Layout_SetFloatPos, cp, (void *)it, 0);
            }
            else {
                ZRect rcCtrl;
                ZSize sz = { rc.right - rc.left, rc.bottom - rc.top };
                if (sz.cx < (int)ZCCALL(ZM_GetMinWidth, pControl, 0, 0)) sz.cx = (int)ZCCALL(ZM_GetMinWidth, pControl, 0, 0);
                if (sz.cx > (int)ZCCALL(ZM_GetMaxWidth, pControl, 0, 0)) sz.cx = (int)ZCCALL(ZM_GetMaxWidth, pControl, 0, 0);
                if (sz.cy < (int)ZCCALL(ZM_GetMinHeight, pControl, 0, 0)) sz.cy = (int)ZCCALL(ZM_GetMinHeight, pControl, 0, 0);
                if (sz.cy > (int)ZCCALL(ZM_GetMaxHeight, pControl, 0, 0)) sz.cy = (int)ZCCALL(ZM_GetMaxHeight, pControl, 0, 0);
                rcCtrl.left = rc.left;
                rcCtrl.top = rc.top;
                rcCtrl.right = rc.left + sz.cx;
                rcCtrl.bottom = rc.top + sz.cy;
                ZCCALL(ZM_SetPos, pControl, &rcCtrl, FALSE);
            }
        }
        break;
    }
    case ZM_OnEvent: {
        TEventUI *event = (TEventUI *)Param1;
        //不响应鼠标消息
        if (!cp->m_bMouseEnabled && event->Type > ZEVENT__MOUSEBEGIN && event->Type < ZEVENT__MOUSEEND) {
            if (cp->m_pParent != NULL)
                ZCCALL(ZM_OnEvent, cp->m_pParent, Param1, NULL);
            else
                ZuiDefaultControlProc(ZM_OnEvent, cp, 0, Param1, NULL);
            return 0;
        }

        if (event->Type == ZEVENT_SETFOCUS)
        {
            p->m_bFocused = TRUE;
            return NULL;
        }
        if (event->Type == ZEVENT_KILLFOCUS)
        {
            p->m_bFocused = FALSE;
            return NULL;
        }
        if (p->m_pVerticalScrollBar != NULL && p->m_pVerticalScrollBar->m_bVisible && p->m_pVerticalScrollBar->m_bEnabled)
        {
            if (event->Type == ZEVENT_KEYDOWN)
            {
                switch (event->chKey) {
                case VK_DOWN:
                    return ZCCALL(ZM_Layout_LineDown, cp, NULL, NULL);
                case VK_UP:
                    return ZCCALL(ZM_Layout_LineUp, cp, NULL, NULL);
                case VK_NEXT:
                    return ZCCALL(ZM_Layout_PageDown, cp, NULL, NULL);
                case VK_PRIOR:
                    return ZCCALL(ZM_Layout_PageUp, cp, NULL, NULL);
                case VK_HOME:
                    return ZCCALL(ZM_Layout_HomeUp, cp, NULL, NULL);
                case VK_END:
                    return ZCCALL(ZM_Layout_EndDown, cp, NULL, NULL);
                case VK_LEFT:
                    return ZCCALL(ZM_Layout_LineLeft, cp, NULL, NULL);
                case VK_RIGHT:
                    return ZCCALL(ZM_Layout_LineRight, cp, NULL, NULL);
                }
            }
            else if (event->Type == ZEVENT_SCROLLWHEEL)
            {
                switch (LOWORD(event->wParam)) {
                case SB_LINEUP:
                    return ZCCALL(ZM_Layout_LineUp, cp, NULL, NULL);
                case SB_LINEDOWN:
                    return ZCCALL(ZM_Layout_LineDown, cp, NULL, NULL);
                }
            }
        }
        else if (p->m_pHorizontalScrollBar != NULL && p->m_pHorizontalScrollBar->m_bVisible && p->m_pHorizontalScrollBar->m_bEnabled) {
            if (event->Type == ZEVENT_KEYDOWN)
            {
                switch (event->chKey) {
                case VK_DOWN:
                    return ZCCALL(ZM_Layout_LineRight, cp, NULL, NULL);
                case VK_UP:
                    return ZCCALL(ZM_Layout_LineLeft, cp, NULL, NULL);
                case VK_NEXT:
                    return ZCCALL(ZM_Layout_PageRight, cp, NULL, NULL);
                case VK_PRIOR:
                    return ZCCALL(ZM_Layout_PageLeft, cp, NULL, NULL);
                case VK_HOME:
                    return ZCCALL(ZM_Layout_HomeLeft, cp, NULL, NULL);
                case VK_END:
                    return ZCCALL(ZM_Layout_EndRight, cp, NULL, NULL);
                }
            }
            else if (event->Type == ZEVENT_SCROLLWHEEL)
            {
                switch (LOWORD(event->wParam)) {
                case SB_LINEUP:
                    return ZCCALL(ZM_Layout_LineLeft, cp, NULL, NULL);
                case SB_LINEDOWN:
                    return ZCCALL(ZM_Layout_LineRight, cp, NULL, NULL);
                }
            }
        }
        break;
    }
    case ZM_Layout_GetChildVAlign: {
        return (ZuiAny)p->m_iChildVAlign;
    }
    case ZM_Layout_GetChildAlign: {
        return (ZuiAny)p->m_iChildAlign;
    }
    case ZM_SetAttribute: {
        if (_tcsicmp(Param1, _T("inset")) == 0) {
            ZRect rcInset = { 0 };
            ZuiText pstr = NULL;
            rcInset.left = _tcstol(Param2, &pstr, 10);  ASSERT(pstr);
            rcInset.top = _tcstol(pstr + 1, &pstr, 10);    ASSERT(pstr);
            rcInset.right = _tcstol(pstr + 1, &pstr, 10);  ASSERT(pstr);
            rcInset.bottom = _tcstol(pstr + 1, &pstr, 10); ASSERT(pstr);
            ZCCALL(ZM_Layout_SetInset, cp, &rcInset, NULL);
        }
        else if (_tcsicmp(Param1, _T("stepsize")) == 0) {
            ZCCALL(ZM_Layout_SetScrollStepSize, cp, (ZuiAny)(_wtoi(Param2)), NULL);
        }
        else if (_tcsicmp(Param1, _T("valign")) == 0) {
            //纵向对齐方式
            if (_tcsicmp(Param2, _T("top")) == 0) {
                p->m_iChildVAlign &= ~(ZDT_BOTTOM | ZDT_VCENTER);
                p->m_iChildVAlign |= ZDT_TOP;
            }
            if (_tcsicmp(Param2, _T("vcenter")) == 0) {
                p->m_iChildVAlign &= ~(ZDT_TOP | ZDT_BOTTOM);
                p->m_iChildVAlign |= ZDT_VCENTER;
            }
            if (_tcsicmp(Param2, _T("bottom")) == 0) {
                p->m_iChildVAlign &= ~(ZDT_TOP | ZDT_VCENTER);
                p->m_iChildVAlign |= ZDT_BOTTOM;
            }
            ZuiControlNeedUpdate(cp);
        }
        else if (_tcsicmp(Param1, _T("align")) == 0) {
            //横向对齐方式
            if (_tcsicmp(Param2, _T("left")) == 0) {
                p->m_iChildAlign &= ~(ZDT_RIGHT | ZDT_CENTER);
                p->m_iChildAlign |= ZDT_LEFT;
            }
            if (_tcsicmp(Param2, _T("center")) == 0) {
                p->m_iChildAlign &= ~(ZDT_LEFT | ZDT_RIGHT);
                p->m_iChildAlign |= ZDT_CENTER;
            }
            if (_tcsicmp(Param2, _T("right")) == 0) {
                p->m_iChildAlign &= ~(ZDT_LEFT | ZDT_CENTER);
                p->m_iChildAlign |= ZDT_RIGHT;
            }
            ZuiControlNeedUpdate(cp);
        }
        else if (_tcsicmp(Param1, _T("vscrollbar")) == 0) {
            ZCCALL(ZM_Layout_EnableScrollBar, cp, (ZuiAny)(_tcscmp(Param2, _T("true")) == 0), (ZuiAny)(ZCCALL(ZM_Layout_GetHorizontalScrollBar, cp, NULL, NULL) != NULL));
        }
        else if (_tcsicmp(Param1, _T("hscrollbar")) == 0) {
            ZCCALL(ZM_Layout_EnableScrollBar, cp, (ZuiAny)(ZCCALL(ZM_Layout_GetVerticalScrollBar, cp, NULL, NULL) != NULL), (ZuiAny)((_tcscmp(Param2, _T("true"))) == 0));
        }
        else if ((_tcsicmp(Param1, _T("sbbkcolor")) == 0) ||
                (_tcsicmp(Param1, _T("sbtnormalcolor")) == 0) ||
                (_tcsicmp(Param1, _T("sbthotcolor")) == 0) ||
                (_tcsicmp(Param1, _T("sbtpushcolor")) == 0) ||
                (_tcsicmp(Param1, _T("sbbnormalcolor")) == 0) ||
                (_tcsicmp(Param1, _T("sbbhotcolor")) == 0) ||
                (_tcsicmp(Param1, _T("sbbpushcolor")) == 0) ||
                (_tcsicmp(Param1, _T("sbdisablecolor")) == 0) ||
                (_tcsicmp(Param1, _T("sbb1show")) == 0) ||
                (_tcsicmp(Param1, _T("sbb2show")) == 0) ||
                (_tcsicmp(Param1, _T("sbimageres")) == 0) ||
                (_tcsicmp(Param1, _T("sbvsrc")) == 0) ||
                (_tcsicmp(Param1, _T("sbhsrc")) == 0)){
                ZCCALL(ProcId, p->m_pVerticalScrollBar, Param1, Param2);
                ZCCALL(ProcId, p->m_pHorizontalScrollBar, Param1, Param2);
        }
        break;
    }
    case ZM_ScrollBar_SetColor: {
        ZCCALL(ProcId, p->m_pVerticalScrollBar, Param1, Param2);
        ZCCALL(ProcId, p->m_pHorizontalScrollBar, Param1, Param2);
    }
    case ZM_SetOs: {
        p->old_call(ProcId, cp, 0, Param1, Param2);
        for (int it = 0; it < darray_len(p->m_items); it++) {
            ZCCALL(ZM_SetOs, (ZuiControl)(p->m_items->data[it]), cp, (ZuiAny)TRUE);
        }
        if (p->m_pVerticalScrollBar != NULL) ZCCALL(ZM_SetOs, p->m_pVerticalScrollBar, cp, (ZuiAny)TRUE);
        if (p->m_pHorizontalScrollBar != NULL) ZCCALL(ZM_SetOs, p->m_pHorizontalScrollBar, cp, (ZuiAny)TRUE);
        return 0;
    }
    case ZM_SetVisible: {
        p->old_call(ZM_SetVisible, cp, 0, Param1, Param2);
        return 0;
        break;
    }
    case ZM_Layout_Add: {
        if (Param1 == NULL) return FALSE;

        if (cp->m_pOs != NULL)
            ZCCALL(ZM_SetOs, (ZuiControl)Param1,cp, (void*)TRUE);
        if (cp->m_bVisible)
            ZuiControlNeedUpdate(cp);

        return (void *)darray_append(p->m_items, Param1);
        break;
    }
    case ZM_Layout_AddAt: {
        if (Param1 == NULL) return FALSE;

        if (cp->m_pOs != NULL)
            ZCCALL(ZM_SetOs, (ZuiControl)Param1, cp, (void*)TRUE);
        if (cp->m_bVisible)
            ZuiControlNeedUpdate(cp);

        return (void *)darray_insert(p->m_items, (int)Param2, (ZuiControl)Param1);
        break;
    }
    case ZM_Layout_Remove: {
        if (Param1 == NULL)
            return FALSE;

        for (int it = 0; it < darray_len(p->m_items); it++) {
            if ((ZuiControl)(p->m_items->data[it]) == (ZuiControl)Param1) {
                ZuiControlNeedUpdate(cp);
				darray_delete(p->m_items, it);
                //if (!Param2)
                //   FreeZuiControl((ZuiControl)Param1, FALSE);
				return (ZuiAny)TRUE;
            }
        }
        return FALSE;
        break;
    }
    case ZM_Layout_RemoveAt: {
        ZuiControl pControl = ZuiLayoutProc(ZM_Layout_GetItemAt, cp, p, Param1, 0);
        if (pControl != NULL) {
            return ZuiLayoutProc(ZM_Layout_Remove, cp, p, pControl, 0);
        }

        return FALSE;
        break;
    }
    case ZM_Layout_RemoveAll: {
        for (int it = 0; it < darray_len(p->m_items); it++) {
            FreeZuiControl((ZuiControl)(p->m_items->data[it]), FALSE);
        }
        darray_empty(p->m_items);
        ZuiControlNeedUpdate(cp);
        break;
    }
    case ZM_Layout_GetCount: {
        return (ZuiAny)darray_len(p->m_items);
        break;
    }
    case ZM_Layout_GetItemIndex: {
        for (int it = 0; it < darray_len(p->m_items); it++) {
            if (p->m_items->data[it] == Param1) {
                return (ZuiAny)it;
            }
        }
        return (ZuiAny)-1;
        break;
    }
    case ZM_Layout_SetItemIndex: {
        for (int it = 0; it < darray_len(p->m_items); it++) {
            if (p->m_items->data[it] == Param1) {
                ZuiControlNeedUpdate(cp);
                darray_delete(p->m_items, it);
                return (ZuiAny)darray_insert(p->m_items, (int)Param2, Param1);
            }
        }

        return FALSE;
        break;
    }
    case ZM_Layout_GetItemAt: {
        if ((int)Param1 < 0 || (int)Param1 >= darray_len(p->m_items)) return NULL;
        return p->m_items->data[(LONG)Param1];
        break;
    }
    case ZM_Layout_SetFloatPos: {
        if ((int)Param1 < 0 || (int)Param1 >= darray_len(p->m_items)) {
            return 0;
        }
        ZuiControl pControl = (ZuiControl)(p->m_items->data[(int)Param1]);

        if (!pControl->m_bVisible)
            return 0;
        if (!pControl->m_bFloat) {
            //不是浮动控件
            return 0;
        }
        ZSize *szXY = (ZSize *)ZCCALL(ZM_GetFixedXY, pControl, 0, 0);
        ZSize sz = { (LONG)ZCCALL(ZM_GetFixedWidth, pControl, 0, 0), (LONG)ZCCALL(ZM_GetFixedHeight, pControl, 0, 0) };
        //ZRectR rcPercent = { 0 };// pControl->GetFloatPercent();
        int width = cp->m_rcItem.right - cp->m_rcItem.left - (cp->m_dwBorderWidth * 2);
        int height = cp->m_rcItem.bottom - cp->m_rcItem.top - (cp->m_dwBorderWidth * 2);
        ZRect rcCtrl = { 0 };
        if (pControl->m_piFloatPercent.left >= 1) {
            rcCtrl.left = (int)(cp->m_rcItem.left + pControl->m_piFloatPercent.left);
        }
        else {
            rcCtrl.left = (int)(width * pControl->m_piFloatPercent.left + szXY->cx);
        }
        if (pControl->m_piFloatPercent.top >= 1) {
            rcCtrl.top = (int)(cp->m_rcItem.top + pControl->m_piFloatPercent.top);
        }
        else {
            rcCtrl.top = (int)(height * pControl->m_piFloatPercent.top + szXY->cy);
        }
        if (pControl->m_piFloatPercent.right >= 1) {
            rcCtrl.right = (int)(rcCtrl.left + pControl->m_piFloatPercent.right);
        }
        else {
            rcCtrl.right = (int)(rcCtrl.left + width * pControl->m_piFloatPercent.right + sz.cx);
        }
        if (pControl->m_piFloatPercent.bottom >= 1) {
            rcCtrl.bottom = (int)(rcCtrl.top + pControl->m_piFloatPercent.bottom);
        }
        else {
            rcCtrl.bottom = (int)(rcCtrl.top + height * pControl->m_piFloatPercent.bottom + sz.cy);
        }
        ZCCALL(ZM_SetPos, pControl, &rcCtrl, FALSE);

        break;
    }
    case ZM_Layout_SetInset: {
        memcpy(&p->m_rcInset, Param1, sizeof(ZRect));
        ZuiControlNeedParentUpdate(cp);
        break;
    }
    case ZM_Layout_GetScrollPos: {
        ZuiSize sz = Param1;
        if (p->m_pVerticalScrollBar && p->m_pVerticalScrollBar->m_bVisible) sz->cy = (int)ZCCALL(ZM_ScrollBar_GetScrollPos, p->m_pVerticalScrollBar, NULL, NULL);
        if (p->m_pHorizontalScrollBar && p->m_pHorizontalScrollBar->m_bVisible) sz->cx = (int)ZCCALL(ZM_ScrollBar_GetScrollPos, p->m_pHorizontalScrollBar, NULL, NULL);
        return 0;
        break;
    }
    case ZM_Layout_GetScrollRange: {
        ZuiSize sz = Param1;
        if (p->m_pVerticalScrollBar && p->m_pVerticalScrollBar->m_bVisible) sz->cy = (int)ZCCALL(ZM_ScrollBar_GetScrollRange, p->m_pVerticalScrollBar, NULL, NULL);
        if (p->m_pHorizontalScrollBar && p->m_pHorizontalScrollBar->m_bVisible) sz->cx = (int)ZCCALL(ZM_ScrollBar_GetScrollRange, p->m_pHorizontalScrollBar, NULL, NULL);
        return 0;
        break;
    }
    case ZM_Layout_SetScrollPos: {
        ZSize *szPos = Param1;
        ZRect rcPos;
        int cx = 0;
        int cy = 0;
        if (p->m_pVerticalScrollBar && p->m_pVerticalScrollBar->m_bVisible) {
            int iLastScrollPos = (int)ZCCALL(ZM_ScrollBar_GetScrollPos, p->m_pVerticalScrollBar, NULL, NULL);
            ZCCALL(ZM_ScrollBar_SetScrollPos, p->m_pVerticalScrollBar, (ZuiAny)szPos->cy, NULL);
            cy = (int)ZCCALL(ZM_ScrollBar_GetScrollPos, p->m_pVerticalScrollBar, NULL, NULL) - iLastScrollPos;
        }

        if (p->m_pHorizontalScrollBar && p->m_pHorizontalScrollBar->m_bVisible) {
            int iLastScrollPos = (int)ZCCALL(ZM_ScrollBar_GetScrollPos, p->m_pHorizontalScrollBar, NULL, NULL);
            ZCCALL(ZM_ScrollBar_SetScrollPos, p->m_pHorizontalScrollBar, (ZuiAny)szPos->cx, NULL);
            cx = (int)ZCCALL(ZM_ScrollBar_GetScrollPos, p->m_pHorizontalScrollBar, NULL, NULL) - iLastScrollPos;
        }

        if (cx == 0 && cy == 0) return 0;

        for (int it2 = 0; it2 < darray_len(p->m_items); it2++) {
            ZuiControl pControl = (ZuiControl)(p->m_items->data[it2]);
            if (!pControl->m_bVisible) continue;
            if (pControl->m_bFloat) continue;
            ZRect *prcPos = (ZRect *)ZCCALL(ZM_GetPos, pControl, NULL, NULL);
            rcPos = *prcPos;
            rcPos.left -= cx;
            rcPos.right -= cx;
            rcPos.top -= cy;
            rcPos.bottom -= cy;
            ZCCALL(ZM_SetPos, pControl, &rcPos, NULL);
        }

        ZuiControlInvalidate(cp, TRUE);
        break;
    }
    case ZM_Layout_SetScrollStepSize: {
        if (Param1 > 0)
            p->m_nScrollStepSize = (int)Param1;
        break;
    }
    case ZM_Layout_GetScrollStepSize: {
        return (ZuiAny)p->m_nScrollStepSize;
        break;
    }
    case ZM_Layout_LineUp: {
        int cyLine = p->m_nScrollStepSize;
        if (p->m_nScrollStepSize == 0)
        {
            cyLine = 8;
        }

        ZCCALL(ZM_Layout_GetScrollPos, cp, &sz, NULL);
        sz.cy -= cyLine;
        ZCCALL(ZM_Layout_SetScrollPos, cp, &sz, NULL);
        break;
    }
    case ZM_Layout_LineDown: {
        int cyLine = p->m_nScrollStepSize;
        if (p->m_nScrollStepSize == 0)
        {
            cyLine = 8;
        }

        ZCCALL(ZM_Layout_GetScrollPos, cp, &sz, NULL);
        sz.cy += cyLine;
        ZCCALL(ZM_Layout_SetScrollPos, cp, &sz, NULL);
        break;
    }
    case ZM_Layout_PageUp: {
        ZCCALL(ZM_Layout_GetScrollPos, cp, &sz, NULL);
        int iOffset = cp->m_rcItem.bottom - cp->m_rcItem.top - p->m_rcInset.top - p->m_rcInset.bottom;
        if (p->m_pHorizontalScrollBar && p->m_pHorizontalScrollBar->m_bVisible) iOffset -= (int)ZCCALL(ZM_GetFixedHeight, p->m_pHorizontalScrollBar, NULL, NULL);
        sz.cy -= iOffset;
        ZCCALL(ZM_Layout_SetScrollPos, cp, &sz, NULL);
        break;
    }
    case ZM_Layout_PageDown: {
        ZCCALL(ZM_Layout_GetScrollPos, cp, &sz, NULL);
        int iOffset = cp->m_rcItem.bottom - cp->m_rcItem.top - p->m_rcInset.top - p->m_rcInset.bottom;
        if (p->m_pHorizontalScrollBar && p->m_pHorizontalScrollBar->m_bVisible) iOffset -= (int)ZCCALL(ZM_GetFixedHeight, p->m_pHorizontalScrollBar, NULL, NULL);
        sz.cy += iOffset;
        ZCCALL(ZM_Layout_SetScrollPos, cp, &sz, NULL);
        break;
    }
    case ZM_Layout_HomeUp: {
        ZCCALL(ZM_Layout_GetScrollPos, cp, &sz, NULL);
        sz.cy = 0;
        ZCCALL(ZM_Layout_SetScrollPos, cp, &sz, NULL);
        break;
    }
    case ZM_Layout_EndDown: {
        ZCCALL(ZM_Layout_GetScrollPos, cp, &sz, NULL);
        ZCCALL(ZM_Layout_GetScrollRange, cp, &sz1, NULL);
        sz.cy = sz1.cy;
        ZCCALL(ZM_Layout_SetScrollPos, cp, &sz, NULL);
        break;
    }
    case ZM_Layout_LineLeft: {
        int cxLine = p->m_nScrollStepSize == 0 ? 8 : p->m_nScrollStepSize;

        ZCCALL(ZM_Layout_GetScrollPos, cp, &sz, NULL);
        sz.cx -= cxLine;
        ZCCALL(ZM_Layout_SetScrollPos, cp, &sz, NULL);
        break;
    }
    case ZM_Layout_LineRight: {
        int cxLine = p->m_nScrollStepSize == 0 ? 8 : p->m_nScrollStepSize;

        ZCCALL(ZM_Layout_GetScrollPos, cp, &sz, NULL);
        sz.cx += cxLine;
        ZCCALL(ZM_Layout_SetScrollPos, cp, &sz, NULL);
        break;
    }
    case ZM_Layout_PageLeft: {
        ZCCALL(ZM_Layout_GetScrollPos, cp, &sz, NULL);
        int iOffset = cp->m_rcItem.right - cp->m_rcItem.left - p->m_rcInset.left - p->m_rcInset.right;
        if (p->m_pVerticalScrollBar && p->m_pVerticalScrollBar->m_bVisible) iOffset -= (int)ZCCALL(ZM_GetFixedWidth, p->m_pVerticalScrollBar, NULL, NULL);
        sz.cx -= iOffset;
        ZCCALL(ZM_Layout_SetScrollPos, cp, &sz, NULL);
        break;
    }
    case ZM_Layout_PageRight: {
        ZCCALL(ZM_Layout_GetScrollPos, cp, &sz, NULL);
        int iOffset = cp->m_rcItem.right - cp->m_rcItem.left - p->m_rcInset.left - p->m_rcInset.right;
        if (p->m_pVerticalScrollBar && p->m_pVerticalScrollBar->m_bVisible) iOffset -= (int)ZCCALL(ZM_GetFixedWidth, p->m_pVerticalScrollBar, NULL, NULL);
        sz.cx += iOffset;
        ZCCALL(ZM_Layout_SetScrollPos, cp, &sz, NULL);
        break;
    }
    case ZM_Layout_HomeLeft: {
        ZCCALL(ZM_Layout_GetScrollPos, cp, &sz, NULL);
        sz.cx = 0;
        ZCCALL(ZM_Layout_SetScrollPos, cp, &sz, NULL);
        break;
    }
    case ZM_Layout_EndRight: {
        ZCCALL(ZM_Layout_GetScrollPos, cp, &sz, NULL);
        ZCCALL(ZM_Layout_GetScrollRange, cp, &sz1, NULL);
        sz.cx = sz1.cx;
        ZCCALL(ZM_Layout_SetScrollPos, cp, &sz, NULL);
        break;
    }
    case ZM_Layout_EnableScrollBar: {
        BOOL bEnableVertical = (BOOL)Param1;
        BOOL bEnableHorizontal = (BOOL)Param2;
        if (bEnableVertical && !p->m_pVerticalScrollBar) {
            p->m_pVerticalScrollBar = NewZuiControl(_T("scrollbar"), NULL, NULL);//创建滚动条
            ZCCALL(ZM_ScrollBar_SetOwner, p->m_pVerticalScrollBar, cp, NULL);
            p->m_pVerticalScrollBar->m_pOs = cp->m_pOs;
            p->m_pVerticalScrollBar->m_pParent = cp;
            p->m_pVerticalScrollBar->m_bVisible = FALSE;
        }
        else if (!bEnableVertical && p->m_pVerticalScrollBar) {
            FreeZuiControl(p->m_pVerticalScrollBar, FALSE);
            p->m_pVerticalScrollBar = NULL;
        }

        if (bEnableHorizontal && !p->m_pHorizontalScrollBar) {
            p->m_pHorizontalScrollBar = NewZuiControl(_T("scrollbar"), NULL, NULL);//创建滚动条
            ZCCALL(ZM_ScrollBar_SetHorizontal, p->m_pHorizontalScrollBar, (ZuiAny)TRUE, NULL);
            ZCCALL(ZM_ScrollBar_SetOwner, p->m_pHorizontalScrollBar, cp, NULL);
            p->m_pHorizontalScrollBar->m_pOs = cp->m_pOs;
            p->m_pHorizontalScrollBar->m_pParent = cp;
            p->m_pHorizontalScrollBar->m_bVisible = FALSE;
        }
        else if (!bEnableHorizontal && p->m_pHorizontalScrollBar) {
            FreeZuiControl(p->m_pHorizontalScrollBar, FALSE);
            p->m_pHorizontalScrollBar = NULL;
        }

        ZuiControlNeedUpdate(cp);
        break;
    }
    case ZM_Layout_GetVerticalScrollBar: {
        return p->m_pVerticalScrollBar;
    }
    case ZM_Layout_GetHorizontalScrollBar: {
        return p->m_pHorizontalScrollBar;
    }
    case ZM_Layout_ProcessScrollBar: {
        ZRect *rc = Param1;
        int cxRequired = LPARAM((int)Param2);
        int cyRequired = HPARAM((int)Param2);

        while (p->m_pHorizontalScrollBar)
        {
            if (cxRequired > rc->right - rc->left && !p->m_pHorizontalScrollBar->m_bVisible)
            {
                ZCCALL(ZM_SetVisible, p->m_pHorizontalScrollBar, (ZuiAny)TRUE, NULL);
                ZCCALL(ZM_ScrollBar_SetScrollRange, p->m_pHorizontalScrollBar, (ZuiAny)(cxRequired - (rc->right - rc->left)), NULL);
                ZCCALL(ZM_ScrollBar_SetScrollPos, p->m_pHorizontalScrollBar, 0, NULL);
                p->m_bScrollProcess = TRUE;
                ZCCALL(ZM_SetPos, cp, &cp->m_rcItem, FALSE);
                p->m_bScrollProcess = FALSE;
                return 0;   //垂直滚动条在 重新布局中刷新，本次数据计算垂直滚动条已经不正确。
            }

            if (!p->m_pHorizontalScrollBar->m_bVisible) break;

            int cxScroll = cxRequired - (rc->right - rc->left);
            if (cxScroll <= 0 && !p->m_bScrollProcess)
            {
                ZCCALL(ZM_SetVisible, p->m_pHorizontalScrollBar, FALSE, NULL);
                ZCCALL(ZM_ScrollBar_SetScrollPos, p->m_pHorizontalScrollBar, 0, NULL);
                ZCCALL(ZM_ScrollBar_SetScrollRange, p->m_pHorizontalScrollBar, 0, NULL);
                ZCCALL(ZM_SetPos, cp, &cp->m_rcItem, FALSE);
                return 0;
            }
            else
            {
                ZRect rcScrollBarPos = { rc->left, rc->bottom, rc->right, rc->bottom + (int)ZCCALL(ZM_GetFixedHeight, p->m_pHorizontalScrollBar, NULL, NULL) };
                ZCCALL(ZM_SetPos, p->m_pHorizontalScrollBar, &rcScrollBarPos, FALSE);

                if (ZCCALL(ZM_ScrollBar_GetScrollRange, p->m_pHorizontalScrollBar, NULL, NULL) != (ZuiAny)cxScroll) {
                    int iScrollPos = (int)ZCCALL(ZM_ScrollBar_GetScrollPos, p->m_pHorizontalScrollBar, NULL, NULL);
                    ZCCALL(ZM_ScrollBar_SetScrollRange, p->m_pHorizontalScrollBar, (ZuiAny)abs(cxScroll), NULL);
                    if (ZCCALL(ZM_ScrollBar_GetScrollRange, p->m_pHorizontalScrollBar, NULL, NULL) == 0) {
                        ZCCALL(ZM_SetVisible, p->m_pHorizontalScrollBar, FALSE, NULL);
                        ZCCALL(ZM_ScrollBar_SetScrollPos, p->m_pHorizontalScrollBar, 0, NULL);
                    }
                    if ((ZuiAny)iScrollPos > ZCCALL(ZM_ScrollBar_GetScrollPos, p->m_pHorizontalScrollBar, NULL, NULL)) {
                        ZCCALL(ZM_SetPos, cp, &cp->m_rcItem, FALSE);
                    }
                }
            }
            break;
        }

        while (p->m_pVerticalScrollBar)
        {
            if (cyRequired > rc->bottom - rc->top && !p->m_pVerticalScrollBar->m_bVisible) {
                ZCCALL(ZM_SetVisible, p->m_pVerticalScrollBar, (ZuiAny)TRUE, NULL);
                ZCCALL(ZM_ScrollBar_SetScrollRange, p->m_pVerticalScrollBar, (ZuiAny)(cyRequired - (rc->bottom - rc->top)), NULL);
                ZCCALL(ZM_ScrollBar_SetScrollPos, p->m_pVerticalScrollBar, 0, NULL);
                p->m_bScrollProcess = TRUE;
                ZCCALL(ZM_SetPos, cp, &cp->m_rcItem, FALSE);
                p->m_bScrollProcess = FALSE;
                break;
            }
            // No scrollbar required
            if (!p->m_pVerticalScrollBar->m_bVisible) break;

            // Scroll not needed anymore?
            int cyScroll = cyRequired - (rc->bottom - rc->top);
            if (cyScroll <= 0 && !p->m_bScrollProcess) {
                ZCCALL(ZM_SetVisible, p->m_pVerticalScrollBar, FALSE, NULL);
                ZCCALL(ZM_ScrollBar_SetScrollPos, p->m_pVerticalScrollBar, 0, NULL);
                ZCCALL(ZM_ScrollBar_SetScrollRange, p->m_pVerticalScrollBar, 0, NULL);
                ZCCALL(ZM_SetPos, cp, &cp->m_rcItem, FALSE);
            }
            else
            {
                ZRect rcScrollBarPos = { rc->right, rc->top, rc->right + (int)ZCCALL(ZM_GetFixedWidth, p->m_pVerticalScrollBar, NULL, NULL), rc->bottom };
                ZCCALL(ZM_SetPos, p->m_pVerticalScrollBar, &rcScrollBarPos, FALSE);

                if (ZCCALL(ZM_ScrollBar_GetScrollRange, p->m_pVerticalScrollBar, NULL, NULL) != (ZuiAny)cyScroll) {
                    int iScrollPos = (int)ZCCALL(ZM_ScrollBar_GetScrollPos, p->m_pVerticalScrollBar, NULL, NULL);
                    ZCCALL(ZM_ScrollBar_SetScrollRange, p->m_pVerticalScrollBar, (ZuiAny)abs(cyScroll), NULL);
                    if (ZCCALL(ZM_ScrollBar_GetScrollRange, p->m_pVerticalScrollBar, NULL, NULL) == 0) {
                        ZCCALL(ZM_SetVisible, p->m_pVerticalScrollBar, FALSE, NULL);
                        ZCCALL(ZM_ScrollBar_SetScrollPos, p->m_pVerticalScrollBar, 0, NULL);
                    }
                    if ((ZuiAny)iScrollPos > ZCCALL(ZM_ScrollBar_GetScrollPos, p->m_pVerticalScrollBar, NULL, NULL)) {
                        ZCCALL(ZM_SetPos, cp, &cp->m_rcItem, FALSE);
                    }
                }
            }
            break;
        }
        break;
    }
    case ZM_OnCreate: {
        p = (ZuiLayout)malloc(sizeof(ZLayout));
        memset(p, 0, sizeof(ZLayout));
        //保存原来的回调地址,创建成功后回调地址指向当前函数
        p->old_call = cp->call;
        p->m_bMouseChildEnabled = TRUE;
        p->m_iChildPadding = 0;
        p->m_items = darray_create();
        return p;
    }
    case ZM_OnDestroy: {
        for (int it = darray_len(p->m_items) - 1; it >= 0; it--) {
            ZCCALL(ZM_OnDestroy, p->m_items->data[it], (ZuiAny)TRUE, Param2);
        }
        ZCtlProc old_call = p->old_call;
        if (p->m_pHorizontalScrollBar)
            FreeZuiControl(p->m_pHorizontalScrollBar, FALSE);
        if (p->m_pVerticalScrollBar)
            FreeZuiControl(p->m_pVerticalScrollBar, FALSE);
        darray_destroy(p->m_items);

        old_call(ProcId, cp, 0, Param1, Param2);
        free(p);
        return 0;
    }
    case ZM_GetObject:
        if (_tcsicmp(Param1, (ZuiAny)Type_Layout) == 0)
            return (ZuiAny)p;
        break;
    case ZM_GetType:
        return (ZuiAny)Type_Layout;
    case ZM_CoreInit:
        return (ZuiAny)TRUE;
    case ZM_CoreUnInit:
        return NULL;
    default:
        break;
    }
    return p->old_call(ProcId, cp, 0, Param1, Param2);
}


