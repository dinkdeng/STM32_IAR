#include "DeviceLCD.h"
#include "DeviceLcdAsciiFont.h"
#include "CoreTickDelay.h"
#include "SystemUtil.h"

/**LCD背光控制 */
#define	DeviceLCD_BackLightSet(n) 		(PBout(15) = n)

//使用NOR/SRAM的 Bank1.sector4,地址位HADDR[27,26]=11
//A6作为数据命令区分线
//注意设置时STM32内部会右移一位对其! 111 1110=0X7E
//LCD基地址
#define DEVICE_LCD_BASE_ADDR        ((uint32_t)(0x6C000000 | 0x0000007E))

//LCD地址结构体
typedef struct DeviceLCD_TypeDef
{
	//寄存器地址映射
	volatile uint16_t DeviceLCD_Reg;
	//数据RAM地址映射
	volatile uint16_t DeviceLCD_Ram;
} DeviceLCD_TypeDef;

//通过基地址定义寄存器写入地址和缓冲区地址
#define DeviceLCD             			((DeviceLCD_TypeDef *) DEVICE_LCD_BASE_ADDR)

/**写缓存指令,针对5510芯片 */
#define DEVICE_LCD_DRIVER_IC_WRITE_RAM_CMD			0X2C00

/**设置X坐标指令,针对5510芯片 */
#define DEVICE_LCD_DRIVER_IC_SET_X_CMD				0X2A00

/**设置Y坐标指令,针对5510芯片 */
#define DEVICE_LCD_DRIVER_IC_SET_Y_CMD				0X2B00

//m^n函数
//返回值:m^n次方.
static uint32_t DeviceLCD_Pow(uint8_t m,uint8_t n)
{
	uint32_t result=1;
	while(n--)
		result*=m;
	return result;
}


//写寄存器地址函数
static void DeviceLCD_WriteRegAddrWithOutData(volatile uint16_t regval)
{
	//使用-O2优化的时候,必须插入的延时
	regval=regval;
	//写入要写的寄存器序号
	DeviceLCD->DeviceLCD_Reg=regval;
}

//写LCD数据
static void DeviceLCD_WriteDataWithOutRegAddr(volatile uint16_t data)
{
	//使用-O2优化的时候,必须插入的延时
	data=data;
	DeviceLCD->DeviceLCD_Ram=data;
}

//读LCD数据
uint16_t DeviceLCD_ReadDataWithOutRegAddr(void)
{
	//防止被优化
	volatile uint16_t ram;
	ram=DeviceLCD->DeviceLCD_Ram;
	return ram;
}

/**写指定寄存器地址的指定寄存器数据 */
void DeviceLCD_WriteRegWithData(uint16_t regAddr,uint16_t regValue)
{
	//写入要写的寄存器序号
	DeviceLCD->DeviceLCD_Reg = regAddr;
	//写入数据
	DeviceLCD->DeviceLCD_Ram = regValue;
}

/**读指定寄存器数据 */
uint16_t DeviceLCD_ReadDataWithRegAddr(uint16_t redAddr)
{
	//写入要读的寄存器序号
	DeviceLCD_WriteRegAddrWithOutData(redAddr);
	Device_LCD_WAIT_US(LCD_READ_REG_WAIT_TIME);
	//返回读到的值
	return DeviceLCD_ReadDataWithOutRegAddr();
}


/**IO初始化 */
static void DeviceLCD_IO_Init()
{
	vu32 i=0;

	GPIO_InitTypeDef  GPIO_InitStructure;
	FSMC_NORSRAMInitTypeDef  FSMC_NORSRAMInitStructure;
	FSMC_NORSRAMTimingInitTypeDef  readWriteTiming;
	FSMC_NORSRAMTimingInitTypeDef  writeTiming;
	//使能PD,PE,PF,PG时钟
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB|RCC_AHB1Periph_GPIOD|RCC_AHB1Periph_GPIOE|RCC_AHB1Periph_GPIOF|RCC_AHB1Periph_GPIOG, ENABLE);
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
	//初始化 //PB15 推挽输出,控制背光
	GPIO_Init(GPIOB, &GPIO_InitStructure);

	//PD0,1,4,5,8,9,10,14,15 AF OUT
	GPIO_InitStructure.GPIO_Pin = (3<<0)|(3<<4)|(7<<8)|(3<<14);
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
	//PE7~15,AF OUT
	GPIO_InitStructure.GPIO_Pin = (0X1FF<<7);
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

	//PF12,FSMC_A6
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12;
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

	//PF12,FSMC_A6
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12;
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

	//PD0,AF12
	GPIO_PinAFConfig(GPIOD,GPIO_PinSource0,GPIO_AF_FSMC);
	//PD1,AF12
	GPIO_PinAFConfig(GPIOD,GPIO_PinSource1,GPIO_AF_FSMC);
	GPIO_PinAFConfig(GPIOD,GPIO_PinSource4,GPIO_AF_FSMC);
	GPIO_PinAFConfig(GPIOD,GPIO_PinSource5,GPIO_AF_FSMC);
	GPIO_PinAFConfig(GPIOD,GPIO_PinSource8,GPIO_AF_FSMC);
	GPIO_PinAFConfig(GPIOD,GPIO_PinSource9,GPIO_AF_FSMC);
	GPIO_PinAFConfig(GPIOD,GPIO_PinSource10,GPIO_AF_FSMC);
	GPIO_PinAFConfig(GPIOD,GPIO_PinSource14,GPIO_AF_FSMC);
	//PD15,AF12
	GPIO_PinAFConfig(GPIOD,GPIO_PinSource15,GPIO_AF_FSMC);
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
	//PF12,AF12
	GPIO_PinAFConfig(GPIOF,GPIO_PinSource12,GPIO_AF_FSMC);
	GPIO_PinAFConfig(GPIOG,GPIO_PinSource12,GPIO_AF_FSMC);

	//地址建立时间（ADDSET）为16个HCLK 1/168M=6ns*16=96ns
	readWriteTiming.FSMC_AddressSetupTime = 0XF;
	//地址保持时间（ADDHLD）模式A未用到
	readWriteTiming.FSMC_AddressHoldTime = 0x00;
	//数据保存时间为60个HCLK	=6*60=360ns
	readWriteTiming.FSMC_DataSetupTime = 60;
	readWriteTiming.FSMC_BusTurnAroundDuration = 0x00;
	readWriteTiming.FSMC_CLKDivision = 0x00;
	readWriteTiming.FSMC_DataLatency = 0x00;
	//模式A
	readWriteTiming.FSMC_AccessMode = FSMC_AccessMode_A;

	//地址建立时间（ADDSET）为9个HCLK =54ns
	writeTiming.FSMC_AddressSetupTime =9;
	//地址保持时间（A
	writeTiming.FSMC_AddressHoldTime = 0x00;
	//数据保存时间为6ns*9个HCLK=54ns
	writeTiming.FSMC_DataSetupTime = 8;
	writeTiming.FSMC_BusTurnAroundDuration = 0x00;
	writeTiming.FSMC_CLKDivision = 0x00;
	writeTiming.FSMC_DataLatency = 0x00;
	//模式A
	writeTiming.FSMC_AccessMode = FSMC_AccessMode_A;

	//  这里我们使用NE4 ，也就对应BTCR[6],[7]。
	FSMC_NORSRAMInitStructure.FSMC_Bank = FSMC_Bank1_NORSRAM4;
	// 不复用数据地址
	FSMC_NORSRAMInitStructure.FSMC_DataAddressMux = FSMC_DataAddressMux_Disable;
	// FSMC_MemoryType_SRAM;  //SRAM
	FSMC_NORSRAMInitStructure.FSMC_MemoryType =FSMC_MemoryType_SRAM;
	//存储器数据宽度为16bit
	FSMC_NORSRAMInitStructure.FSMC_MemoryDataWidth = FSMC_MemoryDataWidth_16b;
	// FSMC_BurstAccessMode_Disable;
	FSMC_NORSRAMInitStructure.FSMC_BurstAccessMode =FSMC_BurstAccessMode_Disable;
	FSMC_NORSRAMInitStructure.FSMC_WaitSignalPolarity = FSMC_WaitSignalPolarity_Low;
	FSMC_NORSRAMInitStructure.FSMC_AsynchronousWait=FSMC_AsynchronousWait_Disable;
	FSMC_NORSRAMInitStructure.FSMC_WrapMode = FSMC_WrapMode_Disable;
	FSMC_NORSRAMInitStructure.FSMC_WaitSignalActive = FSMC_WaitSignalActive_BeforeWaitState;
	//  存储器写使能
	FSMC_NORSRAMInitStructure.FSMC_WriteOperation = FSMC_WriteOperation_Enable;
	FSMC_NORSRAMInitStructure.FSMC_WaitSignal = FSMC_WaitSignal_Disable;
	// 读写使用不同的时序
	FSMC_NORSRAMInitStructure.FSMC_ExtendedMode = FSMC_ExtendedMode_Enable;
	FSMC_NORSRAMInitStructure.FSMC_WriteBurst = FSMC_WriteBurst_Disable;
	//写时序
	FSMC_NORSRAMInitStructure.FSMC_ReadWriteTimingStruct = &readWriteTiming;
	//写时序
	FSMC_NORSRAMInitStructure.FSMC_WriteTimingStruct = &writeTiming;
	//初始化FSMC配置
	FSMC_NORSRAMInit(&FSMC_NORSRAMInitStructure);
	// 使能BANK1
	FSMC_NORSRAMCmd(FSMC_Bank1_NORSRAM4, ENABLE);
	//等待FSMC稳定
 	DEVICE_LCD_WAIT_MS(LCD_FSMC_WAIT_START_TIME);
 	DeviceLCD_WriteRegWithData(0x0000,0x0001);
	//等待FSMC稳定
	DEVICE_LCD_WAIT_MS(LCD_FSMC_WAIT_START_TIME);
}


