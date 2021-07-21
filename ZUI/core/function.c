#include <ZUI.h>
#include <stdlib.h>
#include <platform/platform.h>
#include <core/Register.h>
#include "control.h"
#include "function.h"
#include "resdb.h"
#include "template.h"
#include "builder.h"
#include "export.h"
ZuiText Global_DefaultFontName = _T("font:name='微软雅黑':size=10");   //系统默认字体名称
ZuiRes Global_Font = NULL;                //默认字体

void Rect_Join(ZRect *rc, ZRect *rc1)
{
    if (rc1->left < rc->left) rc->left = rc1->left;
    if (rc1->top < rc->top) rc->top = rc1->top;
    if (rc1->right > rc->right) rc->right = rc1->right;
    if (rc1->bottom > rc->bottom) rc->bottom = rc1->bottom;
}

void ZuiIntersectRect(ZuiRect rc, ZuiRect rc1)
{
    if (rc1->left > rc->left) rc->left = rc1->left;
    if (rc1->top > rc->top) rc->top = rc1->top;
    if (rc1->right < rc->right) rc->right = rc1->right;
    if (rc1->bottom < rc->bottom) rc->bottom = rc1->bottom;
}

uint32_t Zui_Hash(ZuiText str) {
    uint32_t hash = 0;
    ZText ch;
    for (uint32_t i = 0; ch = *str++; i++)
    {
        if ((i & 1) == 0)
        {
            hash ^= ((hash << 7) ^ ch ^ (hash >> 3));
        }
        else
        {
            hash ^= (~((hash << 11) ^ ch ^ (hash >> 5)));
        }
    }
    return hash;
}
ZuiText ZuiCharNext(ZuiText str) {
    return str + 1;
}

ZEXPORT ZuiBool ZCALL ZuiInit(ZuiInitConfig config) {
    if (!config)
        return FALSE;
#if (defined PLATFORM_OS_WIN)
    if (config && config->m_hInstance) {
        m_hInstance = config->m_hInstance;
    }
    else
    {
        m_hInstance = GetModuleHandle(NULL);
    }
#endif

    /*初始化图形层*/
    if (!ZuiGraphInitialize()) {
        return FALSE;
    }
    /*初始化系统层*/
    if (!ZuiOsInitialize(config)) {
        return FALSE;
    }
    /*初始化模版管理器*/
    if (!ZuiTemplateInit())
    {
        return FALSE;
    }
    /*初始化导出接口*/
    if (!ZuiInitZuiFunc())
    {
        return FALSE;
    }
    /*初始化资源池*/
    if (!ZuiResDBInit()) {
        return FALSE;
    }
    /*加载默认资源*/
    ZuiResDBGetRes(config->default_res, ZREST_ZIP);
    /*初始化全局变量*/

    Global_Font = ZuiResDBGetRes(config->default_font? config->default_font: Global_DefaultFontName, ZREST_FONT);

    /*注册全局控件*/
    if (!ZuiClassInit())
    {
        return FALSE;
    }
    return TRUE;
}
ZEXPORT ZuiBool ZCALL ZuiUnInit() {
    /*反注册全局控件*/
    ZuiClassUnInit();
    /*反初始化模版管理器*/
    ZuiTemplateUnInit();
    /*反初始化资源池*/
    ZuiResDBUnInit();
    /*反初始化全局变量*/
    {
#if (defined PLATFORM_OS_WIN)
        if (Global_Font) {
            ZuiResDBDelRes(Global_Font);
            Global_Font = NULL;
        }
#endif
    }
    /*反初始化系统层*/
    ZuiOsUnInitialize();
    /*反初始化图形层*/
    ZuiGraphUnInitialize();
    return TRUE;
}
ZEXPORT int ZCALL ZuiMsgLoop() {
    return ZuiOsMsgLoop();
}
ZEXPORT ZuiVoid ZCALL ZuiMsgLoop_exit(int nRet) {
    ZuiOsMsgLoopExit(nRet);
}


