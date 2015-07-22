/*******************************************************************************
*Copyright(C),2012-2013,mingv150,All rights reserved
*FileName:
*Description:
*Author:
*Version:
*Date:
*******************************************************************************/
#ifndef _APP_PID_H_
#define _APP_PID_H_


typedef struct PIDParam 
{   
    double  SetPoint;      // �趨Ŀ�� Desired value,�Ӳ����̵�����ֵ
    
    double  Proportion;    // ������ Proportional Const    
    double  Integral;      // ��ֳ��� Integral Const    
    double  Derivative;    // ΢�ֳ��� Derivative Const    
    
    double  LastError;     // ƫ�� Error[-1]    
    double  PrevError;     // ƫ�� Error[-2]    
    double  SumError;      // ƫ���ܺ� Sums of Errors    
}t_PIDParam;

#ifndef _APP_PID_C_
void App_Pid_Setup(double SetPoint);
double App_Pid_Calc(double NextPoint);
void App_Pid_Control(double ControlVal);
#endif

#endif