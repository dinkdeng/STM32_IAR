#ifndef __DEVICE_KEY_H_
#define __DEVICE_KEY_H_
#include "stm32f4xx.h"

/*系统外部按键驱动文件*/

typedef enum KEY_INDEX
{
    KEY_LEFT = 0,
    KEY_RIGHT = 1,
    KEY_UP = 2,
    KEY_DOWN = 3,
}KEY_INDEX;

typedef enum KEY_STATUS
{
    KEY_RELEASE = 0,
    KEY_PRESS = 1,
}KEY_STATUS;

/*按键使用的延时函数*/
#define DEVICE_KEY_DELAY_MS(nms)        CoreTickDelayMs(nms)
/*等待按键按下稳定的等待时间,单位毫秒*/
#define DEVICE_WAIT_PRESS_STABLE_MS     10

void DeviceKeyInit(void);

KEY_STATUS DeviceKeyRead(KEY_INDEX index);




#endif



