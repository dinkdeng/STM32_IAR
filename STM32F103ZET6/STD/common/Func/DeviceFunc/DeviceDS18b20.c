#include "DeviceDS18b20.h"
#include "CoreTickDelay.h"
#include "SystemUtil.h"

/*IO方向设置*/
#define DS18B20_IO_IN()  {GPIOG->CRH&=0XFFFF0FFF;GPIOG->CRH|=8<<12;}
#define DS18B20_IO_OUT() {GPIOG->CRH&=0XFFFF0FFF;GPIOG->CRH|=3<<12;}

/*IO操作函数*/
#define	DS18B20_DQ_OUT PGout(11)
#define	DS18B20_DQ_IN  PGin(11)

/**定义DS18B20的指令集 */
typedef enum DEVICE_DS18B20_COMMAND
{
    /*当总线只有一个18b20的时候,用来读取64位的ROM数据*/
    /*包括 八位family数据 48位唯一序列号 8位CRC 一共64位标识符*/
    DEVICE_DS18B20_COMMAND_READ_ROM = 0X33,

    /*该指令之后发送64位标识符,然后主机发送的指令只有匹配*/
    /*这个标识符的芯片才会响应,其他的不会响应.如果所有从机都不响应*/
    /*那么接下来应该发送复位脉冲*/
    DEVICE_DS18B20_COMMAND_MATCH_ROM = 0X55,

    /*用在总线默认只有一个设备的时候,跳过设备识别阶段,默认总线上的全部指令*/
    /*都由默认的芯片来访问,多芯片总线不能使用这个指令,会造成冲突*/
    DEVICE_DS18B20_COMMAND_SKIP_ROM = 0xCC,

    /*当系统刚启动的时候,允许主机去获取系统中所有的设备的ROM*/
    DEVICE_DS18B20_COMMAND_SEARCH_ROM = 0xF0,

    /*设备在初始化的时候响应指令,只有触发闹钟条件的才能触发指令*/
    DEVICE_DS18B20_COMMAND_ALARM_SEARCH = 0XEC,

    /*写暂存器,从TH寄存器开始,写三个字节*/
    DEVICE_DS18B20_COMMAND_WRITE_SCRATCHPAD = 0X4E,

    /*读取暂存器,该命令之后 读取九个字节 八个寄存器 一个CRC*/
    DEVICE_DS18B20_COMMAND_READ_SCRATCHPAD = 0XBE,

    /*将暂存器中的信息存储到EE存储区中*/
    DEVICE_DS18B20_COMMAND_COPY_SCRATCHPAD = 0x48,

    /*启动芯片的温度转换,该指令之后读取总线,如果读取到的是0*/
    /*总线繁忙 1 温度转换完成*/
    DEVICE_DS18B20_COMMAND_CONVERT_TEMP = 0x44,

    /*从EE存储区中读取信息到寄存器中*/
    DEVICE_DS18B20_COMMAND_RECALL_E2 = 0XB8,

    /*读取芯片电源需求 0 总线寄生 1外部电源*/
    DEVICE_DS18B20_COMMAND_READ_POWER_SUPPLY = 0XB4,
}DEVICE_DS18B20_COMMAND;

static void DeviceDS18b20Rst(void)
{
    /*设置为输出*/
    DS18B20_IO_OUT();
    /*拉低DQ*/
    DS18B20_DQ_OUT = 0;
    /*拉低750us*/
    DeviceDS18b20DelayUs(750);
    /*DQ=1*/
    DS18B20_DQ_OUT = 1;
    /*15US*/
    DeviceDS18b20DelayUs(15);
}

static uint8_t DeviceDS18b20Check(void)
{
    uint8_t retry = 0;
    /*设置为输入*/
    DS18B20_IO_IN();
    while (DS18B20_DQ_IN&&retry < 200)
    {
        retry++;
        DeviceDS18b20DelayUs(1);
    };
    if (retry >= 200)
        return 1;
    else
        retry = 0;
    while (!DS18B20_DQ_IN&&retry < 240)
    {
        retry++;
        DeviceDS18b20DelayUs(1);
    };
    if (retry >= 240)
        return 1;
    return 0;
}

