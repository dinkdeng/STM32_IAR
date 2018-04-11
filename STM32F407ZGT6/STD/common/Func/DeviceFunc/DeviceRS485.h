#ifndef __DEVICE_RS485_H_
#define __DEVICE_RS485_H_
#include "stm32f4xx.h"

typedef void (*DeviceRS485_CallBackFunc)(uint8_t recvByte);

/**接收中断优先级定义 */
#define DEVICE_RS485_RX_PREE_PRI    INT_PRE_PRI_2

#define DEVICE_RS485_RX_SUB_PRI     INT_SUB_PRI_2

/**初始化 */
void DeviceRS485_Init(uint32_t bpsSet,DeviceRS485_CallBackFunc callBackFuncPtr);

/**发送字节 */
void DeviceRS485_SendChar(uint8_t byteValue);

/**发送缓冲区 */
void DeviceRS485_SendBuffer(uint8_t* bufferPtr,uint16_t length);

/**发送字符串 */
uint8_t DeviceRS485_SendString(uint8_t* strPtr);

/**默认的回调函数 */
void DeviceRS485_DefaultCallBackFunc(uint8_t byteValue);


#endif




