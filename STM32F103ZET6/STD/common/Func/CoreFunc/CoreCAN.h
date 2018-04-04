#ifndef __CORE_CAN_H_
#define __CORE_CAN_H_
#include "stm32f10x.h"

/**CAN总线速度的定义 */
typedef enum CORE_CAN_BPS
{
    CORE_CAN_500K = 0,
    CORE_CAN_250K = 1,
    CORE_CAN_125K = 2,
}CORE_CAN_BPS;

typedef enum CORE_CAN_MSG_TYPE
{
    CORE_CAN_MSG_STD = CAN_ID_STD,
    CORE_CAN_MSG_EXT = CAN_ID_EXT,
}CORE_CAN_MSG_TYPE;

/**中断函数的定义 */
typedef void (*CoreCAN_CallbackFunc)(CanRxMsg* messagePtr);

/**前置中断优先级 */
#define CORE_CAN_RX_PREE_PRI        INT_PRE_PRI_0           

/**子中断优先级 */
#define CORE_CAN_RX_SUB_PRI         INT_SUB_PRI_3

/**初始化 */
void CoreCAN_Init(CORE_CAN_BPS bps,CoreCAN_CallbackFunc callbackFunc);

/**发送数据 */
void CoreCAN_SendMessage(CORE_CAN_MSG_TYPE type,uint32_t canID,uint8_t* data,uint8_t len);


#endif





