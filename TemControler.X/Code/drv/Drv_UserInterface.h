/*******************************************************************************
*Copyright(C),2012-2013,mingv150,All rights reserved
*FileName:
*Description:
*Author:
*Version:
*Date:
*******************************************************************************/

#ifndef _DRV_USERINTERFACE_H_
#define _DRV_USERINTERFACE_H_



#define UI_SCANLED 0x40
#define SCANNUM1 0x20
#define SCANNUM2 0x10
#define SCANNUM3 0x08
#define SCANNUM4 0x04
#define SCANNUM5 0x02
#define SCANNUM6 0x01

#define SCANKEY4 0x08
#define SCANKEY3 0x04
#define SCANKEY2 0x02
#define SCANKEY1 0x01


#define UI_STRO 0
#define UI_STRA 10
#define UI_STRF 15
#define UI_STRN 16
#define UI_STRL 17
#define UI_MINUS 18
#define UI_NUMDUMMY 19
#define UI_SEGDUMMY 0xFF

#define UI_FLASHTIME 0x80
#define FLASHCOUNT ((FLASHTIME)/20)

// 按键短按抗抖时间
#define SHORTDEBOUNCE 30
// 按键长按时间
#define LONGDEBOUNCE  1500

// 按鍵長按用最高位為1表示
#define UI_KEY1SHORT 0x1
#define UI_KEY1LONG (UI_KEY1SHORT+0x80)
#define UI_KEY2SHORT 0x2
#define UI_KEY2LONG (UI_KEY2SHORT+0x80)
#define UI_KEY3SHORT 0x4
#define UI_KEY3LONG (UI_KEY3SHORT+0x80)
#define UI_KEY4SHORT 0x8
#define UI_KEY4LONG (UI_KEY4SHORT+0x80)
#define UI_KEY12LONG (UI_KEY2LONG|UI_KEY1LONG)
#define UI_KEY14LONG (UI_KEY1LONG|UI_KEY4LONG)
#define UI_KEY123LONG (UI_KEY1LONG|UI_KEY2LONG|UI_KEY3LONG)

#define UI_LEDPWR 0
#define UI_LEDPUMP 1 
#define UI_LEDCOMP 2 
#define UI_LEDALARM 3 
#define UI_LEDPWRF 4 
#define UI_LEDPUMPF 5 
#define UI_LEDCOMPF 6 
#define UI_LEDALARMF 7
#define UI_LEDALL 8

#define UI_OPSERR  6
#define UI_OVERLOADERR 7
#define UI_HVOVERERR 4
#define UI_LVOVERERR 5

#define HEXTOBCD(i,x,y,z) (x)=(i)/100;(y)=(i)/10%10;(z)=(i)%10



typedef union Led
{
    u8 LedByte;
    struct
    {
        u8 Led1:1;
        u8 Led2:1;
        u8 Led3:1;
        u8 Led4:1;
        u8 Led1F:1;
        u8 Led2F:1;
        u8 Led3F:1;
        u8 Led4F:1;
    }LedBit;
}t_Led;

typedef union NumC
{
    u8 NumCByte;
    struct 
    {
        u8 Num1F:1;
        u8 Num2F:1;
        u8 Num3F:1;
        u8 Num2P:1;  
        u8 Num4F:1;
        u8 Num5F:1;
        u8 Num6F:1;
        u8 Num5P:1;
    }NumCBit;
}t_NumC;

typedef struct Display
{
    u8 Num1;
    u8 Num2;
    u8 Num3;
    u8 Num4;
    u8 Num5;
    u8 Num6;
    t_NumC NumC;
    t_Led Led;
    u8 KeyState;
}t_Display;


#ifndef _DRV_USERINTERFACE_C_
void Drv_UI_ScanTask(void);
void Drv_UI_NumFlash(u8 pofl);
void Drv_UI_NumDis(u8 chan,u8 floattype,s16 Value);
void Drv_UI_StrDis(u8 chan,u8 char1,u8 char2,u8 char3);
void Drv_UI_LedDis(u8 OnOff,u8 LedIndex);
void Drv_UI_KeyScan(void);
extern u8 u8_UI_SensorErr;
#endif

#endif
