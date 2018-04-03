#include "CoreRTC.h"
#include "CoreTickDelay.h"
#include "SystemUtil.h"

/*初始化默认时间*/
#define RTC_DEFAULT_YEAR        2018
#define RTC_DEFAULT_MOUTH       04
#define RTC_DEFAULT_DAY         03
#define RTC_DEFAULT_HOUR        15
#define RTC_DEFAULT_MIN         33
#define RTC_DEFAULT_SEC         17


/*月修正数据表*/
const uint8_t table_week[] = { 0,3,3,6,1,4,6,2,5,0,3,5 };
/*平年的月份日期表*/
const uint8_t mon_table[] = { 31,28,31,30,31,30,31,31,30,31,30,31 };

static CoreRTCCallBack coreCallBackFunc = NULL;

static uint8_t CoreRTCIsLeapYear(uint16_t year)
{
    /*必须能被4整除*/
    if (year % 4 == 0)
    {
        if (year % 100 == 0)
        {
            /*如果以00结尾,还要能被400整除 */
            if (year % 400 == 0)
                return 1;
            else
                return 0;
        }
        else
            return 1;
    }
    else
        return 0;
}

static uint8_t CoreRTCGetWeek(CORE_RTC_TIME* rtcTime)
{
    uint16_t temp2;
    uint8_t yearH, yearL;

    yearH = rtcTime->year / 100;
    yearL = rtcTime->year % 100;
    /*如果为21世纪,年份数加100*/
    if (yearH > 19)
        yearL += 100;
    /*所过闰年数只算1900年之后的*/
    temp2 = yearL + yearL / 4;
    temp2 = temp2 % 7;
    temp2 = temp2 + rtcTime->day + table_week[rtcTime->month - 1];
    if (yearL % 4 == 0 && rtcTime->month < 3)
        temp2--;
    return(temp2 % 7);
}

static void CoreRTC_NVIC_Config(void)
{
    /**先禁止中断 */
    NVIC_DisableIRQ(RTC_IRQn);
    /*清除闹钟中断和秒中断*/
    RTC_ClearITPendingBit(RTC_IT_SEC | RTC_IT_OW);

    NVIC_InitTypeDef NVIC_InitStructure;
    //RTC全局中断
    NVIC_InitStructure.NVIC_IRQChannel = RTC_IRQn;
    //先占优先级1位,从优先级3位
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = CORE_RTC_ALARM_PREE_PRI;
    //先占优先级0位,从优先级4位
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = CORE_RTC_ALARM_SUB_PRI;
    //使能该通道中断
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    //根据NVIC_InitStruct中指定的参数初始化外设NVIC寄存器
    NVIC_Init(&NVIC_InitStructure);
}

/*设备初始化,根据不同的外部时钟确定*/
void CoreRTCInit(CORE_RTC_CLOCK_TYPE type)
{

}

/*获取时间*/
void CoreRTCGetTime(CORE_RTC_TIME* rtcTime)
{
    uint32_t timecount = 0;
    uint32_t temp = 0;
    uint16_t temp1 = 0;
    /*得到计数器中的值(秒钟数)*/
    timecount = RTC->CNTH;
    timecount <<= 16;
    timecount += RTC->CNTL;
    /*得到天数(秒钟数对应的)*/
    temp = timecount / 86400;
    /*从1970年开始*/
    temp1 = 1970;
    while (temp >= 365)
    {
        /*是闰年*/
        if (CoreRTCIsLeapYear(temp1))
        {
            /*闰年的秒钟数*/
            if (temp >= 366)
                temp -= 366;
            else break;
        }
        else
            /*平年 */
            temp -= 365;
        temp1++;
    }
    /*得到年份*/
    rtcTime->year = temp1;
    temp1 = 0;
    /*超过了一个月*/
    while (temp >= 28)
    {
        /*当年是不是闰年/2月份*/
        if (CoreRTCIsLeapYear(rtcTime->year) && temp1 == 1)
        {
            if (temp >= 29)
                temp -= 29;
            else
                break;
        }
        else
        {
            /*平年*/
            if (temp >= mon_table[temp1])
                temp -= mon_table[temp1];
            else
                break;
        }
        temp1++;
    }
    /*得到月份*/
    rtcTime->month = temp1 + 1;
    /*得到日期 */
    rtcTime->day = temp + 1;
    /*得到秒钟数*/
    temp = timecount % 86400;
    /*小时*/
    rtcTime->hour = temp / 3600;
    /*分钟	*/
    rtcTime->minute = (temp % 3600) / 60;
    /*秒钟*/
    rtcTime->second = (temp % 3600) % 60;
    /*获取星期   */
    rtcTime->week = CoreRTCGetWeek(rtcTime);
}

