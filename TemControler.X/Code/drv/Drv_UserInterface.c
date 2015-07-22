/*******************************************************************************
*Copyright(C),2012-2013,mingv150,All rights reserved
*FileName:
*Description:
*Author:
*Version:
*Date:
*******************************************************************************/

#define _DRV_USERINTERFACE_C_
#include <htc.h>
#include "../Common.h"
#include "../Model.h"
#include "../oopc.h"

#include "Drv_UserInterface.h"

#include "Drv_Hardware.h"
#include "../app/App_fsm.h"
#include "Drv_NTCSensor.h"

t_Display st_UI_Display = 
{ 
    /*.Num1 = */8,
    /*.Num2 = */8,
    /*.Num3 = */8,
    /*.Num4 = */8,
    /*.Num5 = */8,
    /*.Num6 = */8,
    /*.NumF = */{0},
    /*.Led = */{0x0F},
    /*.State = */ 0xFF
};

u8 u8_UI_SensorErr = 0x00;

/*******************************************************************************
*Function:
*Description:
*Input:
*Output:
*******************************************************************************/
static void Drv_UI_Shifter16(u16 dat)
{
    u8 i;
    
    STRPINOUT(0);
    for(i=16;i!=0;i--)
    {
        REDBIT(dat,i-1)?DATAPINOUT(1):DATAPINOUT(0);
        CLKPINOUT(0);
        CLKPINOUT(1);
        CLKPINOUT(0);
    }
    STRPINOUT(1);
}


/*******************************************************************************
*Function:
*Description:
*Input:
*Output:
*******************************************************************************/
static void Drv_UI_InputScan(u8 SensorState)
{
    static u8 SensorStateBuf = 0xff;
    static u8 Debounce;

    if(SensorState != SensorStateBuf)
    {
        SensorStateBuf = SensorState;
        Debounce = 0;
    }
    else if(Debounce < SHORTDEBOUNCE)
    {
        Debounce++;
    }
    else
    {
        Debounce = 0;
        SensorState = SensorState^0xff;
        
        if(REDBIT(SensorState,UI_OPSERR))
        {
            u8_UI_SensorErr = UI_OPSERR;
        }
        else if(REDBIT(SensorState,UI_OVERLOADERR))
        {
            u8_UI_SensorErr = UI_OVERLOADERR;
        }
        else if(REDBIT(SensorState,UI_HVOVERERR))
        {
            u8_UI_SensorErr = UI_HVOVERERR;
        }
        else if(REDBIT(SensorState,UI_LVOVERERR))
        {
            u8_UI_SensorErr = UI_LVOVERERR;
        }
        else
        {
            u8_UI_SensorErr = 0;
        }
    }   
}


/*******************************************************************************
*Function:
*Description:
*Input: KeyState 8个位可以表示8个Key 每个位为0表示down 为1表示up
*Output:
*******************************************************************************/
static void Drv_UI_KeyDebounce(u8 KeyState)
{
    u8 TrigKey = 0x00;
    static u8 KeyStateBuf = 0x00;
    static u8 ContKey = 0x00;
    static u8 ContKeyBuf = 0x00;
    static u8 ShortDebounce;
    static u16 LongDebounce;

    if(KeyState != KeyStateBuf)
    {
        KeyStateBuf = KeyState;
        ShortDebounce = 0;
    }
    else if(ShortDebounce < SHORTDEBOUNCE)
    {
        ShortDebounce++;
    }
    else
    {
        //ShortDebounce = 0;
        KeyState = KeyStateBuf^0xff;
        TrigKey = KeyState&(KeyState^ContKey);
        ContKey = KeyState;

        /*Just execute One time*/
        // TrigKey确认一次短按
        App_Fsm_RecvKeyEvent(TrigKey);
        
        if(ContKey != ContKeyBuf)
        {
            ContKeyBuf = ContKey;
            LongDebounce = 0;
        }
        else if(LongDebounce < LONGDEBOUNCE)
        {
            LongDebounce++;
        }
        else
        {
            /**/
            //LongDebounce = LongDebounce>>1;
            LongDebounce = LONGDEBOUNCE - 50;  
            SETBIT(ContKey,7);
            // 确认有长按键
            App_Fsm_RecvKeyEvent(ContKey);
        }
    }
}


/****************************************************************************
Function: 按键扫描函数
Description:
Input:
Output:
*****************************************************************************/
void Drv_UI_KeyScan(void)
{
    // 按键抗抖动
    Drv_UI_KeyDebounce(st_UI_Display.KeyState|0xF0);
    Drv_UI_InputScan(st_UI_Display.KeyState|0x0F);
}


