#ifndef __CheckBox_H__
#define __CheckBox_H__
#include <ZUI.h>

/**Check控件结构*/
typedef struct _ZCheckBox
{
    ZCtlProc old_call;
}*ZuiCheckBox, ZCheckBox;
ZEXPORT ZINT ZCALL ZuiCheckBoxProc(ZINT ProcId, ZuiControl cp, ZuiCheckBox p, ZPARAM Param1, ZPARAM Param2);


#endif	//__CheckBox_H__
