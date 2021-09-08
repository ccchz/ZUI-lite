#ifndef __ZUI_H__
#define __ZUI_H__

#include <stddef.h>
#include <stdint.h>
//config
#include "config.h"

#ifdef PLATFORM_OS_WIN
#include <tchar.h>
#include <Windows.h>
    # if defined(BUILDING_ZUI_SHARED)
        //编译成动态库
        #ifdef __cplusplus
        #define ZEXPORT extern "C" __declspec(dllexport)
        #else
        #define ZEXPORT __declspec(dllexport)
        #endif
        #define ZCALL __stdcall
    # elif defined(USING_ZUI_SHARED)
        //使用动态库
        #ifdef __cplusplus
        #define ZEXPORT extern "C" __declspec(dllimport)
        #else
        #define ZEXPORT __declspec(dllimport)
        #endif
        #define ZCALL __stdcall
    # else
        //编译成静态库
        #ifdef __cplusplus
        #define ZEXPORT extern "C"
        #else
        #define ZEXPORT 
        #endif
        #define ZCALL 
    # endif
#else
    #ifdef __cplusplus
        #define ZEXPORT extern "C" 
    #else
        #define ZEXPORT
    #endif
        #define ZCALL __attribute__((__stdcall__))
#endif

#define lengthof(x) (sizeof(x)/sizeof(*x))
#define MAX(a, b) (((a) > (b)) ? (a) : (b))
#define MIN(a, b) (((a) < (b)) ? (a) : (b))
#define CLAMP(x,a,b) (MIN(b,MAX(a,x)))
#ifndef ASSERT
#define ASSERT(expr)  _ASSERTE(expr)
#endif

#define MAKEPARAM(a, b)  (((b) & 0xFFFF) << 16) | ((a) & 0xFFFF)
#define HPARAM(a)   ((a) & 0xFFFF0000) >> 16
#define LPARAM(a)   (a) & 0xFFFF
//--------------------------------------------------------------------基础数据类型
#if defined _WIN64
typedef long long Zint;
typedef unsigned long long ZUint;
#elif defined _WIN32
typedef int Zint;
typedef unsigned int ZUint;
#endif
typedef TCHAR *ZuiText, _ZuiText, ZText;  //内核默认Unicode存储字符
typedef float           ZuiReal;
typedef unsigned int    ZuiBool;
typedef void            ZuiVoid;
typedef void*           ZuiAny;
typedef unsigned int    ZuiColor;
typedef unsigned char   ZuiByte;
/**矩形*/
typedef struct _ZRect
{
    int left;   ///左边
    int top;    ///顶边
    int right;  ///右边
    int bottom; ///底边
} *ZuiRect, ZRect, ZRound;
/**矩形(浮点)*/
typedef struct _ZRectR
{
    ZuiReal left;///左边
    ZuiReal top;///顶边
    ZuiReal right;///宽度
    ZuiReal bottom;///高度
} *ZuiRectR, ZRectR;
/**点*/
typedef struct _ZPoint
{
    int x;
    int y;
} *ZuiPoint, ZPoint;
/**点(浮点)*/
typedef struct _ZPointR
{
    ZuiReal x;
    ZuiReal y;
} *ZuiPointR, ZPointR;
/**大小*/
typedef struct _ZSize
{
    int cx;	///宽度
    int cy;	///高度
} *ZuiSize, ZSize;
/**大小(浮点)*/
typedef struct _ZSizeR
{
	ZuiReal cx;	///宽度
	ZuiReal cy;	///高度
} *ZuiSizeR, ZSizeR;

typedef struct _ZAttribute
{
    ZuiText			name;
    ZuiText			value;
} *ZuiAttribute,ZAttribute;

#define MAKEZRECT(r, L, T, R, B) \
    r.left = L; \
    r.top = T;\
    r.right = R;\
    r.bottom = B;
#define MAKEZRECTP(r, L, T, R, B) \
    r->left = L; \
    r->top = T;\
    r->right = R;\
    r->bottom = B;
