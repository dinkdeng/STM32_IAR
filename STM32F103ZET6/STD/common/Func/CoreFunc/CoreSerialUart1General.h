#ifndef __CORE_SERIAL_UART1_GENERAL_H_
#define __CORE_SERIAL_UART1_GENERAL_H_
#include "stm32f10x.h"

/**串口初始化,不打开中断 */
void CoreSerialUart1GeneralInit(uint32_t bpsSet);

/**串口发送一个字节 */
void CoreSerialUart1GeneralSendChar(uint8_t chValue);

/**串口发送一个缓冲区 */
void CoreSerialUart1GeneralSendBuffer(uint8_t* bufferPtr,uint16_t bufferLength);

/**串口发送字符串 */
void CoreSerialUart1GeneralSendString(uint8_t* strPtr);





#endif




