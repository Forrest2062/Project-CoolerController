/*******************************************************************************
*Copyright(C),2012-2013,mingv150,All rights reserved
*FileName:
*Description:
*Author:
*Version:
*Date:
*******************************************************************************/

#define _DRV_PHASECHK_C_
#include <htc.h>
#include "../Common.h"
#include "../Model.h"
#include "../oopc.h"

#include "Drv_PhaseChk.h"
#include "Drv_Hardware.h"
#include "Drv_UserInterface.h"
#include "Drv_NTCSensor.h"

t_PhaseChk st_PhaseChk_Dat = 
{
    1,1,1,1,1,1,1,1
};


/*******************************************************************************
*Function:
*Description: 相位检测函数
*Input:
*Output:
*******************************************************************************/
void Drv_PhaseChk_1msScan(void)
{
    static bool Phc1New;
    static bool Phc2New;
    static u8 Phc1Count = 0;
    static u8 Phc2Count = 0;
    static u8 Debounce = 0;
    u8 ResultChk = 0;
    
    Phc1New = HW_PHC1PINRED();
    NOP();NOP();NOP();NOP();NOP();
    Phc2New = HW_PHC2PINRED();

    Phc1Count = (++Phc1Count > 254)? 254 : Phc1Count;
    Phc2Count = (++Phc2Count > 254)? 254 : Phc2Count;

    if(st_PhaseChk_Dat.Phc2Old != Phc2New)
    {
        st_PhaseChk_Dat.Phc2Old = Phc2New;

        Phc2Count = 0;
    }

    if(st_PhaseChk_Dat.Phc1Old != Phc1New)
    {
        st_PhaseChk_Dat.Phc1Old = Phc1New;

        Phc1Count = 0;

        if(Phc1New == 1)   /*Rising Edge*/    
        {
            ResultChk = 1;
            if(Phc2New == 0 && Phc2Count < 5)
            {
                st_PhaseChk_Dat.ResultNew = TRUE;
            }
            else
            {
                st_PhaseChk_Dat.ResultNew = FALSE;
            }

        }
    }
    else
    {
        if(Phc1Count > 100)
        {
            ResultChk = 1;
            st_PhaseChk_Dat.ResultNew = FALSE;
        }
    }

    Drv_NTC_MesureIRQ();

    if(st_PhaseChk_Dat.ResultOld != st_PhaseChk_Dat.ResultNew)
    {
        st_PhaseChk_Dat.ResultOld = st_PhaseChk_Dat.ResultNew;
        Debounce = 0;
    }
    else if(ResultChk)
    {
        Debounce++;
        if(Debounce > 20)
        {
            st_PhaseChk_Dat.Result = st_PhaseChk_Dat.ResultOld;
        }
    }

    Drv_NTC_MesureIRQ();
}


/*******************************************************************************
*Function:
*Description:
*Input:
*Output:
*******************************************************************************/
u8 Drv_PhaseChk_GetEvent()
{
    return st_PhaseChk_Dat.Result;
}