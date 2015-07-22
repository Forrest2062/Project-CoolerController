/*******************************************************************************
*Copyright(C),2012-2013,mingv150,All rights reserved
*FileName:
*Description:
*Author:
*Version:
*Date:
*******************************************************************************/

#define _DRV_NTCSENSOR_C_
#include <htc.h>
#include "../Common.h"
#include "../Model.h"
#include "../oopc.h"

#include "Drv_NTCSensor.h"
#include "Drv_UserInterface.h"

#include "Drv_Hardware.h"

static s16 s16_NTC_WatTem = NTC_INVALIDTEM;
static s16 s16_NTC_RoomTem = NTC_INVALIDTEM;
static bool f_NTC_TempretureOK = 0;
static u16 u16_NTC_PulseCount[3];
u8 f_NTC_Sampling = 0;
static bool f_NTC_SamComplete = 0;
static u8 u8_NTC_Channel = 0;


#if(NTC_NTCTYPE == NTC_TYPE2)
// 温度表2
/*-45~110*/
const u16 NTC1Tab[]=
{
    59895,55508,51476,47768,44356,41213,38318,35648,33184,30909,28807,26865,
    25067,23404,21863,20435,19111,17882,16742,15682,14697,13782,12930,12137,
    11398,10709,10068,9469,8910,8388,7900,7444,7018,6619,6245,5895,5567,5260,
    4972,4701,4447,4209,3985,3774,3576,3390,3215,3050,2894,2748,2609,2479,2356,
    2240,2131,2027,1929,1837,1750,1667,1589,1515,1445,1378,1315,1255,1199,1145,
    1094,1046,1000,956,915,876,838,803,769,736,706,676,649,622,597,573,550,528,
    507,487,468,450,432,416,400,385,370,356,343,330,318,307,295,285,274,265,255,
    246,238,229,221,214,206,199,193,186,180,174,168,163,157,152,147,143,138,134,
    129,125,121,118,114,110,107,104,101,98,95,92,89,87,84,82,79,77,75,73,70,68,
    67,65,63,61,59,58,56,55,53,52,
};

#elif(NTC_NTCTYPE == NTC_TYPE1)
/*Tab:*/
const u16 NTC1Tab[]=
{
    /*-40~0*/
    30706.2,28836,27090.6,25461,23938.8,22516.3,21186.4,19942.6,18778.9,
    17689.5,16669.4,15713.8,14818.3,13978.6,13191.1,12452.3,11758.8,11107.6,
    10496,9921.2,9381,8869.6,8389,7937,7511.8,7111.7,6735.1,6380.4,6046.3,5731.4,
    5434.6,5454.7,4890.7,4641.6,4406.5,4184.5,3974.7,3776.6,3589.4,3412.3,3245,

    /*1~10*/
    3085,2934,2791,2656,2528,2407,2293,2185,2082,1985,
    /*11~20*/
    1892,1805,1722,1644,1569,1498,1431,1367,1306,1249,
    /*21~30*/
    1194,1142,1092,1045,1000,957,917,888,841,806,
    /*31~40*/
    772,740,710,681,653,627,601,577,554,532,
    
    /*41~125*/
    511.1,491,471.8,453.4,435.9,419.1,403,387.6,372.9,358.8,
    345.4,332.6,320.3,308.6,297.3,286.5,276.2,266.2,256.7,247.6,238.9,
    230.4,222.4,214.6,207.2,200.1,193.2,186.6,180.3,174.2,168.4,162.7,
    157.3,152.1,147.1,142.3,137.7,133.2,128.9,124.8,120.8,117,113.3,109.7,
    106.3,103,99.8,96.8,93.8,91,88.2,85.6,83,80.5,78.1,75.8,73.6,71.5,
    69.4,67.4,65.4,63.6,61.7,60,58.3,56.6,55.1,53.5,52,50.6,49.2,47.9,
    46.6,45.3,44.1,42.9,41.7,40.6,39.6,38.5,37.5,36.5,35.6,34.6,33.8
};
#else
#error Not Define NTC type
#endif



