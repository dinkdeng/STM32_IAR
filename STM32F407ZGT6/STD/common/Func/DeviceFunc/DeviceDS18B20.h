#ifndef __DEVICE_DS18B20_H_
#define __DEVICE_DS18B20_H_
#include "stm32f4xx.h"

#define DeviceDS18b20DelayUs(nus)       CoreTickDelayUs(nus)

#define DeviceDS18b20DelayMs(nms)       CoreTickDelayMs(nms);

/**设备初始化 */
uint8_t DeviceDS18b20Init(void);

/**设备 读取温度值,返回的是放大十倍的温度值*/
uint8_t DeviceDS18b20ReadTemp(short* temp);


#endif