ZuiControl MsgBox_pRoot;
ZuiAny ZCALL MsgBox_Notify_ctl(int msg, ZuiControl p, ZuiAny UserData, ZuiAny Param1, ZuiAny Param2) {
    if (msg == ZM_OnClick)
    {
        if (_tcsicmp(p->m_sName, _T("WindowCtl_clos")) == 0) {
            ZCCALL(ZM_OnClose, p->m_pOs->m_pRoot, (ZuiAny)ZuiCANCEL, NULL);
        }
        else if (_tcsicmp(p->m_sName, _T("ok")) == 0) {
            ZCCALL(ZM_OnClose, p->m_pOs->m_pRoot, (ZuiAny)ZuiOK, NULL);
        }
        else if (_tcsicmp(p->m_sName, _T("cancel")) == 0) {
            ZCCALL(ZM_OnClose, p->m_pOs->m_pRoot, (ZuiAny)ZuiCANCEL, NULL);
        }
    }
    return 0;
}

ZuiAny ZCALL Default_NotifyProc(int msg, ZuiControl p, ZuiAny UserData, ZuiAny Param1, ZuiAny Param2) {
    if (msg == ZM_OnClose) {
        ZuiOsAddDelayedCleanup(p, Param1, Param2);
    }
    else if (msg == ZM_OnDestroy) {
        ZuiMsgLoop_exit((int)Param1);
    }
    return 0;
}

ZEXPORT int ZCALL ZuiMsgBox(ZuiControl rp, ZuiText text, ZuiText title) {
    ZuiControl p;
    MsgBox_pRoot = NewZuiControl(_T("MessageBox"), NULL, rp);
    if (!MsgBox_pRoot->m_pOs) {
        FreeZuiControl(MsgBox_pRoot, FALSE);
        return 0;
    }
    ZuiControlRegNotify(MsgBox_pRoot, Default_NotifyProc);
    //取消最小化按钮
    p = ZuiControlFindName(MsgBox_pRoot, _T("WindowCtl_min"));
    ZCCALL(ZM_SetVisible, p, FALSE, NULL);
    //取消最大化按钮
    p = ZuiControlFindName(MsgBox_pRoot, _T("WindowCtl_max"));
    ZCCALL(ZM_SetVisible, p, FALSE, NULL);
    //挂接关闭按钮事件
    p = ZuiControlFindName(MsgBox_pRoot, _T("WindowCtl_clos"));
    ZuiControlRegNotify(p, MsgBox_Notify_ctl);
    //挂接确认按钮事件
    p = ZuiControlFindName(MsgBox_pRoot, _T("ok"));
    ZuiControlRegNotify(p, MsgBox_Notify_ctl);
    //挂接取消按钮事件
    p = ZuiControlFindName(MsgBox_pRoot, _T("cancel"));
    ZuiControlRegNotify(p, MsgBox_Notify_ctl);

    p = ZuiControlFindName(MsgBox_pRoot, _T("text"));
    ZCCALL(ZM_SetText, p, text, NULL);
    p = ZuiControlFindName(MsgBox_pRoot, _T("title"));
    ZCCALL(ZM_SetText, p, title, NULL);

    return ZuiDoModel(MsgBox_pRoot);
}
ZuiBool ZuiIsPointInRect(ZuiRect Rect, ZuiPoint pt) {
    int xl, xr, yt, yb;

    if (Rect->left < Rect->right)
    {
        xl = Rect->left;
        xr = Rect->right;
    }
    else
    {
        xl = Rect->right;
        xr = Rect->left;
    }

    if (Rect->top < Rect->bottom)
    {
        yt = Rect->bottom;
        yb = Rect->top;
    }
    else
    {
        yt = Rect->top;
        yb = Rect->bottom;
    }

    return ((pt->x >= xl && pt->x <= xr) && (pt->y >= yb && pt->y <= yt));
}




