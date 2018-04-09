#ifndef __DEVICE_EXTI_H_
#define __DEVICE_EXTI_H_
#include "stm32f4xx.h"

/**外部中断序号枚举 */
typedef enum EXTI_INDEX
{
    EXTI_LEFT = 0,
    EXTI_RIGHT = 1,
    EXTI_UP = 2,
    EXTI_DOWN = 3,
}EXTI_INDEX;

/**按键中断优先级 */
#define EXTI_LEFT_PREE_PRI       INT_PRE_PRI_1
#define EXTI_LEFT_SUB_PRI        INT_SUB_PRI_3

#define EXTI_RIGHT_PREE_PRI      INT_PRE_PRI_1
#define EXTI_RIGHT_SUB_PRI       INT_SUB_PRI_3

#define EXTI_UP_PREE_PRI         INT_PRE_PRI_1
#define EXTI_UP_SUB_PRI          INT_SUB_PRI_3

#define EXTI_DOWN_PREE_PRI       INT_PRE_PRI_1
#define EXTI_DOWN_SUB_PRI        INT_SUB_PRI_3


/**外部中断初始化 */
void DeviceExtiInit(EXTI_INDEX index);

/**获取中断触发次数 */
uint16_t DeviceExtiGetCount(EXTI_INDEX index);

/**清除中断触发次数 */
void DeviceExtiClear(EXTI_INDEX index);


#endif





