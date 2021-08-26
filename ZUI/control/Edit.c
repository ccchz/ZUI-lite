#include "Edit.h"
#include <core/control.h>
#include <core/resdb.h>
#include <core/function.h>
#include <core/builder.h>
#include <platform/platform.h>
ZEXPORT ZuiAny ZCALL ZuiEditProc(int ProcId, ZuiControl cp, ZuiEdit p, ZuiAny Param1, ZuiAny Param2) {
    switch (ProcId)
    {
    case ZM_OnEvent: {
        TEventUI* event = (TEventUI*)Param1;
        switch (event->Type)
        {
        case ZEVENT_SETCURSOR:
        {
            ZuiOsSetCursor((unsigned int)ZIDC_IBEAM);
            return 0;
        }
        case ZEVENT_SETFOCUS:
        {
            cp->m_bFocused = TRUE;
            CreateCaret(cp->m_pOs->m_hWnd, NULL, 1, ZuiFontHeight(cp,p->m_rFont?p->m_rFont->p : Global_Font->p));
            ShowCaret(cp->m_pOs->m_hWnd);
            SetCaretPos(cp->m_rcItem.left + cp->m_dwBorderWidth + p->m_rcPadding.left,
                cp->m_rcItem.top + cp->m_dwBorderWidth + p->m_rcPadding.top);
            ZuiControlInvalidate(cp, TRUE);
            return 0;
        }
        case ZEVENT_KILLFOCUS:
        {
            cp->m_bFocused = FALSE;
            HideCaret(cp->m_pOs->m_hWnd);
            DestroyCaret();
            ZuiControlInvalidate(cp, TRUE);
            return 0;
        }
        case ZEVENT_CHAR: {
            _tprintf(_T("%x.."), event->chKey);
            if (event->chKey == VK_RETURN) {
                p->buffer[p->lastposition] = event->chKey;
                p->lastposition++;
                p->buffer[p->lastposition] = _T('\n');
                p->lastposition++;
            }
            else if (event->chKey == VK_BACK) {
                if (p->lastposition > 0) {
                    p->lastposition--;
                    if ((p->buffer[p->lastposition] & 0xFC00) == 0xDC00)
                        p->lastposition--;
                    p->buffer[p->lastposition] = _T('\0');
                }
                else {
                    ZuiBeep(0);
                }
            }
            else if (event->chKey == VK_ESCAPE) {
                ZuiBeep(0);
            }
            else {
                p->buffer[p->lastposition] = event->chKey;
                p->lastposition++;
            }
            ZuiControlInvalidate(cp, TRUE);
            return 0;
        }
        default:
            break;
        }
        break;
    }
    case ZM_OnPaintText: {
        ZuiColor tmpTColor;
        if (cp->m_bEnabled)
            tmpTColor = p->m_cTextColor;
        else
            tmpTColor = p->m_cTextColorDisabled;
        ZRect* rc = &cp->m_rcItem;
        ZRect pt;
        pt.left = rc->left + p->m_rcPadding.left + cp->m_dwBorderWidth;
        pt.top = rc->top + p->m_rcPadding.top + cp->m_dwBorderWidth;
        pt.right = rc->right - p->m_rcPadding.right - cp->m_dwBorderWidth;
        pt.bottom = rc->bottom - p->m_rcPadding.bottom - cp->m_dwBorderWidth;
        if (p->m_rFont)
            ZuiDrawString2(cp, p->m_rFont->p, p->buffer, p->lastposition, &pt, tmpTColor, p->m_uTextStyle);
        else
            ZuiDrawString2(cp, Global_Font->p, p->buffer, p->lastposition, &pt, tmpTColor, p->m_uTextStyle);
        return 0;
    }
    case ZM_Edit_SetFont: {
        if (p->m_rFont)
            ZuiResDBDelRes(p->m_rFont);
        p->m_rFont = Param1;
        if (!Param2)
            ZuiControlInvalidate(cp, TRUE);
        return 0;
    }
    case ZM_Edit_SetTextColor: {
        p->m_cTextColor = (ZuiColor)Param1;
        if (!Param2)
            ZuiControlInvalidate(cp, TRUE);
        return 0;
    }
    case ZM_Edit_SetTextColorDisabled: {
        p->m_cTextColorDisabled = (ZuiColor)Param1;
        if (!Param2)
            ZuiControlInvalidate(cp, TRUE);
        return 0;
    }
    case ZM_Edit_SetTextPadding: {
        memcpy(&p->m_rcPadding, Param1, sizeof(ZRect));
        if (!Param2)
            ZuiControlNeedUpdate(cp);
        return 0;
    }
    case ZM_SetAttribute: {
        ZuiAttribute zAttr = (ZuiAttribute)Param1;
        if (_tcsicmp(zAttr->name, _T("font")) == 0)
            ZCCALL(ZM_Edit_SetFont, cp, ZuiResDBGetRes(zAttr->value, ZREST_FONT), Param2);
        else if (_tcsicmp(zAttr->name, _T("textcolor")) == 0) {
            ZuiColor clrColor = ZuiStr2Color(zAttr->value);
            ZCCALL(ZM_Edit_SetTextColor, cp, (ZuiAny)clrColor, Param2);
        }
        else if (_tcsicmp(zAttr->name, _T("textcolordisabled")) == 0) {
            ZuiColor clrColor = ZuiStr2Color(zAttr->value);
            ZCCALL(ZM_Edit_SetTextColorDisabled, cp, (ZuiAny)clrColor, Param2);
        }
        else if (_tcsicmp(zAttr->name, _T("textpadding")) == 0) {
            //字体边距
            ZRect rcPadding = { 0 };
            ZuiText pstr = NULL;
            rcPadding.left = _tcstol(zAttr->value, &pstr, 10);  ASSERT(pstr);
            rcPadding.top = _tcstol(pstr + 1, &pstr, 10);    ASSERT(pstr);
            rcPadding.right = _tcstol(pstr + 1, &pstr, 10);  ASSERT(pstr);
            rcPadding.bottom = _tcstol(pstr + 1, &pstr, 10); ASSERT(pstr);
            ZCCALL(ZM_Edit_SetTextPadding, cp, &rcPadding, Param2);
        }
        break;
    }
    case ZM_GetControlFlags: {
        return (ZuiAny)ZFLAG_SETCURSOR;//需要设置鼠标
    }
    case ZM_OnCreate: {
        ZuiEdit np = (ZuiEdit)malloc(sizeof(ZEdit));
        memset(np, 0, sizeof(ZEdit));
        //保存原来的回调地址,创建成功后回调地址指向当前函数
        np->old_call = cp->call;

        np->buffer = (ZuiText)malloc(BUFFERSIZE);
        memset(np->buffer, 0, BUFFERSIZE);
        np->length = BUFFERSIZE;
        np->lastposition = 0;
        //np->m_uTextStyle = ZDT_VCENTER | ZDT_SINGLELINE;
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
        if (_tcsicmp(Param1, (ZuiAny)ZC_Edit) == 0)
            return (ZuiAny)p;
        break;
    case ZM_GetType:
        return (ZuiAny)ZC_Edit;
    case ZM_CoreInit:
        return (ZuiAny)TRUE;
    case ZM_CoreUnInit:
        return (ZuiAny)NULL;
    default:
        break;
    }
    return p->old_call(ProcId, cp, 0, Param1, Param2);
}