ZuiBool ZuiStingIsUtf8(ZuiAny str, int length)
{
    int i;
    //UFT8可用1-6个字节编码,ASCII用一个字节
    int nBytes = 0;
    unsigned char chr;
    //如果全部都是ASCII, 说明不是UTF-8
    ZuiBool bAllAscii = TRUE;
    for (i = 0; i < length; i++)
    {
        chr = *((char *)str + i);
        // 判断是否ASCII编码,如果不是,说明有可能是UTF-8,ASCII用7位编码,但用一个字节存,最高位标记为0,o0xxxxxxx
        if ((chr & 0x80) != 0)
        {
            bAllAscii = FALSE;
        }
        //如果不是ASCII码,应该是多字节符,计算字节数
        if (nBytes == 0)
        {
            if (chr >= 0x80)
            {
                if (chr >= 0xFC && chr <= 0xFD)
                {
                    nBytes = 6;
                }
                else if (chr >= 0xF8)
                {
                    nBytes = 5;
                }
                else if (chr >= 0xF0)
                {
                    nBytes = 4;
                }
                else if (chr >= 0xE0)
                {
                    nBytes = 3;
                }
                else if (chr >= 0xC0)
                {
                    nBytes = 2;
                }
                else
                {
                    return FALSE;
                }
                nBytes--;
            }
        }
        //多字节符的非首字节,应为 10xxxxxx
        else
        {
            if ((chr & 0xC0) != 0x80)
            {
                return FALSE;
            }
            nBytes--;
        }
    }
    //违返规则
    if (nBytes > 0)
    {
        return FALSE;
    }
    //如果全部都是ASCII, 也是字符串
    if (bAllAscii)
    {
        return TRUE;
    }
    return TRUE;
}

ZuiVoid ZuiStingSplitA(char* src, char* pSeparator, char **dest, int *num)
{
    char* pStart, *pEnd;
    size_t sep_len;
    int count = 0;
    if (src == NULL || strlen(src) == 0) return;
    sep_len = strlen(pSeparator);
    pStart = src;
    while (1)
    {
        pEnd = strstr(pStart, pSeparator);
        if (pEnd != NULL)
        {
            memset(pEnd, '\0', sep_len);
            *dest++ = pStart;
            pEnd = pEnd + sep_len;
            pStart = pEnd;
            ++count;
        }
        else
        {
            *dest = pStart;
            ++count;
            break;
        }
    }
    *num = count;
}

ZuiVoid ZuiStingSplit(ZuiText src, ZuiText pSeparator, ZuiText *dest, int *num)
{
    ZuiText pStart, pEnd;
    size_t sep_len;
    int count = 0;
    if (src == NULL || _tcslen(src) == 0) return;
    sep_len = _tcslen(pSeparator);
    pStart = src;
    while (1)
    {
        pEnd = _tcsstr(pStart, pSeparator);
        if (pEnd != NULL)
        {
            memset(pEnd, '\0', sep_len * sizeof(_ZuiText));
            *dest++ = pStart;
            pEnd = pEnd + sep_len;
            pStart = pEnd;
            ++count;
        }
        else
        {
            *dest = pStart;
            ++count;
            break;
        }
    }
    *num = count;
}

int ZuiUtf8ToUnicode(ZuiAny str, int slen, ZuiText out, int olen)
{
    return ZuiOsUtf8ToUnicode(str, slen, out, olen);
}

int ZuiAsciiToUnicode(ZuiAny str, int slen, ZuiText out, int olen)
{
    return ZuiOsAsciiToUnicode(str, slen, out, olen);
}

int ZuiUnicodeToAscii(ZuiText str, int slen, ZuiAny out, int olen)
{
    return ZuiOsUnicodeToAscii(str, slen, out, olen);
}
int ZuiUnicodeToUtf8(ZuiText str, int slen, ZuiAny out, int olen) {
    return ZuiOsUnicodeToUtf8(str, slen, out, olen);
}
ZuiColor ZuiStr2Color(ZuiAny str)
{
    ZuiText pstr = NULL;
    ZuiColor clrColor = 0xFFFFFFFF;
    while (*(ZuiText)str > _T('\0') && *(ZuiText)str <= _T(' '))
        str = ZuiCharNext((ZuiText)str);
    if (*(ZuiText)str == _T('#')) {
        str = ZuiCharNext((ZuiText)str);
    }
    else if (*(ZuiText)str == _T('0') && *((ZuiText)str + 1) == _T('x')) {
        str = ZuiCharNext((ZuiText)str);
        str = ZuiCharNext((ZuiText)str);
    }
    else
        return clrColor;
    clrColor = _tcstoul((ZuiText)str, &pstr, 16);
    return clrColor;
}
