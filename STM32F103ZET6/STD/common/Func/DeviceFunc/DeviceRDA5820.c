#include "DeviceRDA5820.h"
#include "CoreTickDelay.h"
#include "SystemUtil.h"
#include "CoreIIC.h"

#define DEVICE_RDA5820_R00		0X00  //读出16个位的ID =0X5820
#define DEVICE_RDA5820_R02		0X02  //DHIZ[15],DMUTE[14],MONO[13],BASS[12],SEEKUP[9],SEEK[8],SKMODE[7],CLK_MODE[6:4],ANTENNA[2],SOFTRESET[1],ENABLE[0]
#define DEVICE_RDA5820_R03		0X03  //CHAN[15:6],TUNE[4],BAND[3:2],SPACE[1:0]
#define DEVICE_RDA5820_R04		0X04  //STCIEN[14](搜索完成中断),DE[11],I2S_ENABLE[6],GPIO3~1[5:0]
#define DEVICE_RDA5820_R05		0X05  //INT_MODE[15],SEEKTH[14:8](设定信号强度),LNA_PORT_SEL[7:6],LNA_ICSEL_BIT[5:4],VOLUME[3:0];
#define DEVICE_RDA5820_R0A		0X0A  //STC[14](搜索完成标志),SF[13](搜索失败标志),ST[10](立体声标志),READCHAN[9:0](频率).
#define DEVICE_RDA5820_R0B		0X0B  //RSSI[15:9](信号强度),FM_TRUE[8](站点标志),FM_READY[7](fm就绪标志)
#define DEVICE_RDA5820_R40		0X40  //AUTO_SEEK[15](全/半自动搜台),CHAN_SCAN[14](搜空/忙台),CHIP_FUN[3:0](工作模式)
#define DEVICE_RDA5820_R41		0X41  //MEM_CLR[15](内存清除),RPT_TIME[14:9](RDS应答次数),MEM_DEPTH[8:0](FIFO大小,最大256)
#define DEVICE_RDA5820_R42		0X42  //TX_SIG_GAIN[10:8],TX_PA_GAIN[5:0](发射功率).
#define DEVICE_RDA5820_R4A		0X4A  //MEM_INTMOD[4:0]
#define DEVICE_RDA5820_R4B		0X4B  //CHAN_NUM[7:0](仅在全自动搜台有效,代表电台数量).
#define DEVICE_RDA5820_R4C		0X4C  //TX_AUDIO_DEV[15:0]
#define DEVICE_RDA5820_R4D		0X4D  //TX_PILOT_DEV[15:0]
#define DEVICE_RDA5820_R4E		0X4E  //TX_RDS_DEV[15:0]
#define DEVICE_RDA5820_R53		0X53  //CHAN_BOTTON[10:0](通道最低频率,100kHZ为单位)
#define DEVICE_RDA5820_R54		0X54  //CHAN_TOP[10:0](通道最高频率,100kHZ为单位)
#define DEVICE_RDA5820_R64		0X64  //RDS_RXMOD[4:3](FIFO模式[10]或者寄存器模式[11])

//写RDA5820寄存器
static uint8_t DeviceRDA5820_WriteReg(uint8_t addr,uint16_t val)
{
    IIC_WAIT_ACK waitAckResult = IIC_WAIT_ACK_OK;
    CoreIICSendStart();
    /**发送写命令   */
	CoreIICSendByte(DEVICE_RDA5820_WRITE);
	waitAckResult = CoreIICWaitACK();
    if (IIC_WAIT_ACK_FAIL == waitAckResult)
    {
        return 1;
    }
    /**发送地址 */
    CoreIICSendByte(addr);
	waitAckResult = CoreIICWaitACK();
    if (IIC_WAIT_ACK_FAIL == waitAckResult)
    {
        return 1;
    }
    /**发送高字节*/
	CoreIICSendByte(val>>8);
	waitAckResult = CoreIICWaitACK();
    if (IIC_WAIT_ACK_FAIL == waitAckResult)
    {
        return 1;
    }
    /**发送低字节 */
 	CoreIICSendByte(val&0XFF);
 	waitAckResult = CoreIICWaitACK();
    if (IIC_WAIT_ACK_FAIL == waitAckResult)
    {
        return 1;
    }
    /**产生一个停止条件  */
    CoreIICSendStop();
    return 0;
}

