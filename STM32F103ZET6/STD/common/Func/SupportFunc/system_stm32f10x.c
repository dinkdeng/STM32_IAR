#include "stm32f10x.h"
/*启动代码字节调用该函数中的SystemInit函数*/

#if defined (STM32F10X_LD_VL) || (defined STM32F10X_MD_VL) || (defined STM32F10X_HD_VL)
/* #define SYSCLK_FREQ_HSE    HSE_VALUE */
#define SYSCLK_FREQ_24MHz  24000000
#else
/*定义系统目标时钟频率*/
/* #define SYSCLK_FREQ_HSE    HSE_VALUE */
/* #define SYSCLK_FREQ_24MHz  24000000 */
/* #define SYSCLK_FREQ_36MHz  36000000 */
/* #define SYSCLK_FREQ_48MHz  48000000 */
/* #define SYSCLK_FREQ_56MHz  56000000 */
#define SYSCLK_FREQ_72MHz  72000000
#endif

/*当使用以下几种芯片的时候,可以定义数据存在外部ram还是内部ram*/
/*当定义了DATA_IN_ExtSRAM,就需要在systeminit中初始化外部sram*/
#if defined (STM32F10X_HD) || (defined STM32F10X_XL) || (defined STM32F10X_HD_VL)
/* #define DATA_IN_ExtSRAM */
#endif

/*定义中断向量表的存储位置,必须是0x200的整数倍*/
#define VECT_TAB_OFFSET  0x0

/*******************************************************************************
*  定义系统时钟频率变量
*******************************************************************************/
#ifdef SYSCLK_FREQ_HSE
uint32_t SystemCoreClock = SYSCLK_FREQ_HSE;        /*!< System Clock Frequency (Core Clock) */
#elif defined SYSCLK_FREQ_24MHz
uint32_t SystemCoreClock = SYSCLK_FREQ_24MHz;        /*!< System Clock Frequency (Core Clock) */
#elif defined SYSCLK_FREQ_36MHz
uint32_t SystemCoreClock = SYSCLK_FREQ_36MHz;        /*!< System Clock Frequency (Core Clock) */
#elif defined SYSCLK_FREQ_48MHz
uint32_t SystemCoreClock = SYSCLK_FREQ_48MHz;        /*!< System Clock Frequency (Core Clock) */
#elif defined SYSCLK_FREQ_56MHz
uint32_t SystemCoreClock = SYSCLK_FREQ_56MHz;        /*!< System Clock Frequency (Core Clock) */
#elif defined SYSCLK_FREQ_72MHz
uint32_t SystemCoreClock = SYSCLK_FREQ_72MHz;        /*!< System Clock Frequency (Core Clock) */
#else /*!< HSI Selected as System Clock source */
uint32_t SystemCoreClock = HSI_VALUE;        /*!< System Clock Frequency (Core Clock) */
#endif

/*AHB分频寄存器的值以及对应的分频系数*/
__I uint8_t AHBPrescTable[16] = { 0, 0, 0, 0, 0, 0, 0, 0, 1, 2, 3, 4, 6, 7, 8, 9 };

static void SetSysClock(void);

/*根据系统定义的时钟频率启用不同的方法*/
#ifdef SYSCLK_FREQ_HSE
static void SetSysClockToHSE(void);
#elif defined SYSCLK_FREQ_24MHz
static void SetSysClockTo24(void);
#elif defined SYSCLK_FREQ_36MHz
static void SetSysClockTo36(void);
#elif defined SYSCLK_FREQ_48MHz
static void SetSysClockTo48(void);
#elif defined SYSCLK_FREQ_56MHz
static void SetSysClockTo56(void);
#elif defined SYSCLK_FREQ_72MHz
static void SetSysClockTo72(void);
#endif

/*如果定义了外部启动,那么使能该方法*/
#ifdef DATA_IN_ExtSRAM
static void SystemInit_ExtMemCtl(void);
#endif /* DATA_IN_ExtSRAM */

