/*******************************************************************************
*Copyright(C),2012-2013,mingv150,All rights reserved
*FileName:App_fsm.c
*Description: 系统状态处理
*Author:
*Version:
*Date:
*******************************************************************************/
#define _APP_FSM_C_
/*common*/
#include <htc.h>
#include "../Common.h"
#include "../Model.h"
#include "../oopc.h"

/*myself*/
#include "App_fsm.h"

/*mycall*/
#include "../drv/Drv_Hardware.h"
#include "../drv/Drv_Eep.h"
#include "../drv/Drv_UserInterface.h"
#include "../drv/Drv_NTCSensor.h"
#include "../drv/Drv_Timer.h"
#include "../drv/Drv_PhaseChk.h"
#include "../drv/Drv_Event.h"
#include "App_pid.h"



t_SetupParam st_Fsm_SetupParam = 
{
    500,     //HighTemTHR Default Value
    100,     //LowTemTHR Default Value							 
    400,     //ConstHighTHR Default Value
    -150,    //ConstLowTHR Default Value
    99,      //DiffHighTHR Default Value
    -99,     //DiffLowTHR Default Value  
    15,      //CompRunTHR Default Value
    -15,     //CompStopTHR Default Value   
    60,      //CompDlyTHR Default Value
    30,      //EngyLowTHR Default Value
    0,       //RoomTemCal Default Value
    0,       //WatTemCal Default Value
    0,       //Password Default Value
    {1,1,1,0,0,1,0,0,0}
};

// 控制參數定義並初始化
t_CtrParam st_Fsm_CtrParam = 
{
    FSM_TEMINVAILD,FSM_TEMINVAILD,
    500,
    200,25
};

u8 f_SoftwarePwrOn = FALSE;
static u8 EngyLow = 0;



/*******************************************************************************
*Function:
*Description: 蜂鸣器回调函数（被計時器調用）
*Input:
*Output:
*******************************************************************************/
void App_fsm_Spkcb(void)
{
    static bool level = 0;

    // 由开变为关，由关变为开，与上一个蜂鸣器状态相反
    level = ~level;
    SPKPINOUT(level);
}


/****************************************************************************
Function: Name
Description: 关闭所有LED
Input:
Output:
*****************************************************************************/
void App_fsm_AllLedOff(void)
{
    // 关闭所有LED灯显示
    Drv_UI_LedDis(0,UI_LEDALL);
}


/*******************************************************************************
*Function:
*Description: 关闭蜂鸣器
*Input:
*Output:
*******************************************************************************/
void App_fsm_Spkoff(void)
{
    SPKPINOUT(0);
}


/****************************************************************************
Function: Name
Description: 开启蜂鸣器
Input:
Output:
*****************************************************************************/
void App_fsm_Spkon(void)
{
    SPKPINOUT(1);
}

/*******************************************************************************
*Function:
*Description: 开关压缩机函数
*Input:
*Output:
*******************************************************************************/
void App_fsm_CompOnOff(u8 on)
{
    // 开关压缩机
    HW_COMPPINOUT(on);
    // 开关压缩机灯
    Drv_UI_LedDis(on,UI_LEDCOMP);
    // 关闭压缩机灯闪烁
    Drv_UI_LedDis(0,UI_LEDCOMPF);

    if(st_Fsm_SetupParam.OptBit.SwPumpwithCps)
    {
        // 如果有设置压缩机和Pump联动，则相应开关Pump
        HW_PUMPPINOUT(on);
        // 开关Pump灯
        Drv_UI_LedDis(on,UI_LEDPUMP);
    }
}


/*******************************************************************************
*Function:
*Description: 开关泵的函数
*Input:
*Output:
*******************************************************************************/
void App_fsm_PumpOnOff(u8 On)
{
    // 开关Pump
    HW_PUMPPINOUT(On);
    // 开关Pump灯
    Drv_UI_LedDis(On,UI_LEDPUMP);
}


