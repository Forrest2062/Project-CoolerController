/*******************************************************************************
*Copyright(C),2012-2013,mingv150,All rights reserved
*FileName:
*Description:
*Author:
*Version:
*Date:
*******************************************************************************/
#ifndef _DRV_EVENT_H_
#define _DRV_EVENT_H_

#define EVENT_FIFOLEN 8

enum event
{
    EVENT_NONE = 0,
    EVENT_KEY1S,
    EVENT_KEY1L,
    EVENT_KEY2S,
    EVENT_KEY2L,
    EVENT_KEY3S,
    EVENT_KEY3L,
    EVENT_KEY4S,
    EVENT_KEY4L,
    EVENT_KEY12L,
    EVENT_KEY14L,
    EVENT_KEY123L,
    EVENT_ERR0_OVERLOAD,
    EVENT_ERR1_PHASEERR,
    EVENT_ERR2_OPS,
    EVENT_ERR3_HVERR,
    EVENT_ERR4_LVERR,
    EVENT_ERR5_EGYLOW,
    EVENT_ERR6_RNTCERR,
    EVENT_ERR7_WNTCERR,
    EVENT_ERR8_OVERHEAR,
    EVENT_ERR9_LOWTEMP,
    EVENT_ERR10_NOTRUN,
    EVENT_NOERR,
    EVENT_MAX
};


typedef struct EventMage
{
    u8 EventFiFo[EVENT_FIFOLEN];    
    u8 FiFoHead;
    u8 FiFoTail;
}t_EventMage;


#ifndef _DRV_EVENT_C_
void Drv_Event_PutEventToTail(u8 key);
u8 Drv_Event_GetEvent(void);
#endif

#endif

