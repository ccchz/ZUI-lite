#include <stdio.h>
#include <windows.h>
#include <commctrl.h>
#include <tchar.h>
#include <ZUI.h>

ZuiAny ZCALL Main_Notify_ctl_clos(ZuiText msg, ZuiControl p, ZuiAny UserData, ZuiAny Param1, ZuiAny Param2);
ZuiAny ZCALL Main_Notify_ctl_max(ZuiText msg, ZuiControl p, ZuiAny UserData, ZuiAny Param1, ZuiAny Param2);
ZuiAny ZCALL Main_Notify(ZuiText msg, ZuiControl p, ZuiAny UserData, ZuiAny Param1, ZuiAny Param2);
ZuiAny ZCALL Main_Notify_ctl_min(ZuiText msg, ZuiControl p, ZuiAny UserData, ZuiAny Param1, ZuiAny Param2);
ZuiAny ZCALL msgbox_Notify(ZuiText msg, ZuiControl p, ZuiAny UserData, ZuiAny Param1, ZuiAny Param2);
ZuiAny ZCALL Main_Button_enable(ZuiText msg, ZuiControl p, ZuiAny UserData, ZuiAny Param1, ZuiAny Param2);

ZuiControl win;

int _stdcall WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
    ZInitConfig config = { 0 };
    config.m_hInstance = hInstance;
    config.default_res = _T("file:default.zip");
    config.default_fontname = _T("微软雅黑");
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

    p = ZuiControlFindName(win, _T("msgbox"));
    ZuiControlRegNotify(p, msgbox_Notify);

    p = ZuiControlFindName(win, _T("buttonenable"));
    ZuiControlRegNotify(p, Main_Button_enable);

    ZuiMsgLoop();
    printf("Loop done!!\n");
    Sleep(2000);
    ZuiUnInit();
    return 0;
}
int main(int argc, char* argv[]) {
    return WinMain(GetModuleHandle(0), NULL, NULL, 0);
}

ZuiAny ZCALL Main_Button_enable(ZuiText msg, ZuiControl p, ZuiAny UserData, ZuiAny Param1, ZuiAny Param2) {
    if (_tcsicmp(msg, _T("onclick")) == 0)
    {
        ZuiControl tmp = ZuiControlFindName(win, _T("buttondisable"));
        if (!ZuiControlCall(Proc_GetEnabled, tmp, 0, 0)) {
            ZuiControlCall(Proc_SetEnabled, tmp, (ZuiAny)1, 0);
            ZuiControlCall(Proc_SetText, p, _T("禁用"),  0);
        }
        else {
            ZuiControlCall(Proc_SetEnabled, tmp, 0, 0);
            ZuiControlCall(Proc_SetText, p, _T("启用"), 0);
        }
    }
    return 0;
}
ZuiAny ZCALL Main_Notify_ctl_clos(ZuiText msg, ZuiControl p, ZuiAny UserData, ZuiAny Param1, ZuiAny Param2) {
    if (_tcsicmp(msg, _T("onclick")) == 0)
    {
        ZuiControlCall(Proc_OnClose, win, 0, 0);
    }
    return 0;
}

ZuiAny ZCALL Main_Notify_ctl_min(ZuiText msg, ZuiControl p, ZuiAny UserData, ZuiAny Param1, ZuiAny Param2) {
    if (_tcsicmp(msg, _T("onclick")) == 0)
    {
        ZuiControlCall(Proc_Window_SetWindowMin, win, 0, 0);
    }
    return 0;
}

ZuiAny ZCALL Main_Notify_ctl_max(ZuiText msg, ZuiControl p, ZuiAny UserData, ZuiAny Param1, ZuiAny Param2) {
    if (_tcsicmp(msg, _T("selectchanged")) == 0)
    {

        if (ZuiControlCall(Proc_Option_GetSelected, p, 0, 0))
            ZuiControlCall(Proc_Window_SetWindowMax, win, 0, 0);
        else
            ZuiControlCall(Proc_Window_SetWindowRestor, win, 0, 0);

    }
    return 0;
}
ZuiAny ZCALL Main_Notify(ZuiText msg, ZuiControl p, ZuiAny UserData, ZuiAny Param1, ZuiAny Param2) {
    if (_tcsicmp(msg, _T("onclose")) == 0)
    {
        int ret = ZuiMsgBox(win, _T("是否退出程序？"), _T("提示!!"));
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
                ZuiControlCall(Proc_Option_SetSelected, pmax, (ZuiAny)TRUE, NULL);
        }
        else if(Param1 == 0)
        {
            ZuiControl pmax = ZuiControlFindName(p, _T("WindowCtl_max"));
            if (pmax)
                ZuiControlCall(Proc_Option_SetSelected, pmax, (ZuiAny)FALSE, NULL);
        }
    }
    return 0;
}
ZuiAny ZCALL msgbox_Notify(ZuiText msg, ZuiControl p, ZuiAny UserData, ZuiAny Param1, ZuiAny Param2) {
    if (_tcsicmp(msg, _T("onclick")) == 0)
    {
        ZuiMsgBox(win, _T("HELLO WORLD!!"), _T("Hello World!!"));
    }
    return 0;
}
