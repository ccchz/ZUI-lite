#include "Edit.h"
#include <core/control.h>
#include <core/resdb.h>
#include <core/function.h>
#include <core/builder.h>
#include <platform/platform.h>

//������λ��
ZEXPORT void ZCALL  ZEditCalCaretPos(ZuiControl cp, ZuiEdit p) {
    p->m_dwLineHeight = ZuiFontHeight(cp, p->m_rFont ? p->m_rFont->p : Global_Font->p);
    p->m_ptCaret.x = cp->m_rcItem.left + cp->m_dwBorderWidth + cp->m_rcPadding.left;
    p->m_ptCaret.y = cp->m_rcItem.top + cp->m_dwBorderWidth + cp->m_rcPadding.top;
    if (p->m_uTextStyle & ZDT_SINGLELINE) {
        int bottom = cp->m_rcItem.bottom - cp->m_dwBorderWidth - cp->m_rcPadding.bottom;
        p->m_ptCaret.y += (bottom - p->m_ptCaret.y - p->m_dwLineHeight) / 2;
    }
    //ZSizeR sz;
    //ZuiMeasureTextSize(cp, p->m_rFont ? p->m_rFont->p : Global_Font->p, p->buffer, p->m_dwCaretPos, &sz);
    //p->m_ptCaret.x += (int)sz.cx;
}

//���ù��λ��
ZEXPORT void ZCALL ZEditSetCaretPos(ZuiEdit p){
    SetCaretPos(p->m_ptCaret.x, p->m_ptCaret.y);
    //_tprintf(_T("Position %d,%d.."),p->m_dwLines,p->m_dwCols);
}

