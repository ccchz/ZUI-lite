#include "platform/graph.h"
#if PLATFORM_GRAPH_GDIX
#include "gdi+.h"
#include "core/carray.h"
#include "stdlib.h"
#include <core/control.h>
#include <comdef.h>
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

    struct graphgraphics {
        Gdiplus::Graphics* ggp;
        Gdiplus::SolidBrush* brush;
        Gdiplus::Pen* pen;
        Gdiplus::GraphicsPath* path;
        Gdiplus::Region* region;
        //Gdiplus::StringFormat* sf;
    };

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
    ZEXPORT ZuiVoid ZCALL ZuiDrawFillRect(ZuiControl cp, ZuiColor incolor, ZuiRect rc) {
        ZuiGraphics gp = cp->m_pOs->m_hDcOffscreen;
        if (gp)
        {
            gp->ggp->brush->SetColor(incolor);
            Gdiplus::Graphics *gpp = gp->ggp->ggp;
            gpp->FillRectangle(gp->ggp->brush,rc->left,rc->top,rc->right-rc->left,rc->bottom-rc->top);
        }
    }
    /*画矩形*/
    ZEXPORT ZuiVoid ZCALL ZuiDrawRect(ZuiControl cp, ZuiColor incolor, ZuiRect rc, int LineWidth) {
        ZuiGraphics gp = cp->m_pOs->m_hDcOffscreen;
        if (gp) {
            gp->ggp->pen->SetColor(incolor);
            gp->ggp->pen->SetWidth((REAL)LineWidth);
            //pen.SetAlignment(PenAlignmentInset);
            Gdiplus::Graphics* gpp = gp->ggp->ggp;
            gpp->DrawRectangle(gp->ggp->pen, rc->left, rc->top, rc->right - rc->left-1, rc->bottom - rc->top-1);
        }
    }
    //绘制椭圆
    ZEXPORT ZuiVoid ZCALL ZuiDrawEllipse(ZuiControl cp, ZuiColor incolor, ZuiRect rc,int LineWidth) {
        ZuiGraphics gp = cp->m_pOs->m_hDcOffscreen;
        if (gp) {
            gp->ggp->pen->SetColor(incolor);
            gp->ggp->pen->SetWidth((REAL)LineWidth);
            //pen.SetAlignment(PenAlignmentInset);
            Gdiplus::Graphics* gpp = gp->ggp->ggp;
            gpp->DrawEllipse(gp->ggp->pen, rc->left, rc->top, rc->right - rc->left-1, rc->bottom - rc->top-1);;
        }
    }
    //填充椭圆
    ZEXPORT ZuiVoid ZCALL ZuiFillEllipse(ZuiControl cp, ZuiColor incolor, ZuiRect rc) {
        ZuiGraphics gp = cp->m_pOs->m_hDcOffscreen;
        if (gp)
        {
            gp->ggp->brush->SetColor(incolor);
            Gdiplus::Graphics* gpp = gp->ggp->ggp;
            gpp->FillEllipse(gp->ggp->brush, rc->left, rc->top, rc->right - rc->left-1, rc->bottom - rc->top-1);
        }
    }
    //填充圆角矩形
    ZEXPORT ZuiVoid ZCALL ZuiDrawFillRoundRect(ZuiControl cp, ZuiColor incolor, ZuiRect rc, ZRound *rd) {
        ZuiGraphics gp = cp->m_pOs->m_hDcOffscreen;
        if (gp)
        {
            ZRound zrd = { 0 };
            REAL left, top, right, bottom;
            if(rd)
                memcpy(&zrd, rd, sizeof(ZRound));
            left = (REAL)rc->left;
            top = (REAL)rc->top;
            right = (REAL)rc->right-1;
            bottom = (REAL)rc->bottom-1;
            GraphicsPath* path = gp->ggp->path; path->Reset();
            gp->ggp->pen->SetColor(incolor);
            gp->ggp->pen->SetWidth((REAL) 1);
            gp->ggp->pen->SetAlignment(PenAlignmentInset);
            gp->ggp->brush->SetColor(incolor);
            Gdiplus::Graphics* gpp = gp->ggp->ggp;
            path->AddArc(left, top, (REAL)2 * zrd.left, (REAL)2 * zrd.left, 180, 90);
            path->AddLine(left + zrd.left, top, right - zrd.top, top);
            path->AddArc(right - 2 * zrd.top, top, (REAL)2 * zrd.top, (REAL)2 * zrd.top, 270, 90);
            path->AddLine(right, top + zrd.top, right, bottom - zrd.right);
            path->AddArc(right - 2 * zrd.right, bottom - 2 * zrd.right, (REAL)2 * zrd.right, (REAL)2 * zrd.right, 0, 90);
            path->AddLine(left + zrd.bottom, bottom, right - zrd.right, bottom);
            path->AddArc(left, bottom - 2 * zrd.bottom, (REAL)2 * zrd.bottom, (REAL)2 * zrd.bottom, 90, 90);
            path->AddLine(left, bottom - zrd.bottom, left, top + zrd.left);
            gpp->FillPath(gp->ggp->brush, path);
            gpp->DrawPath(gp->ggp->pen, path);
        }
    }
    //绘制圆角矩形
    ZEXPORT ZuiVoid ZCALL ZuiDrawRoundRect(ZuiControl cp, ZuiColor incolor, ZuiRect rc, ZRound *rd, int LineWidth) {
        ZuiGraphics gp = cp->m_pOs->m_hDcOffscreen;
        if (gp) {
            ZRound zrd = { 0 };
            REAL left, top, right, bottom;
            if (rd)
                memcpy(&zrd, rd, sizeof(ZRound));
            left = (REAL)rc->left;
            top = (REAL)rc->top;
            right = (REAL)rc->right-1;
            bottom = (REAL)rc->bottom-1;
            GraphicsPath* path = gp->ggp->path; path->Reset();
            gp->ggp->pen->SetColor(incolor);
            gp->ggp->pen->SetWidth((REAL)LineWidth);
            gp->ggp->pen->SetAlignment(PenAlignmentInset);
            Gdiplus::Graphics* gpp = gp->ggp->ggp;
            path->AddArc(left, top, (REAL)2 * zrd.left, (REAL)2 * zrd.left, 180, 90);
            path->AddLine(left + zrd.left, top, right - zrd.top, top);
            path->AddArc(right - 2 * zrd.top, top, (REAL)2 * zrd.top, (REAL)2 * zrd.top, 270, 90);
            path->AddLine(right, top + zrd.top, right, bottom - zrd.right);
            path->AddArc(right - 2 * zrd.right, bottom - 2 * zrd.right, (REAL)2 * zrd.right, (REAL)2 * zrd.right, 0, 90);
            path->AddLine(left + zrd.bottom, bottom, right - zrd.right, bottom);
            path->AddArc(left, bottom - 2 * zrd.bottom, (REAL)2 * zrd.bottom, (REAL)2 * zrd.bottom, 90, 90);
            path->AddLine(left, bottom - zrd.bottom, left, top + zrd.left);
            gpp->DrawPath(gp->ggp->pen, path);
        }
    }

    //填充三角形
    ZEXPORT ZuiVoid ZCALL ZuiDrawFilltriangle(ZuiControl cp, ZuiColor incolor, int x1, int y1, int x2, int y2, int x3, int y3)
    {
        ZuiGraphics gp = cp->m_pOs->m_hDcOffscreen;
        if (gp)
        {
            Gdiplus::PointF pt[3];
            gp->ggp->brush->SetColor(incolor);
            Gdiplus::Graphics* gpp = gp->ggp->ggp;
            pt[0].X = (REAL)x1;
            pt[0].Y = (REAL)y1;
            pt[1].X = (REAL)x2;
            pt[1].Y = (REAL)y2;
            pt[2].X = (REAL)x3;
            pt[2].Y = (REAL)y3;
            gpp->FillPolygon(gp->ggp->brush, pt, 3);
        }
    }
    //绘制三角形
    ZEXPORT ZuiVoid ZCALL ZuiDrawtriangle(ZuiControl cp, ZuiColor incolor, int x1, int y1, int x2, int y2, int x3, int y3, int LineWidth)
    {
        ZuiGraphics gp = cp->m_pOs->m_hDcOffscreen;
        if (gp)
        {
            Gdiplus::PointF pt[3];
            gp->ggp->pen->SetColor(incolor);
            gp->ggp->pen->SetWidth((REAL)LineWidth);
            gp->ggp->pen->SetAlignment(PenAlignmentInset);
            Gdiplus::Graphics* gpp = gp->ggp->ggp;
            pt[0].X = (REAL)x1;
            pt[0].Y = (REAL)y1;
            pt[1].X = (REAL)x2;
            pt[1].Y = (REAL)y2;
            pt[2].X = (REAL)x3;
            pt[2].Y = (REAL)y3;
            gpp->DrawPolygon(gp->ggp->pen,pt,3);
        }
    }
    /*画直线*/
    ZEXPORT ZuiVoid ZCALL ZuiDrawLine(ZuiControl cp, ZuiColor incolor, ZuiRect rc, int LineWidth)
    {
        ZuiGraphics gp = cp->m_pOs->m_hDcOffscreen;
        if (gp) {
            gp->ggp->pen->SetColor(incolor);
            gp->ggp->pen->SetWidth((REAL)LineWidth);
            Gdiplus::Graphics* gpp = gp->ggp->ggp;
            gpp->DrawLine(gp->ggp->pen,rc->left,rc->top,rc->right,rc->bottom);
        }
    }
    /*画文本(按照计算好的坐标)*/
    ZEXPORT ZuiVoid ZCALL ZuiDrawStringPt(ZuiControl cp, ZuiFont Font, ZuiColor incolor, ZuiText String, int StrLens, ZPointR *Pt) {
        ZuiGraphics gp = cp->m_pOs->m_hDcOffscreen;
        if (String && Font && gp) {
            Gdiplus::PointF rf;
            rf.X = Pt->x;
            rf.Y = Pt->y;
            Gdiplus::Graphics* gpp = gp->ggp->ggp;
            gp->ggp->brush->SetColor(incolor);
            Gdiplus::StringFormat sf;
            sf.GenericTypographic();
            gpp->DrawString(String,StrLens, Font->font->font, rf,&sf, gp->ggp->brush);
        }
    }
    ZEXPORT ZuiVoid ZCALL ZuiDrawString(ZuiControl cp, ZuiFont Font, ZuiText String, int StrLen, ZRect* Rect, ZuiColor incolor, unsigned int TextStyle) {
        ZuiGraphics gp = cp->m_pOs->m_hDcOffscreen;
        if (String && Font && gp) {
            Gdiplus::RectF rf;
            rf.X = (REAL)Rect->left;
            rf.Y = (REAL)Rect->top;
            rf.Width = (REAL)(Rect->right - Rect->left);
            rf.Height = (REAL)(Rect->bottom - Rect->top);
            Gdiplus::Graphics* gpp = gp->ggp->ggp;
            gp->ggp->brush->SetColor(incolor);
            Gdiplus::StringFormat sf;
            sf.GenericTypographic();
            if (TextStyle & ZDT_SINGLELINE) { sf.SetFormatFlags(StringFormatFlagsNoWrap); sf.SetTrimming(StringTrimmingNone); }
            if (TextStyle & ZDT_END_ELLIPSIS) sf.SetTrimming(StringTrimmingEllipsisCharacter);
            if (TextStyle & ZDT_CENTER) sf.SetAlignment(StringAlignmentCenter);
            if (TextStyle & ZDT_VCENTER) {
                sf.SetLineAlignment(StringAlignmentCenter);
            }
            if (TextStyle & ZDT_RIGHT) sf.SetAlignment(StringAlignmentFar);
            if (TextStyle & ZDT_BOTTOM) sf.SetAlignment(StringAlignmentFar);

            gpp->DrawString(String, StrLen, Font->font->font, rf, &sf, gp->ggp->brush);
        }
    }
    ZEXPORT ZuiVoid ZCALL ZuiDrawString2(ZuiControl cp, ZuiFont Font, ZuiText String, int StrLen, ZRect* Rect, ZuiColor incolor, unsigned int TextStyle) {
        ZuiGraphics gp = cp->m_pOs->m_hDcOffscreen;
        if (String && Font && gp) {
            Gdiplus::RectF rf;
            rf.X = (REAL)Rect->left;
            rf.Y = (REAL)Rect->top;
            rf.Width = (REAL)(Rect->right - Rect->left);
            rf.Height = (REAL)(Rect->bottom - Rect->top);
            Gdiplus::Graphics* gpp = gp->ggp->ggp;
            gp->ggp->brush->SetColor(incolor);
            Gdiplus::StringFormat* sf,*sf1 = (Gdiplus::StringFormat*)Gdiplus::StringFormat::GenericTypographic();
            sf = new Gdiplus::StringFormat(sf1);
            sf1->~StringFormat();
            if (TextStyle & ZDT_SINGLELINE) { sf->SetFormatFlags(StringFormatFlagsNoWrap); sf->SetTrimming(StringTrimmingNone); }
            if (TextStyle & ZDT_END_ELLIPSIS) sf->SetTrimming(StringTrimmingEllipsisCharacter);
            if (TextStyle & ZDT_CENTER) sf->SetAlignment(StringAlignmentCenter);
            if (TextStyle & ZDT_VCENTER) {
                sf->SetLineAlignment(StringAlignmentCenter);
            }
            if (TextStyle & ZDT_RIGHT) sf->SetAlignment(StringAlignmentFar);
            if (TextStyle & ZDT_BOTTOM) sf->SetLineAlignment(StringAlignmentFar);

            gpp->DrawString(String, StrLen, Font->font->font, rf, sf, gp->ggp->brush);
            delete sf;
        }
    }
    //字体大小
    ZEXPORT int ZCALL ZuiFontSize(ZuiFont Font)
    {
        // 获取DPI  
        HDC hdc = GetDC(NULL);
        if (hdc != NULL) {
            int g_dpix = GetDeviceCaps(hdc, LOGPIXELSX);
            ReleaseDC(NULL, hdc);
            return (int)(Font->font->font->GetSize() * g_dpix / 72);
        }
        return 0;
    }
    //字体行高
    ZEXPORT int ZCALL ZuiFontHeight(ZuiControl cp, ZuiFont Font)
    {
        ZuiGraphics gp = cp->m_pOs->m_hDcOffscreen;
        Gdiplus::Graphics* gpp = gp->ggp->ggp;
        return (int)Font->font->font->GetHeight(gpp);
    }
    /*测量文本大小*/
    ZEXPORT ZuiVoid ZCALL ZuiMeasureTextSize(ZuiControl cp, ZuiFont Font, ZuiText String,int length, ZuiSizeR Size)
    {
        ZuiGraphics gp = cp->m_pOs->m_hDcOffscreen;
        if (String && Font) {
            Gdiplus::StringFormat *sf = (Gdiplus::StringFormat*)Gdiplus::StringFormat::GenericTypographic();
            sf->SetFormatFlags(StringFormatFlagsMeasureTrailingSpaces);
            Gdiplus::PointF pf;
            pf.X = 0; pf.Y = 0;
            Gdiplus::RectF rf;
            Gdiplus::Graphics* gpp = gp->ggp->ggp;
            gpp->MeasureString(String, length, Font->font->font, pf,sf, &rf);
            //_tprintf(_T("%f..."), rf.Height);
            Size->cx = rf.Width;
            Size->cy = rf.Height;
            sf->~StringFormat();
        }
    }
    /*画图像缩放*/
    ZEXPORT ZuiVoid ZCALL ZuiDrawImageEx(ZuiControl cp, ZuiImage Img, int x, int y, int Right, int Bottom, ZuiByte Alpha) {
        int Width, Height;
        ZuiGraphics gp = cp->m_pOs->m_hDcOffscreen;
        if ((gp && Img)) {
            Gdiplus::Rect rc(x,y,Right-x,Bottom-y);
            Width = Img->src.right - Img->src.left;
            Height = Img->src.bottom - Img->src.top;
            if (Width == 0 && Height == 0) {
                Width = Img->Width;
                Height = Img->Height;
            }
            Gdiplus::Graphics* gpp = gp->ggp->ggp;
            gpp->DrawImage(Img->image->image, rc, Img->src.left, Img->src.top, Width, Height,UnitPixel);
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
    ZEXPORT ZuiVoid ZCALL ZuiGraphicsClear(ZuiControl cp, ZuiColor incolor) {
        ZuiGraphics gp = cp->m_pOs->m_hDcOffscreen;
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
        if (zFont->font->fontfamily->GetLastStatus() != Gdiplus::Ok) {
            LOGFONT lf;
            SystemParametersInfo(SPI_GETICONTITLELOGFONT, sizeof(LOGFONT), &lf, 0);
            zFont->font->fontfamily = new FontFamily(lf.lfFaceName);
        }
        zFont->font->font = new Gdiplus::Font(zFont->font->fontfamily,(REAL)FontSize, fontstyle,Gdiplus::UnitPoint);
        return zFont;
    }
    /*销毁字体*/
    ZEXPORT ZuiVoid ZCALL ZuiDestroyFont(ZuiFont Font) {
        if (Font) {
            delete Font->font->fontfamily;
            delete Font->font->font;
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
        gp->ggp = (struct graphgraphics*)malloc(sizeof(struct graphgraphics));
        if (!gp->ggp) {
            free(gp); return NULL;
        }
        memset(gp->ggp, 0, sizeof(struct graphgraphics));
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
            if (!gp->HBitmap) {
                DeleteDC(gp->hdc);
                return NULL;
            }
            HBITMAP OldBitmap = (HBITMAP)SelectObject(gp->hdc, (HGDIOBJ)gp->HBitmap);
            if (OldBitmap != NULL) {
                DeleteObject(OldBitmap);
            }
        }
        gp->ggp->ggp = new Gdiplus::Graphics(gp->hdc);
        gp->ggp->brush = new Gdiplus::SolidBrush(0xFFFFFFFF);
        gp->ggp->pen = new Gdiplus::Pen(0xFFFFFFFF,1);
        gp->ggp->path = new Gdiplus::GraphicsPath();
        gp->ggp->region = new Gdiplus::Region();
        gp->SmoothingMode = ZuiIsWindowsVersionOrGreater((DWORD)6, (DWORD)0, (DWORD)0);
        if (gp->SmoothingMode) {
            gp->ggp->ggp->SetSmoothingMode(SmoothingModeAntiAlias);
        }
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
            if (gp->ggp) {
                delete gp->ggp->brush;
                delete gp->ggp->pen;
                delete gp->ggp->region;
                delete gp->ggp->path;
                delete gp->ggp->ggp;
                free(gp->ggp);
            }
            free(gp);
        }
    }
    /*附加到一块内存上*/
    ZEXPORT ZuiGraphics ZCALL ZuiCreateGraphicsAttach(ZuiControl cp, ZuiAny bits, int Width, int Height, int stride) {
        ZuiGraphics gp = cp->m_pOs->m_hDcOffscreen;
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
    ZEXPORT ZuiBool ZCALL ZuiGraphicsPushClipRect(ZuiControl cp, ZuiRect box, int mode) {
        ZuiGraphics gp = cp->m_pOs->m_hDcOffscreen;
        if (!gp)
            return FALSE;
        Gdiplus::GraphicsState gps = gp->ggp->ggp->Save();
        darray_append(rcDarray, (ZuiAny)gps);

        ZuiGraphicsSetClip(cp, box, &cp->m_rRound, mode);
        //SelectClipRgn(gp->hdc, hrgn);
        //DeleteObject(hrgn);
        return TRUE;
    }
    /*弹出剪裁区*/
    ZEXPORT ZuiBool ZCALL ZuiGraphicsPopClip(ZuiControl cp) {
        ZuiGraphics gp = cp->m_pOs->m_hDcOffscreen;
        if (!gp)
            return FALSE;
        Gdiplus::GraphicsState gps = (Gdiplus::GraphicsState)darray_getat(rcDarray, rcDarray->count - 1);
        darray_delete(rcDarray, rcDarray->count - 1);
        gp->ggp->ggp->Restore(gps);
        return TRUE;
    }

    //设置剪裁区
    ZEXPORT ZuiBool ZCALL ZuiGraphicsSetClip(ZuiControl cp, ZuiRect rc, ZRound* rd, int mode) {
        ZuiGraphics gp = cp->m_pOs->m_hDcOffscreen;
        REAL left, top, right, bottom;
        ZRound zrd;
        if (cp == cp->m_pOs->m_pRoot) {
            left = (REAL)rc->left - 1;
            top = (REAL)rc->top - 1;
        }
        else {
            left = (REAL)rc->left;
            top = (REAL)rc->top;
        }
        right = (REAL)rc->right;
        bottom = (REAL)rc->bottom;
        memcpy(&zrd, rd, sizeof(ZRound));
        if (zrd.left) zrd.left += 0;
        if (zrd.top) zrd.top += 0;
        if (zrd.right) zrd.right += 0;
        if (zrd.bottom) zrd.bottom += 0;
        GraphicsPath* path = gp->ggp->path; path->Reset();
        Gdiplus::Graphics* gpp = gp->ggp->ggp;
        if (cp->m_pOs->m_bMax && (cp == cp->m_pOs->m_pRoot)) {
            //path.AddLine((REAL)rc->left, (REAL)rc->top, (REAL)rc->right, (REAL)rc->top);
            //path.AddLine((REAL)rc->right, (REAL)rc->top, (REAL)rc->right, (REAL)rc->bottom);
            //path.AddLine((REAL)rc->left, (REAL)rc->bottom, (REAL)rc->right, (REAL)rc->bottom);
            //path.AddLine((REAL)rc->left, (REAL)rc->bottom, (REAL)rc->left, (REAL)rc->top);
            right++; bottom++;
            zrd.left = zrd.right = zrd.top = zrd.bottom = 0;
        }
        //else {
            path->AddArc(left, top, (REAL)2 * zrd.left, (REAL)2 * zrd.left, 180, 90);
            path->AddLine(left + zrd.left, top, right - zrd.top, top);
            path->AddArc(right - 2 * zrd.top, top, (REAL)2 * zrd.top, (REAL)2 * zrd.top, 270, 90);
            path->AddLine(right, top + zrd.top, right, bottom - zrd.right);
            path->AddArc(right - 2 * zrd.right, bottom - 2 * zrd.right, (REAL)2 * zrd.right, (REAL)2 * zrd.right, 0, 90);
            path->AddLine(left + zrd.bottom, bottom, right - zrd.right, bottom);
            path->AddArc(left, bottom - 2 * zrd.bottom, (REAL)2 * zrd.bottom, (REAL)2 * zrd.bottom, 90, 90);
            path->AddLine(left, bottom - zrd.bottom, left, top + zrd.left);
        //}
        Region* region = new Region(path);
        gpp->SetClip(region,(Gdiplus::CombineMode) mode);
        region->~Region();
        delete region;
        return TRUE;
    }
    //获取剪裁区
    ZEXPORT ZuiBool ZCALL ZuiGraphicsGetClipBox(ZuiControl cp, ZuiRect box) {
        return FALSE;
    }
    //重置剪裁区
    ZEXPORT ZuiBool ZCALL ZuiGraphicsResetClip(ZuiControl cp) {
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
            //Img->image->image->~Image();
            delete Img->image->image;
            free(Img->image);
            free(Img);
        }
    }

    ZEXPORT ZuiAny ZCALL ZuiGetRgn(ZuiControl cp, ZuiRect rc, ZRound *rd) {
        ZuiGraphics gp = cp->m_pOs->m_hDcOffscreen;
        REAL left, top, right, bottom;
        ZRound zrd;
        left = (REAL)rc->left - 1;
        top = (REAL)rc->top - 1;
        right = (REAL)rc->right;
        bottom = (REAL)rc->bottom;
        memcpy(&zrd, rd, sizeof(ZRound));
        if (zrd.left) zrd.left += 2;
        if (zrd.top) zrd.top += 2;
        if (zrd.right) zrd.right += 2;
        if (zrd.bottom) zrd.bottom += 2;
        GraphicsPath* path = gp->ggp->path; path->Reset();
        Gdiplus::Graphics* gpp = gp->ggp->ggp;
        if (cp->m_pOs->m_bMax && (cp == cp->m_pOs->m_pRoot)) {
            path->AddLine((REAL)rc->left, (REAL)rc->top, (REAL)rc->right, (REAL)rc->top);
            path->AddLine((REAL)rc->right, (REAL)rc->top, (REAL)rc->right, (REAL)rc->bottom);
            path->AddLine((REAL)rc->left, (REAL)rc->bottom, (REAL)rc->right, (REAL)rc->bottom);
            path->AddLine((REAL)rc->left, (REAL)rc->bottom, (REAL)rc->left, (REAL)rc->top);
        }
        else {
            path->AddArc(left, top, (REAL)2 * zrd.left, (REAL)2 * zrd.left, 180, 90);
            path->AddLine(left + zrd.left, top, right - zrd.top, top);
            path->AddArc(right - 2 * zrd.top, top, (REAL)2 * zrd.top, (REAL)2 * zrd.top, 270, 90);
            path->AddLine(right, top + zrd.top, right, bottom - zrd.right);
            path->AddArc(right - 2 * zrd.right, bottom - 2 * zrd.right, (REAL)2 * zrd.right, (REAL)2 * zrd.right, 0, 90);
            path->AddLine(left + zrd.bottom, bottom, right - zrd.right, bottom);
            path->AddArc(left, bottom - 2 * zrd.bottom, (REAL)2 * zrd.bottom, (REAL)2 * zrd.bottom, 90, 90);
            path->AddLine(left, bottom - zrd.bottom, left, top + zrd.left);
        }
        Region* region = new Region(path);
        HRGN hrgn = region->GetHRGN(gpp);
        region->~Region();
        delete region;
        return hrgn;
    }

#ifdef __cplusplus
}
#endif

#endif // PLATFORM_GRAPH_GDIX