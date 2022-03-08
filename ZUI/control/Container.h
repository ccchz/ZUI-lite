#ifndef __CONTAINER_H__
#define __CONTAINER_H__
#include <ZUI.h>


/**容器控件结构*/
typedef struct _ZContainer
{

    ZuiColor m_ColorNormal;     //正常状态
    ZuiColor m_ColorHot;        //高亮状态
    ZuiColor m_ColorPushed;     //按下状态
    ZuiColor m_ColorFocused;    //焦点图片
    ZuiColor m_ColorDisabled;   //非激活状态

    int m_dwType;               //按钮状态
    ZCtlProc old_call;
    ZVoid old_udata;
}*ZuiContainer, ZContainer;
ZEXPORT ZINT ZCALL ZuiContainerProc(ZINT ProcId, ZuiControl cp, ZuiContainer p, ZPARAM Param1, ZPARAM Param2);


#endif	//__CONTAINER_H__
