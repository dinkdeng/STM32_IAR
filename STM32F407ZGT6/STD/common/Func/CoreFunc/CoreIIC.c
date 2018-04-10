#include "CoreIIC.h"
#include "SystemUtil.h"
#include "CoreTickDelay.h"

//PB9输入模式
#define SDA_IN()  {GPIOB->MODER&=~(3<<(9*2));GPIOB->MODER|=0<<9*2;}	

//PB9输出模式
#define SDA_OUT() {GPIOB->MODER&=~(3<<(9*2));GPIOB->MODER|=1<<9*2;}

//IO输入输出
//SCL
#define IIC_SCL    PBout(8) 
//SDA 输出
#define IIC_SDA    PBout(9) 
//SDA输入
#define READ_SDA   PBin(9) 

/**IIC接口初始化 */
void CoreIIC_Init(void)
{
    GPIO_InitTypeDef  GPIO_InitStructure;

	//使能GPIOB时钟
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);

	//GPIOB8,B9初始化设置
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8 | GPIO_Pin_9;
	//普通输出模式
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
	//推挽输出
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	//100MHz
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
	//上拉
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
	//初始化
	GPIO_Init(GPIOB, &GPIO_InitStructure);
	IIC_SCL=1;
	IIC_SDA=1;
}

/**IIC发送start信号 */
void CoreIIC_SendStart(void)
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
void CoreIIC_SendStop(void)
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
void CoreIIC_SendByte(uint8_t byteValue)
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
uint8_t CoreIIC_RecvByte(void)
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
IIC_WAIT_ACK CoreIIC_WaitACK(void)
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
            CoreIIC_SendStop();
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
void CoreIIC_SendNACK(void)
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
void CoreIIC_SendACK(void)
{
    IIC_SCL = 0;
    SDA_OUT();
    IIC_SDA = 0;
    CORE_IIC_WAIT_US(2);
    IIC_SCL = 1;
    CORE_IIC_WAIT_US(2);
    IIC_SCL = 0;
}

