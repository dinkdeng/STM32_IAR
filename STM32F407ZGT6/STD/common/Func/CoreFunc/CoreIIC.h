#ifndef __CORE_IIC_H_
#define __CORE_IIC_H_
#include "stm32f4xx.h"

/**系统获取ACK的结果 */
typedef enum IIC_WAIT_ACK
{
    IIC_WAIT_ACK_OK = 0,
    IIC_WAIT_ACK_FAIL = 1,
}IIC_WAIT_ACK;

/**IIC平台延时接口*/
#define CORE_IIC_WAIT_MS(nms)               CoreTickDelayMs(nms)

/**iic平台延时接口 */
#define CORE_IIC_WAIT_US(nus)               CoreTickDelayUs(nus)      

/**IIC平台等待ACK响应的最长等待时间 */
#define IIC_WAIT_ACK_TIME_MAX_COUNT         25000

/**IIC接口初始化 */
void CoreIIC_Init(void);

/**IIC发送start信号 */
void CoreIIC_SendStart(void);

/**发送Stop信号 */
void CoreIIC_SendStop(void);

/**发送一个字节的数据 */
void CoreIIC_SendByte(uint8_t byteValue);

/**接收一个字节的数据 */
uint8_t CoreIIC_RecvByte(void);

/**等待ACK */
IIC_WAIT_ACK CoreIIC_WaitACK(void);

/**发送NO ACK */
void CoreIIC_SendNACK(void);

/**发送ACK */
void CoreIIC_SendACK(void);


#endif





