#ifndef __DEVICE_REMOTE_H_
#define __DEVICE_REMOTE_H_
#include "stm32f4xx.h"

/**红外中断的优先级 */
#define DEVICE_REMOTE_PREE_PRI      INT_PRE_PRI_0

#define DEVICE_REMOTE_SUB_PRI       INT_SUB_PRI_3

//红外遥控识别码(ID),每款遥控器的该值基本都不一样,但也有一样的.
//我们选用的遥控器识别码为0
#define DEVICE_REMOTE_ID            0 

void DeviceRemoteInit(void);

uint32_t DeviceRemoteGetKey(uint8_t* keyValue);

uint8_t* DeviceRemoteGetDespWithKey(uint8_t* keyValue);


#endif




