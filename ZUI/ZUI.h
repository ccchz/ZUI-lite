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
#define MAX max
#define MIN min
#define CLAMP(x,a,b) (MIN(b,MAX(a,x)))
#ifndef ASSERT
#define ASSERT(expr)  _ASSERTE(expr)
#endif

#define MAKEPARAM(a, b)  ((b & 0xFFFFFFFF) << 16) | (a & 0xFFFF)
#define HPARAM(a)   (a & 0xFFFFFFFF) >> 16
#define LPARAM(a)   a & 0xFFFF
//--------------------------------------------------------------------基础数据类型
typedef wchar_t* ZuiText;
typedef wchar_t _ZuiText, ZText;   //内核默认Unicode存储字符
typedef float           ZuiReal;
typedef int             ZuiBool;
typedef void            ZuiVoid;
typedef void*           ZuiAny;
typedef uint32_t        ZuiColor;
typedef unsigned        ZuiByte;
/**矩形*/
typedef struct _ZRect
{
    int left;   ///左边
    int top;    ///顶边
    int right;  ///右边
    int bottom; ///底边
} *ZuiRect, ZRect;
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
    ZuiText default_fontname;   //默认字体名字

} *ZuiInitConfig, ZInitConfig;

//--------------------------------------------------------------------回调定义
typedef ZuiControl(ZCALL* FINDCONTROLPROC)(ZuiControl, ZuiAny);
typedef ZuiAny(ZCALL *ZCtlProc)(int ProcId, ZuiControl p, ZuiAny UserData, ZuiAny Param1, ZuiAny Param2);
typedef ZuiAny(ZCALL *ZNotifyProc)(ZuiText msg, ZuiControl p, ZuiAny UserData, ZuiAny Param1, ZuiAny Param2);

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
#define ZLOG_TYPE_WARNING     2
#define ZLOG_TYPE_INFO     3

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
#define Type_Null               _T("Null")

#define ZTYLE_BOX               1   //单线边框
#define ZTYLE_BKGColor          2   //具有背景色
//-----控件事件
#define ZM_CoreInit           1   //用于控件内核初始化
#define ZM_CoreUnInit         2   //用于控件内核反初始化
#define ZM_GetType            3   //取控件类型
#define ZM_GetObject          4  //获取控件被继承后某一类型的对象
#define ZM_OnCreate           5   //内核创建
#define ZM_OnInit             6   //用户初始化
#define ZM_OnDestroy          7   //销毁
#define ZM_OnSize             8   //
#define ZM_OnEvent            9   //
#define ZM_OnNotify           10   //
#define ZM_OnPaint            11  // 绘制循序：背景颜色->背景图->状态图->文本->边框
#define ZM_EndPaint           12  //子控件绘制完毕后调用
#define ZM_OnPaintBkColor     13  //背景色
#define ZM_OnPaintBkImage     14  //背景图片
#define ZM_OnPaintStatusImage 15  //状态图片
#define ZM_OnPaintText        16  //文本
#define ZM_OnPaintBorder      17  //边框
#define ZM_OnPostPaint        18  //

#define ZM_Invalidate         19  //刷新显示
#define ZM_FindControl        20  //查找控件

//----控件属性
#define ZM_SetAttribute       25  //解析属性
#define ZM_GetAttribute       26  //取属性
#define ZM_GetControlFlags    27  //
#define ZM_Activate           28  //活动
#define ZM_SetVisible         29  //设置是否可视
#define ZM_GetVisible         30  //是否可视
#define ZM_SetText            31  //设置文本
#define ZM_GetText            32  //设置文本
#define ZM_SetName            33  //设置名字
#define ZM_SetTooltip         34  //设置提示文本
#define ZM_GetPos             35  //得到控件位置
#define ZM_SetPos             36  //设置控件位置并重绘
#define ZM_SetOs              37  //设置控件的绘制管理者
#define ZM_SetAnimation       38  //设置动画类型
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
#define ZM_SetFocus           65  //设置焦点
#define ZM_SetDrag            66  //设置拖拽控件

//-------绘图资源
#define ZM_SetBkColor           67  //设置背景色
#define ZM_SetBkImage         68  //设置背景图片
#define ZM_SetBorderColor     69  //设置边框颜色

#define ZM_SetAnimationType   70  //设置动画类型
#define	ZM_SetBorderWidth		71
#define	ZM_GetBorderWidth		72
#define ZM_OnClose			73
#define ZM_GetEnabled         74
#define ZM_SetRound           75  //设置圆角
//------属性名称
#define BK_Color        0x00000001 //背景色
#define Border_Color    0x00000002 //边框颜色

