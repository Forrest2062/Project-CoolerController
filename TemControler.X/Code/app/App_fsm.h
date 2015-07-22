/*******************************************************************************
*Copyright(C),2012-2013,mingv150,All rights reserved
*FileName:
*Description:
*Author:
*Version:
*Date:
*******************************************************************************/
#ifndef _APP_FSM_H_
#define _APP_FSM_H_

/*各种系统状态定义*/
enum state
{
    // 软关机状态
    STATE_POWEOFF = 0,
    // 正常状态
    STATE_NORMAL,
    // 显示室温状态
    STATE_DISWTEM, 
    // 设置状态
    STATE_SETUP,
    // 设置菜单F01~F99
    STATE_TEST,
    STATE_F01,
    STATE_F02,
    STATE_F03,
    STATE_F04,
    STATE_F05,
    STATE_F06,
    STATE_F07,
    STATE_F08,
    STATE_F09,
    STATE_F10,
    STATE_F11,
    STATE_F12,
    STATE_F13,
    STATE_F14,
    STATE_F15,
    STATE_F16,
    STATE_F17,
    STATE_F18,
    STATE_F19,
    STATE_F20,
    STATE_F21,
    STATE_F22,
    STATE_F23,
    STATE_F24,
    STATE_F25,
    STATE_F99 = 99+3,
    // 错误状态
    STATE_ERR,
    STATE_SETUPERR,
};


#define FSM_INITTIMER(cnt) (cnt) = 0
#define FSM_TIMEOUTCHK(cnt,maxcnt) (((cnt)>(maxcnt))?1:0)

// #define FSM_DISWTMTIME 10000
// #define FSM_SETUPNDTIME 10000
// #define FSM_PARAMINVALID 0xffff

// 無效溫度值
#define FSM_TEMINVAILD 2000

/*控制参数数据结构*/
typedef struct CtrParam
{
    // 当前水温值
    s16 WatTem;
    // 当前室温值
    s16 RoomTem;
    // 目标设置值
    s16 AllTemAim;
    // 定温设置值
    s16 ConstTemAim;
    // 差温设置值 
    s16 DiffTemAim;
}t_CtrParam;


/*定义设置参数的数据结构*/
typedef struct SetupParam
{
    // 高温阈值设置
    s16 HighTemTHR;
    // 低温阈值设置
    s16 LowTemTHR;
    // 定温模式的阈值上限
    s16 ConstHighTHR;
    // 定温模式的阈值下限
    s16 ConstLowTHR;
    // 差温模式的阈值上限
    s16 DiffHighTHR;
    // 差温模式的阈值下限
    s16 DiffLowTHR;
    // 压缩机开启阈值
    s16 CompRunTHR;
    // 压缩机停止阈值
    s16 CompStopTHR;
    // 压缩机延时
    s16 CompDlyTHR;
    // 能量不足阈值
    s16 EngyLowTHR;
    // 室温校准
    s16 RoomTemCal;
    // 水温校准
    s16 WatTemCal;
    // 设置密码
    u16 Password;
    struct 
    {
        u8 SwRoomTemDis:1;
        u8 SwPumpOnOff:1;
        u8 SwPumpwithCps:1;
        u8 SwCpsCTR:1;       
        u8 SwTemCTRMode:1;
        u8 SwPhaseChk:1;
        u8 SwRemote:1;
        u8 SwReserve:1;
        u8 SwReserveByte;
    }OptBit;
    
}t_SetupParam;

// EEPROM魔幻数
#define FSM_EEPMAGICNUM 0xA5
// EEPROM起始地址
#define FSM_EEPMAGADDR 0x7f
// EEPROM设置参数起始地址
#define FSM_EEPSETUPADDR (FSM_EEPMAGADDR + 2)
// EEPROM其它参数起始地址
#define FSM_EEPOTHERADDR (FSM_EEPSETUPADDR + sizeof(t_SetupParam))
// 差温设置值设置地址
#define FSM_EEPDIFFADDR (FSM_EEPOTHERADDR + 2)
// 定温设置值设置地址
#define FSM_EEPCONSTADDR (FSM_EEPDIFFADDR + 2)


#ifndef _APP_FSM_C_

void App_Fsm_InitParam(void);
void App_Fsm_RecvKeyEvent(u8 KeyValue);
void App_Fsm_RecvSensorEvent(u8 InputValue);
void App_fsm_StateDispose();
void App_fsm_CtrLogic();
void App_fsm_ErrScan(void);
void App_Fsm_StartCheck(void);
void App_fsm_Spkoff(void);
void App_fsm_Spkon(void);
void App_Fsm_GetTemperature(void);

#endif

#endif