//--------------------------------------------------------------------结构定义
//内部结构
typedef struct _ZControl *ZuiControl, ZControl;
//资源包
typedef struct _ZResDB *ZuiResDB, ZResDB;
//单个资源
typedef struct _ZRes *ZuiRes, ZRes;
//图形
typedef struct _ZGraphics *ZuiGraphics, ZGraphics;
//图像
typedef struct _ZImage *ZuiImage, ZImage;
//字体
typedef struct _ZFont *ZuiFont, ZFont;
//路径
typedef struct _ZPath* ZuiPath, ZPath;
//初始化结构
typedef struct _ZuiInitConfig
{
#if (defined PLATFORM_OS_WIN)
    HINSTANCE m_hInstance;  //库所在的模块句柄,动态库默认自动为当前模块,静态链接须设置此参数
    HICON hicon;
#endif
    ZuiBool debug;              //启动调试器
    ZuiText default_res;        //默认资源文件,必备,资源字符串
    ZuiText default_font;   //默认字体名字
    int font_size;
} *ZuiInitConfig, ZInitConfig;

//--------------------------------------------------------------------回调定义
typedef ZuiControl(ZCALL* FINDCONTROLPROC)(ZuiControl, ZuiAny);
typedef ZuiAny(ZCALL *ZCtlProc)(int ProcId, ZuiControl p, ZuiAny UserData, ZuiAny Param1, ZuiAny Param2);
typedef ZuiAny(ZCALL *ZNotifyProc)(int msg, ZuiControl p, ZuiAny Param1, ZuiAny Param2);

#define ZuiLayoutAdd(p,cp) ZuiControlCall(ZM_Layout_Add,(p),(cp),NULL,NULL)
#define ZuiControlSetDrag(p,b)  ZuiControlCall(ZM_SetDrag,(p),(b),NULL,NULL)
#define ZuiControlSetFixedHeight(p,h) ZuiControlCall(ZM_SetFixedHeight,(p),(h),NULL,NULL)
#define ZCCALL(I,C,P1,P2) ZuiControlCall((I),(C),(P1),(P2))

//查找控件参数
#define ZFIND_ALL           0x00000000  //查找全部控件
#define ZFIND_VISIBLE       0x00000001  //查找可视的控件
#define ZFIND_ENABLED       0x00000002  //查找可用的控件
#define ZFIND_HITTEST       0x00000004
#define ZFIND_UPDATETEST    0x00000008
#define ZFIND_TOP_FIRST     0x00000010  //自顶
#define ZFIND_FROM_UPDATE   0x01000000
#define ZFIND_FROM_TAB      0x02000000
#define ZFIND_FROM_POINT    0x04000000
#define ZFIND_FROM_NAME     0x08000000
#define ZFIND_ME_FIRST      0x80000000
//控件标志
#define ZFLAG_TABSTOP       0x00000001  
#define ZFLAG_SETCURSOR     0x00000002  
#define ZFLAG_WANTRETURN    0x00000004  
//--------------------------------------------------------------------ResDB类
/*资源类型*/
enum ZREST
{
    ZREST_IMG = 1,
    ZREST_TXT,
    ZREST_STREAM,
    ZREST_FONT, //字体
    ZREST_ZIP ///此类型比较特殊,如果压缩包包含备注信息则作为资源包打开,由ResDB管理,反之则作为普通资源由Res管理
};
//--------------------------------------------------------------------Debug类
#define ZLOG_TYPE_ERROR     0
#define ZLOG_TYPE_DEBUG     1
#define ZLOG_TYPE_WARNING   2
#define ZLOG_TYPE_INFO      3

#define ZuiOnSize  -1
#define ZuiOK		1
#define ZuiCANCEL	2
#define ZuiYES		3
#define ZuiNO		4
//--------------------------------------------------------------------plugin类
#if 1