/*被启动文件直接调用*/
void SystemInit(void)
{
    /*复位RCC时钟配置并重置其为初始状态 */
    /*打开HSI内部高速时钟*/
    RCC->CR |= (uint32_t)0x00000001;

    /* Reset SW, HPRE, PPRE1, PPRE2, ADCPRE and MCO bits */
#ifndef STM32F10X_CL
  /*关闭MCO 微控制器时钟输出,PCLK2 二分频作为ADC时钟,APB2从HCLK不分频,APB1从HCLK不分频*/
  /*AHB从SYSCLK不分频,选择HSI作为系统时钟*/
    RCC->CFGR &= (uint32_t)0xF8FF0000;
#else
  /*这是互联型*/
    RCC->CFGR &= (uint32_t)0xF0FF0000;
#endif /* STM32F10X_CL */

    /*PLL锁相环使能,时钟安全系统使能,外部高速晶振起振*/
    RCC->CR &= (uint32_t)0xFEF6FFFF;

    /*HSE外部高速时钟旁路设置关闭*/
    RCC->CR &= (uint32_t)0xFFFBFFFF;

    /*PLL 1.5分频作为USB时钟,PLL两倍频 HSE不分频 HSI二分频作为PLL输入时钟*/
    RCC->CFGR &= (uint32_t)0xFF80FFFF;

#ifdef STM32F10X_CL
    /* Reset PLL2ON and PLL3ON bits */
    RCC->CR &= (uint32_t)0xEBFFFFFF;

    /* Disable all interrupts and clear pending bits  */
    RCC->CIR = 0x00FF0000;

    /* Reset CFGR2 register */
    RCC->CFGR2 = 0x00000000;
#elif defined (STM32F10X_LD_VL) || defined (STM32F10X_MD_VL) || (defined STM32F10X_HD_VL)
    /*清除时钟安全系统中断,清除PLL HSE HSI LSE LSI就绪中断*/
    RCC->CIR = 0x009F0000;

    /*如果有CFGR2,就尝试清除他*/
    RCC->CFGR2 = 0x00000000;
#else
    /*清除时钟安全系统中断,清除PLL HSE HSI LSE LSI就绪中断*/
    RCC->CIR = 0x009F0000;
#endif /* STM32F10X_CL */

#if defined (STM32F10X_HD) || (defined STM32F10X_XL) || (defined STM32F10X_HD_VL)
#ifdef DATA_IN_ExtSRAM
    /*根据用户设置,决定是否启用外部SRAM初始化,一般不需要,除非你想程序跑在外部SRAM里面*/
    SystemInit_ExtMemCtl();
#endif /* DATA_IN_ExtSRAM */
#endif

    /*配置时钟的主函数*/
    SetSysClock();

    /*写入中断向量表偏移基地址*/
#ifdef VECT_TAB_SRAM
    SCB->VTOR = SRAM_BASE | VECT_TAB_OFFSET; /* Vector Table Relocation in Internal SRAM. */
#else
    SCB->VTOR = FLASH_BASE | VECT_TAB_OFFSET; /* Vector Table Relocation in Internal FLASH. */
#endif
}

/*计算并更新SystemCoreClock的值*/
void SystemCoreClockUpdate(void)
{
    uint32_t tmp = 0, pllmull = 0, pllsource = 0;

#ifdef  STM32F10X_CL
    uint32_t prediv1source = 0, prediv1factor = 0, prediv2factor = 0, pll2mull = 0;
#endif /* STM32F10X_CL */

#if defined (STM32F10X_LD_VL) || defined (STM32F10X_MD_VL) || (defined STM32F10X_HD_VL)
    uint32_t prediv1factor = 0;
#endif /* STM32F10X_LD_VL or STM32F10X_MD_VL or STM32F10X_HD_VL */

    /*获取系统当前时钟源*/
    tmp = RCC->CFGR & RCC_CFGR_SWS;

    switch (tmp)
    {
    case 0x00:
        /*使用HSI为时钟源,直接设置为HSI的宏定义*/
        SystemCoreClock = HSI_VALUE;
        break;
    case 0x04:
        /*选用HSE为时钟源,直接设置HSE的宏定义*/
        SystemCoreClock = HSE_VALUE;
        break;
    case 0x08:  /* PLL used as system clock */
      /*选择PLL时钟为系统内核时钟源pllmull设置为PLL几倍频输出,pllsource为PLL输入时钟*/
        pllmull = RCC->CFGR & RCC_CFGR_PLLMULL;
        pllsource = RCC->CFGR & RCC_CFGR_PLLSRC;

#ifndef STM32F10X_CL
        /*系统定义寄存器为0的时候,时钟倍频+2,所以要+2*/
        pllmull = (pllmull >> 18) + 2;
        /*若PLL时钟源为HSI分频2之后输入PLL*/
        if (pllsource == 0x00)
        {
            /*计算HSI时钟源经过PLL之后的时钟频率*/
            SystemCoreClock = (HSI_VALUE >> 1) * pllmull;
        }
        else
        {
#if defined (STM32F10X_LD_VL) || defined (STM32F10X_MD_VL) || (defined STM32F10X_HD_VL)
            /*选择了HSE作为时钟源*/
            prediv1factor = (RCC->CFGR2 & RCC_CFGR2_PREDIV1) + 1;
            /* HSE oscillator clock selected as PREDIV1 clock entry */
            SystemCoreClock = (HSE_VALUE / prediv1factor) * pllmull;
#else
            /*若HSE分频2,之后进入PLL*/
            if ((RCC->CFGR & RCC_CFGR_PLLXTPRE) != (uint32_t)RESET)
            {
                /*系统时钟直接计算,要除以2*/
                SystemCoreClock = (HSE_VALUE >> 1) * pllmull;
            }
            else
            {
                /*不分频进入PLL.计算PLL*/
                SystemCoreClock = HSE_VALUE * pllmull;
            }
#endif
        }
#else
        /*对于CL类型的芯片的初始化,目前还没碰到过*/
        pllmull = pllmull >> 18;

        if (pllmull != 0x0D)
        {
            pllmull += 2;
        }
        else
        { /* PLL multiplication factor = PLL input clock * 6.5 */
            pllmull = 13 / 2;
        }

        if (pllsource == 0x00)
        {
            /* HSI oscillator clock divided by 2 selected as PLL clock entry */
            SystemCoreClock = (HSI_VALUE >> 1) * pllmull;
        }
        else
        {/* PREDIV1 selected as PLL clock entry */

          /* Get PREDIV1 clock source and division factor */
            prediv1source = RCC->CFGR2 & RCC_CFGR2_PREDIV1SRC;
            prediv1factor = (RCC->CFGR2 & RCC_CFGR2_PREDIV1) + 1;

            if (prediv1source == 0)
            {
                /* HSE oscillator clock selected as PREDIV1 clock entry */
                SystemCoreClock = (HSE_VALUE / prediv1factor) * pllmull;
            }
            else
            {/* PLL2 clock selected as PREDIV1 clock entry */

              /* Get PREDIV2 division factor and PLL2 multiplication factor */
                prediv2factor = ((RCC->CFGR2 & RCC_CFGR2_PREDIV2) >> 4) + 1;
                pll2mull = ((RCC->CFGR2 & RCC_CFGR2_PLL2MUL) >> 8) + 2;
                SystemCoreClock = (((HSE_VALUE / prediv2factor) * pll2mull) / prediv1factor) * pllmull;
            }
        }
#endif /* STM32F10X_CL */
        break;

    default:
        SystemCoreClock = HSI_VALUE;
        break;
    }

    /* Compute HCLK clock frequency ----------------*/
    /* Get HCLK prescaler */
    tmp = AHBPrescTable[((RCC->CFGR & RCC_CFGR_HPRE) >> 4)];
    /* HCLK clock frequency */
    SystemCoreClock >>= tmp;
}

