/*******************************************************************************
*Copyright(C),2012-2013,mingv150,All rights reserved
*FileName:
*Description:
*Author:
*Version:
*Date:
*******************************************************************************/
#define _APP_PID_C_
/*common*/
#include <htc.h>
#include "../Common.h"
#include "../Model.h"
#include "../oopc.h"

/*myself*/
#include "App_Pid.h"

/*mycall*/
#include "../drv/Drv_PowerCtr.h"
#include "../drv/Drv_Hardware.h"

#if 0
t_PIDParam st_Pid_PidParam;


void App_Pid_Setup(double SetPoint)
{
    st_Pid_PidParam.SetPoint = SetPoint;
    st_Pid_PidParam.Proportion = 1;
    st_Pid_PidParam.Integral = 1;
    st_Pid_PidParam.Derivative = 1;
}

/*******************************************************************************
*Copyright(C),2012-2013,mingv150,All rights reserved
*FileName:
*Description:
*Author:
*Version:
*Date:
*******************************************************************************/
double App_Pid_Calc(double NextPoint)   
{   
    double  dError,Error,result;
    
    Error = st_Pid_PidParam.SetPoint - NextPoint;          // ƫ�� 
    
    st_Pid_PidParam.SumError += Error;                     // ���    
    dError = st_Pid_PidParam.LastError - st_Pid_PidParam.PrevError;   // ��ǰ΢��    
    st_Pid_PidParam.PrevError = st_Pid_PidParam.LastError;   
    st_Pid_PidParam.LastError = Error;
    
    return (st_Pid_PidParam.Proportion * Error             // ������    
           +st_Pid_PidParam.Integral * st_Pid_PidParam.SumError       // �����    
           +st_Pid_PidParam.Derivative * dError            // ΢����    
           );
}   

void App_Pid_Control(double ControlVal)
{
    if(ControlVal)
    {
        //HW_PUMPPINOUT(1);
    }
    else
    {
        //HW_PUMPPINOUT(0);
    }
}

#endif




