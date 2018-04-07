#include "DeviceVS10xx.h"
#include "SystemUtil.h"
#include "CoreSPI.h"
#include "CoreTickDelay.h"

/**DREQ  */
#define DeviceVS10xxGetDQ()     PCin(13)

/**复位引脚 */
#define DeviceVS10xxRstSet(n)   (PEout(6) = n)

/**片选引脚 */
#define DeviceVS10xxXcsSet(n)   (PFout(7) = n)

/**数据选中引脚 */
#define DeviceVS10xxXdcsSet(n)  (PFout(6) = n)

/**指令 */
#define DEVICE_VS10XX_WRITE_COMMAND 	    0x02
#define DEVICE_VS10XX_READ_COMMAND 	        0x03

/**寄存器定义 SPI*/
#define DEVICE_VS10XX_SPI_MODE        	    0x00
#define DEVICE_VS10XX_SPI_STATUS      	    0x01
#define DEVICE_VS10XX_SPI_BASS        	    0x02
#define DEVICE_VS10XX_SPI_CLOCKF      	    0x03
#define DEVICE_VS10XX_SPI_DECODE_TIME 	    0x04
#define DEVICE_VS10XX_SPI_AUDATA      	    0x05
#define DEVICE_VS10XX_SPI_WRAM        	    0x06
#define DEVICE_VS10XX_SPI_WRAMADDR    	    0x07
#define DEVICE_VS10XX_SPI_HDAT0       	    0x08
#define DEVICE_VS10XX_SPI_HDAT1       	    0x09
#define DEVICE_VS10XX_SPI_AIADDR      	    0x0a
#define DEVICE_VS10XX_SPI_VOL         	    0x0b
#define DEVICE_VS10XX_SPI_AICTRL0     	    0x0c
#define DEVICE_VS10XX_SPI_AICTRL1     	    0x0d
#define DEVICE_VS10XX_SPI_AICTRL2     	    0x0e
#define DEVICE_VS10XX_SPI_AICTRL3     	    0x0f

/**SM */
#define DEVICE_VS10XX_SM_DIFF         	    0x01
#define DEVICE_VS10XX_SM_JUMP         	    0x02
#define DEVICE_VS10XX_SM_RESET        	    0x04
#define DEVICE_VS10XX_SM_OUTOFWAV     	    0x08
#define DEVICE_VS10XX_SM_PDOWN        	    0x10
#define DEVICE_VS10XX_SM_TESTS        	    0x20
#define DEVICE_VS10XX_SM_STREAM       	    0x40
#define DEVICE_VS10XX_SM_PLUSV        	    0x80
#define DEVICE_VS10XX_SM_DACT         	    0x100
#define DEVICE_VS10XX_SM_SDIORD       	    0x200
#define DEVICE_VS10XX_SM_SDISHARE     	    0x400
#define DEVICE_VS10XX_SM_SDINEW       	    0x800
#define DEVICE_VS10XX_SM_ADPCM        	    0x1000
#define DEVICE_VS10XX_SM_ADPCM_HP     	    0x2000

static void DeviceVS10xxIO_Init(void)
{
    GPIO_InitTypeDef  GPIO_InitStructure;

    /**使能PB端口时钟 */
 	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC|RCC_APB2Periph_GPIOE|RCC_APB2Periph_GPIOF, ENABLE);

    /**PC13 DREQ */
 	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_13;
 	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
 	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
 	GPIO_Init(GPIOC, &GPIO_InitStructure);

    /**Rst */
 	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_Init(GPIOE, &GPIO_InitStructure);

	/**XCS XDCS */
 	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6|GPIO_Pin_7;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_Init(GPIOF, &GPIO_InitStructure);

	CoreSPI1Init(DEVICE_VS10XX_CPOL,DEVICE_VS10XX_CPHA,DEVICE_VS10XX_SPEED_LOW);
}

static void DeviceVS10xxSetDefaultCp(void)
{
    CoreSPI1SetCp(DEVICE_VS10XX_CPOL,DEVICE_VS10XX_CPHA);
}

static void DeviceVS10xxSetSpeedLow(void)
{
    CoreSPI1SetSpeed(DEVICE_VS10XX_SPEED_LOW);
}