#endif
//--------------------------------------------------------------------Base类
#if 1
//功能宏
#define ZC_Null               _T("Null")
#define ZTYLE_BOX             1   //单线边框
#define ZTYLE_BKGColor        2   //具有背景色
//-----控件事件
#define ZM_CoreInit           1   //用于控件内核初始化
#define ZM_CoreUnInit         2   //用于控件内核反初始化
#define ZM_GetType            3   //取控件类型
#define ZM_GetObject          4  //获取控件被继承后某一类型的对象
#define ZM_OnCreate           5   //内核创建
#define ZM_OnInit             6   //用户初始化
#define ZM_OnClose			  7
#define ZM_OnDestroy          8   //销毁
#define ZM_OnSize             9   //
#define ZM_OnEvent            10   //
#define ZM_OnNotify           11   //
#define ZM_OnPaint            12  // 绘制循序：背景颜色->背景图->状态图->文本->边框
#define ZM_EndPaint           13  //子控件绘制完毕后调用
#define ZM_OnPaintBkColor     14  //背景色
#define ZM_OnPaintBkImage     15  //背景图片
#define ZM_OnPaintStatusImage 16  //状态图片
#define ZM_OnPaintText        17  //文本
#define ZM_OnPaintBorder      18  //边框
#define ZM_OnPostPaint        19  //
#define ZM_FindControl        20  //查找控件
//----控件属性
#define ZM_SetAttribute       25  //解析属性
#define ZM_GetAttribute       26  //取属性
#define ZM_InActivate         27  //不活动
#define ZM_Activate           28  //活动
#define ZM_SetVisible         29  //设置是否可视
#define ZM_GetVisible         30  //是否可视
#define ZM_SetText            31  //设置文本
#define ZM_GetText            32  //获取文本
#define ZM_SetName            33  //设置控件名字
#define ZM_GetName            34  //获取控件名字
#define ZM_SetTooltip         35  //设置提示文本
#define ZM_GetPos             36  //得到控件位置
#define ZM_SetPos             37  //设置控件位置并重绘
#define ZM_SetOs              38  //设置控件的绘制管理者
//设置大小的限制值
#define ZM_GetMinWidth        39  //
#define ZM_SetMinWidth        40  //
#define ZM_GetMaxWidth        41  //
#define ZM_SetMaxWidth        42  //
#define ZM_GetMinHeight       43  //
#define ZM_SetMinHeight       44  //
#define ZM_GetMaxHeight       45  //
#define ZM_SetMaxHeight       46  //
#define ZM_GetWidth           47  //
#define ZM_GetHeight          48  //
#define ZM_GetX               49  //
#define ZM_GetY               50
#define ZM_EstimateSize       51  //获取自适应大小
#define ZM_SetFloatPercent    52  //
#define ZM_GetPadding         53
#define ZM_SetPadding         54  // 设置外边距，由上层窗口绘制
#define ZM_GetFixedXY         55  // 实际大小位置使用GetPos获取，这里得到的是预设的参考值
#define ZM_SetFixedXY         56  // 仅float为true时有效
#define ZM_GetFixedWidth      57  // 实际大小位置使用GetPos获取，这里得到的是预设的参考值
#define ZM_SetFixedWidth      58  // 预设的参考值
#define ZM_GetFixedHeight     59  // 实际大小位置使用GetPos获取，这里得到的是预设的参考值
#define ZM_SetFixedHeight     60  // 预设的参考值
#define ZM_GetRelativePos     61  // 相对(父控件)位置
#define ZM_GetImePoint        62  // 获取输入法位置
#define ZM_SetFloat           63  //设置为浮动控件
#define ZM_SetEnabled         64  //设置可用状态
#define ZM_GetEnabled         65
#define ZM_SetMouseEnabled    66  //设置鼠标响应
#define ZM_SetFocus           67  //设置焦点
#define ZM_KillFocus          68  //设置焦点
#define ZM_SetDrag            69  //设置拖拽控件
#define ZM_SetRound           70  //设置圆角
#define ZM_GetRound           71
#define ZM_SetAnimation       72  //设置动画类型
#define ZM_SetAnimationType   73  //设置动画类型
#define ZM_SetFont            74  //设置缺省字体
#define ZM_SetBkColor         75  //设置背景色
#define ZM_SetBkImage         76 //设置背景图片
#define ZM_SetBorderColor     77  //设置边框颜色
#define ZM_SetBorderColor2    78  //设置边框颜色
#define	ZM_SetBorderWidth     79
#define	ZM_GetBorderWidth	  80
#define ZM_SetNotifyPop       81 //事件冒泡
#define ZM_Invalidate         82  //刷新显示
#define ZM_GetParent          83
#define ZM_GetControlFlags    84  //
#define ZM_GetMargin          85
#define ZM_SetMargin          86 

