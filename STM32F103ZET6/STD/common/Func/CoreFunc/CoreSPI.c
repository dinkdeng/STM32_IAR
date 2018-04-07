#include "CoreSPI.h"
#include "SystemUtil.h"

/**SPI1初始化 */
void CoreSPI1Init(uint16_t cpol,uint16_t cpha,uint16_t speed)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    SPI_InitTypeDef  SPI_InitStructure;
    //PORTB，SPI1时钟使能
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_SPI1, ENABLE);

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5 | GPIO_Pin_6 | GPIO_Pin_7;
    //PA5.6.7复用推挽
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOA, &GPIO_InitStructure);
    //PA5.6.7上拉
    GPIO_SetBits(GPIOA, GPIO_Pin_5 | GPIO_Pin_6 | GPIO_Pin_7);

    //设置SPI单向或者双向的数据模式:SPI设置为双线双向全双工
    SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex;
    //设置SPI工作模式:设置为主SPI
    SPI_InitStructure.SPI_Mode = SPI_Mode_Master;
    //设置SPI的数据大小:SPI发送接收8位帧结构
    SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b;
    //选择了串行时钟的稳态:时钟悬空高
    SPI_InitStructure.SPI_CPOL = cpol;
    //数据捕获于第二个时钟沿
    SPI_InitStructure.SPI_CPHA = cpha;
    //NSS信号由硬件（NSS管脚）还是软件（使用SSI位）管理:内部NSS信号有SSI位控制
    SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;
    //定义波特率预分频的值:波特率预分频值为16
    SPI_InitStructure.SPI_BaudRatePrescaler = speed;
    //指定数据传输从MSB位还是LSB位开始:数据传输从MSB位开始
    SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;
    //CRC值计算的多项式
    SPI_InitStructure.SPI_CRCPolynomial = 7;
    //根据SPI_InitStruct中指定的参数初始化外设SPIx寄存器
    SPI_Init(SPI1, &SPI_InitStructure);
    //使能SPI外设
    SPI_Cmd(SPI1, ENABLE);
}

/**SPI1读写 */
uint8_t CoreSPI1WriteRead(uint8_t writeDat)
{
    uint32_t retry = 0;
    //检查指定的SPI标志位设置与否:发送缓存空标志位
    while (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_TXE) == RESET)
    {
        retry++;
        if (retry > CORE_SPI_WAIT_COUNT_MAX)
            return 0x00;
    }
    //通过外设SPIx发送一个数据
    SPI_I2S_SendData(SPI1, writeDat);
    retry = 0;
    //检查指定的SPI标志位设置与否:接受缓存非空标志位
    while (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_RXNE) == RESET)
    {
        retry++;
        if (retry > CORE_SPI_WAIT_COUNT_MAX)
            return 0x00;
    }
    return SPI_I2S_ReceiveData(SPI1);
}

/**SPI1设置速度 */
void CoreSPI1SetSpeed(uint16_t speed)
{
    SPI1->CR1 &= 0XFFC7;
    SPI1->CR1 |= speed;
    SPI_Cmd(SPI1, ENABLE);
}

/**SPI1设置CPOL CPHA特性 */
void CoreSPI1SetCp(uint16_t cpol,uint16_t cpha)
{
    /**计算设置值 */
    uint16_t setValue = 0;
    setValue = (cpol|cpha);
    setValue &= 0x0003;

    /**清空设置值 */
    SPI1->CR1 &= ~(0x0003);
    SPI1->CR1 |= setValue;
    /**重新使能 */
    SPI_Cmd(SPI1, ENABLE);
}

/**SPI2初始化 */
void CoreSPI2Init(uint16_t cpol,uint16_t cpha,uint16_t speed)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    SPI_InitTypeDef  SPI_InitStructure;
    //PORTB时钟使能
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
    //SPI2时钟使能
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_SPI2, ENABLE);


    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_13 | GPIO_Pin_14 | GPIO_Pin_15;
    //PB13/14/15复用推挽输出
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    //初始化GPIOB
    GPIO_Init(GPIOB, &GPIO_InitStructure);

    //PB13/14/15上拉
    GPIO_SetBits(GPIOB, GPIO_Pin_13 | GPIO_Pin_14 | GPIO_Pin_15);

    SPI_Cmd(SPI2,DISABLE);
    //设置SPI单向或者双向的数据模式:SPI设置为双线双向全双工
    SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex;
    //设置SPI工作模式:设置为主SPI
    SPI_InitStructure.SPI_Mode = SPI_Mode_Master;
    //设置SPI的数据大小:SPI发送接收8位帧结构
    SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b;
    //串行同步时钟的空闲状态为高电平
    SPI_InitStructure.SPI_CPOL = cpol;
    //串行同步时钟的第二个跳变沿（上升或下降）数据被采样
    SPI_InitStructure.SPI_CPHA = cpha;
    //NSS信号由硬件（NSS管脚）还是软件（使用SSI位）管理:内部NSS信号有SSI位控制
    SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;
    //定义波特率预分频的值:波特率预分频值为256
    SPI_InitStructure.SPI_BaudRatePrescaler = speed;
    //指定数据传输从MSB位还是LSB位开始:数据传输从MSB位开始
    SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;
    //CRC值计算的多项式
    SPI_InitStructure.SPI_CRCPolynomial = 7;
    //根据SPI_InitStruct中指定的参数初始化外设SPIx寄存器
    SPI_Init(SPI2, &SPI_InitStructure);
    //使能SPI外设
    SPI_Cmd(SPI2, ENABLE);
    //启动传输
    CoreSPI2WriteRead(0xff);
}

/**SPI2读写 */
uint8_t CoreSPI2WriteRead(uint8_t writeDat)
{
    uint16_t retry = 0;
    //检查指定的SPI标志位设置与否:发送缓存空标志位
    while (SPI_I2S_GetFlagStatus(SPI2, SPI_I2S_FLAG_TXE) == RESET)
    {
        retry++;
        /**超时一律返回0 */
        if (retry > CORE_SPI_WAIT_COUNT_MAX)
            return 0x00;
    }
    //通过外设SPIx发送一个数据
    SPI_I2S_SendData(SPI2, writeDat);
    retry = 0;
    //检查指定的SPI标志位设置与否:接受缓存非空标志位
    while (SPI_I2S_GetFlagStatus(SPI2, SPI_I2S_FLAG_RXNE) == RESET)
    {
        retry++;
        /**超时一律返回0 */
        if (retry > CORE_SPI_WAIT_COUNT_MAX)
            return 0x00;
    }
    return SPI_I2S_ReceiveData(SPI2);
}

/**SPI2设置速度 */
void CoreSPI2SetSpeed(uint16_t speed)
{
    SPI_Cmd(SPI2, DISABLE);
    //先停止SPI传输
    SPI2->CR1 &= 0XFFC7;
    //设置API速度
    SPI2->CR1 |= speed;
    //使能SPI外设
    SPI_Cmd(SPI2, ENABLE);
}

/**SPI2设置CPOL CPHA特性 */
void CoreSPI2SetCp(uint16_t cpol,uint16_t cpha)
{
    /**计算设置值 */
    uint16_t setValue = 0;
    setValue = (cpol|cpha);
    setValue &= 0x0003;

    SPI_Cmd(SPI2, DISABLE);
    /**清空设置值 */
    SPI2->CR1 &= ~(0x0003);
    SPI2->CR1 |= setValue;
    //使能SPI外设
    SPI_Cmd(SPI2, ENABLE);
}


