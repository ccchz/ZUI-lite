﻿#include "SplitterBar.h"
#include <core/control.h>
ZEXPORT ZuiAny ZCALL ZuiSplitterBarProc(int ProcId, ZuiControl cp, ZuiSplitterBar p, ZuiAny Param1, ZuiAny Param2){
    switch (ProcId)
    {
    case ZM_OnEvent: {
        TEventUI *event = (TEventUI *)Param1;
        switch (event->Type)
        {
        case ZEVENT_SETCURSOR:
        {
            if (cp->m_rcItem.right - cp->m_rcItem.left < cp->m_rcItem.bottom - cp->m_rcItem.top)
                SetCursor(LoadCursor(NULL, MAKEINTRESOURCE(IDC_SIZEWE)));
            else
                SetCursor(LoadCursor(NULL, MAKEINTRESOURCE(IDC_SIZENS)));
            return 0;
        }
        break;
        case ZEVENT_LBUTTONDOWN: {
            p->m_bMouseDown = TRUE;
            break;
        }
        case ZEVENT_LBUTTONUP: {
            p->m_bMouseDown = FALSE;
            break;
        }
        case ZEVENT_MOUSEMOVE: {
            if (p->m_bMouseDown && cp->m_pParent)
            {
                int index;
                if ((index = (int)ZCCALL(ZM_Layout_GetItemIndex, cp->m_pParent, cp, NULL)) > 0)
                {
                    ZuiControl lp = ZCCALL(ZM_Layout_GetItemAt, cp->m_pParent, (ZuiAny)(index - 1), NULL);//上一个控件
                    ZuiControl np = ZCCALL(ZM_Layout_GetItemAt, cp->m_pParent, (ZuiAny)(index + 1), NULL);//下一个控件
                    if (lp && np) {
                        if (cp->m_rcItem.right - cp->m_rcItem.left < cp->m_rcItem.bottom - cp->m_rcItem.top) {
                            if (event->ptMouse.x - lp->m_rcItem.left > lp->m_cxyMin.cx && np->m_rcItem.right - lp->m_rcItem.left - event->ptMouse.x + lp->m_rcItem.left - cp->m_rcItem.right + cp->m_rcItem.left > np->m_cxyMin.cx) {
                                int width = np->m_rcItem.right - lp->m_rcItem.left;
                                if (p->m_dwType == 1) {
                                    ZCCALL(ZM_SetFixedWidth, lp, (ZuiAny)(event->ptMouse.x - lp->m_rcItem.left), (ZuiAny)TRUE);
                                }
                                else if (p->m_dwType == 3)
                                {
                                    ZCCALL(ZM_SetFixedWidth, np, (ZuiAny)(width - event->ptMouse.x + lp->m_rcItem.left - cp->m_rcItem.right + cp->m_rcItem.left), (ZuiAny)TRUE);
                                }
                            }
                            else
                                return 0;
                        }
                        else {
                            if (event->ptMouse.y - lp->m_rcItem.top > lp->m_cxyMin.cy && np->m_rcItem.bottom - lp->m_rcItem.top - event->ptMouse.y + lp->m_rcItem.top - cp->m_rcItem.bottom + cp->m_rcItem.top) {
                                int height = np->m_rcItem.bottom - lp->m_rcItem.top;
                                if (p->m_dwType == 2) {
                                    ZCCALL(ZM_SetFixedHeight, lp, (ZuiAny)(event->ptMouse.y - lp->m_rcItem.top), (ZuiAny)TRUE);
                                }
                                else if (p->m_dwType == 4)
                                {
                                    ZCCALL(ZM_SetFixedHeight, np, (ZuiAny)(height - event->ptMouse.y + lp->m_rcItem.top - cp->m_rcItem.bottom + cp->m_rcItem.top), (ZuiAny)TRUE);
                                }
                            }
                            else
                                return 0;
                            //ZCCALL(ZM_SetFixedHeight, lp, event->ptMouse.y - lp->m_rcItem.top, NULL, NULL);
                        }
                        ZuiControlNeedUpdate(cp->m_pParent);
                    }
                }
            }
            break;
        }
        default:
            break;
        }
        break;
    }
    case ZM_OnPaint: {
        ZCCALL(ZM_OnPaintBkColor, cp, Param1, Param2);
        return 0;
    }
    case ZM_SetAttribute: {
        if (_tcsicmp(Param1, _T("sepside")) == 0) {
            if (_tcsicmp(Param2, _T("left")) == 0) {
                p->m_dwType = 1;
            }
            else if (_tcsicmp(Param2, _T("top")) == 0) {
                p->m_dwType = 2;
            }
            else if (_tcsicmp(Param2, _T("right")) == 0) {
                p->m_dwType = 3;
            }
            else if (_tcsicmp(Param2, _T("bottom")) == 0) {
                p->m_dwType = 4;
            }
        }
        break;
    }
    case ZM_GetControlFlags: {
        return (ZuiAny)ZFLAG_SETCURSOR;
        break;
    }
    case ZM_OnCreate: {
        ZuiSplitterBar np = (ZuiSplitterBar)malloc(sizeof(ZSplitterBar));
        memset(np, 0, sizeof(ZSplitterBar));
        //保存原来的回调地址,创建成功后回调地址指向当前函数
        np->old_call = cp->call;
        return np;
    }
    case ZM_OnDestroy: {
        ZCtlProc old_call = p->old_call;
        old_call(ProcId, cp, 0, Param1, Param2);
        free(p);

        return 0;
    }
    case ZM_GetObject:
        if (_tcsicmp(Param1, (ZuiAny)ZC_SplitterBar) == 0)
            return (ZuiAny)p;
        break;
    case ZM_GetType:
        return (ZuiAny)ZC_SplitterBar;
    case ZM_CoreInit:
        return (ZuiAny)TRUE;
    case ZM_CoreUnInit:
        return (ZuiAny)NULL;
    default:
        break;
    }
    return p->old_call(ProcId, cp, 0, Param1, Param2);
}




