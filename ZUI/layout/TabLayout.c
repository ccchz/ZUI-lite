#include "TabLayout.h"
#include "Layout.h"
#include <core/control.h>
#include <core/builder.h>
#include <stdlib.h>

void* ZCALL ZuiTabLayoutProc(int ProcId, ZuiControl cp, ZuiTabLayout p, void* Param1, void* Param2) {
    switch (ProcId)
    {
    case ZM_FindControl: {
        ZuiControl op;
        if ((int)Param2 & ZFIND_FROM_POINT) {
            op = ZCCALL(ZM_Layout_GetItemAt, cp, (ZuiAny)p->m_iCurSel, Param2);
            op = ZCCALL(ProcId, op, Param1, Param2);
            if (!op) 
                op = ZuiDefaultControlProc(ProcId, cp, p, Param1, Param2);;
            return op; 
        }
        break;
    }
    case ZM_TabLayout_GetSelectIndex: {
        return (ZuiAny)p->m_iCurSel;
    }
    case ZM_SetAttribute: {
        ZuiAttribute zAttr = (ZuiAttribute)Param1;
        if (_tcsicmp(zAttr->name, _T("tabselect")) == 0)
            ZCCALL(ZM_TabLayout_SetSelectIndex, cp, (ZuiAny)(_ttoi(zAttr->value)), Param2);
        break;
    }
    case ZM_OnPaint: {
        if (p->m_iCurSel < 0)
            return FALSE;
        ZuiDefaultControlProc(ProcId, cp, 0, Param1, Param2);
        ZuiControl op = ZCCALL(ZM_Layout_GetItemAt, cp, (ZuiAny)p->m_iCurSel, Param2);
        ZCCALL(ZM_OnPaint, op, Param1, Param2);
        return (ZuiAny)TRUE;
        }
    case ZM_Layout_Add: {
        ZuiBool ret = (ZuiBool)p->old_call(ProcId, cp, p->old_udata, Param1, Param2);
        if (!ret)
            return (ZuiAny)ret;

        if (p->m_iCurSel == -1)
        {
            p->m_iCurSel = 0;
        }
        return (ZuiAny)ret;
    }
    case ZM_Layout_AddAt: {
        ZuiBool ret = (ZuiBool)p->old_call(ProcId, cp, p->old_udata, Param1, Param2);
        if (!ret)
            return (ZuiAny)ret;

        if (p->m_iCurSel != -1 && p->m_iCurSel >= (int)Param2)
        {
            p->m_iCurSel += 1;
        }
        return (ZuiAny)ret;
    }
    case ZM_Layout_Remove: {
        if (Param1 == NULL)
            return FALSE;

        int index = (int)ZCCALL(ZM_Layout_GetItemIndex, cp, Param1, Param2);
        if (index == -1)
            return FALSE;
        p->old_call(ProcId, cp, p->old_udata, Param1, Param2);
        FreeZuiControl((ZuiControl)Param1, FALSE);
        if (p->m_iCurSel >= index)
        {
            p->m_iCurSel -= 1;
        }
        return (ZuiAny)1;
    }
    case ZM_Layout_RemoveAll: {
        p->m_iCurSel = -1;
        ZuiLayoutProc(ZM_Layout_RemoveAll, cp, p->old_udata, Param1, Param2);
        return 0;
    }
    case ZM_TabLayout_SetSelectIndex: {
        int iIndex = (int)Param1;
        ZuiLayout op = (ZuiLayout)p->old_udata;
        if (iIndex == p->m_iCurSel)
            return (ZuiAny)TRUE;
        if (iIndex < 0 || iIndex >= darray_len(op->m_items))
            return FALSE;
        p->m_iCurSel = iIndex;
        if (!Param2)
            ZuiControlInvalidate(cp, TRUE);

        return (ZuiAny)TRUE;
    }
    case ZM_OnCreate: {
        p = (ZuiTabLayout)malloc(sizeof(ZTabLayout));
        memset(p, 0, sizeof(ZTabLayout));
        //创建继承的控件 保存数据指针
        p->old_udata = ZuiLayoutProc(ZM_OnCreate, cp, 0, 0, 0);
        p->old_call = (ZCtlProc)&ZuiLayoutProc;
        p->m_iCurSel = -1;
        return p;
    }
    case ZM_OnDestroy: {
        ZCtlProc old_call = p->old_call;
        ZuiAny old_udata = p->old_udata;

        free(p);

        return old_call(ProcId, cp, old_udata, Param1, Param2);
    }
    case ZM_GetType:
        return (ZuiAny)ZC_TabLayout;
    case ZM_CoreInit:
        return (ZuiAny)TRUE;
    case ZM_CoreUnInit:
        return NULL;
    default:
        break;
    }
    return p->old_call(ProcId, cp, p->old_udata, Param1, Param2);
}

