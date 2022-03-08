#ifndef __CONTAINER_H__
#define __CONTAINER_H__
#include <ZUI.h>


/**�����ؼ��ṹ*/
typedef struct _ZContainer
{

    ZuiColor m_ColorNormal;     //����״̬
    ZuiColor m_ColorHot;        //����״̬
    ZuiColor m_ColorPushed;     //����״̬
    ZuiColor m_ColorFocused;    //����ͼƬ
    ZuiColor m_ColorDisabled;   //�Ǽ���״̬

    int m_dwType;               //��ť״̬
    ZCtlProc old_call;
    ZVoid old_udata;
}*ZuiContainer, ZContainer;
ZEXPORT ZINT ZCALL ZuiContainerProc(ZINT ProcId, ZuiControl cp, ZuiContainer p, ZPARAM Param1, ZPARAM Param2);


#endif	//__CONTAINER_H__
