#include "Button.h"
#include "Label.h"
#include <core/control.h>
#include <core/resdb.h>
#include <core/builder.h>
#include <platform/platform.h>
#include <core/function.h>
#include <stdlib.h>
ZEXPORT ZINT ZCALL ZuiButtonProc(ZINT ProcId, ZuiControl cp, ZuiButton p, ZPARAM Param1, ZPARAM Param2) {
    switch (ProcId)
    {
    case ZM_OnEvent: {
        TEventUI *event = (TEventUI *)Param1;
        switch (event->Type)
        {
	    case ZEVENT_KILLFOCUS: {
	        p->m_dwType = 0;
	        break;
	    }
        case ZEVENT_MOUSELEAVE: {
            p->m_dwType = 0;
            ZuiControlInvalidate(cp, TRUE);
            break;
	    }
        case ZEVENT_MOUSEENTER: {
            p->m_dwType = 1;
            ZuiControlInvalidate(cp, TRUE);
            break;
	    }
        case ZEVENT_LDBLCLICK:
        case ZEVENT_LBUTTONDOWN: {
            p->m_dwType = 2;
            ZuiControlInvalidate(cp, TRUE);
	        break;
        }
        case ZEVENT_LBUTTONUP: {
            p->m_dwType = 1;
            ZuiControlInvalidate(cp, TRUE);
	        break;
        }
        default:
            break;
        }
        break;
    }
    case ZM_OnPaintBorder: {
        ZRect* rc = (ZRect*)&cp->m_rcItem;
        if (cp->m_dwBorderWidth && !p->m_ResNormal && p->m_dwType > 0) {
            ZuiDrawRoundRect(cp, cp->m_dwBorderColor2, rc, &cp->m_rRound, cp->m_dwBorderWidth);
            return 0;
        }
        break;
    }
    case ZM_OnPaintStatusImage: {
        ZRect *rc = (ZRect *)&cp->m_rcItem;
        ZuiImage img;
        if (p->m_dwType == 0) {
            if (p->m_ResNormal) {
                img = p->m_ResNormal->p;
                ZuiDrawImageEx(cp, img, rc->left, rc->top, rc->right, rc->bottom, 255);
            }
            else {
                ZuiDrawFillRoundRect(cp, p->m_ColorNormal, rc, &cp->m_rRound);
            }
        }
        else if (p->m_dwType == 1) {
            if (p->m_ResHot) {
                img = p->m_ResHot->p;
                ZuiDrawImageEx(cp, img, rc->left, rc->top, rc->right, rc->bottom, 255);
            }
            else {
                ZuiDrawFillRoundRect(cp, p->m_ColorHot, rc, &cp->m_rRound);
            }
        }
        else if (p->m_dwType == 2) {
            if (p->m_ResPushed) {
                img = p->m_ResPushed->p;
                ZuiDrawImageEx(cp, img, rc->left, rc->top, rc->right, rc->bottom, 255);
            }
            else {
                ZuiDrawFillRoundRect(cp, p->m_ColorPushed, rc, &cp->m_rRound);
            }
        }
        else {
            if (p->m_ResDisabled) {
                img = p->m_ResDisabled->p;
                ZuiDrawImageEx(cp, img, rc->left, rc->top, rc->right, rc->bottom, 255);
            }
            else {
                ZuiDrawFillRoundRect(cp, p->m_ColorDisabled, rc, &cp->m_rRound);
            }
        }
        return 0;
    }
    case ZM_OnPaintText: { //按钮控件带图片绘制。
        if (p->m_ResForeground) {
            ZRect rcc = { 0 };
            ZRect* rc = (ZRect*)&cp->m_rcItem;
            ZuiImage img = p->m_ResForeground->p;
            ZuiLabel zb = p->old_udata;
            ZSizeR sr = {0};
            ZuiMeasureTextSize(cp, zb->m_rFont ? zb->m_rFont->p : Global_Font->p, cp->m_sText, -1, &sr);

            if (p->m_dwStyle == 1) { //图片、文本居中上下结构
                rcc.left = rc->left + ((rc->right - rc->left) - img->Width) / 2;
                if (sr.cy) {
                    rcc.top = rc->top + (int)((rc->bottom - rc->top) - img->Height - sr.cy - sepSize) / 2;
                }
                else { //没有文本时图片尺寸计算
                    rcc.top = rc->top + ((rc->bottom - rc->top) - img->Height) / 2;
                }
            }
            else if (p->m_dwStyle == 2) { //图片文本靠左
                rcc.top = rc->top + ((rc->bottom - rc->top) - img->Height) / 2;
                rcc.left = rc->left + p->m_rcImagePadding.left;
            }
            else { //默认  图片、文本居中左右结构
                if (sr.cx) {
                    rcc.left = rc->left + (int)((rc->right - rc->left) - img->Width - sr.cx - sepSize) / 2;
                }
                else { //没有文本时图片尺寸计算
                    rcc.left = rc->left + ((rc->right - rc->left) - img->Width) / 2;
                }
                rcc.top = rc->top + ((rc->bottom - rc->top) - img->Height) / 2;
            }
            rcc.right = rcc.left + img->Width;
            rcc.bottom = rcc.top + img->Height;
            ZuiIntersectRect(&rcc, rc);
            ZuiDrawImageEx(cp, img, rcc.left, rcc.top, rcc.right, rcc.bottom, 255);
            //绘制文本
            if (p->m_dwStyle == 1) { //图片、文本居中上下结构
                rcc.left = rc->left + cp->m_dwBorderWidth;
                rcc.top = rcc.bottom;
            }
            else if (p->m_dwStyle == 2) { //图片文本靠左
                rcc.left = rc->left + cp->m_dwBorderWidth + zb->m_rcPadding.left;
                rcc.top = rc->top + cp->m_dwBorderWidth;
            }
            else { //没有文本时图片尺寸计算
                rcc.left = rcc.right + sepSize;
                rcc.top = rc->top + cp->m_dwBorderWidth;
            }
            rcc.right = rc->right - cp->m_dwBorderWidth;
            rcc.bottom = rc->bottom - cp->m_dwBorderWidth;
            ZuiColor tmpTColor;
            if (!cp->m_sText)
                return 0;
            if (cp->m_bEnabled)
                tmpTColor = zb->m_cTextColor;
            else
                tmpTColor = zb->m_cTextColorDisabled;
            int tStyle;
            if (p->m_dwStyle == 1) {
                tStyle = ZDT_CENTER;
            }
            else if (p->m_dwStyle == 2) { //图片文本靠左
                tStyle = ZDT_VCENTER | ZDT_SINGLELINE;
            }
            else {
                tStyle = ZDT_VCENTER | ZDT_SINGLELINE;
            }
            ZuiDrawString(cp, zb->m_rFont ? zb->m_rFont->p : Global_Font->p,cp->m_sText, _tcslen(cp->m_sText), & rcc, tmpTColor, tStyle);
            return 0;
        }
        break;
    }
    case ZM_SetEnabled: {
        if (0 == (ZuiBool)Param1)
            p->m_dwType = -1;
        else
            p->m_dwType = 0;
        break;
    }
    case ZM_Button_SetResNormal: {
        if (p->m_ResNormal)
            ZuiResDBDelRes(p->m_ResNormal);
        p->m_ResNormal = (ZuiRes)Param1;
        if(!Param2)
            ZuiControlInvalidate(cp, TRUE);
        return 0;
    }
    case ZM_Button_SetResHot: {
        if (p->m_ResHot)
            ZuiResDBDelRes(p->m_ResHot);
        p->m_ResHot = (ZuiRes)Param1;
        if (!Param2)
            ZuiControlInvalidate(cp, TRUE);
        return 0;
    }
    case ZM_Button_SetResPushed: {
        if (p->m_ResPushed)
            ZuiResDBDelRes(p->m_ResPushed);
        p->m_ResPushed = (ZuiRes)Param1;
        if (!Param2)
            ZuiControlInvalidate(cp, TRUE);
        return 0;
    }
    case ZM_Button_SetResFocused: {
        if (p->m_ResFocused)
            ZuiResDBDelRes(p->m_ResFocused);
        p->m_ResFocused = (ZuiRes)Param1;
        if (!Param2)
            ZuiControlInvalidate(cp, TRUE);
        return 0;
    }
    case ZM_Button_SetResDisabled: {
        if (p->m_ResDisabled)
            ZuiResDBDelRes(p->m_ResDisabled);
        p->m_ResDisabled = (ZuiRes)Param1;
        if (!Param2)
            ZuiControlInvalidate(cp, TRUE);
        return 0;
    }
    case ZM_Button_SetResForeground: {
        if (p->m_ResForeground)
            ZuiResDBDelRes(p->m_ResForeground);
        p->m_ResForeground = (ZuiRes)Param1;
        if (!Param2)
            ZuiControlInvalidate(cp, TRUE);
        return 0;
    }
    case ZM_Button_SetColorNormal: {
        p->m_ColorNormal = (ZuiColor)Param1;
        if (!Param2)
            ZuiControlInvalidate(cp, TRUE);
        return 0;
    }
    case ZM_Button_SetColorHot: {
        p->m_ColorHot = (ZuiColor)Param1;
        if (!Param2)
            ZuiControlInvalidate(cp, TRUE);
        return 0;
    }
    case ZM_Button_SetColorPushed: {
        p->m_ColorPushed = (ZuiColor)Param1;
        if (!Param2)
            ZuiControlInvalidate(cp, TRUE);
        return 0;
    }
    case ZM_Button_SetColorFocused: {
        p->m_ColorFocused = (ZuiColor)Param1;
        if (!Param2)
            ZuiControlInvalidate(cp, TRUE);
        return 0;
    }
    case ZM_Button_SetColorDisabled: {
        p->m_ColorDisabled = (ZuiColor)Param1;
        if (!Param2)
            ZuiControlInvalidate(cp, TRUE);
        return 0;
    }
    case ZM_SetAnimation: {
        if (cp->m_aAnime)
            ZuiAnimationFree(cp->m_aAnime);
        cp->m_aAnime = ZuiAnimationNew(Param1, Param2);
        return 0;
    }
    case ZM_Button_SetImagePadding: {
        memcpy(&p->m_rcImagePadding, (void *)Param1, sizeof(ZRect));
        if(!Param2)
            ZuiControlNeedUpdate(cp);
        return 0;
    }
    case ZM_Button_SetStyle : {
        p->m_dwStyle = (int)Param1;
        return 0;
    }
    case ZM_SetAttribute: {
        ZuiAttribute zAttr = (ZuiAttribute)Param1;
        if (_tcsicmp(zAttr->name, _T("normalimage")) == 0)
		ZCCALL(ZM_Button_SetResNormal, cp, (ZPARAM)ZuiResDBGetRes(zAttr->value, ZREST_IMG), Param2);
        else if (_tcsicmp(zAttr->name, _T("hotimage")) == 0)
		ZCCALL(ZM_Button_SetResHot, cp, (ZPARAM)ZuiResDBGetRes(zAttr->value, ZREST_IMG), Param2);
        else if (_tcsicmp(zAttr->name, _T("pushedimage")) == 0)
		ZCCALL(ZM_Button_SetResPushed, cp, (ZPARAM)ZuiResDBGetRes(zAttr->value, ZREST_IMG), Param2);
        else if (_tcsicmp(zAttr->name, _T("focusedimage")) == 0)
		ZCCALL(ZM_Button_SetResFocused, cp, (ZPARAM)ZuiResDBGetRes(zAttr->value, ZREST_IMG), Param2);
        else if (_tcsicmp(zAttr->name, _T("disabledimage")) == 0)
		ZCCALL(ZM_Button_SetResDisabled, cp, (ZPARAM)ZuiResDBGetRes(zAttr->value, ZREST_IMG), Param2);
        else if (_tcsicmp(zAttr->name, _T("foregroundimage")) == 0)
		ZCCALL(ZM_Button_SetResForeground, cp, (ZPARAM)ZuiResDBGetRes(zAttr->value, ZREST_IMG), Param2);
        else if (_tcsicmp(zAttr->name, _T("normalcolor")) == 0)
		ZCCALL(ZM_Button_SetColorNormal, cp, (ZPARAM)ZuiStr2Color(zAttr->value), Param2);
        else if (_tcsicmp(zAttr->name, _T("hotcolor")) == 0)
		ZCCALL(ZM_Button_SetColorHot, cp, (ZPARAM)ZuiStr2Color(zAttr->value), Param2);
        else if (_tcsicmp(zAttr->name, _T("pushedcolor")) == 0)
		ZCCALL(ZM_Button_SetColorPushed, cp, (ZPARAM)ZuiStr2Color(zAttr->value), Param2);
        else if (_tcsicmp(zAttr->name, _T("focusedcolor")) == 0)
		ZCCALL(ZM_Button_SetColorFocused, cp, (ZPARAM)ZuiStr2Color(zAttr->value), Param2);
        else if (_tcsicmp(zAttr->name, _T("disabledcolor")) == 0)
		ZCCALL(ZM_Button_SetColorDisabled, cp, (ZPARAM)ZuiStr2Color(zAttr->value), Param2);
        else if (_tcsicmp(zAttr->name, _T("imagepadding")) == 0) {
            ZRect rcPadding = { 0 };
            ZuiText pstr = NULL;
            rcPadding.left = _tcstol(zAttr->value, &pstr, 10);  ASSERT(pstr);
            rcPadding.top = _tcstol(pstr + 1, &pstr, 10);    ASSERT(pstr);
            rcPadding.right = _tcstol(pstr + 1, &pstr, 10);  ASSERT(pstr);
            rcPadding.bottom = _tcstol(pstr + 1, &pstr, 10); ASSERT(pstr);
            ZCCALL(ZM_Button_SetImagePadding, cp, &rcPadding, Param2);
        }
        else if (_tcsicmp(zAttr->name, _T("buttonstyle")) == 0)
            ZCCALL(ZM_Button_SetStyle, cp, (ZPARAM)(_ttoi(zAttr->value)), Param2);
        break;
    }
    case ZM_OnCreate: {
        p = (ZuiButton)malloc(sizeof(ZButton));
        memset(p, 0, sizeof(ZButton));

        //保存原来的回调地址,创建成功后回调地址指向当前函数
        p->old_udata = ZuiLabelProc(ZM_OnCreate, cp, 0, 0, 0);
        p->old_call = (ZCtlProc)&ZuiLabelProc;

        p->m_ColorNormal = 0xFF333333;
        p->m_ColorHot = 0xFF3D3D3D;
        p->m_ColorPushed = 0xFF282828;
        p->m_ColorDisabled = 0xFF282828;
        cp->m_dwBorderColor2 = 0xFF05AA05;

        ((ZuiLabel)p->old_udata)->m_uTextStyle |= ZDT_CENTER;

        return p;
    }
    case ZM_OnDestroy: {
        ZCtlProc old_call = p->old_call;
        ZVoid old_udata = p->old_udata;
        if (p->m_ResNormal)
            ZuiResDBDelRes(p->m_ResNormal);
        if (p->m_ResHot)
            ZuiResDBDelRes(p->m_ResHot);
        if (p->m_ResPushed)
            ZuiResDBDelRes(p->m_ResPushed);
        if (p->m_ResFocused)
            ZuiResDBDelRes(p->m_ResFocused);
        if (p->m_ResDisabled)
            ZuiResDBDelRes(p->m_ResDisabled);

        old_call(ProcId, cp, old_udata, Param1, Param2);

        free(p);

        return 0;
    }
    case ZM_GetObject:
        if (_tcsicmp(Param1, ZC_Button) == 0)
            return (ZPARAM)p;
        break;
    case ZM_GetType:
        return (ZPARAM)ZC_Button;
    case ZM_CoreInit:
        return (ZPARAM)TRUE;
    case ZM_CoreUnInit:
        return (ZPARAM)TRUE;
    default:
        break;
    }
    return p->old_call(ProcId, cp, p->old_udata, Param1, Param2);
}




