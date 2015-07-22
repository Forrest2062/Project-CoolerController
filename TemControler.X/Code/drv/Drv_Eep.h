/*******************************************************************************
*Copyright(C),2012-2013,mingv150,All rights reserved
*FileName:
*Description:
*Author:
*Version:
*Date:
*******************************************************************************/
#ifndef _DRV_EEP_H_
#define _DRV_EEP_H_

#ifndef _DRV_EEP_C_
void Drv_Eep_GetParam(u8 StartAddr,u8 *pDes,u8 Size);
void Drv_Eep_SaveParam(u8 StartAddr,const u8 *pSrc,u8 Size);
void Drv_Eep_CompareSave(u8 StartAddr,const u8 *pSrc,u8 Size);
#endif

#endif