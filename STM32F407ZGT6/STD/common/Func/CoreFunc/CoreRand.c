#include "CoreRand.h"

//RNG初始化
void CoreRandInit(void)
{
	//开启RNG时钟,来自PLL48CLK
	RCC_AHB2PeriphClockCmd(RCC_AHB2Periph_RNG, ENABLE);
	//使能RNG
	RNG_Cmd(ENABLE);	
	//等待随机数就绪
	while(RNG_GetFlagStatus(RNG_FLAG_DRDY)==RESET);
}

//得到随机数
uint32_t CoreRandGetNext(void)
{
    //等待随机数就绪  
	while(RNG_GetFlagStatus(RNG_FLAG_DRDY)==RESET);	
    return RNG_GetRandomNumber();
}

//生成[min,max]范围的随机数
uint32_t CoreRandGetNextByRange(uint32_t min,uint32_t max)
{
    uint32_t valueGet = CoreRandGetNext();
    /**缩小范围 */
    valueGet = (uint32_t)((valueGet*(max-min))/0XFFFFFFFF);
    if(valueGet < min)
       valueGet = min;
    if(valueGet > max)
        valueGet = max;
    return valueGet;
}



