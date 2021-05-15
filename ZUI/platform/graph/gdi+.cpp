#include "gdi+.h"
#include "graph.h"
#include "core/carray.h"
#include "stdlib.h"
#include <VersionHelpers.h>
#include <comdef.h>
#if PLATFORM_GRAPH_GDIX
#include "gdiplus.h"
#pragma comment(lib, "Gdiplus.lib")
using namespace Gdiplus;

typedef struct
{
    int GdiplusVersion;
    void * DebugEventCallback;
    int SuppressBackgroundThreadc;
    int SuppressExternalCodecs;
}GdiplusStartupInputQ;

#ifdef __cplusplus
extern "C" {
#endif
    int __stdcall GdiplusStartup(int* token, GdiplusStartupInputQ* input, void* output);
    int __stdcall GdiplusShutdown(int token);
    int __stdcall GdipCreateFromHDC(HDC hdc, void** gp);
    int __stdcall GdipDeleteGraphics(void* gp);
    int __stdcall GdipDrawImageI(void* gp, void* Img, int x, int y);
    int __stdcall GdipGraphicsClear(void* gp, ZuiColor incolor);
    int __stdcall GdipLoadImageFromStream(void* stream, void** Img);
    int __stdcall GdipGetImageWidth(void* Img, int* width);
    int __stdcall GdipGetImageHeight(void* Img, int* height);
    int __stdcall GdipImageGetFrameCount(void* Img, char* guid, int* cunt);
    int __stdcall GdipImageSelectActiveFrame(void* Img, char* guid, int* index);
    int __stdcall GdipGetPropertyItemSize(void*, int, int*);
    int __stdcall GdipGetPropertyItem(void*, int, int, char*);
    int __stdcall GdipDisposeImage(void* Img);
    int __stdcall GdipDrawImageRectRectI(void* gp, void* Img, int dstx, int dsty, int dstwidth, int dstheight, int srcx, int srcy, int srcwidth, int srcheight, int srcUnit, void* ImgAttributes, void* callback, void* callbackData);

    DArray* rcDarray;

    int pGdiToken;
    /*��ʼ��ͼ�νӿ�*/
    ZuiBool ZuiGraphInitialize() {
        GdiplusStartupInputQ gdiplusStartupInput;
        memset(&gdiplusStartupInput, 0, sizeof(GdiplusStartupInputQ));
        gdiplusStartupInput.GdiplusVersion = 1;
        GdiplusStartup((int*)&pGdiToken, &gdiplusStartupInput, NULL);//��ʼ��GDI+
        rcDarray = darray_create();
        return TRUE;
    }
    /*����ʼ��ͼ�νӿ�*/
    ZuiVoid ZuiGraphUnInitialize() {
        GdiplusShutdown(pGdiToken);
        darray_destroy(rcDarray);
    }

    /*������*/
    ZEXPORT ZuiVoid ZCALL ZuiDrawFillRect(ZuiGraphics gp, ZuiColor incolor, ZuiRect rc) {
        if (gp)
        {
            SolidBrush brush(incolor);
            Graphics gpp(gp->hdc);
            gpp.FillRectangle(&brush,rc->left,rc->top,rc->right-rc->left,rc->bottom-rc->top);
            gpp.~Graphics();
        }
    }
    /*������*/
    ZEXPORT ZuiVoid ZCALL ZuiDrawRect(ZuiGraphics gp, ZuiColor incolor, ZuiRect rc, int LineWidth) {
        if (gp) {
            Pen pen(incolor, (REAL)LineWidth);
            pen.SetAlignment(PenAlignmentInset);
            Graphics gpp(gp->hdc);
            gpp.DrawRectangle(&pen, rc->left, rc->top, rc->right - rc->left-1, rc->bottom - rc->top-1);
            pen.~Pen();
            gpp.ReleaseHDC(gp->hdc);
            gpp.~Graphics();
 
        }
    }
    //���Բ�Ǿ���
    ZEXPORT ZuiVoid ZCALL ZuiDrawFillRoundRect(ZuiGraphics gp, ZuiColor incolor, ZuiRect rc, int w, int h) {
        if (gp)
        {
            GraphicsPath path;
            SolidBrush brush(incolor);
            Graphics gpp(gp->hdc);
            if (gp->SmoothingMode) {
                gpp.SetSmoothingMode(SmoothingModeAntiAlias);
            }
            path.AddArc((REAL)rc->left, (REAL)rc->top, (REAL)2*w, (REAL)2*h, 180, 90);
            path.AddLine((REAL)rc->left + w, (REAL)rc->top, (REAL)rc->right - w, (REAL)rc->top);
            path.AddArc((REAL)rc->right-2*w-1, (REAL)rc->top, (REAL)2 * w, (REAL)2 * h, 270, 90);
            path.AddLine((REAL)rc->right-1, (REAL)rc->top+h, (REAL)rc->right-1, (REAL)rc->bottom-h);
            path.AddArc((REAL)rc->right - 2*w-1, (REAL)rc->bottom - 2*h-1, (REAL)2* w, (REAL)2 * h, 0, 90);
            path.AddLine((REAL)rc->left+w, (REAL)rc->bottom-1, (REAL)rc->right-w, (REAL)rc->bottom-1);
            path.AddArc((REAL)rc->left, (REAL)rc->bottom - 2*h-1, (REAL)2 * w, (REAL)2 * h, 90, 90);
            path.AddLine((REAL)rc->left, (REAL)rc->bottom-h-1, (REAL)rc->left, (REAL)rc->top + h);
            gpp.FillPath(&brush, &path);
            path.~GraphicsPath();
            gpp.~Graphics();
        }
    }
    //����Բ�Ǿ���
    ZEXPORT ZuiVoid ZCALL ZuiDrawRoundRect(ZuiGraphics gp, ZuiColor incolor, ZuiRect rc, int w, int h, int LineWidth) {
        if (gp) {
            GraphicsPath path;
            Pen pen(incolor, (REAL)LineWidth);
            pen.SetAlignment(PenAlignmentInset);
            Graphics gpp(gp->hdc);
            if (gp->SmoothingMode) {
                gpp.SetSmoothingMode(SmoothingModeAntiAlias);
            }
            path.AddArc((REAL)rc->left, (REAL)rc->top, (REAL)2 * w, (REAL)2 * h, 180, 90);
            path.AddLine((REAL)rc->left + w, (REAL)rc->top, (REAL)rc->right - w, (REAL)rc->top);
            path.AddArc((REAL)rc->right - 2 * w - 1, (REAL)rc->top, (REAL)2 * w, (REAL)2 * h, 270, 90);
            path.AddLine((REAL)rc->right - 1, (REAL)rc->top + h, (REAL)rc->right - 1, (REAL)rc->bottom - h);
            path.AddArc((REAL)rc->right - 2 * w - 1, (REAL)rc->bottom - 2 * h-1, (REAL)2 * w, (REAL)2 * h, 0, 90);
            path.AddLine((REAL)rc->left + w, (REAL)rc->bottom - 1, (REAL)rc->right - w, (REAL)rc->bottom - 1);
            path.AddArc((REAL)rc->left, (REAL)rc->bottom - 2 * h - 1, (REAL)2 * w, (REAL)2 * h, 90, 90);
            path.AddLine((REAL)rc->left, (REAL)rc->bottom - h - 1, (REAL)rc->left, (REAL)rc->top + h);
            gpp.DrawPath(&pen, &path);
            pen.~Pen();
            path.~GraphicsPath();
            gpp.~Graphics();
        }
    }

    //���������
    ZEXPORT ZuiVoid ZCALL ZuiDrawFilltriangle(ZuiGraphics gp, ZuiColor incolor, int x1, int y1, int x2, int y2, int x3, int y3)
    {
        if (gp)
        {
            PointF pt[3];
            SolidBrush brush(incolor);
            Graphics gpp(gp->hdc);
            pt[0].X = (REAL)x1;
            pt[0].Y = (REAL)y1;
            pt[1].X = (REAL)x2;
            pt[1].Y = (REAL)y2;
            pt[2].X = (REAL)x3;
            pt[2].Y = (REAL)y3;
            gpp.FillPolygon(&brush, pt, 3);
            gpp.~Graphics();
        }
    }
    //����������
    ZEXPORT ZuiVoid ZCALL ZuiDrawtriangle(ZuiGraphics gp, ZuiColor incolor, int x1, int y1, int x2, int y2, int x3, int y3, int LineWidth)
    {
        if (gp)
        {
            Point pt[3];
            Pen pen(incolor, (REAL)LineWidth);
            Graphics gpp(gp->hdc);
            pt[0].X = x1;
            pt[0].Y = y1;
            pt[1].X = x2;
            pt[1].Y = y2;
            pt[2].X = x3;
            pt[2].Y = y3;
            pen.~Pen();
            gpp.DrawPolygon(&pen,pt,3);
            gpp.~Graphics();
        }
    }
    /*��ֱ��*/
    ZEXPORT ZuiVoid ZCALL ZuiDrawLine(ZuiGraphics gp, ZuiColor incolor, ZuiRect rc, int LineWidth)
    {
        if (gp) {
            HPEN gPen = CreatePen(PS_SOLID, LineWidth, incolor);
            HPEN oPen = (HPEN)SelectObject(gp->hdc, gPen);
            MoveToEx(gp->hdc, rc->left, rc->top, NULL);
            LineTo(gp->hdc, rc->right, rc->bottom);
            SelectObject(gp->hdc, oPen);
            DeleteObject(gPen);
        }
    }
    /*���ı�(���ռ���õ�����)*/
    ZEXPORT ZuiVoid ZCALL ZuiDrawStringPt(ZuiGraphics gp, ZuiFont Font, ZuiColor incolor, ZuiText String, int StrLens, ZPointR Pt[]) {
        if (String && Font && gp) {

        }
    }
    ZEXPORT ZuiVoid ZCALL ZuiDrawString(ZuiGraphics gp, ZuiFont Font, ZuiText String, int StrLen, ZRect* Rect, ZuiColor incolor, unsigned int TextStyle) {
        if (String && Font && gp) {
            SetTextColor(gp->hdc, ARGB2BGR(incolor));
            SetBkMode(gp->hdc, TRANSPARENT);
            SelectObject(gp->hdc, Font->font);
            DrawTextEx(gp->hdc, String, StrLen, (LPRECT)Rect, TextStyle, NULL);
        }
    }
    /*�����ı���С*/
    ZEXPORT ZuiVoid ZCALL ZuiMeasureTextSize(ZuiFont Font, _ZuiText String, ZuiSizeR Size)
    {
        if (String && Font) {

        }
    }
    /*��ͼ������*/
    ZEXPORT ZuiVoid ZCALL ZuiDrawImageEx(ZuiGraphics gp, ZuiImage Img, int x, int y, int Right, int Bottom, int xSrc, int ySrc, int WidthSrc, int HeightSrc, ZuiByte Alpha) {
        int Width, Height;
        if ((gp && Img)) {
            //wprintf(_T("draw...");
            Image image((LPSTREAM)Img->pstream,0);
            Rect rc(x,y,Right-x,Bottom-y);
            Width = Img->src.right - Img->src.left;
            Height = Img->src.bottom - Img->src.top;
            if (Width == 0 && Height == 0) {
                Width = image.GetWidth();
                Height = image.GetHeight();
            }
            Graphics gpp(gp->hdc);
            gpp.DrawImage(&image, rc, Img->src.left, Img->src.top, Width, Height,UnitPixel);
            gpp.~Graphics();
            image.~Image();
        }
    }
    /*����λͼ*/
    ZEXPORT ZuiVoid ZCALL ZuiAlphaBlendEx(ZuiGraphics Dest, int srcX1, int srcY1, int srcX2, int srcY2, int dstX, int dstY, ZuiGraphics Src, ZuiByte Alpha) {
        if (Dest && Src) {

        }
    }
    ZEXPORT ZuiVoid ZCALL ZuiBitBltEx(ZuiGraphics Dest, int srcX1, int srcY1, int srcX2, int srcY2, int dstX, int dstY, ZuiGraphics Src) {
        if (Dest && Src) {

        }
    }
    /*���ͼ��*/
    ZEXPORT ZuiVoid ZCALL ZuiGraphicsClear(ZuiGraphics gp, ZuiColor incolor) {
        if (gp)
        {

        }
    }
    /*��������*/
    ZEXPORT ZuiFont ZCALL ZuiCreateFont(ZuiText FontName, int FontSize, ZuiBool Bold, ZuiBool Italic) {
        ZuiFont Font = (ZuiFont)malloc(sizeof(ZFont));
        if (!Font) { return NULL; }
        memset(Font, 0, sizeof(ZFont));
        LOGFONT lf;
        GetObject(GetStockObject(SYSTEM_FONT), sizeof(LOGFONT), &lf);
        lf.lfHeight = -MulDiv(FontSize, GetDeviceCaps(GetDC(0), LOGPIXELSY), 72);
        lf.lfWidth = 0;
        lf.lfItalic = Italic;
        lf.lfWeight = FW_NORMAL;
        if (Bold)
            lf.lfWeight = FW_BOLD;
        wsprintf(lf.lfFaceName, _T("%s\0"), FontName);
        Font->font = CreateFont(lf.lfHeight, lf.lfWidth, lf.lfEscapement, lf.lfOrientation, lf.lfWeight, lf.lfItalic,
            lf.lfUnderline, lf.lfStrikeOut, lf.lfCharSet, lf.lfOutPrecision, lf.lfClipPrecision, lf.lfQuality, lf.lfPitchAndFamily, lf.lfFaceName);
        return Font;
    }
    /*��������*/
    ZEXPORT ZuiVoid ZCALL ZuiDestroyFont(ZuiFont Font) {
        if (Font) {
            DeleteObject(Font->font);
            free(Font);
        }
    }
    /*����ͼ��*/
    ZEXPORT ZuiGraphics ZCALL ZuiCreateGraphics(int Width, int Height) {
        ZuiGraphics gp = (ZuiGraphics)malloc(sizeof(ZGraphics));
        if (!gp) { return NULL; }
        memset(gp, 0, sizeof(ZGraphics));
        if (Width && Height) {
            gp->Width = Width;
            gp->Height = Height;
            gp->Clip.left = 1;
            gp->Clip.top = 1;
            gp->Clip.right = Width;
            gp->Clip.bottom = Height;
            HDC tempdc = GetDC(0);
            gp->hdc = CreateCompatibleDC(tempdc);
            gp->hdc = gp->hdc;
            ReleaseDC(0, tempdc);
            BITMAPINFO BitmapInfo;
            memset(&BitmapInfo, 0, sizeof(BitmapInfo));
            BitmapInfo.bmiHeader.biSize = sizeof(BitmapInfo.bmiHeader);
            BitmapInfo.bmiHeader.biBitCount = 32;
            BitmapInfo.bmiHeader.biWidth = Width;
            BitmapInfo.bmiHeader.biHeight = Height;
            BitmapInfo.bmiHeader.biPlanes = 1;
            BitmapInfo.bmiHeader.biCompression = BI_RGB;
            gp->HBitmap = CreateDIBSection(gp->hdc, &BitmapInfo, DIB_RGB_COLORS, &gp->Bits, 0, 0);
            gp->HBitmap = gp->HBitmap;
            if (!gp->HBitmap) {
                DeleteDC(gp->hdc);
                return NULL;
            }
            HBITMAP OldBitmap = (HBITMAP)SelectObject(gp->hdc, (HGDIOBJ)gp->HBitmap);
            if (OldBitmap != NULL) {
                DeleteObject(OldBitmap);
            }
        }
        gp->SmoothingMode = IsWindowsVistaOrGreater();
        return gp;
    }
    /*����ͼ��*/
    ZEXPORT ZuiVoid ZCALL ZuiDestroyGraphics(ZuiGraphics gp) {
        if (gp) {
            if (gp->HBitmap) {
                DeleteObject(gp->HBitmap);
                gp->HBitmap = NULL;
                DeleteDC(gp->hdc);
                gp->hdc = NULL;
            }
            free(gp);
        }
    }
    /*���ӵ�һ���ڴ���*/
    ZEXPORT ZuiGraphics ZCALL ZuiCreateGraphicsAttach(ZuiGraphics gp, ZuiAny bits, int Width, int Height, int stride) {
        if (!gp) { return NULL; }
        if (gp->hdc) {
            DeleteDC(gp->hdc);
            gp->hdc = NULL;
        }
        if (gp->HBitmap) {
            DeleteObject(gp->HBitmap);
            gp->HBitmap = NULL;
        }
        gp->Bits = bits;
        gp->Width = Width;
        gp->Height = Height;
        return gp;
    }
    ZEXPORT ZuiBool ZCALL ZuiGraphicsPushClipRect(ZuiGraphics gp, ZuiRect box, int mode) {
        if (!gp)
            return FALSE;
        ZuiRect prc = (ZuiRect)malloc(sizeof(ZRect));
        if (!prc)
            return FALSE;
        memcpy(prc, &(gp->Clip), sizeof(ZRect));
        darray_append(rcDarray, prc);
        memcpy(&(gp->Clip), box, sizeof(ZRect));

        HRGN hrgn = CreateRectRgn(box->left, box->top, box->right, box->bottom);
        SelectClipRgn(gp->hdc, hrgn);
        DeleteObject(hrgn);
        return TRUE;
    }
    //ZEXPORT ZuiVoid ZCALL PushClipRegion(IRegion* pRegion, int mode){}
    /*����������*/
    ZEXPORT ZuiBool ZCALL ZuiGraphicsPopClip(ZuiGraphics gp) {
        if (!gp)
            return FALSE;
        ZuiRect prc = (ZuiRect)darray_getat(rcDarray, rcDarray->count - 1);
        memcpy(&(gp->Clip), prc, sizeof(ZRect));
        darray_delete(rcDarray, rcDarray->count - 1);

        HRGN hrgn = CreateRectRgn(prc->left, prc->top, prc->right, prc->bottom);
        SelectClipRgn(gp->hdc, hrgn);
        DeleteObject(hrgn);

        free(prc);
        return TRUE;
    }

    //���ü�����
    ZEXPORT ZuiBool ZCALL ZuiGraphicsSetClipBox(ZuiGraphics gp, ZuiRect box, int mode) {

        return FALSE;
    }
    //��ȡ������
    ZEXPORT ZuiBool ZCALL ZuiGraphicsGetClipBox(ZuiGraphics gp, ZuiRect box) {
        return FALSE;
    }
    //���ü�����
    ZEXPORT ZuiBool ZCALL ZuiGraphicsResetClip(ZuiGraphics gp) {
        return FALSE;
    }


    /*����ͼ�����ڴ�*/
    ZEXPORT ZuiImage ZCALL ZuiLoadImageFromBinary(ZuiAny buf, size_t len) {
        ZuiImage Img = (ZuiImage)malloc(sizeof(ZImage));
        if (!Img) { return NULL; }
        memset(Img, 0, sizeof(ZImage));
        HGLOBAL hMem = ::GlobalAlloc(GMEM_MOVEABLE, len);
        if (!hMem) {
            free(Img);
            return NULL;
        }
        LPVOID pMem = ::GlobalLock(hMem);
        memcpy(pMem, buf, len);
        ::GlobalUnlock(hMem);

        // create IStream* from global memory
        HRESULT hr = CreateStreamOnHGlobal(hMem, TRUE, (LPSTREAM*)&Img->pstream);
        if (hr != S_OK){
            GlobalFree(hMem);
            free(Img);
            return NULL;
        }
        Img->len = len;
        return Img;
    }
    /*����ͼ��*/
    ZEXPORT ZuiVoid ZCALL ZuiDestroyImage(ZuiImage Img) {
        if (Img) {
            if (Img->pstream) {
                LPSTREAM(Img->pstream)->Release();
            }
            free(Img);
        }
    }

#ifdef __cplusplus
}
#endif

#endif // PLATFORM_GRAPH_GDIX