#define ZuiControlSetDrag(p,b)  ZuiControlCall(ZM_SetDrag,(p),(b),NULL,NULL)
#define ZuiControlSetFixedHeight(p,h) ZuiControlCall(ZM_SetFixedHeight,(p),(h),NULL,NULL)
#define ZCCALL(I,C,P1,P2) ZuiControlCall((I),(C),(P1),(P2))
#endif // 1

//--------------------------------------------------------------------Layout类
#if 1
#define Type_Layout                     _T("Layout")
#define Type_VerticalLayout             _T("VerticalLayout")
#define Type_HorizontalLayout           _T("HorizontalLayout")
#define Type_TileLayout                 _T("TileLayout")
#define Type_TabLayout                  _T("TabLayout")

//--------base
#define ZM_Layout_Add                 101 ///添加控件
#define ZM_Layout_AddAt               102 ///插入控件
#define ZM_Layout_Remove              103 ///移除指定控件, 参数1为真由调用方销毁成员
#define ZM_Layout_RemoveAt            104 ///移除指定位置控件
#define ZM_Layout_RemoveAll           105 ///移除全部控件
#define ZM_Layout_GetCount            106 ///取容器控件数量
#define ZM_Layout_GetItemIndex        107 ///取指定控件索引
#define ZM_Layout_SetItemIndex        108 ///设置控件所在位置
#define ZM_Layout_GetItemAt           109 ///索引取控件
#define ZM_Layout_SetFloatPos         110 ///设置浮动控件位置
#define ZM_Layout_SetInset            112 ///设置内边距

//--------滚动条（由Layout管理）
#define ZM_Layout_GetScrollPos        113
#define ZM_Layout_GetScrollRange      114
#define ZM_Layout_SetScrollPos        115
#define ZM_Layout_SetScrollStepSize   116
#define ZM_Layout_GetScrollStepSize   117
#define ZM_Layout_LineUp              118
#define ZM_Layout_LineDown            119
#define ZM_Layout_PageUp              120
#define ZM_Layout_PageDown            121
#define ZM_Layout_HomeUp              122
#define ZM_Layout_EndDown             123
#define ZM_Layout_LineLeft            124
#define ZM_Layout_LineRight           125
#define ZM_Layout_PageLeft            126
#define ZM_Layout_PageRight           127
#define ZM_Layout_HomeLeft            128
#define ZM_Layout_EndRight            129
#define ZM_Layout_EnableScrollBar     130
#define ZM_Layout_GetVerticalScrollBar    131
#define ZM_Layout_GetHorizontalScrollBar  132
#define ZM_Layout_ProcessScrollBar    133 //调整滚动条位置
#define ZM_Layout_GetChildAlign       134
#define ZM_Layout_GetChildVAlign      135

#define ZuiLayoutAdd(p,cp) ZuiControlCall(ZM_Layout_Add,(p),(cp),NULL,NULL)

//--------Tile
#define ZM_TileLayout_SetColumns      150
#define ZM_TileLayout_SetItemSize     151

//--------Tab
#define ZM_TabLayout_SelectItem       161    //当前Tab

#endif // 1

//--------------------------------------------------------------------Label类
#if 1
#define Type_Label                  _T("Label")
#define ZM_Label_SetFont          171     //设置字体
#define ZM_Label_SetTextColor     172     //设置文本颜色
#define ZM_Label_SetTextPadding   173     //字体边距
#define ZM_Label_SetTextColorDisabled     174
#endif // 1

//--------------------------------------------------------------------Button类
#if 1
#define Type_Button                 _T("Button")

#define ZM_Button_SetResNormal    201    //普通颜色
#define ZM_Button_SetResHot       202    //高亮状态
#define ZM_Button_SetResPushed    203    //按下状态
#define ZM_Button_SetResFocused   204    //焦点图片
#define ZM_Button_SetResDisabled  205    //非激活状态

#define ZM_Button_SetColorNormal    206    //普通颜色
#define ZM_Button_SetColorHot       207    //高亮状态
#define ZM_Button_SetColorPushed    208    //按下状态
#define ZM_Button_SetColorFocused   209    //焦点图片
#define ZM_Button_SetColorDisabled  210    //非激活状态
#define ZM_Button_SetBorderColorHot    211     //激活状态下边框颜色
#endif // 1

//--------------------------------------------------------------------类
#if 1

