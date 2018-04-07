#include "DeviceSD.h"
#include "SystemUtil.h"
#include "CoreTickDelay.h"
#include "CoreSPI.h"

// SD卡指令表
#define DEVICE_SD_CMD0    0       //卡复位
#define DEVICE_SD_CMD1    1
#define DEVICE_SD_CMD8    8       //命令8 ，SEND_IF_COND
#define DEVICE_SD_CMD9    9       //命令9 ，读CSD数据
#define DEVICE_SD_CMD10   10      //命令10，读CID数据
#define DEVICE_SD_CMD12   12      //命令12，停止数据传输
#define DEVICE_SD_CMD16   16      //命令16，设置SectorSize 应返回0x00
#define DEVICE_SD_CMD17   17      //命令17，读sector
#define DEVICE_SD_CMD18   18      //命令18，读Multi sector
#define DEVICE_SD_CMD23   23      //命令23，设置多sector写入前预先擦除N个block
#define DEVICE_SD_CMD24   24      //命令24，写sector
#define DEVICE_SD_CMD25   25      //命令25，写Multi sector
#define DEVICE_SD_CMD41   41      //命令41，应返回0x00
#define DEVICE_SD_CMD55   55      //命令55，应返回0x01
#define DEVICE_SD_CMD58   58      //命令58，读OCR信息
#define DEVICE_SD_CMD59   59      //命令59，使能/禁止CRC，应返回0x00

//数据写入回应字意义
#define DEVICE_SD_MSD_DATA_OK                0x05
#define DEVICE_SD_MSD_DATA_CRC_ERROR         0x0B
#define DEVICE_SD_MSD_DATA_WRITE_ERROR       0x0D
#define DEVICE_SD_MSD_DATA_OTHER_ERROR       0xFF

//SD卡回应标记字
#define DEVICE_SD_MSD_RESPONSE_NO_ERROR      0x00
#define DEVICE_SD_MSD_IN_IDLE_STATE          0x01
#define DEVICE_SD_MSD_ERASE_RESET            0x02
#define DEVICE_SD_MSD_ILLEGAL_COMMAND        0x04
#define DEVICE_SD_MSD_COM_CRC_ERROR          0x08
#define DEVICE_SD_MSD_ERASE_SEQUENCE_ERROR   0x10
#define DEVICE_SD_MSD_ADDRESS_ERROR          0x20
#define DEVICE_SD_MSD_PARAMETER_ERROR        0x40
#define DEVICE_SD_MSD_RESPONSE_FAILURE       0xFF

/**SD卡的类型 */
uint8_t  SD_Type=0;


uint8_t DeviceSD_ReadWriteByte(u8 data)
{
	return CoreSPI2WriteRead(data);
}
//SD卡初始化的时候,需要低速
void DeviceSD_SpeedLow(void)
{
    /**设置到低速模式	 */
 	CoreSPI2SetSpeed(DEVICE_SD_SPI_SPEED_LOW);
}
//SD卡正常工作的时候,可以高速了
void DeviceSD_SpeedHigh(void)
{
    /**设置到高速模式	 */
 	CoreSPI2SetSpeed(DEVICE_SD_SPI_SPEED_HIGH);
}

/**设置默认数据格式 */
static void DeviceSD_SetDefaultCP(void)
{
    CoreSPI2SetCp(DEVICE_SD_SPI_CPOL,DEVICE_SD_SPI_CPHA);
}


//SPI硬件层初始化
static void DeviceSD_IO_Init(void)
{
  	GPIO_InitTypeDef  GPIO_InitStructure;

 	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB|RCC_APB2Periph_GPIOD|RCC_APB2Periph_GPIOG, ENABLE);	 //使能PB端口时钟

    //设置硬件上与SD卡相关联的控制引脚输出
	//避免NRF24L01/W25Q32等的影响
	//这里PB12和PG7拉高,是为了防止影响FLASH的烧写.
	//因为他们共用一个SPI口.

    /**PB12 推挽  */
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12;
    /**推挽输出 */
 	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
 	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
 	GPIO_Init(GPIOB, &GPIO_InitStructure);
    /**PB12上拉 */
 	GPIO_SetBits(GPIOB,GPIO_Pin_12);

    /**PD2 推挽 */
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;
	GPIO_Init(GPIOD, &GPIO_InitStructure);
	GPIO_SetBits(GPIOD,GPIO_Pin_2);

    /**PG7 推挽 */
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7;
	GPIO_Init(GPIOG, &GPIO_InitStructure);
	GPIO_SetBits(GPIOG,GPIO_Pin_7);

    CoreSPI2Init(DEVICE_SD_SPI_CPOL,DEVICE_SD_SPI_CPHA,DEVICE_SD_SPI_SPEED_LOW);
	DeviceSD_CS_Set(1);
}

