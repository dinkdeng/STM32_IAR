#ifndef __CORE_SERIAL_UART1_INT_H_
#define __CORE_SERIAL_UART1_INT_H_
#include "stm32f4xx.h"
#include <stdio.h>

typedef void (*CoreSerialUart1IntCallBackFunc)(uint8_t byteValue);

#define CORE_SERIAL_UART1_PERR_PRI      INT_PRE_PRI_2

#define CORE_SERIAL_UART1_SUB_PRI       INT_SUB_PRI_0

/**初始化函数*/
void CoreSerialUart1IntInit(uint32_t bpsSet,CoreSerialUart1IntCallBackFunc callBackFuncPtr);

/**发送字节*/
void CoreSerialUart1IntSendByte(uint8_t byteValue);

/**发送缓冲区*/
void CoreSerialUart1IntSendBuffer(uint8_t* bufferPtr,uint16_t length);

/**发送字符串*/
uint8_t CoreSerialUart1IntSendString(uint8_t* strPtr);

void CoreSerialUart1IntDefaultCallBack(uint8_t byteValue);



#endif





