#ifndef __EDIT_H__
#define __EDIT_H__
#include <ZUI.h>

/**Edit控件结构*/
#define BUFFERSIZE  1024
typedef struct _ZEdit
{
    ZuiRes      m_rFont;         //字体
    unsigned int     m_uTextStyle;    //字体控制
    ZuiColor    m_cTextColor;   //字体颜色
    ZuiColor    m_cTextColorDisabled;
    ZuiBool     m_bAutoBreak;   //是否自动换行
    ZuiText     buffer; //字符缓冲区
    ZuiReal     m_dwLineHeight; //行高
    int         m_dwLength; //字符缓冲区长度
    int         m_dwLastPos; //字符串尾部位置
    int         m_dwCaretPos; //光标在字符串中的位置
    int         m_dwLines; //光标行位置
    int         m_dwCols; //光标列位置
    ZuiBool     m_bChangeLines;
    int         m_dwOldCols;
    ZPoint      m_ptCaret;//光标位置
    ZCtlProc    old_call;
} *ZuiEdit, ZEdit;
ZEXPORT ZINT ZCALL ZuiEditProc(ZINT ProcId, ZuiControl cp, ZuiEdit p, ZPARAM Param1, ZPARAM Param2);
ZEXPORT VOID ZCALL  ZEditCalCaretPos(ZuiControl cp, ZuiEdit p);
ZEXPORT int ZCALL ZEditCharStep(ZuiEdit p, int direction);
//设置光标位置
ZEXPORT void ZCALL ZEditSetCaretPos(ZuiEdit p);
#endif	//__EDIT_H__