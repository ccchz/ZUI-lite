#include "Container.h"
#include <core/control.h>
#include <core/resdb.h>
#include <core/builder.h>
#include <platform/platform.h>
#include <layout/HorizontalLayout.h>
ZEXPORT ZuiAny ZCALL ZuiContainerProc(int ProcId, ZuiControl cp, ZuiContainer p, ZuiAny Param1, ZuiAny Param2) {
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
            ZuiControlInvalidate(cp, TRUE);
            return 0;
        }
        case ZM_Container_SetColorHot: {
            p->m_ColorHot = (ZuiColor)Param1;
            ZuiControlInvalidate(cp, TRUE);
            return 0;
        }
        case ZM_Container_SetColorPushed: {
            p->m_ColorPushed = (ZuiColor)Param1;
            ZuiControlInvalidate(cp, TRUE);
            return 0;
        }
        case ZM_Container_SetColorFocused: {
            p->m_ColorFocused = (ZuiColor)Param1;
            ZuiControlInvalidate(cp, TRUE);
            return 0;
        }
        case ZM_Container_SetColorDisabled: {
            p->m_ColorDisabled = (ZuiColor)Param1;
            ZuiControlInvalidate(cp, TRUE);
            return 0;
        }
        case ZM_SetAttribute: {
            if (_tcsicmp(Param1, _T("normalcolor")) == 0)
                ZCCALL(ZM_Container_SetColorNormal, cp, (ZuiAny)ZuiStr2Color(Param2), NULL);
            else if (_tcsicmp(Param1, _T("hotcolor")) == 0)
                ZCCALL(ZM_Container_SetColorHot, cp, (ZuiAny)ZuiStr2Color(Param2), NULL);
            else if (_tcsicmp(Param1, _T("pushedcolor")) == 0)
                ZCCALL(ZM_Container_SetColorPushed, cp, (ZuiAny)ZuiStr2Color(Param2), NULL);
            else if (_tcsicmp(Param1, _T("focusedcolor")) == 0)
                ZCCALL(ZM_Button_SetColorFocused, cp, (ZuiAny)ZuiStr2Color(Param2), NULL);
            else if (_tcsicmp(Param1, _T("disabledcolor")) == 0)
                ZCCALL(ZM_Container_SetColorDisabled, cp, (ZuiAny)ZuiStr2Color(Param2), NULL);
            break;
        }
        case ZM_OnPaintBorder: {
            ZuiGraphics gp = (ZuiGraphics)Param1;
            ZRect* rc = (ZRect*)&cp->m_rcItem;
            if (cp->m_dwBorderWidth && p->m_dwType > 0) {
                ZuiDrawRoundRect(gp, cp->m_dwBorderColor2, rc, cp->m_rRound.cx, cp->m_rRound.cy, cp->m_dwBorderWidth);
                return 0;
            }
            break;
        }
        case ZM_OnPaintStatusImage: {
            ZuiGraphics gp = (ZuiGraphics)Param1;
            ZRect* rc = (ZRect*)&cp->m_rcItem;
            if (p->m_dwType == 0) {
                    ZuiDrawFillRoundRect(gp, p->m_ColorNormal, rc, cp->m_rRound.cx, cp->m_rRound.cy);
            }
            else if (p->m_dwType == 1) {
                    ZuiDrawFillRoundRect(gp, p->m_ColorHot, rc, cp->m_rRound.cx, cp->m_rRound.cy);
            }
            else if (p->m_dwType == 2) {
                    ZuiDrawFillRoundRect(gp, p->m_ColorPushed, rc, cp->m_rRound.cx, cp->m_rRound.cy);
            }
            else {
                    ZuiDrawFillRoundRect(gp, p->m_ColorDisabled, rc, cp->m_rRound.cx, cp->m_rRound.cy);
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
            ZuiAny old_udata = p->old_udata;
            old_call(ProcId, cp, old_udata, Param1, Param2);

            free(p);

            return 0;
        }
        case ZM_GetObject:
            if (_tcsicmp(Param1, (ZuiAny)ZC_Container) == 0)
                return (ZuiAny)p;
            break;
        case ZM_GetType:
            return (ZuiAny)ZC_Container;
        case ZM_CoreInit:
            return (ZuiAny)TRUE;
        case ZM_CoreUnInit:
            return (ZuiAny)TRUE;
        default:
            break;
    }
    return p->old_call(ProcId, cp, p->old_udata, Param1, Param2);
}