//�����ַ���Ҫ�����ļ�ࡣ
ZEXPORT int ZCALL ZEditCharStep(ZuiEdit p, int direction) {
    int count = 0;
    if (direction == -1) {
        if (p->m_dwCaretPos)
            count++;
#ifdef UNICODE
            if ((p->buffer[p->m_dwCaretPos - 1] >= 0xdc00)
                && (p->buffer[p->m_dwCaretPos - 1] <= 0xdfff)) {
                count++;
            }
            else if (p->buffer[p->m_dwCaretPos - 1] == _T('\n')
                && (p->m_dwCaretPos - 2)
                && p->buffer[p->m_dwCaretPos - 2] == _T('\r')) {
                count++;
            }
#else
            if ((p->buffer[p->m_dwCaretPos - 1] >= 0x80) {
                count++;
            }
            else if (p->buffer[p->m_dwCaretPos - 1] == _T('\n')
                && (p->m_dwCaretPos - 2)
                && p->buffer[p->m_dwCaretPos - 2] == _T('\r')) {
                count++;
            }
#endif
    }
    else if (direction == 1) {
        if (p->m_dwCaretPos < p->m_dwLastPos)
            count++;
#ifdef UNICODE
            if ((p->buffer[p->m_dwCaretPos] >= 0xd800)
                && (p->buffer[p->m_dwCaretPos] <= 0xdBff)) {
                count++;
            }
            else if (p->buffer[p->m_dwCaretPos] == _T('\r')
                && (p->m_dwCaretPos + 1 < p->m_dwLastPos)
                && p->buffer[p->m_dwCaretPos + 1] == _T('\n')) {
                count++;
            }
#else
            if ((p->buffer[p->m_dwCaretPos] >= 0x80) {
                count++;
            }
            else if (p->buffer[p->m_dwCaretPos] == _T('\r')
                && (p->m_dwCaretPos + 1 < p->m_dwLastPos)
                && p->buffer[p->m_dwCaretPos + 1] == _T('\n')) {
                count++;
            }
#endif
    }
    return count;
}

ZEXPORT ZuiAny ZCALL ZuiEditProc(int ProcId, ZuiControl cp, ZuiEdit p, ZuiAny Param1, ZuiAny Param2) {
    switch (ProcId)
    {
    case ZM_OnEvent: {
        TEventUI* event = (TEventUI*)Param1;
        switch (event->Type)
        {
        case ZEVENT_SETCURSOR:
        {
            ZuiOsSetCursor((unsigned int)ZIDC_IBEAM);
            return 0;
        }
        case ZEVENT_SETFOCUS:
        {
            //������꣬����λ�á�
            cp->m_bFocused = TRUE;
            CreateCaret(cp->m_pOs->m_hWnd, NULL, 1, p->m_dwLineHeight);
            ShowCaret(cp->m_pOs->m_hWnd);
            ZEditSetCaretPos(p);
            ZuiControlInvalidate(cp, TRUE);
            return 0;
        }
        case ZEVENT_KILLFOCUS:
        {
            //���ٹ��
            cp->m_bFocused = FALSE;
            HideCaret(cp->m_pOs->m_hWnd);
            DestroyCaret();
            ZuiControlInvalidate(cp, TRUE);
            return 0;
        }
        case ZEVENT_KEYDOWN: {
            D_PRINT(_T("down %x.."), event->chKey);
            if (event->chKey == VK_UP) {
                if (p->m_dwCaretPos && !(p->m_uTextStyle & ZDT_SINGLELINE)) {
                    if (!p->m_bChangeLines) {  //��ʼ�ı��ַ���λ�ã������ַ���λ��
                        p->m_dwOldCols = p->m_dwCols;
                        p->m_bChangeLines = TRUE;
                    }
                    int startPos = p->m_dwCaretPos;
                    ZuiBool lineflag = FALSE; //���з����
                    while (startPos) {  //������һ��β��
                        if (p->buffer[--startPos] == _T('\n')) {
                            lineflag = TRUE;
                            break;
                        }
                    }
                    if (lineflag) {
                        int count = 0;
                        lineflag = FALSE;
                        while (startPos) { //���ұ���ͷ��
                            if (p->buffer[--startPos] == _T('\n')) {
                                lineflag = TRUE;
                                break;
                            }
                            if (p->buffer[startPos] == _T('\r'))
                                continue;
                            count++;
                        }
                        if (lineflag) { //����λ��
                            startPos += 1;
                        }
                        p->m_dwLines--;
                        //���λ�ü���
                        p->m_ptCaret.x = cp->m_rcItem.left + cp->m_dwBorderWidth + cp->m_rcPadding.left;
                        p->m_ptCaret.y = cp->m_rcItem.top + cp->m_dwBorderWidth + cp->m_rcPadding.top + (p->m_dwLines -1) * p->m_dwLineHeight;
                        ZSizeR sz;
                        if (count < p->m_dwOldCols) {
                            p->m_dwCaretPos = startPos + count;
                            p->m_dwCols = count;
                            ZuiMeasureTextSize(cp, p->m_rFont ? p->m_rFont->p : Global_Font->p, p->buffer + startPos, count, &sz);
                            p->m_ptCaret.x += (int)(sz.cx + 1);
                        }
                        else {
                            p->m_dwCaretPos = startPos + p->m_dwOldCols;
                            p->m_dwCols = p->m_dwOldCols;
                            ZuiMeasureTextSize(cp, p->m_rFont ? p->m_rFont->p : Global_Font->p, p->buffer + startPos, p->m_dwCols, &sz);
                            p->m_ptCaret.x += (int)(sz.cx + 1);
                        }
                    }
                }
            }
            else if (event->chKey == VK_DOWN) {
                if ((p->m_dwCaretPos < p->m_dwLastPos) && !(p->m_uTextStyle & ZDT_SINGLELINE)) {
                    if (!p->m_bChangeLines) {  //��ʼ�ı��ַ���λ�ã������ַ���λ��
                        p->m_dwOldCols = p->m_dwCols;
                        p->m_bChangeLines = TRUE;
                    }
                    int startPos = p->m_dwCaretPos;
                    ZuiBool lineflag = FALSE; //���з����
                    while (startPos < p->m_dwLastPos) {  //������һ��ͷ��
                        if (p->buffer[startPos++] == _T('\n')) {
                            lineflag = TRUE;
                            break;
                        }
                    }
                    if (lineflag) {
                        p->m_dwCaretPos = startPos;
                        int count = 0;
                        while (startPos < p->m_dwLastPos) { //���ұ���β����
                            if (p->buffer[startPos++] == _T('\n'))
                                break;
                            if (p->buffer[startPos] == _T('\r'))
                                continue;
                            count++;
                        }
                        p->m_dwLines++;
                        //���λ�ü���
                        p->m_ptCaret.x = cp->m_rcItem.left + cp->m_dwBorderWidth + cp->m_rcPadding.left;
                        p->m_ptCaret.y = cp->m_rcItem.top + cp->m_dwBorderWidth + cp->m_rcPadding.top + (p->m_dwLines - 1) * p->m_dwLineHeight;
                        ZSizeR sz;
                        if (count < p->m_dwOldCols) {
                            
                            ZuiMeasureTextSize(cp, p->m_rFont ? p->m_rFont->p : Global_Font->p, p->buffer + p->m_dwCaretPos, count, &sz);
                            p->m_ptCaret.x += (int)(sz.cx + 1);
                            p->m_dwCaretPos += count;
                            p->m_dwCols = count;
                        }
                        else {
                            ZuiMeasureTextSize(cp, p->m_rFont ? p->m_rFont->p : Global_Font->p, p->buffer + p->m_dwCaretPos, p->m_dwOldCols, &sz);
                            p->m_ptCaret.x += (int)(sz.cx + 1);
                            p->m_dwCaretPos += p->m_dwOldCols;
                            p->m_dwCols = p->m_dwOldCols;
                        }
                    }
                }
            }
            else if (event->chKey == VK_LEFT) {
                int count = ZEditCharStep(p, -1);
                p->m_bChangeLines = FALSE;
                if (count) {
                    p->m_dwCaretPos -= count;
                    p->m_bChangeLines = FALSE;
                    //������λ��
                    ZSizeR sz;
                    int startPos = p->m_dwCaretPos;
                    int length = 1;
                    //�������з�ʱ���λ�ü���
                    if (p->buffer[p->m_dwCaretPos] == _T('\n') || p->buffer[p->m_dwCaretPos] == _T('\r')) {
                        //������һ�п�ʼ�ַ�λ��
                        ZuiBool lineflag = FALSE;
                        while (startPos) {
                            if (p->buffer[--startPos] == _T('\n')) {
                                lineflag = TRUE;
                                break;
                            }
                        }
                        if (lineflag)
                            startPos += 1;
                        length = p->m_dwCaretPos - startPos;
                        p->m_dwLines--;
                        p->m_dwCols = length;
                        p->m_ptCaret.x = cp->m_rcItem.left + cp->m_dwBorderWidth + cp->m_rcPadding.left;
                        p->m_ptCaret.y = cp->m_rcItem.top + cp->m_dwBorderWidth + cp->m_rcPadding.top + (p->m_dwLines - 1) * p->m_dwLineHeight;
                        ZuiMeasureTextSize(cp, p->m_rFont ? p->m_rFont->p : Global_Font->p, p->buffer + startPos, length, &sz);                       
                        p->m_ptCaret.x += (int)(sz.cx + 1);
                    }
                    //�����ַ�ʱ���λ�ü���
                    else {
                        ZuiMeasureTextSize(cp, p->m_rFont ? p->m_rFont->p : Global_Font->p, p->buffer + startPos, length, &sz);
                        p->m_ptCaret.x -= (int)(sz.cx + 1);
                        p->m_dwCols--;
                    }
                }
            }
            else if (event->chKey == VK_RIGHT) {
                int count = ZEditCharStep(p, 1);
                p->m_bChangeLines = FALSE;
                if (count) {
                    //�������з�ʱ���λ�ü���
                    if (p->buffer[p->m_dwCaretPos] == _T('\r') || p->buffer[p->m_dwCaretPos] == _T('\n')) {
                        p->m_dwLines++;
                        p->m_dwCols = 0;
                        p->m_ptCaret.y = cp->m_rcItem.top + cp->m_dwBorderWidth + cp->m_rcPadding.top + (p->m_dwLines - 1) * p->m_dwLineHeight;
                        p->m_ptCaret.x = cp->m_rcItem.left + cp->m_dwBorderWidth + cp->m_rcPadding.left;
                    }
                    //�����ַ�ʱ���λ�ü���
                    else {
                        ZSizeR sz;
                        ZuiMeasureTextSize(cp, p->m_rFont ? p->m_rFont->p : Global_Font->p, p->buffer + p->m_dwCaretPos, 1, &sz);
                        p->m_ptCaret.x += (int)(sz.cx + 1);
                        p->m_dwCols++;
                    }
                    p->m_dwCaretPos += count;
                }
            }
            //ɾ���ַ�
            else if (event->chKey == VK_DELETE) {
                int count = ZEditCharStep(p, 1);
                int startPos = p->m_dwCaretPos + count;
                p->m_bChangeLines = FALSE;
                //�ƶ��ַ�
                while (startPos < p->m_dwLastPos) {
                    p->buffer[startPos - count] = p->buffer[startPos];
                    startPos++;
                }
                p->m_dwLastPos -= count;
                p->buffer[p->m_dwLastPos] = _T('\0');
                ZuiControlInvalidate(cp, TRUE);
            }
            ZEditSetCaretPos(p);
            return 0;
        }
        case ZEVENT_CHAR: {
            D_PRINT(_T("%x.."), event->chKey);
            p->m_bChangeLines = FALSE;
            if (event->chKey == VK_RETURN) {
                if (!(p->m_uTextStyle & ZDT_SINGLELINE)) { //�Ƿ���
                    //���������������ӻ������ߴ硣
                    if (p->m_dwLastPos >= p->m_dwLength - 1) { //��Ҫ2���ַ��ռ�
                        p->m_dwLength += BUFFERSIZE;
                        p->buffer = realloc(p->buffer, p->m_dwLength);
                    }
                    //��겻��β�����Ƴ��ַ��ռ�
                    int startPos = p->m_dwLastPos;
                    while (startPos > p->m_dwCaretPos) {
#if defined _WIN32 || _WIN64
                        p->buffer[startPos + 1] = p->buffer[startPos - 1];
#else
                        p->buffer[startPos] = p->buffer[startPos - 1];
#endif
                        startPos--;
                    }
#if defined _WIN32 || _WIN64 || _MAC
                    p->buffer[p->m_dwCaretPos] = event->chKey;
                    p->m_dwCaretPos++;
                    p->m_dwLastPos++;
#endif
#if defined __linux__ || _WIN32 || _WIN64 || __ANDROID__
                    p->buffer[p->m_dwCaretPos] = _T('\n');
                    p->m_dwCaretPos++;
                    p->m_dwLastPos++;
#endif
                    p->m_dwLines++;
                    p->m_dwCols = 0;
                    //���¼�����λ��
                    p->m_ptCaret.y = cp->m_rcItem.top + cp->m_dwBorderWidth + cp->m_rcPadding.top + (p->m_dwLines - 1) * p->m_dwLineHeight;
                    p->m_ptCaret.x = cp->m_rcItem.left + cp->m_dwBorderWidth + cp->m_rcPadding.left;
                }
            }
            else if (event->chKey == VK_BACK) {
                int count = 0;
                if (p->m_dwCaretPos > 0) {
                    count = ZEditCharStep(p, -1);
                    //������λ��
                    ZSizeR sz;
                    int startPos = p->m_dwCaretPos - count;
                    int length = 1;
                    //���������ַ��Ĺ��λ�ü���
                    if (p->buffer[startPos] == _T('\r') || p->buffer[startPos] == _T('\n')) {
                        //������һ�п�ʼλ��
                        ZuiBool lineflag = FALSE;
                        while (startPos) {
                            if (p->buffer[--startPos] == _T('\n')) {
                                lineflag = TRUE;
                                break;
                            }
                        }
                        if (lineflag)
                            startPos += 1;
                        length = p->m_dwCaretPos - count - startPos;
                        p->m_dwLines--;
                        p->m_dwCols = length;
                        p->m_ptCaret.x = cp->m_rcItem.left + cp->m_dwBorderWidth + cp->m_rcPadding.left;
                        p->m_ptCaret.y = cp->m_rcItem.top + cp->m_dwBorderWidth + cp->m_rcPadding.top + (p->m_dwLines - 1) * p->m_dwLineHeight;
                        ZuiMeasureTextSize(cp, p->m_rFont ? p->m_rFont->p : Global_Font->p, p->buffer + startPos, length, &sz);
                        p->m_ptCaret.x += (int)(sz.cx + 1);
                    }
                    //�����ַ��Ĺ��λ�ü���
                    else {
                        p->m_dwCols--;
                        ZuiMeasureTextSize(cp, p->m_rFont ? p->m_rFont->p : Global_Font->p, p->buffer + startPos, length, &sz);
                        p->m_ptCaret.x -= (int)(sz.cx + 1);
                    }
                    //ɾ��ǰһ���ַ�
                    startPos = p->m_dwCaretPos;
                    while (startPos < p->m_dwLastPos) {
                        p->buffer[startPos - count] = p->buffer[startPos];
                        startPos++;
                    }
                    p->m_dwCaretPos -= count;
                    p->m_dwLastPos -= count;
                    p->buffer[p->m_dwLastPos] = _T('\0');
                }
                else {
                    //�����������
                    ZuiBeep(0);
                }
            }
            else if (event->chKey == VK_ESCAPE) {
                ZuiBeep(0);
            }
            //����ʾ�ַ�������ַ�����������
            else {
                //���������������ӻ������ߴ硣
                if (p->m_dwLastPos >= p->m_dwLength) {
                    p->m_dwLength += BUFFERSIZE;
                    p->buffer = realloc(p->buffer, p->m_dwLength);
                }
                //��겻��β�����Ƴ��ַ��ռ䡣
                int startPos = p->m_dwLastPos;
                while (startPos > p->m_dwCaretPos) {
                    p->buffer[startPos] = p->buffer[startPos - 1];
                    startPos--;
                }
                p->buffer[p->m_dwCaretPos] = event->chKey;
                //���¼�����λ��
                ZSizeR sz;
                ZuiMeasureTextSize(cp, p->m_rFont ? p->m_rFont->p : Global_Font->p, p->buffer + p->m_dwCaretPos, 1, & sz);
                p->m_ptCaret.x += (int)(sz.cx + 1);
                p->m_dwCaretPos++;
                p->m_dwLastPos++;
                p->m_dwCols++;
            }
            ZEditSetCaretPos(p);
            ZuiControlInvalidate(cp, TRUE);
            return 0;
        }
        default:
            break;
        }
        break;
    }
    case ZM_SetPos: {
        p->old_call(ProcId, cp, 0, Param1, Param2);
        ZEditCalCaretPos(cp, p);
        return 0;
    }
    case ZM_OnPaintText: {
        ZuiColor tmpTColor;
        if (cp->m_bEnabled)
            tmpTColor = p->m_cTextColor;
        else
            tmpTColor = p->m_cTextColorDisabled;
        ZRect* rc = &cp->m_rcItem;
        ZRect pt;
        pt.left = rc->left + cp->m_rcPadding.left + cp->m_dwBorderWidth;
        pt.top = rc->top + cp->m_rcPadding.top + cp->m_dwBorderWidth;
        pt.right = rc->right - cp->m_rcPadding.right - cp->m_dwBorderWidth;
        pt.bottom = rc->bottom - cp->m_rcPadding.bottom - cp->m_dwBorderWidth;
        if (p->m_rFont)
            ZuiDrawString2(cp, p->m_rFont->p, p->buffer, p->m_dwLastPos, &pt, tmpTColor, p->m_uTextStyle);
        else
            ZuiDrawString2(cp, Global_Font->p, p->buffer, p->m_dwLastPos, &pt, tmpTColor, p->m_uTextStyle);
        return 0;
    }
    case ZM_GetImePoint: {
        return &p->m_ptCaret;
    }
    case ZM_Edit_SetFont: {
        if (p->m_rFont)
            ZuiResDBDelRes(p->m_rFont);
        p->m_rFont = Param1;
        if (!Param2)
            ZuiControlInvalidate(cp, TRUE);
        return 0;
    }
    case ZM_Edit_SetTextColor: {
        p->m_cTextColor = (ZuiColor)Param1;
        if (!Param2)
            ZuiControlInvalidate(cp, TRUE);
        return 0;
    }
    case ZM_Edit_SetTextColorDisabled: {
        p->m_cTextColorDisabled = (ZuiColor)Param1;
        if (!Param2)
            ZuiControlInvalidate(cp, TRUE);
        return 0;
    }
    case ZM_Edit_SetTextStyle: {
        p->m_uTextStyle = (unsigned int)Param1;
        if (!Param2)
            ZuiControlInvalidate(cp, TRUE);
        return 0;
    }
    case ZM_Edit_GetTextStyle: {
        return (ZuiAny)p->m_uTextStyle;
    }
    case ZM_SetAttribute: {
        ZuiAttribute zAttr = (ZuiAttribute)Param1;
        if (_tcsicmp(zAttr->name, _T("font")) == 0)
            ZCCALL(ZM_Edit_SetFont, cp, ZuiResDBGetRes(zAttr->value, ZREST_FONT), Param2);
        else if (_tcsicmp(zAttr->name, _T("textcolor")) == 0) {
            ZuiColor clrColor = ZuiStr2Color(zAttr->value);
            ZCCALL(ZM_Edit_SetTextColor, cp, (ZuiAny)clrColor, Param2);
        }
        else if (_tcsicmp(zAttr->name, _T("textcolordisabled")) == 0) {
            ZuiColor clrColor = ZuiStr2Color(zAttr->value);
            ZCCALL(ZM_Edit_SetTextColorDisabled, cp, (ZuiAny)clrColor, Param2);
        }
        else if (_tcsicmp(zAttr->name, _T("singleline")) == 0) {
            unsigned int tstyle = (unsigned int)ZCCALL(ZM_Edit_GetTextStyle, cp, 0, 0);
            if (_tcsicmp(zAttr->value, _T("true")) == 0) {
                tstyle |= ZDT_SINGLELINE;
                tstyle |= ZDT_VCENTER;
            }
            else {
                tstyle &= ~ZDT_SINGLELINE;
                tstyle &= ~ZDT_VCENTER;
            }
            ZCCALL(ZM_Edit_SetTextStyle, cp, (ZuiAny)tstyle, Param2);
        }
        break;
    }
    case ZM_GetControlFlags: {
        return (ZuiAny)ZFLAG_SETCURSOR;//��Ҫ�������
    }
    case ZM_OnCreate: {
        ZuiEdit np = (ZuiEdit)malloc(sizeof(ZEdit));
        memset(np, 0, sizeof(ZEdit));
        //����ԭ���Ļص���ַ,�����ɹ���ص���ַָ��ǰ����
        np->old_call = cp->call;

        np->buffer = (ZuiText)malloc(BUFFERSIZE);
        memset(np->buffer, 0, BUFFERSIZE);
        np->m_dwLength = BUFFERSIZE;
        np->m_dwLastPos = 0;
        np->m_dwCaretPos = 0;
        np->m_dwLines = 1;
        np->m_dwCols = 0;
        //np->m_uTextStyle = ZDT_VCENTER | ZDT_SINGLELINE;
        np->m_cTextColor = 0xFFd8d8d8;
        np->m_cTextColorDisabled = 0xFFa8a8a8;
        return np;
    }
    case ZM_OnDestroy: {
        ZCtlProc old_call = p->old_call;

        old_call(ProcId, cp, 0, Param1, Param2);
        if (p->m_rFont && !Param1) ZuiResDBDelRes(p->m_rFont);
        free(p);

        return 0;
    }
    case ZM_GetObject:
        if (_tcsicmp(Param1, (ZuiAny)ZC_Edit) == 0)
            return (ZuiAny)p;
        break;
    case ZM_GetType:
        return (ZuiAny)ZC_Edit;
    case ZM_CoreInit:
        return (ZuiAny)TRUE;
    case ZM_CoreUnInit:
        return (ZuiAny)NULL;
    default:
        break;
    }
    return p->old_call(ProcId, cp, 0, Param1, Param2);
}