/**读取芯片的ID */
static uint16_t DeviceLCD_ReadID()
{
	uint16_t deviceID = 0;
	deviceID = DeviceLCD_ReadDataWithRegAddr(0x0000);
	//读到ID不正确,新增lcddev.id==0X9300判断，因为9341在未被复位的情况下会被读成9300
   	if(deviceID < 0XFF||deviceID == 0XFFFF||deviceID == 0X9300)
	{
		//尝试9341 ID的读取
		DeviceLCD_WriteRegAddrWithOutData(0XD3);
		//dummy read
		deviceID = DeviceLCD_ReadDataWithOutRegAddr();
		//读到0X00
 		deviceID = DeviceLCD_ReadDataWithOutRegAddr();
		//读取93
  		deviceID = DeviceLCD_ReadDataWithOutRegAddr();
 		deviceID <<= 8;
		//读取41
		deviceID |= DeviceLCD_ReadDataWithOutRegAddr();
		//非9341,尝试是不是6804
		if(deviceID != 0X9341)
		{
			DeviceLCD_WriteRegAddrWithOutData(0XBF);
			//dummy read
			deviceID = DeviceLCD_ReadDataWithOutRegAddr();
			//读回0X01
	 		deviceID = DeviceLCD_ReadDataWithOutRegAddr();
			//读回0XD0
	 		deviceID = DeviceLCD_ReadDataWithOutRegAddr();
			//这里读回0X68
	  		deviceID = DeviceLCD_ReadDataWithOutRegAddr();
			deviceID <<= 8;
			//这里读回0X04
	  		deviceID |= DeviceLCD_ReadDataWithOutRegAddr();
			//也不是6804,尝试看看是不是NT35310
			if(deviceID != 0X6804)
			{
				DeviceLCD_WriteRegAddrWithOutData(0XD4);
				//dummy read
				deviceID = DeviceLCD_ReadDataWithOutRegAddr();
				//读回0X01
				deviceID = DeviceLCD_ReadDataWithOutRegAddr();
				//读回0X53
				deviceID = DeviceLCD_ReadDataWithOutRegAddr();
				deviceID <<= 8;
				//这里读回0X10
				deviceID |= DeviceLCD_ReadDataWithOutRegAddr();
				//也不是NT35310,尝试看看是不是NT35510
				if(deviceID != 0X5310)
				{
					DeviceLCD_WriteRegAddrWithOutData(0XDA00);
					//读回0X00
					deviceID =DeviceLCD_ReadDataWithOutRegAddr();
					DeviceLCD_WriteRegAddrWithOutData(0XDB00);
					//读回0X80
					deviceID = DeviceLCD_ReadDataWithOutRegAddr();
					deviceID <<= 8;
					DeviceLCD_WriteRegAddrWithOutData(0XDC00);
					//读回0X00
					deviceID |= DeviceLCD_ReadDataWithOutRegAddr();
					//NT35510读回的ID是8000H,为方便区分,我们强制设置为5510
					if(deviceID == 0x8000)
						deviceID = 0x5510;
					//也不是NT5510,尝试看看是不是SSD1963
					if(deviceID != 0X5510)
					{
						DeviceLCD_WriteRegAddrWithOutData(0XA1);
						deviceID = DeviceLCD_ReadDataWithOutRegAddr();
						//读回0X57
						deviceID = DeviceLCD_ReadDataWithOutRegAddr();
						deviceID <<= 8;
						//读回0X61
						deviceID |= DeviceLCD_ReadDataWithOutRegAddr();
						//SSD1963读回的ID是5761H,为方便区分,我们强制设置为1963
						if(deviceID == 0X5761)
							deviceID = 0X1963;
					}
				}
			}
		}
	}
	return deviceID;
}

static void Device_LCD_FSMC_SpeedReConfig()
{
	/**设置WR时序最快 */
	//重新配置写时序控制寄存器的时序
	//地址建立时间(ADDSET)清零
	FSMC_Bank1E->BWTR[6]&=~(0XF<<0);
	//数据保存时间清零
	FSMC_Bank1E->BWTR[6]&=~(0XF<<8);
	//地址建立时间(ADDSET)为3个HCLK =18ns
	FSMC_Bank1E->BWTR[6]|=3<<0;
	//数据保存时间(DATAST)为6ns*3个HCLK=18ns
	FSMC_Bank1E->BWTR[6]|=2<<8;
}