//取消选择,释放SPI总线
static void DeviceSD_DisSelect(void)
{
	DeviceSD_CS_Set(1);
    /**提供额外的8个时钟 */
 	DeviceSD_ReadWriteByte(0xff);
}

//等待卡准备好
//返回值:0,准备好了;其他,错误代码
uint8_t DeviceSD_WaitReady(void)
{
	uint32_t t=0;
	do
	{
		if(DeviceSD_ReadWriteByte(0XFF)==0XFF)
            return 0;
		t++;
	}while(t<0XFFFFFF);
	return 1;
}

//选择sd卡,并且等待卡准备OK
//返回值:0,成功;1,失败;
static uint8_t DeviceSD_Select(void)
{
	DeviceSD_CS_Set(0);
    /**等待成功 */
	if(DeviceSD_WaitReady()==0)
        return 0;
	DeviceSD_DisSelect();
    /**等待失败 */
	return 1;
}

//等待SD卡回应
//Response:要得到的回应值
//返回值:0,成功得到了该回应值
//    其他,得到回应值失败
static uint8_t DeviceSD_GetResponse(uint8_t Response)
{
    /**等待次数 */
	uint16_t Count=0xFFFF;
    /**等待得到准确的回应  	  */
	while ((DeviceSD_ReadWriteByte(0XFF)!=Response)&&Count)
        Count--;
    /**得到回应失败  */
	if (Count==0)
        return DEVICE_SD_MSD_RESPONSE_FAILURE;
    /**正确回应*/
	else
        return DEVICE_SD_MSD_RESPONSE_NO_ERROR;
}


//从sd卡读取一个数据包的内容
//buf:数据缓存区
//len:要读取的数据长度.
//返回值:0,成功;其他,失败;
static uint8_t DeviceSD_RecvData(uint8_t* buf,uint16_t len)
{
    /**等待SD卡发回数据起始令牌0xFE*/
	if(DeviceSD_GetResponse(0xFE))
        return 1;
    /**开始接收数据 */
    while(len--)
    {
        *buf=DeviceSD_ReadWriteByte(0xFF);
        buf++;
    }
    //下面是2个伪CRC（dummy CRC）
    DeviceSD_ReadWriteByte(0xFF);
    DeviceSD_ReadWriteByte(0xFF);
    /**读取成功*/
    return 0;
}

//向sd卡写入一个数据包的内容 512字节
//buf:数据缓存区
//cmd:指令
//返回值:0,成功;其他,失败;
static uint8_t DeviceSD_SendBlock(uint8_t *buf,uint8_t cmd)
{
	u16 t;
    /**等待准备失效*/
	if(DeviceSD_WaitReady())
        return 1;
	DeviceSD_ReadWriteByte(cmd);
    /**不是结束指令 */
	if(cmd!=0XFD)
	{
        /**提高速度,减少函数传参时间 */
		for(t=0;t<512;t++)
            DeviceSD_ReadWriteByte(buf[t]);
        /**忽略crc */
	    DeviceSD_ReadWriteByte(0xFF);
	    DeviceSD_ReadWriteByte(0xFF);
        /**接收响应 */
		t=DeviceSD_ReadWriteByte(0xFF);
        /**响应错误	 */
		if((t&0x1F)!=0x05)
            return 2;
	}
    /**写入成功*/
    return 0;
}

//向SD卡发送一个命令
//输入: u8 cmd   命令
//      u32 arg  命令参数
//      u8 crc   crc校验值
//返回值:SD卡返回的响应
static uint8_t DeviceSD_SendCmd(uint8_t cmd, uint32_t arg, uint8_t crc)
{
    u8 r1;
	u8 Retry=0;
    /**取消上次片选 */
	DeviceSD_DisSelect();
    /**片选失效  */
	if(DeviceSD_Select())
        return 0XFF;
	//发送
    /**分别写入命令 */
    DeviceSD_ReadWriteByte(cmd | 0x40);
    DeviceSD_ReadWriteByte(arg >> 24);
    DeviceSD_ReadWriteByte(arg >> 16);
    DeviceSD_ReadWriteByte(arg >> 8);
    DeviceSD_ReadWriteByte(arg);
    DeviceSD_ReadWriteByte(crc);
    /**Skip a stuff byte when stop reading */
	if(cmd==DEVICE_SD_CMD12)
        DeviceSD_ReadWriteByte(0xff);
    //等待响应，或超时退出
	Retry=0X1F;
	do
	{
		r1=DeviceSD_ReadWriteByte(0xFF);
	}while((r1&0X80) && Retry--);
	//返回状态值
    return r1;
}