/*******************************************************************************
*Function:
*Description: 判断并初始化EEPROM参数
*Input:
*Output:
*******************************************************************************/
void App_Fsm_InitParam(void)
{
    u8 MagicNum; 
    
    // 从EEPROM中固定地址取MagicNum
    Drv_Eep_GetParam(FSM_EEPMAGADDR,&MagicNum,1);
    
    if(MagicNum == FSM_EEPMAGICNUM)
    {
        // EEPROM中的参数有效，则从EEPROM中取参数
        Drv_Eep_GetParam(FSM_EEPSETUPADDR,(u8 *)&st_Fsm_SetupParam,sizeof(st_Fsm_SetupParam));
        Drv_Eep_GetParam(FSM_EEPDIFFADDR,(u8 *)&(st_Fsm_CtrParam.DiffTemAim),2);
        Drv_Eep_GetParam(FSM_EEPCONSTADDR,(u8 *)&(st_Fsm_CtrParam.ConstTemAim),2);
    }
    else
    {
        // EEPROM参数无效，则重新初始化EEPROM参数
        MagicNum = FSM_EEPMAGICNUM;

        //重新初始化预定参数 
        st_Fsm_SetupParam.HighTemTHR = 500;
        st_Fsm_SetupParam.LowTemTHR = 100;
        st_Fsm_SetupParam.ConstHighTHR = 400;
        st_Fsm_SetupParam.ConstLowTHR = -150;
        st_Fsm_SetupParam.DiffHighTHR = 99;
        st_Fsm_SetupParam.DiffLowTHR = -99;
        st_Fsm_SetupParam.CompRunTHR = 15;
        st_Fsm_SetupParam.CompStopTHR = -15;
        st_Fsm_SetupParam.CompDlyTHR = 60;
        st_Fsm_SetupParam.EngyLowTHR = 30;
        st_Fsm_SetupParam.RoomTemCal = 0;
        st_Fsm_SetupParam.WatTemCal = 0;
        st_Fsm_SetupParam.Password = 0;

        st_Fsm_SetupParam.OptBit.SwRoomTemDis = 1;
        st_Fsm_SetupParam.OptBit.SwPumpOnOff = 1;
        st_Fsm_SetupParam.OptBit.SwPumpwithCps = 1;
        st_Fsm_SetupParam.OptBit.SwCpsCTR = 0;
        st_Fsm_SetupParam.OptBit.SwTemCTRMode = 0;
        st_Fsm_SetupParam.OptBit.SwPhaseChk = 1;
        st_Fsm_SetupParam.OptBit.SwRemote = 0;
        st_Fsm_SetupParam.OptBit.SwReserve = 0;
        st_Fsm_SetupParam.OptBit.SwReserveByte = 0;

        st_Fsm_CtrParam.DiffTemAim = 25;
        st_Fsm_CtrParam.ConstTemAim = 200;
        
        // 重新写入初始化参数
        Drv_Eep_CompareSave(FSM_EEPSETUPADDR,(const u8 *)&st_Fsm_SetupParam,sizeof(st_Fsm_SetupParam));
        Drv_Eep_CompareSave(FSM_EEPDIFFADDR,(const u8 *)&(st_Fsm_CtrParam.DiffTemAim),2);
        Drv_Eep_CompareSave(FSM_EEPCONSTADDR,(const u8 *)&(st_Fsm_CtrParam.ConstTemAim),2);
        
        // 重新写入Magicnum
        Drv_Eep_CompareSave(FSM_EEPMAGADDR,(const u8 *)&MagicNum,1);
    }
}


/*******************************************************************************
*Function: 按键接收回调函数
*Description:
*Input:
*Output:
*******************************************************************************/
void App_Fsm_RecvKeyEvent(u8 KeyValue)
{
    u8 temp;
    
    // 按键键值转换成按键事件值
    switch(KeyValue)
    {
        case UI_KEY1SHORT:
            // 按键1短按
            temp = EVENT_KEY1S;            
            break;
        
        case UI_KEY1LONG:
            // 按键1长按
            temp = EVENT_KEY1L;
            break;

        case UI_KEY2SHORT:
            // 按键2短按
            temp = EVENT_KEY2S;
            break;

        case UI_KEY2LONG:
            // 按键2长按
            temp = EVENT_KEY2L;
            break;

        case UI_KEY3SHORT:
            // 按键3短按
            temp = EVENT_KEY3S;
            break;

        case UI_KEY3LONG:
            // 按键3长按
            temp = EVENT_KEY3L;
            break;

        case UI_KEY4SHORT:
            // 按键4短按
            temp = EVENT_KEY4S;
            break;

        case UI_KEY4LONG:
            // 按键4长按
            temp = EVENT_KEY4L;
            break; 

        case UI_KEY12LONG:
            // 按键1和2长按
            temp = EVENT_KEY12L;
            break;
            
        case UI_KEY14LONG:
            // 按键1和4长按
            temp = EVENT_KEY14L;    
            break;

        case UI_KEY123LONG:
            temp = EVENT_KEY123L;
            break;
            
        default:
            return;
    }

    // 将按键事件放入FIFO中
    Drv_Event_PutEventToTail(temp);
}


/****************************************************************************
Function: Name
Description:
Input:
Output:
*****************************************************************************/
void App_Fsm_GetTemperature(void)
{
    if(Drv_NTC_GetTempResult(&st_Fsm_CtrParam.WatTem,&st_Fsm_CtrParam.RoomTem))
    {
        // 将采集的到温度值按设置值进行校正
        st_Fsm_CtrParam.WatTem += st_Fsm_SetupParam.WatTemCal;
        st_Fsm_CtrParam.RoomTem += st_Fsm_SetupParam.RoomTemCal;
    }
}