/*******************************************************************************
*Function:
*Description: 初始化温度测量模块
*Input:
*Output:
*******************************************************************************/
void Drv_NTC_MesureInit(void)
{
    ANSEL = 0x00;
    ANSELH = 0x00;
	TRISB0 = 1;
	WPUB0 = 1;
	HW_AMPINC(0);
	HW_AMPIN(1);
	DRV_NTC_CHANGND();
	/*外中断设置为上升沿中断(1:Rising edge)*/
	INTEDG = 1;
	INTF = 0;
	INTE = 0;
	f_NTC_Sampling = 0;
}

/*******************************************************************************
*Function:
*Description: 开启测量
*Input:
*Output:
*******************************************************************************/
void Drv_NTC_MesureOn(void)
{
	if(f_NTC_SamComplete)
	{
        return;
	}
	
	if(f_NTC_Sampling)
	{
	    /*Time out*/
		f_NTC_Sampling = 0;
		INTEDG = 1;
		INTE = 0;
		INTF = 0;
		DRV_NTC_CHANGND();
		HW_AMPIN(1);
		return;
	}
	
	/*for Sampling*/
	f_NTC_Sampling = 1;
	
	/*1:Rising edge*/
	DRV_NTC_CHANGND();

    INTF = 0;
	INTEDG = 1;
	INTE = 1;
    HW_AMPIN(0);
	
	return;
}

/*******************************************************************************
*Function:
*Input:
*Output:
*******************************************************************************/
void Drv_NTC_MesureIRQ(void)
{
    //u8 temp;
    static u8 cnt = 0;
 
    if(INTF&&INTE&&f_NTC_Sampling)
    {
        if(INTEDG == 1)
        {
            /*For Next Falling Edge*/
            INTE = 0;
            INTEDG = 0;
            INTF = 0;
            INTE = 1;         
        }
        else
        {
            cnt++;
            
            if(cnt == 1)
            {
                /*Start the time*/
                TMR1H = 0x00;
                TMR1L = 0x00;
                TMR1IF = 0;
                TMR1ON = 1;
        
                /*Switch Compare Channel on*/
                //temp = PORTC & 0x6F;

                switch(u8_NTC_Channel)
            	{
            	case NTC_CHANWAT:
            	    //PORTC = temp|0x00;
                    DRV_NTC_CHANWAT();
            		break;	
            	case NTC_CHANROM:	    
            	    //PORTC = temp|0x10;  
                    DRV_NTC_CHANROM();
            		break;
            	case NTC_CHANREF:
            	    //PORTC = temp|0x80;
            		DRV_NTC_CHANREF();
            		break;
            	default:
            	    //PORTC = temp|0x90;
            	    DRV_NTC_CHANGND();
            		break;
            	}

                /*For Next Falling Edge*/
                //INTE = 0;
                //INTEDG = 0;
                //INTF = 0;
                //INTE = 1;
            }
            else
            {
                cnt= 0;
                /*Stop the time*/
                TMR1ON = 0;

                /*For next Mesure on*/
                INTE = 0;
    			INTEDG = 1;
    			HW_AMPIN(1);
    			DRV_NTC_CHANGND();
                
                if(TMR1IF == 0)
                {
                    /*Get Mesure Value*/
                    u16_NTC_PulseCount[u8_NTC_Channel] = (u16)TMR1L;
                    u16_NTC_PulseCount[u8_NTC_Channel] |= ((u16)TMR1H)<<8;
                    
    				u8_NTC_Channel++; 				
    		        if(u8_NTC_Channel >= 3)
    		        {
    			        u8_NTC_Channel = 0;
    			        f_NTC_SamComplete = 1;
    			    }
                }
                else
                {
                    /*Overflow*/
                    TMR1IF = 0;
                }

                f_NTC_Sampling = 0;
            }
        }
        
        INTF = 0;
    }
}