#endif // 1
//--------------------------------------------------------------------DrawPanel类
#if 1
#define Type_DrawPanel            _T("DrawPanel")
#endif // 1
//--------------------------------------------------------------------SplitterBar类
#if 1
#define Type_SplitterBar            _T("SplitterBar")
#endif // 1
//--------------------------------------------------------------------ProgressBar类
#if 1
#define Type_ProgressBar            _T("ProgressBar")
#define ZM_ProgressBar_SetColor   500
#define ZM_ProgressBar_SetBackColor   501
#define ZM_ProgressBar_SetPos   502
#define ZM_ProgressBar_SetRange   503
#define ZM_ProgressBar_SetHeight  504
#endif

//--------------------------------------------------------------------CheckBox类
#if 1
#define Type_CheckBox               _T("CheckBox")
#endif // 1
//--------------------------------------------------------------------List类
#if 1
#define Type_List                           _T("List")
#define Type_ListBody                       _T("ListBody")
#define Type_ListElement                    _T("listElement")
#define Type_ListHeader                     _T("listHeader")
#define Type_ListHeaderItem                 _T("listHeaderItem")


#define ZM_List_GetHeader                 400    //取列表头控件
#define ZM_List_GetListInfo               401    //取列表属性
#define ZM_List_Add                       ZM_Layout_Add
#define ZM_List_GetCount                  ZM_Layout_GetCount
#define ZM_List_AddAt                     ZM_Layout_AddAt
#define ZM_List_Remove                    ZM_Layout_Remove
#define ZM_List_RemoveAt                  ZM_Layout_RemoveAt
#define ZM_List_RemoveAll                 ZM_Layout_RemoveAll
#define ZM_List_SetScrollPos              ZM_Layout_SetScrollPos
#define ZM_List_GetItemAt                 ZM_Layout_GetItemAt
#define ZM_List_SelectItem                402     //选择表项
#define ZM_List_SelectMultiItem           403     //多项选择
#define ZM_List_SetMultiSelect            404     //置允许多项选择
#define ZM_List_IsMultiSelect             405     //是否允许多项选择
#define ZM_List_UnSelectItem              406
#define ZM_List_SelectAllItems            407     //选择全部行
#define ZM_List_UnSelectAllItems          408     //取消所有选择项
#define ZM_List_EnsureVisible             409     //定位滚动条


#define ZM_List_GetVerticalScrollBar      ZM_Layout_GetVerticalScrollBar
#define ZM_List_GetHorizontalScrollBar    ZM_Layout_GetHorizontalScrollBar
#define ZM_List_EnableScrollBar           ZM_Layout_EnableScrollBar

#define ZM_ListHeaderItem_SetSepWidth     400
#define ZM_ListHeaderItem_SetNormalImage  401
#define ZM_ListHeaderItem_SetHotImage     402
#define ZM_ListHeaderItem_SetPushedImage  403
#define ZM_ListHeaderItem_SetFocusedImage 404
#define ZM_ListHeaderItem_SetSepImage     405
#define ZM_ListHeaderItem_GetThumbRect    406
#define ZM_ListHeaderItem_SetDragable     407
#define ZM_ListHeaderItem_EstimateSize    ZM_EstimateSize

#define ZM_ListBody_SetOwner              400    //设置宿主
#define ZM_ListBody_SetScrollPos          ZM_Layout_SetScrollPos

#define ZM_ListElement_SetOwner           400    //设置宿主
#define ZM_ListElement_SetIndex           401    //
#define ZM_ListElement_GetIndex           402    //
#define ZM_ListElement_Select             403     //选中
#define ZM_ListElement_SelectMulti        404     //多选
#endif // 1

//--------------------------------------------------------------------Window类
#if 1
#define Type_Window                 _T("Window")

#define ZM_Window_SetNoBox        1001    //设置为无边框窗体
#define ZM_Window_SetWindowMin    1002    //
#define ZM_Window_SetWindowMax    1003
#define ZM_Window_SetWindowRestor 1004
#define ZM_Window_SetMinInfo      1005
#define ZM_Window_SetMaxInfo      1006
#define ZM_Window_SetSize         1007
#define ZM_Window_SetComBo        1008
#define ZM_Window_Popup           1009
#define ZM_Window_SetToolWindow   1010
#define ZM_Window_Center          1011

#endif // 1

//--------------------------------------------------------------------Option类
#if 1
#define Type_Option                         _T("Option")
#define ZM_Option_SetSelected             1031    //
#define ZM_Option_GetSelected             1032    //

#define ZM_Option_SetResNormal            1033    //选中的普通状态
#define ZM_Option_SetResHot               1034
#define ZM_Option_SetResPushed            1035
#define ZM_Option_SetResFocused           1036
#define ZM_Option_SetResDisabled          1037

