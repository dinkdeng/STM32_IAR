#ifndef __CORE_SERIAL_UART1_DMA_H_
#define __CORE_SERIAL_UART1_DMA_H_
#include "stm32f10x.h"

/**最大接收缓冲区大小 */
#define UART1_RX_MAX_BUFFER_SIZE        256

/**串口接收中断优先级 */
#define UART1_DMA_PREE_PRI              INT_PRE_PRI_0

/**串口接收中断子优先级 */
#define UART1_DMA_SUB_PRI               INT_SUB_PRI_3


/**初始化 */
void CoreSerialUart1DmaInit(uint32_t bpsSet);

/**发送字节码 */
void CoreSerialUart1DmaSendChar(uint8_t chValue);

/**发送缓冲区 */
void CoreSerialUart1DmaSendBuffer(uint8_t* bufferPtr,uint16_t bufferLength);

/**发送字符串 */
void CoreSerialUart1DmaSendString(uint8_t* strPtr);

/**清除接受缓存 */
void CoreSerialUart1DmaClearRx(void);

/**获取当前接收的一包数据 */
uint16_t CoreSerialUart1DmaGetRecv(uint8_t** recvBuffer);


#endif