/*配置时钟频率,根据宏定义调用不同的函数*/
static void SetSysClock(void)
{
#ifdef SYSCLK_FREQ_HSE
    SetSysClockToHSE();
#elif defined SYSCLK_FREQ_24MHz
    SetSysClockTo24();
#elif defined SYSCLK_FREQ_36MHz
    SetSysClockTo36();
#elif defined SYSCLK_FREQ_48MHz
    SetSysClockTo48();
#elif defined SYSCLK_FREQ_56MHz
    SetSysClockTo56();
#elif defined SYSCLK_FREQ_72MHz
    SetSysClockTo72();
#endif
    /*如果以上定义没有一个被使能,那么将使用系统复位之后的默认配置*/
}


/*如果使能了外部Flash启动,那么需要实现这个函数*/
#ifdef DATA_IN_ExtSRAM

void SystemInit_ExtMemCtl(void)
{
    /*!< FSMC Bank1 NOR/SRAM3 is used for the STM3210E-EVAL, if another Bank is
      required, then adjust the Register Addresses */

      /* Enable FSMC clock */
    RCC->AHBENR = 0x00000114;

    /* Enable GPIOD, GPIOE, GPIOF and GPIOG clocks */
    RCC->APB2ENR = 0x000001E0;

    /* ---------------  SRAM Data lines, NOE and NWE configuration ---------------*/
    /*----------------  SRAM Address lines configuration -------------------------*/
    /*----------------  NOE and NWE configuration --------------------------------*/
    /*----------------  NE3 configuration ----------------------------------------*/
    /*----------------  NBL0, NBL1 configuration ---------------------------------*/

    GPIOD->CRL = 0x44BB44BB;
    GPIOD->CRH = 0xBBBBBBBB;

    GPIOE->CRL = 0xB44444BB;
    GPIOE->CRH = 0xBBBBBBBB;

    GPIOF->CRL = 0x44BBBBBB;
    GPIOF->CRH = 0xBBBB4444;

    GPIOG->CRL = 0x44BBBBBB;
    GPIOG->CRH = 0x44444B44;

    /*----------------  FSMC Configuration ---------------------------------------*/
    /*----------------  Enable FSMC Bank1_SRAM Bank ------------------------------*/

    FSMC_Bank1->BTCR[4] = 0x00001011;
    FSMC_Bank1->BTCR[5] = 0x00000200;
}
#endif /* DATA_IN_ExtSRAM */

#ifdef SYSCLK_FREQ_HSE