static void DeviceVS10xxSetSpeedHigh(void)
{
    CoreSPI1SetSpeed(DEVICE_VS10XX_SPEED_HIGH);
}

static uint8_t DeviceVS10xxWriteRead(uint8_t byteValue)
{
    return CoreSPI1WriteRead(byteValue);
}

static void DeviceVS10xxWriteCommand(uint8_t addr,uint16_t command)
{
    /**等待空闲 */
    while(DeviceVS10xxGetDQ()==0);
    /**低速*/
	DeviceVS10xxSetSpeedLow();
	DeviceVS10xxXdcsSet(1);
	DeviceVS10xxXcsSet(0);
    /**发送VS10XX的写命令 */
	DeviceVS10xxWriteRead(DEVICE_VS10XX_WRITE_COMMAND);
    /**地址 */
	DeviceVS10xxWriteRead(addr);
    /**发送高八位 */
	DeviceVS10xxWriteRead(command>>8);
    /**低八位 */
	DeviceVS10xxWriteRead(command);
	DeviceVS10xxXcsSet(1);
    /**高速*/
	DeviceVS10xxSetSpeedHigh();
}

//向VS10XX写数据
//data:要写入的数据
void DeviceVS10xxWriteData(uint8_t data)
{
    /**高速,对VS1003B,最大值不能超过36.864/4Mhz，这里设置为9M  */
	DeviceVS10xxSetSpeedHigh();
	DeviceVS10xxXdcsSet(0);
	DeviceVS10xxWriteRead(data);
	DeviceVS10xxXdcsSet(1);
}

static uint16_t DeviceVS10xxReadReg(uint8_t reg)
{
    uint16_t temp=0;
    /**非等待空闲状态  */
    while(DeviceVS10xxGetDQ()==0);
    /**低速*/
	DeviceVS10xxSetSpeedLow();
	DeviceVS10xxXdcsSet(1);
	DeviceVS10xxXcsSet(0);
    /**发送VS10XX的读命令 */
	DeviceVS10xxWriteRead(DEVICE_VS10XX_READ_COMMAND);
    /**地址 */
	DeviceVS10xxWriteRead(reg);
    /**读取高字节 */
	temp=DeviceVS10xxWriteRead(0xff);
	temp=temp<<8;
    /**读取低字节 */
	temp+=DeviceVS10xxWriteRead(0xff);
	DeviceVS10xxXcsSet(1);
    /**高速	 */
	DeviceVS10xxSetSpeedHigh();
    return temp;
}

//读取VS10xx的RAM
//addr：RAM地址
//返回值：读到的值
uint16_t DeviceVS10xxReadWRAM(uint16_t addr)
{
	uint16_t res;
 	DeviceVS10xxWriteCommand(DEVICE_VS10XX_SPI_WRAMADDR, addr);
	res=DeviceVS10xxReadReg(DEVICE_VS10XX_SPI_WRAM);
 	return res;
}


/**软复位 */
void DeviceVS10xxSoftReset(void)
{
    uint8_t retry=0;
    DeviceVS10xxSetDefaultCp();
    /**等待软件复位结束*/
	while(DeviceVS10xxGetDQ()==0);
    /**启动传输*/
	DeviceVS10xxWriteRead(0Xff);
	retry=0;
    /**软件复位,新模式 */
	while(DeviceVS10xxReadReg(DEVICE_VS10XX_SPI_MODE)!=0x0800)
	{
        /**软件复位,新模式 */
		DeviceVS10xxWriteCommand(DEVICE_VS10XX_SPI_MODE,0x0804);
        /**等待至少1.35ms */
		CoreTickDelayMs(2);
		if(retry++>100)
            break;
	}
    /**等待软件复位结束 */
	while(DeviceVS10xxGetDQ()==0);
	retry=0;
    /**设置VS10XX的时钟,3倍频 ,1.5xADD  */
	while(DeviceVS10xxReadReg(DEVICE_VS10XX_SPI_CLOCKF)!=0X9800)
	{
        /**设置VS10XX的时钟,3倍频 ,1.5xADD */
		DeviceVS10xxWriteCommand(DEVICE_VS10XX_SPI_CLOCKF,0X9800);
		if(retry++>100)
            break;
	}
	CoreTickDelayMs(20);
}

