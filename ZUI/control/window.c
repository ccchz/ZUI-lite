#include "Window.h"
#include <core/tree.h>
#include <core/carray.h>
#include <core/control.h>
#include <core/builder.h>
#include <core/function.h>
#include <layout/Layout.h>
#include <layout/VerticalLayout.h>

typedef struct _ZWindows
{
    RB_ENTRY(_ZWindows) entry;
    uint32_t key;
    ZuiControl p;
}*ZuiWindows, ZWindows;
RB_HEAD(_ZWindows_Tree, _ZWindows);
static int ZWindows_Compare(struct _ZWindows *e1, struct _ZWindows *e2)
{
    return (e1->key < e2->key ? -1 : e1->key > e2->key);
}
RB_GENERATE_STATIC(_ZWindows_Tree, _ZWindows, entry, ZWindows_Compare);

static struct _ZWindows_Tree *m_window = NULL;
DArray *m_window_array = NULL;

ZEXPORT ZuiAny ZCALL ZuiWindowProc(int ProcId, ZuiControl cp, ZuiWindow p, ZuiAny Param1, ZuiAny Param2) {
    switch (ProcId)
    {
    case ZM_OnEvent: {
        switch (((TEventUI*)Param1)->Type)
        {
        case ZEVENT_WINDOWSIZE:
        {
            ZRect rcClient = { 0 };
            rcClient.right = LPARAM(((TEventUI*)Param1)->lParam);
            rcClient.bottom = HPARAM(((TEventUI*)Param1)->lParam);
            ZCCALL(ZM_SetPos, cp, &rcClient, (ZuiAny)FALSE);
            ZuiControlInvalidate(cp, TRUE);
            return 0;
        }
        default:
            break;
        }
        break;
    }
    case ZM_SetText: {
        return (ZuiAny)ZuiOsSetWindowTitle(cp->m_pOs, Param1);
    }
    case ZM_Window_SetWindowMin: {
        return (ZuiAny)ZuiOsSetWindowMin(cp->m_pOs);
    }
    case ZM_Window_SetWindowMax: {
        return (ZuiAny)ZuiOsSetWindowMax(cp->m_pOs);
    }
    case ZM_Window_SetWindowRestor: {
        return (ZuiAny)ZuiOsSetWindowRestor(cp->m_pOs);
    }
    case ZM_Window_SetMinInfo: {
        cp->m_pOs->m_szMinWindow.cx = ((ZuiSize)Param1)->cx;
        cp->m_pOs->m_szMinWindow.cy = ((ZuiSize)Param1)->cy;
        break;
    }
    case ZM_Window_SetMaxInfo: {
        cp->m_pOs->m_szMaxWindow.cx = ((ZuiSize)Param1)->cx;
        cp->m_pOs->m_szMaxWindow.cy = ((ZuiSize)Param1)->cy;
        break;
    }
    case ZM_SetRound: {
        p->old_call(ProcId, cp, p->old_udata, Param1, Param2); //先执行父类过程。
        ZuiOsSetWindowRgn(cp->m_pOs);
        return 0;
    }
    case ZM_Window_SetSize: {
        ZCCALL(ZM_SetFixedWidth, cp, (ZuiAny)((ZuiSize)Param1)->cx, Param2);
        ZCCALL(ZM_SetFixedHeight, cp, (ZuiAny)((ZuiSize)Param1)->cy, Param2);
        return (ZuiAny)(ZuiOsSetWindowSize(cp->m_pOs, ((ZuiSize)Param1)->cx, ((ZuiSize)Param1)->cy));
    }
    case ZM_Window_SetNoBox: {
        return (ZuiAny)(ZuiOsSetWindowNoBox(cp->m_pOs, (ZuiBool)Param1));
    }
    case ZM_Window_SetComBo: {
        return (ZuiAny)(ZuiOsSetWindowComBo(cp->m_pOs, (ZuiBool)Param1));
    }
    case ZM_Window_SetToolWindow: {
        return (ZuiAny)(ZuiOsSetWindowTool(cp->m_pOs, (ZuiBool)Param1));
    }
    case ZM_Window_Popup: {
        cp->m_bVisible = TRUE;
        ZuiOsWindowPopup(cp->m_pOs, Param1);
        return 0;
    }
    case ZM_Window_Center: {
        ZuiOsSetWindowCenter(cp->m_pOs);
        return 0;
    }
    case ZM_SetFont: {
        if (Global_Font)
            ZuiResDBDelRes(Global_Font);
        Global_Font = Param1;
        if (!Param2)
            ZuiControlInvalidate(cp, TRUE);
        return 0;
    }
    case ZM_SetAttribute: {
        ZuiAttribute zAttr = (ZuiAttribute)Param1;
        if (_tcsicmp(zAttr->name, _T("nobox")) == 0)
		ZCCALL(ZM_Window_SetNoBox, cp, (ZuiAny)(_tcsicmp(zAttr->value, _T("true")) == 0 ? TRUE : FALSE), Param2);
        else if (_tcsicmp(zAttr->name, _T("combo")) == 0)
		ZCCALL(ZM_Window_SetComBo, cp, (ZuiAny)(_tcsicmp(zAttr->value, _T("true")) == 0 ? TRUE : FALSE), Param2);
        else if (_tcsicmp(zAttr->name, _T("toolwindow")) == 0)
		ZCCALL(ZM_Window_SetToolWindow, cp, (ZuiAny)(_tcsicmp(zAttr->value, _T("true")) == 0 ? TRUE : FALSE), Param2);
        else if (_tcsicmp(zAttr->name, _T("layered")) == 0) {

        }
        else if (_tcsicmp(zAttr->name, _T("mininfo")) == 0) {
            ZuiText pstr = NULL;
            ZSize sz = { 0 };
            sz.cx = _tcstol(zAttr->value, &pstr, 10);  ASSERT(pstr);
            sz.cy = _tcstol(pstr + 1, &pstr, 10);    ASSERT(pstr);
            ZCCALL(ZM_Window_SetMinInfo, cp, (ZuiAny)&sz, Param2);
        }
        else if (_tcsicmp(zAttr->name, _T("maxinfo")) == 0) {
            ZuiText pstr = NULL;
            ZSize sz = { 0 };
            sz.cx = _tcstol(zAttr->value, &pstr, 10);  ASSERT(pstr);
            sz.cy = _tcstol(pstr + 1, &pstr, 10);    ASSERT(pstr);
            ZCCALL(ZM_Window_SetMaxInfo, cp, (ZuiAny)&sz, Param2);
        }
        else if (_tcsicmp(zAttr->name, _T("size")) == 0) {
            ZuiText pstr = NULL;
            ZSize sz = { 0 };
            sz.cx = _tcstol(zAttr->value, &pstr, 10);  ASSERT(pstr);
            sz.cy = _tcstol(pstr + 1, &pstr, 10);    ASSERT(pstr);
            ZCCALL(ZM_Window_SetSize, cp, (ZuiAny)&sz, Param2);
        }
        if (_tcsicmp(zAttr->name, _T("font")) == 0)
            ZCCALL(ZM_SetFont, cp, ZuiResDBGetRes(zAttr->value, ZREST_FONT), Param2);
        else if (_tcsicmp(zAttr->name, _T("name")) == 0) {
                if (cp->m_sName) {
		            if (_tcsicmp(cp->m_sName, zAttr->value) != 0) {
			            //删除以前的名字
			            ZWindows theNode = { 0 };
			            ZWindows *c;
			            theNode.key = Zui_Hash(cp->m_sName);
			            c = RB_FIND(_ZWindows_Tree, m_window, &theNode);
			            if (c) {
				            RB_REMOVE(_ZWindows_Tree, m_window, c);
				            free(c);
			            }
			            free(cp->m_sName);
			            cp->m_sName = NULL;
		            }
		            else {
			            return 0;
		            }
                }
                //保存现在的名字
                ZWindows *n = (ZWindows *)malloc(sizeof(ZWindows));
                memset(n, 0, sizeof(ZWindows));
                n->key = Zui_Hash(zAttr->value);
                n->p = cp;
                RB_INSERT(_ZWindows_Tree, m_window, n);
                cp->m_sName = _tcsdup(zAttr->value);
        }
        else if (_tcsicmp(zAttr->name, _T("center")) == 0) {
            if (_tcsicmp(zAttr->value, _T("true")) == 0) {
                ZCCALL(ZM_Window_Center, cp, NULL, NULL);
            }
        }
        else if (_tcsicmp(zAttr->name, _T("desktop")) == 0) {
            //嵌入桌面
        }
        break;
    }
    case ZM_SetVisible: {
        if (cp->m_bVisible == (BOOL)Param1)
            return 0;
        ZuiOsSetWindowVisible(cp->m_pOs, (ZuiBool)Param1);
        break;
    }
    case ZM_GetObject:
        if (_tcsicmp(Param1, (ZuiAny)ZC_Window) == 0)
            return (ZuiAny)p;
        break;
    case ZM_GetType: {
        return (ZuiAny)ZC_Window;
    }
	case ZM_SetPos: {
		if (cp->m_pOs->m_bMax) {
			int tmpwidth = cp->m_dwBorderWidth;
			cp->m_dwBorderWidth = 0;
			p->old_call(ProcId, cp, p->old_udata, Param1, Param2);
			cp->m_dwBorderWidth = tmpwidth;
			return 0;
		}
		break;
	}
    case ZM_OnPaintBorder: {
        ZRect* rc = &cp->m_rcItem;
        if (!cp->m_pOs->m_bMax) {
            ZuiDrawRoundRect(cp, cp->m_pOs->m_bIsActive ? cp->m_dwBorderColor2 : cp->m_dwBorderColor, rc, & cp->m_rRound, cp->m_dwBorderWidth);
        }
        return 0;
    }
    case ZM_OnPaintBkColor: {
        ZRect* rc = (ZRect*)&cp->m_rcItem;
        if (cp->m_BkgColor&& cp->m_pOs->m_bMax) {
            ZuiDrawFillRect(cp, cp->m_BkgColor, rc);
            return 0;
        }
        break;
    }
    case ZM_OnCreate: {
        p = (ZuiWindow)malloc(sizeof(ZWindow));
        if (p)
        {
            memset(p, 0, sizeof(ZWindow));
            //保存原来的回调地址,创建成功后回调地址指向当前函数
            //创建继承的控件 保存数据指针
            p->old_udata = ZuiVerticalLayoutProc(ZM_OnCreate, cp, 0, 0, 0);
            p->old_call = (ZCtlProc)&ZuiVerticalLayoutProc;

            darray_append(m_window_array, cp);
            return p;
        }
        return NULL;
    }
	case ZM_OnClose: {
        if ((int)ZuiControlDelayedNotify(ZM_OnClose, cp, Param1, Param2) == -1) { //未设置控件m_pNotify的默认处理。
            ZuiOsAddDelayedCleanup(cp, Param1, Param2);
        }
		return 0;
	}
    case ZM_OnDestroy: {
        ZCtlProc old_call = p->old_call;
        ZuiAny old_udata = p->old_udata;
		ZuiControlNotify(ZM_OnDestroy, cp, Param1, Param2);

		if (cp->m_sName) {
			ZWindows theNode = { 0 };
			ZWindows *c;
			theNode.key = Zui_Hash(cp->m_sName);
			c = RB_FIND(_ZWindows_Tree, m_window, &theNode);
			if (c) {
				RB_REMOVE(_ZWindows_Tree, m_window, c);
				free(c);
			}
		}
        p->m_osWindow = cp->m_pOs;
        old_call(ProcId, cp, old_udata, Param1, Param2);

        ZuiOsDestroyWindow(p->m_osWindow);
		darray_delete(m_window_array,darray_find(m_window_array,cp));
        free(p);

        return 0;
    }
    case ZM_CoreInit: {
        m_window = (struct _ZWindows_Tree*)malloc(sizeof(struct _ZWindows_Tree));
        memset(m_window, 0, sizeof(struct _ZWindows_Tree));
        m_window_array = darray_create();
        return (ZuiAny)TRUE;
    }
    case ZM_CoreUnInit: {
        //这里销毁掉所有窗口
        struct _ZWindows * c = NULL;
        struct _ZWindows * cc = NULL;
        RB_FOREACH_SAFE(c, _ZWindows_Tree, m_window, cc) {
            RB_REMOVE(_ZWindows_Tree, m_window, c);
            free(c);
        }
        free(m_window);
        for (int i = 0; i < m_window_array->count; i++)
        {
            FreeZuiControl(m_window_array->data[i], TRUE);
        }
        darray_destroy(m_window_array);
        return NULL;
    }
    default:
        break;
    }
    return p->old_call(ProcId, cp, p->old_udata, Param1, Param2);
}

ZEXPORT ZuiControl ZCALL ZuiWindowFindName(ZuiText Name) {
    if (!Name)
        return NULL;
    ZWindows theNode = { 0 };
    ZWindows *node;
    theNode.key = Zui_Hash(Name);
    node = RB_FIND(_ZWindows_Tree, m_window, &theNode);
    if (!node)
        return NULL;
    return node->p;
}