static void SetSysClockToHSE(void)
{
    __IO uint32_t StartUpCounter = 0, HSEStatus = 0;

    /* SYSCLK, HCLK, PCLK2 and PCLK1 configuration ---------------------------*/
    /* Enable HSE */
    RCC->CR |= ((uint32_t)RCC_CR_HSEON);

    /* Wait till HSE is ready and if Time out is reached exit */
    do
    {
        HSEStatus = RCC->CR & RCC_CR_HSERDY;
        StartUpCounter++;
    } while ((HSEStatus == 0) && (StartUpCounter != HSE_STARTUP_TIMEOUT));

    if ((RCC->CR & RCC_CR_HSERDY) != RESET)
    {
        HSEStatus = (uint32_t)0x01;
    }
    else
    {
        HSEStatus = (uint32_t)0x00;
    }

    if (HSEStatus == (uint32_t)0x01)
    {
#if !defined STM32F10X_LD_VL && !defined STM32F10X_MD_VL && !defined STM32F10X_HD_VL
        /* Enable Prefetch Buffer */
        FLASH->ACR |= FLASH_ACR_PRFTBE;

        /* Flash 0 wait state */
        FLASH->ACR &= (uint32_t)((uint32_t)~FLASH_ACR_LATENCY);

#ifndef STM32F10X_CL
        FLASH->ACR |= (uint32_t)FLASH_ACR_LATENCY_0;
#else
        if (HSE_VALUE <= 24000000)
        {
            FLASH->ACR |= (uint32_t)FLASH_ACR_LATENCY_0;
        }
        else
        {
            FLASH->ACR |= (uint32_t)FLASH_ACR_LATENCY_1;
        }
#endif /* STM32F10X_CL */
#endif

        /* HCLK = SYSCLK */
        RCC->CFGR |= (uint32_t)RCC_CFGR_HPRE_DIV1;

        /* PCLK2 = HCLK */
        RCC->CFGR |= (uint32_t)RCC_CFGR_PPRE2_DIV1;

        /* PCLK1 = HCLK */
        RCC->CFGR |= (uint32_t)RCC_CFGR_PPRE1_DIV1;

        /* Select HSE as system clock source */
        RCC->CFGR &= (uint32_t)((uint32_t)~(RCC_CFGR_SW));
        RCC->CFGR |= (uint32_t)RCC_CFGR_SW_HSE;

        /* Wait till HSE is used as system clock source */
        while ((RCC->CFGR & (uint32_t)RCC_CFGR_SWS) != (uint32_t)0x04)
        {
        }
    }
    else
    { /* If HSE fails to start-up, the application will have wrong clock
           configuration. User can add here some code to deal with this error */
    }
}
#elif defined SYSCLK_FREQ_24MHz

static void SetSysClockTo24(void)
{
    __IO uint32_t StartUpCounter = 0, HSEStatus = 0;

    /* SYSCLK, HCLK, PCLK2 and PCLK1 configuration ---------------------------*/
    /* Enable HSE */
    RCC->CR |= ((uint32_t)RCC_CR_HSEON);

    /* Wait till HSE is ready and if Time out is reached exit */
    do
    {
        HSEStatus = RCC->CR & RCC_CR_HSERDY;
        StartUpCounter++;
    } while ((HSEStatus == 0) && (StartUpCounter != HSE_STARTUP_TIMEOUT));

    if ((RCC->CR & RCC_CR_HSERDY) != RESET)
    {
        HSEStatus = (uint32_t)0x01;
    }
    else
    {
        HSEStatus = (uint32_t)0x00;
    }

    if (HSEStatus == (uint32_t)0x01)
    {
#if !defined STM32F10X_LD_VL && !defined STM32F10X_MD_VL && !defined STM32F10X_HD_VL
        /* Enable Prefetch Buffer */
        FLASH->ACR |= FLASH_ACR_PRFTBE;

        /* Flash 0 wait state */
        FLASH->ACR &= (uint32_t)((uint32_t)~FLASH_ACR_LATENCY);
        FLASH->ACR |= (uint32_t)FLASH_ACR_LATENCY_0;
#endif

        /* HCLK = SYSCLK */
        RCC->CFGR |= (uint32_t)RCC_CFGR_HPRE_DIV1;

        /* PCLK2 = HCLK */
        RCC->CFGR |= (uint32_t)RCC_CFGR_PPRE2_DIV1;

        /* PCLK1 = HCLK */
        RCC->CFGR |= (uint32_t)RCC_CFGR_PPRE1_DIV1;

#ifdef STM32F10X_CL
        /* Configure PLLs ------------------------------------------------------*/
        /* PLL configuration: PLLCLK = PREDIV1 * 6 = 24 MHz */
        RCC->CFGR &= (uint32_t)~(RCC_CFGR_PLLXTPRE | RCC_CFGR_PLLSRC | RCC_CFGR_PLLMULL);
        RCC->CFGR |= (uint32_t)(RCC_CFGR_PLLXTPRE_PREDIV1 | RCC_CFGR_PLLSRC_PREDIV1 |
            RCC_CFGR_PLLMULL6);

        /* PLL2 configuration: PLL2CLK = (HSE / 5) * 8 = 40 MHz */
        /* PREDIV1 configuration: PREDIV1CLK = PLL2 / 10 = 4 MHz */
        RCC->CFGR2 &= (uint32_t)~(RCC_CFGR2_PREDIV2 | RCC_CFGR2_PLL2MUL |
            RCC_CFGR2_PREDIV1 | RCC_CFGR2_PREDIV1SRC);
        RCC->CFGR2 |= (uint32_t)(RCC_CFGR2_PREDIV2_DIV5 | RCC_CFGR2_PLL2MUL8 |
            RCC_CFGR2_PREDIV1SRC_PLL2 | RCC_CFGR2_PREDIV1_DIV10);

        /* Enable PLL2 */
        RCC->CR |= RCC_CR_PLL2ON;
        /* Wait till PLL2 is ready */
        while ((RCC->CR & RCC_CR_PLL2RDY) == 0)
        {
        }
#elif defined (STM32F10X_LD_VL) || defined (STM32F10X_MD_VL) || defined (STM32F10X_HD_VL)
        /*  PLL configuration:  = (HSE / 2) * 6 = 24 MHz */
        RCC->CFGR &= (uint32_t)((uint32_t)~(RCC_CFGR_PLLSRC | RCC_CFGR_PLLXTPRE | RCC_CFGR_PLLMULL));
        RCC->CFGR |= (uint32_t)(RCC_CFGR_PLLSRC_PREDIV1 | RCC_CFGR_PLLXTPRE_PREDIV1_Div2 | RCC_CFGR_PLLMULL6);
#else
        /*  PLL configuration:  = (HSE / 2) * 6 = 24 MHz */
        RCC->CFGR &= (uint32_t)((uint32_t)~(RCC_CFGR_PLLSRC | RCC_CFGR_PLLXTPRE | RCC_CFGR_PLLMULL));
        RCC->CFGR |= (uint32_t)(RCC_CFGR_PLLSRC_HSE | RCC_CFGR_PLLXTPRE_HSE_Div2 | RCC_CFGR_PLLMULL6);
#endif /* STM32F10X_CL */

        /* Enable PLL */
        RCC->CR |= RCC_CR_PLLON;

        /* Wait till PLL is ready */
        while ((RCC->CR & RCC_CR_PLLRDY) == 0)
        {
        }

        /* Select PLL as system clock source */
        RCC->CFGR &= (uint32_t)((uint32_t)~(RCC_CFGR_SW));
        RCC->CFGR |= (uint32_t)RCC_CFGR_SW_PLL;

        /* Wait till PLL is used as system clock source */
        while ((RCC->CFGR & (uint32_t)RCC_CFGR_SWS) != (uint32_t)0x08)
        {
        }
    }
    else
    { /* If HSE fails to start-up, the application will have wrong clock
           configuration. User can add here some code to deal with this error */
    }
}
#elif defined SYSCLK_FREQ_36MHz