//输入事件
#define ZM_OnChar             100
#define ZM_OnClick            101
#define ZM_OnDbClick          102
#define ZM_OnLButtonDown      103
#define ZM_OnRButtonDown      104
#define ZM_OnMouseEnter       105
#define ZM_OnMouseLeave       106
#define ZM_OnMouseMove        107
#define ZM_OnSelectChanged    108
#define ZM_OnSetFocus         109  //设置焦点
#define ZM_OnKillFocus        110
 



#endif // 1

//--------------------------------------------------------------------Layout类
#if 1
#define ZC_Layout                     _T("Layout")
#define ZC_VerticalLayout             _T("VerticalLayout")
#define ZC_HorizontalLayout           _T("HorizontalLayout")

//--------base
#define ZM_Layout_Add                 201 ///添加控件
#define ZM_Layout_AddAt               202 ///插入控件
#define ZM_Layout_Remove              203 ///移除指定控件, 参数1为真由调用方销毁成员
#define ZM_Layout_RemoveAt            204 ///移除指定位置控件
#define ZM_Layout_RemoveAll           205 ///移除全部控件
#define ZM_Layout_GetCount            206 ///取容器控件数量
#define ZM_Layout_GetItemIndex        207 ///取指定控件索引
#define ZM_Layout_SetItemIndex        208 ///设置控件所在位置
#define ZM_Layout_GetItemAt           209 ///索引取控件
#define ZM_Layout_SetFloatPos         210 ///设置浮动控件位置
#define ZM_Layout_SetInset            212 ///设置内边距
//--------滚动条（由Layout管理）
#define ZM_Layout_GetScrollPos        213
#define ZM_Layout_GetScrollRange      214
#define ZM_Layout_SetScrollPos        215
#define ZM_Layout_SetScrollStepSize   216
#define ZM_Layout_GetScrollStepSize   217
#define ZM_Layout_LineUp              218
#define ZM_Layout_LineDown            219
#define ZM_Layout_PageUp              220
#define ZM_Layout_PageDown            221
#define ZM_Layout_HomeUp              222
#define ZM_Layout_EndDown             223
#define ZM_Layout_LineLeft            224
#define ZM_Layout_LineRight           225
#define ZM_Layout_PageLeft            226
#define ZM_Layout_PageRight           227
#define ZM_Layout_HomeLeft            228
#define ZM_Layout_EndRight            229
#define ZM_Layout_EnableScrollBar     230
#define ZM_Layout_GetVerticalScrollBar    231
#define ZM_Layout_GetHorizontalScrollBar  232
#define ZM_Layout_ProcessScrollBar    233 //调整滚动条位置
#define ZM_Layout_GetChildAlign       234
#define ZM_Layout_GetChildVAlign      235
#define ZM_Layout_SetChildAlign       236
#define ZM_Layout_SetChildVAlign      237
#define ZM_Layout_SetChildPadding     238

//--------Tile
#define ZC_TileLayout                 _T("TileLayout")
#define ZM_TileLayout_SetColumns      300
#define ZM_TileLayout_SetItemSize     301

//--------Grid
#define ZC_GridLayout                 _T("GridLayout")
#define ZM_GridSetSize                310

//--------Tab
#define ZC_TabLayout                  _T("TabLayout")
#define ZM_TabLayout_SetSelectIndex   320    //当前Tab
#define ZM_TabLayout_GetSelectIndex   321

