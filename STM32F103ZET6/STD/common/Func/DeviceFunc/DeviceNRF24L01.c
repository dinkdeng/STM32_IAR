#include "DeviceNRF24L01.h"
#include "SystemUtil.h"
#include "CoreTickDelay.h"
#include "CoreSPI.h"

/**NRF24L01寄存器设置 */
#define DEVICE_NRF24L01_REG_OBSERVE_TX      	0x08  //发送检测寄存器,bit7:4,数据包丢失计数器;bit3:0,重发计数器
#define DEVICE_NRF24L01_REG_CD              	0x09  //载波检测寄存器,bit0,载波检测;
#define DEVICE_NRF24L01_REG_RX_ADDR_P0      	0x0A  //数据通道0接收地址,最大长度5个字节,低字节在前
#define DEVICE_NRF24L01_REG_RX_ADDR_P1      	0x0B  //数据通道1接收地址,最大长度5个字节,低字节在前
#define DEVICE_NRF24L01_REG_RX_ADDR_P2      	0x0C  //数据通道2接收地址,最低字节可设置,高字节,必须同RX_ADDR_P1[39:8]相等;
#define DEVICE_NRF24L01_REG_RX_ADDR_P3      	0x0D  //数据通道3接收地址,最低字节可设置,高字节,必须同RX_ADDR_P1[39:8]相等;
#define DEVICE_NRF24L01_REG_RX_ADDR_P4      	0x0E  //数据通道4接收地址,最低字节可设置,高字节,必须同RX_ADDR_P1[39:8]相等;
#define DEVICE_NRF24L01_REG_RX_ADDR_P5      	0x0F  //数据通道5接收地址,最低字节可设置,高字节,必须同RX_ADDR_P1[39:8]相等;
#define DEVICE_NRF24L01_REG_TX_ADDR         	0x10  //发送地址(低字节在前),ShockBurstTM模式下,RX_ADDR_P0与此地址相等
#define DEVICE_NRF24L01_REG_RX_PW_P0        	0x11  //接收数据通道0有效数据宽度(1~32字节),设置为0则非法
#define DEVICE_NRF24L01_REG_RX_PW_P1        	0x12  //接收数据通道1有效数据宽度(1~32字节),设置为0则非法
#define DEVICE_NRF24L01_REG_RX_PW_P2        	0x13  //接收数据通道2有效数据宽度(1~32字节),设置为0则非法
#define DEVICE_NRF24L01_REG_RX_PW_P3        	0x14  //接收数据通道3有效数据宽度(1~32字节),设置为0则非法
#define DEVICE_NRF24L01_REG_RX_PW_P4        	0x15  //接收数据通道4有效数据宽度(1~32字节),设置为0则非法
#define DEVICE_NRF24L01_REG_RX_PW_P5        	0x16  //接收数据通道5有效数据宽度(1~32字节),设置为0则非法
#define DEVICE_NRF24L01_REG_NRF_FIFO_STATUS 	0x17  //FIFO状态寄存器;bit0,RX FIFO寄存器空标志;bit1,RX FIFO满标志;bit2,3,保留
                                                      //bit4,TX FIFO空标志;bit5,TX FIFO满标志;bit6,1,循环发送上一数据包.0,不循环;
#define DEVICE_NRF24L01_REG_CONFIG          	0x00  //配置寄存器地址;bit0:1接收模式,0发射模式;bit1:电选择;bit2:CRC模式;bit3:CRC使能;
							                          //bit4:中断MAX_RT(达到最大重发次数中断)使能;bit5:中断TX_DS使能;bit6:中断RX_DR使能
#define DEVICE_NRF24L01_REG_EN_AA           	0x01  //使能自动应答功能  bit0~5,对应通道0~5
#define DEVICE_NRF24L01_REG_EN_RXADDR       	0x02  //接收地址允许,bit0~5,对应通道0~5
#define DEVICE_NRF24L01_REG_SETUP_AW        	0x03  //设置地址宽度(所有数据通道):bit1,0:00,3字节;01,4字节;02,5字节;
#define DEVICE_NRF24L01_REG_SETUP_RETR      	0x04  //建立自动重发;bit3:0,自动重发计数器;bit7:4,自动重发延时 250*x+86us
#define DEVICE_NRF24L01_REG_RF_CH           	0x05  //RF通道,bit6:0,工作通道频率;
#define DEVICE_NRF24L01_REG_RF_SETUP        	0x06  //RF寄存器;bit3:传输速率(0:1Mbps,1:2Mbps);bit2:1,发射功率;bit0:低噪声放大器增益
#define DEVICE_NRF24L01_REG_STATUS          	0x07  //状态寄存器;bit0:TX FIFO满标志;bit3:1,接收数据通道号(最大:6);bit4,达到最多次重发
								                      //bit5:数据发送完成中断;bit6:接收数据中断;