//获取SD卡的CID信息，包括制造商信息
//输入: u8 *cid_data(存放CID的内存，至少16Byte）
//返回值:0：NO_ERR
//		 1：错误
/**获取CID信息 */
uint8_t DeviceSD_Get_CID_Info(uint8_t* cid_data)
{
    uint8_t r1;
    DeviceSD_SpeedLow();
    DeviceSD_SetDefaultCP();
    //发CMD10命令，读CID
    r1=DeviceSD_SendCmd(DEVICE_SD_CMD10,0,0x01);
    if(r1==0x00)
	{
        /**接收16个字节的数据 */
		r1=DeviceSD_RecvData(cid_data,16);
    }
    /**取消片选 */
	DeviceSD_DisSelect();
	if(r1)
        return 1;
	else
        return 0;
}

//获取SD卡的CSD信息，包括容量和速度信息
//输入:u8 *cid_data(存放CID的内存，至少16Byte）
//返回值:0：NO_ERR
//		 1：错误
/**获取CSD信息 */
uint8_t DeviceSD_Get_CSD_Info(uint8_t* csd_data)
{
    uint8_t r1;
    DeviceSD_SpeedLow();
    DeviceSD_SetDefaultCP();
    /**发CMD9命令，读CSD */
    r1=DeviceSD_SendCmd(DEVICE_SD_CMD9,0,0x01);
    /**接收16个字节的数据  */
    if(r1==0)
	{
    	r1=DeviceSD_RecvData(csd_data, 16);
    }
    /**取消片选 */
	DeviceSD_DisSelect();
	if(r1)
        return 1;
	else
        return 0;
}



//获取SD卡的总扇区数（扇区数）
//返回值:0： 取容量出错
//       其他:SD卡的容量(扇区数/512字节)
//每扇区的字节数必为512，因为如果不是512，则初始化不能通过.
/**获取卡片包含的扇区总数 */
uint32_t DeviceSD_GetSectorCount(void)
{
    uint8_t csd[16];
    uint32_t Capacity;
    uint8_t n;
	uint16_t csize;
	//取CSD信息，如果期间出错，返回0
    if(DeviceSD_Get_CSD_Info(csd)!=0)
        return 0;
    //如果为SDHC卡，按照下面方式计算
    if((csd[0]&0xC0)==0x40)	 //V2.00的卡
    {
		csize = csd[9] + ((u16)csd[8] << 8) + 1;
        /**得到扇区数 */
		Capacity = (u32)csize << 10;
    }
    /**V1.XX的卡 */
    else
    {
		n = (csd[5] & 15) + ((csd[10] & 128) >> 7) + ((csd[9] & 3) << 1) + 2;
		csize = (csd[8] >> 6) + ((u16)csd[7] << 2) + ((u16)(csd[6] & 3) << 10) + 1;
        /**得到扇区数  */
		Capacity= (u32)csize << (n - 9);
    }
    return Capacity;
}


//读SD卡
//buf:数据缓存区
//sector:扇区
//cnt:扇区数
//返回值:0,ok;其他,失败.
uint8_t DeviceSD_ReadBlocks(uint8_t* buffer,uint32_t sector,uint8_t count)
{
	uint8_t r1;
    DeviceSD_SpeedHigh();
    DeviceSD_SetDefaultCP();
    /**转换为字节地址 */
	if(SD_Type!=SD_TYPE_V2HC)
        sector <<= 9;
	if(count==1)
	{
        /**读命令 */
		r1=DeviceSD_SendCmd(DEVICE_SD_CMD17,sector,0X01);
        /**指令发送成功 */
		if(r1==0)
		{
            /**接收512个字节 */
			r1=DeviceSD_RecvData(buffer,512);
		}
	}else
	{
        /**连续读命令 */
		r1=DeviceSD_SendCmd(DEVICE_SD_CMD18,sector,0X01);
		do
		{
            /**接收512个字节 */
			r1=DeviceSD_RecvData(buffer,512);
			buffer+=512;
		}while(--count && r1==0);
        /**发送停止命令 */
		DeviceSD_SendCmd(DEVICE_SD_CMD12,0,0X01);
	}
    /**取消片选 */
	DeviceSD_DisSelect();
	return r1;
}
//写SD卡
//buf:数据缓存区
//sector:起始扇区
//cnt:扇区数
//返回值:0,ok;其他,失败.
/**写入数据块 */
uint8_t DeviceSD_WriteBlocks(uint8_t* buffer,uint32_t sector,uint8_t count)
{
	u8 r1;
    DeviceSD_SpeedHigh();
    DeviceSD_SetDefaultCP();
    /**转换为字节地址 */
	if(SD_Type!=SD_TYPE_V2HC)
        sector *= 512;
	if(count==1)
	{
        /**读命令 */
		r1=DeviceSD_SendCmd(DEVICE_SD_CMD24,sector,0X01);
        /**指令发送成功 */
		if(r1==0)
		{
            /**写512个字节 */
			r1=DeviceSD_SendBlock(buffer,0xFE);
		}
	}else
	{
		if(SD_Type!=SD_TYPE_MMC)
		{
			DeviceSD_SendCmd(DEVICE_SD_CMD55,0,0X01);
            /**发送指令	 */
			DeviceSD_SendCmd(DEVICE_SD_CMD23,count,0X01);
		}
        /**连续读命令 */
 		r1=DeviceSD_SendCmd(DEVICE_SD_CMD25,sector,0X01);
		if(r1==0)
		{
			do
			{
				r1=DeviceSD_SendBlock(buffer,0xFC);//接收512个字节
				buffer+=512;
			}while(--count && r1==0);
            /**接收512个字节  */
			r1=DeviceSD_SendBlock(0,0xFD);
		}
	}
    /**取消片选 */
	DeviceSD_DisSelect();
	return r1;
}



