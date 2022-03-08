#include <stdio.h>
#include <windows.h>
#include <commctrl.h>
#include <tchar.h>
#include "resource.h"
#include <ZUI.h>
#include <core/function.h>
#include <platform/platform.h>
#include <core/resdb.h>

#define Array_len 6
ZuiControl win;
ZRound oldrd;
ZuiText cname[Array_len] = { 
                     _T("option0"),
                     _T("option1"), 
                     _T("option2"), 
                     _T("option3"), 
                     _T("option4"), 
                     _T("option5"), };

ZINT ZCALL Main_Button_enable(int msg, ZuiControl p, ZPARAM Param1, ZPARAM Param2) {
    if (msg == ZM_OnClick)
    {
        ZuiControl tmp = ZuiControlFindName(win, _T("buttondisable"));
        if (!ZCCALL(ZM_GetEnabled, tmp, 0, 0)) {
            ZCCALL(ZM_SetEnabled, tmp, (ZPARAM)1, 0);
            ZCCALL(ZM_SetText, p, _T("禁用"),  0);
        }
        else {
            ZCCALL(ZM_SetEnabled, tmp, 0, 0);
            ZCCALL(ZM_SetText, p, _T("启用"), 0);
        }
    }
    return 0;
}

ZINT ZCALL msgbox_Notify(int msg, ZuiControl p, ZPARAM Param1, ZPARAM Param2) {
    if (msg == ZM_OnClick)
    {
        ZuiMsgBox(win, _T("HELLO WORLD!!"), _T("Hello World!!"));
        //ZuiControl cp = ZuiControlFindName(win, _T("tab"));
        //ZCCALL(ZM_TabLayout_SetSelectIndex, cp, (ZPARAM)2, NULL);
    }
    return 0;
}

ZINT ZCALL dialog_proc(int msg, ZuiControl p, ZPARAM Param1, ZPARAM Param2) {
    if (msg == ZM_OnClose) {
        FreeZuiControl(p, 1);
    }
    else if (msg == ZM_OnCreate) {
        ZText zstr[1024] = { 0 };
        _sntprintf_s(zstr, 1023, 1023, _T("%-29s %d.%d\r\n%-25s%s %s\r\n%-25s VisualStudio-%d"),
            _T("版本:"), (ZuiVersion >> 16), ZuiVersion & 0xFFFF,
            _T("编译日期:"), _T(__DATE__), _T(__TIME__),
            _T("编译工具:"), _MSC_VER);
        ZuiControl rp = ZuiControlFindName(p, _T("text"));
        ZCCALL(ZM_SetText, rp, zstr, 0);
    }
    return 0;
}

ZINT ZCALL msgbox_Notify1(int msg, ZuiControl p, ZPARAM Param1, ZPARAM Param2) {
    if (msg == ZM_OnClick)
    {
        //ZuiMsgBox(win, _T("Container点击响应。"), _T("提示..."));
        ZuiDialogBox(win, _T("default://about.xml"), dialog_proc, 1);
    }
    return 0;
}
ZINT ZCALL Option_Notify(int msg, ZuiControl p, ZPARAM Param1, ZPARAM Param2) {
    if (msg == ZM_OnSelectChanged && Param1)
    {
        ZuiText pname = (ZuiText)ZCCALL(ZM_GetName, p, 0, 0);
        //ZuiMsgBox(win, pname, _T("提示..."));
        while (pname && *pname && !isdigit(*pname))
            pname++;
        ZuiControl cp = ZuiControlFindName(win, _T("tab"));
        ZCCALL(ZM_TabLayout_SetSelectIndex, cp, (ZPARAM)_ttoi(pname), NULL);
    }
    return 0;
}

ZINT ZCALL Drawpanel_Notify(int msg, ZuiControl p, ZPARAM Param1, ZPARAM Param2) {
    if (msg == ZM_OnPaint)
    {
        ZuiText str = _T("DrawPanel控件.");
        ZuiRect rc = (ZuiRect)ZCCALL(ZM_GetPos, p, 0, 0);
        ZPointR ptr;
        ZuiDrawEllipse(p, 0xFFff0000, rc, 1);
        ptr.x = (ZuiReal)rc->left;
        ptr.y = (ZuiReal)rc->top + (rc->bottom - rc->top) / 2;
        ZuiDrawStringPt(p, (ZuiFont)Global_Font->p, 0xFF158815, str, _tcsclen(str), &ptr);
    }
    return 0;
}
ZINT ZCALL Main_Notify_ctl_clos(int msg, ZuiControl p, ZPARAM Param1, ZPARAM Param2) {
    if (msg == ZM_OnClick)
    {
        ZCCALL(ZM_OnClose, win, 0, 0);
    }
    return 0;
}