/****************************************************************************
Function: Name
Description:
Input:
Output:
*****************************************************************************/
void App_fsm_ErrScan(void)
{
    static u8 Delay1 = 0;
    static u8 Delay2 = 0;
    static u8 Delay3 = 0;
    static u8 Delay4 = 0;
    static u8 Delay5 = 0;
    
    if(!Drv_PhaseChk_GetEvent() && st_Fsm_SetupParam.OptBit.SwPhaseChk)
    {
        // 相位错误事件
        Delay1 = 0;
        Delay2 = 0;
        Delay3 = 0;
        Delay4 = 0;
        Delay5 = 0;
        Drv_Event_PutEventToTail(EVENT_ERR1_PHASEERR);
        return;
    }

    if(u8_UI_SensorErr == UI_OVERLOADERR)
    {
        // 过载错误事件
        if(Delay1++ > 10)
        {
            Delay1 = 10;
            Drv_Event_PutEventToTail(EVENT_ERR0_OVERLOAD);
        }
        return;
    }
    Delay1 = 0;
    
    if(u8_UI_SensorErr == UI_OPSERR)
    {
        // 传感器错误事件
        if(Delay2++ > 100)
        {
            Delay2 = 100;
            Drv_Event_PutEventToTail(EVENT_ERR2_OPS);
        }
        return;
    }
    Delay2 = 0;
    
    if(u8_UI_SensorErr == UI_HVOVERERR)
    {
        // 高压警报
        if(Delay3++ > 10)
        {
            Delay3 = 10;
            Drv_Event_PutEventToTail(EVENT_ERR3_HVERR);
        }
        return;
    }
    Delay3 = 0;
    
    if(u8_UI_SensorErr == UI_LVOVERERR)
    {
        // 低压警报
        if(Delay4++ > 10)
        {
            Delay4 = 10;
            Drv_Event_PutEventToTail(EVENT_ERR4_LVERR);
        }
        return;
    }
    Delay4 = 0;

    #if(NTC_NTCTYPE == NTC_TYPE1)
    if(st_Fsm_CtrParam.WatTem <= -400)
    #elif(NTC_NTCTYPE == NTC_TYPE2)
    if(st_Fsm_CtrParam.WatTem <= -450)
    #endif    
    {
        // NTC拔掉错误
        Drv_Event_PutEventToTail(EVENT_ERR7_WNTCERR);
        return;
    }

    #if(NTC_NTCTYPE == NTC_TYPE1)
    if(st_Fsm_CtrParam.RoomTem <= -400)
    #elif(NTC_NTCTYPE == NTC_TYPE2)
    if(st_Fsm_CtrParam.RoomTem <= -450)
    #endif
    {
        // NTC拔掉错误
        Drv_Event_PutEventToTail(EVENT_ERR6_RNTCERR);
        return;
    }
   
    if(st_Fsm_CtrParam.WatTem > st_Fsm_SetupParam.HighTemTHR)
    {
        /*OverHeat*/
        /*Turn off when HighTemTHR = 100*/
        if(st_Fsm_SetupParam.HighTemTHR != 1000)
        Drv_Event_PutEventToTail(EVENT_ERR8_OVERHEAR);
        return;
    }
    
    if(st_Fsm_CtrParam.WatTem < st_Fsm_SetupParam.LowTemTHR)
    {
        //LowTem
        if(st_Fsm_SetupParam.LowTemTHR != -400)
        Drv_Event_PutEventToTail(EVENT_ERR9_LOWTEMP);
        return;
    }

    if(EngyLow)
    {
        // 能量不足警报
        Drv_Event_PutEventToTail(EVENT_ERR5_EGYLOW);
        return;
    }
    
    if(Delay5++ > 10)
    {
        Delay5 = 0;
        // 没有错误
        Drv_Event_PutEventToTail(EVENT_NOERR);
    }    
    
}

