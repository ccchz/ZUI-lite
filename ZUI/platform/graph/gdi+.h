#ifndef __ZUI_PLATFORM_GRAPH_GDIX_H__
#define __ZUI_PLATFORM_GRAPH_GDIX_H__

#include <ZUI.h>
#ifdef PLATFORM_OS_WIN
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <windows.h>
#endif
/**ͼ��*/
typedef struct _ZImage {
    int Width;
    int Height;
    ZRect src;	//Դ����
    struct graphimage *image;
}*ZuiImage, ZImage;

/**ͼ��*/
typedef struct _ZGraphics {
    int Width;
    int Height;
    ZRect Clip;
#ifdef PLATFORM_OS_WIN
    HWND hwnd;
    HDC hdc;		///�ڴ泡��
    HBITMAP HBitmap;///λͼ���
    BOOL SmoothingMode;
#endif
    struct graphgraphics* ggp;
    void* Bits;		///�ڴ�ָ��
}*ZuiGraphics, ZGraphics;

/**����*/
typedef struct _ZFont {
    ZuiColor TextColor;
    ZuiColor ShadowColor;
    ZuiReal FontSize;       ///�����С
    struct graphfont* font;
}*ZuiFont, ZFont;

#endif //__ZUI_PLATFORM_GRAPH_GDIX_H__
