#include "export.h"
#include <stdlib.h>
static ZuiFuncs *func;

ZuiBool ZuiInitZuiFunc() {
    ZuiFuncs *p = (ZuiFuncs *)malloc(sizeof(ZuiFuncs));
    p->size = sizeof(ZuiFuncs);
    p->version = ZuiVersion;
     
    p->ZuiMsgLoop = &ZuiMsgLoop;
    p->ZuiMsgLoop_exit = &ZuiMsgLoop_exit;
    p->NewZuiControl = &NewZuiControl;
    p->FreeZuiControl = &FreeZuiControl;
    p->ZuiControlCall = &ZuiControlCall;
    return 1;
}
