#ifndef __DEVICE_IR_H_
#define __DEVICE_IR_H_
#include "stm32f10x.h"

/**设备接收优先级定义 */
#define DEVICE_IR_RECV_PREE_PRI     INT_PRE_PRI_1

#define DEVICE_IR_RECV_SUB_PRI      INT_SUB_PRI_0

//红外遥控识别码(ID),每款遥控器的该值基本都不一样,但也有一样的.
//我们选用的遥控器识别码为0
#define REMOTE_ID       0 

void DeviceIR_Init(void);

//通过按键值获取按键标识字符串
uint8_t* DeviceIR_GetDesp(uint8_t keyValue);

uint8_t DeviceIR_GetKeyValue(uint8_t* keyValue);





#endif