static uint8_t DeviceDS18b20ReadBit(void)
{
    uint8_t data;
    /*设置为输出*/
    DS18B20_IO_OUT();
    DS18B20_DQ_OUT = 0;
    DeviceDS18b20DelayUs(2);
    DS18B20_DQ_OUT = 1;
    /*设置为输入*/
    DS18B20_IO_IN();
    DeviceDS18b20DelayUs(12);
    if (DS18B20_DQ_IN)
        data = 1;
    else
        data = 0;
    DeviceDS18b20DelayUs(50);
    return data;
}

static uint8_t DeviceDS18b20ReadByte(void)
{
    uint8_t i, j, dat;
    dat = 0;
    for (i = 1; i <= 8; i++)
    {
        j = DeviceDS18b20ReadBit();
        dat = (j << 7) | (dat >> 1);
    }
    return dat;
}


static void DeviceDS18b20WriteByte(uint8_t dat)
{
    uint8_t j;
    uint8_t testb;
    /*设置为输出*/
    DS18B20_IO_OUT();
    for (j = 1; j <= 8; j++)
    {
        testb = dat & 0x01;
        dat = dat >> 1;
        /*写1*/
        if (testb)
        {
            DS18B20_DQ_OUT = 0;
            DeviceDS18b20DelayUs(2);
            DS18B20_DQ_OUT = 1;
            DeviceDS18b20DelayUs(60);
        }
        /*写0*/
        else
        {
            DS18B20_DQ_OUT = 0;
            DeviceDS18b20DelayUs(60);
            DS18B20_DQ_OUT = 1;
            DeviceDS18b20DelayUs(2);
        }
    }
}

static void DeviceDS18b20Start(void)
{
    DeviceDS18b20Rst();
    DeviceDS18b20Check();
    /*skip rom*/
    DeviceDS18b20WriteByte(DEVICE_DS18B20_COMMAND_SKIP_ROM);
    /*convert*/
    DeviceDS18b20WriteByte(DEVICE_DS18B20_COMMAND_CONVERT_TEMP);
}




/**设备初始化 */
uint8_t DeviceDS18b20Init(void)
{
    GPIO_InitTypeDef  GPIO_InitStructure;
    //使能PORTG口时钟 
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOG, ENABLE);
    //PORTG.11 推挽输出
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOG, &GPIO_InitStructure);
    //输出1
    GPIO_SetBits(GPIOG, GPIO_Pin_11);
    DeviceDS18b20Rst();
    if (0 == DeviceDS18b20Check())
    {
        return 0;
    }
    else
    {
        return 1;
    }
}

/**设备 读取温度值,返回的是放大十倍的温度值*/
uint8_t DeviceDS18b20ReadTemp(short* temp)
{
    uint8_t tempFlag;
    uint8_t TL, TH;
    short tem;
    /*开始转换*/
    DeviceDS18b20Start();
    DeviceDS18b20Rst();
    /*检测存在*/
    if (1 == DeviceDS18b20Check())
    {
        return 1;
    }
    /*skip rom*/
    DeviceDS18b20WriteByte(DEVICE_DS18B20_COMMAND_SKIP_ROM);
    /*convert	*/
    DeviceDS18b20WriteByte(DEVICE_DS18B20_COMMAND_READ_SCRATCHPAD);
    /*LSB  */
    TL = DeviceDS18b20ReadByte();
    /* MSB */
    TH = DeviceDS18b20ReadByte();
    if (TH > 7)
    {
        TH = ~TH;
        TL = ~TL;
        /*温度为负*/
        tempFlag = 0;
    }
    /*温度为正*/
    else
        tempFlag = 1;
    /*获得高八位*/
    tem = TH;
    tem <<= 8;
    /*获得底八位*/
    tem += TL;
    /*转换 */
    tem = (short)((double)tem*0.625);
    if (tempFlag)
        *temp = tem; //返回温度值
    else
        *temp = -tem;
    return 0;
}