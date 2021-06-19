﻿#include "control.h"
#include "tree.h"
#include "function.h"
#include "resdb.h"
#include "template.h"
#include "builder.h"
#include <core/Register.h>
#include <platform/platform.h>
//创建控件
ZEXPORT ZuiControl ZCALL NewZuiControl(ZuiText classname, ZuiAny Param1, ZuiAny Param2) {
    ZuiControl p = ZuiDefaultControlProc(ZM_OnCreate,0,0,0,0);
    if (p) {
        //通知重载的对象开始创建
        //没有重载的
        /*查找类名*/
        ZText name[256];
        int len = (int)_tcslen(classname);
        if (len > 255)
            return p;
        memset(name, 0, sizeof(name));
        memcpy(name, classname, len * sizeof(ZText));
        _tcslwr(name);

        ZClass theNode = { 0 };
        ZClass *node;
        theNode.key = Zui_Hash(name);
        node = RB_FIND(_ZClass_Tree, Global_ControlClass, &theNode);
        if (node) {
            p->m_sUserData = node->cb(ZM_OnCreate, p, 0, Param1, Param2);
            p->call = node->cb;
            return p;
        }
        //在模版内查找
        ZTemplate theTemp = { 0 };
        ZTemplate *temp;
        theTemp.key = Zui_Hash(name);
        temp = RB_FIND(_ZTemplate_Tree, Global_TemplateClass, &theTemp);
        if (temp) {
            ZuiLoadTemplate(temp->node, p, Param1, Param2);
        }
        return p;
    }
    return NULL;
}
//销毁控件
ZEXPORT void ZCALL FreeZuiControl(ZuiControl p, ZuiBool Delayed)
{
    if (!Delayed)
        ZCCALL(ZM_OnDestroy, p, NULL, NULL);
    else
        ZuiOsAddDelayedCleanup(p,0,0);
}

