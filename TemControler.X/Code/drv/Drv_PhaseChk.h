/*******************************************************************************
*Copyright(C),2012-2013,mingv150,All rights reserved
*FileName:
*Description:
*Author:
*Version:
*Date:
*******************************************************************************/

#ifndef _DRV_PHASECHK_H_
#define _DRV_PHASECHK_H_
#if 0
//3500us
#define PHASECHK_MIDTHD1 11
#define PHASECHK_TOL1
#define PHASECHK_LOWTHD1 6
#define PHASECHK_HIGHTHD1 16
#define PHASECHK_JUDGE1(val) ((val)>PHASECHK_LOWTHD1&&(val)<PHASECHK_HIGHTHD1)?1:0
//16000us
#define PHASECHK_MIDTHD2 52
#define PHASECHK_TOL2
#define PHASECHK_LOWTHD2 32
#define PHASECHK_HIGHTHD2 72
#define PHASECHK_JUDGE2(val) ((val)>PHASECHK_LOWTHD2&&(val)<PHASECHK_HIGHTHD2)?1:0

#define PHASECHK_NOCHG 128
#else

//3500us
#define PHASECHK_MIDTHD1 3
#define PHASECHK_TOL1
#define PHASECHK_LOWTHD1 1
#define PHASECHK_HIGHTHD1 5
#define PHASECHK_JUDGE1(val) ((val)>PHASECHK_LOWTHD1&&(val)<PHASECHK_HIGHTHD1)?1:0
//16000us
#define PHASECHK_MIDTHD2 16
#define PHASECHK_TOL2
#define PHASECHK_LOWTHD2 10
#define PHASECHK_HIGHTHD2 22
#define PHASECHK_JUDGE2(val) ((val)>PHASECHK_LOWTHD2&&(val)<PHASECHK_HIGHTHD2)?1:0

#define PHASECHK_NOCHG 2000
#endif

typedef struct
{
    u8 Result:1;
    u8 ResultOld:1;
    u8 ResultNew:1;
    u8 Phc1Old:1;
    u8 Phc2Old:1;
    u8 Res1:1;
    u8 Res2:1;
    u8 Res3:1;
}t_PhaseChk;

#ifndef _DRV_PHASECHK_C_
void Drv_PhaseChk_1msScan(void);
u8 Drv_PhaseChk_GetEvent(void);
#endif

#ifndef _DRV_PHASECHK_C_

#endif

#endif

