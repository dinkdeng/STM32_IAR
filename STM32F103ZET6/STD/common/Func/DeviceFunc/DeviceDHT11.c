#include "DeviceDHT11.h"
#include "SystemUtil.h"
#include "CoreTickDelay.h"

/*IO方向设置*/
#define DHT11_IO_IN()  {GPIOG->CRH&=0XFFFF0FFF;GPIOG->CRH|=8<<12;}
#define DHT11_IO_OUT() {GPIOG->CRH&=0XFFFF0FFF;GPIOG->CRH|=3<<12;}

/*IO操作函数*/
#define	DHT11_DQ_OUT    PGout(11)
#define	DHT11_DQ_IN     PGin(11)

static void DeviceDHT11Rst(void)
{
    /*设置为输出*/
    DHT11_IO_OUT();
    /*拉低DQ*/
    DHT11_DQ_OUT = 0;
    /*拉低至少18ms*/
    DeviceDHT11DelayMs(20);
    /*DQ=1 */
    DHT11_DQ_OUT = 1;
    /*主机拉高20~40us*/
    DeviceDHT11DelayUs(30);
}

static uint8_t DeviceDHT11Check(void)
{
    uint8_t retry = 0;
    /*设置为输入*/
    DHT11_IO_IN();
    /*DHT11会拉低40~80us*/
    while (DHT11_DQ_IN&&retry < 100)
    {
        retry++;
        DeviceDHT11DelayUs(1);
    };
    if (retry >= 100)
        return 1;
    else
        retry = 0;
    /*DHT11拉低后会再次拉高40~80us*/
    while (!DHT11_DQ_IN&&retry < 100)
    {
        retry++;
        DeviceDHT11DelayUs(1);
    };
    if (retry >= 100)
        return 1;
    return 0;
}

static uint8_t DeviceDHT11ReadBit(void)
{
    uint8_t retry = 0;
    /*等待变为低电平*/
    while (DHT11_DQ_IN&&retry < 100)
    {
        retry++;
        DeviceDHT11DelayUs(1);
    }
    retry = 0;
    /*等待变高电平*/
    while (!DHT11_DQ_IN&&retry < 100)
    {
        retry++;
        DeviceDHT11DelayUs(1);
    }
    /*等待40us*/
    DeviceDHT11DelayUs(40);
    if (DHT11_DQ_IN)
        return 1;
    else
        return 0;
}

static uint8_t DeviceDHT11ReadByte(void)
{
    uint8_t i, dat;
    dat = 0;
    for (i = 0; i < 8; i++)
    {
        dat <<= 1;
        dat |= DeviceDHT11ReadBit();
    }
    return dat;
}





/*驱动初始化*/
uint8_t DeviceDHT11Init(void)
{
    GPIO_InitTypeDef  GPIO_InitStructure;
    //使能PG端口时钟
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOG, ENABLE);
    //PG11端口配置
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11;
    //推挽输出
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    //初始化IO口
    GPIO_Init(GPIOG, &GPIO_InitStructure);
    //PG11 输出高
    GPIO_SetBits(GPIOG, GPIO_Pin_11);
    //复位DHT11
    DeviceDHT11Rst();
    return DeviceDHT11Check();
}

/*读取数据*/
uint8_t DeviceDHT11ReadValue(uint8_t* temp, uint8_t* humi)
{
    uint8_t buf[5];
    uint8_t i;
    DeviceDHT11Rst();
    if (DeviceDHT11Check() == 0)
    {
        /*读取40位数据*/
        for (i = 0; i < 5; i++)
        {
            buf[i] = DeviceDHT11ReadByte();
        }
        if ((buf[0] + buf[1] + buf[2] + buf[3]) == buf[4])
        {
            *humi = buf[0];
            *temp = buf[2];
        }
        else
        {
            return 2;
        }
    }
    else
    {
        return 1;
    }
    return 0;
}


