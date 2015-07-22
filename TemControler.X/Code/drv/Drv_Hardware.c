/*******************************************************************************
*Copyright(C),2012-2013,mingv150,All rights reserved
*FileName:
*Description:
*Author:
*Version:
*Date:
*******************************************************************************/
#define _DRV_HARDWARE_C_

#include <htc.h>
#include "../Common.h"
#include "../Model.h"
#include "../oopc.h"

#include "Drv_Hardware.h"

#include "Drv_UserInterface.h"
#include "Drv_PhaseChk.h"
#include "Drv_Timer.h"
#include "Drv_NTCSensor.h"
#include "../app/App_fsm.h"

/*Configuration Bit Setting*/
//__CONFIG(FOSC_HS & WDTE_ON & PWRTE_OFF & MCLRE_ON & CP_OFF & CPD_OFF & BOREN_ON & IESO_ON & FCMEN_ON & LVP_ON);
//__CONFIG(BOR4V_BOR40V & WRT_OFF);

//__CONFIG(FOSC_HS & WDTE_OFF & PWRTE_ON & MCLRE_ON & CP_ON & CPD_OFF & BOREN_OFF & IESO_OFF & FCMEN_OFF & LVP_OFF);
//__CONFIG(WRT_OFF);
// CPU配置选项配置
__CONFIG(FOSC_HS & WDTE_OFF & PWRTE_OFF & MCLRE_OFF & CP_ON & CPD_OFF & BOREN_ON & IESO_OFF & FCMEN_ON & LVP_OFF & DEBUG_ON);
__CONFIG(BOR4V_BOR40V & WRT_OFF);


bool f_Hw_2ms = 0;
bool f_Hw_32ms = 0;
bool f_Hw_64ms = 0;
bool f_Hw_128ms = 0;

/*******************************************************************************
*Function:
*Description: 中断处理函数
*Input:
*Output:
*******************************************************************************/
void interrupt IRQ(void)
{
    static u8 TimeBase1 = 0;
    static u8 TimeBase2 = 0;
    static u8 TimeBase3 = 0;
    static u8 TimeBase4 = 0;

    Drv_NTC_MesureIRQ();

    if(T0IF&&T0IE)
    {
        TMR0=179;
        
        Drv_PhaseChk_1msScan();
        
        Drv_UI_ScanTask();
        
        /*4*/
        if(++TimeBase1 == 2)
        {
        	TimeBase1 = 0;
            f_Hw_2ms = 1;
        }

        /*32*/
        if(++TimeBase2 == 41)
        {
        	TimeBase2 = 0;
            f_Hw_32ms = 1;
            Drv_NTC_MesureOn();
            Drv_NTC_MesureIRQ();
        }
        
        /*64*/
        if(++TimeBase3 == 63)
        {
        	TimeBase3 = 0;
            f_Hw_64ms = 1;
        }
        
        /*128*/
        if(++TimeBase4 == 129)
        {
        	TimeBase4 = 0;
            f_Hw_128ms = 1;
        }
        
        Drv_Timer_Scan();

        Drv_NTC_MesureIRQ();
        
        T0IF=0;
    }
}


/*******************************************************************************
*Function: void Drv_Hw_IOInit(void)
*Description: 初始化I/O口
*Input:
*Output:
*******************************************************************************/
void Drv_Hw_IOInit(void)
{    
    OSTS = 1;
    SCS = 0;
    HW_PORTAD(0x00);
    // 初始化端口A的方向
    HW_PORTAC(0xF3);
    HW_PORTBD(0x00);
    // 初始化端口B的方向
    HW_PORTBC(0xDD);
    HW_PORTCD(0x00);
    // 初始化端口C的方向
    HW_PORTCC(0x00);
}


/*******************************************************************************
*Function:
*Description: 初始化定时器
*Input:
*Output:
*******************************************************************************/
void Drv_Hw_TmrInit(void)
{
    /*Enable Prescale*/
    /*Clock = 4915200Hz*/
    PSA=0;
    T0CS=0;
    
    /*1:64*/
    PS0=1;PS1=0;PS2=1;  
    /*timeout time:997HZ*/
    TMR0=179;
    /*Tiemr0 int en*/
    T0IE=1;
    GIE=1;

    /*Timer 1 Setting*/
    T1CON = 0b00100100;
	TMR1IE = 0;
    TMR1IF = 0;
    //T1CKPS0=0;T1CKPS1=1;
    //T1OSCEN=0;
    //TMR1CS=0;

    TMR1H=0x00;TMR1L=0x00;
}


/*******************************************************************************
*Function:
*Description: 初始化看门狗
*Input:
*Output:
*******************************************************************************/
void Drv_Hw_WdtInit(void)
{
    PSA=0;
    WDTCON = 0x11;//32us*8192 = 262.144ms
    CLRWDT();
}




