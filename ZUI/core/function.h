#ifndef __ZUI_CORE_FUNCTION_H__
#define __ZUI_CORE_FUNCTION_H__
#include <ZUI.h>
#include "tree.h"

extern  ZuiText Global_DefaultFontName;     //系统默认字体名称
extern  ZuiRes Global_Font;                //默认字体



void Rect_Join(ZRect *rc, ZRect *rc1);
ZuiBool ZuiIsPointInRect(ZuiRect Rect, ZuiPoint pt);
ZuiBool ZuiIntersectRect(ZuiRect rc, ZuiRect rc1);
/*字符串HASH函数*/
uint32_t Zui_Hash(wchar_t* str);

ZuiText ZuiCharNext(ZuiText str);

/** 此函数用作分割文本.
* @param src 将被分割的字符串,需要内存可写
* @param pSeparator 用作分割的文本
* @param dest 目标文本数组
* @param num 目标数组大小
* @return 此函数没有返回值.
*/
ZuiColor ZuiStr2Color(ZuiText str);
ZuiVoid ZuiStingSplitA(char* src, char* pSeparator, char **dest, int *num);
ZuiVoid ZuiStingSplit(ZuiText src, ZuiText pSeparator, ZuiText *dest, int *num);
ZuiBool ZuiStingIsUtf8(ZuiText str, int length);
int ZuiUtf8ToUnicode(ZuiText str, int slen, ZuiText out, int olen);
int ZuiAsciiToUnicode(ZuiText str, int slen, ZuiText out, int olen);
int ZuiUnicodeToAscii(ZuiText str, int slen, ZuiText out, int olen);
int ZuiUnicodeToUtf8(ZuiText str, int slen, ZuiText out, int olen);


#endif //__ZUI_CORE_FUNCTION_H__