static void SetSysClockTo36(void)
{
    __IO uint32_t StartUpCounter = 0, HSEStatus = 0;

    /* SYSCLK, HCLK, PCLK2 and PCLK1 configuration ---------------------------*/
    /* Enable HSE */
    RCC->CR |= ((uint32_t)RCC_CR_HSEON);

    /* Wait till HSE is ready and if Time out is reached exit */
    do
    {
        HSEStatus = RCC->CR & RCC_CR_HSERDY;
        StartUpCounter++;
    } while ((HSEStatus == 0) && (StartUpCounter != HSE_STARTUP_TIMEOUT));

    if ((RCC->CR & RCC_CR_HSERDY) != RESET)
    {
        HSEStatus = (uint32_t)0x01;
    }
    else
    {
        HSEStatus = (uint32_t)0x00;
    }

    if (HSEStatus == (uint32_t)0x01)
    {
        /* Enable Prefetch Buffer */
        FLASH->ACR |= FLASH_ACR_PRFTBE;

        /* Flash 1 wait state */
        FLASH->ACR &= (uint32_t)((uint32_t)~FLASH_ACR_LATENCY);
        FLASH->ACR |= (uint32_t)FLASH_ACR_LATENCY_1;

        /* HCLK = SYSCLK */
        RCC->CFGR |= (uint32_t)RCC_CFGR_HPRE_DIV1;

        /* PCLK2 = HCLK */
        RCC->CFGR |= (uint32_t)RCC_CFGR_PPRE2_DIV1;

        /* PCLK1 = HCLK */
        RCC->CFGR |= (uint32_t)RCC_CFGR_PPRE1_DIV1;

#ifdef STM32F10X_CL
        /* Configure PLLs ------------------------------------------------------*/

        /* PLL configuration: PLLCLK = PREDIV1 * 9 = 36 MHz */
        RCC->CFGR &= (uint32_t)~(RCC_CFGR_PLLXTPRE | RCC_CFGR_PLLSRC | RCC_CFGR_PLLMULL);
        RCC->CFGR |= (uint32_t)(RCC_CFGR_PLLXTPRE_PREDIV1 | RCC_CFGR_PLLSRC_PREDIV1 |
            RCC_CFGR_PLLMULL9);

        /*!< PLL2 configuration: PLL2CLK = (HSE / 5) * 8 = 40 MHz */
        /* PREDIV1 configuration: PREDIV1CLK = PLL2 / 10 = 4 MHz */

        RCC->CFGR2 &= (uint32_t)~(RCC_CFGR2_PREDIV2 | RCC_CFGR2_PLL2MUL |
            RCC_CFGR2_PREDIV1 | RCC_CFGR2_PREDIV1SRC);
        RCC->CFGR2 |= (uint32_t)(RCC_CFGR2_PREDIV2_DIV5 | RCC_CFGR2_PLL2MUL8 |
            RCC_CFGR2_PREDIV1SRC_PLL2 | RCC_CFGR2_PREDIV1_DIV10);

        /* Enable PLL2 */
        RCC->CR |= RCC_CR_PLL2ON;
        /* Wait till PLL2 is ready */
        while ((RCC->CR & RCC_CR_PLL2RDY) == 0)
        {
        }

#else
        /*  PLL configuration: PLLCLK = (HSE / 2) * 9 = 36 MHz */
        RCC->CFGR &= (uint32_t)((uint32_t)~(RCC_CFGR_PLLSRC | RCC_CFGR_PLLXTPRE | RCC_CFGR_PLLMULL));
        RCC->CFGR |= (uint32_t)(RCC_CFGR_PLLSRC_HSE | RCC_CFGR_PLLXTPRE_HSE_Div2 | RCC_CFGR_PLLMULL9);
#endif /* STM32F10X_CL */

        /* Enable PLL */
        RCC->CR |= RCC_CR_PLLON;

        /* Wait till PLL is ready */
        while ((RCC->CR & RCC_CR_PLLRDY) == 0)
        {
        }

        /* Select PLL as system clock source */
        RCC->CFGR &= (uint32_t)((uint32_t)~(RCC_CFGR_SW));
        RCC->CFGR |= (uint32_t)RCC_CFGR_SW_PLL;

        /* Wait till PLL is used as system clock source */
        while ((RCC->CFGR & (uint32_t)RCC_CFGR_SWS) != (uint32_t)0x08)
        {
        }
    }
    else
    { /* If HSE fails to start-up, the application will have wrong clock
           configuration. User can add here some code to deal with this error */
    }
}
#elif defined SYSCLK_FREQ_48MHz

