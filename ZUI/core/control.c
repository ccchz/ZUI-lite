#include "control.h"
#include "tree.h"
#include "function.h"
#include "resdb.h"
#include "template.h"
#include "builder.h"
#include <core/Register.h>
#include <platform/platform.h>

ZuiNotifyMsg zMsg = NULL;
int zMsg_pos = 0;     //通知消息位置

ZEXPORT ZuiBool ZCALL ZuiNotiyMsgInit() {
    zMsg = malloc(sizeof(ZNotifyMsg) * ZMSG_LEN);
    if (zMsg) {
        memset(zMsg, 0, sizeof(ZNotifyMsg) * ZMSG_LEN);
        return TRUE;
    }
    return FALSE;
}

ZEXPORT ZuiBool ZCALL ZuiNotiyMsgUnInit() {
    if (zMsg) {
        free(zMsg);
    }
    return TRUE;
}

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
 /*           ZuiControl pParent = p;
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
            }*/
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
            if(!Param2)
                ZuiControlNeedParentUpdate(p);
        }
        break;
    }
    case ZM_GetParent: {
        return p->m_pParent;
    }
    case ZM_GetVisible: {
        return (ZuiAny)p->m_bVisible;
    }
    case ZM_SetText: {
        CONTROL_SETSTR(p->m_sText, Param1,ZuiText);
        if (!Param2)
            ZuiControlInvalidate(p, TRUE);
        break;
    }
    case ZM_GetText: {
        return p->m_sText;
    }
    case ZM_SetName: {
        CONTROL_SETSTR(p->m_sName, Param1, ZuiText);
        break;
    }
    case ZM_GetName: {
        return p->m_sName;
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
        ZuiBool bSize = FALSE;
        if (rc->right < rc->left) rc->right = rc->left;
        if (rc->bottom < rc->top) rc->bottom = rc->top;

        //防止不必要的调用
        if (rc->right - rc->left != p->m_rcItem.right - p->m_rcItem.left ||
            rc->bottom - rc->top != p->m_rcItem.bottom - p->m_rcItem.top)
            bSize = TRUE;

            memcpy(&p->m_rcItem, rc, sizeof(ZRect));
        //}
        if (p->m_pOs == NULL)
            return 0;

        if (!p->m_bSetPos && bSize) {
            p->m_bSetPos = TRUE;
            ZCCALL(ZM_OnSize, p, Param2,(ZuiAny)(MAKEPARAM((p->m_rcItem.right - p->m_rcItem.left),(p->m_rcItem.bottom - p->m_rcItem.top))));
            p->m_bSetPos = FALSE;
        }

        p->m_bUpdateNeeded = FALSE;
        break;
    }
    case ZM_OnSize: {
        if (p->m_aAnime)
            p->m_aAnime->OnSize(p, Param1, Param2);
        ZuiControlNotify(ZM_OnSize, p, Param1, Param2);
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
        //case ZEVENT_SETCURSOR:
        //{
        //    ZuiOsSetCursor((unsigned int)ZIDC_ARROW);
        //    return 0;
        //}
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
            ZuiControlDelayedNotify(ZM_OnKillFocus, p, NULL, NULL);
            return 0;
        }
        case ZEVENT_MOUSELEAVE: {
            ZuiControlDelayedNotify(ZM_OnMouseLeave, p, NULL, NULL);
            break;
	    }
        case ZEVENT_MOUSEENTER: {
            ZuiControlDelayedNotify(ZM_OnMouseEnter, p, (ZuiAny)(((TEventUI *)Param1)->ptMouse.x), (ZuiAny)(((TEventUI *)Param1)->ptMouse.y));
            break;
	    }
        case ZEVENT_LBUTTONDOWN: {
            ZuiControlDelayedNotify(ZM_OnLButtonDown, p, (ZuiAny)(((TEventUI*)Param1)->ptMouse.x), (ZuiAny)(((TEventUI*)Param1)->ptMouse.y));
            break;
	    }
        case ZEVENT_LDBLCLICK: {
            ZuiControlDelayedNotify(ZM_OnDbClick, p, (ZuiAny)(((TEventUI*)Param1)->ptMouse.x), (ZuiAny)(((TEventUI*)Param1)->ptMouse.y));
            break;
        }
        case ZEVENT_LBUTTONUP: {
            if (ZuiIsPointInRect(&p->m_rcItem, &((TEventUI*)Param1)->ptMouse)) {
                ZuiControlDelayedNotify(ZM_OnClick, p, (ZuiAny)(((TEventUI*)Param1)->ptMouse.x), (ZuiAny)(((TEventUI*)Param1)->ptMouse.y));
            }
            break;
        }
        case ZEVENT_CHAR: {
            ZuiControlDelayedNotify(ZM_OnChar, p, &((TEventUI*)Param1)->wParam, NULL);
            break;
        }
        default:
            break;
        }
        if (p->m_pParent != NULL && p->m_bNotifyPop)
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
        if (!Param2)
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
        if (!Param2)
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
        if (!Param2)
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
        if (!Param2)
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
        if (!Param2)
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
        if (!Param2)
            ZuiControlNeedParentUpdate(p);
        break;
    }
    case ZM_GetPadding: {
        return (void *)&p->m_rcPadding;
        break;
    }
    case ZM_SetPadding: {
        memcpy(&p->m_rcPadding, Param1, sizeof(ZRect));
        if (!Param2)
            ZuiControlNeedParentUpdate(p);
        break;
    }
    case ZM_GetFixedXY: {
        return (void *)&p->m_cXY;
        break;
    }
    case ZM_SetFixedXY: {
        p->m_cXY.cx = ((ZuiRect)Param1)->left;
        p->m_cXY.cy = ((ZuiRect)Param1)->top;
        if (!Param2)
            ZuiControlNeedParentUpdate(p);
        break;
    }
    case ZM_SetRound: {
        memcpy(&p->m_rRound, Param1, sizeof(ZRound));
        if (!Param2)
            ZuiControlInvalidate(p,TRUE);
        break;
    }
    case ZM_GetRound: {
        return &p->m_rRound;
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
        if (!Param2)
            ZuiControlNeedParentUpdate(p);
        break;
    }
    case ZM_SetEnabled: {
        if (p->m_bEnabled == (ZuiBool)Param1)
            return 0;

        p->m_bEnabled = (ZuiBool)Param1;
        if (!Param2)
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
    case ZM_SetNotifyPop: {
        p->m_bNotifyPop = (ZuiBool)Param1;
        break;
    }
    case ZM_SetMouseEnabled: {
        p->m_bMouseEnabled = (ZuiBool)Param1;
        break;
    }
    case ZM_EstimateSize: {
        return (void *)&p->m_cxyFixed;
    }
    case ZM_FindControl: {
        FINDCONTROLPROC findProc = __FindControlsFromUpdate;

        if ((int)Param2 & ZFIND_FROM_UPDATE)
            findProc = __FindControlsFromUpdate;
        else if ((int)Param2 & ZFIND_FROM_TAB)
            findProc = __FindControlFromTab;
        else if ((int)Param2 & ZFIND_FROM_POINT)
            findProc = __FindControlFromPoint;
        else if ((int)Param2 & ZFIND_FROM_NAME)
            findProc = __FindControlFromName;

        if (((unsigned int)Param2 & ZFIND_VISIBLE) != 0 && !p->m_bVisible)
            return NULL;
        if (((unsigned int)Param2 & ZFIND_ENABLED) != 0 && !p->m_bEnabled)
            return NULL;//激活
        if (((unsigned int)Param2 & ZFIND_HITTEST) != 0 && !p->m_bMouseEnabled)
            return NULL;
        if (((unsigned int)Param2 & ZFIND_UPDATETEST) != 0 && ((FINDCONTROLPROC)findProc)(p, Param1) != NULL)
            return NULL;
        return ((FINDCONTROLPROC)findProc)(p, Param1);
        break;
    }
    case ZM_SetBkColor: {
            p->m_BkgColor = (ZuiColor)Param1;
            if (!Param2)
                ZuiControlInvalidate(p, TRUE);
            break;
     }
    case ZM_SetBorderColor: {
        if (!p->m_dwBorderColor && !p->m_dwBorderWidth)
            p->m_dwBorderWidth++;
        p->m_dwBorderColor = (ZuiColor)Param1;
        if (!Param2)
            ZuiControlInvalidate(p, TRUE);
        break;
    }
    case ZM_SetBorderColor2: {
        if (!p->m_dwBorderColor2 && !p->m_dwBorderWidth)
            p->m_dwBorderWidth++;
        p->m_dwBorderColor2 = (ZuiColor)Param1;
        if (!Param2)
            ZuiControlInvalidate(p, TRUE);
        break;
    }
    case ZM_SetBkImage: {
        if (p->m_BkgImg)
            ZuiResDBDelRes(p->m_BkgImg);
        p->m_BkgImg = Param1;
        if (!Param2)
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
        ZRect *rc = (ZRect *)&p->m_rcItem;
        if (p->m_BkgColor) {
                ZuiDrawFillRoundRect(p, p->m_BkgColor, rc, &p->m_rRound);
        }
        break;
    }
    case ZM_OnPaintBkImage: {
        ZRect *rc = &p->m_rcItem;
        if (p->m_BkgImg) {
            ZuiImage img = p->m_BkgImg->p;
            ZuiDrawImageEx(p, img, rc->left, rc->top, rc->right, rc->bottom, 255);
        }
        break;
    }
    case ZM_OnPaintBorder: {
        ZRect *rc = &p->m_rcItem;
	if (p->m_dwBorderColor) {
	    if(p->m_dwBorderWidth)
                ZuiDrawRoundRect(p, p->m_dwBorderColor, rc, &p->m_rRound, p->m_dwBorderWidth);
	    else
                ZuiDrawRoundRect(p, p->m_dwBorderColor, rc, &p->m_rRound, 1);
	    }
        break;
    }
    case ZM_OnCreate: {
        ZuiControl p = (ZuiControl)malloc(sizeof(ZControl));
        if (p) {
            memset(p, 0, sizeof(ZControl));
            p->m_bNotifyPop = TRUE;
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
        ZuiAttribute zAttr = (ZuiAttribute)Param1;
        if (_tcsicmp(zAttr->name, _T("text")) == 0)
		    ZCCALL(ZM_SetText, p, zAttr->value, Param2);
        else if (_tcsicmp(zAttr->name, _T("tooltip")) == 0)
		    ZCCALL(ZM_SetTooltip, p, zAttr->value, Param2);
        else if (_tcsicmp(zAttr->name, _T("width")) == 0)
    		ZCCALL(ZM_SetFixedWidth, p, (ZuiAny)(_ttoi(zAttr->value)), Param2);
	    else if (_tcsicmp(zAttr->name, _T("borderwidth")) == 0)
	    	ZCCALL(ZM_SetBorderWidth, p, (ZuiAny)(_ttoi(zAttr->value)), Param2);
        else if (_tcsicmp(zAttr->name, _T("height")) == 0)
		    ZCCALL(ZM_SetFixedHeight, p, (ZuiAny)(_ttoi(zAttr->value)), Param2);
        else if (_tcsicmp(zAttr->name, _T("minwidth")) == 0)
		    ZCCALL(ZM_SetMinWidth, p, (ZuiAny)(_ttoi(zAttr->value)), Param2);
        else if (_tcsicmp(zAttr->name, _T("minheight")) == 0)
		    ZCCALL(ZM_SetMinHeight, p, (ZuiAny)(_ttoi(zAttr->value)), Param2);
        else if (_tcsicmp(zAttr->name, _T("maxwidth")) == 0)
		    ZCCALL(ZM_SetMaxWidth, p, (ZuiAny)(_ttoi(zAttr->value)), Param2);
        else if (_tcsicmp(zAttr->name, _T("maxheight")) == 0)
		    ZCCALL(ZM_SetMaxHeight, p, (ZuiAny)(_ttoi(zAttr->value)), Param2);
        else if (_tcsicmp(zAttr->name, _T("round")) == 0) {
            ZuiText pstr = NULL;
            ZRound rd = { 0 };
            rd.left = _tcstol(zAttr->value, &pstr, 10);  ASSERT(pstr);
            rd.top = _tcstol(pstr + 1, &pstr, 10);    ASSERT(pstr);
            rd.right = _tcstol(pstr + 1, &pstr, 10);  ASSERT(pstr);
            rd.bottom = _tcstol(pstr + 1, &pstr, 10); ASSERT(pstr);
            ZCCALL(ZM_SetRound, p, (ZuiAny)&rd, Param2);
        }
        else if (_tcsicmp(zAttr->name, _T("bkcolor")) == 0) {
			ZuiColor clrColor;
			clrColor = ZuiStr2Color(zAttr->value);
            ZCCALL(ZM_SetBkColor, p, (ZuiAny)clrColor, Param2);
        }
        else if (_tcsicmp(zAttr->name, _T("drag")) == 0)
		    ZCCALL(ZM_SetDrag, p, (ZuiAny)(_tcsicmp(zAttr->value, _T("true")) == 0 ? TRUE : FALSE), Param2);
        else if (_tcsicmp(zAttr->name, _T("mouseenable")) == 0)
            ZCCALL(ZM_SetMouseEnabled, p, (ZuiAny)(_tcsicmp(zAttr->value, _T("true")) == 0 ? TRUE : FALSE), Param2);
        else if (_tcsicmp(zAttr->name, _T("bkimage")) == 0)
		    ZCCALL(ZM_SetBkImage, p, ZuiResDBGetRes(zAttr->value, ZREST_IMG), Param2);
        else if (_tcsicmp(zAttr->name, _T("padding")) == 0) {
            ZRect rcPadding = { 0 };
            ZuiText pstr = NULL;
            rcPadding.left = _tcstol(zAttr->value, &pstr, 10);  ASSERT(pstr);
            rcPadding.top = _tcstol(pstr + 1, &pstr, 10);    ASSERT(pstr);
            rcPadding.right = _tcstol(pstr + 1, &pstr, 10);  ASSERT(pstr);
            rcPadding.bottom = _tcstol(pstr + 1, &pstr, 10); ASSERT(pstr);
            ZCCALL(ZM_SetPadding, p, &rcPadding, Param2);
        }
        else if (_tcsicmp(zAttr->name, _T("bordercolor")) == 0) {
			ZuiColor clrColor;
			clrColor = ZuiStr2Color(zAttr->value);
            ZCCALL(ZM_SetBorderColor, p, (ZuiAny)clrColor, Param2);
        }
        else if (_tcsicmp(zAttr->name, _T("bordercolor2")) == 0) {
            ZuiColor clrColor;
            clrColor = ZuiStr2Color(zAttr->value);
            ZCCALL(ZM_SetBorderColor2, p, (ZuiAny)clrColor, Param2);
        }
        else if (_tcsicmp(zAttr->name, _T("name")) == 0)
		    ZCCALL(ZM_SetName, p, zAttr->value, Param2);
        else if (_tcsicmp(zAttr->name, _T("float")) == 0) {
            if (_tcschr(zAttr->value, ',') == 0) {
                ZCCALL(ZM_SetFloat, p, (ZuiAny)(_tcsicmp(zAttr->value, _T("true")) == 0 ? TRUE : FALSE), Param2);
            }
            else {
                ZRectR piFloatPercent = { 0 };
                ZuiText pstr = NULL;
                piFloatPercent.left = _tcstof(zAttr->value, &pstr);  ASSERT(pstr);
                if (*pstr == _T('%'))
                    { piFloatPercent.left /= 100; pstr++; }
                piFloatPercent.top = _tcstof(pstr + 1, &pstr);    ASSERT(pstr);
                if (*pstr == _T('%'))
                    { piFloatPercent.top /= 100; pstr++; }
                piFloatPercent.right = _tcstof(pstr + 1, &pstr);  ASSERT(pstr);
                if (*pstr == _T('%'))
                    { piFloatPercent.right /= 100; pstr++; }
                piFloatPercent.bottom = _tcstof(pstr + 1, &pstr); ASSERT(pstr);
                if (*pstr == _T('%'))
                    { piFloatPercent.bottom /= 100; pstr++; }
                ZCCALL(ZM_SetFloatPercent, p, &piFloatPercent, Param2);
                ZCCALL(ZM_SetFloat, p, (ZuiAny)TRUE, Param2);
            }
        }
        else if (_tcsicmp(zAttr->name, _T("visible")) == 0)
		    ZCCALL(ZM_SetVisible, p, (ZuiAny)(_tcsicmp(zAttr->value, _T("true")) == 0 ? TRUE : FALSE), Param2);
        else if (_tcsicmp(zAttr->name, _T("pos")) == 0) {
            ZRect rcPos = { 0 };
            ZuiText pstr = NULL;
            rcPos.left = _tcstol(zAttr->value, &pstr, 10);  ASSERT(pstr);
            rcPos.top = _tcstol(pstr + 1, &pstr, 10);    ASSERT(pstr);
            rcPos.right = _tcstol(pstr + 1, &pstr, 10);  ASSERT(pstr);
            rcPos.bottom = _tcstol(pstr + 1, &pstr, 10); ASSERT(pstr);
            ZCCALL(ZM_SetFixedXY, p, (ZuiAny)&rcPos, Param2);
            ZCCALL(ZM_SetFixedWidth, p, (ZuiAny)(rcPos.right - rcPos.left), Param2);
            ZCCALL(ZM_SetFixedHeight, p, (ZuiAny)(rcPos.bottom - rcPos.top), Param2);
        }
        else if (_tcsicmp(zAttr->name, _T("enabled")) == 0)
		    ZCCALL(ZM_SetEnabled, p, (ZuiAny)(_tcsicmp(zAttr->value, _T("true")) == 0 ? TRUE : FALSE), Param2);
        else if (_tcsicmp(zAttr->name, _T("notifypop")) == 0)
            ZCCALL(ZM_SetNotifyPop, p, (ZuiAny)(_tcsicmp(zAttr->value, _T("true")) == 0 ? TRUE : FALSE), Param2);
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

ZEXPORT ZuiAny ZCALL ZuiControlDelayedNotify(int msg, ZuiControl p, ZuiAny Param1, ZuiAny Param2) {
    if (p->m_pNotify) {
        ZuiNotifyMsg msgp = zMsg + zMsg_pos;
        msgp->msg = msg;
        msgp->p = p;
        msgp->Param1 = Param1;
        msgp->param2 = Param2;
        ZuiOsPostMessage(p, (ZuiAny)(ZM_APP + 2), (ZuiAny)zMsg_pos, 0);
        zMsg_pos++;
        zMsg_pos %= ZMSG_LEN;
        return 0;
    }
    return (ZuiAny)-1;
}
ZEXPORT ZuiAny ZCALL ZuiControlNotify(int msg, ZuiControl p, ZuiAny Param1, ZuiAny Param2) {
    if (p->m_pNotify)
    {
        return p->m_pNotify(msg, p, Param1, Param2);
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
    if (p) {
        p->m_bUpdateNeeded = TRUE;
        ZCCALL(ZM_SetPos, p, &p->m_rcItem, (ZuiAny)ZuiOnSize);
        ZuiControlInvalidate(p, TRUE);
    }
}

ZEXPORT ZuiVoid ZCALL ZuiControlNeedParentUpdate(ZuiControl p)
{
    if (p) {
       ZuiControlNeedUpdate(p->m_pParent);
    }
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