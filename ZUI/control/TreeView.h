#ifndef __TREE_H__
#define __TREE_H__
#include <ZUI.h>
#include <core/carray.h>
/**树形框结构*/
typedef struct _ZTreeNode
{
    long	m_iTreeLavel;
    ZuiBool	m_bIsVisable;
    ZuiBool	m_bIsCheckBox;
    ZuiColor	m_dwItemTextColor;
    ZuiColor	m_dwItemHotTextColor;
    ZuiColor	m_dwSelItemTextColor;
    ZuiColor	m_dwSelItemHotTextColor;

    ZuiControl          pTreeView;          //所属树形框
    ZuiControl          pHoriz;
    ZuiControl          pFolderButton;
    ZuiControl          pDottedLine;
    ZuiControl          pCheckBox;
    ZuiControl          pItemButton;

    ZuiControl          pParentTreeNode;    //父节点

    DArray              *mTreeNodes;        //子节点数组

    ZCtlProc old_call;
    ZVoid old_udata;
}*ZuiTreeNode, ZTreeNode;
/**树形框结构*/
typedef struct _ZTreeView
{
    unsigned int m_uItemMinWidth;
    ZuiBool m_bVisibleFolderBtn;
    ZuiBool m_bVisibleCheckBtn;

    ZCtlProc old_call;
    ZVoid old_udata;
}*ZuiTreeView, ZTreeView;
ZEXPORT ZINT ZCALL ZuiTreeViewProc(ZINT ProcId, ZuiControl cp, ZuiTreeView p, ZPARAM Param1, ZPARAM Param2);
ZEXPORT ZINT ZCALL ZuiTreeNodeProc(ZINT ProcId, ZuiControl cp, ZuiTreeNode p, ZPARAM Param1, ZPARAM Param2);

#endif	//__TREE_H__
