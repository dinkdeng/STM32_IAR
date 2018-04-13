#include "CoreRTC.h"
#include "CoreTickDelay.h"
#include "SystemUtil.h"


CoreRTC_AlarmCallBackFunc alarmCallBackFuncPtr = NULL;
CoreRTC_WakeupCallBackFunc wakeupCallBackFuncPtr = NULL;

//RTC时间设置
//返回1成功 返回0 失败
static uint8_t RTC_Set_Time(uint8_t hour,uint8_t min,uint8_t sec)
{
	RTC_TimeTypeDef RTC_TimeTypeInitStructure;

	RTC_TimeTypeInitStructure.RTC_Hours=hour;
	RTC_TimeTypeInitStructure.RTC_Minutes=min;
	RTC_TimeTypeInitStructure.RTC_Seconds=sec;
	RTC_TimeTypeInitStructure.RTC_H12=RTC_H12_AM;

	return RTC_SetTime(RTC_Format_BIN,&RTC_TimeTypeInitStructure);
}

//RTC日期设置
//返回1成功 返回0 失败
static uint8_t RTC_Set_Date(uint8_t year,uint8_t month,uint8_t date,uint8_t week)
{
	RTC_DateTypeDef RTC_DateTypeInitStructure;
	RTC_DateTypeInitStructure.RTC_Date=date;
	RTC_DateTypeInitStructure.RTC_Month=month;
	RTC_DateTypeInitStructure.RTC_WeekDay=week;
	RTC_DateTypeInitStructure.RTC_Year=year;
	return RTC_SetDate(RTC_Format_BIN,&RTC_DateTypeInitStructure);
}

/*设备初始化,根据不同的外部时钟确定*/
uint8_t CoreRTC_Init(CORE_RTC_CLOCK_TYPE type)
{
    RTC_InitTypeDef RTC_InitStructure;
	//重试
	uint16_t retry=0;
	//使能PWR时钟
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR, ENABLE);
	//使能后备寄存器访问
	PWR_BackupAccessCmd(ENABLE);
    if(type == CORE_RTC_CLOCK_LSE)
    {
        //LSE 开启
		RCC_LSEConfig(RCC_LSE_ON);
		//检查指定的RCC标志位设置与否,等待低速晶振就绪
		while (RCC_GetFlagStatus(RCC_FLAG_LSERDY) == RESET)
		{
			retry++;
			if(retry > CORE_RTC_WAIT_STABLE_MS)
				break;
			CORE_RTC_WAIT_MS(1);
		}
		//LSE 开启失败.
		if(retry > CORE_RTC_WAIT_STABLE_MS)
			return 1;
		//设置RTC时钟(RTCCLK),选择LSE作为RTC时钟
		RCC_RTCCLKConfig(RCC_RTCCLKSource_LSE);
    }
    else
    {
        RCC_LSICmd(ENABLE);
        //检查指定的RCC标志位设置与否,等待低速晶振就绪
		while (RCC_GetFlagStatus(RCC_FLAG_LSIRDY) == RESET)
		{
			retry++;
			if(retry > CORE_RTC_WAIT_STABLE_MS)
				break;
			CORE_RTC_WAIT_MS(1);
		}
        //LSE 开启失败.
		if(retry > CORE_RTC_WAIT_STABLE_MS)
			return 1;
        //设置RTC时钟(RTCCLK),选择LSE作为RTC时钟
		RCC_RTCCLKConfig(RCC_RTCCLKSource_LSI);
    }
    /**时钟配置完成 */
    //使能RTC时钟
    RCC_RTCCLKCmd(ENABLE);

    //RTC异步分频系数(1~0X7F)
    RTC_InitStructure.RTC_AsynchPrediv = 0x7F;
    //RTC同步分频系数(0~7FFF)
    RTC_InitStructure.RTC_SynchPrediv  = 0xFF;
    //RTC设置为,24小时格式
    RTC_InitStructure.RTC_HourFormat   = RTC_HourFormat_24;
    RTC_Init(&RTC_InitStructure);
	//是否第一次配置? 依靠备份寄存器数据决定
	if(RTC_ReadBackupRegister(RTC_BKP_DR0) != CORE_RTC_LAST_SET_FLAG)
	{
		//设置时间
		RTC_Set_Time(CORE_RTC_DEFAULT_HOUR,CORE_RTC_DEFAULT_MINUTE,CORE_RTC_DEFAULT_SECOND);
		//设置日期
		RTC_Set_Date(CORE_RTC_DEFAULT_YEAR,CORE_RTC_DEFAULT_MONTH,CORE_RTC_DEFAULT_DAY,CORE_RTC_DEFAULT_WEEK);
		//写入初始化标记
		RTC_WriteBackupRegister(RTC_BKP_DR0,CORE_RTC_LAST_SET_FLAG);
	}
	return 0;
}


