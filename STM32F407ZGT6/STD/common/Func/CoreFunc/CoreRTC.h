#ifndef __CORE_RTC_H_
#define __CORE_RTC_H_
#include "stm32f4xx.h"

#define CORE_RTC_WAIT_MS(nms)               CoreTickDelayMs(nms)

/**等待LSE稳定的时间,单位为MS */
#define CORE_RTC_WAIT_STABLE_MS             0X00FF

/**RTC时钟已经设置过一次的初始化标记 */
#define CORE_RTC_LAST_SET_FLAG              0X5050

/*RTC数据类型结构体*/
typedef struct CORE_RTC_TIME
{
    uint16_t year;
    uint8_t month;
    uint8_t day;
    uint8_t week;
    uint8_t hour;
    uint8_t minute;
    uint8_t second;
}CORE_RTC_TIME;

/**rtc的时钟类型选择 */
typedef enum CORE_RTC_CLOCK_TYPE
{
    CORE_RTC_CLOCK_LSI = 0,
    CORE_RTC_CLOCK_LSE = 1,
}CORE_RTC_CLOCK_TYPE;

#define CORE_RTC_DEFAULT_YEAR           18
#define CORE_RTC_DEFAULT_MONTH          04
#define CORE_RTC_DEFAULT_DAY            14
#define CORE_RTC_DEFAULT_WEEK           6
#define CORE_RTC_DEFAULT_HOUR           02
#define CORE_RTC_DEFAULT_MINUTE         00
#define CORE_RTC_DEFAULT_SECOND         00

typedef void(*CoreRTC_AlarmCallBackFunc)(void);
typedef void(*CoreRTC_WakeupCallBackFunc)(void);

#define CORE_RTC_ALARM_PREE_PRI               INT_SUB_PRI_1
#define CORE_RTC_ALARM_SUB_PRI                INT_PRE_PRI_0

#define CORE_RTC_WAKEUP_PREE_PRI              INT_SUB_PRI_1
#define CORE_RTC_WAKEUP_SUB_PRI               INT_PRE_PRI_0


/*设备初始化,根据不同的外部时钟确定*/
uint8_t CoreRTC_Init(CORE_RTC_CLOCK_TYPE type);

/*获取时间*/
void CoreRTC_GetTime(CORE_RTC_TIME* rtcTime);

/*设置时间*/
uint8_t CoreRTC_SetTime(CORE_RTC_TIME* rtcTime);

//设置闹钟时间(按星期闹铃,24小时制)
//week:星期几(1~7) @ref  RTC_Alarm_Definitions
//hour,min,sec:小时,分钟,秒钟
void CoreRTC_SetAlarm(uint8_t week,uint8_t hour,uint8_t min,uint8_t sec,CoreRTC_AlarmCallBackFunc callBackFunc);

//周期性唤醒定时器设置  
/*wksel:  @ref RTC_Wakeup_Timer_Definitions
#define RTC_WakeUpClock_RTCCLK_Div16        ((uint32_t)0x00000000)
#define RTC_WakeUpClock_RTCCLK_Div8         ((uint32_t)0x00000001)
#define RTC_WakeUpClock_RTCCLK_Div4         ((uint32_t)0x00000002)
#define RTC_WakeUpClock_RTCCLK_Div2         ((uint32_t)0x00000003)
#define RTC_WakeUpClock_CK_SPRE_16bits      ((uint32_t)0x00000004)
#define RTC_WakeUpClock_CK_SPRE_17bits      ((uint32_t)0x00000006)
*/
//cnt:自动重装载值.减到0,产生中断.
void CoreRTC_SetWakeup(uint32_t wksel,uint16_t cnt,CoreRTC_WakeupCallBackFunc callBackFunc);






#endif





