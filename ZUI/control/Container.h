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
    ZuiAny old_udata;
}*ZuiContainer, ZContainer;
ZEXPORT ZuiAny ZCALL ZuiContainerProc(int ProcId, ZuiControl cp, ZuiContainer p, ZuiAny Param1, ZuiAny Param2);


#endif	//__CONTAINER_H__