#endif // 1

//--------------------------------------------------------------------Label类
#if 1
#define ZC_Label                          _T("Label")
#define ZM_Label_SetFont                  330     //设置字体
#define ZM_Label_SetTextColor             331     //设置文本颜色
#define ZM_Label_SetTextPadding           332     //字体边距
#define ZM_Label_SetTextColorDisabled     333
#define ZM_Label_SetTextStyle             334
#define ZM_Label_GetTextStyle             335
#endif // 1

//--------------------------------------------------------------------Button类
#if 1
#define ZC_Button                   _T("Button")
#define ZM_Button_SetResNormal      340    //普通颜色
#define ZM_Button_SetResHot         341    //高亮状态
#define ZM_Button_SetResPushed      342    //按下状态
#define ZM_Button_SetResFocused     343    //焦点图片
#define ZM_Button_SetResDisabled    344    //非激活状态
#define ZM_Button_SetResForeground  345    //前景图片
#define ZM_Button_SetColorNormal    346    //普通颜色
#define ZM_Button_SetColorHot       347    //高亮状态
#define ZM_Button_SetColorPushed    348    //按下状态
#define ZM_Button_SetColorFocused   349    //焦点颜色
#define ZM_Button_SetColorDisabled  350    //非激活状态
#define ZM_Button_SetBorderColorHot 351   //激活状态下边框颜色
#define ZM_Button_SetImagePadding   352
#define ZM_Button_SetStyle          353
#endif // 1

//--------------------------------------------------------------------Container类
#if 1
#define ZC_Container                    _T("Container")
#define ZM_Container_SetColorNormal     360              //普通颜色
#define ZM_Container_SetColorHot        361              //高亮状态颜色
#define ZM_Container_SetColorPushed     362              //按下状态颜色
#define ZM_Container_SetColorFocused    363              //焦点颜色
#define ZM_Container_SetColorDisabled   364              //禁用状态颜色
#endif // 1
//--------------------------------------------------------------------DrawPanel类
#if 1
#define ZC_DrawPanel                    _T("DrawPanel")
#endif // 1
//--------------------------------------------------------------------SplitterBar类
#if 1
#define ZC_SplitterBar                  _T("SplitterBar")
#endif // 1
//--------------------------------------------------------------------ProgressBar类
#if 1
#define ZC_ProgressBar                  _T("ProgressBar")
#define ZM_ProgressBar_SetColor         370
#define ZM_ProgressBar_SetBackColor     371
#define ZM_ProgressBar_SetPos           372
#define ZM_ProgressBar_SetRange         373
#define ZM_ProgressBar_SetHeight        374
#define ZM_ProgressBar_SetHorizontal    375

#endif

