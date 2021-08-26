#ifndef __EDIT_H__
#define __EDIT_H__
#include <ZUI.h>

/**Edit�ؼ��ṹ*/
#define BUFFERSIZE  1024
typedef struct _ZEdit
{
    ZRect		m_rcPadding;
    ZuiRes      m_rFont;         //����
    unsigned int     m_uTextStyle;    //�������
    ZuiColor    m_cTextColor;   //������ɫ
    ZuiColor    m_cTextColorDisabled;
    ZuiText     buffer; //�ַ�������
    int         length; //�ַ�����������
    int         lastposition; //�ַ���β��λ��
    int         caretPosition; //������ַ����е�λ��
    int         lines; //�����λ��
    int         cols; //�����λ��
    ZCtlProc old_call;
} *ZuiEdit, ZEdit;
ZEXPORT ZuiAny ZCALL ZuiEditProc(int ProcId, ZuiControl cp, ZuiEdit p, ZuiAny Param1, ZuiAny Param2);
#endif	//__EDIT_H__