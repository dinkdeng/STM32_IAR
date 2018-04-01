#ifndef __CORE_ITEMP_H_
#define __CORE_ITEMP_H_
#include "stm32f10x.h"

/**初始化传感器 */
void CoreITEMPInit(void);

/**获取当前温度值 */
float CoreITEMPGet(uint16_t times);


#endif