/*获取时间*/
void CoreRTC_GetTime(CORE_RTC_TIME* rtcTime)
{
    RTC_TimeTypeDef RTC_TimeStruct;
	RTC_DateTypeDef RTC_DateStruct;
    RTC_GetTime(RTC_Format_BIN,&RTC_TimeStruct);
    RTC_GetDate(RTC_Format_BIN,&RTC_DateStruct);
    if(rtcTime == NULL)
        return;
    rtcTime->year = 2000 + RTC_DateStruct.RTC_Year;
    rtcTime->month = RTC_DateStruct.RTC_Month;
    rtcTime->day = RTC_DateStruct.RTC_Date;
    rtcTime->week = RTC_DateStruct.RTC_WeekDay;
    rtcTime->hour = RTC_TimeStruct.RTC_Hours;
    rtcTime->minute = RTC_TimeStruct.RTC_Minutes;
    rtcTime->second = RTC_TimeStruct.RTC_Seconds;
    return;
}

/*设置时间*/
uint8_t CoreRTC_SetTime(CORE_RTC_TIME* rtcTime)
{
    if(rtcTime == NULL)
        return 1;
    if(ERROR == RTC_Set_Date((uint8_t)(rtcTime->year-2000),rtcTime->month,rtcTime->day,rtcTime->week))
    {
        return 1;
    }
    if(ERROR == RTC_Set_Time(rtcTime->hour,rtcTime->minute,rtcTime->second))
    {
        return 1;
    }
    return 0;
}

