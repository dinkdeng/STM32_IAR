#ifndef __DEVICE_LIGHT_SENSOR_H_
#define __DEVICE_LIGHT_SENSOR_H_
#include "stm32f4xx.h"

/**光敏传感器初始化*/
void DeviceLightSensorInit(void);

/**光敏传感器获取ADC的值*/
uint16_t DeviceLightSensorGetPowerful(uint8_t times);



#endif