//读RDA5820寄存器
static uint8_t DeviceRDA5820_ReadReg(uint8_t addr,uint16_t* regValue)
{
	uint16_t res;
    IIC_WAIT_ACK waitAckResult = IIC_WAIT_ACK_OK;
    CoreIICSendStart();
    /**发送写命令  */
	CoreIICSendByte(DEVICE_RDA5820_WRITE);
	waitAckResult = CoreIICWaitACK();
    if (IIC_WAIT_ACK_FAIL == waitAckResult)
    {
        return 1;
    }
    /**发送地址 */
    CoreIICSendByte(addr);
	waitAckResult = CoreIICWaitACK();
    if (IIC_WAIT_ACK_FAIL == waitAckResult)
    {
        return 1;
    }
    CoreIICSendStart();
    /**发送读命令 */
	CoreIICSendByte(DEVICE_RDA5820_READ);
	waitAckResult = CoreIICWaitACK();
    if (IIC_WAIT_ACK_FAIL == waitAckResult)
    {
        return 1;
    }
    /**读高字节,发送ACK */
 	res=CoreIICRecvByte();
  	res <<= 8;
    CoreIICSendACK();
    /**读低字节,发送NACK */
  	res |= CoreIICRecvByte();
    CoreIICSendNACK();
  	/**产生一个停止条件  */
    CoreIICSendStop();
    *regValue = res;
    return 0;
}

//设置RDA5820为RX模式
void DeviceRDA5820_RX_Mode(void)
{
	uint16_t temp;
    /**读取0X40的内容 */
	DeviceRDA5820_ReadReg(DEVICE_RDA5820_R40,&temp);
    /**RX 模式 */
	temp &= 0xfff0;
    /**FM RX模式  */
	DeviceRDA5820_WriteReg(DEVICE_RDA5820_R40,temp) ;
}

//设置RDA5820为TX模式
void DeviceRDA5820_TX_Mode(void)
{
	uint16_t temp;
    /**读取0X40的内容 */
	DeviceRDA5820_ReadReg(DEVICE_RDA5820_R40,&temp);
	temp &= 0xfff0;
    /**TX 模式 */
	temp |= 0x0001;
    /**FM TM 模式  */
	DeviceRDA5820_WriteReg(DEVICE_RDA5820_R40,temp) ;
}

/**获取当前信号强度 */
uint8_t DeviceRDA5820_GetSingleStrength(void)
{
    uint16_t temp;
    /**读取0X0B的内容 */
	DeviceRDA5820_ReadReg(DEVICE_RDA5820_R0B,&temp);
    /**返回信号强度*/
	return (uint8_t)(temp>>9);
}

/**设置静音 */
void DeviceRDA5820_SetMute(uint8_t muteFlag)
{
    uint16_t temp;
	DeviceRDA5820_ReadReg(DEVICE_RDA5820_R02,&temp);
	if(muteFlag)
        temp|=1<<14;
	else
        temp&=~(1<<14);
    /**设置MUTE */
	DeviceRDA5820_WriteReg(DEVICE_RDA5820_R02,temp) ;
}

/**设置音量 */
void DeviceRDA5820_SetVol(uint8_t volSet)
{
    uint16_t temp;
	DeviceRDA5820_ReadReg(DEVICE_RDA5820_R05,&temp);
	temp &= 0XFFF0;
	temp |= volSet&0X0F;
	DeviceRDA5820_WriteReg(DEVICE_RDA5820_R05,temp) ;
}

/**设置分辨率 */
void DeviceRDA5820_SetResolutionRatio(DEVICE_RDA5820_RESOLUTION_RATIO resolutionRatio)
{
    uint16_t temp;
	DeviceRDA5820_ReadReg(DEVICE_RDA5820_R03,&temp);
	temp &= 0XFFFC;
	temp |= ((uint16_t)resolutionRatio);
	DeviceRDA5820_WriteReg(DEVICE_RDA5820_R03,temp) ;
}

/**设置频段 */
void DeviceRDA5820_SetFreqBaud(DEVICE_RDA5820_FREQ_BAUD freqBaud)
{
    uint16_t temp;
    /**读取0X03的内容 */
	DeviceRDA5820_ReadReg(DEVICE_RDA5820_R03,&temp);
	temp &= 0XFFF3;
	temp |= ((uint16_t)freqBaud)<<2;
    /**设置BAND */
	DeviceRDA5820_WriteReg(DEVICE_RDA5820_R03,temp) ;
}

/**设置当前频率 */
void DeviceRDA520_SetFreq(uint16_t freqBase10khz)
{
    uint16_t temp;
	uint8_t spc=0,band=0;
	uint16_t fbtm,chan;
    /**读取0X03的内容 */
	DeviceRDA5820_ReadReg(DEVICE_RDA5820_R03,&temp);
	temp&=0X001F;
    /**得到频带 */
	band=(temp>>2)&0x03;
    /**得到分辨率 */
	spc=temp&0x03;
	if(spc==0)
        spc=10;
	else if(spc==1)
        spc=20;
	else
        spc=5;
	if(band==0)
        fbtm=8700;
	else if(band==1||band==2)
        fbtm=7600;
	else
	{
        /**得到bottom频率 */
		DeviceRDA5820_ReadReg(0X53,&fbtm);
		fbtm*=10;
	}
	if(freqBase10khz<fbtm)
        return;
    /**得到CHAN应该写入的值 */
	chan=(freqBase10khz-fbtm)/spc;
    /**取低10位 */
	chan&=0X3FF;
	temp|=chan<<6;
    /**TONE ENABLE */
	temp|=1<<4;
    /**设置频率 */
	DeviceRDA5820_WriteReg(DEVICE_RDA5820_R03,temp);
    /**等待20ms*/
	CoreTickDelayMs(20);
    do
    {
        /**等待FM_READY */
        DeviceRDA5820_ReadReg(DEVICE_RDA5820_R0B,&temp);
    }while((temp&(1<<7))==0);
}

