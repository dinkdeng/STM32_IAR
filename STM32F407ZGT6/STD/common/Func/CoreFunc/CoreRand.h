#ifndef __CORE_RAND_H_
#define __CORE_RAND_H_
#include "stm32f4xx.h"

//RNG初始化
void CoreRandInit(void);

//得到随机数
uint32_t CoreRandGetNext(void);

//生成[min,max]范围的随机数
uint32_t CoreRandGetNextByRange(uint32_t min,uint32_t max);



#endif




