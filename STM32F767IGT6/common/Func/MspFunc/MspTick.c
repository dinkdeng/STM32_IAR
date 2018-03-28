#include "stm32f7xx.h"

__IO uint32_t systickCountMs = 0;

HAL_StatusTypeDef HAL_InitTick(uint32_t TickPriority)
{
    /**临时关闭中断 */
    SysTick->CTRL &= ~(SysTick_CTRL_TICKINT_Msk);
    /**重新计算内核时钟 */
    SystemCoreClockUpdate();
    /**配置Tick系统,使用HCLK作为时钟源,计时1MS中断 */
    HAL_SYSTICK_Config(SystemCoreClock/1000);
    /**设置中断 */
    HAL_NVIC_SetPriority(SysTick_IRQn, TickPriority ,0);
    /**完成之后打开中断 */
    SysTick->CTRL |= (SysTick_CTRL_TICKINT_Msk);
    /**返回结果状态 */
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
  while((HAL_GetTick() - tickstart) < Delay)
  {
    
  }
}


/**滴答定时器中断 */
void SysTick_Handler(void)
{
  systickCountMs++;
}
