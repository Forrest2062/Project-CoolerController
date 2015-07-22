/*******************************************************************************
*Copyright(C),2012-2013,mingv150,All rights reserved
*FileName:
*Description:
*Author:
*Version:
*Date:
*******************************************************************************/
#define _DRV_EEP_C_

#include <htc.h>
#include "../Common.h"
#include "../Model.h"
#include "../oopc.h"

#include "Drv_NTCSensor.h"
#include "Drv_Eep.h"

/*******************************************************************************
*Function:
*Description:
*Input:
*Output:
*******************************************************************************/
void delay(u16 x)
{
    u8 i;
    for(x;x>0;x--)
    {
        for(i=250;i>0;i--);
    }
}



u16 Drv_Eep_CheckSum(const u8 *buffer,u16 size)
{
    u32 cksum=0;
	
    while (size>1)
    {
		cksum += *(buffer++);
		size -= sizeof(u8);
	}
	
	if(size)
	{
		cksum += *buffer;
	}
    //将32位转换为16位
    while (cksum>>16)
		cksum = (cksum>>16) + (cksum & 0xffff);
    return (u16) (~cksum);
}

/*******************************************************************************
*Function:
*Description:
*Input:
*Output:
*******************************************************************************/
void Drv_Eep_GetParam(u8 StartAddr,u8 *pDes,u8 Size)
{
    u8 i;
	u8 temp1;
	u8 temp2;

    for(i=0;i<Size;i++)
    {
        //*(pDes+i) = eeprom_read(StartAddr+i);
        //for(j=0;j<254;j++);
        //*(pDes+i) = EEPROM_READ(StartAddr+i);
		do
		{
			do{;}
			while(RD==1);                        //上一次读操作是否完成
			EEADR=(StartAddr+i);                //EEPROM地址为00H 
			EEPGD=0;                            //指向EEPROM数据储存器
			RD=1;                                //开始读
			do{;}
			while(RD==1);                        //等待读操作完成
			temp1 = EEDATA;                            //读出
		
			do{;}
			while(RD==1);                        //上一次读操作是否完成
			EEADR=(StartAddr+i);                //EEPROM地址为00H 
			EEPGD=0;                            //指向EEPROM数据储存器
			RD=1;                                //开始读
			do{;}
			while(RD==1);                        //等待读操作完成
			temp2 = EEDATA;                            //读出
		}while(temp1^temp2);
		
		*(pDes+i) = temp2;
    }
}

/*******************************************************************************
*Function:
*Description:
*Input:
*Output:
*******************************************************************************/
void Drv_Eep_SaveParam(u8 StartAddr,const u8 *pSrc,u8 Size)
{
    u8 i;
    u8 GIETemp = 0;
	u8 ReadTemp = 0;
    
    for(i=0;i<Size;i++)
    {
        //eeprom_write(StartAddr+i,*(pSrc+i));
        //EEIF=0;
	    //while(WR==1)
	    //{;}
        //for(j=0;j<254;j++);
        //EEPROM_WRITE(StartAddr+i,*(pSrc+i));
        //EEIF=0;
		do
		{
			do{;}
			while(WR == 1);                        //上一次写操作是否完成
			EEADR = (StartAddr+i);                 //EEPROM地址
			EEDATA = *(pSrc+i);                    //准备写入EEPROM的数据
			GIETemp = 0;
			while(f_NTC_Sampling);
			if(GIE)
				GIETemp = 1;
			GIE = 0;
			EEPGD = 0;                            //指向EEPROM数据储存器
			WREN = 1;                             //使能写操作
			EECON2 = 0x55;                        //设置通用参数
			EECON2 = 0xAA;                        //设置通用参数
			WR = 1;                               //开始写
			do{;}
			while(WR == 1);                       //等待写操作完成
			WREN = 0;                             //禁止写操作
			EEIF = 0;
			
			//delay(10);
			
			do{;}
			while(RD == 1);                       //上一次读操作是否完成
			EEADR = (StartAddr+i);                //EEPROM地址为00H 
			EEPGD = 0;                            //指向EEPROM数据储存器
			RD = 1;                               //开始读
			do{;}
			while(RD == 1);                       //等待读操作完成
			ReadTemp = EEDATA;                    //读出
			if(GIETemp)
				GIE = 1;
		}while(ReadTemp != *(pSrc+i));
    }
}


/*******************************************************************************
*Function:
*Description:
*Input:
*Output:
*******************************************************************************/
void Drv_Eep_CompareSave(u8 StartAddr,const u8 *pSrc,u8 Size)
{
    u8 i;
    u8 GIETemp = 0;
	u8 ReadTemp = 0;
	//u16 CheckSum = 0;
	
	//CheckSum = Drv_Eep_CheckSum(pSrc,(u16)Size);
    
    for(i=0;i<Size;i++)
    {
        //eeprom_write(StartAddr+i,*(pSrc+i));
        //EEIF=0;
	    //while(WR==1)
	    //{;}
        //for(j=0;j<254;j++);
        //EEPROM_WRITE(StartAddr+i,*(pSrc+i));
        //EEIF=0;

        do{;}
		while(RD == 1);                       //上一次读操作是否完成
		EEADR = (StartAddr+i);                //EEPROM地址为00H 
		EEPGD = 0;                            //指向EEPROM数据储存器
		RD = 1;                               //开始读
		do{;}
		while(RD == 1);                       //等待读操作完成
		ReadTemp = EEDATA;                    //读出

		if(ReadTemp != *(pSrc+i))
		{	
    		do
    		{
    			do{;}
    			while(WR == 1);                        //上一次写操作是否完成
    			EEADR = (StartAddr+i);                 //EEPROM地址
    			EEDATA = *(pSrc+i);                    //准备写入EEPROM的数据
    			GIETemp = 0;
    			while(f_NTC_Sampling);
    			if(GIE)
    				GIETemp = 1;
    			GIE = 0;
    			EEPGD = 0;                            //指向EEPROM数据储存器
    			WREN = 1;                             //使能写操作
    			EECON2 = 0x55;                        //设置通用参数
    			EECON2 = 0xAA;                        //设置通用参数
    			WR = 1;                               //开始写
    			do{;}
    			while(WR == 1);                       //等待写操作完成
    			WREN = 0;                             //禁止写操作
    			EEIF = 0;
    			if(GIETemp)
    				GIE = 1;
    			delay(10);
    			
    			do{;}
    			while(RD == 1);                       //上一次读操作是否完成
    			EEADR = (StartAddr+i);                //EEPROM地址为00H 
    			EEPGD = 0;                            //指向EEPROM数据储存器
    			RD = 1;                               //开始读
    			do{;}
    			while(RD == 1);                       //等待读操作完成
    			ReadTemp = EEDATA;                    //读出
    		}while(ReadTemp != *(pSrc+i));
		}
    }
}


