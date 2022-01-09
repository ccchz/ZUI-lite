#ifndef __ZUI_PLATFORM_H__
#define __ZUI_PLATFORM_H__
/*
* 平台移植层头文件
*/
#include <config.h>


#if defined PLATFORM_OS_WIN
#include "os/win.h"
//窗口样式
#define Z_APPWINDOW					 WS_OVERLAPPEDWINDOW | WS_VISIBLE
#define Z_POPWINDOW					 WS_POPUPWINDOW | WS_VISIBLE

//字体对齐格式
#define ZDT_TOP                  DT_TOP                   //顶对齐
#define ZDT_LEFT                 DT_LEFT                  //左对齐
#define ZDT_CENTER               DT_CENTER                //右对齐
#define ZDT_RIGHT                DT_RIGHT                 //左对齐
#define ZDT_VCENTER              DT_VCENTER               //纵向居中
#define ZDT_BOTTOM               DT_BOTTOM                //底对齐
#define ZDT_WORDBREAK            DT_WORDBREAK             
#define ZDT_SINGLELINE           DT_SINGLELINE            //单行
#define ZDT_EXPANDTABS           DT_EXPANDTABS        
#define ZDT_TABSTOP              DT_TABSTOP           
#define ZDT_NOCLIP               DT_NOCLIP            
#define ZDT_EXTERNALLEADING      DT_EXTERNALLEADING   
#define ZDT_CALCRECT             DT_CALCRECT          
#define ZDT_NOPREFIX             DT_NOPREFIX          
#define ZDT_INTERNAL             DT_INTERNAL          
#define ZDT_EDITCONTROL          DT_EDITCONTROL       
#define ZDT_PATH_ELLIPSIS        DT_PATH_ELLIPSIS     
#define ZDT_END_ELLIPSIS         DT_END_ELLIPSIS      
#define ZDT_MODIFYSTRING         DT_MODIFYSTRING      
#define ZDT_RTLREADING           DT_RTLREADING        
#define ZDT_WORD_ELLIPSIS        DT_WORD_ELLIPSIS  

//系统鼠标样式
#define ZIDC_ARROW      IDC_ARROW          
#define ZIDC_IBEAM      IDC_IBEAM        
#define ZIDC_WAIT       IDC_WAIT         
#define ZIDC_CROSS      IDC_CROSS        
#define ZIDC_UPARROW    IDC_UPARROW      
#define ZIDC_SIZE       IDC_SIZE           
#define ZIDC_ICON       IDC_ICON           
#define ZIDC_SIZENWSE   IDC_SIZENWSE     
#define ZIDC_SIZENESW   IDC_SIZENESW     
#define ZIDC_SIZEWE     IDC_SIZEWE       
#define ZIDC_SIZENS     IDC_SIZENS       
#define ZIDC_SIZEALL    IDC_SIZEALL      
#define ZIDC_NO         IDC_NO            
#if(WINVER >= 0x0500)
#define ZIDC_HAND         IDC_HAND           
#endif /* WINVER >= 0x0500 */
#define ZIDC_APPSTARTING  IDC_APPSTARTING   
#if(WINVER >= 0x0400)
#define ZIDC_HELP         IDC_HELP    
#endif /* WINVER >= 0x0400 */

#if(WINVER >= 0x0606)
#define ZIDC_PIN          IDC_PIN     
#define ZIDC_PERSON       IDC_PERSON   
#endif /* WINVER >= 0x0606 */
#elif (defined PLATFORM_OS_LINUX)
#include "os/linux.h"
#elif (defined PLATFORM_OS_MACX)
#include "os/macx.h"
#elif (defined PLATFORM_OS_ANDROID)
#include "os/android.h"
#endif


#if (defined PLATFORM_GRAPH_GDIX)&&(PLATFORM_GRAPH_GDIX == 1)
    #include "graph/gdi+.h"
#endif

/*移植层接口定义头文件*/
#include "graph.h"
#include "os.h"


#endif //__ZUI_PLATFORM_H__