/**NRF24L01 指令设置 */
#define DEVICE_NRF24L01_COMMAND_READ_REG_NRF        0x00  //读配置寄存器,低5位为寄存器地址
#define DEVICE_NRF24L01_COMMAND_WRITE_REG_NRF       0x20  //写配置寄存器,低5位为寄存器地址
#define DEVICE_NRF24L01_COMMAND_RD_RX_PLOAD     	0x61  //读RX有效数据,1~32字节
#define DEVICE_NRF24L01_COMMAND_WR_TX_PLOAD     	0xA0  //写TX有效数据,1~32字节
#define DEVICE_NRF24L01_COMMAND_FLUSH_TX        	0xE1  //清除TX FIFO寄存器.发射模式下用
#define DEVICE_NRF24L01_COMMAND_FLUSH_RX        	0xE2  //清除RX FIFO寄存器.接收模式下用
#define DEVICE_NRF24L01_COMMAND_REUSE_TX_PL     	0xE3  //重新使用上一包数据,CE为高,数据包被不断发送.
#define DEVICE_NRF24L01_COMMAND_NOP             	0xFF  //空操作,可以用来读状态寄存器

/**设备发送接收返回的结果代码 */
#define DEVICE_NRF24L01_RESULT_MAX_TX  			0x10
#define DEVICE_NRF24L01_RESULT_TX_OK   			0x20
#define DEVICE_NRF24L01_RESULT_RX_OK   			0x40


/**内置地址 */
static uint8_t deviceNRF24L01TxAddr[DEVICE_NRF24L01_ADDR_WIDTH] = DEVICE_NRF24L01_TX_ADDR;
static uint8_t deviceNRF24L01RxAddr[DEVICE_NRF24L01_ADDR_WIDTH] = DEVICE_NRF24L01_RX_ADDR;

/**接口初始化 */
void DeviceNRF24L01_IO_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;

    /**使能PB,D,G端口时钟 */
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB|RCC_APB2Periph_GPIOD|RCC_APB2Periph_GPIOG, ENABLE);

	/**PB12上拉 防止W25X的干扰 */
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12;
    /**推挽输出 */
 	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
 	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    /**初始化指定IO */
 	GPIO_Init(GPIOB, &GPIO_InitStructure);
    /**上拉 */
 	GPIO_SetBits(GPIOB,GPIO_Pin_12);

    /**PD2推挽输出上拉   禁止SD卡的干扰 */
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;
    /**推挽输出 */
 	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
 	GPIO_SetBits(GPIOD,GPIO_Pin_2);

    /**PG6 7 推挽  */
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6|GPIO_Pin_7;
    /**初始化指定IO*/
 	GPIO_Init(GPIOG, &GPIO_InitStructure);

    /**PG8 输入   */
	GPIO_InitStructure.GPIO_Pin  = GPIO_Pin_8;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPD;
	GPIO_Init(GPIOG, &GPIO_InitStructure);

    /**初始化使能都关闭 */
    DeviceNRF24L01_CSN_Set(1);
    DeviceNRF24L01_CE_Set(1);

    /**SPI初始化 */
    CoreSPI2Init(DEVICE_NRF24L01_CPOL,DEVICE_NRF24L01_CPHA,DEVICE_NRF24L01_SPEED_LOW);
}

//SPI写寄存器
//reg:指定寄存器地址
//value:写入的值
uint8_t DeviceNRF24L01WriteReg(uint8_t reg,uint8_t value)
{
	uint8_t status;
    /**使能SPI传输 */
   	DeviceNRF24L01_CSN_Set(0);
    /**发送寄存器号 */
  	status =CoreSPI2WriteRead(reg);
    /**写入寄存器的值 */
  	CoreSPI2WriteRead(value);
    /**禁止SPI传输 */
  	DeviceNRF24L01_CSN_Set(1);
    /**返回状态值*/
  	return (status);
}


