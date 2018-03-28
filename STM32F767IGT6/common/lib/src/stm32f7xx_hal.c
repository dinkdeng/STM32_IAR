#include "stm32f7xx_hal.h"
/**1.2.0版本 */

#define __STM32F7xx_HAL_VERSION_MAIN   (0x01) /*!< [31:24] main version */
#define __STM32F7xx_HAL_VERSION_SUB1   (0x02) /*!< [23:16] sub1 version */
#define __STM32F7xx_HAL_VERSION_SUB2   (0x00) /*!< [15:8]  sub2 version */
#define __STM32F7xx_HAL_VERSION_RC     (0x00) /*!< [7:0]  release candidate */
#define __STM32F7xx_HAL_VERSION         ((__STM32F7xx_HAL_VERSION_MAIN << 24)\
                                        |(__STM32F7xx_HAL_VERSION_SUB1 << 16)\
                                        |(__STM32F7xx_HAL_VERSION_SUB2 << 8 )\
                                        |(__STM32F7xx_HAL_VERSION_RC))

#define IDCODE_DEVID_MASK    ((uint32_t)0x00000FFF)

HAL_StatusTypeDef HAL_Init(void)
{
  /**使能ART加速器 */
#if (ART_ACCLERATOR_ENABLE != 0)
   __HAL_FLASH_ART_ENABLE();
#endif

  /**设置系统中断分组 */
  HAL_NVIC_SetPriorityGrouping(NVIC_PRIORITYGROUP_2);

  /**初始化TICK定时器用作HAL库计时基准 */
  HAL_InitTick(TICK_INT_PRIORITY);

  /**初始化底层硬件,主要是初始化时钟系统,缓存系统 */
  HAL_MspInit();

  /**返回初始化状态 */
  return HAL_OK;
}

/**
  * @brief  This function de-Initializes common part of the HAL and stops the systick.
  *         This function is optional.
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_DeInit(void)
{
  /* Reset of all peripherals */
  __HAL_RCC_APB1_FORCE_RESET();
  __HAL_RCC_APB1_RELEASE_RESET();

  __HAL_RCC_APB2_FORCE_RESET();
  __HAL_RCC_APB2_RELEASE_RESET();

  __HAL_RCC_AHB1_FORCE_RESET();
  __HAL_RCC_AHB1_RELEASE_RESET();

  __HAL_RCC_AHB2_FORCE_RESET();
  __HAL_RCC_AHB2_RELEASE_RESET();

  __HAL_RCC_AHB3_FORCE_RESET();
  __HAL_RCC_AHB3_RELEASE_RESET();

  /* De-Init the low level hardware */
  HAL_MspDeInit();

  /* Return function status */
  return HAL_OK;
}

/**
  * @brief  Initializes the MSP.
  * @retval None
  */
__weak void HAL_MspInit(void)
{
  /* NOTE : This function Should not be modified, when the callback is needed,
            the HAL_MspInit could be implemented in the user file
   */
}

/**
  * @brief  DeInitializes the MSP.
  * @retval None
  */
__weak void HAL_MspDeInit(void)
{
  /* NOTE : This function Should not be modified, when the callback is needed,
            the HAL_MspDeInit could be implemented in the user file
   */
}

/**
  * @brief This function configures the source of the time base.
  *        The time source is configured  to have 1ms time base with a dedicated
  *        Tick interrupt priority.
  * @note This function is called  automatically at the beginning of program after
  *       reset by HAL_Init() or at any time when clock is reconfigured  by HAL_RCC_ClockConfig().
  * @note In the default implementation, SysTick timer is the source of time base.
  *       It is used to generate interrupts at regular time intervals.
  *       Care must be taken if HAL_Delay() is called from a peripheral ISR process,
  *       The the SysTick interrupt must have higher priority (numerically lower)
  *       than the peripheral interrupt. Otherwise the caller ISR process will be blocked.
  *       The function is declared as __weak  to be overwritten  in case of other
  *       implementation  in user file.
  * @param TickPriority: Tick interrupt priority.
  * @retval HAL status
  */
__weak HAL_StatusTypeDef HAL_InitTick(uint32_t TickPriority)
{
  /*Configure the SysTick to have interrupt in 1ms time basis*/
  HAL_SYSTICK_Config(SystemCoreClock/1000);

  /*Configure the SysTick IRQ priority */
  HAL_NVIC_SetPriority(SysTick_IRQn, TickPriority ,0);

  /* Return function status */
  return HAL_OK;
}


/**
  * @brief Provides a tick value in millisecond.
  * @note This function is declared as __weak to be overwritten in case of other
  *       implementations in user file.
  * @retval tick value
  */
__weak uint32_t HAL_GetTick(void)
{
  return 0;
}

/**
  * @brief This function provides accurate delay (in milliseconds) based
  *        on variable incremented.
  * @note In the default implementation , SysTick timer is the source of time base.
  *       It is used to generate interrupts at regular time intervals where uwTick
  *       is incremented.
  * @note This function is declared as __weak to be overwritten in case of other
  *       implementations in user file.
  * @param Delay: specifies the delay time length, in milliseconds.
  * @retval None
  */
__weak void HAL_Delay(__IO uint32_t Delay)
{

}

/**
  * @brief  Returns the HAL revision
  * @retval version : 0xXYZR (8bits for each decimal, R for RC)
  */