/*******************************************************************************
*Function:
*Description:1ms call
*Input:
*Output:
*******************************************************************************/
void App_fsm_CtrLogic(void)
{
    static u8 ComStartPrepare = 0;
    static u8 ComDebounce = 50;
    
    static u8 CompDlyTmrID = TIMER_NOTIMER;
    static u8 CompStartDelayTmrID = TIMER_NOTIMER;
    static u8 EngyLowWarnTmrID = TIMER_NOTIMER;
    
    /*Warn Logic*/
    if(f_SoftwarePwrOn)
    {
        // 软开机处理
        f_SoftwarePwrOn = FALSE;
        ComStartPrepare = 0;
        Drv_Timer_Cancel(&CompDlyTmrID);
        Drv_Timer_Cancel(&EngyLowWarnTmrID);
        EngyLow = 0;
        Drv_Timer_Cancel(&CompStartDelayTmrID);
    }

    /*Pump Control Logic*/
    if(!st_Fsm_SetupParam.OptBit.SwPumpwithCps)
    {
        App_fsm_PumpOnOff(1);
    }
    else
    {
        if(HW_COMPPINRED())
        App_fsm_PumpOnOff(1);
    }

    if(st_Fsm_SetupParam.OptBit.SwCpsCTR)
    {
        if(CompStartDelayTmrID == TIMER_NOTIMER)
        {
            // 设置打开压缩机延时时间
            Drv_Timer_Create(&CompStartDelayTmrID,((u32)st_Fsm_SetupParam.CompDlyTHR)<<10,1,NULL);         
        }
        else
        {
            if(Drv_Timer_TimeOutChk(&CompStartDelayTmrID))
            {
                // 延时时间到打开压缩机
                App_fsm_CompOnOff(1);      
            }
			else
			{
                // 时间没到则闪速LED
				if(HW_COMPPINRED() != 1)
				{
					Drv_UI_LedDis(1,UI_LEDCOMP);
					Drv_UI_LedDis(1,UI_LEDCOMPF);
				}
			}
        }
        
        return;
    }

    Drv_Timer_Cancel(&CompStartDelayTmrID);

    /*Compressor Control Logic*/
    if(st_Fsm_CtrParam.WatTem >= (st_Fsm_CtrParam.AllTemAim + st_Fsm_SetupParam.CompRunTHR))
    {
        if(ComDebounce-- < 40)
        {
            ComDebounce = 50;

            if(!ComStartPrepare)
            {
                /*Prepare to Start Compressor*/
                Drv_Timer_Create(&CompDlyTmrID,((u32)st_Fsm_SetupParam.CompDlyTHR)<<10,1,NULL);

                Drv_Timer_Create(&EngyLowWarnTmrID,((u32)st_Fsm_SetupParam.EngyLowTHR*60)<<10,1,NULL);

                if((CompDlyTmrID != TIMER_NOTIMER) && (EngyLowWarnTmrID != TIMER_NOTIMER))
                {
                    ComStartPrepare = 1;

                    Drv_UI_LedDis(1,UI_LEDCOMP);
            
                    Drv_UI_LedDis(1,UI_LEDCOMPF);
                }
                else
                {
                    ComStartPrepare = 0;
                    
                    Drv_Timer_Cancel(&CompDlyTmrID);

                    Drv_Timer_Cancel(&EngyLowWarnTmrID);     
                }
            }
            else
            {
                if(Drv_Timer_TimeOutChk(&CompDlyTmrID))
                {
                    /*Start Compressor*/
                    App_fsm_CompOnOff(1);
                }
                
                if(Drv_Timer_TimeOutChk(&EngyLowWarnTmrID))
                {
                    EngyLow = 1;
                }
            }
        }  
    }
    else if(st_Fsm_CtrParam.WatTem <= (st_Fsm_CtrParam.AllTemAim + st_Fsm_SetupParam.CompStopTHR))
    {
        
        if(ComDebounce++ > 60)
        {
            /*Stop Compressor*/
            
            ComDebounce = 50;

            ComStartPrepare = 0;

            Drv_Timer_Cancel(&CompDlyTmrID);

            Drv_Timer_Cancel(&EngyLowWarnTmrID);

            EngyLow = 0;

            if(!st_Fsm_SetupParam.OptBit.SwCpsCTR)
            App_fsm_CompOnOff(0);
        }
    }
    else
    {
        if((st_Fsm_CtrParam.WatTem > st_Fsm_CtrParam.AllTemAim))
        {
            if(Drv_Timer_TimeOutChk(&EngyLowWarnTmrID))
            {
                EngyLow = 1;
            }
        }
        else
        {
            Drv_Timer_Cancel(&EngyLowWarnTmrID);
            EngyLow = 0;
        }
    
        if(CompDlyTmrID != TIMER_NOTIMER)
        {
            ComStartPrepare = 0;
            
            Drv_UI_LedDis(0,UI_LEDCOMP);
            
            Drv_UI_LedDis(0,UI_LEDCOMPF);

            Drv_Timer_Cancel(&CompDlyTmrID);

            Drv_Timer_Cancel(&EngyLowWarnTmrID);
            EngyLow = 0;
        }
    }
}


