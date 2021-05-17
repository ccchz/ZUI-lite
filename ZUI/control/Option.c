﻿#include "Option.h"
#include <core/control.h>
#include <core/resdb.h>
#include "Button.h"
ZEXPORT ZuiAny ZCALL ZuiOptionProc(int ProcId, ZuiControl cp, ZuiOption p, ZuiAny Param1, ZuiAny Param2) {
    switch (ProcId)
    {
    case ZM_OnEvent: {
        TEventUI *event = (TEventUI *)Param1;
        switch (event->Type)
        {
        case ZEVENT_LBUTTONUP: {
            if (ZuiIsPointInRect(&cp->m_rcItem, &((TEventUI*)Param1)->ptMouse)) {
                ZCCALL(ZM_Option_SetSelected, cp, (ZuiAny)(!ZCCALL(ZM_Option_GetSelected, cp, NULL, NULL)), TRUE);
            }
            break;
        }
        default:
            break;
        }
        break;
    }
    case ZM_OnPaintBorder: {
        ZuiGraphics gp = (ZuiGraphics)Param1;
        ZRect* rc = (ZRect*)&cp->m_rcItem;
        if (cp->m_dwBorderWidth && p->m_bSelected && !p->m_ResSelected && ((ZuiButton)p->old_udata)->type > 0) {
            ZuiDrawRoundRect(gp, ((ZuiButton)p->old_udata)->m_BorderColor, rc, cp->m_rRound.cx, cp->m_rRound.cy, cp->m_dwBorderWidth);
            return 0;
        }
        break;
    }
    case ZM_OnPaintStatusImage: {
        ZuiGraphics gp = (ZuiGraphics)Param1;
        ZRect *rc = &cp->m_rcItem;
        ZRect rcc = { 0 }, line1 = { 0 }, line2 = { 0 };
        ZuiButton button = p->old_udata; //需要用到Button类的变量。
        ZuiImage img;
        rcc.left = rc->left + ((rc->bottom - rc->top) - ResSize) / 2;
        rcc.top = rc->top + ((rc->bottom - rc->top) - ResSize) / 2;
        rcc.right = rcc.left + ResSize;
        rcc.bottom = rcc.top + ResSize;
        //CheckBox控件选择状态线段。
        line1.left = rcc.left+1; line1.top = rcc.top + ResSize * 0.5; line1.right = rcc.left + ResSize * 0.4; line1.bottom = rcc.bottom-2;
        line2.left = line1.right; line2.top = line1.bottom; line2.right = rcc.right-2, line2.bottom = rcc.top + ResSize * 0.1;
        if (p->m_bSelected) {
            if (button->type == 0) {
                if (p->m_ResSelected) {
                    img = p->m_ResSelected->p;
                    ZuiDrawImageEx(gp, img, rc->left, rc->top, rc->right, rc->bottom, 0, 0, img->Width, img->Height, 255);
                }
                else if (p->m_bCheck) { //绘制CheckBox控件
                    ZuiDrawRect(gp, p->m_ColorSelected, &rcc, 1);
                    ZuiDrawLine(gp, p->m_ColorSelected, &line1, 3);
                    ZuiDrawLine(gp, p->m_ColorSelected, &line2, 3);
                }
                else{  //绘制Option控件
                    ZuiDrawEllipse(gp, p->m_ColorSelected, &rcc, 1);
                    rcc.left += 2; rcc.top += 2; rcc.right -= 2; rcc.bottom -= 2;
                    ZuiFillEllipse(gp, p->m_ColorSelected, &rcc);
                }
            }
            else if (button->type == 1) {
                if (p->m_ResSelectedHot) {
                    img = p->m_ResSelectedHot->p;
                    ZuiDrawImageEx(gp, img, rc->left, rc->top, rc->right, rc->bottom, 0, 0, img->Width, img->Height, 255);
                }
                else if (p->m_bCheck) { //绘制CheckBox控件
                    ZuiDrawRect(gp, p->m_ColorSelectedHot, &rcc, 1);
                    ZuiDrawLine(gp, p->m_ColorSelectedHot, &line1, 3);
                    ZuiDrawLine(gp, p->m_ColorSelectedHot, &line2, 3);
                }
                else {  //绘制Option控件
                    ZuiDrawEllipse(gp, p->m_ColorSelectedHot, &rcc, 1);
                    rcc.left += 2; rcc.top += 2; rcc.right -= 2; rcc.bottom -= 2;
                    ZuiFillEllipse(gp, p->m_ColorSelectedHot, &rcc);
                }
            }
            else if (button->type == 2) {
                if (p->m_ResSelectedPushed) {
                    img = p->m_ResSelectedPushed->p;
                    ZuiDrawImageEx(gp, img, rc->left, rc->top, rc->right, rc->bottom, 0, 0, img->Width, img->Height, 255);
                }
                else if (p->m_bCheck) { //绘制CheckBox控件
                    ZuiDrawRect(gp, p->m_ColorSelectedPushed, &rcc, 1);
                    ZuiDrawLine(gp, p->m_ColorSelectedPushed, &line1, 3);
                    ZuiDrawLine(gp, p->m_ColorSelectedPushed, &line2, 3);
                }
                else { //绘制Option控件
                    ZuiDrawEllipse(gp, p->m_ColorSelectedPushed, &rcc, 1);
                    rcc.left += 2; rcc.top += 2; rcc.right -= 2; rcc.bottom -= 2;
                    ZuiFillEllipse(gp, p->m_ColorSelectedPushed, &rcc);
                }
            }
            else {
                if (p->m_ResSelectedDisabled) {
                    img = p->m_ResSelectedDisabled->p;
                    ZuiDrawImageEx(gp, img, rc->left, rc->top, rc->right, rc->bottom, 0, 0, img->Width, img->Height, 255);
                }
                else if (p->m_bCheck) { //绘制CheckBox控件
                    ZuiDrawRect(gp, p->m_ColorSelectedDisabled, &rcc, 1);
                    ZuiDrawLine(gp, p->m_ColorSelectedDisabled, &line1, 3);
                    ZuiDrawLine(gp, p->m_ColorSelectedDisabled, &line2, 3);
                }
                else {  //绘制Option控件
                    ZuiDrawEllipse(gp, p->m_ColorSelectedDisabled, &rcc, 1);
                    rcc.left += 2; rcc.top += 2; rcc.right -= 2; rcc.bottom -= 2;
                    ZuiFillEllipse(gp, p->m_ColorSelectedDisabled, &rcc);
                }
            }
        }
        else {
            if (button->type == 0) {
                if (button->m_ResNormal) {
                    img = button->m_ResNormal->p;
                    ZuiDrawImageEx(gp, img, rc->left, rc->top, rc->right, rc->bottom, 0, 0, img->Width, img->Height, 255);
                }
                else if (p->m_bCheck) { //绘制CheckBox控件
                    ZuiDrawRect(gp, button->m_ColorNormal, &rcc, 1);
                }
                else {  //绘制Option控件
                    ZuiDrawEllipse(gp, button->m_ColorNormal, &rcc,1);
                }
            }
            else if (button->type == 1) {
                if (button->m_ResHot) {
                    img = button->m_ResHot->p;
                    ZuiDrawImageEx(gp, img, rc->left, rc->top, rc->right, rc->bottom, 0, 0, img->Width, img->Height, 255);
                }
                else if (p->m_bCheck) { //绘制CheckBox控件
                    ZuiDrawRect(gp, button->m_ColorHot, &rcc, 1);
                }
                else {  //绘制Option控件
                    ZuiDrawEllipse(gp, button->m_ColorHot, &rcc, 1);
                }
            }
            else if (button->type == 2) {
                if (button->m_ResPushed) {
                    img = button->m_ResPushed->p;
                    ZuiDrawImageEx(gp, img, rc->left, rc->top, rc->right, rc->bottom, 0, 0, img->Width, img->Height, 255);
                }
                else if (p->m_bCheck) { //绘制CheckBox控件
                    ZuiDrawRect(gp, button->m_ColorPushed, &rcc, 1);
                }
                else {  //绘制Option控件
                    ZuiDrawEllipse(gp, button->m_ColorPushed, &rcc, 1);
                }
            }
            else {
                if (button->m_ResDisabled) {
                    img = button->m_ResDisabled->p;
                    ZuiDrawImageEx(gp, img, rc->left, rc->top, rc->right, rc->bottom, 0, 0, img->Width, img->Height, 255);
                }
                else if (p->m_bCheck) { //绘制CheckBox控件
                    ZuiDrawRect(gp, button->m_ColorDisabled, &rcc, 1);
                }
                else {  //绘制Option控件
                    ZuiDrawEllipse(gp, button->m_ColorDisabled, &rcc,1);
                }
            }
        }
        return 0;
    }
    case ZM_Option_SetSelected: {
        if (p->m_bCheck && !(ZuiBool)Param2) return 0;  //Check控件选中状态不是点击产生的，不处理。
        if (p->m_bSelected == (ZuiBool)Param1) return 0;
        p->m_bSelected = (ZuiBool)Param1;
        if (!p->m_bCheck) { //不是Option控件，不需要取消其他Option状态。
            if (Param1) {
                for (size_t i = 0; i < (size_t)ZCCALL(ZM_Layout_GetCount, cp->m_pParent, NULL, NULL); i++)
                {
                    ZuiControl pControl = ZCCALL(ZM_Layout_GetItemAt, cp->m_pParent, (ZuiAny)i, NULL);
                    if (pControl != cp)
                    {
                        ZCCALL(ZM_Option_SetSelected, pControl, FALSE, NULL);
                    }

                }
            }
            else {
                //在分组情况下反选,需要保证至少一个被选中
                if (p->m_bGroup) {
                    int select = 0;
                    for (size_t i = 0; i < (size_t)ZCCALL(ZM_Layout_GetCount, cp->m_pParent, NULL, NULL); i++)
                    {
                        ZuiControl pControl = ZCCALL(ZM_Layout_GetItemAt, cp->m_pParent, (ZuiAny)i, NULL);
                        select += ZCCALL(ZM_Option_GetSelected, pControl, NULL, NULL) ? 1 : 0;
                    }
                    if (!select) {
                        p->m_bSelected = !p->m_bSelected;
                        return 0;
                    }
                }
            }
        }
        ZuiControlNotify(_T("selectchanged"), cp, Param1, NULL);
        ZuiControlInvalidate(cp, TRUE);
        break;
    }
    case ZM_Option_SetGroup: {
        p->m_bGroup = (ZuiBool)Param1;
        break;
    }
    case ZM_Option_GetSelected: {
        return (ZuiAny)p->m_bSelected;
    }
    case ZM_Option_SetResNormal: {
        if (p->m_ResSelected)
            ZuiResDBDelRes(p->m_ResSelected);
        p->m_ResSelected = Param1;
        ZuiControlInvalidate(cp, TRUE);
        return 0;
    }
    case ZM_Option_SetResHot: {
        if (p->m_ResSelectedHot)
            ZuiResDBDelRes(p->m_ResSelectedHot);
        p->m_ResSelectedHot = Param1;
        ZuiControlInvalidate(cp, TRUE);
        return 0;
    }
    case ZM_Option_SetResPushed: {
        if (p->m_ResSelectedPushed)
            ZuiResDBDelRes(p->m_ResSelectedPushed);
        p->m_ResSelectedPushed = Param1;
        ZuiControlInvalidate(cp, TRUE);
        return 0;
    }
    case ZM_Option_SetResFocused: {
        if (p->m_ResSelectedFocused)
            ZuiResDBDelRes(p->m_ResSelectedFocused);
        p->m_ResSelectedFocused = Param1;
        ZuiControlInvalidate(cp, TRUE);
        return 0;
    }
    case ZM_Option_SetResDisabled: {
        if (p->m_ResSelectedDisabled)
            ZuiResDBDelRes(p->m_ResSelectedDisabled);
        p->m_ResSelectedDisabled = Param1;
        ZuiControlInvalidate(cp, TRUE);
        return 0;
    }
    case ZM_Option_SetColorNormal: {
        p->m_ColorSelected = (ZuiColor)Param1;
        ZuiControlInvalidate(cp, TRUE);
        return 0;
    }
    case ZM_Option_SetColorHot: {
        p->m_ColorSelectedHot = (ZuiColor)Param1;
        ZuiControlInvalidate(cp, TRUE);
        return 0;
    }
    case ZM_Option_SetColorPushed: {
        p->m_ColorSelectedPushed = (ZuiColor)Param1;
        ZuiControlInvalidate(cp, TRUE);
        return 0;
    }
    case ZM_Option_SetColorFocused: {
        p->m_ColorSelectedFocused = (ZuiColor)Param1;
        ZuiControlInvalidate(cp, TRUE);
        return 0;
    }
    case ZM_Option_SetColorDisabled: {
        p->m_ColorSelectedDisabled = (ZuiColor)Param1;
        ZuiControlInvalidate(cp, TRUE);
        return 0;
    }
    case ZM_SetAttribute: {
        if (_tcscmp(Param1, _T("group")) == 0)
            ZCCALL(ZM_Option_SetGroup, cp, (ZuiAny)(_tcsicmp(Param2, _T("true")) == 0 ? TRUE : FALSE), NULL);
        else if (_tcscmp(Param1, _T("selected")) == 0)
            ZCCALL(ZM_Option_SetSelected, cp, (ZuiAny)(_tcsicmp(Param2, _T("true")) == 0 ? TRUE : FALSE), NULL);
        else if (_tcscmp(Param1, _T("selectedimage")) == 0)
            ZCCALL(ZM_Option_SetResNormal, cp, (ZuiAny)ZuiResDBGetRes(Param2, ZREST_IMG), NULL);
        else if (_tcscmp(Param1, _T("selectedhotimage")) == 0)
            ZCCALL(ZM_Option_SetResHot, cp, (ZuiAny)ZuiResDBGetRes(Param2, ZREST_IMG), NULL);
        else if (_tcscmp(Param1, _T("selectedpushedimage")) == 0)
            ZCCALL(ZM_Option_SetResPushed, cp, (ZuiAny)ZuiResDBGetRes(Param2, ZREST_IMG), NULL);
        else if (_tcscmp(Param1, _T("selectedfocusedimage")) == 0)
            ZCCALL(ZM_Option_SetResFocused, cp, (ZuiAny)ZuiResDBGetRes(Param2, ZREST_IMG), NULL);
        else if (_tcscmp(Param1, _T("selecteddisabledimage")) == 0)
            ZCCALL(ZM_Option_SetResDisabled, cp, (ZuiAny)ZuiResDBGetRes(Param2, ZREST_IMG), NULL);
        else if (_tcsicmp(Param1, _T("selectedcolor")) == 0)
            ZCCALL(ZM_Option_SetColorNormal, cp, (ZuiAny)ZuiStr2Color(Param2), NULL);
        else if (_tcsicmp(Param1, _T("hotselectedcolor")) == 0)
            ZCCALL(ZM_Option_SetColorHot, cp, (ZuiAny)ZuiStr2Color(Param2), NULL);
        else if (_tcsicmp(Param1, _T("pushedselectedcolor")) == 0)
            ZCCALL(ZM_Option_SetColorPushed, cp, (ZuiAny)ZuiStr2Color(Param2), NULL);
        else if (_tcsicmp(Param1, _T("focusedselectedcolor")) == 0)
            ZCCALL(ZM_Option_SetColorFocused, cp, (ZuiAny)ZuiStr2Color(Param2), NULL);
        else if (_tcsicmp(Param1, _T("disabledselectedcolor")) == 0)
            ZCCALL(ZM_Option_SetColorDisabled, cp, (ZuiAny)ZuiStr2Color(Param2), NULL);
        break;
    }
    case ZM_OnCreate: {
        p = (ZuiOption)malloc(sizeof(ZOption));
        memset(p, 0, sizeof(ZOption));
        //保存原来的回调地址,创建成功后回调地址指向当前函数
        //创建继承的控件 保存数据指针
        p->old_udata = ZuiButtonProc(ZM_OnCreate, cp, 0, 0, 0);
        p->old_call = (ZCtlProc)&ZuiButtonProc;
        p->m_ColorSelected = 0xFF0C4499;		//选中的普通状态
        p->m_ColorSelectedHot = 0xFF0C5599;		//选中的点燃状态
        p->m_ColorSelectedPushed = 0xFF0C6699;	//选中的按下状态
        p->m_ColorSelectedDisabled = 0xFF989898;
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
        if (_tcsicmp(Param1, (ZuiAny)Type_Option) == 0)
            return (ZuiAny)p;
        break;
    case ZM_GetType:
        return (ZuiAny)Type_Option;
    case ZM_CoreInit:
        return (ZuiAny)TRUE;
    case ZM_CoreUnInit:
        return (ZuiAny)TRUE;
    default:
        break;
    }
    return p->old_call(ProcId, cp, p->old_udata, Param1, Param2);
}




