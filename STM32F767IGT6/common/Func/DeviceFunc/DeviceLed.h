#ifndef __DEVICE_LED_H_
#define __DEVICE_LED_H_
#include "stm32f7xx.h"

/*系统LED驱动函数*/
typedef enum LED_INDEX
{
    LED_INDEX_RED = 0,
    LED_INDEX_GREEN = 1,
}LED_INDEX;

typedef enum LED_STATUS
{
    LED_STATUS_OFF = 0,
    LED_STATUS_ON = 1,
}LED_STATUS;

/*系统LED初始化*/
void DeviceLedInit(LED_STATUS initStatus);

/*设置LED状态*/
void DeviceLedSet(LED_INDEX index, LED_STATUS status);

/*获取当前LED状态*/
LED_STATUS DeviceLedGet(LED_INDEX index);

/*指定LED状态切换*/
void DeviceLedToogle(LED_INDEX index);


#endif




