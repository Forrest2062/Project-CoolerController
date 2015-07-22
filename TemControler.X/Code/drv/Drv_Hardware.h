/*******************************************************************************
*Copyright(C),2012-2013,mingv150,All rights reserved
*FileName:
*Description:
*Author:
*Version:
*Date:
*******************************************************************************/
#ifndef _DRV_HARDWARE_H_
#define _DRV_HARDWARE_H_

#define HW_PORTAC(x) (TRISA = (x))
#define HW_PORTAD(x) (PORTA = (x))
#define HW_PORTBC(x) (TRISB = (x))
#define HW_PORTBD(x) (PORTB = (x))
#define HW_PORTCC(x) (TRISC = (x))
#define HW_PORTCD(x) (PORTC = (x))

#define HW_PUMPPINOUT(x) (RA2=(x))
#define HW_COMPPINOUT(x) (RC2=(x))
#define HW_COMPPINRED()  (RC2)
#define HW_ALARMPINOUT(x) (RC0=(x))

#define SPKPINOUT(x) (RC1=(x))
#define STRPINOUT(x) (RC6=(x))
#define DATAPINOUT(x) (RC5=(x))
#define CLKPINOUT(x) (RC3=(x))
#define KEYPINRED() (RA4)
#define SWPINRED() (RA5)
#define HW_PHC1PINRED() (RA0)
#define HW_PHC2PINRED() (RA1)
#define HW_4051ASET(x) (RC4=(x))
#define HW_4051BSET(x) (RC7=(x))
#define HW_AMPIN(x) (RB5=(x))
#define HW_AMPINC(x) (TRISB5=(x))
#define HW_TESTPINC(x) (TRISB1=(x))
#define HW_TESTPIN(x) (RB1=(x))


#ifndef _DRV_HARDWARE_C_
void Drv_Hw_IOInit(void);
void Drv_Hw_TmrInit(void);
void Drv_Hw_WdtInit(void);

extern bool f_Hw_2ms;
extern bool f_Hw_32ms;
extern bool f_Hw_64ms;
extern bool f_Hw_128ms;
#endif

#endif
