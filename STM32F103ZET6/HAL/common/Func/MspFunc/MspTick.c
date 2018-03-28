#include "stm32f1xx.h"

__IO uint32_t systickCountMs = 0;

/**使用系统Tick定时器用作系统延时
 * 初始化为1MS一次中断,HAL库的很多函数需要使用
 */
HAL_StatusTypeDef HAL_InitTick(uint32_t TickPriority)
{
    /**禁止中断 */
    SysTick->CTRL &= ~(SysTick_CTRL_TICKINT_Msk);

    /**配置为1MS中断,默认使用HCLK,而不是八分频 */
    HAL_SYSTICK_Config(HAL_RCC_GetSysClockFreq() / 1000);

    /*Configure the SysTick IRQ priority */
    HAL_NVIC_SetPriority(SysTick_IRQn, TickPriority, 0);

    /**启用中断 */
    SysTick->CTRL |= (SysTick_CTRL_TICKINT_Msk);

    /* Return function status */
    return HAL_OK;
}

uint32_t HAL_GetTick(void)
{
    return systickCountMs;
}

void HAL_Delay(__IO uint32_t Delay)
{
    uint32_t tickstart = 0;
    tickstart = HAL_GetTick();
    while ((HAL_GetTick() - tickstart) < Delay)
    {
        
    }
}


void SysTick_Handler(void)
{
    systickCountMs++;
}