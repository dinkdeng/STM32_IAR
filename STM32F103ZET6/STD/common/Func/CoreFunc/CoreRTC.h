#ifndef __CORE_RTC_H_
#define __CORE_RTC_H_
#include "stm32f10x.h"

/*中断回调指针*/
typedef void(*CoreRTCCallBack)(void);

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

/**闹钟中断优先级 */
#define CORE_RTC_ALARM_PREE_PRI     INT_PRE_PRI_3

/**子中断优先级 */
#define CORE_RTC_ALARM_SUB_PRI      INT_SUB_PRI_3

#define CORE_RTC_WAIT_MS(nms)       CoreTickDelayMs(nms)

/*设备初始化,根据不同的外部时钟确定*/
uint8_t CoreRTCInit(CORE_RTC_TIME* rtcTime,CORE_RTC_CLOCK_TYPE type);

/*获取时间*/
void CoreRTCGetTime(CORE_RTC_TIME* rtcTime);

/*设置时间*/
uint8_t CoreRTCSetTime(CORE_RTC_TIME* rtcTime);

/*设置闹钟和闹钟对应的中断处理函数*/
uint8_t CoreRTCSetAlarm(CORE_RTC_TIME* alarmTime,CoreRTCCallBack alarmCallBack);



#endif




