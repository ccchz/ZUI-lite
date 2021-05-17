#include "CheckBox.h"
#include <core/control.h>
#include "Option.h"

ZEXPORT ZuiAny ZCALL ZuiCheckBoxProc(int ProcId, ZuiControl cp, ZuiCheckBox p, ZuiAny Param1, ZuiAny Param2) {
    switch (ProcId)
    {
    case ZM_OnCreate: {
        ZuiOption zop =  ZuiOptionProc(ZM_OnCreate, cp, (ZuiOption)p, Param1, Param2);
        zop->m_bCheck = TRUE;
        return zop;
    }
    case ZM_GetObject:
        if (_tcsicmp(Param1, (ZuiAny)Type_CheckBox) == 0)
            return (ZuiAny)p;
        break;
    case ZM_GetType:
        return (ZuiAny)Type_CheckBox;
    case ZM_CoreInit:
        return (ZuiAny)TRUE;
    case ZM_CoreUnInit:
        return (ZuiAny)NULL;
    default:
        break;
    }
    return ZuiOptionProc(ProcId, cp, (ZuiOption)p, Param1, Param2);
}




