#ifndef __DEVICE_BEEP_H_
#define __DEVICE_BEEP_H_
#include "stm32f10x.h"

/**蜂鸣器状态 */
typedef enum BEEP_STATUS
{
    BEEP_OFF = 0,
    BEEP_ON = 1,
}BEEP_STATUS;

/**蜂鸣器初始化 */
void DeviceBeepInit(BEEP_STATUS initStatus);

/**设置蜂鸣器状态 */
void DeviceBeepSet(BEEP_STATUS setStatus);

#endif




