#include "CheckBox.h"
#include <core/control.h>
#include "Option.h"

ZEXPORT ZINT ZCALL ZuiCheckBoxProc(ZINT ProcId, ZuiControl cp, ZuiCheckBox p, ZPARAM Param1, ZPARAM Param2) {
    switch (ProcId)
    {
    case ZM_OnCreate: {
        ZuiOption zop =  ZuiOptionProc(ZM_OnCreate, cp, (ZuiOption)p, Param1, Param2);
        zop->m_bCheck = TRUE;
        return zop;
    }
    case ZM_GetObject:
        if (_tcsicmp(Param1, (ZPARAM)ZC_CheckBox) == 0)
            return (ZPARAM)p;
        break;
    case ZM_GetType:
        return (ZPARAM)ZC_CheckBox;
    case ZM_CoreInit:
        return (ZPARAM)TRUE;
    case ZM_CoreUnInit:
        return (ZPARAM)NULL;
    default:
        break;
    }
    return ZuiOptionProc(ProcId, cp, (ZuiOption)p, Param1, Param2);
}




