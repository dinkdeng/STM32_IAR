#include "stm32f4xx.h"

static __IO uint32_t systemTickCountMs = 0;

/**Tick初始化函数 */
HAL_StatusTypeDef HAL_InitTick (uint32_t TickPriority)
{
    /**临时关闭中断 */
    SysTick->CTRL &= ~(SysTick_CTRL_TICKINT_Msk);
    /**更新系统时钟 */
    SystemCoreClockUpdate();
    /**按照系统当前时钟初始化定时器 */
    HAL_SYSTICK_Config(SystemCoreClock/1000U);
    /**配置中断优先级 */
    HAL_NVIC_SetPriority(SysTick_IRQn, TickPriority ,0U);
    /**完成之后打开中断 */
    SysTick->CTRL |= (SysTick_CTRL_TICKINT_Msk);
    /**返回状态 */
    return HAL_OK;
}

/**获取当前值计数值 */
uint32_t HAL_GetTick(void)
{
    return systemTickCountMs;
}

/**延时函数,单位是毫秒 */
void HAL_Delay(__IO uint32_t Delay)
{
    uint32_t tickstart = 0U;
    tickstart = HAL_GetTick();
    while((HAL_GetTick() - tickstart) < Delay)
    {

    }
}


void SysTick_Handler(void)
{
    systemTickCountMs++;
}