uint32_t HAL_GetHalVersion(void)
{
 return __STM32F7xx_HAL_VERSION;
}

/**
  * @brief  Returns the device revision identifier.
  * @retval Device revision identifier
  */
uint32_t HAL_GetREVID(void)
{
   return((DBGMCU->IDCODE) >> 16U);
}

/**
  * @brief  Returns the device identifier.
  * @retval Device identifier
  */
uint32_t HAL_GetDEVID(void)
{
   return((DBGMCU->IDCODE) & IDCODE_DEVID_MASK);
}

/**
  * @brief  Enable the Debug Module during SLEEP mode
  * @retval None
  */
void HAL_DBGMCU_EnableDBGSleepMode(void)
{
  SET_BIT(DBGMCU->CR, DBGMCU_CR_DBG_SLEEP);
}

/**
  * @brief  Disable the Debug Module during SLEEP mode
  * @retval None
  */
void HAL_DBGMCU_DisableDBGSleepMode(void)
{
  CLEAR_BIT(DBGMCU->CR, DBGMCU_CR_DBG_SLEEP);
}

/**
  * @brief  Enable the Debug Module during STOP mode
  * @retval None
  */
void HAL_DBGMCU_EnableDBGStopMode(void)
{
  SET_BIT(DBGMCU->CR, DBGMCU_CR_DBG_STOP);
}

/**
  * @brief  Disable the Debug Module during STOP mode
  * @retval None
  */
void HAL_DBGMCU_DisableDBGStopMode(void)
{
  CLEAR_BIT(DBGMCU->CR, DBGMCU_CR_DBG_STOP);
}

/**
  * @brief  Enable the Debug Module during STANDBY mode
  * @retval None
  */
void HAL_DBGMCU_EnableDBGStandbyMode(void)
{
  SET_BIT(DBGMCU->CR, DBGMCU_CR_DBG_STANDBY);
}

/**
  * @brief  Disable the Debug Module during STANDBY mode
  * @retval None
  */
void HAL_DBGMCU_DisableDBGStandbyMode(void)
{
  CLEAR_BIT(DBGMCU->CR, DBGMCU_CR_DBG_STANDBY);
}

/**
  * @brief  Enables the I/O Compensation Cell.
  * @note   The I/O compensation cell can be used only when the device supply
  *         voltage ranges from 2.4 to 3.6 V.
  * @retval None
  */
void HAL_EnableCompensationCell(void)
{
  SYSCFG->CMPCR |= SYSCFG_CMPCR_CMP_PD;
}

/**
  * @brief  Power-down the I/O Compensation Cell.
  * @note   The I/O compensation cell can be used only when the device supply
  *         voltage ranges from 2.4 to 3.6 V.
  * @retval None
  */
void HAL_DisableCompensationCell(void)
{
  SYSCFG->CMPCR &= (uint32_t)~((uint32_t)SYSCFG_CMPCR_CMP_PD);
}

/**
  * @brief  Enables the FMC Memory Mapping Swapping.
  *
  * @note   SDRAM is accessible at 0x60000000
  *         and NOR/RAM is accessible at 0xC0000000
  *
  * @retval None
  */
void HAL_EnableFMCMemorySwapping(void)
{
  SYSCFG->MEMRMP |= SYSCFG_MEMRMP_SWP_FMC_0;
}

/**
  * @brief  Disables the FMC Memory Mapping Swapping
  *
  * @note   SDRAM is accessible at 0xC0000000 (default mapping)
  *         and NOR/RAM is accessible at 0x60000000 (default mapping)
  *
  * @retval None
  */
void HAL_DisableFMCMemorySwapping(void)
{

  SYSCFG->MEMRMP &= (uint32_t)~((uint32_t)SYSCFG_MEMRMP_SWP_FMC);
}

#if defined (STM32F765xx) || defined (STM32F767xx) || defined (STM32F769xx) || defined (STM32F777xx) || defined (STM32F779xx)
/**
* @brief  Enable the Internal FLASH Bank Swapping.
*
* @note   This function can be used only for STM32F77xx/STM32F76xx devices.
*
* @note   Flash Bank2 mapped at 0x08000000 (AXI) (aliased at 0x00200000 (TCM))
*         and Flash Bank1 mapped at 0x08100000 (AXI) (aliased at 0x00300000 (TCM))
*
* @retval None
*/
void HAL_EnableMemorySwappingBank(void)
{
  SET_BIT(SYSCFG->MEMRMP, SYSCFG_MEMRMP_SWP_FB);
}

/**
* @brief  Disable the Internal FLASH Bank Swapping.
*
* @note   This function can be used only for STM32F77xx/STM32F76xx devices.
*
* @note   The default state : Flash Bank1 mapped at 0x08000000 (AXI) (aliased at 0x00200000 (TCM))
*         and Flash Bank2 mapped at 0x08100000 (AXI)( aliased at 0x00300000 (TCM))
*
* @retval None
*/
void HAL_DisableMemorySwappingBank(void)
{
  CLEAR_BIT(SYSCFG->MEMRMP, SYSCFG_MEMRMP_SWP_FB);
}
#endif /* STM32F767xx || STM32F769xx || STM32F777xx || STM32F779xx */

/**
  * @}
  */

/**
  * @}
  */

/**
  * @}
  */

/**
  * @}
  */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