/*******************************************************************************
*Function:
*Description:
*Input:
*Output:
*******************************************************************************/
void Drv_UI_ScanTask(void)
{
    u8 i;
    u8 FlashFlag = 0;
    u8 KeyBitNum;
    u16 ScanValue;
    
    static u8 ScanStep = UI_SCANLED;
    static u8 FlashCount = 0;
    static u8 KeyStatus = 0xff;  
    const u8 Segtab[] =
    {
        /*Common cathode and interver*/
        0x0A,0xFA,0x8C,0x98,0x78,0x19,0x09,0xDA,
        0x08,0x18,0x48,0x29,0x0F,0xA8,0x0D,0x4D,
        0xE9,0x2F,0xFD,0xFF
    };

    FlashCount++;
    
    if(FlashCount&UI_FLASHTIME)
    {
        FlashFlag = 1;
    }

    Drv_NTC_MesureIRQ();

    /*Scan Led & Seg*/
    switch(ScanStep)
    {
        // 扫描LED
        case UI_SCANLED:       
            ScanValue = UI_SEGDUMMY;
            if(st_UI_Display.Led.LedBit.Led1&&(!FlashFlag||!st_UI_Display.Led.LedBit.Led1F))
            {
                ScanValue &= 0xBF;
            }
            if(st_UI_Display.Led.LedBit.Led2&&(!FlashFlag||!st_UI_Display.Led.LedBit.Led2F))
            {
                ScanValue &= 0xFB;
            }
            if(st_UI_Display.Led.LedBit.Led3&&(!FlashFlag||!st_UI_Display.Led.LedBit.Led3F))
            {
                ScanValue &= 0xFE;
            }
            if(st_UI_Display.Led.LedBit.Led4&&(!FlashFlag||!st_UI_Display.Led.LedBit.Led4F))
            {
                ScanValue &= 0xDF;
            }
            break;

        // 扫描 数字1 
        case SCANNUM1:
            ScanValue = Segtab[st_UI_Display.Num1];
            if(FlashFlag&&st_UI_Display.NumC.NumCBit.Num1F)
            {
                ScanValue = UI_SEGDUMMY;
            }
            break;

        // 扫描 数字2     
        case SCANNUM2:
            ScanValue = Segtab[st_UI_Display.Num2];
            if(st_UI_Display.NumC.NumCBit.Num2P)
            {
                ScanValue &= 0xF7; 
            }
            if(FlashFlag&&st_UI_Display.NumC.NumCBit.Num2F)
            {
                ScanValue = UI_SEGDUMMY;
            }
            break;
        
        // 扫描 数字3 
        case SCANNUM3:
            KeyBitNum = 3;
            ScanValue = Segtab[st_UI_Display.Num3];
            if(FlashFlag&&st_UI_Display.NumC.NumCBit.Num3F)
            {
                ScanValue = UI_SEGDUMMY;
            }
            break;
        
        // 扫描 数字4     
        case SCANNUM4:
            KeyBitNum = 2;
            ScanValue = Segtab[st_UI_Display.Num4];
            if(FlashFlag&&st_UI_Display.NumC.NumCBit.Num4F)
            {
                ScanValue = UI_SEGDUMMY;
            }            
            break;
            
        // 扫描 数字5  
        case SCANNUM5:
            KeyBitNum = 1;
            ScanValue = Segtab[st_UI_Display.Num5];
            if(st_UI_Display.NumC.NumCBit.Num5P)
            {
                ScanValue &= 0xF7; 
            } 
            if(FlashFlag&&st_UI_Display.NumC.NumCBit.Num5F)
            {
                ScanValue = UI_SEGDUMMY;
            }            
            break;
        
        // 扫描 数字6      
        case SCANNUM6:
            KeyBitNum = 0;
            ScanValue = Segtab[st_UI_Display.Num6];
            if(FlashFlag&&st_UI_Display.NumC.NumCBit.Num6F)
            {
                ScanValue = UI_SEGDUMMY;
            }            
            break;
            
        default:
            break;
    }

    Drv_NTC_MesureIRQ();
    
    #if SIMULATION
    ScanValue = (~ScanValue)<<8;
    ScanValue |= (u8)(~ScanStep);
    #else
    ScanValue = ScanValue<<8;
    ScanValue |= (u8)ScanStep;
    #endif

    /*Shift dat to Seg*/
    STRPINOUT(0);
    for(i=16;i!=0;i--)
    {
        (REDBIT(ScanValue,i-1))?(DATAPINOUT(1)):(DATAPINOUT(0));
        NOP();
        CLKPINOUT(0);
        NOP();
        CLKPINOUT(1);
        NOP();
        CLKPINOUT(0);
        NOP();

        Drv_NTC_MesureIRQ();
    }
    STRPINOUT(1);
    NOP();
    NOP();
    NOP();
    
    if(ScanStep < SCANNUM2)
    {
        (KEYPINRED()) ? (SETBIT(KeyStatus,KeyBitNum)) : (CLRBIT(KeyStatus,KeyBitNum));
        
        if(KEYPINRED() == 1)
        {
            SETBIT(KeyStatus,KeyBitNum);

        }
        else
        {
            CLRBIT(KeyStatus,KeyBitNum);
        }
        (SWPINRED())? (SETBIT(KeyStatus,KeyBitNum+4)) : (CLRBIT(KeyStatus,KeyBitNum+4));
    }
    
    if(ScanStep == SCANNUM6)
    {
        st_UI_Display.KeyState = KeyStatus;
        //Drv_UI_KeyDebounce(KeyStatus|0xF0);
        //Drv_UI_InputScan(KeyStatus|0x0F);
    }
    
    ScanStep = ScanStep >>1;
    ScanStep = (ScanStep) ? (ScanStep) : UI_SCANLED;

    Drv_NTC_MesureIRQ();
}