/*******************************************************************************
*Function:
*Description:
*Input:
*Output:
*******************************************************************************/
static u16 Drv_NTC_CalcResValue(u16 RefWidth,u16 NtcWidth)
{
    u32 ResValue;
	
    if((u32)NtcWidth >= ((u32)RefWidth*2) || (RefWidth == 0) || (NtcWidth == 0))
    {
        return 0xffff;        
    }

    // 根据时间比例计算电阻值
    ResValue = 1000*(u32)NtcWidth;
    ResValue = ResValue/(u32)((u32)RefWidth*2 - (u32)NtcWidth);

    return (u16)ResValue;
    
}


/*******************************************************************************
*Function:
*Description: 中值滤波算法
*Input:
*Output:
*******************************************************************************/
static u16 Drv_NTC_GetMiddleValue(u16 Array[],u16 n)
{
    u16 i,j,t;
 
    // 排序
    for(i=0;i<n-1;i++)
    {
        for(j=0;j<n-1-i;j++)
        {
            if(Array[j]>Array[j+1])
            {
               t = Array[j+1];
               Array[j+1] = Array[j];
               Array[j] = t;
            }
        }
    }

    // 取中间值
    return(Array[(n-1)/2]);
}


/*******************************************************************************
*Function:
*Description: 
*Input:
*Output:
*******************************************************************************/
/*
static u16 Drv_NTC_ADFilter(u8 Channel)
{
    u8 i;
    u16 ADValue[5];
    u16 ResultTemp;
    static u16 u16_NTC_WatADValue = 0;
    static u16 u16_NTC_RomADValue = 0;

    for(i=0;i<5;i++)
    {
        ADValue[i] = (u16)Drv_NTC_GetADCResult(Channel);
    }
    
    //median filter
    
    
    //first-order filter
    
    if(Channel)
    {
        u16_NTC_WatADValue = (u16_NTC_WatADValue*NTC_ADCFILTERK + ADValue[2]*(32-NTC_ADCFILTERK))/32;
        ResultTemp = u16_NTC_WatADValue;
    }
    else
    {
        u16_NTC_RomADValue = (u16_NTC_RomADValue*NTC_ADCFILTERK + ADValue[2]*(32-NTC_ADCFILTERK))/32;
        ResultTemp = u16_NTC_RomADValue;
    }
    return ResultTemp;
}
*/

/*******************************************************************************
*Function:
*Description:  温度查表函数
*Input:
*Output:
*******************************************************************************/
static float Drv_NTC_CheckNTCTable(u16 ResVal)
{
    float Temperature;
    float k;
    u8 TempMin = 0;
    u8 TempMax = 0;
    u8 MidVal = 0;

    TempMax = (sizeof(NTC1Tab)/sizeof(NTC1Tab[0])) - 1;
    
    if(ResVal >= NTC1Tab[0])
    {
        #if(NTC_NTCTYPE == NTC_TYPE1)
    	return (0 - 40);
    	#elif(NTC_NTCTYPE == NTC_TYPE2)
        return (0 - 45);
        #else
        #error Not Define NTC type
        #endif         
    }
    
    if(ResVal <= NTC1Tab[TempMax])
    {
        #if(NTC_NTCTYPE == NTC_TYPE1)
    	return (TempMax - 40);
    	#elif(NTC_NTCTYPE == NTC_TYPE2)
        return (TempMax - 45);
        #else
        #error Not Define NTC type
        #endif
    }
    
    /*Dichotomy*/
    while(ResVal > NTC1Tab[TempMax])
    {
        MidVal = (TempMax + TempMin)/2;

        if((MidVal == TempMin)||(MidVal == TempMax))
        {
            break;
        }
        if(ResVal >= NTC1Tab[MidVal])
        {
            TempMax = MidVal;
        }
        else
        {
            TempMin = MidVal;
        }
    }

    /*linear interpolation*/
    k = (TempMin - TempMax);
    k /= (NTC1Tab[TempMin] - NTC1Tab[TempMax]);
	Temperature = TempMax + k*(ResVal - NTC1Tab[TempMax]);
	
	#if(NTC_NTCTYPE == NTC_TYPE1)
	Temperature = Temperature - 40;
	#elif(NTC_NTCTYPE == NTC_TYPE2)
    Temperature = Temperature - 45;
    #else
    #error Not Define NTC type
    #endif
    
    return Temperature;
}

