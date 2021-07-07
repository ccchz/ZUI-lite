#ifndef __BUTTON_H__
#define __BUTTON_H__
#include <ZUI.h>

#define sepSize 8           //间隙尺寸
/**按钮控件结构*/
typedef struct _ZButton
{
    ZuiRes m_ResNormal;     //正常状态
    ZuiRes m_ResHot;        //高亮状态
    ZuiRes m_ResPushed;     //按下状态
    ZuiRes m_ResFocused;    //焦点图片
    ZuiRes m_ResDisabled;   //非激活状态
    ZuiRes m_ResForeground;   //前景图片

    ZuiColor m_ColorNormal;     //正常状态
    ZuiColor m_ColorHot;        //高亮状态
    ZuiColor m_ColorPushed;     //按下状态
    ZuiColor m_ColorFocused;    //焦点图片
    ZuiColor m_ColorDisabled;   //非激活状态

    ZRect m_rcImagePadding;     //图片控制区域
    int m_dwStyle;              //图片文字组合结构-0 左右 1 上下。。。
    int m_dwType;               //按钮状态
    ZCtlProc old_call;
    ZuiAny old_udata;
}*ZuiButton, ZButton;
ZEXPORT ZuiAny ZCALL ZuiButtonProc(int ProcId, ZuiControl cp, ZuiButton p, ZuiAny Param1, ZuiAny Param2);


#endif	//__BUTTON_H__
