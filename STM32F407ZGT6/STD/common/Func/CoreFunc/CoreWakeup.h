#ifndef __CORE_WAKEUP_H_
#define __CORE_WAKEUP_H_
#include "stm32f4xx.h"

#define CORE_WAKEUP_PREE_PRI        INT_PRE_PRI_1

#define CORE_WAKEUP_SUB_PRI         INT_SUB_PRI_0

/**初始化 */
void CoreWakeupInit(void);

/**进入待机 */
void CoreWakeUpEnterStandBy(void);





#endif





