/*******************************************************************************
*Copyright(C),2012-2013,mingv150,All rights reserved
*FileName:mian.c
*Description:
*Author:
*Version:
*Date:
*******************************************************************************/

#define _MAIN_C_
#include <htc.h>
#include "../Common.h"
#include "../Model.h"
#include "../oopc.h"

#include "../drv/Drv_Hardware.h"
#include "../drv/Drv_UserInterface.h"
#include "../drv/Drv_PhaseChk.h"
#include "../drv/Drv_NTCSensor.h"
#include "../drv/Drv_Timer.h"
#include "App_fsm.h"

// 标记开机第一次温度采集是否完成
bool InitComplete = 0;

/*******************************************************************************
*Function: void main()
*Description: main function
*Input:none
*Output:none
*******************************************************************************/
void main()
{
    u8 StartDelayTmrID;
    InitComplete = 0;

    // 初始化I/O口
    Drv_Hw_IOInit();

    // 初始化定时器
    Drv_Hw_TmrInit();

    // 初始化温度NTC采集模块
    Drv_NTC_MesureInit();
    
    // 开机创建一个两秒的定时器响蜂鸣器
    Drv_Timer_Create(&StartDelayTmrID,2000,1,NULL);

    // 打开蜂鸣器
    App_fsm_Spkon(); 
    // 检查定时器是否时间到 
    while(!Drv_Timer_TimeOutChk(&StartDelayTmrID));
    // 关闭蜂鸣器
    App_fsm_Spkoff();
	
    // 开机判断EEPROM是否有效，有效则读取，无效则初始化
    App_Fsm_InitParam();

    // 关闭所有的LED
    Drv_UI_LedDis(0,UI_LEDALL);

    // 打开看门狗
    Drv_Hw_WdtInit();

    // 进入主循环
    while(1)
    {
        if(f_Hw_2ms)
        {
            // 每2ms进行一次按键扫描
            f_Hw_2ms = 0;
            Drv_UI_KeyScan();
        }
        
        if(f_Hw_32ms)
        {
            f_Hw_32ms = 0;
        }
        
        if(f_Hw_64ms)
        {
            f_Hw_64ms = 0;
        }
        
        if(f_Hw_128ms)
        {
            f_Hw_128ms = 0;

            // 每128ms进行错误事件扫描
            if(InitComplete)
             App_fsm_ErrScan();

            // 每128ms进行喂狗
             CLRWDT();
        }

        // 判断温度采集是否完成
        if(Drv_NTC_CalTemperature())
        {
            // 温度采集成功则取温度
            App_Fsm_GetTemperature();
            
            // 温度采集完成值置系统初始化完成标志
            InitComplete = TRUE;
        }      
        
        if(InitComplete)
        {
            // 进行系统状态处理
            App_fsm_StateDispose();
        }
    }
}
