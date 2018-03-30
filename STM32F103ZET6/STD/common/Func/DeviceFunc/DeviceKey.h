#ifndef __DEVICE_KEY_H_
#define __DEVICE_KEY_H_
#include "stm32f10x.h"

/**系统外部按键驱动程序 */

/**按键序列号定义 */
typedef enum KEY_INDEX
{
    KEY_LEFT = 0,
    KEY_RIGHT = 1,
    KEY_UP = 2,
    KEY_DOWN = 3,
}KEY_INDEX;

/**按键状态定义 */
typedef enum KEY_STATUS
{
    KEY_RELEASE = 0,
    KEY_PRESS = 1,
}KEY_STATUS;

/**按键延时消抖函数 */
#define DeviceWaitKeyStable()       CoreTickDelayMs(10)

/**系统按键扫描初始化 */
void DeviceKeyInit(void);

/**扫描系统按键,返回是否有按键被触发*/
KEY_STATUS DeviceKeyRead(KEY_INDEX index);





#endif