/**5510芯片的初始化 */

/**配置SSD芯片 */
static void DeviceLCD_SSD_5510_Init(void)
{
	DeviceLCD_WriteRegWithData(0xF000,0x55);
	DeviceLCD_WriteRegWithData(0xF001,0xAA);
	DeviceLCD_WriteRegWithData(0xF002,0x52);
	DeviceLCD_WriteRegWithData(0xF003,0x08);
	DeviceLCD_WriteRegWithData(0xF004,0x01);
	//AVDD Set AVDD 5.2V
	DeviceLCD_WriteRegWithData(0xB000,0x0D);
	DeviceLCD_WriteRegWithData(0xB001,0x0D);
	DeviceLCD_WriteRegWithData(0xB002,0x0D);
	//AVDD ratio
	DeviceLCD_WriteRegWithData(0xB600,0x34);
	DeviceLCD_WriteRegWithData(0xB601,0x34);
	DeviceLCD_WriteRegWithData(0xB602,0x34);
	//AVEE -5.2V
	DeviceLCD_WriteRegWithData(0xB100,0x0D);
	DeviceLCD_WriteRegWithData(0xB101,0x0D);
	DeviceLCD_WriteRegWithData(0xB102,0x0D);
	//AVEE ratio
	DeviceLCD_WriteRegWithData(0xB700,0x34);
	DeviceLCD_WriteRegWithData(0xB701,0x34);
	DeviceLCD_WriteRegWithData(0xB702,0x34);
	//VCL -2.5V
	DeviceLCD_WriteRegWithData(0xB200,0x00);
	DeviceLCD_WriteRegWithData(0xB201,0x00);
	DeviceLCD_WriteRegWithData(0xB202,0x00);
	//VCL ratio
	DeviceLCD_WriteRegWithData(0xB800,0x24);
	DeviceLCD_WriteRegWithData(0xB801,0x24);
	DeviceLCD_WriteRegWithData(0xB802,0x24);
	//VGH 15V (Free pump)
	DeviceLCD_WriteRegWithData(0xBF00,0x01);
	DeviceLCD_WriteRegWithData(0xB300,0x0F);
	DeviceLCD_WriteRegWithData(0xB301,0x0F);
	DeviceLCD_WriteRegWithData(0xB302,0x0F);
	//VGH ratio
	DeviceLCD_WriteRegWithData(0xB900,0x34);
	DeviceLCD_WriteRegWithData(0xB901,0x34);
	DeviceLCD_WriteRegWithData(0xB902,0x34);
	//VGL_REG -10V
	DeviceLCD_WriteRegWithData(0xB500,0x08);
	DeviceLCD_WriteRegWithData(0xB501,0x08);
	DeviceLCD_WriteRegWithData(0xB502,0x08);
	DeviceLCD_WriteRegWithData(0xC200,0x03);
	//VGLX ratio
	DeviceLCD_WriteRegWithData(0xBA00,0x24);
	DeviceLCD_WriteRegWithData(0xBA01,0x24);
	DeviceLCD_WriteRegWithData(0xBA02,0x24);
	//VGMP/VGSP 4.5V/0V
	DeviceLCD_WriteRegWithData(0xBC00,0x00);
	DeviceLCD_WriteRegWithData(0xBC01,0x78);
	DeviceLCD_WriteRegWithData(0xBC02,0x00);
	//VGMN/VGSN -4.5V/0V
	DeviceLCD_WriteRegWithData(0xBD00,0x00);
	DeviceLCD_WriteRegWithData(0xBD01,0x78);
	DeviceLCD_WriteRegWithData(0xBD02,0x00);
	//VCOM
	DeviceLCD_WriteRegWithData(0xBE00,0x00);
	DeviceLCD_WriteRegWithData(0xBE01,0x64);
	//Gamma Setting
	DeviceLCD_WriteRegWithData(0xD100,0x00);
	DeviceLCD_WriteRegWithData(0xD101,0x33);
	DeviceLCD_WriteRegWithData(0xD102,0x00);
	DeviceLCD_WriteRegWithData(0xD103,0x34);
	DeviceLCD_WriteRegWithData(0xD104,0x00);
	DeviceLCD_WriteRegWithData(0xD105,0x3A);
	DeviceLCD_WriteRegWithData(0xD106,0x00);
	DeviceLCD_WriteRegWithData(0xD107,0x4A);
	DeviceLCD_WriteRegWithData(0xD108,0x00);
	DeviceLCD_WriteRegWithData(0xD109,0x5C);
	DeviceLCD_WriteRegWithData(0xD10A,0x00);
	DeviceLCD_WriteRegWithData(0xD10B,0x81);
	DeviceLCD_WriteRegWithData(0xD10C,0x00);
	DeviceLCD_WriteRegWithData(0xD10D,0xA6);
	DeviceLCD_WriteRegWithData(0xD10E,0x00);
	DeviceLCD_WriteRegWithData(0xD10F,0xE5);
	DeviceLCD_WriteRegWithData(0xD110,0x01);
	DeviceLCD_WriteRegWithData(0xD111,0x13);
	DeviceLCD_WriteRegWithData(0xD112,0x01);
	DeviceLCD_WriteRegWithData(0xD113,0x54);
	DeviceLCD_WriteRegWithData(0xD114,0x01);
	DeviceLCD_WriteRegWithData(0xD115,0x82);
	DeviceLCD_WriteRegWithData(0xD116,0x01);
	DeviceLCD_WriteRegWithData(0xD117,0xCA);
	DeviceLCD_WriteRegWithData(0xD118,0x02);
	DeviceLCD_WriteRegWithData(0xD119,0x00);
	DeviceLCD_WriteRegWithData(0xD11A,0x02);
	DeviceLCD_WriteRegWithData(0xD11B,0x01);
	DeviceLCD_WriteRegWithData(0xD11C,0x02);
	DeviceLCD_WriteRegWithData(0xD11D,0x34);
	DeviceLCD_WriteRegWithData(0xD11E,0x02);
	DeviceLCD_WriteRegWithData(0xD11F,0x67);
	DeviceLCD_WriteRegWithData(0xD120,0x02);
	DeviceLCD_WriteRegWithData(0xD121,0x84);
	DeviceLCD_WriteRegWithData(0xD122,0x02);
	DeviceLCD_WriteRegWithData(0xD123,0xA4);
	DeviceLCD_WriteRegWithData(0xD124,0x02);
	DeviceLCD_WriteRegWithData(0xD125,0xB7);
	DeviceLCD_WriteRegWithData(0xD126,0x02);
	DeviceLCD_WriteRegWithData(0xD127,0xCF);
	DeviceLCD_WriteRegWithData(0xD128,0x02);
	DeviceLCD_WriteRegWithData(0xD129,0xDE);
	DeviceLCD_WriteRegWithData(0xD12A,0x02);
	DeviceLCD_WriteRegWithData(0xD12B,0xF2);
	DeviceLCD_WriteRegWithData(0xD12C,0x02);
	DeviceLCD_WriteRegWithData(0xD12D,0xFE);
	DeviceLCD_WriteRegWithData(0xD12E,0x03);
	DeviceLCD_WriteRegWithData(0xD12F,0x10);
	DeviceLCD_WriteRegWithData(0xD130,0x03);
	DeviceLCD_WriteRegWithData(0xD131,0x33);
	DeviceLCD_WriteRegWithData(0xD132,0x03);
	DeviceLCD_WriteRegWithData(0xD133,0x6D);
	DeviceLCD_WriteRegWithData(0xD200,0x00);
	DeviceLCD_WriteRegWithData(0xD201,0x33);
	DeviceLCD_WriteRegWithData(0xD202,0x00);
	DeviceLCD_WriteRegWithData(0xD203,0x34);
	DeviceLCD_WriteRegWithData(0xD204,0x00);
	DeviceLCD_WriteRegWithData(0xD205,0x3A);
	DeviceLCD_WriteRegWithData(0xD206,0x00);
	DeviceLCD_WriteRegWithData(0xD207,0x4A);
	DeviceLCD_WriteRegWithData(0xD208,0x00);
	DeviceLCD_WriteRegWithData(0xD209,0x5C);
	DeviceLCD_WriteRegWithData(0xD20A,0x00);
	DeviceLCD_WriteRegWithData(0xD20B,0x81);
	DeviceLCD_WriteRegWithData(0xD20C,0x00);
	DeviceLCD_WriteRegWithData(0xD20D,0xA6);
	DeviceLCD_WriteRegWithData(0xD20E,0x00);
	DeviceLCD_WriteRegWithData(0xD20F,0xE5);
	DeviceLCD_WriteRegWithData(0xD210,0x01);
	DeviceLCD_WriteRegWithData(0xD211,0x13);
	DeviceLCD_WriteRegWithData(0xD212,0x01);
	DeviceLCD_WriteRegWithData(0xD213,0x54);
	DeviceLCD_WriteRegWithData(0xD214,0x01);
	DeviceLCD_WriteRegWithData(0xD215,0x82);
	DeviceLCD_WriteRegWithData(0xD216,0x01);
	DeviceLCD_WriteRegWithData(0xD217,0xCA);
	DeviceLCD_WriteRegWithData(0xD218,0x02);
	DeviceLCD_WriteRegWithData(0xD219,0x00);
	DeviceLCD_WriteRegWithData(0xD21A,0x02);
	DeviceLCD_WriteRegWithData(0xD21B,0x01);
	DeviceLCD_WriteRegWithData(0xD21C,0x02);
	DeviceLCD_WriteRegWithData(0xD21D,0x34);
	DeviceLCD_WriteRegWithData(0xD21E,0x02);
	DeviceLCD_WriteRegWithData(0xD21F,0x67);
	DeviceLCD_WriteRegWithData(0xD220,0x02);
	DeviceLCD_WriteRegWithData(0xD221,0x84);
	DeviceLCD_WriteRegWithData(0xD222,0x02);
	DeviceLCD_WriteRegWithData(0xD223,0xA4);
	DeviceLCD_WriteRegWithData(0xD224,0x02);
	DeviceLCD_WriteRegWithData(0xD225,0xB7);
	DeviceLCD_WriteRegWithData(0xD226,0x02);
	DeviceLCD_WriteRegWithData(0xD227,0xCF);
	DeviceLCD_WriteRegWithData(0xD228,0x02);
	DeviceLCD_WriteRegWithData(0xD229,0xDE);
	DeviceLCD_WriteRegWithData(0xD22A,0x02);
	DeviceLCD_WriteRegWithData(0xD22B,0xF2);
	DeviceLCD_WriteRegWithData(0xD22C,0x02);
	DeviceLCD_WriteRegWithData(0xD22D,0xFE);
	DeviceLCD_WriteRegWithData(0xD22E,0x03);
	DeviceLCD_WriteRegWithData(0xD22F,0x10);
	DeviceLCD_WriteRegWithData(0xD230,0x03);
	DeviceLCD_WriteRegWithData(0xD231,0x33);
	DeviceLCD_WriteRegWithData(0xD232,0x03);
	DeviceLCD_WriteRegWithData(0xD233,0x6D);
	DeviceLCD_WriteRegWithData(0xD300,0x00);
	DeviceLCD_WriteRegWithData(0xD301,0x33);
	DeviceLCD_WriteRegWithData(0xD302,0x00);
	DeviceLCD_WriteRegWithData(0xD303,0x34);
	DeviceLCD_WriteRegWithData(0xD304,0x00);
	DeviceLCD_WriteRegWithData(0xD305,0x3A);
	DeviceLCD_WriteRegWithData(0xD306,0x00);
	DeviceLCD_WriteRegWithData(0xD307,0x4A);
	DeviceLCD_WriteRegWithData(0xD308,0x00);
	DeviceLCD_WriteRegWithData(0xD309,0x5C);
	DeviceLCD_WriteRegWithData(0xD30A,0x00);
	DeviceLCD_WriteRegWithData(0xD30B,0x81);
	DeviceLCD_WriteRegWithData(0xD30C,0x00);
	DeviceLCD_WriteRegWithData(0xD30D,0xA6);
	DeviceLCD_WriteRegWithData(0xD30E,0x00);
	DeviceLCD_WriteRegWithData(0xD30F,0xE5);
	DeviceLCD_WriteRegWithData(0xD310,0x01);
	DeviceLCD_WriteRegWithData(0xD311,0x13);
	DeviceLCD_WriteRegWithData(0xD312,0x01);
	DeviceLCD_WriteRegWithData(0xD313,0x54);
	DeviceLCD_WriteRegWithData(0xD314,0x01);
	DeviceLCD_WriteRegWithData(0xD315,0x82);
	DeviceLCD_WriteRegWithData(0xD316,0x01);
	DeviceLCD_WriteRegWithData(0xD317,0xCA);
	DeviceLCD_WriteRegWithData(0xD318,0x02);
	DeviceLCD_WriteRegWithData(0xD319,0x00);
	DeviceLCD_WriteRegWithData(0xD31A,0x02);
	DeviceLCD_WriteRegWithData(0xD31B,0x01);
	DeviceLCD_WriteRegWithData(0xD31C,0x02);
	DeviceLCD_WriteRegWithData(0xD31D,0x34);
	DeviceLCD_WriteRegWithData(0xD31E,0x02);
	DeviceLCD_WriteRegWithData(0xD31F,0x67);
	DeviceLCD_WriteRegWithData(0xD320,0x02);
	DeviceLCD_WriteRegWithData(0xD321,0x84);
	DeviceLCD_WriteRegWithData(0xD322,0x02);
	DeviceLCD_WriteRegWithData(0xD323,0xA4);
	DeviceLCD_WriteRegWithData(0xD324,0x02);
	DeviceLCD_WriteRegWithData(0xD325,0xB7);
	DeviceLCD_WriteRegWithData(0xD326,0x02);
	DeviceLCD_WriteRegWithData(0xD327,0xCF);
	DeviceLCD_WriteRegWithData(0xD328,0x02);
	DeviceLCD_WriteRegWithData(0xD329,0xDE);
	DeviceLCD_WriteRegWithData(0xD32A,0x02);
	DeviceLCD_WriteRegWithData(0xD32B,0xF2);
	DeviceLCD_WriteRegWithData(0xD32C,0x02);
	DeviceLCD_WriteRegWithData(0xD32D,0xFE);
	DeviceLCD_WriteRegWithData(0xD32E,0x03);
	DeviceLCD_WriteRegWithData(0xD32F,0x10);
	DeviceLCD_WriteRegWithData(0xD330,0x03);
	DeviceLCD_WriteRegWithData(0xD331,0x33);
	DeviceLCD_WriteRegWithData(0xD332,0x03);
	DeviceLCD_WriteRegWithData(0xD333,0x6D);
	DeviceLCD_WriteRegWithData(0xD400,0x00);
	DeviceLCD_WriteRegWithData(0xD401,0x33);
	DeviceLCD_WriteRegWithData(0xD402,0x00);
	DeviceLCD_WriteRegWithData(0xD403,0x34);
	DeviceLCD_WriteRegWithData(0xD404,0x00);
	DeviceLCD_WriteRegWithData(0xD405,0x3A);
	DeviceLCD_WriteRegWithData(0xD406,0x00);
	DeviceLCD_WriteRegWithData(0xD407,0x4A);
	DeviceLCD_WriteRegWithData(0xD408,0x00);
	DeviceLCD_WriteRegWithData(0xD409,0x5C);
	DeviceLCD_WriteRegWithData(0xD40A,0x00);
	DeviceLCD_WriteRegWithData(0xD40B,0x81);
	DeviceLCD_WriteRegWithData(0xD40C,0x00);
	DeviceLCD_WriteRegWithData(0xD40D,0xA6);
	DeviceLCD_WriteRegWithData(0xD40E,0x00);
	DeviceLCD_WriteRegWithData(0xD40F,0xE5);
	DeviceLCD_WriteRegWithData(0xD410,0x01);
	DeviceLCD_WriteRegWithData(0xD411,0x13);
	DeviceLCD_WriteRegWithData(0xD412,0x01);
	DeviceLCD_WriteRegWithData(0xD413,0x54);
	DeviceLCD_WriteRegWithData(0xD414,0x01);
	DeviceLCD_WriteRegWithData(0xD415,0x82);
	DeviceLCD_WriteRegWithData(0xD416,0x01);
	DeviceLCD_WriteRegWithData(0xD417,0xCA);
	DeviceLCD_WriteRegWithData(0xD418,0x02);
	DeviceLCD_WriteRegWithData(0xD419,0x00);
	DeviceLCD_WriteRegWithData(0xD41A,0x02);
	DeviceLCD_WriteRegWithData(0xD41B,0x01);
	DeviceLCD_WriteRegWithData(0xD41C,0x02);
	DeviceLCD_WriteRegWithData(0xD41D,0x34);
	DeviceLCD_WriteRegWithData(0xD41E,0x02);
	DeviceLCD_WriteRegWithData(0xD41F,0x67);
	DeviceLCD_WriteRegWithData(0xD420,0x02);
	DeviceLCD_WriteRegWithData(0xD421,0x84);
	DeviceLCD_WriteRegWithData(0xD422,0x02);
	DeviceLCD_WriteRegWithData(0xD423,0xA4);
	DeviceLCD_WriteRegWithData(0xD424,0x02);
	DeviceLCD_WriteRegWithData(0xD425,0xB7);
	DeviceLCD_WriteRegWithData(0xD426,0x02);
	DeviceLCD_WriteRegWithData(0xD427,0xCF);
	DeviceLCD_WriteRegWithData(0xD428,0x02);
	DeviceLCD_WriteRegWithData(0xD429,0xDE);
	DeviceLCD_WriteRegWithData(0xD42A,0x02);
	DeviceLCD_WriteRegWithData(0xD42B,0xF2);
	DeviceLCD_WriteRegWithData(0xD42C,0x02);
	DeviceLCD_WriteRegWithData(0xD42D,0xFE);
	DeviceLCD_WriteRegWithData(0xD42E,0x03);
	DeviceLCD_WriteRegWithData(0xD42F,0x10);
	DeviceLCD_WriteRegWithData(0xD430,0x03);
	DeviceLCD_WriteRegWithData(0xD431,0x33);
	DeviceLCD_WriteRegWithData(0xD432,0x03);
	DeviceLCD_WriteRegWithData(0xD433,0x6D);
	DeviceLCD_WriteRegWithData(0xD500,0x00);
	DeviceLCD_WriteRegWithData(0xD501,0x33);
	DeviceLCD_WriteRegWithData(0xD502,0x00);
	DeviceLCD_WriteRegWithData(0xD503,0x34);
	DeviceLCD_WriteRegWithData(0xD504,0x00);
	DeviceLCD_WriteRegWithData(0xD505,0x3A);
	DeviceLCD_WriteRegWithData(0xD506,0x00);
	DeviceLCD_WriteRegWithData(0xD507,0x4A);
	DeviceLCD_WriteRegWithData(0xD508,0x00);
	DeviceLCD_WriteRegWithData(0xD509,0x5C);
	DeviceLCD_WriteRegWithData(0xD50A,0x00);
	DeviceLCD_WriteRegWithData(0xD50B,0x81);
	DeviceLCD_WriteRegWithData(0xD50C,0x00);
	DeviceLCD_WriteRegWithData(0xD50D,0xA6);
	DeviceLCD_WriteRegWithData(0xD50E,0x00);
	DeviceLCD_WriteRegWithData(0xD50F,0xE5);
	DeviceLCD_WriteRegWithData(0xD510,0x01);
	DeviceLCD_WriteRegWithData(0xD511,0x13);
	DeviceLCD_WriteRegWithData(0xD512,0x01);
	DeviceLCD_WriteRegWithData(0xD513,0x54);
	DeviceLCD_WriteRegWithData(0xD514,0x01);
	DeviceLCD_WriteRegWithData(0xD515,0x82);
	DeviceLCD_WriteRegWithData(0xD516,0x01);
	DeviceLCD_WriteRegWithData(0xD517,0xCA);
	DeviceLCD_WriteRegWithData(0xD518,0x02);
	DeviceLCD_WriteRegWithData(0xD519,0x00);
	DeviceLCD_WriteRegWithData(0xD51A,0x02);
	DeviceLCD_WriteRegWithData(0xD51B,0x01);
	DeviceLCD_WriteRegWithData(0xD51C,0x02);
	DeviceLCD_WriteRegWithData(0xD51D,0x34);
	DeviceLCD_WriteRegWithData(0xD51E,0x02);
	DeviceLCD_WriteRegWithData(0xD51F,0x67);
	DeviceLCD_WriteRegWithData(0xD520,0x02);
	DeviceLCD_WriteRegWithData(0xD521,0x84);
	DeviceLCD_WriteRegWithData(0xD522,0x02);
	DeviceLCD_WriteRegWithData(0xD523,0xA4);
	DeviceLCD_WriteRegWithData(0xD524,0x02);
	DeviceLCD_WriteRegWithData(0xD525,0xB7);
	DeviceLCD_WriteRegWithData(0xD526,0x02);
	DeviceLCD_WriteRegWithData(0xD527,0xCF);
	DeviceLCD_WriteRegWithData(0xD528,0x02);
	DeviceLCD_WriteRegWithData(0xD529,0xDE);
	DeviceLCD_WriteRegWithData(0xD52A,0x02);
	DeviceLCD_WriteRegWithData(0xD52B,0xF2);
	DeviceLCD_WriteRegWithData(0xD52C,0x02);
	DeviceLCD_WriteRegWithData(0xD52D,0xFE);
	DeviceLCD_WriteRegWithData(0xD52E,0x03);
	DeviceLCD_WriteRegWithData(0xD52F,0x10);
	DeviceLCD_WriteRegWithData(0xD530,0x03);
	DeviceLCD_WriteRegWithData(0xD531,0x33);
	DeviceLCD_WriteRegWithData(0xD532,0x03);
	DeviceLCD_WriteRegWithData(0xD533,0x6D);
	DeviceLCD_WriteRegWithData(0xD600,0x00);
	DeviceLCD_WriteRegWithData(0xD601,0x33);
	DeviceLCD_WriteRegWithData(0xD602,0x00);
	DeviceLCD_WriteRegWithData(0xD603,0x34);
	DeviceLCD_WriteRegWithData(0xD604,0x00);
	DeviceLCD_WriteRegWithData(0xD605,0x3A);
	DeviceLCD_WriteRegWithData(0xD606,0x00);
	DeviceLCD_WriteRegWithData(0xD607,0x4A);
	DeviceLCD_WriteRegWithData(0xD608,0x00);
	DeviceLCD_WriteRegWithData(0xD609,0x5C);
	DeviceLCD_WriteRegWithData(0xD60A,0x00);
	DeviceLCD_WriteRegWithData(0xD60B,0x81);
	DeviceLCD_WriteRegWithData(0xD60C,0x00);
	DeviceLCD_WriteRegWithData(0xD60D,0xA6);
	DeviceLCD_WriteRegWithData(0xD60E,0x00);
	DeviceLCD_WriteRegWithData(0xD60F,0xE5);
	DeviceLCD_WriteRegWithData(0xD610,0x01);
	DeviceLCD_WriteRegWithData(0xD611,0x13);
	DeviceLCD_WriteRegWithData(0xD612,0x01);
	DeviceLCD_WriteRegWithData(0xD613,0x54);
	DeviceLCD_WriteRegWithData(0xD614,0x01);
	DeviceLCD_WriteRegWithData(0xD615,0x82);
	DeviceLCD_WriteRegWithData(0xD616,0x01);
	DeviceLCD_WriteRegWithData(0xD617,0xCA);
	DeviceLCD_WriteRegWithData(0xD618,0x02);
	DeviceLCD_WriteRegWithData(0xD619,0x00);
	DeviceLCD_WriteRegWithData(0xD61A,0x02);
	DeviceLCD_WriteRegWithData(0xD61B,0x01);
	DeviceLCD_WriteRegWithData(0xD61C,0x02);
	DeviceLCD_WriteRegWithData(0xD61D,0x34);
	DeviceLCD_WriteRegWithData(0xD61E,0x02);
	DeviceLCD_WriteRegWithData(0xD61F,0x67);
	DeviceLCD_WriteRegWithData(0xD620,0x02);
	DeviceLCD_WriteRegWithData(0xD621,0x84);
	DeviceLCD_WriteRegWithData(0xD622,0x02);
	DeviceLCD_WriteRegWithData(0xD623,0xA4);
	DeviceLCD_WriteRegWithData(0xD624,0x02);
	DeviceLCD_WriteRegWithData(0xD625,0xB7);
	DeviceLCD_WriteRegWithData(0xD626,0x02);
	DeviceLCD_WriteRegWithData(0xD627,0xCF);
	DeviceLCD_WriteRegWithData(0xD628,0x02);
	DeviceLCD_WriteRegWithData(0xD629,0xDE);
	DeviceLCD_WriteRegWithData(0xD62A,0x02);
	DeviceLCD_WriteRegWithData(0xD62B,0xF2);
	DeviceLCD_WriteRegWithData(0xD62C,0x02);
	DeviceLCD_WriteRegWithData(0xD62D,0xFE);
	DeviceLCD_WriteRegWithData(0xD62E,0x03);
	DeviceLCD_WriteRegWithData(0xD62F,0x10);
	DeviceLCD_WriteRegWithData(0xD630,0x03);
	DeviceLCD_WriteRegWithData(0xD631,0x33);
	DeviceLCD_WriteRegWithData(0xD632,0x03);
	DeviceLCD_WriteRegWithData(0xD633,0x6D);
	//LV2 Page 0 enable
	DeviceLCD_WriteRegWithData(0xF000,0x55);
	DeviceLCD_WriteRegWithData(0xF001,0xAA);
	DeviceLCD_WriteRegWithData(0xF002,0x52);
	DeviceLCD_WriteRegWithData(0xF003,0x08);
	DeviceLCD_WriteRegWithData(0xF004,0x00);
	//Display control
	DeviceLCD_WriteRegWithData(0xB100, 0xCC);
	DeviceLCD_WriteRegWithData(0xB101, 0x00);
	//Source hold time
	DeviceLCD_WriteRegWithData(0xB600,0x05);
	//Gate EQ control
	DeviceLCD_WriteRegWithData(0xB700,0x70);
	DeviceLCD_WriteRegWithData(0xB701,0x70);
	//Source EQ control (Mode 2)
	DeviceLCD_WriteRegWithData(0xB800,0x01);
	DeviceLCD_WriteRegWithData(0xB801,0x03);
	DeviceLCD_WriteRegWithData(0xB802,0x03);
	DeviceLCD_WriteRegWithData(0xB803,0x03);
	//Inversion mode (2-dot)
	DeviceLCD_WriteRegWithData(0xBC00,0x02);
	DeviceLCD_WriteRegWithData(0xBC01,0x00);
	DeviceLCD_WriteRegWithData(0xBC02,0x00);
	//Timing control 4H w/ 4-delay
	DeviceLCD_WriteRegWithData(0xC900,0xD0);
	DeviceLCD_WriteRegWithData(0xC901,0x02);
	DeviceLCD_WriteRegWithData(0xC902,0x50);
	DeviceLCD_WriteRegWithData(0xC903,0x50);
	DeviceLCD_WriteRegWithData(0xC904,0x50);
	DeviceLCD_WriteRegWithData(0x3500,0x00);
	//16-bit/pixel
	DeviceLCD_WriteRegWithData(0x3A00,0x55);
	DeviceLCD_WriteRegAddrWithOutData(0x1100);
	DEVICE_LCD_WAIT_MS(LCD_WAIT_CONFIG_TIME);
	DeviceLCD_WriteRegAddrWithOutData(0x2900);
}

