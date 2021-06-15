#include "Button.h"
#include "Label.h"
#include <core/control.h>
#include <core/resdb.h>
#include <core/builder.h>
#include <platform/platform.h>
#include <stdlib.h>
ZEXPORT ZuiAny ZCALL ZuiButtonProc(int ProcId, ZuiControl cp, ZuiButton p, ZuiAny Param1, ZuiAny Param2) {
    switch (ProcId)
    {
    case ZM_OnEvent: {
        TEventUI *event = (TEventUI *)Param1;
        switch (event->Type)
        {
	case ZEVENT_KILLFOCUS: {
	    p->type = 0;
	    break;
	}
        case ZEVENT_MOUSELEAVE: {
            p->type = 0;
            ZuiControlInvalidate(cp, TRUE);
            break;
	}
        case ZEVENT_MOUSEENTER: {
            p->type = 1;
            ZuiControlInvalidate(cp, TRUE);
            break;
	}
        case ZEVENT_LBUTTONDOWN: {
            p->type = 2;
            ZuiControlInvalidate(cp, TRUE);
	    break;
        }
        case ZEVENT_LBUTTONUP: {
            p->type = 1;
            ZuiControlInvalidate(cp, TRUE);
	    break;
        }
        default:
            break;
        }
        break;
    }
    case ZM_OnPaint: {
        //调整绘制顺序
        ZCCALL(ZM_OnPaintBkColor, cp, Param1, Param2);
        ZCCALL(ZM_OnPaintBkImage, cp, Param1, Param2);
        ZCCALL(ZM_OnPaintStatusImage, cp, Param1, Param2);
        ZCCALL(ZM_OnPaintText, cp, Param1, Param2);
        ZCCALL(ZM_OnPaintBorder, cp, Param1, Param2);
        return 0;
    }
    case ZM_OnPaintBorder: {
        ZuiGraphics gp = (ZuiGraphics)Param1;
        ZRect* rc = (ZRect*)&cp->m_rcItem;
        if (cp->m_dwBorderWidth && !p->m_ResNormal && p->type > 0) {
            ZuiDrawRoundRect(gp, p->m_BorderColor, rc, cp->m_rRound.cx, cp->m_rRound.cy,cp->m_dwBorderWidth);
            return 0;
        }
        break;
    }
    case ZM_OnPaintStatusImage: {
        ZuiGraphics gp = (ZuiGraphics)Param1;
        ZRect *rc = (ZRect *)&cp->m_rcItem;
        ZuiImage img;
        if (p->type == 0) {
            if (p->m_ResNormal) {
                img = p->m_ResNormal->p;
                ZuiDrawImageEx(gp, img, rc->left, rc->top, rc->right, rc->bottom, 0, 0, 0, 0, 255);
            }
            else {
                ZuiDrawFillRoundRect(gp, p->m_ColorNormal, rc,cp->m_rRound.cx,cp->m_rRound.cy);
            }
        }
        else if (p->type == 1) {
            if (p->m_ResHot) {
                img = p->m_ResHot->p;
                ZuiDrawImageEx(gp, img, rc->left, rc->top, rc->right, rc->bottom, 0, 0, 0, 0, 255);
            }
            else {
                ZuiDrawFillRoundRect(gp, p->m_ColorHot, rc, cp->m_rRound.cx, cp->m_rRound.cy);
            }
        }
        else if (p->type == 2) {
            if (p->m_ResPushed) {
                img = p->m_ResPushed->p;
                ZuiDrawImageEx(gp, img, rc->left, rc->top, rc->right, rc->bottom, 0, 0, 0, 0, 255);
            }
            else {
                ZuiDrawFillRoundRect(gp, p->m_ColorPushed, rc, cp->m_rRound.cx, cp->m_rRound.cy);
            }
        }
        else {
            if (p->m_ResDisabled) {
                img = p->m_ResDisabled->p;
                ZuiDrawImageEx(gp, img, rc->left, rc->top, rc->right, rc->bottom, 0, 0, 0, 0, 255);
            }
            else {
                ZuiDrawFillRoundRect(gp, p->m_ColorDisabled, rc, cp->m_rRound.cx, cp->m_rRound.cy);
            }
        }
        return 0;
    }
    case ZM_OnPaintText: { //按钮控件带图片绘制。
        if (p->m_ResForeground) {
            ZuiGraphics gp = (ZuiGraphics)Param1;
            ZRect rcc = { 0 };
            ZRect* rc = (ZRect*)&cp->m_rcItem;
            ZuiImage img = p->m_ResForeground->p;
            ZuiLabel zb = p->old_udata;
            ZSizeR sr = {0};
            ZuiMeasureTextSize(gp, zb->m_rFont ? zb->m_rFont->p : Global_Font, cp->m_sText, &sr);

            rcc.left = rc->left + ((rc->right - rc->left) - img->Width - sr.cx - sepSize) / 2;
            rcc.top = rc->top + ((rc->bottom - rc->top) - img->Height) / 2;
            rcc.right = rcc.left + img->Width;
            rcc.bottom = rcc.top + img->Height;
            ZuiDrawImageEx(gp, img, rcc.left, rcc.top, rcc.right, rcc.bottom, 0, 0, 0, 0, 255);
            rcc.left = rcc.right + sepSize;
            rcc.right = rc->right - cp->m_dwBorderWidth;
            rcc.bottom = rc->bottom - cp->m_dwBorderWidth;
            ZuiColor tmpTColor;
            if (!cp->m_sText)
                return 0;
            if (cp->m_bEnabled)
                tmpTColor = zb->m_cTextColor;
            else
                tmpTColor = zb->m_cTextColorDisabled;
            ZuiDrawString(gp, zb->m_rFont ? zb->m_rFont->p : Global_Font,cp->m_sText, _tcslen(cp->m_sText), & rcc, tmpTColor, ZDT_VCENTER | ZDT_SINGLELINE);
            return 0;
        }
        break;
    }
    case ZM_SetEnabled: {
        if (0 == (ZuiBool)Param1)
            p->type = -1;
        else
            p->type = 0;
        break;
    }
    case ZM_Button_SetResNormal: {
        if (p->m_ResNormal)
            ZuiResDBDelRes(p->m_ResNormal);
        p->m_ResNormal = Param1;
        ZuiControlInvalidate(cp, TRUE);
        return 0;
    }
    case ZM_Button_SetResHot: {
        if (p->m_ResHot)
            ZuiResDBDelRes(p->m_ResHot);
        p->m_ResHot = Param1;
        ZuiControlInvalidate(cp, TRUE);
        return 0;
    }
    case ZM_Button_SetResPushed: {
        if (p->m_ResPushed)
            ZuiResDBDelRes(p->m_ResPushed);
        p->m_ResPushed = Param1;
        ZuiControlInvalidate(cp, TRUE);
        return 0;
    }
    case ZM_Button_SetResFocused: {
        if (p->m_ResFocused)
            ZuiResDBDelRes(p->m_ResFocused);
        p->m_ResFocused = Param1;
        ZuiControlInvalidate(cp, TRUE);
        return 0;
    }
    case ZM_Button_SetResDisabled: {
        if (p->m_ResDisabled)
            ZuiResDBDelRes(p->m_ResDisabled);
        p->m_ResDisabled = Param1;
        ZuiControlInvalidate(cp, TRUE);
        return 0;
    }
    case ZM_Button_SetResForeground: {
        if (p->m_ResForeground)
            ZuiResDBDelRes(p->m_ResForeground);
        p->m_ResForeground = Param1;
        ZuiControlInvalidate(cp, TRUE);
        return 0;
    }
    case ZM_Button_SetColorNormal: {
        p->m_ColorNormal = (ZuiColor)Param1;
        ZuiControlInvalidate(cp, TRUE);
        return 0;
    }
    case ZM_Button_SetColorHot: {
        p->m_ColorHot = (ZuiColor)Param1;
        ZuiControlInvalidate(cp, TRUE);
        return 0;
    }
    case ZM_Button_SetBorderColorHot: {
        p->m_BorderColor = (ZuiColor)Param1;
        ZuiControlInvalidate(cp, TRUE);
        return 0;
    }
    case ZM_Button_SetColorPushed: {
        p->m_ColorPushed = (ZuiColor)Param1;
        ZuiControlInvalidate(cp, TRUE);
        return 0;
    }
    case ZM_Button_SetColorFocused: {
        p->m_ColorFocused = (ZuiColor)Param1;
        ZuiControlInvalidate(cp, TRUE);
        return 0;
    }
    case ZM_Button_SetColorDisabled: {
        p->m_ColorDisabled = (ZuiColor)Param1;
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
        memcpy(&p->m_rcImagePadding, Param1, sizeof(ZRect));
        ZuiControlNeedUpdate(cp);
        return 0;
    }
    case ZM_SetAttribute: {
        if (_tcsicmp(Param1, _T("normalimage")) == 0)
		ZCCALL(ZM_Button_SetResNormal, cp, ZuiResDBGetRes(Param2, ZREST_IMG), NULL);
        else if (_tcsicmp(Param1, _T("hotimage")) == 0)
		ZCCALL(ZM_Button_SetResHot, cp, ZuiResDBGetRes(Param2, ZREST_IMG), NULL);
        else if (_tcsicmp(Param1, _T("pushedimage")) == 0)
		ZCCALL(ZM_Button_SetResPushed, cp, ZuiResDBGetRes(Param2, ZREST_IMG), NULL);
        else if (_tcsicmp(Param1, _T("focusedimage")) == 0)
		ZCCALL(ZM_Button_SetResFocused, cp, ZuiResDBGetRes(Param2, ZREST_IMG), NULL);
        else if (_tcsicmp(Param1, _T("disabledimage")) == 0)
		ZCCALL(ZM_Button_SetResDisabled, cp, ZuiResDBGetRes(Param2, ZREST_IMG), NULL);
        else if (_tcsicmp(Param1, _T("foregroundimage")) == 0)
		ZCCALL(ZM_Button_SetResForeground, cp, ZuiResDBGetRes(Param2, ZREST_IMG), NULL);
        else if (_tcsicmp(Param1, _T("normalcolor")) == 0)
		ZCCALL(ZM_Button_SetColorNormal, cp, (ZuiAny)ZuiStr2Color(Param2), NULL);
        else if (_tcsicmp(Param1, _T("hotcolor")) == 0)
		ZCCALL(ZM_Button_SetColorHot, cp, (ZuiAny)ZuiStr2Color(Param2), NULL);
        else if (_tcsicmp(Param1, _T("hotbordercolor")) == 0)
		ZCCALL(ZM_Button_SetBorderColorHot, cp, (ZuiAny)ZuiStr2Color(Param2), NULL);
        else if (_tcsicmp(Param1, _T("pushedcolor")) == 0)
		ZCCALL(ZM_Button_SetColorPushed, cp, (ZuiAny)ZuiStr2Color(Param2), NULL);
        else if (_tcsicmp(Param1, _T("focusedcolor")) == 0)
		ZCCALL(ZM_Button_SetColorFocused, cp, (ZuiAny)ZuiStr2Color(Param2), NULL);
        else if (_tcsicmp(Param1, _T("disabledcolor")) == 0)
		ZCCALL(ZM_Button_SetColorDisabled, cp, (ZuiAny)ZuiStr2Color(Param2), NULL);
        else if (_tcsicmp(Param1, _T("imagepadding")) == 0) {
            ZRect rcPadding = { 0 };
            ZuiText pstr = NULL;
            rcPadding.left = _tcstol(Param2, &pstr, 10);  ASSERT(pstr);
            rcPadding.top = _tcstol(pstr + 1, &pstr, 10);    ASSERT(pstr);
            rcPadding.right = _tcstol(pstr + 1, &pstr, 10);  ASSERT(pstr);
            rcPadding.bottom = _tcstol(pstr + 1, &pstr, 10); ASSERT(pstr);
            ZCCALL(ZM_Button_SetImagePadding, cp, &rcPadding, NULL);
        }
        break;
    }
    case ZM_OnCreate: {
        ZuiButton np = (ZuiButton)malloc(sizeof(ZButton));
        memset(np, 0, sizeof(ZButton));

        //保存原来的回调地址,创建成功后回调地址指向当前函数
        np->old_udata = ZuiLabelProc(ZM_OnCreate, cp, 0, 0, 0);
        np->old_call = (ZCtlProc)&ZuiLabelProc;

        np->m_ColorNormal = 0xFF383838;
        np->m_ColorHot = 0xFF585858;
        np->m_ColorPushed = 0xFF787878;
        np->m_ColorDisabled = 0xFF989898;
        np->m_BorderColor = 0xFF1874CD;

        ((ZuiLabel)np->old_udata)->m_uTextStyle |= ZDT_CENTER;

        return np;
    }
    case ZM_OnDestroy: {
        ZCtlProc old_call = p->old_call;
        ZuiAny old_udata = p->old_udata;
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
        if (_tcsicmp(Param1, (ZuiAny)ZC_Button) == 0)
            return (ZuiAny)p;
        break;
    case ZM_GetType:
        return (ZuiAny)ZC_Button;
    case ZM_CoreInit:
        return (ZuiAny)TRUE;
    case ZM_CoreUnInit:
        return (ZuiAny)TRUE;
    default:
        break;
    }
    return p->old_call(ProcId, cp, p->old_udata, Param1, Param2);
}




