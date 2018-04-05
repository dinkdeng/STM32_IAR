#ifndef __DEVICE_PWM_DAC_H_
#define __DEVICE_PWM_DAC_H_
#include "stm32f10x.h"

/**使用PWM+RC滤波来实现一个DAC */

/**初始化 */
void DevicePWM_DAC_Init(void);

/**设置输出值,实际电压乘以1000,最大3300 */
uint8_t DevicePWM_DAC_SetValue(uint16_t value);


#endif




