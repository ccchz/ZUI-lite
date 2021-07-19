#include <stdio.h>
#include <windows.h>
#include <commctrl.h>
#include <tchar.h>
#include "resource.h"
#include <ZUI.h>

ZuiControl win;
ZuiText cname[5] = { _T("option0"),
                     _T("option1"), 
                     _T("option2"), 
                     _T("option3"), 
                     _T("option4"), };

ZuiAny ZCALL Main_Button_enable(ZuiText msg, ZuiControl p, ZuiAny UserData, ZuiAny Param1, ZuiAny Param2) {
    if (_tcsicmp(msg, _T("onclick")) == 0)
    {
        ZuiControl tmp = ZuiControlFindName(win, _T("buttondisable"));
        if (!ZCCALL(ZM_GetEnabled, tmp, 0, 0)) {
            ZCCALL(ZM_SetEnabled, tmp, (ZuiAny)1, 0);
            ZCCALL(ZM_SetText, p, _T("禁用"),  0);
        }
        else {
            ZCCALL(ZM_SetEnabled, tmp, 0, 0);
            ZCCALL(ZM_SetText, p, _T("启用"), 0);
        }
    }
    return 0;
}
ZuiAny ZCALL Main_Notify_ctl_clos(ZuiText msg, ZuiControl p, ZuiAny UserData, ZuiAny Param1, ZuiAny Param2) {
    if (_tcsicmp(msg, _T("onclick")) == 0)
    {
        ZCCALL(ZM_OnClose, win, 0, 0);
    }
    return 0;
}

ZuiAny ZCALL Main_Notify_ctl_min(ZuiText msg, ZuiControl p, ZuiAny UserData, ZuiAny Param1, ZuiAny Param2) {
    if (_tcsicmp(msg, _T("onclick")) == 0)
    {
        ZCCALL(ZM_Window_SetWindowMin, win, 0, 0);
    }
    return 0;
}

ZuiAny ZCALL Main_Notify_ctl_max(ZuiText msg, ZuiControl p, ZuiAny UserData, ZuiAny Param1, ZuiAny Param2) {
    if (_tcsicmp(msg, _T("selectchanged")) == 0)
    {

        if (ZCCALL(ZM_Option_GetSelected, p, 0, 0))
            ZCCALL(ZM_Window_SetWindowMax, win, 0, 0);
        else
            ZCCALL(ZM_Window_SetWindowRestor, win, 0, 0);

    }
    return 0;
}
ZuiAny ZCALL Main_Notify(ZuiText msg, ZuiControl p, ZuiAny UserData, ZuiAny Param1, ZuiAny Param2) {
    if (_tcsicmp(msg, _T("onclose")) == 0)
    {
        int ret = ZuiMsgBox(win, _T("是否退出程序?"), _T("提示!!"));
        if (ret == ZuiOK) {
            FreeZuiControl(win, 1);
        }
    }
    else if (_tcsicmp(msg, _T("ondestroy")) == 0)
    {
        ZuiMsgLoop_exit(0);
    }
    else if (_tcsicmp(msg, _T("onsize")) == 0)
    {
        if ((LPARAM)Param1 == 2) {
            ZuiControl pmax = ZuiControlFindName(win, _T("WindowCtl_max"));
            if (pmax)
                ZCCALL(ZM_Option_SetSelected, pmax, (ZuiAny)TRUE, NULL);
        }
        else if(Param1 == 0)
        {
            ZuiControl pmax = ZuiControlFindName(p, _T("WindowCtl_max"));
            if (pmax)
                ZCCALL(ZM_Option_SetSelected, pmax, (ZuiAny)FALSE, NULL);
        }
    }
    return 0;
}
ZuiAny ZCALL msgbox_Notify(ZuiText msg, ZuiControl p, ZuiAny UserData, ZuiAny Param1, ZuiAny Param2) {
    if (_tcsicmp(msg, _T("onclick")) == 0)
    {
        //ZuiMsgBox(win, _T("HELLO WORLD!!"), _T("Hello World!!"));
        ZuiControl cp = ZuiControlFindName(win, _T("tab"));
        ZCCALL(ZM_TabLayout_SelectItem, cp, (ZuiAny)2, NULL);
    }
    return 0;
}

ZuiAny ZCALL msgbox_Notify1(ZuiText msg, ZuiControl p, ZuiAny UserData, ZuiAny Param1, ZuiAny Param2) {
    if (_tcsicmp(msg, _T("onclick")) == 0)
    {
        ZuiMsgBox(win, _T("Container点击响应。"), _T("提示..."));
    }
    return 0;
}

ZuiAny ZCALL Option_Notify(ZuiText msg, ZuiControl p, ZuiAny UserData, ZuiAny Param1, ZuiAny Param2) {
    if (_tcsicmp(msg, _T("selectchanged")) == 0 && Param1)
    {
        ZuiText pname = ZCCALL(ZM_GetName, p, 0, 0);
        //ZuiMsgBox(win, pname, _T("提示..."));
        while (pname && *pname && !isdigit(*pname))
            pname++;
        ZuiControl cp = ZuiControlFindName(win, _T("tab"));
        ZCCALL(ZM_TabLayout_SelectItem, cp, (ZuiAny)_ttoi(pname), NULL);
    }
    return 0;
}

int _stdcall WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
    ZInitConfig config = { 0 };
    config.m_hInstance = hInstance;
    config.hicon = LoadIcon(config.m_hInstance, MAKEINTRESOURCE(IDI_ICON1));
    config.default_res = _T("file:default.zip");
    config.default_font = _T("font:name='微软雅黑':size=10");
    if (!ZuiInit(&config)) return 0;

    ZuiRes	res = ZuiResDBGetRes(_T("default:default_msgbox.xml"), ZREST_STREAM);
    int	len = 0;
    ZuiAny	xml = ZuiResGetData(res, &len);
    ZuiLayoutLoad(xml, len);


    res = ZuiResDBGetRes(_T("file:test.xml"), ZREST_STREAM);
    len = 0;
    xml = ZuiResGetData(res, &len);
    win = ZuiLayoutLoad(xml, len);
    if (!win) return 0;
    ZuiControlRegNotify(win, Main_Notify);

    ZuiControl p = ZuiControlFindName(win, _T("WindowCtl_clos"));
    ZuiControlRegNotify(p, Main_Notify_ctl_clos);

    p = ZuiControlFindName(win, _T("WindowCtl_max"));
    ZuiControlRegNotify(p, Main_Notify_ctl_max);

    p = ZuiControlFindName(win, _T("WindowCtl_min"));
    ZuiControlRegNotify(p, Main_Notify_ctl_min);

    for (int i = 0; i < 5; i++) {
        p = ZuiControlFindName(win, cname[i]);
        ZuiControlRegNotify(p, Option_Notify);
    }

    p = ZuiControlFindName(win, _T("container1"));
    ZuiControlRegNotify(p, msgbox_Notify1);


    ZuiMsgLoop();
    printf("Loop done!!\n");
    Sleep(2000);
    ZuiUnInit();
    return 0;
}
int main(int argc, char* argv[]) {
    return WinMain(GetModuleHandle(0), NULL, NULL, 0);
}