static void DeviceLCD_InitScanDir(uint8_t scanDir)
{
	uint16_t regValue = 0;
	switch(scanDir)
	{
		case L2R_U2D://从左到右,从上到下
				regValue|=(0<<7)|(0<<6)|(0<<5);
				break;
		case L2R_D2U://从左到右,从下到上
				regValue|=(1<<7)|(0<<6)|(0<<5);
				break;
		case R2L_U2D://从右到左,从上到下
				regValue|=(0<<7)|(1<<6)|(0<<5);
				break;
		case R2L_D2U://从右到左,从下到上
				regValue|=(1<<7)|(1<<6)|(0<<5);
				break;
		case U2D_L2R://从上到下,从左到右
				regValue|=(0<<7)|(0<<6)|(1<<5);
				break;
		case U2D_R2L://从上到下,从右到左
				regValue|=(0<<7)|(1<<6)|(1<<5);
				break;
		case D2U_L2R://从下到上,从左到右
				regValue|=(1<<7)|(0<<6)|(1<<5);
				break;
		case D2U_R2L://从下到上,从右到左
				regValue|=(1<<7)|(1<<6)|(1<<5);
				break;
	}
	/**扫描方向设置 */
	DeviceLCD_WriteRegWithData(0X3600,regValue);
	DeviceLCD_WriteRegAddrWithOutData(DEVICE_LCD_DRIVER_IC_SET_X_CMD);
	DeviceLCD_WriteDataWithOutRegAddr(0);
	DeviceLCD_WriteRegAddrWithOutData(DEVICE_LCD_DRIVER_IC_SET_X_CMD+1);
	DeviceLCD_WriteDataWithOutRegAddr(0);
	DeviceLCD_WriteRegAddrWithOutData(DEVICE_LCD_DRIVER_IC_SET_X_CMD+2);
	DeviceLCD_WriteDataWithOutRegAddr((DEVICE_LCD_WIDTH-1)>>8);
	DeviceLCD_WriteRegAddrWithOutData(DEVICE_LCD_DRIVER_IC_SET_X_CMD+3);
	DeviceLCD_WriteDataWithOutRegAddr((DEVICE_LCD_WIDTH-1)&0XFF);
	DeviceLCD_WriteRegAddrWithOutData(DEVICE_LCD_DRIVER_IC_SET_Y_CMD);
	DeviceLCD_WriteDataWithOutRegAddr(0);
	DeviceLCD_WriteRegAddrWithOutData(DEVICE_LCD_DRIVER_IC_SET_Y_CMD+1);
	DeviceLCD_WriteDataWithOutRegAddr(0);
	DeviceLCD_WriteRegAddrWithOutData(DEVICE_LCD_DRIVER_IC_SET_Y_CMD+2);
	DeviceLCD_WriteDataWithOutRegAddr((DEVICE_LCD_HEIGHT-1)>>8);
	DeviceLCD_WriteRegAddrWithOutData(DEVICE_LCD_DRIVER_IC_SET_Y_CMD+3);
	DeviceLCD_WriteDataWithOutRegAddr((DEVICE_LCD_HEIGHT-1)&0XFF);
}

