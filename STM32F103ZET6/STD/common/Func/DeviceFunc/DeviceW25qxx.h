#ifndef __DEVICE_W25QXX_H_
#define __DEVICE_W25QXX_H_
#include "stm32f10x.h"

#define DEVICE_W25QXX_WAIT_MS(nms)          CoreTickDelay(nms)

/**芯片初始化 */
uint8_t DeviceW25QXXInit(void);

/**读取指定位置 */
uint8_t DeviceW25QXXRead(uint8_t* pBuffer,uint32_t readAddr,uint16_t numByteToRead);

/**写入指定位置 */
uint8_t DeviceW25QXXWrite(uint8_t* pBuffer,uint32_t writeAddr,uint16_t numByteToWrite);			



#endif




