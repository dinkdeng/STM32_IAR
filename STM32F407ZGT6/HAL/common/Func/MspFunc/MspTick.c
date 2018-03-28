#include "stm32f4xx.h"

/**系统时间,单位为毫秒 */
__IO uint32_t systemTimeMs = 0;

/**SysTick初始化*/
HAL_StatusTypeDef HAL_InitTick(uint32_t TickPriority)
{
    /**禁止中断 */
    SysTick->CTRL &= ~(SysTick_CTRL_TICKINT_Msk);
    /**设置Reload数值 */
    HAL_SYSTICK_Config(SystemCoreClock/1000U);
    /**设置中断优先级 */
    HAL_NVIC_SetPriority(SysTick_IRQn, TickPriority ,0U);
    /**启用中断 */
    SysTick->CTRL |= (SysTick_CTRL_TICKINT_Msk);
    return HAL_OK;
}

/**获取当前时钟*/
uint32_t HAL_GetTick(void)
{
    return systemTimeMs;
}

/**延时指定毫秒数 */
void HAL_Delay(__IO uint32_t Delay)
{
    uint32_t tickstart = HAL_GetTick();
    while((HAL_GetTick() - tickstart) < Delay)
    {

    }
}

/**SysTick定时器中断 */
void SysTick_Handler(void)
{
    systemTimeMs++;
}


