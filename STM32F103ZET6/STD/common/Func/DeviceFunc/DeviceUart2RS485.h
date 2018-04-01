#ifndef __DEVICE_UART2_RS485_H_
#define __DEVICE_UART2_RS485_H_
#include "stm32f10x.h"

#define DEVICE_UART2_RS485_BUFFER_MAX   255

#define DEVICE_UART2_RS485_PREE_PRI     INT_PRE_PRI_0

#define DEVICE_UART2_RS485_SUB_PRI      INT_SUB_PRI_1

/**485接口初始化 */
void DeviceUart2RS485Init(uint32_t bps);

/**发送一个字节 */
void DeviceUart2RS485SendChar(uint8_t chValue);

/**发送一个缓冲区 */
void DeviceUart2RS485SendBuffer(uint8_t* bufferPtr,uint16_t bufferLength);

/**发送一个字符串 */
void DeviceUart2RS485SendString(uint8_t* strPtr);

/**获取接收到的缓存 */
uint16_t DeviceUart2RS485GetRecv(uint8_t** recvBufferPtr);

/**清除接收缓冲 */
void DeviceUart2RS485ClearRecv(void);


#endif




