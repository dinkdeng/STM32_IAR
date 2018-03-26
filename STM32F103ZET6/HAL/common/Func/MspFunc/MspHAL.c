#include "stm32f1xx_hal.h"



/**
* Fn:  void SetSystemClockHSETo72M(void)
*
* Summary: 设置系统时钟增加到72M.
*
* Author:  Dink.
*
* Date:    2017/9/8.
*/

void SetSystemClockHSETo72M(void)
{
    /*RCC 时钟源结构*/
    RCC_OscInitTypeDef RCC_OscInitStructure = { 0 };
    /*RCC 时钟初始化结构*/
    RCC_ClkInitTypeDef RCC_ClkInitStructure = { 0 };
    /*使能电源接口时钟,稍微等待一段时间等待电源接口使能*/
    __HAL_RCC_PWR_CLK_ENABLE();

    /*起震HSI*/
    RCC_OscInitStructure.OscillatorType = RCC_OSCILLATORTYPE_HSI;
    /*打开HSI*/
    RCC_OscInitStructure.HSIState = RCC_HSI_ON;
    /*不配置PLL相关外设*/
    RCC_OscInitStructure.PLL.PLLState = RCC_PLL_NONE;
    if (HAL_RCC_OscConfig(&RCC_OscInitStructure) != HAL_OK)
    {
        /*设置失败,进入死循环*/
        while (1);
    }

    /*首先选择HSI作为系统时钟,这样就可以修改PLL了*/
    RCC_ClkInitStructure.ClockType = RCC_CLOCKTYPE_SYSCLK;
    RCC_ClkInitStructure.SYSCLKSource = RCC_SYSCLKSOURCE_HSI;
    /*使用8M系统时钟,处于0-24M之间,等待周期设置为0*/
    if (HAL_RCC_ClockConfig(&RCC_ClkInitStructure, FLASH_LATENCY_0) != HAL_OK)
    {
        /*设置失败,进入死循环*/
        while (1);
    }

    /* -2- 使能 HSE  时钟, 选择他作为PLL时钟并等待他启动 */
    /*选择HSE起震*/
    RCC_OscInitStructure.OscillatorType = RCC_OSCILLATORTYPE_HSE;
    RCC_OscInitStructure.HSEState = RCC_HSE_ON;

    /*PLL时钟源为HSE*/
    RCC_OscInitStructure.PLL.PLLSource = RCC_PLLSOURCE_HSE;
    RCC_OscInitStructure.PLL.PLLState = RCC_PLL_ON;
    /*PLL时钟源为HSE不分频,也就是8M*/
    RCC_OscInitStructure.HSEPredivValue = RCC_HSE_PREDIV_DIV1;
    /*PLL时钟倍频系数9,8*9 = 72M,PLL输出72M*/
    RCC_OscInitStructure.PLL.PLLMUL = RCC_PLL_MUL9;
    /*设置PLL锁相环*/
    if (HAL_RCC_OscConfig(&RCC_OscInitStructure) != HAL_OK)
    {
        /*设置失败,进入死循环*/
        while (1);
    }

    /* -3- 选择PLL作为系统时钟源 配置 HCLK, PCLK1 and PCLK2 分频 */
    RCC_ClkInitStructure.ClockType = (RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2);
    /*选择系统时钟源72M*/
    RCC_ClkInitStructure.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
    /*AHB不分频,也是72M*/
    RCC_ClkInitStructure.AHBCLKDivider = RCC_SYSCLK_DIV1;
    /*低速APB二分频 36M*/
    RCC_ClkInitStructure.APB1CLKDivider = RCC_HCLK_DIV2;
    /*高速APB不分频,72M*/
    RCC_ClkInitStructure.APB2CLKDivider = RCC_HCLK_DIV1;
    /*72M区间,flash等待周期应该设置为2*/
    if (HAL_RCC_ClockConfig(&RCC_ClkInitStructure, FLASH_LATENCY_2) != HAL_OK)
    {
        /*设置失败,进入死循环*/
        while (1);
    }
    /*默认预取指使能*/
    __HAL_FLASH_PREFETCH_BUFFER_ENABLE();

    /* 可选操作: 如果系统不再需要HSI时钟,那么禁用HSI时钟*/
    /*注意,Flash操作必须打开HSI*/
    /*注意,如果配置了这一段语句,那么重入该函数切换HSI时钟的时候会因为HSI时钟被禁用而导致失败*/
    RCC_OscInitStructure.OscillatorType = RCC_OSCILLATORTYPE_HSI;
    /*关闭HSI*/
    RCC_OscInitStructure.HSIState = RCC_HSI_OFF;
    /*不配置PLL相关外设*/
    RCC_OscInitStructure.PLL.PLLState = RCC_PLL_NONE;
    if (HAL_RCC_OscConfig(&RCC_OscInitStructure) != HAL_OK)
    {
        /*设置失败,进入死循环*/
        while (1);
    }
}