/*设置时间*/
uint8_t CoreRTCSetTime(CORE_RTC_TIME* rtcTime)
{
    u16 t;
    u32 seccount = 0;
    if (rtcTime->year < 1970 || rtcTime->year>2099)
        return 1;
    //把所有年份的秒钟相加
    for (t = 1970; t < rtcTime->year; t++)
    {
        //闰年的秒钟数
        if (CoreRTCIsLeapYear(t))
            seccount += 31622400;
        //平年的秒钟数
        else seccount += 31536000;
    }
    rtcTime->month -= 1;
    //把前面月份的秒钟数相加*/
    for (t = 0; t < rtcTime->month; t++)
    {
        /*月份秒钟数相加*/
        seccount += (u32)mon_table[t] * 86400;
        //闰年2月份增加一天的秒钟数
        if (CoreRTCIsLeapYear(rtcTime->year) && t == 1)
            seccount += 86400;
    }
    //把前面日期的秒钟数相加
    seccount += (u32)(rtcTime->day - 1) * 86400;
    //小时秒钟数
    seccount += (u32)rtcTime->hour * 3600;
    //分钟秒钟数
    seccount += (u32)rtcTime->minute * 60;
    //最后的秒钟加上去
    seccount += rtcTime->second;
    //使能PWR和BKP外设时钟
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR | RCC_APB1Periph_BKP, ENABLE);
    //使能RTC和后备寄存器访问
    PWR_BackupAccessCmd(ENABLE);
    //设置RTC计数器的值
    RTC_SetCounter(seccount);
    //等待最近一次对RTC寄存器的写操作完成
    RTC_WaitForLastTask();
    return 0;
}

/*设置闹钟和闹钟对应的中断处理函数*/
uint8_t CoreRTCSetAlarm(CORE_RTC_TIME* alarmTime,CoreRTCCallBack alarmCallBack)
{
    uint16_t t;
    uint32_t seccount = 0;
    /*关闭闹钟中断*/
    RTC->CRH &= ~(RTC_CRH_ALRIE);
    if (alarmTime->year < 1970 || alarmTime->year>2099)
        return 1;
    /**配置闹钟中断 */
    CoreRTC_NVIC_Config();
    /*把所有年份的秒钟相加*/
    for (t = 1970; t < alarmTime->year; t++)
    {
        if (CoreRTCIsLeapYear(t))
            /*闰年的秒钟数*/
            seccount += 31622400;
        else
            /*平年的秒钟数*/
            seccount += 31536000;
    }
    alarmTime->month -= 1;
    /*把前面月份的秒钟数相加*/
    for (t = 0; t < alarmTime->month; t++)
    {
        /*月份秒钟数相加*/
        seccount += (uint32_t)mon_table[t] * 86400;
        /*闰年2月份增加一天的秒钟数*/
        if (CoreRTCIsLeapYear(alarmTime->year) && t == 1)
            seccount += 86400;
    }
    /*把前面日期的秒钟数相加 */
    seccount += (uint32_t)(alarmTime->day - 1) * 86400;
    /*小时秒钟数*/
    seccount += (uint32_t)alarmTime->hour * 3600;
    /*分钟秒钟数*/
    seccount += (uint32_t)alarmTime->minute * 60;
    /*最后的秒钟加上去*/
    seccount += alarmTime->second;
    /*设置时钟*/
    //使能PWR和BKP外设时钟
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR | RCC_APB1Periph_BKP, ENABLE);
    //使能RTC和后备寄存器访问
    PWR_BackupAccessCmd(ENABLE);
    /*允许配置*/
    RTC->CRL |= 1 << 4;
    RTC->ALRL = seccount & 0xffff;
    RTC->ALRH = seccount >> 16;
    /*配置更新*/
    RTC->CRL &= ~(1 << 4);
    /*等待RTC寄存器操作完成 */
    while (!(RTC->CRL&(1 << 5)));
    /*关闭BKP的访问权限*/
    PWR_BackupAccessCmd(DISABLE);
    /*允许闹钟中断*/
    RTC->CRH |= RTC_CRH_ALRIE;
    coreCallBackFunc = alarmCallBack;
    
    return 0;
}









