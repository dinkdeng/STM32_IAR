/**HAL库版本1.6.0 */
#include "stm32f4xx_hal.h"


#define __STM32F4xx_HAL_VERSION_MAIN   (0x01) /*!< [31:24] main version */
#define __STM32F4xx_HAL_VERSION_SUB1   (0x06) /*!< [23:16] sub1 version */
#define __STM32F4xx_HAL_VERSION_SUB2   (0x00) /*!< [15:8]  sub2 version */
#define __STM32F4xx_HAL_VERSION_RC     (0x00) /*!< [7:0]  release candidate */
#define __STM32F4xx_HAL_VERSION         ((__STM32F4xx_HAL_VERSION_MAIN << 24U)\
                                        |(__STM32F4xx_HAL_VERSION_SUB1 << 16U)\
                                        |(__STM32F4xx_HAL_VERSION_SUB2 << 8U )\
                                        |(__STM32F4xx_HAL_VERSION_RC))

#define IDCODE_DEVID_MASK    ((uint32_t)0x00000FFFU)

/* ------------ RCC registers bit address in the alias region ----------- */
#define SYSCFG_OFFSET             (SYSCFG_BASE - PERIPH_BASE)
/* ---  MEMRMP Register ---*/
/* Alias word address of UFB_MODE bit */
#define MEMRMP_OFFSET             SYSCFG_OFFSET
#define UFB_MODE_BIT_NUMBER       POSITION_VAL(SYSCFG_MEMRMP_UFB_MODE)
#define UFB_MODE_BB               (uint32_t)(PERIPH_BB_BASE + (MEMRMP_OFFSET * 32U) + (UFB_MODE_BIT_NUMBER * 4U))

/* ---  CMPCR Register ---*/
/* Alias word address of CMP_PD bit */
#define CMPCR_OFFSET              (SYSCFG_OFFSET + 0x20U)
#define CMP_PD_BIT_NUMBER         POSITION_VAL(SYSCFG_CMPCR_CMP_PD)
#define CMPCR_CMP_PD_BB           (uint32_t)(PERIPH_BB_BASE + (CMPCR_OFFSET * 32U) + (CMP_PD_BIT_NUMBER * 4U))

/* ---  MCHDLYCR Register ---*/
/* Alias word address of BSCKSEL bit */
#define MCHDLYCR_OFFSET            (SYSCFG_OFFSET + 0x30U)
#define BSCKSEL_BIT_NUMBER         POSITION_VAL(SYSCFG_MCHDLYCR_BSCKSEL)
#define MCHDLYCR_BSCKSEL_BB        (uint32_t)(PERIPH_BB_BASE + (MCHDLYCR_OFFSET * 32U) + (BSCKSEL_BIT_NUMBER * 4U))



/**HAL库的初始化 */
HAL_StatusTypeDef HAL_Init(void)
{
  /**配置指令缓存 */
#if (INSTRUCTION_CACHE_ENABLE != 0U)
   __HAL_FLASH_INSTRUCTION_CACHE_ENABLE();
#endif /* INSTRUCTION_CACHE_ENABLE */

  /**配置数据缓存 */
#if (DATA_CACHE_ENABLE != 0U)
   __HAL_FLASH_DATA_CACHE_ENABLE();
#endif /* DATA_CACHE_ENABLE */

  /**打开Flash预取指 */
#if (PREFETCH_ENABLE != 0U)
  __HAL_FLASH_PREFETCH_BUFFER_ENABLE();
#endif /* PREFETCH_ENABLE */

  /**设置系统中断分组 */
  HAL_NVIC_SetPriorityGrouping(NVIC_PRIORITYGROUP_2);

  /**调用底层的硬件初始化程序来初始化时钟,该函数需要手动实现 */
  HAL_MspInit();

  /**使用SysTick建立一个1MS的中断*/
  HAL_InitTick(TICK_INT_PRIORITY);

  /* Return function status */
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

/**初始化系统Tick定时器,可以选择自己定义 */
__weak HAL_StatusTypeDef HAL_InitTick(uint32_t TickPriority)
{
  return HAL_OK;
}

/**获取当前时钟*/
__weak uint32_t HAL_GetTick(void)
{
  return 0;
}

/**使用HAL库来进行延时*/
__weak void HAL_Delay(__IO uint32_t Delay)
{
  return;
}

/**暂停Tick的中断 */
__weak void HAL_SuspendTick(void)
{
  /* Disable SysTick Interrupt */
  SysTick->CTRL &= ~SysTick_CTRL_TICKINT_Msk;
}

/**恢复Tick的中断 */
__weak void HAL_ResumeTick(void)
{
  /* Enable SysTick Interrupt */
  SysTick->CTRL  |= SysTick_CTRL_TICKINT_Msk;
}

/**获取当前HAL库的版本 */
uint32_t HAL_GetHalVersion(void)
{
 return __STM32F4xx_HAL_VERSION;
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
  *(__IO uint32_t *)CMPCR_CMP_PD_BB = (uint32_t)ENABLE;
}

/**
  * @brief  Power-down the I/O Compensation Cell.
  * @note   The I/O compensation cell can be used only when the device supply
  *         voltage ranges from 2.4 to 3.6 V.
  * @retval None
  */
void HAL_DisableCompensationCell(void)
{
  *(__IO uint32_t *)CMPCR_CMP_PD_BB = (uint32_t)DISABLE;
}

#if defined(STM32F427xx) || defined(STM32F437xx) || defined(STM32F429xx)|| defined(STM32F439xx) ||\
    defined(STM32F469xx) || defined(STM32F479xx)
/**
  * @brief  Enables the Internal FLASH Bank Swapping.
  *
  * @note   This function can be used only for STM32F42xxx/43xxx devices.
  *
  * @note   Flash Bank2 mapped at 0x08000000 (and aliased @0x00000000)
  *         and Flash Bank1 mapped at 0x08100000 (and aliased at 0x00100000)
  *
  * @retval None
  */
void HAL_EnableMemorySwappingBank(void)
{
  *(__IO uint32_t *)UFB_MODE_BB = (uint32_t)ENABLE;
}

/**
  * @brief  Disables the Internal FLASH Bank Swapping.
  *
  * @note   This function can be used only for STM32F42xxx/43xxx devices.
  *
  * @note   The default state : Flash Bank1 mapped at 0x08000000 (and aliased @0x00000000)
  *         and Flash Bank2 mapped at 0x08100000 (and aliased at 0x00100000)
  *
  * @retval None
  */
void HAL_DisableMemorySwappingBank(void)
{

  *(__IO uint32_t *)UFB_MODE_BB = (uint32_t)DISABLE;
}
#endif /* STM32F427xx || STM32F437xx || STM32F429xx || STM32F439xx || STM32F469xx || STM32F479xx */