/**
* Fn:  void SetSystemClockHSITo72M(void)
*
* Summary: Sets system clock hsi to 72 m.
*
* Author:  Dink.
*
* Date:    2017/9/8.
*/

void SetSystemClockHSITo72M(void)
{
    RCC_ClkInitTypeDef RCC_ClkInitStruct = { 0 };
    RCC_OscInitTypeDef RCC_OscInitStruct = { 0 };

    RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
    /*打开HSE*/
    RCC_OscInitStruct.HSEState = RCC_HSE_ON;
    RCC_OscInitStruct.PLL.PLLState = RCC_PLL_NONE;
    if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
    {
        /*设置失败,进入死循环*/
        while (1);
    }

    /* 选择HSE作为系统时钟,这样就可以修改PLL了 */
    RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_SYSCLK;
    /*输入振荡器选择HSE*/
    RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_HSE;
    if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_0) != HAL_OK)
    {
        /*设置失败,进入死循环*/
        while (1);
    }

    /* -2- 使能HSI, 选择HSI作为PLL时钟源并启动PLL */
    /*使能HSI*/
    RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
    RCC_OscInitStruct.HSIState = RCC_HSI_ON;
    /*HSI校正系数为默认值*/
    RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
    /*打开PLL*/
    RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
    /*HSI分频2,输出PLL 4M*/
    RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI_DIV2;
    /*PLL倍频系数16,4*16 64M*/
    RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL16;
    if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
    {
        /*设置失败,进入死循环*/
        while (1);
    }

    /* -3- 选择PLL输出作为系统时钟并设置 HCLK, PCLK1 and PCLK2 的分频系数 */
    RCC_ClkInitStruct.ClockType = (RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2);
    /*选择PLL作为系统时钟源*/
    RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
    /*AHB 64M*/
    RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
    /*APB1 32M*/
    RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
    /*APB2 64M*/
    RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;
    if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK)
    {
        /*设置失败,进入死循环*/
        while (1);
    }

    /* -4- 可选操作,关闭HSE振荡器 */
    RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
    /*关闭HSE*/
    RCC_OscInitStruct.HSEState = RCC_HSE_OFF;
    RCC_OscInitStruct.PLL.PLLState = RCC_PLL_NONE;
    if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
    {
        /*设置失败,进入死循环*/
        while (1);
    }
}

/**
* @brief  Initializes the Global MSP.
* 该函数是HAL_Init函数调用的,完成系统预取指缓冲区打开,中断分组设置以及可能的systick中断设置后,程序会运行到这里.
* 我会在这个地方设置系统时钟和设置正常的flash预取指周期
* @param  None
* @retval None
*/
void HAL_MspInit(void)
{
    /*设置系统时钟,HSE 72M coreClk*/
    SetSystemClockHSETo72M();
    /*更新系统时钟*/
    SystemCoreClockUpdate();
}

/**
* @brief  DeInitializes the Global MSP.
* @param  None
* @retval None
*/
void HAL_MspDeInit(void)
{

}