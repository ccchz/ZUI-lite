#include "Label.h"
#include <core/control.h>
#include <core/resdb.h>
#include <core/function.h>
#include <core/builder.h>
#include <platform/platform.h>
#include <stdlib.h>
ZEXPORT ZuiAny ZCALL ZuiLabelProc(int ProcId, ZuiControl cp, ZuiLabel p, ZuiAny Param1, ZuiAny Param2) {
    switch (ProcId)
    {
    case ZM_OnPaintText: {
        ZuiColor tmpTColor;
        if (!cp->m_sText)
            return 0;
        if (cp->m_bEnabled)
            tmpTColor = p->m_cTextColor;
        else
            tmpTColor = p->m_cTextColorDisabled;
        ZRect *rc = &cp->m_rcItem;
        ZRect pt;
        pt.left = rc->left + p->m_rcPadding.left + cp->m_dwBorderWidth;
        pt.top = rc->top + p->m_rcPadding.top + cp->m_dwBorderWidth;
        pt.right = rc->right - p->m_rcPadding.right - cp->m_dwBorderWidth;
        pt.bottom = rc->bottom - p->m_rcPadding.bottom - cp->m_dwBorderWidth;
        if (p->m_rFont)
            ZuiDrawString(cp, p->m_rFont->p, cp->m_sText, _tcslen(cp->m_sText), &pt, tmpTColor, p->m_uTextStyle);
        else
            ZuiDrawString(cp, Global_Font->p, cp->m_sText, _tcslen(cp->m_sText), &pt, tmpTColor, p->m_uTextStyle);
        return 0;
    }
    case ZM_Label_SetFont: {
        if (p->m_rFont)
            ZuiResDBDelRes(p->m_rFont);
        p->m_rFont = Param1;
        if (!Param2)
            ZuiControlInvalidate(cp, TRUE);
        return 0;
    }
    case ZM_Label_SetTextColor: {
        p->m_cTextColor = (ZuiColor)Param1;
        if (!Param2)
            ZuiControlInvalidate(cp, TRUE);
        return 0;
    }
    case ZM_Label_SetTextColorDisabled: {
        p->m_cTextColorDisabled = (ZuiColor)Param1;
        if (!Param2)
            ZuiControlInvalidate(cp, TRUE);
        return 0;
    }
    case ZM_Label_SetTextPadding: {
        memcpy(&p->m_rcPadding, Param1, sizeof(ZRect));
        if (!Param2)
            ZuiControlNeedUpdate(cp);
        return 0;
    }
    case ZM_Label_SetTextStyle: {
        p->m_uTextStyle = (unsigned int)Param1;
        if (!Param2)
            ZuiControlInvalidate(cp, TRUE);
        return 0;
    }
    case ZM_Label_GetTextStyle: {
        return (ZuiAny)p->m_uTextStyle;
    }
    case ZM_SetAttribute: {
        ZuiAttribute zAttr = (ZuiAttribute)Param1;
        if (_tcsicmp(zAttr->name, _T("font")) == 0)
            ZCCALL(ZM_Label_SetFont, cp, ZuiResDBGetRes(zAttr->value, ZREST_FONT), Param2);
        if (_tcsicmp(zAttr->name, _T("align")) == 0) {
            //横向对齐方式
            unsigned int tstyle = (unsigned int)ZCCALL(ZM_Label_GetTextStyle, cp, 0, 0);
            if (_tcsicmp(zAttr->value, _T("left")) == 0) {
                tstyle &= ~(ZDT_CENTER | ZDT_RIGHT | ZDT_WORDBREAK | ZDT_EDITCONTROL);
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
            ZCCALL(ZM_Label_SetTextStyle, cp, (ZuiAny)tstyle, Param2);
        }
        else if (_tcsicmp(zAttr->name, _T("valign")) == 0) {
            //纵向对齐方式
            unsigned int tstyle = (unsigned int)ZCCALL(ZM_Label_GetTextStyle, cp, 0, 0);
            if (_tcsicmp(zAttr->value, _T("top")) == 0) {
                tstyle &= ~(ZDT_BOTTOM | ZDT_VCENTER | ZDT_WORDBREAK | ZDT_EDITCONTROL);
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
            ZCCALL(ZM_Label_SetTextStyle, cp, (ZuiAny)tstyle, Param2);
        }
        else if (_tcsicmp(zAttr->name, _T("textcolor")) == 0) {
            ZuiColor clrColor = ZuiStr2Color(zAttr->value);
            ZCCALL(ZM_Label_SetTextColor, cp, (ZuiAny)clrColor, Param2);
        }
        else if (_tcsicmp(zAttr->name, _T("textcolordisabled")) == 0) {
            ZuiColor clrColor = ZuiStr2Color(zAttr->value);
            ZCCALL(ZM_Label_SetTextColorDisabled, cp, (ZuiAny)clrColor, Param2);
        }
        else if (_tcsicmp(zAttr->name, _T("textpadding")) == 0) {
            //字体边距
            ZRect rcPadding = { 0 };
            ZuiText pstr = NULL;
            rcPadding.left = _tcstol(zAttr->value, &pstr, 10);  ASSERT(pstr);
            rcPadding.top = _tcstol(pstr + 1, &pstr, 10);    ASSERT(pstr);
            rcPadding.right = _tcstol(pstr + 1, &pstr, 10);  ASSERT(pstr);
            rcPadding.bottom = _tcstol(pstr + 1, &pstr, 10); ASSERT(pstr);
            ZCCALL(ZM_Label_SetTextPadding, cp, &rcPadding, Param2);
        }
        else if (_tcsicmp(zAttr->name, _T("wordbreak")) == 0) {
            //自动换行
            unsigned int tstyle = (unsigned int)ZCCALL(ZM_Label_GetTextStyle, cp, 0, 0);
            if (_tcsicmp(zAttr->value, _T("true")) == 0) {
                tstyle &= 0;  //清除对齐方式。
                tstyle |= ZDT_WORDBREAK | ZDT_EDITCONTROL;
            }
            else {
                tstyle &= ~ZDT_WORDBREAK & ~ZDT_EDITCONTROL;
                tstyle |= ZDT_SINGLELINE;
            }
            ZCCALL(ZM_Label_SetTextStyle, cp, (ZuiAny)tstyle, Param2);
        }
        else if (_tcsicmp(zAttr->name, _T("singleline")) == 0) {
            unsigned int tstyle = (unsigned int)ZCCALL(ZM_Label_GetTextStyle, cp, 0, 0);
            if (_tcsicmp(zAttr->value, _T("true")) == 0) {
                tstyle |= ZDT_SINGLELINE;
            }
            else {
                tstyle &= ~ZDT_SINGLELINE;
            }
            ZCCALL(ZM_Label_SetTextStyle, cp, (ZuiAny)tstyle, Param2);
        }
        else if (_tcsicmp(zAttr->name, _T("endellipsis")) == 0) {
            //替换超出部分为...
            unsigned int tstyle = (unsigned int)ZCCALL(ZM_Label_GetTextStyle, cp, 0, 0);
            if (_tcsicmp(zAttr->value, _T("true")) == 0) {
                tstyle &= ~(ZDT_WORDBREAK | ZDT_EDITCONTROL); //自动换行和超出替换互斥。
                tstyle |= ZDT_END_ELLIPSIS;
            }
            else 
                tstyle &= ~ZDT_END_ELLIPSIS;
            ZCCALL(ZM_Label_SetTextStyle, cp, (ZuiAny)tstyle, Param2);
        }
        break;
    }
    case ZM_OnCreate: {
        ZuiLabel np = (ZuiLabel)malloc(sizeof(ZLabel));
        memset(np, 0, sizeof(ZLabel));
        //保存原来的回调地址,创建成功后回调地址指向当前函数
        np->old_call = cp->call;

        np->m_uTextStyle = ZDT_VCENTER | ZDT_SINGLELINE;
        np->m_cTextColor = 0xFFd8d8d8;
        np->m_cTextColorDisabled = 0xFFa8a8a8;
        ZRect rctmp = { 2,1,2,1 };
        np->m_rcPadding = rctmp;
        return np;
    }
    case ZM_OnDestroy: {
        ZCtlProc old_call = p->old_call;

        old_call(ProcId, cp, 0, Param1, Param2);
        if (p->m_rFont && !Param1) ZuiResDBDelRes(p->m_rFont);
        free(p);

        return 0;
    }
    case ZM_GetObject:
        if (_tcsicmp(Param1, (ZuiAny)ZC_Label) == 0)
            return (ZuiAny)p;
        break;
    case ZM_GetType:
        return (ZuiAny)ZC_Label;
    case ZM_CoreInit:
        return (ZuiAny)TRUE;
    case ZM_CoreUnInit:
        return (ZuiAny)NULL;
    default:
        break;
    }
    return p->old_call(ProcId, cp, 0, Param1, Param2);
}




