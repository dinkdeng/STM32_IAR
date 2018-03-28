#include "stm32f1xx_hal.h"

/** @addtogroup STM32F1xx_HAL_Driver
  * @{
  */

  /** @defgroup HAL HAL
    * @brief HAL module driver.
    * @{
    */

#ifdef HAL_MODULE_ENABLED

    /* Private typedef -----------------------------------------------------------*/
    /* Private define ------------------------------------------------------------*/

    /** @defgroup HAL_Private_Constants HAL Private Constants
      * @{
      */

      /**
       * @brief STM32F1xx HAL Driver version number
         */
#define __STM32F1xx_HAL_VERSION_MAIN   (0x01) /*!< [31:24] main version */
#define __STM32F1xx_HAL_VERSION_SUB1   (0x00) /*!< [23:16] sub1 version */
#define __STM32F1xx_HAL_VERSION_SUB2   (0x04) /*!< [15:8]  sub2 version */
#define __STM32F1xx_HAL_VERSION_RC     (0x00) /*!< [7:0]  release candidate */
#define __STM32F1xx_HAL_VERSION         ((__STM32F1xx_HAL_VERSION_MAIN << 24)\
                                        |(__STM32F1xx_HAL_VERSION_SUB1 << 16)\
                                        |(__STM32F1xx_HAL_VERSION_SUB2 << 8 )\
                                        |(__STM32F1xx_HAL_VERSION_RC))

#define IDCODE_DEVID_MASK    ((uint32_t)0x00000FFF)

         /**
           * @}
           */

           /* Private macro -------------------------------------------------------------*/
           /* Private variables ---------------------------------------------------------*/

           /** @defgroup HAL_Private_Variables HAL Private Variables
             * @{
             */

static __IO uint32_t uwTick;

/**
  * @}
  */

  /* Private function prototypes -----------------------------------------------*/
  /* Exported functions ---------------------------------------------------------*/

  /** @defgroup HAL_Exported_Functions HAL Exported Functions
    * @{
    */

    /** @defgroup HAL_Exported_Functions_Group1 Initialization and de-initialization Functions
     *  @brief    Initialization and de-initialization functions
     *
    @verbatim
     ===============================================================================
                  ##### Initialization and de-initialization functions #####
     ===============================================================================
       [..]  This section provides functions allowing to:
          (+) Initializes the Flash interface, the NVIC allocation and initial clock
              configuration. It initializes the source of time base also when timeout
              is needed and the backup domain when enabled.
          (+) de-Initializes common part of the HAL.
          (+) Configure The time base source to have 1ms time base with a dedicated
              Tick interrupt priority.
            (++) Systick timer is used by default as source of time base, but user
                 can eventually implement his proper time base source (a general purpose
                 timer for example or other time source), keeping in mind that Time base
                 duration should be kept 1ms since PPP_TIMEOUT_VALUEs are defined and
                 handled in milliseconds basis.
            (++) Time base configuration function (HAL_InitTick ()) is called automatically
                 at the beginning of the program after reset by HAL_Init() or at any time
                 when clock is configured, by HAL_RCC_ClockConfig().
            (++) Source of time base is configured  to generate interrupts at regular
                 time intervals. Care must be taken if HAL_Delay() is called from a
                 peripheral ISR process, the Tick interrupt line must have higher priority
                (numerically lower) than the peripheral interrupt. Otherwise the caller
                ISR process will be blocked.
           (++) functions affecting time base configurations are declared as __Weak
                 to make  override possible  in case of other  implementations in user file.

    @endverbatim
      * @{
      */

      /**
        * @brief This function configures the Flash prefetch,
        *        Configures time base source, NVIC and Low level hardware
        * @note This function is called at the beginning of program after reset and before
        *       the clock configuration
        * @note The time base configuration is based on MSI clock when exiting from Reset.
        *       Once done, time base tick start incrementing.
        *        In the default implementation,Systick is used as source of time base.
        *       The tick variable is incremented each 1ms in its ISR.
        * @retval HAL status
        */