//读取SPI寄存器值
//reg:要读的寄存器
uint8_t DeviceNRF24L01ReadReg(uint8_t reg)
{
	uint8_t reg_val;
 	DeviceNRF24L01_CSN_Set(0);
  	CoreSPI2WriteRead(reg);
  	reg_val=CoreSPI2WriteRead(0XFF);
  	DeviceNRF24L01_CSN_Set(1);
  	return(reg_val);
}

//在指定位置读出指定长度的数据
//reg:寄存器(位置)
//*pBuf:数据指针
//len:数据长度
//返回值,此次读到的状态寄存器值
uint8_t DeviceNRF24L01ReadBuf(uint8_t reg,uint8_t *pBuf,uint8_t len)
{
	uint8_t status;
    uint8_t i = 0;
  	DeviceNRF24L01_CSN_Set(0);
    /**发送寄存器值(位置),并读取状态值   */
  	status=CoreSPI2WriteRead(reg);
    /**读出数据*/
 	for(i=0;i<len;i++)
        pBuf[i]=CoreSPI2WriteRead(0XFF);
    /**关闭SPI传输 */
  	DeviceNRF24L01_CSN_Set(1);
    /**返回读到的状态值*/
  	return status;
}

//在指定位置写指定长度的数据
//reg:寄存器(位置)
//*pBuf:数据指针
//len:数据长度
//返回值,此次读到的状态寄存器值
uint8_t DeviceNRF24L01WriteBuf(uint8_t reg, uint8_t *pBuf, uint8_t len)
{
	uint8_t status;
    uint8_t i;
 	DeviceNRF24L01_CSN_Set(0);
    /**发送寄存器值(位置),并读取状态值 */
  	status = CoreSPI2WriteRead(reg);
  	for(i=0; i<len; i++)
        CoreSPI2WriteRead(*pBuf++);
  	DeviceNRF24L01_CSN_Set(1);
    /**返回读到的状态值 */
  	return status;
}

//该函数初始化NRF24L01到TX模式
//设置TX地址,写TX数据宽度,设置RX自动应答的地址,填充TX发送数据,选择RF频道,波特率和LNA HCURR
//PWR_UP,CRC使能
//当CE变高后,即进入RX模式,并可以接收数据了
//CE为高大于10us,则启动发送.
void DeviceNRF24L01ModeChangeTX(void)
{
	DeviceNRF24L01_CE_Set(0);
    /**设置极性和速度*/
    CoreSPI2SetCp(DEVICE_NRF24L01_CPOL,DEVICE_NRF24L01_CPHA);
    CoreSPI2SetSpeed(DEVICE_NRF24L01_SPEED_LOW);
    /**写TX节点地址 */
  	DeviceNRF24L01WriteBuf(DEVICE_NRF24L01_COMMAND_WRITE_REG_NRF+DEVICE_NRF24L01_REG_TX_ADDR,(uint8_t*)deviceNRF24L01TxAddr,DEVICE_NRF24L01_ADDR_WIDTH);
    /**设置TX节点地址,主要为了使能ACK*/
  	DeviceNRF24L01WriteBuf(DEVICE_NRF24L01_COMMAND_WRITE_REG_NRF+DEVICE_NRF24L01_REG_RX_ADDR_P0,(uint8_t*)deviceNRF24L01RxAddr,DEVICE_NRF24L01_ADDR_WIDTH);

    /**使能通道0的自动应答 */
  	DeviceNRF24L01WriteReg(DEVICE_NRF24L01_COMMAND_WRITE_REG_NRF+DEVICE_NRF24L01_REG_EN_AA,0x01);
    /**使能通道0的接收地址  */
  	DeviceNRF24L01WriteReg(DEVICE_NRF24L01_COMMAND_WRITE_REG_NRF+DEVICE_NRF24L01_REG_EN_RXADDR,0x01);
    /**设置自动重发间隔时间:500us + 86us;最大自动重发次数:10次 */
  	DeviceNRF24L01WriteReg(DEVICE_NRF24L01_COMMAND_WRITE_REG_NRF+DEVICE_NRF24L01_REG_SETUP_RETR,0x1a);
    /**设置RF通道为40 */
  	DeviceNRF24L01WriteReg(DEVICE_NRF24L01_COMMAND_WRITE_REG_NRF+DEVICE_NRF24L01_REG_RF_CH,40);
    /**设置TX发射参数,0db增益,2Mbps,低噪声增益开启   */
  	DeviceNRF24L01WriteReg(DEVICE_NRF24L01_COMMAND_WRITE_REG_NRF+DEVICE_NRF24L01_REG_RF_SETUP,0x0f);
    /**配置基本工作模式的参数;PWR_UP,EN_CRC,16BIT_CRC,接收模式,开启所有中断*/
  	DeviceNRF24L01WriteReg(DEVICE_NRF24L01_COMMAND_WRITE_REG_NRF+DEVICE_NRF24L01_REG_CONFIG,0x0e);
	DeviceNRF24L01_CE_Set(0);
    /**等待10MS,状态切换 */
    DEVICE_NRF24L01_WAIT_MS(5);
}