/**硬件复位 */
uint8_t DeviceVS10xxHardwareReset(void)
{
    uint8_t retry=0;
    DeviceVS10xxSetDefaultCp();
	DeviceVS10xxRstSet(0);
	CoreTickDelayMs(20);
    /**取消数据传输 */
	DeviceVS10xxXdcsSet(1);
    /**取消数据传输 */
	DeviceVS10xxXcsSet(1);
    /**芯片复位 */
	DeviceVS10xxRstSet(1);
    /**等待DREQ为高 */
	while(DeviceVS10xxGetDQ()==0&&retry<200)
	{
		retry++;
		CoreTickDelayUs(50);
	};
	CoreTickDelayMs(20);
	if(retry>=200)
        return 1;
	else
        return 0;
}

//正弦测试
void DeviceVS10xxSineTest(void)
{
    DeviceVS10xxSetDefaultCp();
	DeviceVS10xxHardwareReset();
    /**设置音量*/
	DeviceVS10xxWriteCommand(0x0b,0X2020);
    /**进入VS10XX的测试模式*/
 	DeviceVS10xxWriteCommand(DEVICE_VS10XX_SPI_MODE,0x0820);
    /**等待DREQ为高*/
	while(DeviceVS10xxGetDQ()==0);
    /**低速 */
  	DeviceVS10xxSetSpeedLow();
    /**选中数据传输 */
	DeviceVS10xxXdcsSet(0);
	DeviceVS10xxWriteRead(0x53);
	DeviceVS10xxWriteRead(0xef);
	DeviceVS10xxWriteRead(0x6e);
	DeviceVS10xxWriteRead(0x24);
	DeviceVS10xxWriteRead(0x00);
	DeviceVS10xxWriteRead(0x00);
	DeviceVS10xxWriteRead(0x00);
	DeviceVS10xxWriteRead(0x00);
	CoreTickDelayMs(DEVICE_VS10XX_SINE_TEST_MS);
	DeviceVS10xxXdcsSet(1);
    //退出正弦测试
    /**选中数据传输 */
    DeviceVS10xxXdcsSet(0);
	DeviceVS10xxWriteRead(0x45);
	DeviceVS10xxWriteRead(0x78);
	DeviceVS10xxWriteRead(0x69);
	DeviceVS10xxWriteRead(0x74);
	DeviceVS10xxWriteRead(0x00);
	DeviceVS10xxWriteRead(0x00);
	DeviceVS10xxWriteRead(0x00);
	DeviceVS10xxWriteRead(0x00);
	CoreTickDelayMs(DEVICE_VS10XX_SINE_TEST_MS);
	DeviceVS10xxXdcsSet(1);

    //再次进入正弦测试并设置n值为0x44，即将正弦波的频率设置为另外的值
    DeviceVS10xxXdcsSet(0);
	DeviceVS10xxWriteRead(0x53);
	DeviceVS10xxWriteRead(0xef);
	DeviceVS10xxWriteRead(0x6e);
	DeviceVS10xxWriteRead(0x44);
	DeviceVS10xxWriteRead(0x00);
	DeviceVS10xxWriteRead(0x00);
	DeviceVS10xxWriteRead(0x00);
	DeviceVS10xxWriteRead(0x00);
	CoreTickDelayMs(DEVICE_VS10XX_SINE_TEST_MS);
 	DeviceVS10xxXdcsSet(1);

    //退出正弦测试
    DeviceVS10xxXdcsSet(0);
	DeviceVS10xxWriteRead(0x45);
	DeviceVS10xxWriteRead(0x78);
	DeviceVS10xxWriteRead(0x69);
	DeviceVS10xxWriteRead(0x74);
	DeviceVS10xxWriteRead(0x00);
	DeviceVS10xxWriteRead(0x00);
	DeviceVS10xxWriteRead(0x00);
	DeviceVS10xxWriteRead(0x00);
	CoreTickDelayMs(DEVICE_VS10XX_SINE_TEST_MS);
	DeviceVS10xxXdcsSet(1);
}