//设置光标位置
//Xpos:横坐标
//Ypos:纵坐标
static void DeviceLCD_SetCursor(uint16_t Xpos, uint16_t Ypos)
{
	DeviceLCD_WriteRegAddrWithOutData(DEVICE_LCD_DRIVER_IC_SET_X_CMD);
	DeviceLCD_WriteDataWithOutRegAddr(Xpos>>8);
	DeviceLCD_WriteRegAddrWithOutData(DEVICE_LCD_DRIVER_IC_SET_X_CMD+1);
	DeviceLCD_WriteDataWithOutRegAddr(Xpos&0XFF);
	DeviceLCD_WriteRegAddrWithOutData(DEVICE_LCD_DRIVER_IC_SET_Y_CMD);
	DeviceLCD_WriteDataWithOutRegAddr(Ypos>>8);
	DeviceLCD_WriteRegAddrWithOutData(DEVICE_LCD_DRIVER_IC_SET_Y_CMD+1);
	DeviceLCD_WriteDataWithOutRegAddr(Ypos&0XFF);
}

//开始写GRAM
static void DeviceLCD_WriteRAM_Prepare(void)
{
 	DeviceLCD->DeviceLCD_Reg = DEVICE_LCD_DRIVER_IC_WRITE_RAM_CMD;
}