//控件默认处理函数
ZEXPORT ZuiAny ZCALL ZuiDefaultControlProc(int ProcId, ZuiControl p, ZuiAny UserData, ZuiAny Param1, ZuiAny Param2) {
    switch (ProcId)
    {
    case ZM_Invalidate: {
        ZRect invalidateRc = p->m_rcItem;
        if (!p->m_bVisible)
		    return 0;
	    else
	    {
            ZuiControl pParent = p;
            ZRect rcTemp;
            ZRect *rcParent;
            while (pParent = pParent->m_pParent)
            {
                rcTemp = invalidateRc;
                rcParent = (ZRect *)ZCCALL(ZM_GetPos, pParent, NULL, NULL);
                if (!IntersectRect((LPRECT)&invalidateRc, (const RECT *)&rcTemp, (const RECT *)rcParent))
                {
                    continue;
                }
            }
            //重置动画
            if (Param1 && p->m_aAnime)
                p->m_aAnime->steup = 0;
            if (p->m_pOs != NULL) ZuiOsInvalidateRect(p->m_pOs, &invalidateRc);
        }
        return 0;
    }
    case ZM_Activate: {
        if (!p->m_bVisible)
		return FALSE;
        if (!p->m_bEnabled)
		return FALSE;
    }
    case ZM_SetVisible: {
        BOOL v = p->m_bVisible;
        if (p->m_bVisible == (BOOL)Param1)
            return 0;
        p->m_bVisible = (BOOL)Param1;
        if (p->m_bFocused)
            p->m_bFocused = FALSE;
        if (!(BOOL)Param1 && p->m_pOs && p->m_pOs->m_pFocus == p) {
            ZuiOsSetFocus(p->m_pOs, NULL, TRUE);
        }
        if (p->m_bVisible != v) {
            ZuiControlNeedParentUpdate(p);
        }
        break;
    }
    case ZM_GetVisible: {
        return (ZuiAny)p->m_bVisible;
    }
    case ZM_SetText: {
        ZuiControlInvalidate(p, TRUE);
        CONTROL_SETSTR(p->m_sText, Param1,ZuiText);
        break;
    }
    case ZM_GetText: {
        return p->m_sText;
    }
    case ZM_SetName: {
        CONTROL_SETSTR(p->m_sName, Param1, ZuiText);
        break;
    }
    case ZM_SetTooltip: {
        CONTROL_SETSTR(p->m_sToolTip, Param1, ZuiText);
        break;
    }
    case ZM_GetPos: {
        return (void *)&p->m_rcItem;
        break;
    }
    case ZM_SetPos: {// 只有控件为float的时候，外部调用SetPos和Move才是有效的，位置参数是相对父控件的位置
        ZRect *rc = (ZRect *)Param1;
        BOOL bNeedInvalidate = (BOOL)Param2;
        ZRect invalidateRc = p->m_rcItem;
        ZuiBool bSize = FALSE;
        if (rc->right < rc->left) rc->right = rc->left;
        if (rc->bottom < rc->top) rc->bottom = rc->top;

        if (IsRectEmpty((RECT *)&invalidateRc)) invalidateRc = *rc;
        //防止不必要的调用
        if (rc->right - rc->left != p->m_rcItem.right - p->m_rcItem.left ||
            rc->bottom - rc->top != p->m_rcItem.bottom - p->m_rcItem.top)
            bSize = TRUE;

        if (p->m_bFloat) {
            ZuiControl pParent = p->m_pParent;
            if (pParent != NULL) {
                ZRect *rcParentPos = (ZRect *)ZCCALL(ZM_GetPos, pParent, NULL, NULL);
                ZRect rcCtrl = { rcParentPos->left + rc->left, rcParentPos->top + rc->top,
                    rcParentPos->left + rc->right, rcParentPos->top + rc->bottom };
                p->m_rcItem = rcCtrl;

/*                int width = rcParentPos->right - rcParentPos->left;
                int height = rcParentPos->bottom - rcParentPos->top;
                ZRect rcPercent = { (int)(width*p->m_piFloatPercent.left), (int)(height*p->m_piFloatPercent.top),
                    (int)(width*p->m_piFloatPercent.right), (int)(height*p->m_piFloatPercent.bottom) };
                p->m_cXY.cx = rc->left - rcPercent.left;
                p->m_cXY.cy = rc->top - rcPercent.top;
                p->m_cxyFixed.cx = rc->right - rcPercent.right - p->m_cXY.cx;
                p->m_cxyFixed.cy = rc->bottom - rcPercent.bottom - p->m_cXY.cy; */
            }
        }
        else {
            memcpy(&p->m_rcItem, rc, sizeof(ZRect));
        }

        //wprintf(_T("%s:%d,%d,%d,%d\n", p->m_sName, rc->left, rc->top, rc->right, rc->bottom);

        if (p->m_pOs == NULL)
            return 0;

        if (!p->m_bSetPos && bSize) {
            p->m_bSetPos = TRUE;
            ZCCALL(ZM_OnSize, p, (ZuiAny)-1,(ZuiAny)(MAKEPARAM((p->m_rcItem.right - p->m_rcItem.left),(p->m_rcItem.bottom - p->m_rcItem.top))));
            p->m_bSetPos = FALSE;
        }

        p->m_bUpdateNeeded = FALSE;

        if (bNeedInvalidate && p->m_bVisible) {
            Rect_Join(&invalidateRc, &p->m_rcItem);
            ZuiControl pParent = p;
            ZRect rcTemp;
            ZRect *rcParent;
            while (pParent = pParent->m_pParent) {
                if (!pParent->m_bVisible)
                    return 0;
                rcTemp = invalidateRc;
                rcParent = (ZRect *)ZCCALL(ZM_GetPos, pParent, NULL, NULL);
                if (!IntersectRect((LPRECT)&invalidateRc, (const RECT *)&rcTemp, (const RECT *)rcParent))
                    return 0;
            }
            ZuiOsInvalidateRect(p->m_pOs, &invalidateRc);
        }
        break;
    }
    case ZM_OnSize: {
        if (p->m_aAnime)
            p->m_aAnime->OnSize(p, Param1, Param2);
        ZuiControlNotify(_T("onsize"), p, Param1, Param2);
        break;
    }
    case ZM_SetOs: {
        p->m_pOs = ((ZuiControl)Param1)->m_pOs;
        p->m_pParent = (ZuiControl)Param1;
        if (Param2 && p->m_pParent)
            ZCCALL(ZM_OnInit, p, NULL, NULL);
        break;
    }
    case ZM_OnEvent: {
        switch (((TEventUI *)Param1)->Type)
        {
        case ZEVENT_SETCURSOR:
        {
            ZuiOsSetCursor((unsigned int)ZDC_ARROW);
            return 0;
        }
        case ZEVENT_SETFOCUS:
        {
            p->m_bFocused = TRUE;
            ZuiControlInvalidate(p, TRUE);
            return 0;
        }
        case ZEVENT_KILLFOCUS:
        {
            p->m_bFocused = FALSE;
            ZuiControlInvalidate(p, TRUE);
            return 0;
        }
        case ZEVENT_MOUSELEAVE: {
            ZuiControlNotify(_T("onmouseleave"), p, NULL, NULL);
            break;
	}
        case ZEVENT_MOUSEENTER: {
            ZuiControlNotify(_T("onmouseenter"), p, (ZuiAny)(((TEventUI *)Param1)->ptMouse.x), (ZuiAny)(((TEventUI *)Param1)->ptMouse.y));
            break;
	}
        case ZEVENT_LBUTTONDOWN: {
            ZuiControlNotify(_T("onlbuttondown"), p, (ZuiAny)(((TEventUI *)Param1)->ptMouse.x), (ZuiAny)(((TEventUI *)Param1)->ptMouse.y));
            break;
	}
        case ZEVENT_LBUTTONUP: {
            if (ZuiIsPointInRect(&p->m_rcItem, &((TEventUI*)Param1)->ptMouse)) {
                ZuiControlNotify(_T("onclick"), p, (ZuiAny)(((TEventUI*)Param1)->ptMouse.x), (ZuiAny)(((TEventUI*)Param1)->ptMouse.y));
            }
            break;
        }
        case ZEVENT_CHAR: {
            ZuiControlNotify(_T("onchar"), p, &((TEventUI*)Param1)->wParam, NULL);
            break;
        }
        case ZEVENT_WINDOWSIZE:
        {
            ZCCALL(ZM_OnSize, p, (ZuiAny)((TEventUI*)Param1)->wParam, (ZuiAny)((TEventUI*)Param1)->lParam);
            return 0;
        }
        default:
            break;
        }
        if (p->m_pParent != NULL)
            ZCCALL(ZM_OnEvent, p->m_pParent, Param1, NULL);
        if (p->m_aAnime)
            p->m_aAnime->OnEvent(p, Param1);
        break;
    }
    case ZM_GetMinWidth: {
        return (void *)p->m_cxyMin.cx;
    }
    case ZM_SetMinWidth: {
        if (p->m_cxyMin.cx == (int)Param1)
            return 0;
        if ((int)Param1 < 0)
            return 0;
        p->m_cxyMin.cx = (int)Param1;
        ZuiControlNeedParentUpdate(p);
        break;
    }
    case ZM_GetBorderWidth: {
	return (void *)p->m_dwBorderWidth;
	break;
	}
    case ZM_SetBorderWidth: {
	if (p->m_dwBorderWidth == (int)Param1)
		return 0;
	if ((int)Param1 < 0)
		return 0;
	p->m_dwBorderWidth = (int)Param1;
	ZuiControlNeedParentUpdate(p);
	break;
    }
    case ZM_GetMaxWidth: {
        return (void *)p->m_cxyMax.cx;
        break;
    }
    case ZM_SetMaxWidth: {
        if (p->m_cxyMax.cx == (int)Param1)
            return 0;
        if ((int)Param1 < 0)
            return 0;
        p->m_cxyMax.cx = (int)Param1;
        ZuiControlNeedParentUpdate(p);
        break;
    }
    case ZM_GetMinHeight: {
        return (void *)p->m_cxyMin.cy;
        break;
    }
    case ZM_SetMinHeight: {
        if (p->m_cxyMin.cy == (int)Param1)
            return 0;
        if ((int)Param1 < 0)
            return 0;
        p->m_cxyMin.cy = (int)Param1;
        ZuiControlNeedParentUpdate(p);
        break;
    }
    case ZM_GetMaxHeight: {
        return (void *)p->m_cxyMax.cy;
        break;
    }
    case ZM_SetMaxHeight: {
        if (p->m_cxyMax.cy == (int)Param1)
            return 0;
        if ((int)Param1 < 0)
            return 0;
        p->m_cxyMax.cy = (int)Param1;
        ZuiControlNeedParentUpdate(p);
        break;
    }
    case ZM_GetWidth: {
        return (void *)(p->m_rcItem.right - p->m_rcItem.left);
        break;
    }
    case ZM_GetHeight: {
        return (void *)(p->m_rcItem.bottom - p->m_rcItem.top);
        break;
    }
    case ZM_GetX: {
        return (void *)p->m_rcItem.left;
        break;
    }
    case ZM_GetY: {
        return (void *)p->m_rcItem.top;
        break;
    }
    case ZM_SetFloatPercent: {
        memcpy(&p->m_piFloatPercent, Param1, sizeof(ZRectR));
        ZuiControlNeedParentUpdate(p);
        break;
    }
    case ZM_GetPadding: {
        return (void *)&p->m_rcPadding;
        break;
    }
    case ZM_SetPadding: {
        memcpy(&p->m_rcPadding, Param1, sizeof(ZRect));
        ZuiControlNeedParentUpdate(p);
        break;
    }
    case ZM_GetFixedXY: {
        return (void *)&p->m_cXY;
        break;
    }
    case ZM_SetFixedXY: {
        p->m_cXY.cx = (int)Param1;
        p->m_cXY.cy = (int)Param2;
        ZuiControlNeedParentUpdate(p);
        break;
    }
    case ZM_SetRound: {
        p->m_rRound.cx = (int)Param1;
        p->m_rRound.cy = (int)Param2;
        ZuiControlNeedParentUpdate(p);
        break;
    }
    case ZM_GetFixedWidth: {
        return (void *)p->m_cxyFixed.cx;
        break;
    }
    case ZM_SetFixedWidth: {
        if ((int)Param1 < 0)
            return 0;
        p->m_cxyFixed.cx = (int)Param1;
        if (!Param2)
            ZuiControlNeedParentUpdate(p);
        break;
    }
    case ZM_GetFixedHeight: {
        return (void *)p->m_cxyFixed.cy;
        break;
    }
    case ZM_SetFixedHeight: {
        if ((int)Param1 < 0)
            return 0;
        p->m_cxyFixed.cy = (int)Param1;
        if (!Param2)
            ZuiControlNeedParentUpdate(p);
        break;
    }
    case ZM_GetRelativePos: {
        ZuiControl pParent = p->m_pParent;
        if (pParent != NULL) {
            ZRect *rcParentPos = (ZRect *)ZCCALL(ZM_GetPos, pParent, NULL, NULL);
            memcpy(Param1,&p->m_rcItem,sizeof(ZRect));
            OffsetRect((LPRECT)Param1, -rcParentPos->left, -rcParentPos->top);
            return 0;
        }
        break;
    }
    case ZM_SetFloat: {
        if (p->m_bFloat == (BOOL)Param1)
            return 0;
        p->m_bFloat = (BOOL)Param1;
        ZuiControlNeedParentUpdate(p);
        break;
    }
    case ZM_SetEnabled: {
        if (p->m_bEnabled == (ZuiBool)Param1)
            return 0;

        p->m_bEnabled = (ZuiBool)Param1;
        ZuiControlInvalidate(p, TRUE);
        break;
    }
    case ZM_GetEnabled: {
        return (ZuiAny)p->m_bEnabled;
    }
    case ZM_SetFocus: {
        if (p->m_pOs != NULL)
            ZuiOsSetFocus(p->m_pOs, p, FALSE);
        break;
    }
    case ZM_SetDrag: {
        if (p->m_drag == (ZuiBool)Param1)
            return 0;

        p->m_drag = (ZuiBool)Param1;
        break;
    }
    case ZM_EstimateSize: {
        return (void *)&p->m_cxyFixed;
    }
    case ZM_FindControl: {
        FINDCONTROLPROC findProc = __FindControlsFromUpdate;

        if ((int)Param2 & ZFIND_FROM_UPDATE) findProc = __FindControlsFromUpdate;
        else if ((int)Param2 & ZFIND_FROM_TAB) findProc = __FindControlFromTab;
        else if ((int)Param2 & ZFIND_FROM_POINT) findProc = __FindControlFromPoint;
        else if ((int)Param2 & ZFIND_FROM_NAME) findProc = __FindControlFromName;

        if (((unsigned int)Param2 & ZFIND_VISIBLE) != 0 && !p->m_bVisible)
            return NULL;
        if (((unsigned int)Param2 & ZFIND_ENABLED) != 0 && !p->m_bEnabled)
            return NULL;//激活
        if (((unsigned int)Param2 & ZFIND_HITTEST) != 0 && (!p->m_bMouseEnabled || !ZuiIsPointInRect(&p->m_rcItem, Param1)))
            return NULL;
        if (((unsigned int)Param2 & ZFIND_UPDATETEST) != 0 && ((FINDCONTROLPROC)findProc)(p, Param1) != NULL)
            return NULL;
        return ((FINDCONTROLPROC)findProc)(p, Param1);
        break;
    }
    case ZM_SetBkColor: {
        if ((int)Param1 & BK_Color) {
            p->m_BkgColor = (ZuiColor)Param2;
            ZuiControlInvalidate(p, TRUE);
            break;
        }
        else if ((int)Param1 & Border_Color) {
            if (!p->m_dwBorderColor && !p->m_dwBorderWidth)
                p->m_dwBorderWidth++;
            p->m_dwBorderColor = (ZuiColor)Param2;
            ZuiControlInvalidate(p, TRUE);
            break;
        }
        break;
    }
    case ZM_SetBkImage: {
        if (p->m_BkgImg)
            ZuiResDBDelRes(p->m_BkgImg);
        p->m_BkgImg = Param1;
        ZuiControlInvalidate(p, TRUE);
        break;
    }
    case ZM_OnPaint: {
        //开始绘制
        ZCCALL(ZM_OnPaintBkColor, p, Param1, Param2);
        ZCCALL(ZM_OnPaintBkImage, p, Param1, Param2);
        ZCCALL(ZM_OnPaintStatusImage, p, Param1, Param2);
        ZCCALL(ZM_OnPaintText, p, Param1, Param2);
        ZCCALL(ZM_OnPaintBorder, p, Param1, Param2);
        break;
    }
    case ZM_OnPaintBkColor: {
        ZuiGraphics gp = (ZuiGraphics)Param1;
        ZRect *rc = (ZRect *)&p->m_rcItem;
        if (p->m_BkgColor) {
            //if (p->m_rRound.cx)
                ZuiDrawFillRoundRect(gp, p->m_BkgColor, rc, p->m_rRound.cx, p->m_rRound.cy);
            //else
            //    ZuiDrawFillRect(gp, p->m_BkgColor, rc);
        }
        break;
    }
    case ZM_OnPaintBkImage: {
        ZuiGraphics gp = (ZuiGraphics)Param1;
        ZRect *rc = &p->m_rcItem;
        if (p->m_BkgImg) {
            ZuiImage img = p->m_BkgImg->p;
            ZuiDrawImageEx(gp, img, rc->left, rc->top, rc->right, rc->bottom, 0, 0, img->Width, img->Height, 255);
        }
        break;
    }
    case ZM_OnPaintBorder: {
        ZuiGraphics gp = (ZuiGraphics)Param1;
        ZRect *rc = &p->m_rcItem;
	if (p->m_dwBorderColor) {
	    if(p->m_dwBorderWidth)
                ZuiDrawRoundRect(gp, p->m_dwBorderColor, rc, p->m_rRound.cx, p->m_rRound.cy, p->m_dwBorderWidth);
	    else
                ZuiDrawRoundRect(gp, p->m_dwBorderColor, rc, p->m_rRound.cx, p->m_rRound.cy,1);
	    }
        break;
    }
    case ZM_OnCreate: {
        ZuiControl p = (ZuiControl)malloc(sizeof(ZControl));
        if (p) {
            memset(p, 0, sizeof(ZControl));
            p->m_pParent = NULL;
            p->m_bUpdateNeeded = TRUE;
            p->m_bVisible = TRUE;
            p->m_bFocused = FALSE;
            p->m_bEnabled = TRUE;
            p->m_bMouseEnabled = TRUE;
            p->m_bKeyboardEnabled = TRUE;
            p->m_bFloat = FALSE;
            p->m_bSetPos = FALSE;
            p->m_chShortcut = '\0';
            p->m_nTooltipWidth = 300;
            p->m_aAnime = NULL;

            p->m_cXY.cx = p->m_cXY.cy = 0;
            p->m_cxyFixed.cx = p->m_cxyFixed.cy = 0;
            p->m_cxyMin.cx = p->m_cxyMin.cy = 0;
            p->m_cxyMax.cx = p->m_cxyMax.cy = 9999;


            p->m_piFloatPercent.left = p->m_piFloatPercent.top = p->m_piFloatPercent.right = p->m_piFloatPercent.bottom = 0.0f;

            p->call = ZuiDefaultControlProc;
            return p;
        }
        return NULL;
    }
    case ZM_OnDestroy: {
        if (p->m_aAnime)
            ZuiAnimationFree(p->m_aAnime);
        if (p->m_sText)
            free(p->m_sText);
        if (p->m_sName)
            free(p->m_sName);
        if (p->m_sToolTip)
            free(p->m_sToolTip);
        if (p->m_BkgImg)
            ZuiResDBDelRes(p->m_BkgImg);
        if (p->m_pParent && !Param1)
            ZCCALL(ZM_Layout_Remove, p->m_pParent, p, (ZuiAny)TRUE);
        if (p->m_pOs != NULL)
            ZuiOsReapObjects(p->m_pOs, p);
        free(p);
        break;
    }
    case ZM_SetAttribute: {
        if (_tcsicmp(Param1, _T("text")) == 0)
		ZCCALL(ZM_SetText, p, Param2, NULL);
        else if (_tcsicmp(Param1, _T("tooltip")) == 0)
		ZCCALL(ZM_SetTooltip, p, Param2, NULL);
        else if (_tcsicmp(Param1, _T("width")) == 0)
		ZCCALL(ZM_SetFixedWidth, p, (ZuiAny)(_ttoi(Param2)), NULL);
	    else if (_tcsicmp(Param1, _T("borderwidth")) == 0)
		ZCCALL(ZM_SetBorderWidth, p, (ZuiAny)(_ttoi(Param2)), NULL);
        else if (_tcsicmp(Param1, _T("height")) == 0)
		ZCCALL(ZM_SetFixedHeight, p, (ZuiAny)(_ttoi(Param2)), NULL);
        else if (_tcsicmp(Param1, _T("minwidth")) == 0)
		ZCCALL(ZM_SetMinWidth, p, (ZuiAny)(_ttoi(Param2)), NULL);
        else if (_tcsicmp(Param1, _T("minheight")) == 0)
		ZCCALL(ZM_SetMinHeight, p, (ZuiAny)(_ttoi(Param2)), NULL);
        else if (_tcsicmp(Param1, _T("maxwidth")) == 0)
		ZCCALL(ZM_SetMaxWidth, p, (ZuiAny)(_ttoi(Param2)), NULL);
        else if (_tcsicmp(Param1, _T("maxheight")) == 0)
		ZCCALL(ZM_SetMaxHeight, p, (ZuiAny)(_ttoi(Param2)), NULL);
        else if (_tcsicmp(Param1, _T("round")) == 0) {
            ZuiText pstr = NULL;
            int cx = _tcstol(Param2, &pstr, 10);  ASSERT(pstr);
            int cy = _tcstol(pstr + 1, &pstr, 10);    ASSERT(pstr);
            ZCCALL(ZM_SetRound, p, (ZuiAny)cx, (ZuiAny)cy);
        }
        else if (_tcsicmp(Param1, _T("bkcolor")) == 0) {
			ZuiColor clrColor;
			clrColor = ZuiStr2Color(Param2);
            ZCCALL(ZM_SetBkColor, p, (ZuiAny)BK_Color,(ZuiAny)clrColor);
        }
        else if (_tcsicmp(Param1, _T("drag")) == 0)
		ZCCALL(ZM_SetDrag, p, (ZuiAny)(_tcsicmp(Param2, _T("true")) == 0 ? TRUE : FALSE), NULL);
        else if (_tcsicmp(Param1, _T("bkimage")) == 0)
		ZCCALL(ZM_SetBkImage, p, ZuiResDBGetRes(Param2, ZREST_IMG), NULL);
        else if (_tcsicmp(Param1, _T("padding")) == 0) {
            ZRect rcPadding = { 0 };
            ZuiText pstr = NULL;
            rcPadding.left = _tcstol(Param2, &pstr, 10);  ASSERT(pstr);
            rcPadding.top = _tcstol(pstr + 1, &pstr, 10);    ASSERT(pstr);
            rcPadding.right = _tcstol(pstr + 1, &pstr, 10);  ASSERT(pstr);
            rcPadding.bottom = _tcstol(pstr + 1, &pstr, 10); ASSERT(pstr);
            ZCCALL(ZM_SetPadding, p, &rcPadding, NULL);
        }
        else if (_tcsicmp(Param1, _T("bordercolor")) == 0) {
			ZuiColor clrColor;
			clrColor = ZuiStr2Color(Param2);
            ZCCALL(ZM_SetBkColor, p, (ZuiAny)Border_Color,(ZuiAny)clrColor);
        }
        else if (_tcsicmp(Param1, _T("name")) == 0)
		ZCCALL(ZM_SetName, p, Param2, NULL);
        else if (_tcsicmp(Param1, _T("float")) == 0) {
            if (_tcschr(Param2, ',') == 0) {
                ZCCALL(ZM_SetFloat, p, (ZuiAny)(_tcsicmp(Param2, _T("true")) == 0 ? TRUE : FALSE), NULL);
            }
            else {
                ZRectR piFloatPercent = { 0 };
                ZuiText pstr = NULL;
                piFloatPercent.left = _tcstof(Param2, &pstr);  ASSERT(pstr); if (*pstr == _T('%')) { piFloatPercent.left /= 100; pstr++; }
                piFloatPercent.top = _tcstof(pstr + 1, &pstr);    ASSERT(pstr); if (*pstr == _T('%')) { piFloatPercent.top /= 100; pstr++; }
                piFloatPercent.right = _tcstof(pstr + 1, &pstr);  ASSERT(pstr); if (*pstr == _T('%')) { piFloatPercent.right /= 100; pstr++; }
                piFloatPercent.bottom = _tcstof(pstr + 1, &pstr); ASSERT(pstr); if (*pstr == _T('%')) { piFloatPercent.bottom /= 100; pstr++; }
                ZCCALL(ZM_SetFloatPercent, p, &piFloatPercent, NULL);
                ZCCALL(ZM_SetFloat, p, (ZuiAny)TRUE, NULL);
            }
        }
        else if (_tcsicmp(Param1, _T("visible")) == 0)
		ZCCALL(ZM_SetVisible, p, (ZuiAny)(_tcsicmp(Param2, _T("true")) == 0 ? TRUE : FALSE), NULL);
        else if (_tcsicmp(Param1, _T("pos")) == 0) {
            ZRect rcPos = { 0 };
            ZuiText pstr = NULL;
            rcPos.left = _tcstol(Param2, &pstr, 10);  ASSERT(pstr);
            rcPos.top = _tcstol(pstr + 1, &pstr, 10);    ASSERT(pstr);
            rcPos.right = _tcstol(pstr + 1, &pstr, 10);  ASSERT(pstr);
            rcPos.bottom = _tcstol(pstr + 1, &pstr, 10); ASSERT(pstr);
            ZCCALL(ZM_SetFixedXY, p, (ZuiAny)rcPos.left, (ZuiAny)rcPos.top);
            ZCCALL(ZM_SetFixedWidth, p, (ZuiAny)(rcPos.right - rcPos.left), NULL);
            ZCCALL(ZM_SetFixedHeight, p, (ZuiAny)(rcPos.bottom - rcPos.top), NULL);
        }
        else if (_tcsicmp(Param1, _T("enabled")) == 0)
		ZCCALL(ZM_SetEnabled, p, (ZuiAny)(_tcsicmp(Param2, _T("true")) == 0 ? TRUE : FALSE), NULL);
        break;
    }
    case ZM_GetAttribute: {
        break;
    }
    case ZM_GetObject: {
        if (_tcsicmp(Param1, (ZuiAny)ZC_Null) == 0)
            return p;
        break;
    }
    case ZM_GetType: {
        return (ZuiAny)ZC_Null;
    }
    case ZM_CoreInit: {
        return (ZuiAny)TRUE;
    }
    case ZM_GetControlFlags: {
        return 0;
    }
    default:
        break;
    }
    return 0;
}

