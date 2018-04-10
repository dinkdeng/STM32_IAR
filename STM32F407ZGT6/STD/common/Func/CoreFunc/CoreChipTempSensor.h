#ifndef __CORE_CHIP_TEMP_SENSOR_H_
#define __CORE_CHIP_TEMP_SENSOR_H_
#include "stm32f4xx.h"

/**初始化传感器 */
void CoreChipTempSensorInit(void);

/**获取当前温度值 */
float CoreChipTempSensorGet(uint16_t times);


#endif