void DeviceLCD_Clear(uint16_t color)
{
	uint32_t index=0;
	uint32_t totalpoint=DEVICE_LCD_WIDTH;
	//得到总点数
	totalpoint*=DEVICE_LCD_HEIGHT;
	DeviceLCD_SetCursor(0x00,0x0000);
	//开始写入GRAM
	DeviceLCD_WriteRAM_Prepare();
	for(index=0;index<totalpoint;index++)
	{
		DeviceLCD->DeviceLCD_Ram=color;
	}
}

uint8_t DeviceLCD_Init()
{
	uint16_t deviceID = 0;
	/**IO初始化 */
	DeviceLCD_IO_Init();
	deviceID = DeviceLCD_ReadID();
	/**ID匹配 */
	if(deviceID == DEVICE_LCD_DRIVER_IC_ID)
	{
		/**设置FSMC总线 */
		Device_LCD_FSMC_SpeedReConfig();
		/**初始化驱动IC */
		DeviceLCD_SSD_5510_Init();
		/**设定屏幕尺寸和扫描方向 */
		DeviceLCD_InitScanDir(DFT_SCAN_DIR);
		/**点亮背光 */
		DeviceLCD_BackLightSet(1);
		/**清除屏幕 */
		DeviceLCD_Clear(WHITE);
		return 0;
	}
	else
	{
		return 1;
	}
}