/*******************************************************************************
*Function:void App_fsm_StateDispose(void)
*Description: 系统状态处理
*Input:
*Output:
*******************************************************************************/
void App_fsm_StateDispose(void)
{
    u8 Event;
    static u8 PowerUpTest = 1;
    static u16 Password = 000;
    static u8 PasswordOK = 0;
    static u8 StateSys = STATE_NORMAL;
    static u8 StateMenu = STATE_F01;
    static u8 StateTimerID = TIMER_NOTIMER;
    static bool floatflag;
    static u8 SpkAlarmTmrID = TIMER_NOTIMER;
    static u8 SpkKeyTmrID = TIMER_NOTIMER;
    static s16 *Param = NULL;
    u8 bitindex = 0xff;
    static u8 ParamAlter = FALSE;
    s16 ValueMax;
    s16 ValueMin;
    u8 ErrCode = 0;
    u8 temp = 0;
    
    // 从事件fifo中取事件
    Event = Drv_Event_GetEvent();

    if(PowerUpTest && Event == EVENT_KEY123L)
    {
        StateSys = STATE_TEST;
        PowerUpTest = 0;
        return;
    }
    // 判断是否是短按键事件
    if(Event == EVENT_KEY1S || Event == EVENT_KEY2S || Event == EVENT_KEY3S || Event == EVENT_KEY4S)
    {
        PowerUpTest = 0;
        if(SpkKeyTmrID == TIMER_NOTIMER)
        {
            // 短按键事件则打开蜂鸣器响250ms
            Drv_Timer_Create(&SpkKeyTmrID,250,1,NULL);
            App_fsm_Spkon();
        }
    }

    if(Drv_Timer_TimeOutChk(&SpkKeyTmrID))
    {   
        // 250ms时间到，关闭短按键蜂鸣器
        App_fsm_Spkoff();
    }
    
    switch(StateSys)
    {
        case STATE_TEST:
            Drv_UI_StrDis(0,temp,temp,temp);
            Drv_UI_StrDis(1,temp,temp,temp);
            Drv_UI_LedDis(1,UI_LEDALL);
            if(Event == EVENT_KEY4S)
            {
                temp++;
                if(temp >= 10)
                {
                    temp = 0;
                    Drv_UI_LedDis(0,UI_LEDALL);
                    StateSys = STATE_NORMAL;
                }        
            }
            break;

        case STATE_POWEOFF:   
            /*Power off Function*/
            // 关机状态下关闭所有显示
            Drv_UI_StrDis(0,UI_NUMDUMMY,UI_NUMDUMMY,UI_NUMDUMMY);
            Drv_UI_StrDis(1,UI_NUMDUMMY,UI_NUMDUMMY,UI_NUMDUMMY);
            Drv_UI_LedDis(1,UI_LEDPWR);

            // 关机状态下关闭Pump和压缩机。报警输出关闭
            App_fsm_CompOnOff(0);
            App_fsm_PumpOnOff(0);
            HW_ALARMPINOUT(0);
            u8_UI_SensorErr = 0x00;
            f_SoftwarePwrOn= TRUE;
            
            if(Event == EVENT_KEY1S)
            {
                // 开机则从EEPROM取参数或初始化参数
                App_Fsm_InitParam();
                StateSys = STATE_NORMAL;
            }
            
            #if SIMULATION
            if(Event == EVENT_KEY2L)
            #else
            if(Event == EVENT_KEY12L)
            #endif
            {
                // 如果是长按1、2键则进入设置模式
                StateMenu = STATE_F01;
                StateSys = STATE_SETUP;
            }
            break;
            
        case STATE_NORMAL:
        
            /*Normal Function*/ 
            Drv_UI_NumDis(0,1,st_Fsm_CtrParam.WatTem);
            Drv_UI_LedDis(1,UI_LEDPWR);

            HW_ALARMPINOUT(1);
            
            if(st_Fsm_SetupParam.OptBit.SwTemCTRMode)
            {
                // 差温模式显示差温设置值
                st_Fsm_CtrParam.AllTemAim = st_Fsm_CtrParam.DiffTemAim + st_Fsm_CtrParam.RoomTem;
                Drv_UI_NumDis(1,1,st_Fsm_CtrParam.DiffTemAim);
            }
            else
            {
                // 定温模式显示定温设置值
                st_Fsm_CtrParam.AllTemAim = st_Fsm_CtrParam.ConstTemAim; 
                Drv_UI_NumDis(1,1,st_Fsm_CtrParam.ConstTemAim);
            }            
            
			/*Event process in Normal Mode*/
            switch(Event)
            {
                case EVENT_KEY1S:
                    // 短按1键关机
                    if(st_Fsm_SetupParam.OptBit.SwRemote)
                        break;
                    Drv_UI_LedDis(0,UI_LEDALL);
                    StateSys = STATE_POWEOFF;
                    break;
                case EVENT_KEY2S:
                    // 短按2键显示室温
                    Drv_Timer_Create(&StateTimerID,5000,1,NULL);
                    StateSys = STATE_DISWTEM;
                    break;
                case EVENT_KEY3S:
                case EVENT_KEY3L:
                    // 长短按3键，加设置值
                    if(st_Fsm_SetupParam.OptBit.SwTemCTRMode)
                    {
                        if(++st_Fsm_CtrParam.DiffTemAim > st_Fsm_SetupParam.DiffHighTHR)
                        {
                             st_Fsm_CtrParam.DiffTemAim = st_Fsm_SetupParam.DiffHighTHR;
                        }
                        // 保存到EEPROM
                        Drv_Eep_CompareSave(FSM_EEPDIFFADDR,(const u8 *)&(st_Fsm_CtrParam.DiffTemAim),2);
                    }
                    else
                    {
                        if(++st_Fsm_CtrParam.ConstTemAim > st_Fsm_SetupParam.ConstHighTHR)
                        {
                            st_Fsm_CtrParam.ConstTemAim = st_Fsm_SetupParam.ConstHighTHR;
                        }
                        Drv_Eep_CompareSave(FSM_EEPCONSTADDR,(const u8 *)&(st_Fsm_CtrParam.ConstTemAim),2);
                    }
                    break;
                    
                case EVENT_KEY4S:
                case EVENT_KEY4L:
                    // 长短按4键，减设置值
                    if(st_Fsm_SetupParam.OptBit.SwTemCTRMode)
                    {
                        if(--st_Fsm_CtrParam.DiffTemAim < st_Fsm_SetupParam.DiffLowTHR)
                        {
                            st_Fsm_CtrParam.DiffTemAim = st_Fsm_SetupParam.DiffLowTHR;
                        }
                        // 保存到EEPROM
                        Drv_Eep_CompareSave(FSM_EEPDIFFADDR,(const u8 *)&(st_Fsm_CtrParam.DiffTemAim),2);
                    }
                    else
                    {
                        if(--st_Fsm_CtrParam.ConstTemAim < st_Fsm_SetupParam.ConstLowTHR)
                        {
                            st_Fsm_CtrParam.ConstTemAim = st_Fsm_SetupParam.ConstLowTHR;
                        }
                        Drv_Eep_CompareSave(FSM_EEPCONSTADDR,(const u8 *)&(st_Fsm_CtrParam.ConstTemAim),2);
                    }
                    break;
                    
                #if SIMULATION
                case EVENT_KEY2L:
                #else
                case EVENT_KEY12L:
                #endif
                    // 进入设置模式
                    StateMenu = STATE_F01;
                    StateSys = STATE_SETUP;
                    break;
                default:
                    break;
            }

            if(Event >= EVENT_ERR0_OVERLOAD && Event != EVENT_NOERR)
            {
                // 如果是Err错误事件，则进入错误报警状态
                StateSys = STATE_ERR;
            }
            
            break;
            
        case STATE_DISWTEM:
            // 室温显示状态
            if(st_Fsm_SetupParam.OptBit.SwRoomTemDis)
                Drv_UI_NumDis(0,1,st_Fsm_CtrParam.RoomTem);
            else
                // 室温显示选项关闭则直接显示25度
                Drv_UI_NumDis(0,1,250);
             
            // 显示闪烁 
            Drv_UI_NumFlash(0x07);

            if(Drv_Timer_TimeOutChk(&StateTimerID) || StateTimerID == TIMER_NOTIMER)
            {
                // 室温显示时间到，返回正常状态
                StateSys = STATE_NORMAL;
                Drv_UI_NumFlash(0x00);
            }
            
            #if SIMULATION
            if(Event == EVENT_KEY2L)
            #else
            if(Event == EVENT_KEY12L)
            #endif
            {
                // 该状态下碰到长按12键，则进入设置模式
                StateMenu = STATE_F01;
                StateSys = STATE_SETUP;
                Drv_UI_NumFlash(0x00);
            }
            break;
            
        case STATE_SETUP:
            // 正常模式下进入的设置模式要判断是否有错误事件产生
            if(Event >= EVENT_ERR0_OVERLOAD && Event != EVENT_NOERR)
            {
                // 状态跳转前判断参数是否有被改变，有则保存
                if(ParamAlter)
                {
                    ParamAlter = FALSE;
                    Drv_Eep_CompareSave(FSM_EEPSETUPADDR,(const u8 *)(&st_Fsm_SetupParam),sizeof(st_Fsm_SetupParam));
                }
                // 系统状态跳转到错误报警状态
                StateSys = STATE_ERR;
            }

        case STATE_SETUPERR:
            // 错误状态下的进入设置模式，不需要判断是否有错误状态事件
            if(st_Fsm_SetupParam.Password != 0 && PasswordOK == 0 && StateMenu != STATE_F21)
            {
                StateMenu = STATE_F99;
                // 进入密码输入界面
            }
            switch(StateMenu)
            {
                case STATE_F01:
                    // 高温报警设置界面
                    Param = (s16 *)(&st_Fsm_SetupParam.HighTemTHR);
                    ValueMax = 1000;
                    ValueMin = 102;
                    floatflag = 1;
                    break;
                    
                case STATE_F02:
                    // 低温报警设置界面
                    Param = (s16 *)(&st_Fsm_SetupParam.LowTemTHR);
                    ValueMax = 500;
                    ValueMin = -400;
                    floatflag = 1;
                    break;
                    
                case STATE_F03:
                    // 定温模式上限设置
                    Param = (s16 *)(&st_Fsm_SetupParam.ConstHighTHR);
                    ValueMax = 1000;
                    ValueMin = st_Fsm_SetupParam.ConstLowTHR;
                    floatflag = 1;
                    break;
                    
                case STATE_F04:
                    // 定温模式下限设置
                    Param = (s16 *)(&st_Fsm_SetupParam.ConstLowTHR);
                    ValueMax = st_Fsm_SetupParam.ConstHighTHR;
                    ValueMin = -400;
                    floatflag = 1;
                    break;

                case STATE_F05:
                    // 差温模式上限设置
                    Param = (s16 *)(&st_Fsm_SetupParam.DiffHighTHR);
                    ValueMax = 99;
                    ValueMin = st_Fsm_SetupParam.DiffLowTHR;
                    floatflag = 1;
                    break;
                    
                case STATE_F06:
                    // 差温模式下限设置
                    Param = (s16 *)(&st_Fsm_SetupParam.DiffLowTHR);
                    ValueMax = st_Fsm_SetupParam.DiffHighTHR;
                    ValueMin = -99;
                    floatflag = 1;
                    break;
             
                case STATE_F07:
                    // 压缩机开启温差 
                    Param = (s16 *)(&st_Fsm_SetupParam.CompRunTHR);
                    ValueMax = 99;
                    ValueMin = st_Fsm_SetupParam.CompStopTHR;
                    floatflag = 1;
                    break;

                case STATE_F08:
                    // 压缩机关闭温差 
                    Param = (s16 *)(&st_Fsm_SetupParam.CompStopTHR);
                    ValueMax = st_Fsm_SetupParam.CompRunTHR;
                    ValueMin = -99;
                    floatflag = 1;
                    break;
                    
                case STATE_F09:
                    Param = (s16 *)(&st_Fsm_SetupParam.CompDlyTHR);
                    ValueMax = 250;
                    ValueMin = 0;
                    floatflag = 0;
                    break;
                             
                case STATE_F10:
                    // 能量不足报警时间设置
                    Param = (s16 *)(&st_Fsm_SetupParam.EngyLowTHR);
                    ValueMax = 60;
                    ValueMin = 0;
                    floatflag = 0;
                    break;
                    
                case STATE_F11:
                    // 室温校准设置
                    Param = (s16 *)(&st_Fsm_SetupParam.RoomTemCal);
                    ValueMax = 400;
                    ValueMin = -400;
                    floatflag = 1;
                    break;
                    
                case STATE_F12:
                    // 水温校准设置
                    Param = (s16 *)(&st_Fsm_SetupParam.WatTemCal);
                    ValueMax = 400;
                    ValueMin = -400;
                    floatflag = 1;
                    break;       
                    
                case STATE_F13:
                    Param = (u16 *)(&st_Fsm_SetupParam.OptBit);
                    bitindex = 0;
                    break;
                    
                case STATE_F14:
                    bitindex = 1;
                    break;
                    
                case STATE_F15:
                    bitindex = 2;        
                    break;
                    
                case STATE_F16:
                    bitindex = 3;
                    break;
                    
                case STATE_F17:
                    bitindex = 4;
                    break;
                    
                case STATE_F18:
                    bitindex = 5;
                    break;
                    
                case STATE_F19:
                    bitindex = 6;
                    break;
                    
                case STATE_F20:
                    bitindex = 7;
                    break; 
                    
                case STATE_F21:
                    // 密码设定
                    Param = (u16 *)(&st_Fsm_SetupParam.Password);
                    ValueMax = 999;
                    ValueMin = 0;
                    floatflag = 0;
                    break;
                    
                case STATE_F99:
                    Param = (u16 *)(&Password);
                    ValueMax = 999;
                    ValueMin = 0;
                    floatflag = 0;
                    break;
            }

            // 处理设置界面的显示
            Drv_UI_StrDis(0,0xF,(StateMenu-3)/10,(StateMenu-3)%10);
            
            if(Param != NULL)
            {
                if(bitindex != 0xff)
                {
                    // 显示设置的ON/OFF
                    if(REDBIT((*Param),bitindex))
                    {
                        // 第二排数码管显示OFF
                        Drv_UI_StrDis(1,UI_NUMDUMMY,UI_STRO,UI_STRN);
                    }
                    else
                    {
                        // 第二排数码管显示ON
                        Drv_UI_StrDis(1,UI_STRO,UI_STRF,UI_STRF);
                    }  
                }
                else
                {
                    // 显示设置的数值
                    Drv_UI_NumDis(1,floatflag,(s16)*Param);
                }
            }

            // 设置菜单的加键处理
            if((Event == EVENT_KEY3S || Event == EVENT_KEY3L)&& Param != NULL)
            {
                ParamAlter = TRUE;
                if(bitindex != 0xff)
                {
                    // 设置相应的位
                    REDBIT(*Param,bitindex) ? CLRBIT(*Param,bitindex):SETBIT(*Param,bitindex);
                }
                else
                {
                    // 加设置值
                    ((*Param)>=1000 || (*Param)<= -100)?((*Param)+=10):((*Param)++);

                    if((*Param) > ValueMax)
                    {
                        *Param = ValueMax;
                    }
                }
            }
            
            // 设置菜单的减键处理
            if((Event == EVENT_KEY4S || Event == EVENT_KEY4L)&& Param != NULL)
            {
                ParamAlter= TRUE;
                if(bitindex != 0xff)
                {
                    // 设置相应的位
                    REDBIT(*Param,bitindex) ? CLRBIT(*Param,bitindex):SETBIT(*Param,bitindex);
                }
                else
                {
                    // 减设置值
                    ((*Param)>=1000 || (*Param)<= -100)?((*Param)-= 10):((*Param)--);
                    
                    if((*Param) < ValueMin)
                    {
                        *Param = ValueMin;
                    }
                }
            }

            // 判断是否是参数复位
            if((bitindex == 7 && REDBIT(*Param,bitindex)) || Event == EVENT_KEY14L)
            {
                temp = 0;
                // 改变Magic值
                Drv_Eep_CompareSave(FSM_EEPMAGADDR,(const u8 *)&temp,1);
                CLRBIT(*Param,bitindex);
                Drv_UI_LedDis(0,UI_LEDALL);
                // 参数复位后，系统软关机
                StateSys = STATE_POWEOFF;
            }
            
            // 短按2键的处理
            if(Event == EVENT_KEY2S)
            {
                // 如果还在密码界面判断密码输入是否正确
                if(StateMenu == STATE_F99)
                {
                    if(Password == st_Fsm_SetupParam.Password)
                    {
                        // 密码输入正确，进入菜单F01
                        PasswordOK = 1;
                        Password = 0;
                        StateMenu = STATE_F01;
                    }
                    else
                    {
                        // 密码错误
                        PasswordOK = 0;
                    }
                }
                else
                {
                    // 切换菜单项，大于F21返回F01
                    if((++StateMenu)>STATE_F21)
                    {
                        StateMenu = STATE_F01;
                    }
                    
                    // 如果参数有改动则保存到EEPROM
                    if(ParamAlter)
                    {
                        ParamAlter = FALSE;
                        Drv_Eep_CompareSave(FSM_EEPSETUPADDR,(const u8 *)(&st_Fsm_SetupParam),sizeof(st_Fsm_SetupParam));
                    }
                }
            }

            // 短按1键的处理
            if(Event == EVENT_KEY1S)
            {
                PasswordOK = 0;
                Password = 0;
                
                // 参数变动则保存
                if(ParamAlter)
                {
                    ParamAlter = FALSE;
                    Drv_Eep_CompareSave(FSM_EEPSETUPADDR,(const u8 *)(&st_Fsm_SetupParam),sizeof(st_Fsm_SetupParam));
                }
                
                if(StateSys == STATE_SETUPERR)
                {

                    Drv_Timer_Cancel(&SpkAlarmTmrID);
                    App_fsm_Spkoff();
                    Drv_UI_LedDis(0,UI_LEDALL);
                    Drv_UI_LedDis(0,UI_LEDALARMF);
                    // 错误状态下的设置界面按1键退出则直接关机
                    StateSys = STATE_POWEOFF;
                }
                else
                {
                    // 正常的设置状态按1键则返回正常状态
                    StateSys = STATE_NORMAL;
                }
            }
           
            break;
            
        case STATE_ERR:
            // 系统狀態處於错误報警狀態
            if(Event >= EVENT_ERR0_OVERLOAD && Event != EVENT_NOERR)
            {
                if(SpkAlarmTmrID == TIMER_NOTIMER)
                {
                    // 开启蜂鸣器报警
                    Drv_Timer_Create(&SpkAlarmTmrID,250,TIMER_INFINITE,App_fsm_Spkcb);
                }
                
                // 报警灯闪烁
                Drv_UI_LedDis(1,UI_LEDALARM);
                Drv_UI_LedDis(1,UI_LEDALARMF);

                HW_ALARMPINOUT(0);
                
                // 对不同的系统错误进行处理
                switch(Event)
                {
                    case EVENT_ERR0_OVERLOAD:
                        // 过载错误处理
                        ErrCode = 0;
                        App_fsm_CompOnOff(0);
                        if(!st_Fsm_SetupParam.OptBit.SwPumpOnOff)
                        App_fsm_PumpOnOff(0);
                        break;
                    case EVENT_ERR1_PHASEERR:
                        // 相位错误处理
                        ErrCode = 1;
                        App_fsm_CompOnOff(0);
                        App_fsm_PumpOnOff(0);
                        break;
                    case EVENT_ERR2_OPS:
                        // 缺油报警
                        ErrCode = 2;
                        App_fsm_CompOnOff(0);
                        if(!st_Fsm_SetupParam.OptBit.SwPumpOnOff)
                        App_fsm_PumpOnOff(0);
                        break;
                    case EVENT_ERR3_HVERR:
                        // 高压报警
                        ErrCode = 3;
                        App_fsm_CompOnOff(0);
                        if(!st_Fsm_SetupParam.OptBit.SwPumpOnOff)
                        App_fsm_PumpOnOff(0);
                        break;
                    case EVENT_ERR4_LVERR:
                        // 低压报警
                        ErrCode = 4;
                        App_fsm_CompOnOff(0);
                        if(!st_Fsm_SetupParam.OptBit.SwPumpOnOff)
                        App_fsm_PumpOnOff(0);
                        break;
                    case EVENT_ERR5_EGYLOW:
                        // 能量不足报警
                        ErrCode = 5;
                        break;
                    case EVENT_ERR6_RNTCERR:
                        // 室温NTC错误报警
                        ErrCode = 6;
                        App_fsm_CompOnOff(0);
                        if(!st_Fsm_SetupParam.OptBit.SwPumpOnOff)
                        App_fsm_PumpOnOff(0);
                        break;
                    case EVENT_ERR7_WNTCERR:
                        // 水温NTC错误报警
                        ErrCode = 7;
                        App_fsm_CompOnOff(0);
                        if(!st_Fsm_SetupParam.OptBit.SwPumpOnOff)
                        App_fsm_PumpOnOff(0);
                        break;
                    case EVENT_ERR8_OVERHEAR:
                        // 高温报警
                        ErrCode = 8;
                        break;
                    case EVENT_ERR9_LOWTEMP:
                        // 低温报警
                        ErrCode = 9;
                        App_fsm_CompOnOff(0);
                        if(!st_Fsm_SetupParam.OptBit.SwPumpOnOff)
                        App_fsm_PumpOnOff(0);
                        break;
                    case EVENT_ERR10_NOTRUN:
                        ErrCode = 10;
                        break;
                     
                    default:
                        break;
                }
                
                // 数码管显示错误的代码
                Drv_UI_StrDis(0,UI_STRA,UI_STRL,ErrCode);
            }
            
            #if SIMULATION
            if(Event == EVENT_KEY2L)
            #else
            if(Event == EVENT_KEY12L)
            #endif
            {
                // 在错误状态中进入设置界面
                StateMenu = STATE_F01;
                StateSys = STATE_SETUPERR;
            }
            
            if(Event == EVENT_KEY1S)
            {
                // 关闭蜂鸣器报警
                // 关闭LED显示
                HW_ALARMPINOUT(0);
                Drv_UI_LedDis(0,UI_LEDALL);
                Drv_Timer_Cancel(&SpkAlarmTmrID);
                App_fsm_Spkoff();
                // 按1鍵系统关机
                StateSys = STATE_POWEOFF;
            }
            
            break;
    }
    
    if((StateSys != STATE_TEST) && (StateSys != STATE_ERR) && (StateSys != STATE_POWEOFF) && (StateSys != STATE_SETUPERR))
    {
        // 处理正常状态的控制逻辑
        App_fsm_CtrLogic();
    }
}