/**获取当前频率 */
uint16_t DeviceRDA5820_GetCurrentFreq(void)
{
    uint16_t temp;
	uint8_t spc=0,band=0;
	uint16_t fbtm,chan;
    /**读取0X03的内容 */
	DeviceRDA5820_ReadReg(0X03,&temp);
	chan=temp>>6;
    /**得到频带 */
	band=(temp>>2)&0x03;
    /**得到分辨率 */
	spc=temp&0x03;
	if(spc==0)
        spc=10;
	else if(spc==1)
        spc=20;
	else
        spc=5;
	if(band==0)
        fbtm=8700;
	else if(band==1||band==2)
        fbtm=7600;
	else
	{
        /**得到bottom频率 */
		DeviceRDA5820_ReadReg(DEVICE_RDA5820_R53,&fbtm);
		fbtm*=10;
	}
 	temp=fbtm+chan*spc;
    /**返回频率值*/
	return temp;
}

//设置TX发送功率
//gain:0~63
void DeviceRDA5820_TxPAG_Set(uint8_t gain)
{
	uint16_t temp;
    /**读取0X42的内容 */
	DeviceRDA5820_ReadReg(0X42,&temp);
	temp&=0XFFC0;
    /**GAIN */
	temp|=gain;
    /**设置PA的功率 */
	DeviceRDA5820_WriteReg(0X42,temp) ;
}
//设置TX 输入信号增益
//gain:0~7
void DeviceRDA5820_TxPGA_Set(uint8_t gain)
{
	uint16_t temp;
    /**读取0X42的内容 */
	DeviceRDA5820_ReadReg(0X42,&temp);
	temp&=0XF8FF;
    /**GAIN */
	temp|=gain<<8;
    /**设置PGA */
	DeviceRDA5820_WriteReg(0X42,temp) ;
}

/**检测当前频率是否为有效站点 */
uint8_t DeviceRDA5820_CurrentFreqIsAState(void)
{
    uint16_t temp = 0;
    DeviceRDA5820_ReadReg(0X0B,&temp);
    if(temp&(1<<8))
        return 1;
    else
        return 0;
}

/**设备初始化 */
uint8_t DeviceRDA5820_Init(DEVICE_RDA5820_FREQ_BAUD freqBaud,DEVICE_RDA5820_RESOLUTION_RATIO resolutionRatio,uint16_t freqBase10khz,uint8_t volSet)
{
    uint16_t id;
    uint8_t result = 0;
    /**初始化IIC口 */
	CoreIICInit();
    /**读取ID =0X5805*/
	result = DeviceRDA5820_ReadReg(DEVICE_RDA5820_R00,&id);
    if(result)
        return result;
    /**读取ID正确 */
	if(id==0X5805)
	{
	 	DeviceRDA5820_WriteReg(DEVICE_RDA5820_R02,0x0002);	//软复位
		CoreTickDelayMs(50);
	 	DeviceRDA5820_WriteReg(DEVICE_RDA5820_R02,0xC001);	//立体声,上电
        /**等待时钟稳定  */
		CoreTickDelayMs(800);
        /**搜索强度8,LNAN,1.8mA,VOL最大*/
	 	DeviceRDA5820_WriteReg(DEVICE_RDA5820_R05,0X884F);
 	 	DeviceRDA5820_WriteReg(0X07,0X7800);
	 	DeviceRDA5820_WriteReg(0X13,0X0008);
	 	DeviceRDA5820_WriteReg(0X15,0x1420);
	 	DeviceRDA5820_WriteReg(0X16,0XC000);
	 	DeviceRDA5820_WriteReg(0X1C,0X3126);
	 	DeviceRDA5820_WriteReg(0X22,0X9C24);
	 	DeviceRDA5820_WriteReg(0X47,0XF660);
        CoreTickDelayMs(100);
        /**接收模式 */
        DeviceRDA5820_SetFreqBaud(freqBaud);
        DeviceRDA5820_SetResolutionRatio(resolutionRatio);
        DeviceRDA5820_TxPGA_Set(3);	//信号增益设置为3
	    DeviceRDA5820_TxPAG_Set(63);	//发射功率为最大.
        DeviceRDA5820_SetVol(volSet);
        DeviceRDA5820_RX_Mode();
        DeviceRDA520_SetFreq(freqBase10khz);
        return 0;
 	}
    /**初始化失败 */
    else
        return 1;
}





















