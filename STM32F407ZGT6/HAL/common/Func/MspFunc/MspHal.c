#include "stm32f4xx.h"

//时钟系统配置函数
//Fvco=Fs*(plln/pllm);
//SYSCLK=Fvco/pllp=Fs*(plln/(pllm*pllp));
//Fusb=Fvco/pllq=Fs*(plln/(pllm*pllq));

//Fvco:VCO频率
//SYSCLK:系统时钟频率
//Fusb:USB,SDIO,RNG等的时钟频率
//Fs:PLL输入时钟频率,可以是HSI,HSE等. 
//plln:主PLL倍频系数(PLL倍频),取值范围:64~432.
//pllm:主PLL和音频PLL分频系数(PLL之前的分频),取值范围:2~63.
//pllp:系统时钟的主PLL分频系数(PLL之后的分频),取值范围:2,4,6,8.(仅限这4个值!)
//pllq:USB/SDIO/随机数产生器等的主PLL分频系数(PLL之后的分频),取值范围:2~15.
//外部晶振为8M的时候,推荐值:plln=336,pllm=8,pllp=2,pllq=7.
//得到:Fvco=8*(336/8)=336Mhz
//     SYSCLK=336/2=168Mhz
//     Fusb=336/7=48Mhz
//返回值:0,成功;1,失败
void SystemClockInit(uint32_t plln,uint32_t pllm,uint32_t pllp,uint32_t pllq)
{
    HAL_StatusTypeDef ret = HAL_OK;
    RCC_OscInitTypeDef RCC_OscInitStructure; 
    RCC_ClkInitTypeDef RCC_ClkInitStructure;
    
    //使能PWR时钟
    __HAL_RCC_PWR_CLK_ENABLE(); 
    
    //下面这个设置用来设置调压器输出电压级别，以便在器件未以最大频率工作
    //时使性能与功耗实现平衡。
    //设置调压器输出电压级别1
    __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);
    
    //时钟源为HSE
    RCC_OscInitStructure.OscillatorType=RCC_OSCILLATORTYPE_HSE; 
    //打开HSE   
    RCC_OscInitStructure.HSEState=RCC_HSE_ON;   
    //打开PLL                   
    RCC_OscInitStructure.PLL.PLLState=RCC_PLL_ON;
    //PLL时钟源选择HSE
    RCC_OscInitStructure.PLL.PLLSource=RCC_PLLSOURCE_HSE;
    //主PLL和音频PLL分频系数(PLL之前的分频),取值范围:2~63.
    RCC_OscInitStructure.PLL.PLLM=pllm; 
    //主PLL倍频系数(PLL倍频),取值范围:64~432.  
    RCC_OscInitStructure.PLL.PLLN=plln; 
    //系统时钟的主PLL分频系数(PLL之后的分频),取值范围:2,4,6,8.(仅限这4个值!)
    RCC_OscInitStructure.PLL.PLLP=pllp; 
    //USB/SDIO/随机数产生器等的主PLL分频系数(PLL之后的分频),取值范围:2~15.
    RCC_OscInitStructure.PLL.PLLQ=pllq; 
    //初始化
    ret=HAL_RCC_OscConfig(&RCC_OscInitStructure);
	
    /**初始化失败,锁住 */
    if(ret!=HAL_OK) 
        while(1);
    
    //选中PLL作为系统时钟源并且配置HCLK,PCLK1和PCLK2
    RCC_ClkInitStructure.ClockType=(RCC_CLOCKTYPE_SYSCLK|RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2);
    //设置系统时钟时钟源为PLL
    RCC_ClkInitStructure.SYSCLKSource=RCC_SYSCLKSOURCE_PLLCLK;
    //AHB分频系数为1
    RCC_ClkInitStructure.AHBCLKDivider=RCC_SYSCLK_DIV1;
    //APB1分频系数为4
    RCC_ClkInitStructure.APB1CLKDivider=RCC_HCLK_DIV4; 
    //APB2分频系数为2
    RCC_ClkInitStructure.APB2CLKDivider=RCC_HCLK_DIV2; 
    //同时设置FLASH延时周期为5WS，也就是6个CPU周期。
    ret=HAL_RCC_ClockConfig(&RCC_ClkInitStructure,FLASH_LATENCY_5);
		
    if(ret!=HAL_OK) while(1);

	 //STM32F405x/407x/415x/417x Z版本的器件支持预取功能
	if (HAL_GetREVID() == 0x1001)
	{
        //使能flash预取
		__HAL_FLASH_PREFETCH_BUFFER_ENABLE();  
	}
}

/**系统底层初始化 */
void HAL_MspInit(void)
{
    /**初始化PLL时钟源,8/8*336/2 = 168MHZ */
    SystemClockInit(336,8,2,7);
}

/**系统底层复位 */
void HAL_MspDeInit(void)
{
    return;
}