//ram 测试
//返回值:RAM测试结果
//VS1003如果得到的值为0x807F，则表明完好;VS1053为0X83FF.
uint16_t DeviceVS10xxRamTest(void)
{
    DeviceVS10xxSetDefaultCp();
	DeviceVS10xxHardwareReset();
    /**进入VS10XX的测试模式 */
 	DeviceVS10xxWriteCommand(DEVICE_VS10XX_SPI_MODE,0x0820);
    /**等待DREQ为高	 */
	while (DeviceVS10xxGetDQ()==0);
    /**低速*/
 	DeviceVS10xxSetSpeedLow();
    /**xDCS = 1，选择VS10XX的数据接口 */
	DeviceVS10xxXdcsSet(0);
	DeviceVS10xxWriteRead(0x4d);
	DeviceVS10xxWriteRead(0xea);
	DeviceVS10xxWriteRead(0x6d);
	DeviceVS10xxWriteRead(0x54);
	DeviceVS10xxWriteRead(0x00);
	DeviceVS10xxWriteRead(0x00);
	DeviceVS10xxWriteRead(0x00);
	DeviceVS10xxWriteRead(0x00);
	CoreTickDelayMs(150);
	DeviceVS10xxXdcsSet(1);
    /**VS1003如果得到的值为0x807F，则表明完好;VS1053为0X83FF.; */
	return DeviceVS10xxReadReg(DEVICE_VS10XX_SPI_HDAT0);
}


void DeviceVS10xxSetPlaySpeed(uint8_t speedMul)
{
    DeviceVS10xxSetDefaultCp();
    /**速度控制地址  */
    DeviceVS10xxWriteCommand(DEVICE_VS10XX_SPI_WRAMADDR,0X1E04);
    /**等待空闲*/
	while(DeviceVS10xxGetDQ()==0);
    /**写入播放速度 */
	DeviceVS10xxWriteCommand(DEVICE_VS10XX_SPI_WRAM,speedMul);
}

//FOR WAV HEAD0 :0X7761 HEAD1:0X7665
//FOR MIDI HEAD0 :other info HEAD1:0X4D54
//FOR WMA HEAD0 :data speed HEAD1:0X574D
//FOR MP3 HEAD0 :data speed HEAD1:ID
//比特率预定值,阶层III
const uint16_t bitrate[2][16]=
{
    {0,8,16,24,32,40,48,56,64,80,96,112,128,144,160,0},
    {0,32,40,48,56,64,80,96,112,128,160,192,224,256,320,0}
};

//返回Kbps的大小
//返回值：得到的码率
uint16_t DeviceVS10xxGetKbpsInfo(void)
{
	uint32_t HEAD0;
	uint32_t HEAD1;
    DeviceVS10xxSetDefaultCp();
 	HEAD0=DeviceVS10xxReadReg(DEVICE_VS10XX_SPI_HDAT0);
    HEAD1=DeviceVS10xxReadReg(DEVICE_VS10XX_SPI_HDAT1);
    switch(HEAD1)
    {
        case 0x7665://WAV格式
        case 0X4D54://MIDI格式
		case 0X4154://AAC_ADTS
		case 0X4144://AAC_ADIF
		case 0X4D34://AAC_MP4/M4A
		case 0X4F67://OGG
        case 0X574D://WMA格式
		case 0X664C://FLAC格式
        {
            /**相当于*8/100 */
            HEAD1=HEAD0*2/25;
            /**对小数点第一位四舍五入 */
            if((HEAD1%10)>5)
                return HEAD1/10+1;
            else
                return HEAD1/10;
        }
        default://MP3格式,仅做了阶层III的表
        {
            HEAD1>>=3;
            HEAD1=HEAD1&0x03;
            if(HEAD1==3)
                HEAD1=1;
            else
                HEAD1=0;
            return bitrate[HEAD1][HEAD0>>12];
        }
    }
}

//得到平均字节数
//返回值：平均字节数速度
uint16_t DeviceVS10xxGetBitRate(void)
{
    DeviceVS10xxSetDefaultCp();
    /**平均位速 */
	return DeviceVS10xxReadWRAM(0X1E05);
}