//--------------------------------------------------------------------CheckBox类
#if 1
#define ZC_CheckBox                     _T("CheckBox")
#endif // 1
//--------------------------------------------------------------------List类
#if 1
#define ZC_List                         _T("List")
#define ZC_ListBody                     _T("ListBody")
#define ZC_ListElement                  _T("ListElement")
#define ZC_ListHeader                   _T("ListHeader")
#define ZC_ListHeaderItem               _T("ListHeaderItem")
#define ZM_List_Add                     ZM_Layout_Add
#define ZM_List_GetCount                ZM_Layout_GetCount
#define ZM_List_AddAt                   ZM_Layout_AddAt
#define ZM_List_Remove                  ZM_Layout_Remove
#define ZM_List_RemoveAt                ZM_Layout_RemoveAt
#define ZM_List_RemoveAll               ZM_Layout_RemoveAll
#define ZM_List_SetScrollPos            ZM_Layout_SetScrollPos
#define ZM_List_GetItemAt               ZM_Layout_GetItemAt
#define ZM_List_GetVerticalScrollBar    ZM_Layout_GetVerticalScrollBar
#define ZM_List_GetHorizontalScrollBar  ZM_Layout_GetHorizontalScrollBar
#define ZM_List_EnableScrollBar         ZM_Layout_EnableScrollBar
#define ZM_ListBody_SetScrollPos        ZM_Layout_SetScrollPos
#define ZM_List_GetHeader               380    //取列表头控件
#define ZM_List_GetListInfo             381    //取列表属性
#define ZM_List_SelectItem              382     //选择表项
#define ZM_List_SelectMultiItem         383     //多项选择
#define ZM_List_SetMultiSelect          384     //置允许多项选择
#define ZM_List_IsMultiSelect           385     //是否允许多项选择
#define ZM_List_UnSelectItem            386
#define ZM_List_SelectAllItems          387     //选择全部行
#define ZM_List_UnSelectAllItems        388     //取消所有选择项
#define ZM_List_EnsureVisible           389     //定位滚动条
//ListHeaderItem 
#define ZM_ListHeaderItem_SetSepWidth       400
#define ZM_ListHeaderItem_SetNormalImage    401
#define ZM_ListHeaderItem_SetHotImage       402
#define ZM_ListHeaderItem_SetPushedImage    403
#define ZM_ListHeaderItem_SetFocusedImage   404
#define ZM_ListHeaderItem_SetSepImage       405
#define ZM_ListHeaderItem_GetThumbRect      406
#define ZM_ListHeaderItem_SetDragable       407
#define ZM_ListHeaderItem_EstimateSize      408
#define ZM_ListHeaderItem_SetColorNormal    409    //普通颜色
#define ZM_ListHeaderItem_SetColorHot       410    //高亮状态
#define ZM_ListHeaderItem_SetColorPushed    411    //按下状态
#define ZM_ListHeaderItem_SetColorFocused   412    //焦点颜色
#define ZM_ListHeaderItem_SetColorDisabled  413    //非激活状态
#define ZM_ListHeaderItem_SetTextStyle      414
#define ZM_ListHeaderItem_GetTextStyle      415
#define ZM_ListHeaderItem_SetListTextStyle  416
#define ZM_ListHeaderItem_GetListTextStyle  417
#define ZM_ListHeaderItem_SetFont           418
#define ZM_ListHeaderItem_SetListFont       419
#define ZM_ListHeaderItem_SetTextColor      420
#define ZM_ListHeaderItem_SetTextPadding    421
//ListBody
#define ZM_ListBody_SetOwner                430    //设置宿主
//ListElememt
#define ZM_ListElement_SetOwner             440    //设置宿主
#define ZM_ListElement_SetIndex             441    //
#define ZM_ListElement_GetIndex             442    //
#define ZM_ListElement_Select               443    //选中
#define ZM_ListElement_SelectMulti          444    //多选
#endif // 1

//--------------------------------------------------------------------Window类
#if 1
#define ZC_Window                 _T("Window")
#define ZM_Window_SetNoBox        450     //设置为无边框窗体
#define ZM_Window_SetWindowMin    451     //
#define ZM_Window_SetWindowMax    452
#define ZM_Window_SetWindowRestor 453
#define ZM_Window_SetMinInfo      454
#define ZM_Window_SetMaxInfo      455
#define ZM_Window_SetSize         456
#define ZM_Window_SetComBo        457
#define ZM_Window_Popup           458
#define ZM_Window_SetToolWindow   459
#define ZM_Window_Center          460
#endif // 1

//--------------------------------------------------------------------Option类
#if 1
#define ZC_Option                         _T("Option")
#define ZM_Option_SetSelected             470    //
#define ZM_Option_GetSelected             471    //
#define ZM_Option_SetResNormal            472    //选中的普通状态
#define ZM_Option_SetResHot               473
#define ZM_Option_SetResPushed            474
#define ZM_Option_SetResFocused           475
#define ZM_Option_SetResDisabled          476
#define ZM_Option_SetColorNormal          477
#define ZM_Option_SetColorHot             478
#define ZM_Option_SetColorPushed          479
#define ZM_Option_SetColorFocused         480
#define ZM_Option_SetColorDisabled        481
#define ZM_Option_SetGroup                482    //设置到组
#define ZM_Option_SetResType              483
#define ZM_Option_SetResSize              484
#endif // 1