/*******************************************************************************
*Function:
*Description: 数码管闪烁控制函数
*Input:
*Output:
*******************************************************************************/
void Drv_UI_NumFlash(u8 Value)
{    
    Value &= 0x77;
    st_UI_Display.NumC.NumCByte &= 0x88;
    st_UI_Display.NumC.NumCByte |= Value;
}


/*******************************************************************************
*Function:
*Description: 数码管显示数字函数
*Input:
*Output:
*******************************************************************************/
void Drv_UI_NumDis(u8 chan,u8 floattype,s16 Value)
{
    u8 x=0,y=0,z=0;
    
    //处理百位
    if(Value < 0)
    {   
        // 如果是一个负数值
        Value = -Value;
        if(Value > 99)
        {
            Value = Value/10;
            floattype = 0;
        }
        x = UI_MINUS;
    }
    else
    {
        // 如果数值大于999
        if(Value > 999)
        {
            Value = Value/10;
            floattype = 0;
        }
        x = (u8)(Value/100);
    }

    // 处理小数点
    if(chan)
    {
        st_UI_Display.NumC.NumCBit.Num5P = 0;
        if(floattype)
        st_UI_Display.NumC.NumCBit.Num5P = 1; 
    }
    else
    {
        st_UI_Display.NumC.NumCBit.Num2P = 0;
        if(floattype)
        st_UI_Display.NumC.NumCBit.Num2P = 1; 
    }
    
    //处理十位跟个位 
    y = (u8)(Value/10%10);
    z = (u8)(Value%10);
    
    if(chan)
    {
        st_UI_Display.Num6 = x;  
        st_UI_Display.Num5 = y;
        st_UI_Display.Num4 = z;
    }
    else
    {
        st_UI_Display.Num3 = x;  
        st_UI_Display.Num2 = y;
        st_UI_Display.Num1 = z;
    }
}


/*******************************************************************************
*Function:
*Description: 数码管显示字符函数
*Input:
*Output:
*******************************************************************************/
void Drv_UI_StrDis(u8 chan,u8 char1,u8 char2,u8 char3)
{    
    if(chan)
    {
        // 数码管通道1
        st_UI_Display.Num6 = char1;  
        st_UI_Display.Num5 = char2;
        st_UI_Display.Num4 = char3;
        st_UI_Display.NumC.NumCByte &= 0x0F; 
    }
    else
    {
        // 数码管通道0
        st_UI_Display.Num3 = char1;  
        st_UI_Display.Num2 = char2;
        st_UI_Display.Num1 = char3;
        st_UI_Display.NumC.NumCByte &= 0xF0; 
    }
}


/*******************************************************************************
*Function:
*Description: LED 显示函数
*Input:
*Output:
*******************************************************************************/
void Drv_UI_LedDis(u8 OnOff,u8 LedIndex)
{
    if(LedIndex >= UI_LEDALL)
    {
        if(OnOff)
        {
            // 点亮所有的LED
            st_UI_Display.Led.LedByte = 0x0F;
        }
        else
        {
            // 关闭所有的LED
            st_UI_Display.Led.LedByte = 0x00;
        }
    }
    
    if(OnOff)
    SETBIT(st_UI_Display.Led.LedByte,LedIndex);
    else
    CLRBIT(st_UI_Display.Led.LedByte,LedIndex);
}


/*******************************************************************************
*Function:
*Description:
*Input:Freq unit = 100ms
*Output:
*******************************************************************************/
void Drv_UI_Buzzer(u8 Freq,u8 RepeatTime)
{
    
}