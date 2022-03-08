#ifndef __EDIT_H__
#define __EDIT_H__
#include <ZUI.h>

/**Edit�ؼ��ṹ*/
#define BUFFERSIZE  1024
typedef struct _ZEdit
{
    ZuiRes      m_rFont;         //����
    unsigned int     m_uTextStyle;    //�������
    ZuiColor    m_cTextColor;   //������ɫ
    ZuiColor    m_cTextColorDisabled;
    ZuiBool     m_bAutoBreak;   //�Ƿ��Զ�����
    ZuiText     buffer; //�ַ�������
    ZuiReal     m_dwLineHeight; //�и�
    int         m_dwLength; //�ַ�����������
    int         m_dwLastPos; //�ַ���β��λ��
    int         m_dwCaretPos; //������ַ����е�λ��
    int         m_dwLines; //�����λ��
    int         m_dwCols; //�����λ��
    ZuiBool     m_bChangeLines;
    int         m_dwOldCols;
    ZPoint      m_ptCaret;//���λ��
    ZCtlProc    old_call;
} *ZuiEdit, ZEdit;
ZEXPORT ZINT ZCALL ZuiEditProc(ZINT ProcId, ZuiControl cp, ZuiEdit p, ZPARAM Param1, ZPARAM Param2);
ZEXPORT VOID ZCALL  ZEditCalCaretPos(ZuiControl cp, ZuiEdit p);
ZEXPORT int ZCALL ZEditCharStep(ZuiEdit p, int direction);
//���ù��λ��
ZEXPORT void ZCALL ZEditSetCaretPos(ZuiEdit p);
#endif	//__EDIT_H__