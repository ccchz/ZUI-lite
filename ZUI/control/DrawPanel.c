#include "DrawPanel.h"
#include <core/control.h>
ZEXPORT ZuiAny ZCALL ZuiDrawPanelProc(int ProcId, ZuiControl cp, ZuiDrawPanel p, ZuiAny Param1, ZuiAny Param2) {
    switch (ProcId)
    {
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
        if (_tcsicmp(Param1, (ZuiAny)Type_DrawPanel) == 0)
            return (ZuiAny)p;
        break;
    case ZM_GetType:
        return (ZuiAny)Type_DrawPanel;
    case ZM_CoreInit:
        return (ZuiAny)TRUE;
    case ZM_CoreUnInit:
        return (ZuiAny)NULL;
    default:
        break;
    }
    return p->old_call(ProcId, cp, 0, Param1, Param2);
}




