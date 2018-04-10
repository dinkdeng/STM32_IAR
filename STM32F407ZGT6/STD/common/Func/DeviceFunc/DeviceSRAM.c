#include "DeviceSRAM.h"

/**设备挂载的SRAM初始化 */
uint8_t DeviceSRAM_Init(void)
{
    GPIO_InitTypeDef  GPIO_InitStructure;
	FSMC_NORSRAMInitTypeDef  FSMC_NORSRAMInitStructure;
	FSMC_NORSRAMTimingInitTypeDef  readWriteTiming;

	//使能PD,PE,PF,PG时钟
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB|RCC_AHB1Periph_GPIOD|RCC_AHB1Periph_GPIOE| \
	RCC_AHB1Periph_GPIOF|RCC_AHB1Periph_GPIOG, ENABLE);
	//使能FSMC时钟
	RCC_AHB3PeriphClockCmd(RCC_AHB3Periph_FSMC,ENABLE);

	//PB15 推挽输出,控制背光
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_15;
	//普通输出模式
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
	//推挽输出
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	//100MHz
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	//上拉
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
	//初始化
	GPIO_Init(GPIOB, &GPIO_InitStructure);

	//PD0,1,4,5,8~15 AF OUT
	GPIO_InitStructure.GPIO_Pin = (3<<0)|(3<<4)|(0XFF<<8);
	//复用输出
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
	//推挽输出
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	//100MHz
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
	//上拉
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
	//初始化
	GPIO_Init(GPIOD, &GPIO_InitStructure);

	//PE0,1,7~15,AF OUT
	GPIO_InitStructure.GPIO_Pin = (3<<0)|(0X1FF<<7);
	//复用输出
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
	//推挽输出
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	//100MHz
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
	//上拉
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
	//初始化
	GPIO_Init(GPIOE, &GPIO_InitStructure);

	//PF0~5,12~15
	GPIO_InitStructure.GPIO_Pin = (0X3F<<0)|(0XF<<12);
	//复用输出
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
	//推挽输出
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	//100MHz
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
	//上拉
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
	//初始化
	GPIO_Init(GPIOF, &GPIO_InitStructure);

	//PG0~5,10
	GPIO_InitStructure.GPIO_Pin =(0X3F<<0)| GPIO_Pin_10;
	//复用输出
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
	//推挽输出
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	//100MHz
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
	//上拉
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
	//初始化
	GPIO_Init(GPIOG, &GPIO_InitStructure);

	//设定端口复用
	//PD0,AF12
	GPIO_PinAFConfig(GPIOD,GPIO_PinSource0,GPIO_AF_FSMC);
	//PD1,AF12
	GPIO_PinAFConfig(GPIOD,GPIO_PinSource1,GPIO_AF_FSMC);
	GPIO_PinAFConfig(GPIOD,GPIO_PinSource4,GPIO_AF_FSMC);
	GPIO_PinAFConfig(GPIOD,GPIO_PinSource5,GPIO_AF_FSMC);
	GPIO_PinAFConfig(GPIOD,GPIO_PinSource8,GPIO_AF_FSMC);
	GPIO_PinAFConfig(GPIOD,GPIO_PinSource9,GPIO_AF_FSMC);
	GPIO_PinAFConfig(GPIOD,GPIO_PinSource10,GPIO_AF_FSMC);
	GPIO_PinAFConfig(GPIOD,GPIO_PinSource11,GPIO_AF_FSMC);
	GPIO_PinAFConfig(GPIOD,GPIO_PinSource12,GPIO_AF_FSMC);
	GPIO_PinAFConfig(GPIOD,GPIO_PinSource13,GPIO_AF_FSMC);
	GPIO_PinAFConfig(GPIOD,GPIO_PinSource14,GPIO_AF_FSMC);
	//PD15,AF12
	GPIO_PinAFConfig(GPIOD,GPIO_PinSource15,GPIO_AF_FSMC);

	GPIO_PinAFConfig(GPIOE,GPIO_PinSource0,GPIO_AF_FSMC);
	GPIO_PinAFConfig(GPIOE,GPIO_PinSource1,GPIO_AF_FSMC);
	//PE7,AF12
	GPIO_PinAFConfig(GPIOE,GPIO_PinSource7,GPIO_AF_FSMC);
	GPIO_PinAFConfig(GPIOE,GPIO_PinSource8,GPIO_AF_FSMC);
	GPIO_PinAFConfig(GPIOE,GPIO_PinSource9,GPIO_AF_FSMC);
	GPIO_PinAFConfig(GPIOE,GPIO_PinSource10,GPIO_AF_FSMC);
	GPIO_PinAFConfig(GPIOE,GPIO_PinSource11,GPIO_AF_FSMC);
	GPIO_PinAFConfig(GPIOE,GPIO_PinSource12,GPIO_AF_FSMC);
	GPIO_PinAFConfig(GPIOE,GPIO_PinSource13,GPIO_AF_FSMC);
	GPIO_PinAFConfig(GPIOE,GPIO_PinSource14,GPIO_AF_FSMC);
	//PE15,AF12
	GPIO_PinAFConfig(GPIOE,GPIO_PinSource15,GPIO_AF_FSMC);

	//PF0,AF12
	GPIO_PinAFConfig(GPIOF,GPIO_PinSource0,GPIO_AF_FSMC);
	//PF1,AF12
	GPIO_PinAFConfig(GPIOF,GPIO_PinSource1,GPIO_AF_FSMC);
	//PF2,AF12
	GPIO_PinAFConfig(GPIOF,GPIO_PinSource2,GPIO_AF_FSMC);
	//PF3,AF12
	GPIO_PinAFConfig(GPIOF,GPIO_PinSource3,GPIO_AF_FSMC);
	//PF4,AF12
	GPIO_PinAFConfig(GPIOF,GPIO_PinSource4,GPIO_AF_FSMC);
	//PF5,AF12
	GPIO_PinAFConfig(GPIOF,GPIO_PinSource5,GPIO_AF_FSMC);
	//PF12,AF12
	GPIO_PinAFConfig(GPIOF,GPIO_PinSource12,GPIO_AF_FSMC);
	//PF13,AF12
	GPIO_PinAFConfig(GPIOF,GPIO_PinSource13,GPIO_AF_FSMC);
	//PF14,AF12
	GPIO_PinAFConfig(GPIOF,GPIO_PinSource14,GPIO_AF_FSMC);
	//PF15,AF12
	GPIO_PinAFConfig(GPIOF,GPIO_PinSource15,GPIO_AF_FSMC);

	GPIO_PinAFConfig(GPIOG,GPIO_PinSource0,GPIO_AF_FSMC);
	GPIO_PinAFConfig(GPIOG,GPIO_PinSource1,GPIO_AF_FSMC);
	GPIO_PinAFConfig(GPIOG,GPIO_PinSource2,GPIO_AF_FSMC);
	GPIO_PinAFConfig(GPIOG,GPIO_PinSource3,GPIO_AF_FSMC);
	GPIO_PinAFConfig(GPIOG,GPIO_PinSource4,GPIO_AF_FSMC);
	GPIO_PinAFConfig(GPIOG,GPIO_PinSource5,GPIO_AF_FSMC);
	GPIO_PinAFConfig(GPIOG,GPIO_PinSource10,GPIO_AF_FSMC);

	//地址建立时间（ADDSET）为1个HCLK 1/36M=27ns
	readWriteTiming.FSMC_AddressSetupTime = 0x00;
	//地址保持时间（ADDHLD）模式A未用到
	readWriteTiming.FSMC_AddressHoldTime = 0x00;
	//数据保持时间（DATAST）为9个HCLK 6*9=54ns
	readWriteTiming.FSMC_DataSetupTime = 0x08;
	readWriteTiming.FSMC_BusTurnAroundDuration = 0x00;
	readWriteTiming.FSMC_CLKDivision = 0x00;
	readWriteTiming.FSMC_DataLatency = 0x00;
	//模式A
	readWriteTiming.FSMC_AccessMode = FSMC_AccessMode_A;

	//  这里我们使用NE3 ，也就对应BTCR[4],[5]。
	FSMC_NORSRAMInitStructure.FSMC_Bank = FSMC_Bank1_NORSRAM3;
	FSMC_NORSRAMInitStructure.FSMC_DataAddressMux = FSMC_DataAddressMux_Disable;
	// FSMC_MemoryType_SRAM
	FSMC_NORSRAMInitStructure.FSMC_MemoryType =FSMC_MemoryType_SRAM;
	//存储器数据宽度为16bit
	FSMC_NORSRAMInitStructure.FSMC_MemoryDataWidth = FSMC_MemoryDataWidth_16b;
	// FSMC_BurstAccessMode_Disable;
	FSMC_NORSRAMInitStructure.FSMC_BurstAccessMode =FSMC_BurstAccessMode_Disable;
	FSMC_NORSRAMInitStructure.FSMC_WaitSignalPolarity = FSMC_WaitSignalPolarity_Low;
	FSMC_NORSRAMInitStructure.FSMC_AsynchronousWait=FSMC_AsynchronousWait_Disable;
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
	// 使能BANK1区域3
	FSMC_NORSRAMCmd(FSMC_Bank1_NORSRAM3, ENABLE);

    return DeviceSRAM_Test(1024);
}

uint8_t DeviceSRAM_Read(uint32_t baseAddr, uint8_t* bufferPtr,
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

uint8_t DeviceSRAM_Write(uint32_t baseAddr, uint8_t* bufferPtr,
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
uint8_t DeviceSRAM_Test(uint16_t testStep)
{
    uint32_t i = 0;
    uint8_t temp = 0;
    /*在地址0读到的数据*/
    uint8_t sval = 0;
    //每隔4K字节,写入一个数据,总共写入256个数据,刚好是1M字节
    for (i = 0; i < DEVICE_SRAM_LENGTH; i += testStep)
    {
        DeviceSRAM_Write(DEVICE_SRAM_BASE_ADDR + i, &temp, 1);
        temp++;
    }
    //依次读出之前写入的数据,进行校验
    for (i = 0; i < DEVICE_SRAM_LENGTH; i += testStep)
    {
        DeviceSRAM_Read(DEVICE_SRAM_BASE_ADDR + i, &temp, 1);
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


