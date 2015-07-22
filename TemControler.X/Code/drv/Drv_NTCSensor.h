/*******************************************************************************
*Copyright(C),2012-2013,mingv150,All rights reserved
*FileName:
*Description:
*Author:
*Version:
*Date:
*******************************************************************************/
#ifndef _DRV_NTCSENSOR_H_
#define _DRV_NTCSENSOR_H_

#define NTC_ADCFILTERK 16
#define NTC_INVALIDTEM 3000

#define NTC_CHANWAT 0
#define NTC_CHANROM 1
#define NTC_CHANREF 2

#define NTC_TYPE1 1
#define NTC_TYPE2 2
#define NTC_NTCTYPE NTC_TYPE2




//#define NTC_CHANGND 3   

#define DRV_NTC_CHANWAT()   do {HW_4051BSET(0);HW_4051ASET(0);NOP();NOP();}while(0)
#define DRV_NTC_CHANROM()   do {HW_4051BSET(0);HW_4051ASET(1);NOP();NOP();}while(0)
#define DRV_NTC_CHANREF()   do {HW_4051BSET(1);HW_4051ASET(0);NOP();NOP();}while(0)
#define DRV_NTC_CHANGND()   do {HW_4051BSET(1);HW_4051ASET(1);NOP();NOP();}while(0)

#ifndef _DRV_NTCSENSOR_C_
void Drv_NTC_MesureInit(void);
void Drv_NTC_MesureOn(void);
void Drv_NTC_MesureIRQ();
u8 Drv_NTC_CalTemperature(void);
u8 Drv_NTC_GetTempResult(s16 *pWatTem,s16 *pRoomTem);
extern u8 f_NTC_Sampling;
#endif

#endif
