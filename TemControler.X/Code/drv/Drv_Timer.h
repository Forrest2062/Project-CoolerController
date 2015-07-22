/*******************************************************************************
*Copyright(C),2012-2013,mingv150,All rights reserved
*FileName:
*Description:
*Author:
*Version:
*Date:
*******************************************************************************/
#ifndef _DRV_TIMER_H_
#define _DRV_TIMER_H_

#define TIMER_TIMERNUM  7
#define TIMER_NOTIMER   0xFF
#define TIMER_INFINITE  0xF

typedef void (*t_TimerCB)(void);

typedef struct
{
    u8 *pTimerID;
    u8 Reserve:6;
    u8 IsActive:1;
    u8 TimeOut:1;
    u8 RepeatTotal:4;
    u8 RepeatCount:4;
    u32 Tick;
    u32 Time;
    t_TimerCB CallBack;

}t_TimerAttr;

#ifndef _DRV_TIMER_C_
void Drv_Timer_Init(void);
u8 Drv_Timer_Create(u8 *pTimerID,u32 MSecs,u8 Repeat,t_TimerCB FunCb);
void Drv_Timer_Cancel(u8 *pTimerID);
u8 Drv_Timer_TimeOutChk(u8 *pTimerID);
void Drv_Timer_Scan(void);
#endif

#endif