HAL_StatusTypeDef HAL_Init(void)
{
    /* Configure Flash prefetch */
    /*根据conf里面的配置确定是否使能flash预取指*/
#if (PREFETCH_ENABLE != 0)
#if defined(STM32F101x6) || defined(STM32F101xB) || defined(STM32F101xE) || defined(STM32F101xG) || \
    defined(STM32F102x6) || defined(STM32F102xB) || \
    defined(STM32F103x6) || defined(STM32F103xB) || defined(STM32F103xE) || defined(STM32F103xG) || \
    defined(STM32F105xC) || defined(STM32F107xC)

  /* Prefetch buffer is not available on value line devices */
    /*打开flash预取指缓冲区,但是这个函数没有设置预取指时间延迟*/
    __HAL_FLASH_PREFETCH_BUFFER_ENABLE();
#endif
#endif /* PREFETCH_ENABLE */

    /* Set Interrupt Group Priority */
    /*设置系统中断优先级分组*/
    HAL_NVIC_SetPriorityGrouping(USER_SYSTEM_INT_GROUP);

    /* Use systick as time base source and configure 1ms tick (default clock after Reset is MSI) */
    HAL_InitTick(TICK_INT_PRIORITY);

    /* Init the low level hardware */
    /*HAL的底层硬件初始化*/
    HAL_MspInit();

    /* Return function status */
    return HAL_OK;
}

/**
  * @brief This function de-Initializes common part of the HAL and stops the source
  *        of time base.
  * @note This function is optional.
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_DeInit(void)
{
    /* Reset of all peripherals */
    __HAL_RCC_APB1_FORCE_RESET();
    __HAL_RCC_APB1_RELEASE_RESET();

    __HAL_RCC_APB2_FORCE_RESET();
    __HAL_RCC_APB2_RELEASE_RESET();

#if defined(STM32F105xC) || defined(STM32F107xC)
    __HAL_RCC_AHB_FORCE_RESET();
    __HAL_RCC_AHB_RELEASE_RESET();
#endif

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
  *       The function is declared as __Weak  to be overwritten  in case of other
  *       implementation  in user file.
  * @param TickPriority: Tick interrupt priority.
  * @retval HAL status
  */
__weak HAL_StatusTypeDef HAL_InitTick(uint32_t TickPriority)
{
    /*Configure the SysTick to have interrupt in 1ms time basis*/
    HAL_SYSTICK_Config(HAL_RCC_GetSysClockFreq() / 1000);

    /*Configure the SysTick IRQ priority */
    HAL_NVIC_SetPriority(SysTick_IRQn, TickPriority, 0);

    /* Return function status */
    return HAL_OK;
}

/**
  * @brief Provides a tick value in millisecond.
  * @note   This function is declared as __weak  to be overwritten  in case of other
  *       implementations in user file.
  * @retval tick value
  */
__weak uint32_t HAL_GetTick(void)
{
    return uwTick;
}

/**
  * @brief This function provides accurate delay (in milliseconds) based
  *        on variable incremented.
  * @note In the default implementation , SysTick timer is the source of time base.
  *       It is used to generate interrupts at regular time intervals where uwTick
  *       is incremented.
  * @note ThiS function is declared as __weak to be overwritten in case of other
  *       implementations in user file.
  * @param Delay: specifies the delay time length, in milliseconds.
  * @retval None
  */
__weak void HAL_Delay(__IO uint32_t Delay)
{
    uint32_t tickstart = 0;
    tickstart = HAL_GetTick();
    while ((HAL_GetTick() - tickstart) < Delay)
    {
    }
}

/**
  * @brief  This method returns the HAL revision
  * @retval version: 0xXYZR (8bits for each decimal, R for RC)
  */
uint32_t HAL_GetHalVersion(void)
{
    return __STM32F1xx_HAL_VERSION;
}

/**
  * @brief Returns the device revision identifier.
  * Note: On devices STM32F10xx8 and STM32F10xxB,
  *                  STM32F101xC/D/E and STM32F103xC/D/E,
  *                  STM32F101xF/G and STM32F103xF/G
  *                  STM32F10xx4 and STM32F10xx6
  *       Debug registers DBGMCU_IDCODE and DBGMCU_CR are accessible only in
  *       debug mode (not accessible by the user software in normal mode).
  *       Refer to errata sheet of these devices for more details.
  * @retval Device revision identifier
  */
uint32_t HAL_GetREVID(void)
{
    return((DBGMCU->IDCODE) >> POSITION_VAL(DBGMCU_IDCODE_REV_ID));
}

/**
  * @brief  Returns the device identifier.
  * Note: On devices STM32F10xx8 and STM32F10xxB,
  *                  STM32F101xC/D/E and STM32F103xC/D/E,
  *                  STM32F101xF/G and STM32F103xF/G
  *                  STM32F10xx4 and STM32F10xx6
  *       Debug registers DBGMCU_IDCODE and DBGMCU_CR are accessible only in
  *       debug mode (not accessible by the user software in normal mode).
  *       Refer to errata sheet of these devices for more details.
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
  * Note: On devices STM32F10xx8 and STM32F10xxB,
  *                  STM32F101xC/D/E and STM32F103xC/D/E,
  *                  STM32F101xF/G and STM32F103xF/G
  *                  STM32F10xx4 and STM32F10xx6
  *       Debug registers DBGMCU_IDCODE and DBGMCU_CR are accessible only in
  *       debug mode (not accessible by the user software in normal mode).
  *       Refer to errata sheet of these devices for more details.
  * @retval None
  */