//当mdk -O1时间优化时需要设置
//延时i
static void opt_delay(u8 i)
{
	while(i--);
}

//读取个某点的颜色值
//x,y:坐标
//返回值:此点的颜色
uint16_t DeviceLCD_ReadPoint(u16 x,u16 y)
{
	uint16_t r=0,g=0,b=0;
	//坐标超过了范围,直接返回
	if(x >= DEVICE_LCD_WIDTH || y >= DEVICE_LCD_HEIGHT)
		return 0;
	DeviceLCD_SetCursor(x,y);
	DeviceLCD_WriteRegAddrWithOutData(0X2E00);
	//dummy Read
 	r=DeviceLCD_ReadDataWithOutRegAddr();
	opt_delay(2);
	//实际坐标颜色
 	r=DeviceLCD_ReadDataWithOutRegAddr();
	opt_delay(2);
	b=DeviceLCD_ReadDataWithOutRegAddr();
	//对于9341/5310/5510,第一次读取的是RG的值,R在前,G在后,各占8位
	g=r&0XFF;
	g<<=8;
	return (((r>>11)<<11)|((g>>10)<<5)|(b>>11));
}

//LCD开启显示
void DeviceLCD_DisplayOn(void)
{
	DeviceLCD_WriteRegAddrWithOutData(0X2900);
}

//LCD关闭显示
void DeviceLCD_DisplayOff(void)
{
	DeviceLCD_WriteRegAddrWithOutData(0X2800);
}

//画点
//x,y:坐标
//POINT_COLOR:此点的颜色
void DeviceLCD_DrawPoint(uint16_t x,uint16_t y,uint16_t color)
{
	//设置光标位置
	DeviceLCD_SetCursor(x,y);
	//开始写入GRAM
	DeviceLCD_WriteRAM_Prepare();
	DeviceLCD->DeviceLCD_Ram=color;
}