//--------------------------------------------------------------------ScrollBar类
#if 1
#define ZC_ScrollBar                  _T("ScrollBar")
#define ZM_ScrollBar_SetHorizontal    490 //设置为横向滚动条
#define ZM_ScrollBar_SetScrollPos     491 //设置位置
#define ZM_ScrollBar_GetScrollPos     492 //获取位置
#define ZM_ScrollBar_GetScrollRange   493
#define ZM_ScrollBar_SetOwner         494
#define ZM_ScrollBar_SetScrollRange   495
#define ZM_ScrollBar_SetColor         496
#define ZM_ScrollBar_bShow            497
#define ZM_ScrollBar_SetImageRes      498
#define ZM_ScrollBar_tN_Color         499 //滑块普通颜色
#define ZM_ScrollBar_tH_Color         500
#define ZM_ScrollBar_tP_Color         501
#define ZM_ScrollBar_bN_Color         502 //按钮普通颜色
#define ZM_ScrollBar_bH_Color         503
#define ZM_ScrollBar_bP_Color         504
#define ZM_ScrollBar_Di_Color         505 //禁用时颜色
#define ZM_ScrollBar_BK_Color         506 //背景色
#define ZM_ScrollBar_B1_Show          507 //显示方向块
#define ZM_ScrollBar_B2_Show          508

#endif //1
//--------------------------------------------------------------------TreeView类
#if 1
#define ZC_TreeView                       _T("TreeView")
#define ZC_TreeNode                       _T("TreeNode")
#define ZM_TreeView_Add                   ZM_Layout_Add  
#define ZM_TreeView_AddAt                 ZM_Layout_AddAt
#define ZM_TreeView_Remove                ZM_Layout_Remove
#define ZM_TreeView_RemoveAt              ZM_Layout_RemoveAt
#define ZM_TreeView_RemoveAll             ZM_Layout_RemoveAll
#define ZM_TreeNode_Add                   ZM_Layout_Add  
#define ZM_TreeNode_AddAt                 ZM_Layout_AddAt
#define ZM_TreeNode_Remove                ZM_Layout_Remove
#define ZM_TreeView_SetItemExpand         510    //展开关闭
#define ZM_TreeView_SetItemCheckBox       511    //选中反选
#define ZM_TreeNode_GetCountChild         512
#define ZM_TreeNode_GetChildNode          513
#define ZM_TreeNode_SetTreeView           514
#define ZM_TreeNode_AddChildNode          515
#define ZM_TreeNode_CalLocation           516    //计算缩进
#define ZM_TreeNode_GetLastNode           517
#define ZM_TreeNode_GetTreeIndex          518    //取得全局树视图的索引
#define ZM_TreeNode_GetDottedLine         519
#define ZM_TreeNode_SetParentNode         520    //设置父节点
#define ZM_TreeNode_GetItemButton         521
#define ZM_TreeNode_IsHasChild            522    //是否有子节点
#define ZM_TreeNode_SetVisibleFolderBtn   523
#define ZM_TreeNode_GetVisibleFolderBtn   524
#define ZM_TreeNode_SetVisibleCheckBtn    525
#define ZM_TreeNode_GetVisibleCheckBtn    526
#define ZM_TreeNode_GetFolderButton       527
#define ZM_TreeNode_GetCheckBox           528
#endif // 1

