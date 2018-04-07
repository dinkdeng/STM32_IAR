#ifndef __DEVICE_VS10XX_H_
#define __DEVICE_VS10XX_H_
#include "stm32f10x.h"

#define DEVICE_VS10XX_CPOL              CORE_SPI_CPOL_HIGH
#define DEVICE_VS10XX_CPHA              CORE_SPI_CPHA_2EDGE

#define DEVICE_VS10XX_SPEED_LOW         CORE_SPI_SPEED_256

#define DEVICE_VS10XX_SPEED_HIGH        CORE_SPI_SPEED_8

#define DEVICE_VS10XX_SINE_TEST_MS              1000

/**默认音量 0-254 */
#define DEVICE_VS10XX_DEAFULT_VOL               220

/**默认低音截止上限频率 2-15 单位10HZ */
#define DEVICE_VS10XX_DEAFULT_BUTTOM_FREQ       6

/**默认高音截止下限频率  1-15 单位KHZ */
#define DEVICE_VS10XX_DEFAULT_TOP_FREQ          10

/**低音增益 0-15 单位分贝 */
#define DEVICE_VS10XX_DEFAULT_BASS              15

/**高音增益 1-15 单位1.5分贝DB,小于9为负数 */
#define DEVICE_VS10XX_DEFAULT_TREBLE            15

/**音效,0-3 0最弱 3最强 */
#define DEVICE_VS10XX_DEFAULT_EFFECT            3

#define DEVICE_VS1053_ID                        0X83FF

#define DEVICE_VS1003_ID                        0X807F


/**设备初始化 */
uint8_t DeviceVS1053Init(void);

//设定音效
//eft:0,关闭;1,最小;2,中等;3,最大.
void DeviceVS10xxSetEffect(uint8_t eft);

//设定高低音控制
//bfreq:低频上限频率	2~15(单位:10Hz)
//bass:低频增益			0~15(单位:1dB)
//tfreq:高频下限频率 	1~15(单位:Khz)
//treble:高频增益  	 	0~15(单位:1.5dB,小于9的时候为负数)
void DeviceVS10xxSetBass(uint8_t bfreq,uint8_t bass,uint8_t tfreq,uint8_t treble);

//设定VS10XX播放的音量和高低音
//volx:音量大小(0~254)
void DeviceVS10xxSetVol(uint8_t volx);

//vs10xx装载patch.
//patch：patch首地址
//len：patch长度
void DeviceVS10xxLoadPatch(uint16_t *patch,uint16_t len);

//得到mp3的播放时间n sec
//返回值：解码时长
uint16_t DeviceVS10xxGetDecodeTime(void);

//重设解码时间
void DeviceVS10xxResetDecodeTime(void);

//切歌
//通过此函数切歌，不会出现切换“噪声”
void DeviceVS10xxRestartPlay(void);

//发送一次音频数据
//固定为32字节
//返回值:0,发送成功
//		 1,VS10xx不缺数据,本次数据未成功发送
uint8_t DeviceVS10xxSendMusicData(uint8_t* buf);

//得到需要填充的数字
//返回值:需要填充的数字
uint16_t DeviceVS10xxGetEndFillByte(void);

//得到平均字节数
//返回值：平均字节数速度
uint16_t DeviceVS10xxGetBitRate(void);

//返回Kbps的大小
//返回值：得到的码率
uint16_t DeviceVS10xxGetKbpsInfo(void);

void DeviceVS10xxSetPlaySpeed(uint8_t speedMul);

//正弦测试
void DeviceVS10xxSineTest(void);

/**硬件复位 */
uint8_t DeviceVS10xxHardwareReset(void);

/**软复位 */
void DeviceVS10xxSoftReset(void);

//设置音量,音效等.
void DeviceVS10xxSetAllDefault(void);




#endif




