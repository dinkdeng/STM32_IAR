#ifndef __DEVICE_RDA5820_H_
#define __DEVICE_RDA5820_H_
#include "stm32f10x.h"

/**频段设置 */
typedef enum DEVICE_RDA5820_FREQ_BAUD
{
    RDA5820_BAUD_87_108MHZ = 0,
    RDA5820_BAUD_76_91MHZ = 1,
    RDA5820_BAUD_76_108MHZ = 2,
}DEVICE_RDA5820_FREQ_BAUD;

/**分辨率设置 */
typedef enum DEVICE_RDA5820_RESOLUTION_RATIO
{
    RDA5820_RESOLUTION_100K = 0,
    RDA5820_RESOLUTION_200K = 1,
    RDA5820_RESOLUTION_50K = 3,
}DEVICE_RDA5820_RESOLUTION_RATIO;

/**读RDA5820 */
#define DEVICE_RDA5820_READ     0X23  

/**写RDA5820 */
#define DEVICE_RDA5820_WRITE	0X22  



/**获取当前信号强度 */
uint8_t DeviceRDA5820_GetSingleStrength(void);

/**设置静音 */
void DeviceRDA5820_SetMute(uint8_t muteFlag);

/**设置音量 */
void DeviceRDA5820_SetVol(uint8_t volSet);

/**设置分辨率 */
void DeviceRDA5820_SetResolutionRatio(DEVICE_RDA5820_RESOLUTION_RATIO resolutionRatio);

/**设置频段 */
void DeviceRDA5820_SetFreqBaud(DEVICE_RDA5820_FREQ_BAUD freqBaud);

/**设置当前频率 */
void DeviceRDA520_SetFreq(uint16_t freqBase10khz);

/**获取当前频率 */
uint16_t DeviceRDA5820_GetCurrentFreq(void);

/**设备初始化 */
uint8_t DeviceRDA5820_Init(DEVICE_RDA5820_FREQ_BAUD freqBaud,DEVICE_RDA5820_RESOLUTION_RATIO resolutionRatio,uint16_t freqBase10khz,uint8_t volSet);

/**检测当前频率是否是有效站点 */
uint8_t DeviceRDA5820_CurrentFreqIsAState(void);


#endif





