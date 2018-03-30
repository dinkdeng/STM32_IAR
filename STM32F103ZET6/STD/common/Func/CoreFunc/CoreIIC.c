#include "CoreIIC.h"
#include "SystemUtil.h"
#include "CoreTickDelay.h"

//PB10 SCL 11 SDA
#define SDA_IN()  {GPIOB->CRH&=0XFFFF0FFF;GPIOB->CRH|=8<<12;}
#define SDA_OUT() {GPIOB->CRH&=0XFFFF0FFF;GPIOB->CRH|=3<<12;}

//IO输入输出
//SCL
#define IIC_SCL    PBout(10)
//SDA 输出
#define IIC_SDA    PBout(11)
//SDA输入
#define READ_SDA   PBin(11)

/**IIC接口初始化 */
void CoreIICInit(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10 | GPIO_Pin_11;
    //初始化
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOB, &GPIO_InitStructure);
    //PB10,PB11 设置为高
    GPIO_SetBits(GPIOB, GPIO_Pin_10 | GPIO_Pin_11);
}

/**IIC发送start信号 */
void CoreIICSendStart(void)
{
    SDA_OUT();
    IIC_SDA = 1;
    IIC_SCL = 1;
    CORE_IIC_WAIT_US(4);
    /*当SCL为高电平的时候,SDA从高电平切换低电平*/
    IIC_SDA = 0;
    CORE_IIC_WAIT_US(4);
    /*钳住I2C总线，准备发送或接收数据 */
    IIC_SCL = 0;
}

/**发送Stop信号 */
void CoreIICSendStop(void)
{
    /*sda输出*/
    SDA_OUT();
    IIC_SCL = 0;
    /*当SCL为高电平的时候,SDA从低电平切换高电平*/
    IIC_SDA = 0;
    CORE_IIC_WAIT_US(4);
    IIC_SCL = 1;
    /*发送I2C总线结束信号*/
    IIC_SDA = 1;
    CORE_IIC_WAIT_US(4);
}

/**发送一个字节的数据 */
void CoreIICSendByte(uint8_t byteValue)
{
    uint8_t t;
    SDA_OUT();
    /*拉低时钟开始传输数据*/
    IIC_SCL = 0;
    for (t = 0; t < 8; t++)
    {
        if ((byteValue & 0x80) >> 7)
            IIC_SDA = 1;
        else
            IIC_SDA = 0;
        byteValue <<= 1;
        CORE_IIC_WAIT_US(2);
        IIC_SCL = 1;
        CORE_IIC_WAIT_US(2);
        IIC_SCL = 0;
        CORE_IIC_WAIT_US(2);
    }
}

/**接收一个字节的数据 */
uint8_t CoreIICRecvByte(void)
{
    uint8_t i = 0;
    uint8_t receive = 0;
    /*SDA设置为输入*/
    SDA_IN();
    for (i = 0; i < 8; i++)
    {
        IIC_SCL = 0;
        CORE_IIC_WAIT_US(2);
        IIC_SCL = 1;
        receive <<= 1;
        if (READ_SDA)
            receive++;
        CORE_IIC_WAIT_US(1);
    }
    return receive;
}

/**等待ACK 自带Stop信号,应用端出错无需手动停止*/
IIC_WAIT_ACK CoreIICWaitACK(void)
{
    uint16_t ucErrTime = 0;
    /*SDA设置为输入  */
    SDA_IN();
    IIC_SDA = 1;
    CORE_IIC_WAIT_US(1);
    IIC_SCL = 1;
    CORE_IIC_WAIT_US(1);
    while (READ_SDA)
    {
        ucErrTime++;
        if (ucErrTime > IIC_WAIT_ACK_TIME_MAX_COUNT)
        {
            CoreIICSendStop();
            /*没收到ACK*/
            return IIC_WAIT_ACK_FAIL;
        }
    }
    /*时钟输出0 */
    IIC_SCL = 0;
    /*收到ACK*/
    return IIC_WAIT_ACK_OK;
}

/**发送NO ACK */
void CoreIICSendNACK(void)
{
    IIC_SCL = 0;
    SDA_OUT();
    IIC_SDA = 1;
    CORE_IIC_WAIT_US(2);
    IIC_SCL = 1;
    CORE_IIC_WAIT_US(2);
    IIC_SCL = 0;
}

/**发送ACK */
void CoreIICSendACK(void)
{
    IIC_SCL = 0;
    SDA_OUT();
    IIC_SDA = 0;
    CORE_IIC_WAIT_US(2);
    IIC_SCL = 1;
    CORE_IIC_WAIT_US(2);
    IIC_SCL = 0;
}

