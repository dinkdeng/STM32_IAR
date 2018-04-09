#ifndef __CORE_SERIAL_UART1_DMA_H_
#define __CORE_SERIAL_UART1_DMA_H_
#include <stdio.h>
#include "stm32f4xx.h"

typedef void (*CoreSerialUart1DMA_CallBackFunc)(uint8_t byteValue);

#define CORE_SERIAL_UART1_DMA_PERR_PRI      INT_PRE_PRI_2

#define CORE_SERIAL_UART1_DMA_SUB_PRI       INT_SUB_PRI_0

/**初始化*/
void CoreSerialUart1DMA_Init(uint32_t bpsSet,CoreSerialUart1DMA_CallBackFunc callBackFuncPtr);

/**发送字节*/
void CoreSerialUart1DMA_SendByte(uint8_t byteValue);

/**发送缓冲区*/
void CoreSerialUart1DMA_SendBuffer(uint8_t* bufferPtr,uint16_t length);

/**发送字符串*/
uint8_t CoreSerialUart1DMA_SendString(uint8_t* strPtr);

void CoreSerialUart1DMA_DefaultCallBack(uint8_t byteValue);


#endif




