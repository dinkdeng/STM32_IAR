#ifndef __DEVICE_DHT11_H_
#define __DEVICE_DHT11_H_
#include "stm32f4xx.h"

#define DeviceDHT11DelayUs(nus)       CoreTickDelayUs(nus)

#define DeviceDHT11DelayMs(nms)       CoreTickDelayMs(nms);

/*驱动初始化*/
uint8_t DeviceDHT11Init(void);

/*读取数据*/
uint8_t DeviceDHT11ReadValue(uint8_t* temp, uint8_t* humi);


#endif




