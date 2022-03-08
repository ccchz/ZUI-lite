#include "DrawPanel.h"
#include <core/control.h>
ZEXPORT ZINT ZCALL ZuiDrawPanelProc(ZINT ProcId, ZuiControl cp, ZuiDrawPanel p, ZPARAM Param1, ZPARAM Param2) {
    switch (ProcId)
    {
    case ZM_OnPaint: {
        p->old_call(ProcId, cp, 0, Param1, Param2);
        return ZuiControlNotify(ZM_OnPaint, cp, Param1, Param2);
    }
    case ZM_OnCreate: {
        p = (ZuiDrawPanel)malloc(sizeof(ZDrawPanel));
        memset(p, 0, sizeof(ZuiDrawPanel));
        //保存原来的回调地址,创建成功后回调地址指向当前函数
        p->old_call = cp->call;

        return p;
    }
    case ZM_OnDestroy: {
        p->old_call(ProcId, cp, 0, Param1, Param2);
        free(p);

        return 0;
    }
    case ZM_GetObject:
        if (_tcsicmp(Param1, (ZPARAM)ZC_DrawPanel) == 0)
            return (ZPARAM)p;
        break;
    case ZM_GetType:
        return (ZPARAM)ZC_DrawPanel;
    case ZM_CoreInit:
        return (ZPARAM)TRUE;
    case ZM_CoreUnInit:
        return (ZPARAM)NULL;
    default:
        break;
    }
    return p->old_call(ProcId, cp, 0, Param1, Param2);
}