//快速画点
//x,y:坐标
//color:颜色
void DeviceLCD_FastDrawPoint(uint16_t x,uint16_t y,uint16_t color)
{
	DeviceLCD_WriteRegAddrWithOutData(DEVICE_LCD_DRIVER_IC_SET_X_CMD);
	DeviceLCD_WriteDataWithOutRegAddr(x>>8);
	DeviceLCD_WriteRegAddrWithOutData(DEVICE_LCD_DRIVER_IC_SET_X_CMD+1);
	DeviceLCD_WriteDataWithOutRegAddr(x&0XFF);
	DeviceLCD_WriteRegAddrWithOutData(DEVICE_LCD_DRIVER_IC_SET_Y_CMD);
	DeviceLCD_WriteDataWithOutRegAddr(y>>8);
	DeviceLCD_WriteRegAddrWithOutData(DEVICE_LCD_DRIVER_IC_SET_Y_CMD+1);
	DeviceLCD_WriteDataWithOutRegAddr(y&0XFF);
	DeviceLCD->DeviceLCD_Reg=DEVICE_LCD_DRIVER_IC_WRITE_RAM_CMD;
	DeviceLCD->DeviceLCD_Ram=color;
}

//在指定区域内填充单个颜色
//(sx,sy),(ex,ey):填充矩形对角坐标,区域大小为:(ex-sx+1)*(ey-sy+1)
//color:要填充的颜色
void DeviceLCD_FillSingleColor(uint16_t sx,uint16_t sy,uint16_t ex,uint16_t ey,uint16_t color)
{
	uint16_t i,j;
	uint16_t xlen=0;
	xlen=ex-sx+1;
	for(i=sy;i<=ey;i++)
	{
		//设置光标位置
		DeviceLCD_SetCursor(sx,i);
		//开始写入GRAM
		DeviceLCD_WriteRAM_Prepare();
		//显示颜色
		for(j=0;j<xlen;j++)
			DeviceLCD->DeviceLCD_Ram=color;
	}
}

//在指定区域内填充指定颜色块
//(sx,sy),(ex,ey):填充矩形对角坐标,区域大小为:(ex-sx+1)*(ey-sy+1)
//color:要填充的颜色
void DeviceLCD_FillColorBuffer(uint16_t sx,uint16_t sy,uint16_t ex,uint16_t ey,uint16_t *color)
{
	uint16_t height,width;
	uint16_t i,j;
	//得到填充的宽度
	width=ex-sx+1;
	//高度
	height=ey-sy+1;
 	for(i=0;i<height;i++)
	{
		//设置光标位置
 		DeviceLCD_SetCursor(sx,sy+i);
		//开始写入GRAM
		DeviceLCD_WriteRAM_Prepare();
		//写入数据
		for(j=0;j<width;j++)
			DeviceLCD->DeviceLCD_Ram=color[i*width+j];
	}
}


//画线
//x1,y1:起点坐标
//x2,y2:终点坐标
void DeviceLCD_DrawLine(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2,uint16_t color)
{
	uint16_t t;
	int xerr=0,yerr=0,delta_x,delta_y,distance;
	int incx,incy,uRow,uCol;
	//计算坐标增量
	delta_x=x2-x1;
	delta_y=y2-y1;
	uRow=x1;
	uCol=y1;
	//设置单步方向
	if(delta_x>0)
		incx=1;
	//垂直线
	else if(delta_x==0)
		incx=0;
	else
	{
		incx=-1;
		delta_x=-delta_x;
	}
	if(delta_y>0)
		incy=1;
	//水平线
	else if(delta_y==0)
		incy=0;
	else
	{
		incy=-1;
		delta_y=-delta_y;
	}
	//选取基本增量坐标轴
	if( delta_x>delta_y)
		distance=delta_x;
	else
		distance=delta_y;
	//画线输出
	for(t=0;t<=distance+1;t++ )
	{
		//画点
		DeviceLCD_DrawPoint(uRow,uCol,color);
		xerr+=delta_x ;
		yerr+=delta_y ;
		if(xerr>distance)
		{
			xerr-=distance;
			uRow+=incx;
		}
		if(yerr>distance)
		{
			yerr-=distance;
			uCol+=incy;
		}
	}
}

//画矩形
//(x1,y1),(x2,y2):矩形的对角坐标
void DeviceLCD_DrawRectangle(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2,uint16_t color)
{
	DeviceLCD_DrawLine(x1,y1,x2,y1,color);
	DeviceLCD_DrawLine(x1,y1,x1,y2,color);
	DeviceLCD_DrawLine(x1,y2,x2,y2,color);
	DeviceLCD_DrawLine(x2,y1,x2,y2,color);
}



//在指定位置显示一个字符
//x,y:起始坐标
//num:要显示的字符:" "--->"~"
//显示黑底白字
void DeviceLCD_ShowChar(uint16_t x,uint16_t y,uint8_t num)
{
    uint8_t temp,t1,t;
	uint16_t y0=y;
	//得到字体一个字符对应点阵集所占的字节数
	uint8_t csize=(12/8+((12%8)?1:0))*(12/2);
	//得到偏移后的值（ASCII字库是从空格开始取模，所以-' '就是对应字符的字库）
 	num=num - ' ';
	for(t=0;t<csize;t++)
	{
		temp=LCD_FONT_1206[num][t];
		for(t1=0;t1<8;t1++)
		{
			if(temp&0x80)
				DeviceLCD_FastDrawPoint(x,y,WHITE);
			else
				DeviceLCD_FastDrawPoint(x,y,BLACK);
			temp<<=1;
			y++;
			//超区域了
			if(y>=DEVICE_LCD_HEIGHT)
				return;
			if((y-y0)==12)
			{
				y=y0;
				x++;
				//超区域了
				if(x>=DEVICE_LCD_WIDTH)
					return;
				break;
			}
		}
	}
}

//显示数字,高位为0,则不显示
//x,y :起点坐标
//len :数字的位数
//num:数值(0~4294967295);
void DeviceLCD_ShowNum(uint16_t x,uint16_t y,uint32_t num,uint8_t len)
{
	uint8_t t,temp;
	uint8_t enshow=0;
	for(t=0;t<len;t++)
	{
		temp=(num/DeviceLCD_Pow(10,len-t-1))%10;
		if(enshow==0&&t<(len-1))
		{
			if(temp==0)
			{
				DeviceLCD_ShowChar(x+(12/2)*t,y,' ');
				continue;
			}
			else
				enshow=1;

		}
	 	DeviceLCD_ShowChar(x+(12/2)*t,y,temp+'0');
	}
}


//显示字符串
//x,y:起点坐标
//*p:字符串起始地址		  
void DeviceLCD_ShowString(uint16_t x,uint16_t y,uint8_t *p)
{         
	u8 x0=x;
    while((*p<='~')&&(*p>=' '))//判断是不是非法字符!
    {       
        if(x>=DEVICE_LCD_WIDTH)
		{
			x=x0;
			y+=12;
		}
		//超出范围,退出
        if(y>=DEVICE_LCD_HEIGHT)
			break;
        DeviceLCD_ShowChar(x,y,*p);
        x+=6;
        p++;
    }  
}










