/**卡片初始化,返回卡片类型 */
uint8_t DeviceSD_Init(void)
{
    uint8_t r1;      // 存放SD卡的返回值
    uint16_t retry;  // 用来进行超时计数
    uint8_t buf[4];
	uint16_t i;

    /**初始化IO */
	DeviceSD_IO_Init();
    /**设置到低速模式  */
 	DeviceSD_SpeedLow();
    /**发送最少74个脉冲 */
 	for(i=0;i<10;i++)
        DeviceSD_ReadWriteByte(0XFF);
	retry=20;
	do
	{
        /**进入IDLE状态 */
		r1=DeviceSD_SendCmd(DEVICE_SD_CMD0,0,0x95);
	}while((r1!=0X01) && retry--);
    /**默认无卡 */
 	SD_Type=0;
	if(r1==0X01)
	{
        /**SD V2.0 */
		if(DeviceSD_SendCmd(DEVICE_SD_CMD8,0x1AA,0x87)==1)
		{
            /**Get trailing return value of R7 resp */
			for(i=0;i<4;i++)
                buf[i]=DeviceSD_ReadWriteByte(0XFF);
            /**卡是否支持2.7~3.6V*/
			if(buf[2]==0X01&&buf[3]==0XAA)
			{
				retry=0XFFFE;
				do
				{
                    /**发送CMD55 */
					DeviceSD_SendCmd(DEVICE_SD_CMD55,0,0X01);
                    /**发送CMD41 */
					r1=DeviceSD_SendCmd(DEVICE_SD_CMD41,0x40000000,0X01);
				}while(r1&&retry--);
                /**鉴别SD2.0卡版本开始 */
				if(retry&&DeviceSD_SendCmd(DEVICE_SD_CMD58,0,0X01)==0)
				{
                    /**得到OCR值 */
					for(i=0;i<4;i++)
                        buf[i]=DeviceSD_ReadWriteByte(0XFF);
                    /**检查CCS */
					if(buf[0]&0x40)
                        SD_Type=SD_TYPE_V2HC;
					else
                        SD_Type=SD_TYPE_V2;
				}
			}
		}else//SD V1.x/ MMC	V3
		{
            /**发送CMD55 */
			DeviceSD_SendCmd(DEVICE_SD_CMD55,0,0X01);
            /**发送CMD41 */
			r1=DeviceSD_SendCmd(DEVICE_SD_CMD41,0,0X01);
			if(r1<=1)
			{
				SD_Type=SD_TYPE_V1;
				retry=0XFFFE;
				do //等待退出IDLE模式
				{
                    /**发送CMD55 */
					DeviceSD_SendCmd(DEVICE_SD_CMD55,0,0X01);
                    /**发送CMD41 */
					r1=DeviceSD_SendCmd(DEVICE_SD_CMD41,0,0X01);
				}while(r1&&retry--);
			}else
			{
                /**MMC V3 */
				SD_Type=SD_TYPE_MMC;
				retry=0XFFFE;
                /**等待退出IDLE模式 */
				do
				{
                    /**发送CMD1*/
					r1=DeviceSD_SendCmd(DEVICE_SD_CMD1,0,0X01);
				}while(r1&&retry--);
			}
            /**错误的卡 */
			if(retry==0||DeviceSD_SendCmd(DEVICE_SD_CMD16,512,0X01)!=0)
                SD_Type=SD_TYPE_ERR;
		}
	}
    /**取消片选 */
	DeviceSD_DisSelect();
    /**高速 */
	DeviceSD_SpeedHigh();
	if(SD_Type)
        return 0;
	else if(r1)
        return r1;
    /**其他错误*/
	return 0xaa;
}





