//该函数初始化NRF24L01到RX模式
//设置RX地址,写RX数据宽度,选择RF频道,波特率和LNA HCURR
//当CE变高后,即进入RX模式,并可以接收数据了
void DeviceNRF24L01ModeChangeRX(void)
{
    /**CE为低,使能芯片 */
	DeviceNRF24L01_CE_Set(0);
    /**设置极性和速度*/
    CoreSPI2SetCp(DEVICE_NRF24L01_CPOL,DEVICE_NRF24L01_CPHA);
    CoreSPI2SetSpeed(DEVICE_NRF24L01_SPEED_LOW);
    /**写RX节点地址 */
  	DeviceNRF24L01WriteBuf(DEVICE_NRF24L01_COMMAND_WRITE_REG_NRF+DEVICE_NRF24L01_REG_RX_ADDR_P0,(uint8_t*)deviceNRF24L01RxAddr,DEVICE_NRF24L01_ADDR_WIDTH);

    /**使能通道0的自动应答 */
  	DeviceNRF24L01WriteReg(DEVICE_NRF24L01_COMMAND_WRITE_REG_NRF+DEVICE_NRF24L01_REG_EN_AA,0x01);
    /**使能通道0的接收地址 */
  	DeviceNRF24L01WriteReg(DEVICE_NRF24L01_COMMAND_WRITE_REG_NRF+DEVICE_NRF24L01_REG_EN_RXADDR,0x01);
    /**设置RF通信频率*/
  	DeviceNRF24L01WriteReg(DEVICE_NRF24L01_COMMAND_WRITE_REG_NRF+DEVICE_NRF24L01_REG_RF_CH,40);
    /**选择通道0的有效数据宽度 */
  	DeviceNRF24L01WriteReg(DEVICE_NRF24L01_COMMAND_WRITE_REG_NRF+DEVICE_NRF24L01_REG_RX_PW_P0,DEVICE_NRF24L01_DATA_TRANS_LENGTH);
    /**设置TX发射参数,0db增益,2Mbps,低噪声增益开启  */
  	DeviceNRF24L01WriteReg(DEVICE_NRF24L01_COMMAND_WRITE_REG_NRF+DEVICE_NRF24L01_REG_RF_SETUP,0x0f);
    /**配置基本工作模式的参数;PWR_UP,EN_CRC,16BIT_CRC,接收模式 */
  	DeviceNRF24L01WriteReg(DEVICE_NRF24L01_COMMAND_WRITE_REG_NRF+DEVICE_NRF24L01_REG_CONFIG, 0x0f);
    /**CE为高,进入接收模式  */
  	DeviceNRF24L01_CE_Set(1);
    /**等待10MS,状态切换 */
    DEVICE_NRF24L01_WAIT_MS(5);
}

