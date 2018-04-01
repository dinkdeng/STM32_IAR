#include "DeviceSRAM.h"

/**设备挂载的SRAM初始化 */
uint8_t DeviceSRAMInit(void)
{
    FSMC_NORSRAMInitTypeDef  FSMC_NORSRAMInitStructure;
    FSMC_NORSRAMTimingInitTypeDef  readWriteTiming;
    GPIO_InitTypeDef  GPIO_InitStructure;

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOD | RCC_APB2Periph_GPIOE | RCC_APB2Periph_GPIOF | RCC_APB2Periph_GPIOG, ENABLE);
    RCC_AHBPeriphClockCmd(RCC_AHBPeriph_FSMC, ENABLE);

    //PORTD复用推挽输出 
    GPIO_InitStructure.GPIO_Pin = 0xFF33;
    //复用推挽输出
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOD, &GPIO_InitStructure);

    //PORTE复用推挽输出 
    GPIO_InitStructure.GPIO_Pin = 0xFF83;
    GPIO_Init(GPIOE, &GPIO_InitStructure);
    //PORTD复用推挽输出 
    GPIO_InitStructure.GPIO_Pin = 0xF03F;
    GPIO_Init(GPIOF, &GPIO_InitStructure);
    //PORTD复用推挽输出 
    GPIO_InitStructure.GPIO_Pin = 0x043F;
    GPIO_Init(GPIOG, &GPIO_InitStructure);

    //地址建立时间（ADDSET）为1个HCLK 1/36M=27ns		  
    readWriteTiming.FSMC_AddressSetupTime = 0x00;
    //地址保持时间（ADDHLD）模式A未用到
    readWriteTiming.FSMC_AddressHoldTime = 0x00;
    //数据保持时间（DATAST）为3个HCLK 4/72M=55ns(对EM的SRAM芯片)
    readWriteTiming.FSMC_DataSetupTime = 0x03;
    readWriteTiming.FSMC_BusTurnAroundDuration = 0x00;
    readWriteTiming.FSMC_CLKDivision = 0x00;
    readWriteTiming.FSMC_DataLatency = 0x00;
    //模式A
    readWriteTiming.FSMC_AccessMode = FSMC_AccessMode_A;

    //  这里我们使用NE3 ，也就对应BTCR[4],[5]。
    FSMC_NORSRAMInitStructure.FSMC_Bank = FSMC_Bank1_NORSRAM3;
    FSMC_NORSRAMInitStructure.FSMC_DataAddressMux = FSMC_DataAddressMux_Disable;
    // FSMC_MemoryType_SRAM;  //SRAM 
    FSMC_NORSRAMInitStructure.FSMC_MemoryType = FSMC_MemoryType_SRAM;
    //存储器数据宽度为16bit 
    FSMC_NORSRAMInitStructure.FSMC_MemoryDataWidth = FSMC_MemoryDataWidth_16b;
    // FSMC_BurstAccessMode_Disable;  访问加速模式被禁止
    FSMC_NORSRAMInitStructure.FSMC_BurstAccessMode = FSMC_BurstAccessMode_Disable;
    FSMC_NORSRAMInitStructure.FSMC_WaitSignalPolarity = FSMC_WaitSignalPolarity_Low;
    FSMC_NORSRAMInitStructure.FSMC_AsynchronousWait = FSMC_AsynchronousWait_Disable;
    FSMC_NORSRAMInitStructure.FSMC_WrapMode = FSMC_WrapMode_Disable;
    FSMC_NORSRAMInitStructure.FSMC_WaitSignalActive = FSMC_WaitSignalActive_BeforeWaitState;
    //存储器写使能 
    FSMC_NORSRAMInitStructure.FSMC_WriteOperation = FSMC_WriteOperation_Enable;
    FSMC_NORSRAMInitStructure.FSMC_WaitSignal = FSMC_WaitSignal_Disable;
    // 读写使用相同的时序
    FSMC_NORSRAMInitStructure.FSMC_ExtendedMode = FSMC_ExtendedMode_Disable;
    FSMC_NORSRAMInitStructure.FSMC_WriteBurst = FSMC_WriteBurst_Disable;
    FSMC_NORSRAMInitStructure.FSMC_ReadWriteTimingStruct = &readWriteTiming;
    //读写同样时序
    FSMC_NORSRAMInitStructure.FSMC_WriteTimingStruct = &readWriteTiming;
    //初始化FSMC配置
    FSMC_NORSRAMInit(&FSMC_NORSRAMInitStructure);
    // 使能BANK3
    FSMC_NORSRAMCmd(FSMC_Bank1_NORSRAM3, ENABLE);

    return DeviceSRAMTest(1024);
}

static uint8_t DeviceSRAMRead(uint32_t baseAddr, uint8_t* bufferPtr,
    uint32_t bufferLength)
{
    if (baseAddr < DEVICE_SRAM_BASE_ADDR || baseAddr >(DEVICE_SRAM_BASE_ADDR + DEVICE_SRAM_LENGTH))
        return 1;
    if ((baseAddr + bufferLength-1) > (DEVICE_SRAM_BASE_ADDR + DEVICE_SRAM_LENGTH))
        return 2;
    for (; bufferLength != 0; bufferLength--)
    {
        *bufferPtr++ = *(volatile uint8_t*)(baseAddr);
        baseAddr++;
    }
    return 0;
}

static uint8_t DeviceSRAMWrite(uint32_t baseAddr, uint8_t* bufferPtr,
    uint32_t bufferLength)
{
    if (baseAddr < DEVICE_SRAM_BASE_ADDR || baseAddr >(DEVICE_SRAM_BASE_ADDR + DEVICE_SRAM_LENGTH))
        return 1;
    if ((baseAddr + bufferLength) > (DEVICE_SRAM_BASE_ADDR + DEVICE_SRAM_LENGTH))
        return 2;
    for (; bufferLength != 0; bufferLength--)
    {
        *(volatile uint8_t*)(baseAddr) = *bufferPtr;
        baseAddr++;
        bufferPtr++;
    }
    return 0;
}

/**设备挂载的SRAM测试 */
uint8_t DeviceSRAMTest(uint16_t testStep)
{
    uint32_t i = 0;
    uint8_t temp = 0;
    /*在地址0读到的数据*/
    uint8_t sval = 0;
    //每隔4K字节,写入一个数据,总共写入256个数据,刚好是1M字节
    for (i = 0; i < DEVICE_SRAM_LENGTH; i += testStep)
    {
        DeviceSRAMWrite(DEVICE_SRAM_BASE_ADDR + i, &temp, 1);
        temp++;
    }
    //依次读出之前写入的数据,进行校验
    for (i = 0; i < DEVICE_SRAM_LENGTH; i += testStep)
    {
        DeviceSRAMRead(DEVICE_SRAM_BASE_ADDR + i, &temp, 1);
        if (i == 0)
            sval = temp;
        /*后面读出的数据一定要比第一次读到的数据大.	*/
        else if (temp <= sval)
        {
            /*256一个循环,进行特殊处理*/
            if ((i % 256) == 0)
                sval = temp;
            else
                return 1;
        }
        else
            sval = temp;
    }
    return 0;
}