void HAL_DBGMCU_DisableDBGSleepMode(void)
{
    CLEAR_BIT(DBGMCU->CR, DBGMCU_CR_DBG_SLEEP);
}

/**
  * @brief  Enable the Debug Module during STOP mode
  * Note: On devices STM32F10xx8 and STM32F10xxB,
  *                  STM32F101xC/D/E and STM32F103xC/D/E,
  *                  STM32F101xF/G and STM32F103xF/G
  *                  STM32F10xx4 and STM32F10xx6
  *       Debug registers DBGMCU_IDCODE and DBGMCU_CR are accessible only in
  *       debug mode (not accessible by the user software in normal mode).
  *       Refer to errata sheet of these devices for more details.
  * Note: On all STM32F1 devices:
  *       If the system tick timer interrupt is enabled during the Stop mode
  *       debug (DBG_STOP bit set in the DBGMCU_CR register ), it will wakeup
  *       the system from Stop mode.
  *       Workaround: To debug the Stop mode, disable the system tick timer
  *       interrupt.
  *       Refer to errata sheet of these devices for more details.
  * Note: On all STM32F1 devices:
  *       If the system tick timer interrupt is enabled during the Stop mode
  *       debug (DBG_STOP bit set in the DBGMCU_CR register ), it will wakeup
  *       the system from Stop mode.
  *       Workaround: To debug the Stop mode, disable the system tick timer
  *       interrupt.
  *       Refer to errata sheet of these devices for more details.
  * @retval None
  */
void HAL_DBGMCU_EnableDBGStopMode(void)
{
    SET_BIT(DBGMCU->CR, DBGMCU_CR_DBG_STOP);
}

/**
  * @brief  Disable the Debug Module during STOP mode
  * Note: On devices STM32F10xx8 and STM32F10xxB,
  *                  STM32F101xC/D/E and STM32F103xC/D/E,
  *                  STM32F101xF/G and STM32F103xF/G
  *                  STM32F10xx4 and STM32F10xx6
  *       Debug registers DBGMCU_IDCODE and DBGMCU_CR are accessible only in
  *       debug mode (not accessible by the user software in normal mode).
  *       Refer to errata sheet of these devices for more details.
  * @retval None
  */
void HAL_DBGMCU_DisableDBGStopMode(void)
{
    CLEAR_BIT(DBGMCU->CR, DBGMCU_CR_DBG_STOP);
}

/**
  * @brief  Enable the Debug Module during STANDBY mode
  * Note: On devices STM32F10xx8 and STM32F10xxB,
  *                  STM32F101xC/D/E and STM32F103xC/D/E,
  *                  STM32F101xF/G and STM32F103xF/G
  *                  STM32F10xx4 and STM32F10xx6
  *       Debug registers DBGMCU_IDCODE and DBGMCU_CR are accessible only in
  *       debug mode (not accessible by the user software in normal mode).
  *       Refer to errata sheet of these devices for more details.
  * @retval None
  */
void HAL_DBGMCU_EnableDBGStandbyMode(void)
{
    SET_BIT(DBGMCU->CR, DBGMCU_CR_DBG_STANDBY);
}

/**
  * @brief  Disable the Debug Module during STANDBY mode
  * Note: On devices STM32F10xx8 and STM32F10xxB,
  *                  STM32F101xC/D/E and STM32F103xC/D/E,
  *                  STM32F101xF/G and STM32F103xF/G
  *                  STM32F10xx4 and STM32F10xx6
  *       Debug registers DBGMCU_IDCODE and DBGMCU_CR are accessible only in
  *       debug mode (not accessible by the user software in normal mode).
  *       Refer to errata sheet of these devices for more details.
  * @retval None
  */
void HAL_DBGMCU_DisableDBGStandbyMode(void)
{
    CLEAR_BIT(DBGMCU->CR, DBGMCU_CR_DBG_STANDBY);
}

/**
  * @}
  */

  /**
    * @}
    */

#endif /* HAL_MODULE_ENABLED */
    /**
      * @}
      */

      /**
        * @}
        */

        /************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