//启动NRF24L01发送一次数据
//txbuf:待发送数据首地址
//返回值:发送完成状况
uint8_t DeviceNRF24L01SendPackage(uint8_t* sendBufPtr)
{
	uint8_t sta;
 	/**设置极性和速度*/
    CoreSPI2SetCp(DEVICE_NRF24L01_CPOL,DEVICE_NRF24L01_CPHA);
    CoreSPI2SetSpeed(DEVICE_NRF24L01_SPEED_HIGH);
	DeviceNRF24L01_CE_Set(0);
    /**写数据到TX BUF  32个字节 */
  	DeviceNRF24L01WriteBuf(DEVICE_NRF24L01_COMMAND_WR_TX_PLOAD,sendBufPtr,DEVICE_NRF24L01_DATA_TRANS_LENGTH);
    /**启动发送	 */
 	DeviceNRF24L01_CE_Set(1);
    /**等待发送完成 */
	while(DeviceNRF24L01_IRQ_Get != 0);
    /**读取状态寄存器的值 */
	sta=DeviceNRF24L01ReadReg(DEVICE_NRF24L01_REG_STATUS);
    /**清除TX_DS或MAX_RT中断标志 */
	DeviceNRF24L01WriteReg(DEVICE_NRF24L01_COMMAND_WRITE_REG_NRF+DEVICE_NRF24L01_REG_STATUS,sta);
    /**达到最大重发次数 */
	if(sta&DEVICE_NRF24L01_RESULT_MAX_TX)
	{
        /**清除TX FIFO寄存器  */
		DeviceNRF24L01WriteReg(DEVICE_NRF24L01_COMMAND_FLUSH_TX,0xff);
		return 1;
	}
    /**发送完成 */
	if(sta&DEVICE_NRF24L01_RESULT_TX_OK)
	{
		return 0;
	}
    /**其他原因发送失败 */
	return 0xff;
}

uint8_t DeviceNRF24L01RecvPackage(uint8_t* recvBuffer)
{
    uint8_t sta;
	/**设置极性和速度*/
    CoreSPI2SetCp(DEVICE_NRF24L01_CPOL,DEVICE_NRF24L01_CPHA);
    CoreSPI2SetSpeed(DEVICE_NRF24L01_SPEED_HIGH);
	DeviceNRF24L01_CE_Set(0);
    /**读取状态寄存器的值 */
	sta=DeviceNRF24L01ReadReg(DEVICE_NRF24L01_REG_STATUS);
    /**清除TX_DS或MAX_RT中断标志 */
	DeviceNRF24L01WriteReg(DEVICE_NRF24L01_COMMAND_WRITE_REG_NRF+DEVICE_NRF24L01_REG_STATUS,sta);
    /**接收到数据 */
	if(sta&DEVICE_NRF24L01_RESULT_RX_OK)
	{
        /**读取数据 */
		DeviceNRF24L01ReadBuf(DEVICE_NRF24L01_COMMAND_RD_RX_PLOAD,recvBuffer,DEVICE_NRF24L01_DATA_TRANS_LENGTH);
        /**清除RX FIFO寄存器  */
		DeviceNRF24L01WriteReg(DEVICE_NRF24L01_COMMAND_FLUSH_RX,0xff);
		DeviceNRF24L01_CE_Set(1);
		return 0;
	}
	DeviceNRF24L01_CE_Set(1);
    /**没收到任何数据 */
	return 1;
}

//检测24L01是否存在
//返回值:0，成功;1，失败
uint8_t DeviceNRF24L01Check(void)
{
	uint8_t buf[5]={0XA5,0XA5,0XA5,0XA5,0XA5};
	uint8_t i;
	DeviceNRF24L01_CE_Set(0);
    /**写入5个字节的地址.*/
	DeviceNRF24L01WriteBuf(DEVICE_NRF24L01_COMMAND_WRITE_REG_NRF+DEVICE_NRF24L01_REG_TX_ADDR,buf,5);
    /**读出写入的地址 */
	DeviceNRF24L01ReadBuf(DEVICE_NRF24L01_REG_TX_ADDR,buf,5);
	DeviceNRF24L01_CE_Set(1);
	for(i=0;i<5;i++)
        if(buf[i]!=0XA5)
            break;
    /**检测24L01错误*/
	if(i!=5)
        return 1;
	return 0;
}

/**设备初始化 */
uint8_t DeviceNRF24L01Init(void)
{
    DeviceNRF24L01_IO_Init();
    return DeviceNRF24L01Check();
}







