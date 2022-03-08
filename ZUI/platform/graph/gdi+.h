#ifndef __ZUI_PLATFORM_GRAPH_GDIX_H__
#define __ZUI_PLATFORM_GRAPH_GDIX_H__

#include <ZUI.h>
#ifdef PLATFORM_OS_WIN
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <windows.h>
#endif

#define ZCombineModeReplace       0
#define ZCombineModeIntersect     1
#define ZCombineModeUnion         2
#define ZCombineModeXor           3
#define ZCombineModeExclude       4
#define ZCombineModeComplement    5

/**图像*/
typedef struct _ZImage {
    int Width;
    int Height;
    ZRect src;	//源矩形
    struct graphimage *image;
}*ZuiImage, ZImage;

/**图形*/
typedef struct _ZGraphics {
    int Width;
    int Height;
    ZRect Clip;
#ifdef PLATFORM_OS_WIN
    HWND hwnd;
    HDC hdc;		///内存场景
    HBITMAP HBitmap;///位图句柄
    BOOL SmoothingMode;
#endif
    struct graphgraphics* ggp;
    void* Bits;		///内存指针
}*ZuiGraphics, ZGraphics;

/**字体*/
typedef struct _ZFont {
    ZuiColor TextColor;
    ZuiColor ShadowColor;
    ZuiReal FontSize;       ///字体大小
    struct graphfont* font;
}*ZuiFont, ZFont;

#endif //__ZUI_PLATFORM_GRAPH_GDIX_H__
