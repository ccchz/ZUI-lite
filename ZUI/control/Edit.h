#ifndef __EDIT_H__
#define __EDIT_H__
#include <ZUI.h>

/**Edit控件结构*/
#define BUFFERSIZE  1024
typedef struct _ZEdit
{
    ZRect		m_rcPadding;
    ZuiRes      m_rFont;         //字体
    unsigned int     m_uTextStyle;    //字体控制
    ZuiColor    m_cTextColor;   //字体颜色
    ZuiColor    m_cTextColorDisabled;
    ZuiText     buffer; //字符缓冲区
    int         length; //字符缓冲区长度
    int         lastposition; //字符串尾部位置
    int         caretPosition; //光标在字符串中的位置
    int         lines; //光标行位置
    int         cols; //光标列位置
    ZCtlProc old_call;
} *ZuiEdit, ZEdit;
ZEXPORT ZuiAny ZCALL ZuiEditProc(int ProcId, ZuiControl cp, ZuiEdit p, ZuiAny Param1, ZuiAny Param2);
#endif	//__EDIT_H__