#include "ScrollBar.h"
#include <core/control.h>
#include <core/resdb.h>
#include <core/function.h>
#include <platform/platform.h>
#include <stdlib.h>

ZEXPORT ZuiAny ZCALL ZuiScrollBarProc(int ProcId, ZuiControl cp, ZuiScrollBar p, ZuiAny Param1, ZuiAny Param2) {
    switch (ProcId)
    {
    case ZM_OnEvent: {
        TEventUI* event = (TEventUI*)Param1;
        if (!cp->m_bMouseEnabled && event->Type > ZEVENT__MOUSEBEGIN && event->Type < ZEVENT__MOUSEEND) {
            if (p->m_pOwner != NULL)
                ZCCALL(ZM_OnEvent, p->m_pOwner, Param1, NULL);
            else
                ZuiDefaultControlProc(ZM_OnEvent, cp, 0, Param1, NULL);
            return 0;
        }

        if (event->Type == ZEVENT_SETFOCUS)
        {
            return 0;
        }
        if (event->Type == ZEVENT_KILLFOCUS)
        {
            return 0;
        }
        if (event->Type == ZEVENT_LBUTTONDOWN || event->Type == ZEVENT_LDBLCLICK)
        {
            if (!cp->m_bEnabled)
                return 0;

            p->m_nLastScrollOffset = 0;
            p->m_nScrollRepeatDelay = 0;
            ZuiOsSetTimer(cp, DEFAULT_TIMERID, 50U);
            if (ZuiIsPointInRect(&p->m_rcButton1, &event->ptMouse)) {
                p->m_uButton1State |= ZSTATE_PUSHED;
                if (!p->m_bHorizontal) {
                    if (p->m_pOwner != NULL)
                        ZCCALL(ZM_Layout_LineUp, p->m_pOwner, NULL, NULL);
                    else
                        ZCCALL(ZM_ScrollBar_SetScrollPos, cp, (ZuiAny)(p->m_nScrollPos - p->m_nLineSize), NULL);
                }
                else {
                    if (p->m_pOwner != NULL)
                        ZCCALL(ZM_Layout_LineLeft, p->m_pOwner, NULL, NULL);
                    else
                        ZCCALL(ZM_ScrollBar_SetScrollPos, cp, (ZuiAny)(p->m_nScrollPos - p->m_nLineSize), NULL);
                }
            }
            else if (ZuiIsPointInRect(&p->m_rcButton2, &event->ptMouse)) {
                p->m_uButton2State |= ZSTATE_PUSHED;
                if (!p->m_bHorizontal) {
                    if (p->m_pOwner != NULL)
                        ZCCALL(ZM_Layout_LineDown, p->m_pOwner, NULL, NULL);
                    else
                        ZCCALL(ZM_ScrollBar_SetScrollPos, cp, (ZuiAny)(p->m_nScrollPos + p->m_nLineSize), NULL);
                }
                else {
                    if (p->m_pOwner != NULL)
                        ZCCALL(ZM_Layout_LineRight, p->m_pOwner, NULL, NULL);
                    else
                        ZCCALL(ZM_ScrollBar_SetScrollPos, cp, (ZuiAny)(p->m_nScrollPos + p->m_nLineSize), NULL);
                }
            }
            else if (ZuiIsPointInRect(&p->m_rcThumb, &event->ptMouse)) {
                p->m_uThumbState |= ZSTATE_CAPTURED | ZSTATE_PUSHED;
                p->ptLastMouse = event->ptMouse;
                p->m_nLastScrollPos = p->m_nScrollPos;
            }
            else {
                if (!p->m_bHorizontal) {
                    if (event->ptMouse.y < p->m_rcThumb.top) {
                        if (p->m_pOwner != NULL)
                            ZCCALL(ZM_Layout_PageUp, p->m_pOwner, NULL, NULL);
                        else
                            ZCCALL(ZM_ScrollBar_SetScrollPos, cp, (ZuiAny)(p->m_nScrollPos + cp->m_rcItem.top - cp->m_rcItem.bottom), NULL);
                    }
                    else if (event->ptMouse.y > p->m_rcThumb.bottom) {
                        if (p->m_pOwner != NULL)
                            ZCCALL(ZM_Layout_PageDown, p->m_pOwner, NULL, NULL);
                        else
                            ZCCALL(ZM_ScrollBar_SetScrollPos, cp, (ZuiAny)(p->m_nScrollPos - cp->m_rcItem.top + cp->m_rcItem.bottom), NULL);
                    }
                }
                else {
                    if (event->ptMouse.x < p->m_rcThumb.left) {
                        if (p->m_pOwner != NULL)
                            ZCCALL(ZM_Layout_PageLeft, p->m_pOwner, NULL, NULL);
                        else
                            ZCCALL(ZM_ScrollBar_SetScrollPos, cp, (ZuiAny)(p->m_nScrollPos + cp->m_rcItem.left - cp->m_rcItem.right), NULL);
                    }
                    else if (event->ptMouse.x > p->m_rcThumb.right) {
                        if (p->m_pOwner != NULL)
                            ZCCALL(ZM_Layout_PageRight, p->m_pOwner, NULL, NULL);
                        else
                            ZCCALL(ZM_ScrollBar_SetScrollPos, cp, (ZuiAny)(p->m_nScrollPos - cp->m_rcItem.left + cp->m_rcItem.right), NULL);
                    }
                }
            }
            return 0;
        }
        if (event->Type == ZEVENT_LBUTTONUP)
        {
            p->m_nScrollRepeatDelay = 0;
            p->m_nLastScrollOffset = 0;
            ZuiOsKillTimer_Id(cp, DEFAULT_TIMERID);

            if ((p->m_uThumbState & ZSTATE_CAPTURED) != 0) {
                p->m_uThumbState &= ~(ZSTATE_CAPTURED | ZSTATE_PUSHED);
                ZuiControlInvalidate(cp, TRUE);
            }
            else if ((p->m_uButton1State & ZSTATE_PUSHED) != 0) {
                p->m_uButton1State &= ~ZSTATE_PUSHED;
                ZuiControlInvalidate(cp, TRUE);
            }
            else if ((p->m_uButton2State & ZSTATE_PUSHED) != 0) {
                p->m_uButton2State &= ~ZSTATE_PUSHED;
                ZuiControlInvalidate(cp, TRUE);
            }
            return 0;
        }
        if (event->Type == ZEVENT_MOUSEMOVE)
        {
            if ((p->m_uThumbState & ZSTATE_CAPTURED) != 0) {
                if (!p->m_bHorizontal) {

                    int vRange = cp->m_rcItem.bottom - cp->m_rcItem.top - p->m_rcThumb.bottom + p->m_rcThumb.top - 2 * cp->m_cxyFixed.cx;

                    if (vRange != 0)
                        p->m_nLastScrollOffset = (event->ptMouse.y - p->ptLastMouse.y) * p->m_nRange / vRange;

                }
                else {

                    int hRange = cp->m_rcItem.right - cp->m_rcItem.left - p->m_rcThumb.right + p->m_rcThumb.left - 2 * cp->m_cxyFixed.cy;

                    if (hRange != 0)
                        p->m_nLastScrollOffset = (event->ptMouse.x - p->ptLastMouse.x) * p->m_nRange / hRange;
                }
            }
            else {
                if (p->m_bShowButton1 && ZuiIsPointInRect(&p->m_rcButton1, &event->ptMouse)) {
                    if ((p->m_uButton1State & ZSTATE_HOT) == 0) {
                        p->m_uButton1State |= ZSTATE_HOT;
                        p->m_uButton2State &= ~ZSTATE_HOT;
                        p->m_uThumbState &= ~ZSTATE_HOT;
                        ZuiControlInvalidate(cp, TRUE);
                    }
                }
                else if (p->m_bShowButton2 && ZuiIsPointInRect(&p->m_rcButton2, &event->ptMouse)) {
                    if ((p->m_uButton2State & ZSTATE_HOT) == 0) {
                        p->m_uButton2State |= ZSTATE_HOT;
                        p->m_uThumbState &= ~ZSTATE_HOT;
                        p->m_uButton1State &= ~ZSTATE_HOT;
                        ZuiControlInvalidate(cp, TRUE);
                    }
                }
                else if (ZuiIsPointInRect(&p->m_rcThumb, &event->ptMouse)) {
                    if ((p->m_uThumbState & ZSTATE_HOT) == 0) {
                        p->m_uThumbState |= ZSTATE_HOT;
                        p->m_uButton1State &= ~ZSTATE_HOT;
                        p->m_uButton2State &= ~ZSTATE_HOT;
                        ZuiControlInvalidate(cp, TRUE);
                    }
                }
                else if ((p->m_uButton1State & ZSTATE_HOT) == 0 || (p->m_uButton2State & ZSTATE_HOT) == 0 || (p->m_uThumbState & ZSTATE_HOT) == 0)
                {
                    p->m_uThumbState &= ~ZSTATE_HOT;
                    p->m_uButton1State &= ~ZSTATE_HOT;
                    p->m_uButton2State &= ~ZSTATE_HOT;
                    ZuiControlInvalidate(cp, TRUE);
                }
            }
            return 0;
        }
        if (event->Type == ZEVENT_CONTEXTMENU)
        {
            return 0;
        }
        if (event->Type == ZEVENT_TIMER && event->wParam == DEFAULT_TIMERID)
        {
            ++p->m_nScrollRepeatDelay;
            if ((p->m_uThumbState & ZSTATE_CAPTURED) != 0) {
                if (!p->m_bHorizontal) {
                    if (p->m_pOwner != NULL) {
                        ZSize sz = { 0,0 };
                        ZCCALL(ZM_Layout_GetScrollPos, p->m_pOwner, &sz, NULL);
                        sz.cy = p->m_nLastScrollPos + p->m_nLastScrollOffset;
                        ZCCALL(ZM_Layout_SetScrollPos, p->m_pOwner, &sz, NULL);
                    }
                    else
                        ZCCALL(ZM_ScrollBar_SetScrollPos, cp, (ZuiAny)(p->m_nLastScrollPos + p->m_nLastScrollOffset), NULL);
                }
                else {
                    if (p->m_pOwner != NULL) {
                        ZSize sz = { 0,0 };
                        ZCCALL(ZM_Layout_GetScrollPos, p->m_pOwner, &sz, NULL);
                        sz.cx = p->m_nLastScrollPos + p->m_nLastScrollOffset;
                        ZCCALL(ZM_Layout_SetScrollPos, p->m_pOwner, &sz, NULL);
                    }
                    else
                        ZCCALL(ZM_ScrollBar_SetScrollPos, cp, (ZuiAny)(p->m_nLastScrollPos + p->m_nLastScrollOffset), NULL);
                }
                ZuiControlInvalidate(cp, TRUE);
            }
            else if ((p->m_uButton1State & ZSTATE_PUSHED) != 0) {
                if (p->m_nScrollRepeatDelay <= 5)
                    return 0;
                if (!p->m_bHorizontal) {
                    if (p->m_pOwner != NULL)
                        ZCCALL(ZM_Layout_LineUp, p->m_pOwner, NULL, NULL);
                    else
                        ZCCALL(ZM_ScrollBar_SetScrollPos, cp, (ZuiAny)(p->m_nScrollPos - p->m_nLineSize), NULL);
                }
                else {
                    if (p->m_pOwner != NULL)
                        ZCCALL(ZM_Layout_LineLeft, p->m_pOwner, NULL, NULL);
                    else
                        ZCCALL(ZM_ScrollBar_SetScrollPos, cp, (ZuiAny)(p->m_nScrollPos - p->m_nLineSize), NULL);
                }
            }
            else if ((p->m_uButton2State & ZSTATE_PUSHED) != 0) {
                if (p->m_nScrollRepeatDelay <= 5)
                    return 0;
                if (!p->m_bHorizontal) {
                    if (p->m_pOwner != NULL)
                        ZCCALL(ZM_Layout_LineDown, p->m_pOwner, NULL, NULL);
                    else
                        ZCCALL(ZM_ScrollBar_SetScrollPos, cp, (ZuiAny)(p->m_nScrollPos + p->m_nLineSize), NULL);
                }
                else {
                    if (p->m_pOwner != NULL)
                        ZCCALL(ZM_Layout_LineRight, p->m_pOwner, NULL, NULL);
                    else
                        ZCCALL(ZM_ScrollBar_SetScrollPos, cp, (ZuiAny)(p->m_nScrollPos + p->m_nLineSize), NULL);
                }
            }
            else {
                ZPoint pt = { 0 };
                if (p->m_nScrollRepeatDelay <= 5)
                    return 0;
                GetCursorPos((LPPOINT)& pt);
                ZuiScreenToClient(cp, &pt);
                if (!p->m_bHorizontal) {
                    if (pt.y < p->m_rcThumb.top) {
                        if (p->m_pOwner != NULL)
                            ZCCALL(ZM_Layout_PageUp, p->m_pOwner, NULL, NULL);
                        else
                            ZCCALL(ZM_ScrollBar_SetScrollPos, cp, (ZuiAny)(p->m_nScrollPos + cp->m_rcItem.top - cp->m_rcItem.bottom), NULL);
                    }
                    else if (pt.y > p->m_rcThumb.bottom) {
                        if (p->m_pOwner != NULL)
                            ZCCALL(ZM_Layout_PageDown, p->m_pOwner, NULL, NULL);
                        else
                            ZCCALL(ZM_ScrollBar_SetScrollPos, cp, (ZuiAny)(p->m_nScrollPos - cp->m_rcItem.top + cp->m_rcItem.bottom), NULL);
                    }
                }
                else {
                    if (pt.x < p->m_rcThumb.left) {
                        if (p->m_pOwner != NULL)
                            ZCCALL(ZM_Layout_PageLeft, p->m_pOwner, NULL, NULL);
                        else
                            ZCCALL(ZM_ScrollBar_SetScrollPos, cp, (ZuiAny)(p->m_nScrollPos + cp->m_rcItem.left - cp->m_rcItem.right), NULL);
                    }
                    else if (pt.x > p->m_rcThumb.right) {
                        if (p->m_pOwner != NULL)
                            ZCCALL(ZM_Layout_PageRight, p->m_pOwner, NULL, NULL);
                        else
                            ZCCALL(ZM_ScrollBar_SetScrollPos, cp, (ZuiAny)(p->m_nScrollPos - cp->m_rcItem.left + cp->m_rcItem.right), NULL);
                    }
                }
            }
            return 0;
        }
        if (event->Type == ZEVENT_MOUSEENTER)
        {
            if (cp->m_bEnabled) {
                if (p->m_bShowButton1 && ZuiIsPointInRect(&p->m_rcButton1, &event->ptMouse)) {
                    p->m_uButton1State |= ZSTATE_HOT;
                }
                else if (p->m_bShowButton2 && ZuiIsPointInRect(&p->m_rcButton2, &event->ptMouse)) {
                    p->m_uButton2State |= ZSTATE_HOT;
                }
                else if (ZuiIsPointInRect(&p->m_rcThumb, &event->ptMouse)) {
                    p->m_uThumbState |= ZSTATE_HOT;
                }
                ZuiControlInvalidate(cp, TRUE);
            }
            return 0;
        }
        if (event->Type == ZEVENT_MOUSELEAVE)
        {
            if (cp->m_bEnabled) {
                p->m_uButton1State &= ~ZSTATE_HOT;
                p->m_uButton2State &= ~ZSTATE_HOT;
                p->m_uThumbState &= ~ZSTATE_HOT;
                ZuiControlInvalidate(cp, TRUE);
            }
            return 0;
        }

        if (p->m_pOwner != NULL)
            ZCCALL(ZM_OnEvent, p->m_pOwner, Param1, NULL);
        else
            ZuiDefaultControlProc(ZM_OnEvent, cp, 0, Param1, NULL);
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
    case ZM_OnPaintBkImage: {
        ZRect* rc = (ZRect*)& cp->m_rcItem;
        ZuiImage img;
        if (p->m_sImageRes) {
            img = p->m_sImageRes->p;
            memcpy(&img->src, &p->m_rcArray[5 * COL_RES - 4], sizeof(ZRect));
            ZuiDrawImageEx(cp, img, rc->left, rc->top, rc->right - rc->left, rc->bottom - rc->top, 255);
        }
        return 0;
    }
    case ZM_OnPaintStatusImage: {
        ZRect* rc = (ZRect*)& cp->m_rcItem;
        ZRect rcc;
        ZuiImage img;
        int x1, y1, x2, y2, x3, y3;
        //绘制第一个按钮
        if (p->m_bShowButton1) {

            rc = &p->m_rcButton1;
            if (!p->m_bHorizontal) {
                x1 = rc->left + cp->m_cxyFixed.cx / 2;  y1 = rc->top + SB_INSET + 1;
                x2 = rc->left + SB_INSET;               y2 = rc->bottom - SB_INSET;
                x3 = rc->right - SB_INSET - 1;          y3 = rc->bottom - SB_INSET;
            }
            else {
                x1 = rc->left + SB_INSET + 1;   y1 = rc->top + cp->m_cxyFixed.cy / 2;
                x2 = rc->right - SB_INSET;      y2 = rc->top + SB_INSET;
                x3 = rc->right - SB_INSET;      y3 = rc->bottom - SB_INSET - 1;
            }
            if (!cp->m_bEnabled) p->m_uButton1State |= ZSTATE_DISABLED;
            else p->m_uButton1State &= ~ZSTATE_DISABLED;

            if ((p->m_uButton1State & ZSTATE_DISABLED) != 0) {
                if (p->m_sImageRes) {
                    img = p->m_sImageRes->p;
                    memcpy(&img->src, &p->m_rcArray[COL_RES - 1], sizeof(ZRect));
                    ZuiDrawImageEx(cp, img, rc->left, rc->top, rc->right, rc->bottom, 255);
                }
                else {
                        ZuiDrawFilltriangle(cp, p->m_DisableColor, x1, y1, x2, y2, x3, y3);
                }
            }
            else if ((p->m_uButton1State & ZSTATE_PUSHED) != 0) {
                if (p->m_sImageRes) {
                    img = p->m_sImageRes->p;
                    memcpy(&img->src, &p->m_rcArray[COL_RES - 2], sizeof(ZRect));
                    ZuiDrawImageEx(cp, img, rc->left, rc->top, rc->right, rc->bottom, 255);
                }
                else {
                        ZuiDrawFilltriangle(cp, p->m_bPushColor, x1, y1, x2, y2, x3, y3);
                }
            }
            else if ((p->m_uButton1State & ZSTATE_HOT) != 0) {
                if (p->m_sImageRes) {
                    img = p->m_sImageRes->p;
                    memcpy(&img->src, &p->m_rcArray[COL_RES - 3], sizeof(ZRect));
                    ZuiDrawImageEx(cp, img, rc->left, rc->top, rc->right, rc->bottom, 255);
                }
                else {
                        ZuiDrawFilltriangle(cp, p->m_bHotColor, x1, y1, x2, y2, x3, y3);
                }
            }
            else {
                if (p->m_sImageRes) {
                    img = p->m_sImageRes->p;
                    memcpy(&img->src, &p->m_rcArray[COL_RES - 4], sizeof(ZRect));
                    ZuiDrawImageEx(cp, img, rc->left, rc->top, rc->right, rc->bottom, 255);
                }
                else {
                        ZuiDrawFilltriangle(cp, p->m_bNormalColor, x1, y1, x2, y2, x3, y3);
                }
            }
        }
        //绘制第二个按钮
        if (p->m_bShowButton2) {

            rc = &p->m_rcButton2;
            if (!p->m_bHorizontal) {
                x1 = rc->left + cp->m_cxyFixed.cx / 2;  y1 = rc->bottom - SB_INSET - 1;
                x2 = rc->left + SB_INSET;               y2 = rc->top + SB_INSET - 1;
                x3 = rc->right - SB_INSET - 1;          y3 = rc->top + SB_INSET - 1;
            }
            else {
                x1 = rc->right - SB_INSET - 1;  y1 = rc->top + cp->m_cxyFixed.cy / 2;
                x2 = rc->left + SB_INSET - 1;   y2 = rc->top + SB_INSET;
                x3 = rc->left + SB_INSET - 1;   y3 = rc->bottom - SB_INSET - 1;
            }
            if (!cp->m_bEnabled) p->m_uButton2State |= ZSTATE_DISABLED;
            else p->m_uButton2State &= ~ZSTATE_DISABLED;
            if ((p->m_uButton2State & ZSTATE_DISABLED) != 0) {
                if (p->m_sImageRes) {
                    img = p->m_sImageRes->p;
                    memcpy(&img->src, &p->m_rcArray[2 * COL_RES - 1], sizeof(ZRect));
                    ZuiDrawImageEx(cp, img, rc->left, rc->top, rc->right, rc->bottom, 255);
                }
                else {
                        ZuiDrawFilltriangle(cp, p->m_DisableColor, x1, y1, x2, y2, x3, y3);
                }
            }
            else if ((p->m_uButton2State & ZSTATE_PUSHED) != 0) {
                if (p->m_sImageRes) {
                    img = p->m_sImageRes->p;
                    memcpy(&img->src, &p->m_rcArray[2 * COL_RES - 2], sizeof(ZRect));
                    ZuiDrawImageEx(cp, img, rc->left, rc->top, rc->right, rc->bottom, 255);
                }
                else {
                        ZuiDrawFilltriangle(cp, p->m_bPushColor, x1, y1, x2, y2, x3, y3);
                }
            }
            else if ((p->m_uButton2State & ZSTATE_HOT) != 0) {
                if (p->m_sImageRes) {
                    img = p->m_sImageRes->p;
                    memcpy(&img->src, &p->m_rcArray[2 * COL_RES - 3], sizeof(ZRect));
                    ZuiDrawImageEx(cp, img, rc->left, rc->top, rc->right, rc->bottom, 255);
                }
                else {
                        ZuiDrawFilltriangle(cp, p->m_bHotColor, x1, y1, x2, y2, x3, y3);
                }
            }
            else {
                if (p->m_sImageRes) {
                    img = p->m_sImageRes->p;
                    memcpy(&img->src, &p->m_rcArray[2 * COL_RES - 4], sizeof(ZRect));
                    ZuiDrawImageEx(cp, img, rc->left, rc->top, rc->right, rc->bottom, 255);
                }
                else {
                        ZuiDrawFilltriangle(cp, p->m_bNormalColor, x1, y1, x2, y2, x3, y3);
                }
            }
        }

        if (p->m_rcThumb.left == 0 && p->m_rcThumb.top == 0 && p->m_rcThumb.right == 0 && p->m_rcThumb.bottom == 0) return 0;
        if (!cp->m_bEnabled) p->m_uThumbState |= ZSTATE_DISABLED;
        else p->m_uThumbState &= ~ZSTATE_DISABLED;

        rc = &p->m_rcThumb;
        if ((p->m_uThumbState & ZSTATE_DISABLED) != 0) {
            if (p->m_sImageRes) {
                img = p->m_sImageRes->p;
                memcpy(&img->src, &p->m_rcArray[3 * COL_RES - 1], sizeof(ZRect));
                ZuiDrawImageEx(cp, img, rc->left, rc->top, rc->right, rc->bottom, 255);
                memcpy(&img->src, &p->m_rcArray[4 * COL_RES - 1], sizeof(ZRect));
                if (!p->m_bHorizontal)
                    ZuiDrawImageEx(cp, img, rc->left,
                        rc->top + (rc->bottom - rc->top) / 2 - p->m_rcArray[4 * COL_RES - 1].bottom / 2,
                        rc->left + p->m_rcArray[4 * COL_RES - 1].right,
                        rc->top + (rc->bottom - rc->top) / 2 - p->m_rcArray[4 * COL_RES - 1].bottom / 2 + p->m_rcArray[4 * COL_RES - 1].bottom,
                        255);
                else
                    ZuiDrawImageEx(cp, img, rc->left + (rc->right - rc->left) / 2 - p->m_rcArray[4 * COL_RES - 1].right / 2,
                        rc->top,
                        rc->left + (rc->right - rc->left) / 2 - p->m_rcArray[4 * COL_RES - 1].right / 2 + p->m_rcArray[4 * COL_RES - 1].right,
                        rc->top + p->m_rcArray[4 * COL_RES - 1].bottom,
                        255);
            }
            else {
                if (!p->m_bHorizontal) {
                    MAKEZRECT(rcc, rc->left + SB_INSET, rc->top, rc->right - SB_INSET, rc->bottom);
                    ZuiDrawFillRect(cp, p->m_DisableColor, &rcc);
                }
                else {
                    MAKEZRECT(rcc, rc->left, rc->top + SB_INSET, rc->right, rc->bottom - SB_INSET)
                    ZuiDrawFillRect(cp, p->m_DisableColor, &rcc);
                }
            }
        }
        else if ((p->m_uThumbState & ZSTATE_PUSHED) != 0) {
            if (p->m_sImageRes) {
                img = p->m_sImageRes->p;
                memcpy(&img->src, &p->m_rcArray[3 * COL_RES - 2], sizeof(ZRect));
                ZuiDrawImageEx(cp, img, rc->left, rc->top, rc->right, rc->bottom, 255);
                memcpy(&img->src, &p->m_rcArray[4 * COL_RES - 2], sizeof(ZRect));
                if (!p->m_bHorizontal)
                    ZuiDrawImageEx(cp, img, rc->left,
                        rc->top + (rc->bottom - rc->top) / 2 - p->m_rcArray[4 * COL_RES - 1].bottom / 2,
                        rc->left + p->m_rcArray[4 * COL_RES - 1].right,
                        rc->top + (rc->bottom - rc->top) / 2 - p->m_rcArray[4 * COL_RES - 1].bottom / 2 + p->m_rcArray[4 * COL_RES - 1].bottom,
                        255);
                else
                    ZuiDrawImageEx(cp, img, rc->left + (rc->right - rc->left) / 2 - p->m_rcArray[4 * COL_RES - 1].right / 2,
                        rc->top,
                        rc->left + (rc->right - rc->left) / 2 - p->m_rcArray[4 * COL_RES - 1].right / 2 + p->m_rcArray[4 * COL_RES - 1].right,
                        rc->top + p->m_rcArray[4 * COL_RES - 1].bottom,
                        255);
            }
            else {
                if (!p->m_bHorizontal) {
                    MAKEZRECT(rcc, rc->left + SB_INSET, rc->top, rc->right - SB_INSET, rc->bottom);
                    ZuiDrawFillRect(cp, p->m_tPushColor, &rcc);
                }
                else {
                    MAKEZRECT(rcc, rc->left, rc->top + SB_INSET, rc->right, rc->bottom - SB_INSET);
                    ZuiDrawFillRect(cp, p->m_tPushColor, &rcc);
                }
            }
        }
        else if ((p->m_uThumbState & ZSTATE_HOT) != 0) {
            if (p->m_sImageRes) {
                img = p->m_sImageRes->p;
                memcpy(&img->src, &p->m_rcArray[3 * COL_RES - 3], sizeof(ZRect));
                ZuiDrawImageEx(cp, img, rc->left, rc->top, rc->right, rc->bottom, 255);
                memcpy(&img->src, &p->m_rcArray[4 * COL_RES - 3], sizeof(ZRect));
                if (!p->m_bHorizontal)
                    ZuiDrawImageEx(cp, img, rc->left,
                        rc->top + (rc->bottom - rc->top) / 2 - p->m_rcArray[4 * COL_RES - 1].bottom / 2,
                        rc->top + (rc->bottom - rc->top) / 2 - p->m_rcArray[4 * COL_RES - 1].bottom / 2 + rc->left + p->m_rcArray[4 * COL_RES - 1].right,
                        p->m_rcArray[4 * COL_RES - 1].bottom,
                        255);
                else
                    ZuiDrawImageEx(cp, img, rc->left + (rc->right - rc->left) / 2 - p->m_rcArray[4 * COL_RES - 1].right / 2,
                        rc->top,
                        rc->left + (rc->right - rc->left) / 2 - p->m_rcArray[4 * COL_RES - 1].right / 2 + p->m_rcArray[4 * COL_RES - 1].right,
                        rc->top + p->m_rcArray[4 * COL_RES - 1].bottom,
                        255);
            }
            else {
                if (!p->m_bHorizontal) {
                    MAKEZRECT(rcc, rc->left + SB_INSET, rc->top, rc->right - SB_INSET, rc->bottom);
                    ZuiDrawFillRect(cp, p->m_tHotColor, &rcc);
                }
                else {
                    MAKEZRECT(rcc, rc->left, rc->top + SB_INSET, rc->right, rc->bottom - SB_INSET);
                    ZuiDrawFillRect(cp, p->m_tHotColor, &rcc);
                }
            }
        }
        else {
            if (p->m_sImageRes) {
                img = p->m_sImageRes->p;
                memcpy(&img->src, &p->m_rcArray[3 * COL_RES - 4], sizeof(ZRect));
                ZuiDrawImageEx(cp, img, rc->left, rc->top, rc->right, rc->bottom,255);
                memcpy(&img->src, &p->m_rcArray[4 * COL_RES - 4], sizeof(ZRect));
                if (!p->m_bHorizontal)
                    ZuiDrawImageEx(cp, img, rc->left,
                        rc->top + (rc->bottom - rc->top) / 2 - p->m_rcArray[4 * COL_RES - 1].bottom / 2,
                        rc->left + p->m_rcArray[4 * COL_RES - 1].right,
                        rc->top + (rc->bottom - rc->top) / 2 - p->m_rcArray[4 * COL_RES - 1].bottom / 2 + p->m_rcArray[4 * COL_RES - 1].bottom,
                        255);
                else
                    ZuiDrawImageEx(cp, img, rc->left + (rc->right - rc->left) / 2 - p->m_rcArray[4 * COL_RES - 1].right / 2,
                        rc->top,
                        rc->left + (rc->right - rc->left) / 2 - p->m_rcArray[4 * COL_RES - 1].right / 2 + p->m_rcArray[4 * COL_RES - 1].right,
                        rc->top + p->m_rcArray[4 * COL_RES - 1].bottom,
                        255);
            }
            else {
                if (!p->m_bHorizontal) {
                    MAKEZRECT(rcc, rc->left + SB_INSET, rc->top, rc->right - SB_INSET, rc->bottom);
                    ZuiDrawFillRect(cp, p->m_tNormalColor, &rcc);
                }
                else {
                    MAKEZRECT(rcc, rc->left, rc->top + SB_INSET, rc->right, rc->bottom - SB_INSET);
                    ZuiDrawFillRect(cp, p->m_tNormalColor, &rcc);
                }
            }
        }
        return 0;
    }
    case ZM_SetPos: {
        ZuiDefaultControlProc(ProcId, cp, 0, Param1, Param2);
        ZRect rc = cp->m_rcItem;

        if (p->m_bHorizontal) {
            int cx = rc.right - rc.left;
            if (p->m_bShowButton1) cx -= cp->m_cxyFixed.cy;
            if (p->m_bShowButton2) cx -= cp->m_cxyFixed.cy;
            if (cx > cp->m_cxyFixed.cy) {
                p->m_rcButton1.left = rc.left;
                p->m_rcButton1.top = rc.top;
                if (p->m_bShowButton1) {
                    p->m_rcButton1.right = rc.left + cp->m_cxyFixed.cy;
                    p->m_rcButton1.bottom = rc.top + cp->m_cxyFixed.cy;
                }
                else {
                    p->m_rcButton1.right = p->m_rcButton1.left;
                    p->m_rcButton1.bottom = p->m_rcButton1.top;
                }

                p->m_rcButton2.top = rc.top;
                p->m_rcButton2.right = rc.right;
                if (p->m_bShowButton2) {
                    p->m_rcButton2.left = rc.right - cp->m_cxyFixed.cy;
                    p->m_rcButton2.bottom = rc.top + cp->m_cxyFixed.cy;
                }
                else {
                    p->m_rcButton2.left = p->m_rcButton2.right;
                    p->m_rcButton2.bottom = p->m_rcButton2.top;
                }

                p->m_rcThumb.top = rc.top;
                p->m_rcThumb.bottom = rc.top + cp->m_cxyFixed.cy;
                if (p->m_nRange > 0) {
                    int cxThumb = cx * (rc.right - rc.left) / (p->m_nRange + rc.right - rc.left);
                    
                    p->m_rcThumb.left = (int)((float)p->m_nScrollPos / (float)p->m_nRange * (cx - cxThumb)) + p->m_rcButton1.right;
                    if (cxThumb < cp->m_cxyFixed.cy) cxThumb = cp->m_cxyFixed.cy;
                    p->m_rcThumb.right = p->m_rcThumb.left + cxThumb;
                    if (p->m_rcThumb.right > p->m_rcButton2.left) {
                        p->m_rcThumb.left = p->m_rcButton2.left - cxThumb;
                        p->m_rcThumb.right = p->m_rcButton2.left;
                    }
                }
                else {
                    p->m_rcThumb.left = p->m_rcButton1.right;
                    p->m_rcThumb.right = p->m_rcButton2.left;
                }
            }
            else {
                int cxButton = (rc.right - rc.left) / 2;
                if (cxButton > cp->m_cxyFixed.cy) cxButton = cp->m_cxyFixed.cy;
                p->m_rcButton1.left = rc.left;
                p->m_rcButton1.top = rc.top;
                if (p->m_bShowButton1) {
                    p->m_rcButton1.right = rc.left + cxButton;
                    p->m_rcButton1.bottom = rc.top + cp->m_cxyFixed.cy;
                }
                else {
                    p->m_rcButton1.right = p->m_rcButton1.left;
                    p->m_rcButton1.bottom = p->m_rcButton1.top;
                }

                p->m_rcButton2.top = rc.top;
                p->m_rcButton2.right = rc.right;
                if (p->m_bShowButton2) {
                    p->m_rcButton2.left = rc.right - cxButton;
                    p->m_rcButton2.bottom = rc.top + cp->m_cxyFixed.cy;
                }
                else {
                    p->m_rcButton2.left = p->m_rcButton2.right;
                    p->m_rcButton2.bottom = p->m_rcButton2.top;
                }

                memset(&p->m_rcThumb, 0, sizeof(p->m_rcThumb));
            }
        }
        else {
            int cy = rc.bottom - rc.top;
            if (p->m_bShowButton1) cy -= cp->m_cxyFixed.cx;
            if (p->m_bShowButton2) cy -= cp->m_cxyFixed.cx;
            if (cy > cp->m_cxyFixed.cx) {
                p->m_rcButton1.left = rc.left;
                p->m_rcButton1.top = rc.top;
                if (p->m_bShowButton1) {
                    p->m_rcButton1.right = rc.left + cp->m_cxyFixed.cx;
                    p->m_rcButton1.bottom = rc.top + cp->m_cxyFixed.cx;
                }
                else {
                    p->m_rcButton1.right = p->m_rcButton1.left;
                    p->m_rcButton1.bottom = p->m_rcButton1.top;
                }

                p->m_rcButton2.left = rc.left;
                p->m_rcButton2.bottom = rc.bottom;
                if (p->m_bShowButton2) {
                    p->m_rcButton2.top = rc.bottom - cp->m_cxyFixed.cx;
                    p->m_rcButton2.right = rc.left + cp->m_cxyFixed.cx;
                }
                else {
                    p->m_rcButton2.top = p->m_rcButton2.bottom;
                    p->m_rcButton2.right = p->m_rcButton2.left;
                }

                p->m_rcThumb.left = rc.left;
                p->m_rcThumb.right = rc.left + cp->m_cxyFixed.cx;
                if (p->m_nRange > 0) {
                    int cyThumb = cy * (rc.bottom - rc.top) / (p->m_nRange + rc.bottom - rc.top);
                    
                    p->m_rcThumb.top = (int)((float) p->m_nScrollPos / (float) p->m_nRange * (cy - cyThumb))  + p->m_rcButton1.bottom;
                    if (cyThumb < cp->m_cxyFixed.cx) cyThumb = cp->m_cxyFixed.cx;
                    p->m_rcThumb.bottom = p->m_rcThumb.top + cyThumb;
                    if (p->m_rcThumb.bottom > p->m_rcButton2.top) {
                        p->m_rcThumb.top = p->m_rcButton2.top - cyThumb;
                        p->m_rcThumb.bottom = p->m_rcButton2.top;
                    }
                }
                else {
                    p->m_rcThumb.top = p->m_rcButton1.bottom;
                    p->m_rcThumb.bottom = p->m_rcButton2.top;
                }
            }
            else {
                int cyButton = (rc.bottom - rc.top) / 2;
                if (cyButton > cp->m_cxyFixed.cx) cyButton = cp->m_cxyFixed.cx;
                p->m_rcButton1.left = rc.left;
                p->m_rcButton1.top = rc.top;
                if (p->m_bShowButton1) {
                    p->m_rcButton1.right = rc.left + cp->m_cxyFixed.cx;
                    p->m_rcButton1.bottom = rc.top + cyButton;
                }
                else {
                    p->m_rcButton1.right = p->m_rcButton1.left;
                    p->m_rcButton1.bottom = p->m_rcButton1.top;
                }

                p->m_rcButton2.left = rc.left;
                p->m_rcButton2.bottom = rc.bottom;
                if (p->m_bShowButton2) {
                    p->m_rcButton2.top = rc.bottom - cyButton;
                    p->m_rcButton2.right = rc.left + cp->m_cxyFixed.cx;
                }
                else {
                    p->m_rcButton2.top = p->m_rcButton2.bottom;
                    p->m_rcButton2.right = p->m_rcButton2.left;
                }

                memset(&p->m_rcThumb, 0, sizeof(p->m_rcThumb));
            }
        }
        return 0;
    }
    case ZM_ScrollBar_SetHorizontal: {
        if (p->m_bHorizontal == (ZuiBool)Param1)
            return 0;

        p->m_bHorizontal = (ZuiBool)Param1;
        if (p->m_bHorizontal) {
            if (cp->m_cxyFixed.cy == 0) {
                cp->m_cxyFixed.cx = 0;
                cp->m_cxyFixed.cy = DEFAULT_SCROLLBAR_SIZE;
            }
        }
        else {
            if (cp->m_cxyFixed.cx == 0) {
                cp->m_cxyFixed.cx = DEFAULT_SCROLLBAR_SIZE;
                cp->m_cxyFixed.cy = 0;
            }
        }

        //if (p->m_pOwner)
        //    ZuiControlNeedUpdate(p->m_pOwner);
        //else
        //    ZuiControlNeedParentUpdate(cp);
        return 0;
    }
    case ZM_ScrollBar_SetScrollPos: {
        if (p->m_nScrollPos == (int)Param1)
            return 0;

        p->m_nScrollPos = (int)Param1;
        if (p->m_nScrollPos < 0) p->m_nScrollPos = 0;
        if (p->m_nScrollPos > p->m_nRange) p->m_nScrollPos = p->m_nRange;
        ZCCALL(ZM_SetPos, cp, &cp->m_rcItem, (ZuiAny)ZuiOnSize);
        break;
    }
    case ZM_ScrollBar_GetScrollPos: {
        return (ZuiAny)p->m_nScrollPos;
    }
    case ZM_ScrollBar_GetScrollRange: {
        return (ZuiAny)p->m_nRange;
    }
    case ZM_ScrollBar_SetOwner: {
        p->m_pOwner = Param1;
        break;
    }
    case ZM_ScrollBar_SetScrollRange: {
        if (p->m_nRange == (int)Param1)
            return 0;

        p->m_nRange = (int)Param1;
        if (p->m_nRange < 0) p->m_nRange = 0;
        if (p->m_nScrollPos > p->m_nRange) p->m_nScrollPos = p->m_nRange;
        ZCCALL(ZM_SetPos, cp, &cp->m_rcItem, (ZuiAny)ZuiOnSize);
        break;
    }
    case ZM_SetAttribute: {
        ZuiAttribute zAttr = (ZuiAttribute)Param1;
        if (_tcsicmp(zAttr->name, _T("sbtnormalcolor")) == 0) {
            ZCCALL(ZM_ScrollBar_tN_Color, cp, (ZuiAny)ZuiStr2Color(zAttr->value), Param2);
            break;
        }
        else if (_tcsicmp(zAttr->name, _T("sbthotcolor")) == 0) {
            ZCCALL(ZM_ScrollBar_tH_Color, cp, (ZuiAny)ZuiStr2Color(zAttr->value), Param2);
            break;
        }
        else if (_tcsicmp(zAttr->name, _T("sbtpushcolor")) == 0) {
            ZCCALL(ZM_ScrollBar_tP_Color, cp, (ZuiAny)ZuiStr2Color(zAttr->value), Param2);
            break;
        }
        else if (_tcsicmp(zAttr->name, _T("sbbnormalcolor")) == 0) {
            ZCCALL(ZM_ScrollBar_bN_Color, cp, (ZuiAny)ZuiStr2Color(zAttr->value), Param2);
            break;
        }
        else if (_tcsicmp(zAttr->name, _T("sbbhotcolor")) == 0) {
            ZCCALL(ZM_ScrollBar_bH_Color, cp, (ZuiAny)ZuiStr2Color(zAttr->value), Param2);
            break;
        }
        else if (_tcsicmp(zAttr->name, _T("sbbpushcolor")) == 0) {
            ZCCALL(ZM_ScrollBar_bP_Color, cp, (ZuiAny)ZuiStr2Color(zAttr->value), Param2);
            break;
        }
        else if (_tcsicmp(zAttr->name, _T("sbdisablecolor")) == 0) {
            ZCCALL(ZM_ScrollBar_Di_Color, cp, (ZuiAny)ZuiStr2Color(zAttr->value), Param2);
            break;
        }
        else if (_tcsicmp(zAttr->name, _T("sbbkcolor")) == 0) {
            ZCCALL(ZM_SetBkColor, cp, (ZuiAny)ZuiStr2Color(zAttr->value), Param2);
            break;
        }
        else if (_tcsicmp(zAttr->name, _T("sbb1show")) == 0) {
            ZCCALL(ZM_ScrollBar_B1_Show, cp, (ZuiAny)(_tcscmp(zAttr->value, _T("true")) == 0 ? TRUE : FALSE), Param2);
            break;
        }
        else if (_tcsicmp(zAttr->name, _T("sbb2show")) == 0) {
            ZCCALL(ZM_ScrollBar_B2_Show, cp, (ZuiAny)(_tcscmp(zAttr->value, _T("true")) == 0 ? TRUE : FALSE), Param2);
            break;
        }
        else if (_tcsicmp(zAttr->name, _T("sbimageres")) == 0) {
            ZCCALL(ZM_ScrollBar_SetImageRes, cp, ZuiResDBGetRes(zAttr->value, ZREST_IMG), Param2);
            break;
        }
        else if (_tcsicmp(zAttr->name, _T("sbvsrc")) == 0) {
            if (p->m_bHorizontal)
                break;
            ZRect rcTmp = { 0 };
            ZuiText pstr = NULL;
            rcTmp.left = _tcstol(zAttr->value, &pstr, 10);  ASSERT(pstr);
            rcTmp.top = _tcstol(pstr + 1, &pstr, 10);    ASSERT(pstr);
            rcTmp.right = _tcstol(pstr + 1, &pstr, 10);  ASSERT(pstr);
            rcTmp.bottom = _tcstol(pstr + 1, &pstr, 10); ASSERT(pstr);

            for (int i = 0; i < ROW_RES; i++) {
                for (int j = 0; j < COL_RES; j++) {
                    p->m_rcArray[i * COL_RES + j].left = rcTmp.left + j * rcTmp.right + j * RES_SEP;
                    p->m_rcArray[i * COL_RES + j].top = rcTmp.top + i * rcTmp.bottom + i * RES_SEP;
                    p->m_rcArray[i * COL_RES + j].right = rcTmp.right;
                    p->m_rcArray[i * COL_RES + j].bottom = rcTmp.bottom;
                }
            }
            break;
        }
        else if (_tcsicmp(zAttr->name, _T("sbhsrc")) == 0) {
            if (!p->m_bHorizontal)
                break;
            ZRect rcTmp = { 0 };
            ZuiText pstr = NULL;
            rcTmp.left = _tcstol(zAttr->value, &pstr, 10);  ASSERT(pstr);
            rcTmp.top = _tcstol(pstr + 1, &pstr, 10);    ASSERT(pstr);
            rcTmp.right = _tcstol(pstr + 1, &pstr, 10);  ASSERT(pstr);
            rcTmp.bottom = _tcstol(pstr + 1, &pstr, 10); ASSERT(pstr);

            for (int i = 0; i < ROW_RES; i++) {
                for (int j = 0; j < COL_RES; j++) {
                    p->m_rcArray[i * COL_RES + j].left = rcTmp.left + j * rcTmp.right + j * RES_SEP;
                    p->m_rcArray[i * COL_RES + j].top = rcTmp.top + i * rcTmp.bottom + i * RES_SEP;
                    p->m_rcArray[i * COL_RES + j].right = rcTmp.right;
                    p->m_rcArray[i * COL_RES + j].bottom = rcTmp.bottom;
                }
            }
            break;
        }
        break;
    }
    case ZM_ScrollBar_SetImageRes: {
        if (p->m_sImageRes)
            ZuiResDBDelRes(p->m_sImageRes);
        p->m_sImageRes = (ZuiRes)Param1;
        if (!Param2)
            ZuiControlInvalidate(cp, TRUE);
        return 0;
    }
    case ZM_ScrollBar_B1_Show: {
        p->m_bShowButton1 = (ZuiBool)Param1;
        if(!Param2)
            ZuiControlNeedUpdate(cp);
        return 0;
    }
    case ZM_ScrollBar_B2_Show: {
        p->m_bShowButton1 = (ZuiBool)Param1;
        if (!Param2)
            ZuiControlNeedUpdate(cp);
        return 0;
    }
    case ZM_ScrollBar_tN_Color: {
        p->m_tNormalColor = (ZuiColor)Param1;
        break;
    }
    case ZM_ScrollBar_tH_Color: {
        p->m_tHotColor = (ZuiColor)Param1;
        break;
    }
    case ZM_ScrollBar_tP_Color: {
        p->m_tPushColor = (ZuiColor)Param1;
        break;
    }
    case ZM_ScrollBar_bN_Color: {
        p->m_bNormalColor = (ZuiColor)Param1;
        break;
    }
    case ZM_ScrollBar_bH_Color: {
        p->m_bHotColor = (ZuiColor)Param1;
        break;
    }
    case ZM_ScrollBar_bP_Color: {
        p->m_bPushColor = (ZuiColor)Param1;
        break;
    }
    case ZM_ScrollBar_Di_Color: {
        p->m_DisableColor = (ZuiColor)Param1;
        break;
    }
    case ZM_OnCreate: {
        p = (ZuiScrollBar)malloc(sizeof(ZScrollBar));
        memset(p, 0, sizeof(ZScrollBar));
        //保存原来的回调地址,创建成功后回调地址指向当前函数
        p->old_call = cp->call;

        p->m_nRange = 0;
        p->m_nLineSize = 8;
        p->m_bShowButton1 = TRUE;
        p->m_bShowButton2 = TRUE;
        cp->m_cxyFixed.cx = DEFAULT_SCROLLBAR_SIZE;
        p->m_tNormalColor = 0xFF888888;
        p->m_tHotColor = 0xFFA8A8A8;
        p->m_tPushColor = 0xFFD8D8D8;
        p->m_DisableColor = 0xFF282828;
        p->m_bNormalColor = 0xFF888888;
        p->m_bHotColor = 0xFF1874CD;
        p->m_bPushColor = 0xFF1C86EE;
        return p;
    }
    case ZM_OnDestroy: {
        p->old_call(ProcId, cp, 0, Param1, Param2);

        if (p->m_sImageRes)
            ZuiResDBDelRes(p->m_sImageRes);
        free(p);
        return 0;
    }
    case ZM_GetObject:
        if (_tcsicmp(Param1, (ZuiAny)ZC_ScrollBar) == 0)
            return (ZuiAny)p;
        break;
    case ZM_GetType:
        return (ZuiAny)ZC_ScrollBar;
    case ZM_CoreInit: {
        return (ZuiAny)TRUE;
    }
    case ZM_CoreUnInit:
        return (ZuiAny)NULL;
    default:
        break;
    }
    return p->old_call(ProcId, cp, 0, Param1, Param2);
}
