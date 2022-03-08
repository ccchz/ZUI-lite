#ifndef __LIST_H__
#define __LIST_H__
#include <ZUI.h>
#include <core/carray.h>
/**列表头元素结构*/
typedef struct _ZListHeaderItem
{
    ZPoint ptLastMouse;
    ZuiBool m_bDragable;
    unsigned int m_uButtonState;     //按钮状态
    int m_iSepWidth;         //分隔符区域


    ZuiRes m_ResNormal;          //通常图片
    ZuiRes m_ResHot;             //点燃图片
    ZuiRes m_ResPushed;          //按下图片
    ZuiRes m_ResFocused;         //焦点图片
    ZuiRes m_ResSep;
	ZuiColor m_cColorNormal;     //正常状态
	ZuiColor m_cColorHot;        //高亮状态
	ZuiColor m_cColorPushed;     //按下状态
	ZuiColor m_cColorFocused;    //焦点图片
	ZuiColor m_cColorSep;

    ZuiRes      m_rFont;         //字体
	ZuiRes		m_rListFont;
    unsigned int     m_uTextStyle;    //字体控制
	unsigned int		m_uListTextStyle;//ListElement字体控制
    ZuiColor    m_cTextColor;   //字体颜色
	ZRect		m_rcPadding;

    ZRect m_rcThumb;
    ZVoid old_udata;
    ZCtlProc old_call;
}*ZuiListHeaderItem, ZListHeaderItem;

/**列表头结构*/
typedef struct _ZListHeader
{
    ZSize m_szXY;  //表头大小
    ZVoid old_udata;
    ZCtlProc old_call;
}*ZuiListHeader, ZListHeader;

/**列表体结构*/
typedef struct _ZListBody
{
    ZuiControl m_pOwner;//宿主列表
    ZVoid old_udata;
    ZCtlProc old_call;
}*ZuiListBody, ZListBody;

/**列表元素结构*/
typedef struct _ZListElement
{
    int m_iIndex;        //列索引
    ZuiBool m_bSelected;    //选中
    unsigned int m_uButtonState;    //列状态
    ZuiControl m_pOwner;    //宿主

    ZVoid old_udata;
    ZCtlProc old_call;
}*ZuiListElement, ZListElement;

/**列表属性结构*/
#define ZLIST_MAX_COLUMNS 32
typedef struct _ZListInfo
{
    int m_iColumns;//列数量
    ZRect m_rcColumn[ZLIST_MAX_COLUMNS];

    ZuiBool m_bAlternateBk;       //使用交替背景色
    ZuiColor m_cLineColor;          //行间线颜色
	ZuiColor m_cColumnColor;			//列间线颜色
    ZuiColor m_cBkColor;            //背景色
    ZuiColor m_cHotBkColor;         //点燃背景色
    ZuiColor m_cSelectedBkColor;    //选中背景色
    ZuiColor m_cDisabledBkColor;    //禁用背景色

    ZuiBool m_bShowRowLine;       //显示行线
    ZuiBool m_bShowColumnLine;    //显示列线
} *ZuiListInfo, ZListInfo;

/**列表控件结构*/
typedef struct _ZList
{
    ZuiBool m_bScrollSelect;
    int m_iCurSel;           //现行选中项
    DArray* m_aSelItems;        //选中项数组
    ZuiBool m_bMultiSel;        //允许多选
    int m_iExpandedItem;

    ZuiControl m_pList;     //列表体容器
    ZuiControl m_pHeader;   //表头容器

    ZListInfo m_ListInfo;   //列表属性

    ZVoid old_udata;
    ZCtlProc old_call;
}*ZuiList, ZList;
ZEXPORT ZINT ZCALL ZuiListProc(ZINT ProcId, ZuiControl cp, ZuiList p, ZPARAM Param1, ZPARAM Param2);
ZEXPORT ZINT ZCALL ZuiListBodyProc(ZINT ProcId, ZuiControl cp, ZuiListBody p, ZPARAM Param1, ZPARAM Param2);
ZEXPORT ZINT ZCALL ZuiListElementProc(ZINT ProcId, ZuiControl cp, ZuiListElement p, ZPARAM Param1, ZPARAM Param2);
ZEXPORT ZINT ZCALL ZuiListHeaderProc(ZINT ProcId, ZuiControl cp, ZuiListHeader p, ZPARAM Param1, ZPARAM Param2);
ZEXPORT ZINT ZCALL ZuiListHeaderItemProc(ZINT ProcId, ZuiControl cp, ZuiListHeaderItem p, ZPARAM Param1, ZPARAM Param2);


#endif  //__LIST_H__