#define ZM_Option_SetColorNormal          1038
#define ZM_Option_SetColorHot             1039
#define ZM_Option_SetColorPushed          1040
#define ZM_Option_SetColorFocused         1041
#define ZM_Option_SetColorDisabled        1042

#define ZM_Option_SetGroup                1043    //设置到组
#endif // 1

//--------------------------------------------------------------------ScrollBar类
#if 1
#define Type_ScrollBar                  _T("ScrollBar")
#define ZM_ScrollBar_SetHorizontal    1050 //设置为横向滚动条
#define ZM_ScrollBar_SetScrollPos     1051 //设置位置
#define ZM_ScrollBar_GetScrollPos     1052 //获取位置
#define ZM_ScrollBar_GetScrollRange   1053
#define ZM_ScrollBar_SetOwner         1054
#define ZM_ScrollBar_SetScrollRange   1055
#define ZM_ScrollBar_SetColor         1056
#define ZM_ScrollBar_bShow            1057
//------属性名称
#define ScrollBar_tN_Color        0x00000001 //滑块普通颜色
#define ScrollBar_tH_Color        0x00000002
#define ScrollBar_tP_Color        0x00000004
#define ScrollBar_bN_Color        0x00000008 //按钮普通颜色
#define ScrollBar_bH_Color        0x00000010
#define ScrollBar_bP_Color        0x00000020
#define ScrollBar_Di_Color        0x00000040 //禁用时颜色
#define ScrollBar_BK_Color        0x00000080 //背景色
#define ScrollBar_B1_Show         0x00000001
#define ScrollBar_B2_Show         0x00000002

#endif //1
//--------------------------------------------------------------------TreeView类
#if 1
#define Type_TreeView                   _T("TreeView")
#define Type_TreeNode                   _T("TreeNode")

#define ZM_TreeView_Add                   ZM_Layout_Add  
#define ZM_TreeView_AddAt                 ZM_Layout_AddAt
#define ZM_TreeView_Remove                ZM_Layout_Remove
#define ZM_TreeView_RemoveAt              ZM_Layout_RemoveAt
#define ZM_TreeView_RemoveAll             ZM_Layout_RemoveAll
#define ZM_TreeView_SetItemExpand         1100    //展开关闭
#define ZM_TreeView_SetItemCheckBox       1101    //选中反选

#define ZM_TreeNode_GetCountChild         1100
#define ZM_TreeNode_GetChildNode          1101
#define ZM_TreeNode_SetTreeView           1102
#define ZM_TreeNode_Add                   ZM_Layout_Add  
#define ZM_TreeNode_AddAt                 ZM_Layout_AddAt
#define ZM_TreeNode_Remove                ZM_Layout_Remove
#define ZM_TreeNode_AddChildNode          1103
#define ZM_TreeNode_CalLocation           1104    //计算缩进
#define ZM_TreeNode_GetLastNode           1105
#define ZM_TreeNode_GetTreeIndex          1106    //取得全局树视图的索引
#define ZM_TreeNode_GetDottedLine         1107
#define ZM_TreeNode_SetParentNode         1108    //设置父节点
#define ZM_TreeNode_GetItemButton         1109
#define ZM_TreeNode_IsHasChild            1110    //是否有子节点
#define ZM_TreeNode_SetVisibleFolderBtn   1111
#define ZM_TreeNode_GetVisibleFolderBtn   1112
#define ZM_TreeNode_SetVisibleCheckBtn    1113
#define ZM_TreeNode_GetVisibleCheckBtn    1114
#define ZM_TreeNode_GetFolderButton       1115
#define ZM_TreeNode_GetCheckBox           1116
#endif // 1
//--------------------------------------------------------------------内部函数导出表
//zui引擎
#define ZuiFuncsVersion                     0x00010001
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
    ZEXPORT ZuiVoid ZCALL FreeZuiControl(ZuiControl p, ZuiBool Delayed);//销毁控件
    ZEXPORT ZuiAny ZCALL ZuiControlCall(int ProcId, ZuiControl p, ZuiAny Param1, ZuiAny Param2);//调用控件处理函数
    ZEXPORT ZuiControl ZCALL ZuiControlFindName(ZuiControl p, ZuiText Name);
    ZEXPORT ZuiVoid ZCALL ZuiControlRegNotify(ZuiControl p, ZNotifyProc pNotify);
    ZEXPORT int ZCALL ZuiMsgBox(ZuiControl rp, ZuiText text, ZuiText title);

    //载入布局窗口
    ZEXPORT ZuiControl ZCALL ZuiLayoutLoad(ZuiAny xml, int len);

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
