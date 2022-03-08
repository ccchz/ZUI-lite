#include "Container.h"
#include <core/control.h>
#include <core/resdb.h>
#include <core/builder.h>
#include <platform/platform.h>
#include <layout/HorizontalLayout.h>
ZEXPORT ZINT ZCALL ZuiContainerProc(ZINT ProcId, ZuiControl cp, ZuiContainer p, ZPARAM Param1, ZPARAM Param2) {
    switch(ProcId){
        case ZM_OnEvent: {
            TEventUI* event = (TEventUI*)Param1;
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
        case ZM_Container_SetColorNormal: {
            p->m_ColorNormal = (ZuiColor)Param1;
            if (!Param2)
                ZuiControlInvalidate(cp, TRUE);
            return 0;
        }
        case ZM_Container_SetColorHot: {
            p->m_ColorHot = (ZuiColor)Param1;
            if (!Param2)
                ZuiControlInvalidate(cp, TRUE);
            return 0;
        }
        case ZM_Container_SetColorPushed: {
            p->m_ColorPushed = (ZuiColor)Param1;
            if (!Param2)
                ZuiControlInvalidate(cp, TRUE);
            return 0;
        }
        case ZM_Container_SetColorFocused: {
            p->m_ColorFocused = (ZuiColor)Param1;
            if (!Param2)
                ZuiControlInvalidate(cp, TRUE);
            return 0;
        }
        case ZM_Container_SetColorDisabled: {
            p->m_ColorDisabled = (ZuiColor)Param1;
            if (!Param2)
                ZuiControlInvalidate(cp, TRUE);
            return 0;
        }
        case ZM_SetAttribute: {
            ZuiAttribute zAttr = (ZuiAttribute)Param1;
            if (_tcsicmp(zAttr->name, _T("normalcolor")) == 0)
                ZCCALL(ZM_Container_SetColorNormal, cp, (ZPARAM)ZuiStr2Color(zAttr->value), Param2);
            else if (_tcsicmp(zAttr->name, _T("hotcolor")) == 0)
                ZCCALL(ZM_Container_SetColorHot, cp, (ZPARAM)ZuiStr2Color(zAttr->value), Param2);
            else if (_tcsicmp(zAttr->name, _T("pushedcolor")) == 0)
                ZCCALL(ZM_Container_SetColorPushed, cp, (ZPARAM)ZuiStr2Color(zAttr->value), Param2);
            else if (_tcsicmp(zAttr->name, _T("focusedcolor")) == 0)
                ZCCALL(ZM_Button_SetColorFocused, cp, (ZPARAM)ZuiStr2Color(zAttr->value), Param2);
            else if (_tcsicmp(zAttr->name, _T("disabledcolor")) == 0)
                ZCCALL(ZM_Container_SetColorDisabled, cp, (ZPARAM)ZuiStr2Color(zAttr->value), Param2);
            break;
        }
        case ZM_OnPaintBorder: {
            ZRect* rc = (ZRect*)&cp->m_rcItem;
            if (cp->m_dwBorderWidth && p->m_dwType > 0) {
                ZuiDrawRoundRect(cp, cp->m_dwBorderColor2, rc, &cp->m_rRound, cp->m_dwBorderWidth);
                return 0;
            }
            break;
        }
        case ZM_OnPaintStatusImage: {
            ZRect* rc = (ZRect*)&cp->m_rcItem;
            if (p->m_dwType == 0) {
                    ZuiDrawFillRoundRect(cp, p->m_ColorNormal, rc, &cp->m_rRound);
            }
            else if (p->m_dwType == 1) {
                    ZuiDrawFillRoundRect(cp, p->m_ColorHot, rc, &cp->m_rRound);
            }
            else if (p->m_dwType == 2) {
                    ZuiDrawFillRoundRect(cp, p->m_ColorPushed, rc, &cp->m_rRound);
            }
            else {
                    ZuiDrawFillRoundRect(cp, p->m_ColorDisabled, rc, &cp->m_rRound);
            }
            return 0;
        }
	    case ZM_OnCreate: {
            p = (ZuiContainer)malloc(sizeof(ZContainer));
            memset(p, 0, sizeof(ZContainer));
            
            //保存原来的回调地址,创建成功后回调地址指向当前函数
            p->old_udata = ZuiHorizontalLayoutProc(ZM_OnCreate, cp, 0, 0, 0);
            p->old_call = (ZCtlProc)&ZuiHorizontalLayoutProc;

            p->m_ColorNormal = 0xFF333333;
            p->m_ColorHot = 0xFF3D3D3D;
            p->m_ColorPushed = 0xFF282828;
            p->m_ColorDisabled = 0xFF282828;
            cp->m_dwBorderColor2 = 0xFF05AA05;

            return p;
        }
        case ZM_OnDestroy: {
            ZCtlProc old_call = p->old_call;
            ZVoid old_udata = p->old_udata;
            old_call(ProcId, cp, old_udata, Param1, Param2);

            free(p);

            return 0;
        }
        case ZM_GetObject:
            if (_tcsicmp(Param1, (ZPARAM)ZC_Container) == 0)
                return (ZPARAM)p;
            break;
        case ZM_GetType:
            return (ZPARAM)ZC_Container;
        case ZM_CoreInit:
            return (ZPARAM)TRUE;
        case ZM_CoreUnInit:
            return (ZPARAM)TRUE;
        default:
            break;
    }
    return p->old_call(ProcId, cp, p->old_udata, Param1, Param2);
}