static void SetSysClockTo48(void)
{
    __IO uint32_t StartUpCounter = 0, HSEStatus = 0;

    /* SYSCLK, HCLK, PCLK2 and PCLK1 configuration ---------------------------*/
    /* Enable HSE */
    RCC->CR |= ((uint32_t)RCC_CR_HSEON);

    /* Wait till HSE is ready and if Time out is reached exit */
    do
    {
        HSEStatus = RCC->CR & RCC_CR_HSERDY;
        StartUpCounter++;
    } while ((HSEStatus == 0) && (StartUpCounter != HSE_STARTUP_TIMEOUT));

    if ((RCC->CR & RCC_CR_HSERDY) != RESET)
    {
        HSEStatus = (uint32_t)0x01;
    }
    else
    {
        HSEStatus = (uint32_t)0x00;
    }

    if (HSEStatus == (uint32_t)0x01)
    {
        /* Enable Prefetch Buffer */
        FLASH->ACR |= FLASH_ACR_PRFTBE;

        /* Flash 1 wait state */
        FLASH->ACR &= (uint32_t)((uint32_t)~FLASH_ACR_LATENCY);
        FLASH->ACR |= (uint32_t)FLASH_ACR_LATENCY_1;

        /* HCLK = SYSCLK */
        RCC->CFGR |= (uint32_t)RCC_CFGR_HPRE_DIV1;

        /* PCLK2 = HCLK */
        RCC->CFGR |= (uint32_t)RCC_CFGR_PPRE2_DIV1;

        /* PCLK1 = HCLK */
        RCC->CFGR |= (uint32_t)RCC_CFGR_PPRE1_DIV2;

#ifdef STM32F10X_CL
        /* Configure PLLs ------------------------------------------------------*/
        /* PLL2 configuration: PLL2CLK = (HSE / 5) * 8 = 40 MHz */
        /* PREDIV1 configuration: PREDIV1CLK = PLL2 / 5 = 8 MHz */

        RCC->CFGR2 &= (uint32_t)~(RCC_CFGR2_PREDIV2 | RCC_CFGR2_PLL2MUL |
            RCC_CFGR2_PREDIV1 | RCC_CFGR2_PREDIV1SRC);
        RCC->CFGR2 |= (uint32_t)(RCC_CFGR2_PREDIV2_DIV5 | RCC_CFGR2_PLL2MUL8 |
            RCC_CFGR2_PREDIV1SRC_PLL2 | RCC_CFGR2_PREDIV1_DIV5);

        /* Enable PLL2 */
        RCC->CR |= RCC_CR_PLL2ON;
        /* Wait till PLL2 is ready */
        while ((RCC->CR & RCC_CR_PLL2RDY) == 0)
        {
        }

        /* PLL configuration: PLLCLK = PREDIV1 * 6 = 48 MHz */
        RCC->CFGR &= (uint32_t)~(RCC_CFGR_PLLXTPRE | RCC_CFGR_PLLSRC | RCC_CFGR_PLLMULL);
        RCC->CFGR |= (uint32_t)(RCC_CFGR_PLLXTPRE_PREDIV1 | RCC_CFGR_PLLSRC_PREDIV1 |
            RCC_CFGR_PLLMULL6);
#else
        /*  PLL configuration: PLLCLK = HSE * 6 = 48 MHz */
        RCC->CFGR &= (uint32_t)((uint32_t)~(RCC_CFGR_PLLSRC | RCC_CFGR_PLLXTPRE | RCC_CFGR_PLLMULL));
        RCC->CFGR |= (uint32_t)(RCC_CFGR_PLLSRC_HSE | RCC_CFGR_PLLMULL6);
#endif /* STM32F10X_CL */

        /* Enable PLL */
        RCC->CR |= RCC_CR_PLLON;

        /* Wait till PLL is ready */
        while ((RCC->CR & RCC_CR_PLLRDY) == 0)
        {
        }

        /* Select PLL as system clock source */
        RCC->CFGR &= (uint32_t)((uint32_t)~(RCC_CFGR_SW));
        RCC->CFGR |= (uint32_t)RCC_CFGR_SW_PLL;

        /* Wait till PLL is used as system clock source */
        while ((RCC->CFGR & (uint32_t)RCC_CFGR_SWS) != (uint32_t)0x08)
        {
        }
    }
    else
    { /* If HSE fails to start-up, the application will have wrong clock
           configuration. User can add here some code to deal with this error */
    }
}

