#include "ProgressBar.h"
#include <core/control.h>
#include <core/resdb.h>
ZEXPORT ZuiAny ZCALL ZuiProgressBarProc(int ProcId, ZuiControl cp, ZuiProgressBar p, ZuiAny Param1, ZuiAny Param2) {
    switch (ProcId)
    {
    case ZM_OnPaint: {
        ZRect rc;
        ZuiReal percent =(ZuiReal)p->m_dwPos / (ZuiReal)p->m_dwRange;
        //计算背景区域。
        if (p->m_bHorizontal) {
            rc.left = cp->m_rcItem.left;
            rc.top = cp->m_rcItem.top + ((cp->m_rcItem.bottom - cp->m_rcItem.top) - p->m_dwHeight) / 2;
            rc.right = cp->m_rcItem.right;
            rc.bottom = cp->m_rcItem.bottom - ((cp->m_rcItem.bottom - cp->m_rcItem.top) - p->m_dwHeight) / 2;
        }
        else {
            rc.left = cp->m_rcItem.left + ((cp->m_rcItem.right - cp->m_rcItem.left) - p->m_dwHeight) / 2;
            rc.top = cp->m_rcItem.top;
            rc.right = cp->m_rcItem.right - ((cp->m_rcItem.right - cp->m_rcItem.left) - p->m_dwHeight) / 2;
            rc.bottom = cp->m_rcItem.bottom;
        }
        //绘制背景区域。
        ZuiDrawFillRoundRect(cp, p->m_cBkColor, &rc, cp->m_rRound.cx, cp->m_rRound.cy);
        //计算进度区域。
        if (p->m_bHorizontal) {
            rc.right = cp->m_rcItem.left + ((cp->m_rcItem.right - cp->m_rcItem.left) * percent);
        }
        else {
            rc.top = cp->m_rcItem.bottom - ((cp->m_rcItem.bottom - cp->m_rcItem.top) * percent);
        }
        //绘制进度区域。
        ZuiDrawFillRoundRect(cp, p->m_cColor, &rc, cp->m_rRound.cx, cp->m_rRound.cy);
        if (cp->m_dwBorderWidth) {
            if (p->m_bHorizontal) {
                rc.right = cp->m_rcItem.right;
            }
            else {
                rc.top = cp->m_rcItem.top;
            }
            //绘制边框。
            ZuiDrawRoundRect(cp, cp->m_dwBorderColor, &rc, cp->m_rRound.cx, cp->m_rRound.cy, cp->m_dwBorderWidth);
        }
        return 0;
    }
    case ZM_SetAttribute: {
        if (_tcsicmp(Param1, _T("pbcolor")) == 0)
            ZCCALL(ZM_Button_SetColorNormal, cp, (ZuiAny)ZuiStr2Color(Param2), NULL);
        else if (_tcsicmp(Param1, _T("pbbkcolor")) == 0)
            ZCCALL(ZM_Button_SetColorHot, cp, (ZuiAny)ZuiStr2Color(Param2), NULL);
        else if (_tcsicmp(Param1, _T("pbheight")) == 0)
            ZCCALL(ZM_ProgressBar_SetHeight, cp, (ZuiAny)(_ttoi(Param2)), NULL);
        else if (_tcsicmp(Param1, _T("pbpos")) == 0)
            ZCCALL(ZM_ProgressBar_SetPos, cp, (ZuiAny)(_ttoi(Param2)), NULL);
        else if (_tcsicmp(Param1, _T("pbrange")) == 0)
            ZCCALL(ZM_ProgressBar_SetRange, cp, (ZuiAny)(_ttoi(Param2)), NULL);
        else if (_tcsicmp(Param1, _T("pbhorizontal")) == 0)
            ZCCALL(ZM_ProgressBar_SetHorizontal, cp, (ZuiAny)(_tcsicmp(Param2,_T("true"))? FALSE : TRUE), NULL);
        break;
    }
    case ZM_ProgressBar_SetPos: {
        p->m_dwPos = (int)Param1;
        ZuiControlInvalidate(cp, TRUE);
        return 0;
    }
    case ZM_ProgressBar_SetRange: {
        p->m_dwRange = (int)Param1;
        ZuiControlInvalidate(cp, TRUE);
        return 0;
    }
    case ZM_ProgressBar_SetHorizontal: {
        p->m_bHorizontal = (ZuiBool)Param1;
        ZuiControlInvalidate(cp, TRUE);
        return 0;
    }
    case ZM_ProgressBar_SetHeight: {
        p->m_dwHeight = (int)Param1;
        ZuiControlInvalidate(cp, TRUE);
        return 0;
    }
    case ZM_ProgressBar_SetColor: {
        p->m_cColor = (ZuiColor)Param1;
        ZuiControlInvalidate(cp, TRUE);
        return 0;
    }
    case ZM_ProgressBar_SetBackColor: {
        p->m_cBkColor = (ZuiColor)Param1;
        ZuiControlInvalidate(cp, TRUE);
        return 0;
    }
    case ZM_OnCreate: {
        ZuiProgressBar np = (ZuiProgressBar)malloc(sizeof(ZProgressBar));
        memset(np, 0, sizeof(ZProgressBar));
        //保存原来的回调地址,创建成功后回调地址指向当前函数
        np->old_call = cp->call;
        np->m_cBkColor = 0xFF383838;
        np->m_cColor = 0xFF585858;
        np->m_dwHeight = PBSIZE;
        np->m_dwPos = 0;
        np->m_dwRange = 100;  //滚动条区域默认值
        np->m_bHorizontal = TRUE; //默认水平样式。

        return np;
    }
    case ZM_OnDestroy: {
        ZCtlProc old_call = p->old_call;

        old_call(ProcId, cp, 0, Param1, Param2);
        free(p);

        return 0;
    }
    case ZM_GetObject:
        if (_tcsicmp(Param1, (ZuiAny)ZC_ProgressBar) == 0)
            return (ZuiAny)p;
        break;
    case ZM_GetType:
        return (ZuiAny)ZC_ProgressBar;
    case ZM_CoreInit:
        return (ZuiAny)TRUE;
    case ZM_CoreUnInit:
        return (ZuiAny)NULL;
    default:
        break;
    }
    return p->old_call(ProcId, cp, 0, Param1, Param2);
}