//--------------------------------------------------------------------Edit类
#define ZC_Edit                          _T("Edit")
#define ZM_Edit_SetFont                  550     //设置字体
#define ZM_Edit_SetTextColor             551     //设置文本颜色
#define ZM_Edit_SetTextPadding           552     //字体边距
#define ZM_Edit_SetTextColorDisabled     553
#define ZM_Edit_GetTextStyle             554
#define ZM_Edit_SetTextStyle             555
//--------------------------------------------------------------------内部函数导出表
//zui引擎
#define ZuiVersion                     0x00010001
typedef struct _ZuiFuncs {
    int size;      //结构大小
    int version;   //结构版本

    int(ZCALL *ZuiMsgLoop)();
    ZuiVoid(ZCALL *ZuiMsgLoop_exit)(int nRet);
    ZuiControl(ZCALL *NewZuiControl)(ZuiText classname, ZuiAny Param1, ZuiAny Param2);
    ZuiVoid(ZCALL *FreeZuiControl)(ZuiControl p, ZuiBool Delayed);
    ZuiAny(ZCALL *ZuiControlCall)(int ProcId, ZuiControl p, ZuiAny Param1, ZuiAny Param2);

}ZuiFuncs;

//--------------------------------------------------------------------导出函数
#if defined(__cplusplus)
extern "C"
{
#endif
    //初始化
    ZEXPORT ZuiBool ZCALL ZuiInit(ZuiInitConfig config);
    //反初始化
    ZEXPORT ZuiBool ZCALL ZuiUnInit();
    //Zui消息循环.
    ZEXPORT int ZCALL ZuiMsgLoop();
    //退出Zui消息循环.
    ZEXPORT ZuiVoid ZCALL ZuiMsgLoop_exit(int nRet);
    
    ZEXPORT ZuiControl ZCALL NewZuiControl(ZuiText classname, ZuiAny Param1, ZuiAny Param2);//创建控件
    ZEXPORT ZuiControl ZCALL NewZuiControlFromXml(ZuiControl ParentControl, ZuiText resname, ZNotifyProc ctrlproc);
    ZEXPORT ZuiVoid ZCALL FreeZuiControl(ZuiControl p, ZuiBool Delayed);//销毁控件
    ZEXPORT ZuiAny ZCALL ZuiControlCall(int ProcId, ZuiControl p, ZuiAny Param1, ZuiAny Param2);//调用控件处理函数
    ZEXPORT ZuiControl ZCALL ZuiControlFindName(ZuiControl p, ZuiText Name);
    ZEXPORT ZuiVoid ZCALL ZuiControlRegNotify(ZuiControl p, ZNotifyProc pNotify);
    ZEXPORT int ZCALL ZuiMsgBox(ZuiControl ParentControl, ZuiText text, ZuiText title);
    ZEXPORT int ZCALL ZuiDialogBox(ZuiControl ParentControl, ZuiText resname, ZNotifyProc dialogproc, ZuiBool model);
    ZEXPORT ZuiVoid ZCALL ZuiLoadXml(ZuiText resname);

    //载入布局窗口
    ZEXPORT ZuiControl ZCALL ZuiLayoutLoad(ZuiControl ParentControl,ZuiAny xml, int len);

    //资源包
    ZEXPORT ZuiResDB ZCALL ZuiResDBCreateFromBuf(ZuiAny data, int len, ZuiText Pass);
    ZEXPORT ZuiResDB ZCALL ZuiResDBCreateFromFile(ZuiText Path, ZuiText Pass);
    ZEXPORT ZuiVoid ZCALL ZuiResDBDestroy(ZuiResDB db);
    //资源
    ZEXPORT ZuiRes ZCALL ZuiResDBNewTempRes(ZuiAny b, int buflen, int type);
    ZEXPORT ZuiRes ZCALL ZuiResDBGetRes(ZuiText Path, int type);//获取一个资源
    ZEXPORT ZuiVoid ZCALL ZuiResDBDelRes(ZuiRes res);//释放一个资源
    ZEXPORT ZuiAny ZCALL ZuiResGetData(ZuiRes res,int *plen);//获取资源中的数据
#ifdef PLATFORM_OS_WIN
    ZEXPORT ZuiBool ZCALL ZuiResDBAddPE(ZuiText name, ZuiAny hInstance);//添加一个PE文件到资源池
#endif // PLATFORM_OS_WIN


#if defined(__cplusplus)
}
#endif

#endif
