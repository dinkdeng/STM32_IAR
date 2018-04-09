#ifndef __CORE_SERIAL_UART1_GENERAL_H_
#define __CORE_SERIAL_UART1_GENERAL_H_
#include "stm32f4xx.h"
#include <stdio.h>


/**初始化*/
void CoreSerialUart1GeneralInit(uint32_t bpsSet);

/**发送字节*/
void CoreSerialUart1GeneralSendByte(uint8_t byteValue);

/**发送缓冲区*/
void CoreSerialUart1GeneralSendBuffer(uint8_t* bufferPtr,uint16_t length);

/**发送字符串*/
uint8_t CoreSerialUart1GeneralSendString(uint8_t* strPtr);



#endif




