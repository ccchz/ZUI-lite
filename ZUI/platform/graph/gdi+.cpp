#include "gdi+.h"
#include "graph.h"
#include "core/carray.h"
#include "stdlib.h"
#include <VersionHelpers.h>
#include <core/control.h>
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

    struct graphimage {
        Gdiplus::Image *image;
    };

    struct graphfont {
        Gdiplus::FontFamily* fontfamily;
        Gdiplus::Font* font;
    };

    int pGdiToken;
    /*初始化图形接口*/
    ZuiBool ZuiGraphInitialize() {
        GdiplusStartupInputQ gdiplusStartupInput;
        memset(&gdiplusStartupInput, 0, sizeof(GdiplusStartupInputQ));
        gdiplusStartupInput.GdiplusVersion = 1;
        GdiplusStartup((int*)&pGdiToken, &gdiplusStartupInput, NULL);//初始化GDI+
        rcDarray = darray_create();
        return TRUE;
    }
    /*反初始化图形接口*/
    ZuiVoid ZuiGraphUnInitialize() {
        GdiplusShutdown(pGdiToken);
        darray_destroy(rcDarray);
    }

    /*填充矩形*/
    ZEXPORT ZuiVoid ZCALL ZuiDrawFillRect(ZuiGraphics gp, ZuiColor incolor, ZuiRect rc) {
        if (gp)
        {
            SolidBrush brush(incolor);
            Gdiplus::Graphics gpp(gp->hdc);
            gpp.FillRectangle(&brush,rc->left,rc->top,rc->right-rc->left,rc->bottom-rc->top);
            //gpp.~Graphics();
        }
    }
    /*画矩形*/
    ZEXPORT ZuiVoid ZCALL ZuiDrawRect(ZuiGraphics gp, ZuiColor incolor, ZuiRect rc, int LineWidth) {
        if (gp) {
            Pen pen(incolor, (REAL)LineWidth);
            pen.SetAlignment(PenAlignmentInset);
            Gdiplus::Graphics gpp(gp->hdc);
            gpp.DrawRectangle(&pen, rc->left, rc->top, rc->right - rc->left-1, rc->bottom - rc->top-1);
            pen.~Pen();
            //gpp.ReleaseHDC(gp->hdc);
            //gpp.~Graphics();
        }
    }
    //绘制椭圆
    ZEXPORT ZuiVoid ZCALL ZuiDrawEllipse(ZuiGraphics gp, ZuiColor incolor, ZuiRect rc,int LineWidth) {
        if (gp) {
            Pen pen(incolor, (REAL)LineWidth);
            pen.SetAlignment(PenAlignmentInset);
            Gdiplus::Graphics gpp(gp->hdc);
            if (gp->SmoothingMode) {
                gpp.SetSmoothingMode(SmoothingModeAntiAlias);
            }
            gpp.DrawEllipse(&pen, rc->left, rc->top, rc->right - rc->left, rc->bottom - rc->top);
            pen.~Pen();
            //gpp.ReleaseHDC(gp->hdc);
            //gpp.~Graphics();
        }
    }
    //填充椭圆
    ZEXPORT ZuiVoid ZCALL ZuiFillEllipse(ZuiGraphics gp, ZuiColor incolor, ZuiRect rc) {
        if (gp)
        {
            SolidBrush brush(incolor);
            Gdiplus::Graphics gpp(gp->hdc);
            if (gp->SmoothingMode) {
                gpp.SetSmoothingMode(SmoothingModeAntiAlias);
            }
            gpp.FillEllipse(&brush, rc->left, rc->top, rc->right - rc->left, rc->bottom - rc->top);
            //gpp.~Graphics();
        }
    }
    //填充圆角矩形
    ZEXPORT ZuiVoid ZCALL ZuiDrawFillRoundRect(ZuiGraphics gp, ZuiColor incolor, ZuiRect rc, int w, int h) {
        if (gp)
        {
            REAL left, top, right, bottom;
            left = (REAL)rc->left;
            top = (REAL)rc->top;
            right = (REAL)rc->right-1;
            bottom = (REAL)rc->bottom-1;
            GraphicsPath path;
            Pen pen(incolor, (REAL)1);
            pen.SetAlignment(PenAlignmentInset);
            SolidBrush brush(incolor);
            Gdiplus::Graphics gpp(gp->hdc);
            if (gp->SmoothingMode) {
                gpp.SetSmoothingMode(SmoothingModeAntiAlias);
            }
            path.AddArc(left, top, (REAL)2 * w, (REAL)2 * h, 180, 90);
            path.AddLine(left + w, top, right - w, top);
            path.AddArc(right - 2 * w , top, (REAL)2 * w, (REAL)2 * h, 270, 90);
            path.AddLine(right , top + h, right , bottom - h);
            path.AddArc(right - 2 * w , bottom - 2 * h , (REAL)2 * w, (REAL)2 * h, 0, 90);
            path.AddLine(left + w, bottom , right - w, bottom );
            path.AddArc(left, bottom - 2 * h , (REAL)2 * w, (REAL)2 * h, 90, 90);
            path.AddLine(left, bottom - h , left, top + h);
            gpp.FillPath(&brush, &path);
            gpp.DrawPath(&pen, &path);
            pen.~Pen();
            path.~GraphicsPath();
            //gpp.~Graphics();
        }
    }
    //绘制圆角矩形
    ZEXPORT ZuiVoid ZCALL ZuiDrawRoundRect(ZuiGraphics gp, ZuiColor incolor, ZuiRect rc, int w, int h, int LineWidth) {
        if (gp) {
            REAL left, top, right, bottom;
            left = (REAL)rc->left;
            top = (REAL)rc->top;
            right = (REAL)rc->right-1;
            bottom = (REAL)rc->bottom-1;
            GraphicsPath path;
            Pen pen(incolor, (REAL)LineWidth);
            pen.SetAlignment(PenAlignmentInset);
            Gdiplus::Graphics gpp(gp->hdc);
            if (gp->SmoothingMode) {
                gpp.SetSmoothingMode(SmoothingModeAntiAlias);
            }
            path.AddArc(left, top, (REAL)2 * w, (REAL)2 * h, 180, 90);
            path.AddLine(left + w, top, right - w, (REAL)top);
            path.AddArc(right - 2 * w, top, (REAL)2 * w, (REAL)2 * h, 270, 90);
            path.AddLine(right, top + h, right, (REAL)bottom - h);
            path.AddArc(right - 2 * w, bottom - 2 * h, (REAL)2 * w, (REAL)2 * h, 0, 90);
            path.AddLine(left + w, bottom, right - w, bottom);
            path.AddArc(left, bottom - 2 * h, (REAL)2 * w, (REAL)2 * h, 90, 90);
            path.AddLine(left, bottom - h, left, top + h);
            gpp.DrawPath(&pen, &path);
            pen.~Pen();
            path.~GraphicsPath();
            //gpp.~Graphics();
        }
    }

    //填充三角形
    ZEXPORT ZuiVoid ZCALL ZuiDrawFilltriangle(ZuiGraphics gp, ZuiColor incolor, int x1, int y1, int x2, int y2, int x3, int y3)
    {
        if (gp)
        {
            PointF pt[3];
            SolidBrush brush(incolor);
            Gdiplus::Graphics gpp(gp->hdc);
            if (gp->SmoothingMode) {
                gpp.SetSmoothingMode(SmoothingModeAntiAlias);
            }
            pt[0].X = (REAL)x1;
            pt[0].Y = (REAL)y1;
            pt[1].X = (REAL)x2;
            pt[1].Y = (REAL)y2;
            pt[2].X = (REAL)x3;
            pt[2].Y = (REAL)y3;
            gpp.FillPolygon(&brush, pt, 3);
            //gpp.~Graphics();
        }
    }
    //绘制三角形
    ZEXPORT ZuiVoid ZCALL ZuiDrawtriangle(ZuiGraphics gp, ZuiColor incolor, int x1, int y1, int x2, int y2, int x3, int y3, int LineWidth)
    {
        if (gp)
        {
            Point pt[3];
            Pen pen(incolor, (REAL)LineWidth);
            pen.SetAlignment(PenAlignmentInset);
            Gdiplus::Graphics gpp(gp->hdc);
            if (gp->SmoothingMode) {
                gpp.SetSmoothingMode(SmoothingModeAntiAlias);
            }
            pt[0].X = x1;
            pt[0].Y = y1;
            pt[1].X = x2;
            pt[1].Y = y2;
            pt[2].X = x3;
            pt[2].Y = y3;
            gpp.DrawPolygon(&pen,pt,3);
            pen.~Pen();
            //gpp.~Graphics();
        }
    }
    /*画直线*/
    ZEXPORT ZuiVoid ZCALL ZuiDrawLine(ZuiGraphics gp, ZuiColor incolor, ZuiRect rc, int LineWidth)
    {
        if (gp) {
            Pen pen(incolor, (REAL)LineWidth);
            Gdiplus::Graphics gpp(gp->hdc);
            if (gp->SmoothingMode) {
                gpp.SetSmoothingMode(SmoothingModeAntiAlias);
            }
            gpp.DrawLine(&pen,rc->left,rc->top,rc->right,rc->bottom);
            pen.~Pen();
            //gpp.~Graphics();
        }
    }
    /*画文本(按照计算好的坐标)*/
    ZEXPORT ZuiVoid ZCALL ZuiDrawStringPt(ZuiGraphics gp, ZuiFont Font, ZuiColor incolor, ZuiText String, int StrLens, ZPointR Pt[]) {
        if (String && Font && gp) {

        }
    }
    ZEXPORT ZuiVoid ZCALL ZuiDrawString(ZuiGraphics gp, ZuiFont Font, ZuiText String, int StrLen, ZRect* Rect, ZuiColor incolor, unsigned int TextStyle) {
        if (String && Font && gp) {
            Gdiplus::RectF rf;
            rf.X = (REAL)Rect->left;
            rf.Y = (REAL)Rect->top;
            rf.Width = (REAL)(Rect->right - Rect->left);
            rf.Height = (REAL)(Rect->bottom - Rect->top);
            Gdiplus::Graphics gpp(gp->hdc);
            SolidBrush brush(incolor);
            Gdiplus::StringFormat sf;
            sf.GenericTypographic();
            if (TextStyle & ZDT_SINGLELINE) sf.SetFormatFlags(StringFormatFlagsNoWrap);
            if (TextStyle & ZDT_END_ELLIPSIS) sf.SetTrimming(StringTrimmingEllipsisCharacter);
            if (TextStyle & ZDT_CENTER) sf.SetAlignment(StringAlignmentCenter);
            if (TextStyle & ZDT_VCENTER) {
                sf.SetLineAlignment(StringAlignmentCenter);
                SizeF sf1,sf2;
                sf1.Height = rf.Height;
                sf1.Width = rf.Width;
                int lines;
                gpp.MeasureString(String, -1, Font->font->font,(const SizeF)sf1,&sf, &sf2,0,&lines);
               
                if (sf2.Width < rf.Width) {
                    //_tprintf(_T("%f...%f.."), sf2.Height,Font->font->font->GetHeight(&gpp));
                    sf2.Height = Font->font->font->GetHeight(&gpp);
                    rf.Y = rf.Y + (( rf.Height - sf2.Height) / 2)+1;
                    rf.Height =  sf2.Height;
                }
            }
            if (TextStyle & ZDT_RIGHT) sf.SetAlignment(StringAlignmentFar);
            if (TextStyle & ZDT_BOTTOM) sf.SetAlignment(StringAlignmentFar);

            gpp.DrawString(String, StrLen, Font->font->font,rf,&sf,&brush);
        }
    }
    /*测量文本大小*/
    ZEXPORT ZuiVoid ZCALL ZuiMeasureTextSize(ZuiGraphics gp, ZuiFont Font, ZuiText String, ZuiSizeR Size)
    {
        if (String && Font) {
            Gdiplus::StringFormat sf; //sf.GenericTypographic();
            sf.SetFormatFlags(StringFormatFlagsMeasureTrailingSpaces);
            PointF pf;
            pf.X = 0; pf.Y = 0;
            RectF rf;
            Gdiplus::Graphics gpp(gp->hdc);
            gpp.MeasureString(String, -1, Font->font->font, pf,&sf, &rf);
            //_tprintf(_T("%f..."), rf.Width);
            Size->cx = rf.Width;
            Size->cy = rf.Height;
        }
    }
    /*画图像缩放*/
    ZEXPORT ZuiVoid ZCALL ZuiDrawImageEx(ZuiGraphics gp, ZuiImage Img, int x, int y, int Right, int Bottom, int xSrc, int ySrc, int WidthSrc, int HeightSrc, ZuiByte Alpha) {
        int Width, Height;
        if ((gp && Img)) {
            Rect rc(x,y,Right-x,Bottom-y);
            Width = Img->src.right - Img->src.left;
            Height = Img->src.bottom - Img->src.top;
            if (Width == 0 && Height == 0) {
                Width = Img->Width;
                Height = Img->Height;
            }
            Gdiplus::Graphics gpp(gp->hdc);
            gpp.DrawImage(Img->image->image, rc, Img->src.left, Img->src.top, Width, Height,UnitPixel);
            //gpp.~Graphics();
        }
    }
    /*复制位图*/
    ZEXPORT ZuiVoid ZCALL ZuiAlphaBlendEx(ZuiGraphics Dest, int srcX1, int srcY1, int srcX2, int srcY2, int dstX, int dstY, ZuiGraphics Src, ZuiByte Alpha) {
        if (Dest && Src) {

        }
    }
    ZEXPORT ZuiVoid ZCALL ZuiBitBltEx(ZuiGraphics Dest, int srcX1, int srcY1, int srcX2, int srcY2, int dstX, int dstY, ZuiGraphics Src) {
        if (Dest && Src) {

        }
    }
    /*清除图形*/
    ZEXPORT ZuiVoid ZCALL ZuiGraphicsClear(ZuiGraphics gp, ZuiColor incolor) {
        if (gp)
        {

        }
    }
    /*创建字体*/
    ZEXPORT ZuiFont ZCALL ZuiCreateFont(ZuiText FontName, int FontSize, ZuiBool Bold, ZuiBool Italic) {
        ZuiFont zFont = (ZuiFont)malloc(sizeof(ZFont));
        if (!zFont) {
            return NULL;
        }
        memset(zFont, 0, sizeof(ZFont));
        zFont->font = (struct graphfont*)malloc(sizeof(struct graphfont));
        if (!zFont->font) {
            free(zFont); return NULL;
        }
        Gdiplus::FontStyle fontstyle;
        if (Bold) { 
            if (Italic)
                fontstyle = Gdiplus::FontStyleBoldItalic;
            else
                fontstyle = Gdiplus::FontStyleBold;
        }
        else {
            if (Italic)
                fontstyle = Gdiplus::FontStyleItalic;
            else
                fontstyle = Gdiplus::FontStyleRegular;
        }

        zFont->font->fontfamily = new FontFamily(FontName);
        zFont->font->font = new Gdiplus::Font(zFont->font->fontfamily,(REAL)FontSize, fontstyle,Gdiplus::UnitPoint);
        return zFont;
    }
    /*销毁字体*/
    ZEXPORT ZuiVoid ZCALL ZuiDestroyFont(ZuiFont Font) {
        if (Font) {
            Font->font->fontfamily->~FontFamily();
            Font->font->font->~Font();
            free(Font->font);
            free(Font);
        }
    }
    /*创建图形*/
    ZEXPORT ZuiGraphics ZCALL ZuiCreateGraphics(ZuiOsWindow p,int Width, int Height) {
        ZuiGraphics gp = (ZuiGraphics)malloc(sizeof(ZGraphics));
        if (!gp) {
            return NULL; 
        }
        memset(gp, 0, sizeof(ZGraphics));
        if (Width && Height) {
            gp->Width = Width;
            gp->Height = Height;
            gp->Clip.left = 1;
            gp->Clip.top = 1;
            gp->Clip.right = Width;
            gp->Clip.bottom = Height;
            HDC tempdc = GetDC(p->m_hWnd);
            gp->hdc = CreateCompatibleDC(tempdc);
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
    /*销毁图形*/
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
    /*附加到一块内存上*/
    ZEXPORT ZuiGraphics ZCALL ZuiCreateGraphicsAttach(ZuiGraphics gp, ZuiAny bits, int Width, int Height, int stride) {
        if (!gp) {
            return NULL;
        }
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
    /*弹出剪裁区*/
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

    //设置剪裁区
    ZEXPORT ZuiBool ZCALL ZuiGraphicsSetClip(ZuiGraphics gp, ZuiRect rc, int w, int h, int mode) {

        return TRUE;
    }
    //获取剪裁区
    ZEXPORT ZuiBool ZCALL ZuiGraphicsGetClipBox(ZuiGraphics gp, ZuiRect box) {
        return FALSE;
    }
    //重置剪裁区
    ZEXPORT ZuiBool ZCALL ZuiGraphicsResetClip(ZuiGraphics gp) {
        return FALSE;
    }


    /*加载图像自内存*/
    ZEXPORT ZuiImage ZCALL ZuiLoadImageFromBinary(ZuiAny buf, size_t len) {
        ZuiImage Img = (ZuiImage)malloc(sizeof(ZImage));
        if (!Img) {
            return NULL;
        }
        memset(Img, 0, sizeof(ZImage));
        Img->image = (struct graphimage *)malloc(sizeof(struct graphimage));
        if (!Img->image) {
            free(Img);
            return NULL;
        }

        HGLOBAL hMem = ::GlobalAlloc(GMEM_MOVEABLE, len);
        if (!hMem) {
            free(Img->image);
            free(Img);
            return NULL;
        }
        LPVOID pMem = ::GlobalLock(hMem);
        memcpy(pMem, buf, len);
        ::GlobalUnlock(hMem);

        // create IStream* from global memory
        LPSTREAM pstream;
        HRESULT hr = CreateStreamOnHGlobal(hMem, TRUE, (LPSTREAM*)&pstream);
        if (hr != S_OK){
            GlobalFree(hMem);
            free(Img->image);
            free(Img);
            return NULL;
        }

        Img->image->image = new Image((LPSTREAM)pstream, 0);
        Img->Width = Img->image->image->GetWidth();
        Img->Height = Img->image->image->GetHeight();
        LPSTREAM(pstream)->Release();
        return Img;
    }
    /*销毁图像*/
    ZEXPORT ZuiVoid ZCALL ZuiDestroyImage(ZuiImage Img) {
        if (Img) {
            Img->image->image->~Image();
            free(Img->image);
            free(Img);
        }
    }

    ZEXPORT ZuiVoid ZCALL ZuiSetWindowRgn(ZuiGraphics gp, ZuiRect rc, int w, int h) {
        REAL left, top, right, bottom;
        left = (REAL)rc->left-1;
        top = (REAL)rc->top-1;
        right = (REAL)rc->right;
        bottom = (REAL)rc->bottom;
        GraphicsPath path;
        Gdiplus::Graphics gpp(gp->hdc);
        if (gp->SmoothingMode) {
            gpp.SetSmoothingMode(SmoothingModeAntiAlias);
        }
        if(w && h){
            w+=2; h+=2;
            path.AddArc(left, top, (REAL)2 * w, (REAL)2 * h, 180, 90);
            path.AddLine(left + w, top, right - w, top);
            path.AddArc(right - 2 * w, top, (REAL)2 * w, (REAL)2 * h, 270, 90);
            path.AddLine(right, top + h, right, bottom - h);
            path.AddArc(right - 2 * w, bottom - 2 * h, (REAL)2 * w, (REAL)2 * h, 0, 90);
            path.AddLine(left + w, bottom, right - w, bottom);
            path.AddArc(left, bottom - 2 * h, (REAL)2 * w, (REAL)2 * h, 90, 90);
            path.AddLine(left, bottom - h, left, top + h);
        }
        else {
            path.AddLine((REAL)rc->left, (REAL)rc->top, (REAL)rc->right, (REAL)rc->top);
            path.AddLine((REAL)rc->right, (REAL)rc->top, (REAL)rc->right, (REAL)rc->bottom);
            path.AddLine((REAL)rc->left, (REAL)rc->bottom , (REAL)rc->right, (REAL)rc->bottom );
            path.AddLine((REAL)rc->left, (REAL)rc->bottom, (REAL)rc->left, (REAL)rc->top);
        }
        Region region(&path);
        gpp.SetClip(&region);
        HRGN hrgn = region.GetHRGN(&gpp);
        SetWindowRgn(gp->hwnd, hrgn, TRUE);
        DeleteObject(hrgn);
        path.~GraphicsPath();
        region.~Region();
        //gpp.~Graphics();
    }

#ifdef __cplusplus
}
#endif

#endif // PLATFORM_GRAPH_GDIX