#elif defined SYSCLK_FREQ_56MHz

static void SetSysClockTo56(void)
{
    __IO uint32_t StartUpCounter = 0, HSEStatus = 0;

    /* SYSCLK, HCLK, PCLK2 and PCLK1 configuration ---------------------------*/
    /* Enable HSE */
    RCC->CR |= ((uint32_t)RCC_CR_HSEON);

    /* Wait till HSE is ready and if Time out is reached exit */
    do
    {
        HSEStatus = RCC->CR & RCC_CR_HSERDY;
        StartUpCounter++;
    } while ((HSEStatus == 0) && (StartUpCounter != HSE_STARTUP_TIMEOUT));

    if ((RCC->CR & RCC_CR_HSERDY) != RESET)
    {
        HSEStatus = (uint32_t)0x01;
    }
    else
    {
        HSEStatus = (uint32_t)0x00;
    }

    if (HSEStatus == (uint32_t)0x01)
    {
        /* Enable Prefetch Buffer */
        FLASH->ACR |= FLASH_ACR_PRFTBE;

        /* Flash 2 wait state */
        FLASH->ACR &= (uint32_t)((uint32_t)~FLASH_ACR_LATENCY);
        FLASH->ACR |= (uint32_t)FLASH_ACR_LATENCY_2;

        /* HCLK = SYSCLK */
        RCC->CFGR |= (uint32_t)RCC_CFGR_HPRE_DIV1;

        /* PCLK2 = HCLK */
        RCC->CFGR |= (uint32_t)RCC_CFGR_PPRE2_DIV1;

        /* PCLK1 = HCLK */
        RCC->CFGR |= (uint32_t)RCC_CFGR_PPRE1_DIV2;

#ifdef STM32F10X_CL
        /* Configure PLLs ------------------------------------------------------*/
        /* PLL2 configuration: PLL2CLK = (HSE / 5) * 8 = 40 MHz */
        /* PREDIV1 configuration: PREDIV1CLK = PLL2 / 5 = 8 MHz */

        RCC->CFGR2 &= (uint32_t)~(RCC_CFGR2_PREDIV2 | RCC_CFGR2_PLL2MUL |
            RCC_CFGR2_PREDIV1 | RCC_CFGR2_PREDIV1SRC);
        RCC->CFGR2 |= (uint32_t)(RCC_CFGR2_PREDIV2_DIV5 | RCC_CFGR2_PLL2MUL8 |
            RCC_CFGR2_PREDIV1SRC_PLL2 | RCC_CFGR2_PREDIV1_DIV5);

        /* Enable PLL2 */
        RCC->CR |= RCC_CR_PLL2ON;
        /* Wait till PLL2 is ready */
        while ((RCC->CR & RCC_CR_PLL2RDY) == 0)
        {
        }

        /* PLL configuration: PLLCLK = PREDIV1 * 7 = 56 MHz */
        RCC->CFGR &= (uint32_t)~(RCC_CFGR_PLLXTPRE | RCC_CFGR_PLLSRC | RCC_CFGR_PLLMULL);
        RCC->CFGR |= (uint32_t)(RCC_CFGR_PLLXTPRE_PREDIV1 | RCC_CFGR_PLLSRC_PREDIV1 |
            RCC_CFGR_PLLMULL7);
#else
        /* PLL configuration: PLLCLK = HSE * 7 = 56 MHz */
        RCC->CFGR &= (uint32_t)((uint32_t)~(RCC_CFGR_PLLSRC | RCC_CFGR_PLLXTPRE | RCC_CFGR_PLLMULL));
        RCC->CFGR |= (uint32_t)(RCC_CFGR_PLLSRC_HSE | RCC_CFGR_PLLMULL7);

#endif /* STM32F10X_CL */

        /* Enable PLL */
        RCC->CR |= RCC_CR_PLLON;

        /* Wait till PLL is ready */
        while ((RCC->CR & RCC_CR_PLLRDY) == 0)
        {
        }

        /* Select PLL as system clock source */
        RCC->CFGR &= (uint32_t)((uint32_t)~(RCC_CFGR_SW));
        RCC->CFGR |= (uint32_t)RCC_CFGR_SW_PLL;

        /* Wait till PLL is used as system clock source */
        while ((RCC->CFGR & (uint32_t)RCC_CFGR_SWS) != (uint32_t)0x08)
        {
        }
    }
    else
    { /* If HSE fails to start-up, the application will have wrong clock
           configuration. User can add here some code to deal with this error */
    }
}

