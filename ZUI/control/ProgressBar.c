#include "ProgressBar.h"
#include <core/control.h>
#include <core/resdb.h>
ZEXPORT ZuiAny ZCALL ZuiProgressBarProc(int ProcId, ZuiControl cp, ZuiProgressBar p, ZuiAny Param1, ZuiAny Param2) {
    switch (ProcId)
    {
    case ZM_OnPaint: {
        ZuiGraphics gp = (ZuiGraphics)Param1;
        ZRect rc;
        rc.left = cp->m_rcItem.left;
        rc.top = cp->m_rcItem.top + ((cp->m_rcItem.bottom - cp->m_rcItem.top) - p->pbHeight) / 2;
        rc.right = cp->m_rcItem.right;
        rc.bottom = cp->m_rcItem.bottom - ((cp->m_rcItem.bottom - cp->m_rcItem.top) - p->pbHeight) / 2;
        if (cp->m_rRound.cx && cp->m_rRound.cy) {
            ZuiDrawFillRoundRect(gp, p->pbBkColor, &rc, cp->m_rRound.cx, cp->m_rRound.cy);
            rc.right = cp->m_rcItem.left + ((cp->m_rcItem.right - cp->m_rcItem.left) * p->pbPercent) / 100;
            ZuiDrawFillRoundRect(gp, p->pbColor, &rc, cp->m_rRound.cx, cp->m_rRound.cy);
            if (cp->m_dwBorderWidth) {
                rc.right = cp->m_rcItem.right;
                ZuiDrawRoundRect(gp, cp->m_dwBorderColor, &rc, cp->m_rRound.cx, cp->m_rRound.cy, cp->m_dwBorderWidth);
            }
        }
        else {
            ZuiDrawFillRect(gp, p->pbBkColor, &rc);
            rc.right = cp->m_rcItem.left + ((cp->m_rcItem.right - cp->m_rcItem.left) * p->pbPercent) / 100;
            ZuiDrawFillRect(gp, p->pbColor, &rc);
            if (cp->m_dwBorderWidth) {
                rc.right = cp->m_rcItem.right;
                ZuiDrawRect(gp, cp->m_dwBorderColor, &rc, cp->m_dwBorderWidth);
            }
        }
        return 0;
    }
    case ZM_SetAttribute: {
        if (_tcsicmp(Param1, _T("pbcolor")) == 0) ZCCALL(ZM_Button_SetColorNormal, cp, (ZuiAny)ZuiStr2Color(Param2), NULL);
        else if (_tcsicmp(Param1, _T("pbbkcolor")) == 0) ZCCALL(ZM_Button_SetColorHot, cp, (ZuiAny)ZuiStr2Color(Param2), NULL);
        else if (_tcsicmp(Param1, _T("pbheight")) == 0) ZCCALL(ZM_ProgressBar_SetHeight, cp, (ZuiAny)(_wtoi(Param2)), NULL);
        else if (_tcsicmp(Param1, _T("pbpercent")) == 0) ZCCALL(ZM_ProgressBar_SetPercet, cp, (ZuiAny)(_wtoi(Param2)), NULL);
        break;
    }
    case ZM_ProgressBar_SetPercet: {
        p->pbPercent = (int)Param1;
        return 0;
    }
    case ZM_ProgressBar_SetHeight: {
        p->pbHeight = (int)Param1;
        return 0;
    }
    case ZM_ProgressBar_SetColor: {
        p->pbColor = (ZuiColor)Param1;
        return 0;
    }
    case ZM_ProgressBar_SetBackColor: {
        p->pbBkColor = (ZuiColor)Param1;
        return 0;
    }
    case ZM_OnCreate: {
        ZuiProgressBar np = (ZuiProgressBar)malloc(sizeof(ZProgressBar));
        memset(np, 0, sizeof(ZProgressBar));
        //保存原来的回调地址,创建成功后回调地址指向当前函数
        np->old_call = cp->call;
        np->pbBkColor = 0xFF383838;
        np->pbColor = 0xFF585858;
        np->pbHeight = PBSIZE;

        return np;
    }
    case ZM_OnDestroy: {
        ZCtlProc old_call = p->old_call;

        old_call(ProcId, cp, 0, Param1, Param2);
        free(p);

        return 0;
    }
    case ZM_GetObject:
        if (_tcsicmp(Param1, (ZuiAny)Type_ProgressBar) == 0)
            return (ZuiAny)p;
        break;
    case ZM_GetType:
        return (ZuiAny)Type_ProgressBar;
    case ZM_CoreInit:
        return (ZuiAny)TRUE;
    case ZM_CoreUnInit:
        return (ZuiAny)NULL;
    default:
        break;
    }
    return p->old_call(ProcId, cp, 0, Param1, Param2);
}




