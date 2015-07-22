/*******************************************************************************
*Copyright(C),2012-2013,mingv150,All rights reserved
*FileName:
*Description:
*Author:
*Version:
*Date:
*******************************************************************************/
#define _DRV_EVENT_C_

#include <htc.h>
#include "../Common.h"
#include "../Model.h"
#include "../oopc.h"

#include "Drv_Event.h"


static t_EventMage st_Event_EventMage = 
{
    {0,0,0,0,0,0,0,0},
    0,
    0
};

/*******************************************************************************
*Function:
*Description: 将事件放入fifo中
*Input:
*Output:
*******************************************************************************/
void Drv_Event_PutEventToTail(u8 key)
{
    if(st_Event_EventMage.FiFoHead == (st_Event_EventMage.FiFoTail+1)%EVENT_FIFOLEN)
    {
        return;
    }
    st_Event_EventMage.EventFiFo[st_Event_EventMage.FiFoTail] = key;
    st_Event_EventMage.FiFoTail = (++st_Event_EventMage.FiFoTail)%EVENT_FIFOLEN;
}


/*******************************************************************************
*Function:
*Description: 从fifo中取出事件
*Input:
*Output:
*******************************************************************************/
u8 Drv_Event_GetEvent(void)
{
    u8 key;

    if(st_Event_EventMage.FiFoHead == st_Event_EventMage.FiFoTail)
    {
        return EVENT_NONE;
    }
    key = st_Event_EventMage.EventFiFo[st_Event_EventMage.FiFoHead];       
    st_Event_EventMage.FiFoHead = (++st_Event_EventMage.FiFoHead)%EVENT_FIFOLEN;
    return key;
}
