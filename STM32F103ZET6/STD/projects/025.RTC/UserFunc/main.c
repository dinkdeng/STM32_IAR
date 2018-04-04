#include "stm32f10x.h"
#include "SystemUtil.h"
#include "CoreTickDelay.h"
#include "DeviceLed.h"
#include "DeviceExti.h"
#include "CoreSerialUart1Dma.h"
#include "DeviceBeep.h"
#include "DeviceSRAM.h"
#include "MemManagerLocal.h"
#include "MemManagerSRAM.h"
#include "DeviceW25qxx.h"
#include "DeviceNRF24L01.h"
#include "CoreRTC.h"

CORE_RTC_TIME systemRtc = {0};

void SetDefaultTime(CORE_RTC_TIME* rtcTime)
{
    rtcTime->year = 2018;
    rtcTime->month = 4;
    rtcTime->day = 4;
    rtcTime->week = 3;
    rtcTime->hour = 11;
    rtcTime->minute = 4;
    rtcTime->minute = 4;
    rtcTime->second = 0;
}

void RTC_Printf(CORE_RTC_TIME* rtcTime)
{

    printf("RTC Timer Is : year : %d  month : %d day : %d  week : %d \r\n", rtcTime->year,
        rtcTime->month, rtcTime->day, rtcTime->week);
    printf("               hour : %d  minute : %d  second : %d\r\n", rtcTime->hour,
        rtcTime->minute, rtcTime->second);
}

void RTCTest()
{
    if(DeviceExtiGetCount(EXTI_LEFT) > 0)
    {
        DeviceExtiClear(EXTI_LEFT);
        printf("Left Key Event\r\n");
        printf("Get SystemTime\r\n");
        CoreRTCGetTime(&systemRtc);
        RTC_Printf(&systemRtc);
    }
    if(DeviceExtiGetCount(EXTI_RIGHT) > 0)
    {
        DeviceExtiClear(EXTI_RIGHT);
        printf("Right Key Event\r\n");
        printf("Set SystemTime\r\n");
        SetDefaultTime(&systemRtc);
        CoreRTCSetTime(&systemRtc);
        RTC_Printf(&systemRtc);
    }
    if(DeviceExtiGetCount(EXTI_UP) > 0)
    {
        DeviceExtiClear(EXTI_UP);
        printf("Up Key Event\r\n");
    }
    if(DeviceExtiGetCount(EXTI_DOWN) > 0)
    {
        DeviceExtiClear(EXTI_DOWN);
        printf("Down Key Event\r\n");
    }
}

int main(void)
{
    uint8_t* uartRecvDatBufferPtr = NULL;
    uint16_t recvLength = 0;

    /*初始化系统分组*/
    SystemUtilSetIntGroup();
    /*初始化延时函数*/
    CoreTickDelayInit();
    /**外部按键初始化*/
    DeviceExtiInit(EXTI_LEFT);
    DeviceExtiInit(EXTI_RIGHT);
    DeviceExtiInit(EXTI_UP);
    DeviceExtiInit(EXTI_DOWN);
    /**初始化串口 */
    CoreSerialUart1DmaInit(115200);
    /**每次复位自动打印该数据 */
    CoreSerialUart1DmaSendString("SystemInit\r\n");
    /**初始化Beep */
    DeviceBeepInit(BEEP_OFF);
    /**初始化sram*/
    while(0 != DeviceSRAMInit())
    {
        printf("SRAM Init Failed\r\n");
        CoreTickDelayMs(500);
    }
    MemManagerLocalInit();
    MemManagerSRAMInit();
    /**初始化W25QXX */
    while(0 != DeviceW25QXXInit())
    {
        printf("DeviceW25QXX Init Failed\r\n");
        CoreTickDelayMs(500);
    }
    /**初始化NRF24L01 */
    while(0 != DeviceNRF24L01Init())
    {
        printf("DeviceNRF24L01Init Init Failed\r\n");
        CoreTickDelayMs(500);
    }
    /**初始化RTC */
    while(0 != CoreRTCInit(NULL,CORE_RTC_CLOCK_LSI))
    {
        printf("CoreRTCInit Failed\r\n");
        CoreTickDelayMs(500);
    }
    /**RTC初始化 */
    while(1)
    {
        RTCTest();
        recvLength = CoreSerialUart1DmaGetRecv(&uartRecvDatBufferPtr);
        if(recvLength != 0)
        {
            CoreSerialUart1DmaSendBuffer(uartRecvDatBufferPtr,recvLength);
            CoreSerialUart1DmaSendString("\r\n");
            CoreSerialUart1DmaClearRx();
        }
    }
}