#elif defined SYSCLK_FREQ_72MHz

/*系统时钟设置为72M的时候的调用的函数*/
static void SetSysClockTo72(void)
{
    __IO uint32_t StartUpCounter = 0, HSEStatus = 0;

    /* Enable HSE 外部高速时钟晶振起震*/
    RCC->CR |= ((uint32_t)RCC_CR_HSEON);

    /*等待时钟起震稳定*/
    do
    {
        HSEStatus = RCC->CR & RCC_CR_HSERDY;
        StartUpCounter++;
    } while ((HSEStatus == 0) && (StartUpCounter != HSE_STARTUP_TIMEOUT));

    if ((RCC->CR & RCC_CR_HSERDY) != RESET)
    {
        HSEStatus = (uint32_t)0x01;
    }
    else
    {
        HSEStatus = (uint32_t)0x00;
    }
    /*时钟稳定起震*/
    if (HSEStatus == (uint32_t)0x01)
    {
        /*打开flash预取指*/
        FLASH->ACR |= FLASH_ACR_PRFTBE;

        /*Flash等待周期设置为2*/
        FLASH->ACR &= (uint32_t)((uint32_t)~FLASH_ACR_LATENCY);
        FLASH->ACR |= (uint32_t)FLASH_ACR_LATENCY_2;

        /* HCLK = SYSCLK 设置HCLK分频数*/
        RCC->CFGR |= (uint32_t)RCC_CFGR_HPRE_DIV1;

        /* PCLK2 = HCLK 设置APB2分频数*/
        RCC->CFGR |= (uint32_t)RCC_CFGR_PPRE2_DIV1;

        /* PCLK1 = HCLK/2 设置APB1分频数*/
        RCC->CFGR |= (uint32_t)RCC_CFGR_PPRE1_DIV2;

#ifdef STM32F10X_CL
        /* Configure PLLs ------------------------------------------------------*/
        /* PLL2 configuration: PLL2CLK = (HSE / 5) * 8 = 40 MHz */
        /* PREDIV1 configuration: PREDIV1CLK = PLL2 / 5 = 8 MHz */

        RCC->CFGR2 &= (uint32_t)~(RCC_CFGR2_PREDIV2 | RCC_CFGR2_PLL2MUL |
            RCC_CFGR2_PREDIV1 | RCC_CFGR2_PREDIV1SRC);
        RCC->CFGR2 |= (uint32_t)(RCC_CFGR2_PREDIV2_DIV5 | RCC_CFGR2_PLL2MUL8 |
            RCC_CFGR2_PREDIV1SRC_PLL2 | RCC_CFGR2_PREDIV1_DIV5);

        /* Enable PLL2 */
        RCC->CR |= RCC_CR_PLL2ON;
        /* Wait till PLL2 is ready */
        while ((RCC->CR & RCC_CR_PLL2RDY) == 0)
        {
        }

        /* PLL configuration: PLLCLK = PREDIV1 * 9 = 72 MHz */
        RCC->CFGR &= (uint32_t)~(RCC_CFGR_PLLXTPRE | RCC_CFGR_PLLSRC | RCC_CFGR_PLLMULL);
        RCC->CFGR |= (uint32_t)(RCC_CFGR_PLLXTPRE_PREDIV1 | RCC_CFGR_PLLSRC_PREDIV1 |
            RCC_CFGR_PLLMULL9);
#else
        /*  PLL configuration: PLLCLK = HSE * 9 = 72 MHz */
        RCC->CFGR &= (uint32_t)((uint32_t)~(RCC_CFGR_PLLSRC | RCC_CFGR_PLLXTPRE |
            RCC_CFGR_PLLMULL));
        RCC->CFGR |= (uint32_t)(RCC_CFGR_PLLSRC_HSE | RCC_CFGR_PLLMULL9);
#endif /* STM32F10X_CL */

        /* Enable PLL */
        RCC->CR |= RCC_CR_PLLON;

        /* Wait till PLL is ready */
        while ((RCC->CR & RCC_CR_PLLRDY) == 0)
        {
        }

        /* Select PLL as system clock source */
        RCC->CFGR &= (uint32_t)((uint32_t)~(RCC_CFGR_SW));
        RCC->CFGR |= (uint32_t)RCC_CFGR_SW_PLL;

        /* Wait till PLL is used as system clock source */
        while ((RCC->CFGR & (uint32_t)RCC_CFGR_SWS) != (uint32_t)0x08)
        {
        }
    }
    else
    {
        /*外部HSE起震失败,在此处死循环吧*/
        while (1);
    }
}
#endif