//得到需要填充的数字
//返回值:需要填充的数字
uint16_t DeviceVS10xxGetEndFillByte(void)
{
    DeviceVS10xxSetDefaultCp();
    /**填充字节 */
	return DeviceVS10xxReadWRAM(0X1E06);
}

//发送一次音频数据
//固定为32字节
//返回值:0,发送成功
//		 1,VS10xx不缺数据,本次数据未成功发送
uint8_t DeviceVS10xxSendMusicData(uint8_t* buf)
{
	u8 n;
    DeviceVS10xxSetDefaultCp();
    DeviceVS10xxSetSpeedHigh();
    /**送数据给VS10XX */
	if(DeviceVS10xxGetDQ()!=0)
	{
		DeviceVS10xxXdcsSet(0);
        for(n=0;n<32;n++)
		{
			DeviceVS10xxWriteRead(buf[n]);
		}
		DeviceVS10xxXdcsSet(1);
        return 0;
	}
    else
        return 1;
}

//切歌
//通过此函数切歌，不会出现切换“噪声”
void DeviceVS10xxRestartPlay(void)
{
	uint16_t temp;
	uint16_t i;
	uint8_t n;
	uint8_t vsbuf[32];
    DeviceVS10xxSetDefaultCp();
    /**清零 */
	for(n=0;n<32;n++)
        vsbuf[n]=0;
    /**读取SPI_MODE的内容 */
	temp=DeviceVS10xxReadReg(DEVICE_VS10XX_SPI_MODE);
    /**设置SM_CANCEL位 */
	temp|=1<<3;
    /**设置SM_LAYER12位,允许播放MP1,MP2 */
	temp|=1<<2;
    /**设置取消当前解码指令 */
	DeviceVS10xxWriteCommand(DEVICE_VS10XX_SPI_MODE,temp);
    /**发送2048个0,期间读取SM_CANCEL位.如果为0,则表示已经取消了当前解码*/
	for(i=0;i<2048;)
	{
        /**每发送32个字节后检测一次 */
		if(DeviceVS10xxSendMusicData(vsbuf)==0)
		{
            /**发送了32个字节 */
			i += 32;
   			temp=DeviceVS10xxReadReg(DEVICE_VS10XX_SPI_MODE);
            /**成功取消了*/
 			if((temp&(1<<3))==0)
                break;
		}
	}
	if(i<2048)//SM_CANCEL正常
	{
        /**读取填充字节 */
		temp=DeviceVS10xxGetEndFillByte()&0xff;
        /**填充字节放入数组 */
		for(n=0;n<32;n++)
            vsbuf[n]=temp;
		for(i=0;i<2052;)
		{
            /**填充	 */
			if(DeviceVS10xxSendMusicData(vsbuf)==0)
                i+=32;
		}
	}
    /**SM_CANCEL不成功,坏情况,需要软复位  */
    else
        DeviceVS10xxSoftReset();
	temp = DeviceVS10xxReadReg(DEVICE_VS10XX_SPI_HDAT0);
    temp += DeviceVS10xxReadReg(DEVICE_VS10XX_SPI_HDAT1);
    /**软复位,还是没有成功取消,放杀手锏,硬复位 */
	if(temp)
	{
        /**硬复位 */
		DeviceVS10xxHardwareReset();
        /**软复位 */
		DeviceVS10xxSoftReset();
	}
}

//重设解码时间
void DeviceVS10xxResetDecodeTime(void)
{
    DeviceVS10xxSetDefaultCp();
	DeviceVS10xxWriteCommand(DEVICE_VS10XX_SPI_DECODE_TIME,0x0000);
    /**操作两次 */
	DeviceVS10xxWriteCommand(DEVICE_VS10XX_SPI_DECODE_TIME,0x0000);
}

