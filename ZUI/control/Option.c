#include "Option.h"
#include <core/control.h>
#include <core/resdb.h>
#include "Button.h"
ZEXPORT ZuiAny ZCALL ZuiOptionDrawResStyle(ZuiControl cp, ZuiOption p, ZuiColor color) {
    ZRect* rc = &cp->m_rcItem;
    ZRect rcc = { 0 }, line1 = { 0 }, line2 = { 0 };
    ZuiButton button = p->old_udata;
    ZRound rd = { 1,1,1,1 };
    rcc.left = rc->left + (ResSize) / 4;
    rcc.top = rc->top + ((rc->bottom - rc->top) - ResSize) / 2;
    rcc.right = rcc.left + ResSize;
    rcc.bottom = rcc.top + ResSize;

    switch (p->m_dwOptionStyle)
    {
    case 1: {  //圆形
        if (p->m_bSelected) {
            if (button->m_dwType >= 1) {
                ZuiFillEllipse(cp, color & 0x33FFFFFF, &rcc);
            }
            ZuiDrawEllipse(cp, color, &rcc, 1);
            rcc.left += 4; rcc.top += 4; rcc.right -= 4; rcc.bottom -= 4;
            ZuiFillEllipse(cp, color, &rcc);
        }
        else {
            if (button->m_dwType >= 1) {
                ZuiFillEllipse(cp, color & 0x33FFFFFF, &rcc);
            }
            ZuiDrawEllipse(cp, color, &rcc, 1);
        }
        break;
    }
    case 2: { //方形        
        if (p->m_bSelected) {
            //CheckBox控件选择状态线段。
            if (button->m_dwType >= 1) {
                ZuiDrawFillRoundRect(cp, p->m_ColorSelectedHot & 0x33FFFFFF, &rcc, &cp->m_rRound);
                color = p->m_ColorSelected;
            }
            ZuiDrawRoundRect(cp, color, &rcc, &cp->m_rRound, 1);
            line1.left = rcc.left + 3;
            line1.top = rcc.top + (int)(ResSize * 0.5);
            line1.right = rcc.left + (int)(ResSize * 0.4);
            line1.bottom = rcc.bottom - 4;
            line2.left = line1.right;
            line2.top = line1.bottom;
            line2.right = rcc.right - 3;
            line2.bottom = rcc.top + (int)(ResSize * 0.3);
            ZuiDrawLine(cp, color, &line1, 1);
            ZuiDrawLine(cp, color, &line2, 1);
        }
        else {
            if (button->m_dwType >= 1) {
                ZuiDrawFillRoundRect(cp, color & 0x33FFFFFF, &rcc, &cp->m_rRound);
            }
            ZuiDrawRoundRect(cp, color, &rcc, &cp->m_rRound, 1);
        }

        break;
    }
    case 3: {
        rcc.right += ResSize;
        if (p->m_bSelected) {
            ZuiDrawFillRoundRect(cp, color, &rcc, &cp->m_rRound);
            if (button->m_dwType < 1) {
                rcc.right -= 4;
                rcc.top += 4;
                rcc.bottom -= 4;
                rcc.left = rcc.right - rcc.bottom + rcc.top;
                ZuiFillEllipse(cp, 0xFF000000, &rcc);
            }
        }
        else {
            ZuiDrawFillRoundRect(cp, color & 0x33FFFFFF, &rcc, &cp->m_rRound);
            ZuiDrawRoundRect(cp, color, &rcc, &cp->m_rRound, 1);
            if (button->m_dwType < 1) {
                rcc.left += 4;
                rcc.top += 4;
                rcc.bottom -= 4;
                rcc.right = rcc.left + rcc.bottom - rcc.top;
                ZuiFillEllipse(cp, 0x99FFFFFF, &rcc);
            }
        }
        break;
    }
    case 5: {
        if (p->m_bSelected) {
            ZuiDrawFillRoundRect(cp, color, rc, &cp->m_rRound);
            rcc.left = rc->left + cp->m_dwBorderWidth;
            rcc.top = rc->top + cp->m_dwBorderWidth + ResSize / 4;
            rcc.right = rcc.left + ResSize / 4;
            rcc.bottom = rc->bottom - cp->m_dwBorderWidth - ResSize / 4;
            ZuiDrawFillRoundRect(cp, p->m_ColorSelected, &rcc, &rd);
        }
        else if (button->m_dwType >= 1) {
            ZuiDrawFillRoundRect(cp, color, rc, &cp->m_rRound);
        }
        break;
    }
    case 6: {
        if (p->m_bSelected) {
            ZuiDrawFillRoundRect(cp, color, rc, &cp->m_rRound);
            rcc.left = rc->right - cp->m_dwBorderWidth - ResSize / 4;
            rcc.top = rc->top + cp->m_dwBorderWidth + ResSize / 4;
            rcc.right = rc->right - cp->m_dwBorderWidth;
            rcc.bottom = rc->bottom - cp->m_dwBorderWidth - ResSize / 4;
            ZuiDrawFillRoundRect(cp, p->m_ColorSelected, &rcc, &rd);
        }
        else if (button->m_dwType >= 1) {
            ZuiDrawFillRoundRect(cp, color, rc, &cp->m_rRound);
        }
        break;
    }
    case 7: {
        if (p->m_bSelected) {
            ZuiDrawFillRoundRect(cp, color, rc, &cp->m_rRound);
            rcc.left = rc->left + cp->m_dwBorderWidth + ResSize / 4;
            rcc.top = rc->bottom - cp->m_dwBorderWidth - ResSize / 4 ;
            rcc.right = rc->right - cp->m_dwBorderWidth - ResSize / 4;
            rcc.bottom = rc->bottom - cp->m_dwBorderWidth;
            ZuiDrawFillRoundRect(cp, p->m_ColorSelected, &rcc, &rd);
        }
        else if (button->m_dwType >= 1) {
            ZuiDrawFillRoundRect(cp, color, rc, &cp->m_rRound);
        }
        break;
    }
    case 8: {
        if (p->m_bSelected) {
            ZuiDrawFillRoundRect(cp, color, rc, &cp->m_rRound);
            rcc.left = rc->left + cp->m_dwBorderWidth + ResSize / 4;
            rcc.top = rc->top + cp->m_dwBorderWidth;
            rcc.right = rc->right - cp->m_dwBorderWidth - ResSize / 4;
            rcc.bottom = rcc.top + ResSize / 4;
            ZuiDrawFillRoundRect(cp, p->m_ColorSelected, &rcc, &rd);
        }
        else if (button->m_dwType >= 1) {
            ZuiDrawFillRoundRect(cp, color, rc, &cp->m_rRound);
        }
        break;
    }
    default:{ //绘制背景色
        ZuiDrawFillRoundRect(cp, color, rc, &cp->m_rRound);
        break;
    }
    }
    return NULL;
}
ZEXPORT ZuiAny ZCALL ZuiOptionProc(int ProcId, ZuiControl cp, ZuiOption p, ZuiAny Param1, ZuiAny Param2) {
    switch (ProcId)
    {
    case ZM_OnEvent: {
        TEventUI *event = (TEventUI *)Param1;
        switch (event->Type)
        {
        case ZEVENT_LDBLCLICK:
        case ZEVENT_LBUTTONUP: {
            p->old_call(ProcId, cp, p->old_udata, Param1, Param2); //先处理父类过程。
            if (ZuiIsPointInRect(&cp->m_rcItem, &((TEventUI*)Param1)->ptMouse)) {
                ZCCALL(ZM_Option_SetSelected, cp, (ZuiAny)(!ZCCALL(ZM_Option_GetSelected, cp, NULL, NULL)), NULL);
            }
            return 0;
        }
        default:
            break;
        }
        break;
    }
    case ZM_OnPaintBorder: {
        ZRect* rc = (ZRect*)&cp->m_rcItem;
        if (cp->m_dwBorderWidth && p->m_bSelected && !p->m_ResSelected && ((ZuiButton)p->old_udata)->m_dwType > 0) {
            ZuiDrawRoundRect(cp, cp->m_dwBorderColor2, rc, &cp->m_rRound, cp->m_dwBorderWidth);
            return 0;
        }
        break;
    }
    case ZM_OnPaintStatusImage: {
        ZRect *rc = &cp->m_rcItem;
        ZuiColor zcolor;
        ZuiButton button = p->old_udata; //需要用到Button类的变量。
        ZuiImage img;

        if (p->m_bSelected) {
            if (button->m_dwType == 0) {
                if (p->m_ResSelected) {
                    img = p->m_ResSelected->p;
                    ZuiDrawImageEx(cp, img, rc->left, rc->top, rc->right, rc->bottom, 255);
                }
                else{  //绘制Option控件
                    if (p->m_dwOptionStyle > 4)
                        zcolor = button->m_ColorNormal;
                    else
                        zcolor = p->m_ColorSelected;
                    ZuiOptionDrawResStyle(cp, p, zcolor);
                }
            }
            else if (button->m_dwType == 1) {
                if (p->m_ResSelectedHot) {
                    img = p->m_ResSelectedHot->p;
                    ZuiDrawImageEx(cp, img, rc->left, rc->top, rc->right, rc->bottom, 255);
                }
                else {  //绘制Option控件
                    if (p->m_dwOptionStyle > 4)
                        zcolor = button->m_ColorHot;
                    else
                        zcolor = p->m_ColorSelectedHot;
                    ZuiOptionDrawResStyle(cp, p, zcolor);
                }
            }
            else if (button->m_dwType == 2) {
                if (p->m_ResSelectedPushed) {
                    img = p->m_ResSelectedPushed->p;
                    ZuiDrawImageEx(cp, img, rc->left, rc->top, rc->right, rc->bottom, 255);
                }
                else { //绘制Option控件
                    if (p->m_dwOptionStyle > 4)
                        zcolor = button->m_ColorPushed;
                    else
                        zcolor = p->m_ColorSelectedPushed;
                    ZuiOptionDrawResStyle(cp, p, zcolor);
                }
            }
            else {
                if (p->m_ResSelectedDisabled) {
                    img = p->m_ResSelectedDisabled->p;
                    ZuiDrawImageEx(cp, img, rc->left, rc->top, rc->right, rc->bottom, 255);
                }
                else {  //绘制Option控件
                    if (p->m_dwOptionStyle > 4)
                        zcolor = button->m_ColorDisabled;
                    else
                        zcolor = p->m_ColorSelectedDisabled;
                    ZuiOptionDrawResStyle(cp, p, zcolor);
                }
            }
        }
        else {
            if (button->m_dwType == 0) {
                if (button->m_ResNormal) {
                    img = button->m_ResNormal->p;
                    ZuiDrawImageEx(cp, img, rc->left, rc->top, rc->right, rc->bottom, 255);
                }
                else {  //绘制Option控件
                    ZuiOptionDrawResStyle(cp, p, button->m_ColorNormal);
                }
            }
            else if (button->m_dwType == 1) {
                if (button->m_ResHot) {
                    img = button->m_ResHot->p;
                    ZuiDrawImageEx(cp, img, rc->left, rc->top, rc->right, rc->bottom, 255);
                }
                else {  //绘制Option控件
                    ZuiOptionDrawResStyle(cp, p, button->m_ColorHot);
                }
            }
            else if (button->m_dwType == 2) {
                if (button->m_ResPushed) {
                    img = button->m_ResPushed->p;
                    ZuiDrawImageEx(cp, img, rc->left, rc->top, rc->right, rc->bottom, 255);
                }
                else {  //绘制Option控件
                    ZuiOptionDrawResStyle(cp, p, button->m_ColorPushed);
                }
            }
            else {
                if (button->m_ResDisabled) {
                    img = button->m_ResDisabled->p;
                    ZuiDrawImageEx(cp, img, rc->left, rc->top, rc->right, rc->bottom, 255);
                }
                else {  //绘制Option控件
                    ZuiOptionDrawResStyle(cp, p, button->m_ColorDisabled);
                }
            }
        }
        return 0;
    }
    case ZM_Option_SetSelected: {
        if (p->m_bSelected == (ZuiBool)Param1) return 0;
        p->m_bSelected = (ZuiBool)Param1;
        if (!p->m_bCheck) { //不是Option控件，不需要取消其他Option状态。
            if (Param1) {
                for (size_t i = 0; i < (size_t)ZCCALL(ZM_Layout_GetCount, cp->m_pParent, NULL, NULL); i++)
                {
                    ZuiControl pControl = ZCCALL(ZM_Layout_GetItemAt, cp->m_pParent, (ZuiAny)i, NULL);
                    ZuiBool isoption = _tcsicmp(ZCCALL(ZM_GetType, pControl, NULL, NULL),ZC_Option);
                    if (pControl != cp && !isoption)
                    {
                        ZCCALL(ZM_Option_SetSelected, pControl, FALSE, Param2);
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
                        ZuiBool isoption = _tcsicmp(ZCCALL(ZM_GetType, pControl, NULL, NULL), ZC_Option);
                        if(!isoption)
                            select += ZCCALL(ZM_Option_GetSelected, pControl, NULL, NULL) ? 1 : 0;
                    }
                    if (!select) {
                        p->m_bSelected = !p->m_bSelected;
                        return 0;
                    }
                }
            }
        }
        if (!Param2)
            ZuiControlInvalidate(cp, TRUE);
        ZuiControlNotify(ZM_OnSelectChanged, cp, (ZuiAny)p->m_bSelected, NULL);
        return 0;
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
        if (!Param2)
            ZuiControlInvalidate(cp, TRUE);
        return 0;
    }
    case ZM_Option_SetResHot: {
        if (p->m_ResSelectedHot)
            ZuiResDBDelRes(p->m_ResSelectedHot);
        p->m_ResSelectedHot = Param1;
        if (!Param2)
            ZuiControlInvalidate(cp, TRUE);
        return 0;
    }
    case ZM_Option_SetResPushed: {
        if (p->m_ResSelectedPushed)
            ZuiResDBDelRes(p->m_ResSelectedPushed);
        p->m_ResSelectedPushed = Param1;
        if (!Param2)
            ZuiControlInvalidate(cp, TRUE);
        return 0;
    }
    case ZM_Option_SetResFocused: {
        if (p->m_ResSelectedFocused)
            ZuiResDBDelRes(p->m_ResSelectedFocused);
        p->m_ResSelectedFocused = Param1;
        if (!Param2)
            ZuiControlInvalidate(cp, TRUE);
        return 0;
    }
    case ZM_Option_SetResDisabled: {
        if (p->m_ResSelectedDisabled)
            ZuiResDBDelRes(p->m_ResSelectedDisabled);
        p->m_ResSelectedDisabled = Param1;
        if (!Param2)
            ZuiControlInvalidate(cp, TRUE);
        return 0;
    }
    case ZM_Option_SetColorNormal: {
        p->m_ColorSelected = (ZuiColor)Param1;
        if (!Param2)
            ZuiControlInvalidate(cp, TRUE);
        return 0;
    }
    case ZM_Option_SetColorHot: {
        p->m_ColorSelectedHot = (ZuiColor)Param1;
        if (!Param2)
            ZuiControlInvalidate(cp, TRUE);
        return 0;
    }
    case ZM_Option_SetColorPushed: {
        p->m_ColorSelectedPushed = (ZuiColor)Param1;
        if (!Param2)
            ZuiControlInvalidate(cp, TRUE);
        return 0;
    }
    case ZM_Option_SetColorFocused: {
        p->m_ColorSelectedFocused = (ZuiColor)Param1;
        if (!Param2)
            ZuiControlInvalidate(cp, TRUE);
        return 0;
    }
    case ZM_Option_SetColorDisabled: {
        p->m_ColorSelectedDisabled = (ZuiColor)Param1;
        if (!Param2)
            ZuiControlInvalidate(cp, TRUE);
        return 0;
    }
    case ZM_Option_SetResType: {
        p->m_dwOptionStyle = (int)Param1;
        if (!Param2)
            ZuiControlInvalidate(cp, TRUE);
        return 0;
    }
    case ZM_SetAttribute: {
        ZuiAttribute zAttr = (ZuiAttribute)Param1;
        if (_tcscmp(zAttr->name, _T("group")) == 0)
            ZCCALL(ZM_Option_SetGroup, cp, (ZuiAny)(_tcsicmp(zAttr->value, _T("true")) == 0 ? TRUE : FALSE), Param2);
        else if (_tcscmp(zAttr->name, _T("selected")) == 0)
            ZCCALL(ZM_Option_SetSelected, cp, (ZuiAny)(_tcsicmp(zAttr->value, _T("true")) == 0 ? TRUE : FALSE), Param2);
        else if (_tcscmp(zAttr->name, _T("selectedimage")) == 0)
            ZCCALL(ZM_Option_SetResNormal, cp, (ZuiAny)ZuiResDBGetRes(zAttr->value, ZREST_IMG), Param2);
        else if (_tcscmp(zAttr->name, _T("selectedhotimage")) == 0)
            ZCCALL(ZM_Option_SetResHot, cp, (ZuiAny)ZuiResDBGetRes(zAttr->value, ZREST_IMG), Param2);
        else if (_tcscmp(zAttr->name, _T("selectedpushedimage")) == 0)
            ZCCALL(ZM_Option_SetResPushed, cp, (ZuiAny)ZuiResDBGetRes(zAttr->value, ZREST_IMG), Param2);
        else if (_tcscmp(zAttr->name, _T("selectedfocusedimage")) == 0)
            ZCCALL(ZM_Option_SetResFocused, cp, (ZuiAny)ZuiResDBGetRes(zAttr->value, ZREST_IMG), Param2);
        else if (_tcscmp(zAttr->name, _T("selecteddisabledimage")) == 0)
            ZCCALL(ZM_Option_SetResDisabled, cp, (ZuiAny)ZuiResDBGetRes(zAttr->value, ZREST_IMG), Param2);
        else if (_tcsicmp(zAttr->name, _T("selectedcolor")) == 0)
            ZCCALL(ZM_Option_SetColorNormal, cp, (ZuiAny)ZuiStr2Color(zAttr->value), Param2);
        else if (_tcsicmp(zAttr->name, _T("hotselectedcolor")) == 0)
            ZCCALL(ZM_Option_SetColorHot, cp, (ZuiAny)ZuiStr2Color(zAttr->value), Param2);
        else if (_tcsicmp(zAttr->name, _T("pushedselectedcolor")) == 0)
            ZCCALL(ZM_Option_SetColorPushed, cp, (ZuiAny)ZuiStr2Color(zAttr->value), Param2);
        else if (_tcsicmp(zAttr->name, _T("focusedselectedcolor")) == 0)
            ZCCALL(ZM_Option_SetColorFocused, cp, (ZuiAny)ZuiStr2Color(zAttr->value), Param2);
        else if (_tcsicmp(zAttr->name, _T("disabledselectedcolor")) == 0)
            ZCCALL(ZM_Option_SetColorDisabled, cp, (ZuiAny)ZuiStr2Color(zAttr->value), Param2);
        else if (_tcsicmp(zAttr->name, _T("optionstyle")) == 0)
            ZCCALL(ZM_Option_SetResType, cp, (ZuiAny)(_ttoi(zAttr->value)), Param2);
        break;
    }
    case ZM_OnCreate: {
        p = (ZuiOption)malloc(sizeof(ZOption));
        memset(p, 0, sizeof(ZOption));
        //保存原来的回调地址,创建成功后回调地址指向当前函数
        //创建继承的控件 保存数据指针
        p->old_udata = ZuiButtonProc(ZM_OnCreate, cp, 0, 0, 0);
        p->old_call = (ZCtlProc)&ZuiButtonProc;
        p->m_ColorSelected = 0xFF05AA05;
        p->m_ColorSelectedHot = 0xFF55AA55;		//选中的点燃状态
        p->m_ColorSelectedPushed = 0xFF75AA75;	//选中的按下状态
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
        if (_tcsicmp(Param1, (ZuiAny)ZC_Option) == 0)
            return (ZuiAny)p;
        break;
    case ZM_GetType:
        return (ZuiAny)ZC_Option;
    case ZM_CoreInit:
        return (ZuiAny)TRUE;
    case ZM_CoreUnInit:
        return (ZuiAny)TRUE;
    default:
        break;
    }
    return p->old_call(ProcId, cp, p->old_udata, Param1, Param2);
}