/*******************************************************************************
*Function:
*Description: 计算温度
*Input:
*Output:
*******************************************************************************/
u8 Drv_NTC_CalTemperature(void)
{
    #define MIDVALUESIZE 5
    u8 i;
    static u16 NtcValue[2][MIDVALUESIZE];
    static u8 Index = 0;
    s16 temp1;
    s16 temp2;
    s32 temp3;
    s32 temp4;
    s32 temp5;
    s32 temp6;
    
    if(f_NTC_SamComplete)
	{
	    //for(i=0;i<3;i++)
	    //{
        //    if(u16_NTC_PulseCount[i] == 0)
        //    {
        //        f_NTC_SamComplete = 0;
        //        return FALSE;
        //    }
	    //}
	    
        // 计算电阻值
		NtcValue[0][Index] = Drv_NTC_CalcResValue(u16_NTC_PulseCount[NTC_CHANREF],u16_NTC_PulseCount[NTC_CHANWAT]);
		
		NtcValue[1][Index] = Drv_NTC_CalcResValue(u16_NTC_PulseCount[NTC_CHANREF],u16_NTC_PulseCount[NTC_CHANROM]);

		f_NTC_SamComplete = 0;
		
		if(++Index >= MIDVALUESIZE)
		{
			Index = 0;

			/*Median Filtering*/
			NtcValue[0][0] = Drv_NTC_GetMiddleValue(NtcValue[0],MIDVALUESIZE);
			
			NtcValue[1][0] = Drv_NTC_GetMiddleValue(NtcValue[1],MIDVALUESIZE);

			//Drv_UI_NumDis(0,0,(NtcValue[0][0]>>8)&0x00ff);

			//Drv_UI_NumDis(1,0,NtcValue[0][0]&0x00ff);

            // 查表得到温度值
            temp1 = (s16)(Drv_NTC_CheckNTCTable(NtcValue[0][0])*10);

			temp2 = (s16)(Drv_NTC_CheckNTCTable(NtcValue[1][0])*10);

            if(s16_NTC_WatTem == NTC_INVALIDTEM)
            {
                s16_NTC_WatTem = temp1;
                s16_NTC_RoomTem = temp2;
                f_NTC_TempretureOK = TRUE;
                return TRUE;
            }
            
            // 一介滤波
            /*first-order filter*/
            temp3 = ((u32)s16_NTC_WatTem)<<4;
            temp4 = ((u32)s16_NTC_RoomTem)<<4;
            temp5 = ((u32)temp1)<<4;
            temp6 = ((u32)temp2)<<4;
            s16_NTC_WatTem = (s16)(temp3+temp5)>>5;
            s16_NTC_RoomTem = (s16)(temp4+temp6)>>5;

            return TRUE;
            
		}
	}
	return FALSE;
}


/*******************************************************************************
*Function:
*Description:
*Input:
*Output:
*******************************************************************************/
u8 Drv_NTC_GetTempResult(s16 *pWatTem,s16 *pRoomTem)
{
    if(f_NTC_TempretureOK)
    {
        *pWatTem = s16_NTC_WatTem;
        *pRoomTem = s16_NTC_RoomTem;
        return TRUE;
    }
    return FALSE;
}