//得到mp3的播放时间n sec
//返回值：解码时长
uint16_t DeviceVS10xxGetDecodeTime(void)
{
	uint16_t dt=0;
    DeviceVS10xxSetDefaultCp();
	dt=DeviceVS10xxReadReg(DEVICE_VS10XX_SPI_DECODE_TIME);
 	return dt;
}
//vs10xx装载patch.
//patch：patch首地址
//len：patch长度
void DeviceVS10xxLoadPatch(uint16_t *patch,uint16_t len)
{
	uint16_t i;
	uint16_t addr, n, val;
    DeviceVS10xxSetDefaultCp();
	for(i=0;i<len;)
	{
		addr = patch[i++];
		n    = patch[i++];
        /**RLE run, replicate n samples*/
		if(n & 0x8000U)
		{
			n  &= 0x7FFF;
			val = patch[i++];
			while(n--)
                DeviceVS10xxWriteCommand(addr, val);
		}
        /**copy run, copy n sample  */
        else
		{
			while(n--)
			{
				val = patch[i++];
				DeviceVS10xxWriteCommand(addr, val);
			}
		}
	}
}

//设定VS10XX播放的音量和高低音
//volx:音量大小(0~254)
void DeviceVS10xxSetVol(uint8_t volx)
{
    /**暂存音量值 */
    uint16_t volt=0;
    /**取反一下,得到最大值,表示最大的表示*/
    volt=254-volx;
	volt<<=8;
    /**得到音量设置后大小 */
    volt+=254-volx;
    DeviceVS10xxSetDefaultCp();
    /**设音量  */
    DeviceVS10xxWriteCommand(DEVICE_VS10XX_SPI_VOL,volt);
}

//设定高低音控制
//bfreq:低频上限频率	2~15(单位:10Hz)
//bass:低频增益			0~15(单位:1dB)
//tfreq:高频下限频率 	1~15(单位:Khz)
//treble:高频增益  	 	0~15(单位:1.5dB,小于9的时候为负数)
void DeviceVS10xxSetBass(uint8_t bfreq,uint8_t bass,uint8_t tfreq,uint8_t treble)
{
    /**暂存音调寄存器值 */
    uint16_t bass_set=0;
    int8_t temp=0;
    /**变换*/
	if(treble==0)
        temp=0;
	else if(treble>8)
        temp=treble-8;
 	else
        temp=treble-9;
    /**高音设定 */
	bass_set=temp&0X0F;
	bass_set<<=4;
    /**高音下限频率 */
	bass_set+=tfreq&0xf;
	bass_set<<=4;
    /**低音设定 */
	bass_set+=bass&0xf;
	bass_set<<=4;
    /**低音上限  */
	bass_set+=bfreq&0xf;
    DeviceVS10xxSetDefaultCp();
    /**BASS*/
	DeviceVS10xxWriteCommand(DEVICE_VS10XX_SPI_BASS,bass_set);
}

//设定音效
//eft:0,关闭;1,最小;2,中等;3,最大.
void DeviceVS10xxSetEffect(uint8_t eft)
{
	uint16_t temp;
    DeviceVS10xxSetDefaultCp();
    /**读取SPI_MODE的内容 */
	temp=DeviceVS10xxReadReg(DEVICE_VS10XX_SPI_MODE);
    /**设定LO */
	if(eft&0X01)
        temp|=1<<4;
	else
        temp&=~(1<<5);
    /** 设定HO*/
	if(eft&0X02)
        temp|=1<<7;
	else
        temp&=~(1<<7);
    /**设定模式 */
	DeviceVS10xxWriteCommand(DEVICE_VS10XX_SPI_MODE,temp);
}



//设置音量,音效等.
void DeviceVS10xxSetAllDefault(void)
{
	DeviceVS10xxSetVol(DEVICE_VS10XX_DEAFULT_VOL);
	DeviceVS10xxSetBass(DEVICE_VS10XX_DEAFULT_BUTTOM_FREQ,DEVICE_VS10XX_DEFAULT_BASS,DEVICE_VS10XX_DEFAULT_TOP_FREQ,DEVICE_VS10XX_DEFAULT_TREBLE);
	DeviceVS10xxSetEffect(DEVICE_VS10XX_DEFAULT_EFFECT);
}

/**设备初始化 */
uint8_t DeviceVS1053Init(void)
{
    DeviceVS10xxIO_Init();
    DeviceVS10xxSetAllDefault();
    if(DEVICE_VS1053_ID == DeviceVS10xxRamTest())
    {
        return 0;
    }
    return 1;
}



