//设置闹钟时间(按星期闹铃,24小时制)
//week:星期几(1~7) @ref  RTC_Alarm_Definitions
//hour,min,sec:小时,分钟,秒钟
void CoreRTC_SetAlarm(uint8_t week,uint8_t hour,uint8_t min,uint8_t sec,CoreRTC_AlarmCallBackFunc callBackFunc)
{
    EXTI_InitTypeDef   EXTI_InitStructure;
	RTC_AlarmTypeDef RTC_AlarmTypeInitStructure;
	RTC_TimeTypeDef RTC_TimeTypeInitStructure;
	NVIC_InitTypeDef   NVIC_InitStructure;

	//关闭闹钟A 然后才能设置闹钟
	RTC_AlarmCmd(RTC_Alarm_A,DISABLE);

	//小时
	RTC_TimeTypeInitStructure.RTC_Hours=hour;
	//分钟
	RTC_TimeTypeInitStructure.RTC_Minutes=min;
	//秒
	RTC_TimeTypeInitStructure.RTC_Seconds=sec;
	RTC_TimeTypeInitStructure.RTC_H12=RTC_H12_AM;

	//星期
	RTC_AlarmTypeInitStructure.RTC_AlarmDateWeekDay=week;
	//按星期闹
	RTC_AlarmTypeInitStructure.RTC_AlarmDateWeekDaySel=RTC_AlarmDateWeekDaySel_WeekDay;
	//精确匹配星期，时分秒
	RTC_AlarmTypeInitStructure.RTC_AlarmMask=RTC_AlarmMask_None;
	RTC_AlarmTypeInitStructure.RTC_AlarmTime=RTC_TimeTypeInitStructure;
	RTC_SetAlarm(RTC_Format_BIN,RTC_Alarm_A,&RTC_AlarmTypeInitStructure);

	//清除RTC闹钟A的标志
	RTC_ClearITPendingBit(RTC_IT_ALRA);
	//清除LINE17上的中断标志位  RTC闹钟走的是外部中断向量
	EXTI_ClearITPendingBit(EXTI_Line17);

	//开启闹钟A中断
	RTC_ITConfig(RTC_IT_ALRA,ENABLE);
	//开启闹钟A
	RTC_AlarmCmd(RTC_Alarm_A,ENABLE);

	//LINE17
	EXTI_InitStructure.EXTI_Line = EXTI_Line17;
	//中断事件
	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
	//上升沿触发
	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising;
	//使能LINE17
	EXTI_InitStructure.EXTI_LineCmd = ENABLE;
	//配置
	EXTI_Init(&EXTI_InitStructure);

	//配置中断向量
	NVIC_InitStructure.NVIC_IRQChannel = RTC_Alarm_IRQn;
	//抢占优先级
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = CORE_RTC_ALARM_PREE_PRI;
	//子优先级
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = CORE_RTC_ALARM_SUB_PRI;
	//使能外部中断通道
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	//配置
	NVIC_Init(&NVIC_InitStructure);

    alarmCallBackFuncPtr = callBackFunc;
}

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
void CoreRTC_SetWakeup(uint32_t wksel,uint16_t cnt,CoreRTC_WakeupCallBackFunc callBackFunc)
{
    EXTI_InitTypeDef   EXTI_InitStructure;
	NVIC_InitTypeDef   NVIC_InitStructure;
	//关闭WAKE UP
	RTC_WakeUpCmd(DISABLE);
	//唤醒时钟选择
	RTC_WakeUpClockConfig(wksel);
	//设置WAKE UP自动重装载寄存器
	RTC_SetWakeUpCounter(cnt);

	//清除RTC WAKE UP的标志
	RTC_ClearITPendingBit(RTC_IT_WUT);
	//清除LINE22上的中断标志位
	EXTI_ClearITPendingBit(EXTI_Line22);

	//开启WAKE UP 定时器中断
	RTC_ITConfig(RTC_IT_WUT,ENABLE);
	//开启WAKE UP 定时器
	RTC_WakeUpCmd(ENABLE);

	//LINE22 这个中断走的是外部中断22
	EXTI_InitStructure.EXTI_Line = EXTI_Line22;
	//中断事件
	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
	//上升沿触发
	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising;
	//使能LINE22
	EXTI_InitStructure.EXTI_LineCmd = ENABLE;
	//配置
	EXTI_Init(&EXTI_InitStructure);


	NVIC_InitStructure.NVIC_IRQChannel = RTC_WKUP_IRQn;
	//抢占优先级
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = CORE_RTC_WAKEUP_PREE_PRI;
	//子优先级
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = CORE_RTC_WAKEUP_SUB_PRI;
	//使能外部中断通道
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	//配置
	NVIC_Init(&NVIC_InitStructure);

    wakeupCallBackFuncPtr = callBackFunc;
}


//RTC闹钟中断服务函数
void RTC_Alarm_IRQHandler(void)
{
	//ALARM A中断?
	if(RTC_GetFlagStatus(RTC_FLAG_ALRAF)==SET)
	{
		//清除中断标志
		RTC_ClearFlag(RTC_FLAG_ALRAF);
		if(alarmCallBackFuncPtr != NULL)
            alarmCallBackFuncPtr();
	}
	//清除中断线17的中断标志
	EXTI_ClearITPendingBit(EXTI_Line17);
}

//RTC WAKE UP中断服务函数
void RTC_WKUP_IRQHandler(void)
{
	//WK_UP中断?
	if(RTC_GetFlagStatus(RTC_FLAG_WUTF)==SET)
	{
		//清除中断标志
		RTC_ClearFlag(RTC_FLAG_WUTF);
		if(wakeupCallBackFuncPtr != NULL)
            wakeupCallBackFuncPtr();
	}
	//清除中断线22的中断标志
	EXTI_ClearITPendingBit(EXTI_Line22);
}