//调用控件函数
ZEXPORT ZuiAny ZCALL ZuiControlCall(int ProcId, ZuiControl p, ZuiAny Param1, ZuiAny Param2) {
    if (!p)
        return NULL;
    if (p->call)
    {
        return p->call(ProcId, p, p->m_sUserData, Param1, Param2);
    }
    return NULL;
}

ZEXPORT ZuiAny ZCALL ZuiControlNotify(ZuiText msg, ZuiControl p, ZuiAny Param1, ZuiAny Param2) {
    if (p->m_pNotify)
    {
        return p->m_pNotify(msg, p, p->m_sUserData, Param1, Param2);
    }
    return (ZuiAny)-1;
}

ZEXPORT ZuiVoid ZCALL ZuiControlRegNotify(ZuiControl p, ZNotifyProc pNotify) {
    if (p)
    {
        p->m_pNotify = pNotify;
    }
}


ZEXPORT ZuiControl ZCALL ZuiControlFindName(ZuiControl p, ZuiText Name) {
    if (!p)
        return NULL;
    if (!p->m_pOs)
        return NULL;
    if (!p->m_pOs->m_pRoot)
        return NULL;
    return (ZuiControl)ZCCALL(ZM_FindControl, p->m_pOs->m_pRoot, Name, (void *)(ZFIND_FROM_NAME | ZFIND_ALL));
}

