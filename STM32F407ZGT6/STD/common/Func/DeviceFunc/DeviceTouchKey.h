#ifndef __DEVICE_TOUCH_KEY_H_
#define __DEVICE_TOUCH_KEY_H_
#include "stm32f4xx.h"

/**按键扫描模式 */
typedef enum DEVICE_TOUCH_KEY_MODE
{
    TOUCH_KEY_MODE_SINGLE = 0,
    TOUCH_KEY_MODE_CONTINUE = 1,
}DEVICE_TOUCH_KEY_MODE;



/**按键初始化 */
uint8_t DeviceTouchKeyInit(uint8_t sensitivity);

/**按键扫描 */
uint8_t DeviceTouchKeyScan(DEVICE_TOUCH_KEY_MODE mode);


#endif