ZINT ZCALL Main_Notify_ctl_min(int msg, ZuiControl p, ZPARAM Param1, ZPARAM Param2) {
    if (msg == ZM_OnClick)
    {
        ZCCALL(ZM_Window_SetWindowMin, win, 0, 0);
    }
    return 0;
}

ZINT ZCALL Main_Notify_ctl_max(int msg, ZuiControl p, ZPARAM Param1, ZPARAM Param2) {
    if (msg == ZM_OnSelectChanged)
    {
        ZuiControl pmax;
        ZRound rd;
        if (Param1) {
            if (!ZuiOsIsZoomed(p))
                ZCCALL(ZM_Window_SetWindowMax, win, 0, 0);
            pmax = ZCCALL(ZM_GetParent, p, 0, 0);   //更改关闭按钮父级控件圆角属性。
            memcpy(&oldrd, ZCCALL(ZM_GetRound, pmax, 0, 0), sizeof(ZRound));
            rd.left = rd.right = rd.top = rd.bottom = 0;
            ZCCALL(ZM_SetRound, pmax, &rd, (ZPARAM)1);
        }
        else {
            if (ZuiOsIsZoomed(p))
                ZCCALL(ZM_Window_SetWindowRestor, win, 0, 0);
            pmax = ZCCALL(ZM_GetParent, p, 0, 0); //还原关闭按钮父级控件圆角属性。
            ZCCALL(ZM_SetRound, pmax, &oldrd, (ZPARAM)1);
        }
    }
    return 0;
}
ZINT ZCALL Main_Notify(int msg, ZuiControl cp, ZPARAM Param1, ZPARAM Param2) {
    if (msg == ZM_OnClose)
    {
        int ret = ZuiMsgBox(win, _T("是否退出程序?"), _T("提示!!"));
        if (ret == ZuiOK) {
            
            FreeZuiControl(win, 1);
        }
    }
    else if (msg == ZM_OnCreate) {
        ZuiControl p = ZuiControlFindName(cp, _T("WindowCtl_clos"));
        ZuiControlRegNotify(p, Main_Notify_ctl_clos);

        p = ZuiControlFindName(cp, _T("WindowCtl_max"));
        ZuiControlRegNotify(p, Main_Notify_ctl_max);

        p = ZuiControlFindName(cp, _T("WindowCtl_min"));
        ZuiControlRegNotify(p, Main_Notify_ctl_min);

        for (int i = 0; i < Array_len; i++) {
            p = ZuiControlFindName(cp, cname[i]);
            ZuiControlRegNotify(p, Option_Notify);
        }

        p = ZuiControlFindName(cp, _T("container1"));
        ZuiControlRegNotify(p, msgbox_Notify1);
        p = ZuiControlFindName(cp, _T("drawpanel1"));
        ZuiControlRegNotify(p, Drawpanel_Notify);
    }
    else if (msg == ZM_OnDestroy)
    {
        ZuiMsgLoop_exit(0);
    }
    else if (msg == ZM_OnSize)
    {
       
        if ((int)Param1 == 2) { //窗口最大化
            ZuiControl pmax = ZuiControlFindName(win, _T("WindowCtl_max"));
            if (pmax) {
                ZCCALL(ZM_Option_SetSelected, pmax, (ZPARAM)TRUE, (ZPARAM)TRUE);
            }
        }
        else if(Param1 == 0)  //窗口还原
        {
            ZuiControl pmax = ZuiControlFindName(cp, _T("WindowCtl_max"));
            if (pmax) {
                ZCCALL(ZM_Option_SetSelected, pmax, (ZPARAM)FALSE, (ZPARAM)TRUE);
            }
        }
    }
    return 0;
}

int _stdcall WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
    ZInitConfig config = { 0 };
    config.m_hInstance = hInstance;
    config.hicon = LoadIcon(config.m_hInstance, MAKEINTRESOURCE(IDI_ICON1));
    config.default_res = _T("file:///default.zip");
    config.default_font = _T("font://fontfamily='微软雅黑':size=10");
    if (!ZuiInit(&config))
        return 0;

    ZuiLoadXml(_T("default://default_msgbox.xml"));

    win = NewZuiControlFromXml(NULL, _T("file:///test.xml"), Main_Notify);
    if (!win)
        return 0;


    ZuiMsgLoop();
    D_PRINT(_T("Loop done!!\n"));
    Sleep(2000);
    ZuiUnInit();
    return 0;
}
int main(int argc, char* argv[]) {
    return WinMain(GetModuleHandle(0), NULL, NULL, 0);
}