ZEXPORT ZuiVoid ZCALL ZuiControlInvalidate(ZuiControl p, ZuiBool ResetAnimation)
{
    ZCCALL(ZM_Invalidate, p, (ZuiAny)ResetAnimation, NULL);
}

ZEXPORT ZuiVoid ZCALL ZuiControlNeedUpdate(ZuiControl p)
{
    if (!p->m_bVisible) return;
    p->m_bUpdateNeeded = TRUE;
    ZuiControlInvalidate(p, TRUE);
    if (p->m_pOs != NULL)
        p->m_pOs->m_bUpdateNeeded = TRUE;
}

ZEXPORT ZuiVoid ZCALL ZuiControlNeedParentUpdate(ZuiControl p)
{
    if (p->m_pParent) {
        ZuiControlNeedUpdate(p->m_pParent);
        ZuiControlInvalidate(p->m_pParent, TRUE);
    }
    else {
        ZuiControlNeedUpdate(p);
    }

    if (p->m_pOs != NULL)
        p->m_pOs->m_bUpdateNeeded = TRUE;
}

ZEXPORT ZuiVoid ZCALL ZuiControlEvent(ZuiControl p, TEventUI *event)
{
    ZCCALL(ZM_OnEvent, p, event, NULL);
}


ZuiVoid ZuiClientToScreen(ZuiControl p, ZuiPoint pt) {
    ZuiOsClientToScreen(p, pt);
}

ZuiVoid ZuiScreenToClient(ZuiControl p, ZuiPoint pt) {
    ZuiOsScreenToClient(p, pt);
}


//控件查找函数

ZuiControl ZCALL __FindControlFromCount(ZuiControl pThis, ZuiAny pData)
{
    int* pnCount = (int*)(pData);
    (*pnCount)++;
    return NULL;  // Count all controls
}

ZuiControl ZCALL __FindControlFromPoint(ZuiControl pThis, ZuiAny pData)
{
    return ZuiIsPointInRect(ZCCALL(ZM_GetPos, pThis, NULL, NULL), pData) ? pThis : NULL;
}

ZuiControl ZCALL __FindControlFromTab(ZuiControl pThis, ZuiAny pData)
{
    FINDTABINFO* pInfo = (FINDTABINFO*)(pData);
    if (pInfo->pFocus == pThis) {
        if (pInfo->bForward) pInfo->bNextIsIt = TRUE;
        return pInfo->bForward ? NULL : pInfo->pLast;
    }
    if (((int)ZCCALL(ZM_GetControlFlags, pThis, NULL, NULL) & ZFLAG_TABSTOP) == 0)
        return NULL;
    pInfo->pLast = pThis;
    if (pInfo->bNextIsIt) return pThis;
    if (pInfo->pFocus == NULL) return pThis;
    return NULL;  // Examine all controls
}

ZuiControl ZCALL __FindControlFromShortcut(ZuiControl pThis, ZuiAny pData)
{
    FINDSHORTCUT* pFS = (FINDSHORTCUT*)(pData);
    if (!pThis->m_bVisible) return NULL;
    if (pFS->ch == toupper(pThis->m_chShortcut)) pFS->bPickNext = TRUE;
    return pFS->bPickNext ? pThis : NULL;
}

ZuiControl ZCALL __FindControlsFromUpdate(ZuiControl pThis, ZuiAny pData)
{
    if (pThis->m_bUpdateNeeded) {
        darray_append(pThis->m_pOs->m_aFoundControls, (ZuiAny)pThis);
        return pThis;
    }
    return NULL;
}

ZuiControl ZCALL __FindControlFromName(ZuiControl pThis, ZuiAny pData)
{
    wchar_t* pstrName = (wchar_t *)(pData);
    if (!pThis->m_sName) return NULL;
    return (_tcsicmp(pThis->m_sName, pstrName) == 0) ? pThis : NULL;
}