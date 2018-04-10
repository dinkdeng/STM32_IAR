#include "stm32f4xx.h"
#include "SystemUtil.h"
#include "CoreTickDelay.h"
#include "DeviceLed.h"
#include "DeviceKey.h"
#include "DeviceBeep.h"
#include "CoreSerialUart1DMA.h"
#include "DeviceExti.h"
#include "DeviceAT24CXX.h"
#include "CoreRand.h"
#include "DeviceW25QXX.h"
#include "DeviceDS18B20.h"

void ShowBuffer(uint8_t* bufferPtr,uint16_t length)
{
    for(uint16_t i = 0 ; i < length; i++)
    {
        printf("  0x%02X",bufferPtr[i]);
    }
    printf("\r\n");
}

void ClearBuffer(uint8_t* bufferPtr,uint16_t length)
{
    for(uint16_t i = 0 ; i < length; i++)
    {
        bufferPtr[i] = 0;
    }
    printf("\r\n");
}

/**读取10字节 */
uint8_t testBuffer[32] = {0};

void DeviceW25QXX_Test(void)
{
    if(DeviceW25QXX_Read(testBuffer,0,32))
    {
        printf("Read Buffer Failed\r\n");
        return;
    }
    printf("Read Buffer : ");
    ShowBuffer(testBuffer,32);
    ClearBuffer(testBuffer,32);
    for(uint8_t i = 0; i < 32; i++)
    {
        testBuffer[i] = (uint8_t)CoreRandGetNextByRange(0,255);
    }
    printf("Write Buffer  : ");
    if(DeviceW25QXX_Write(testBuffer,0,32))
    {
        printf("Write Buffer Failed\r\n");
        return;
    }
    else
    {
         ShowBuffer(testBuffer,32);
    }
    ClearBuffer(testBuffer,32);
    if(DeviceW25QXX_Read(testBuffer,0,32))
    {
        printf("Read Buffer Failed\r\n");
        return;
    }
    printf("Read Buffer : ");
    ShowBuffer(testBuffer,32);
    ClearBuffer(testBuffer,32);
}


void DeviceLeftExtiProcess()
{
    DeviceExtiClear(EXTI_LEFT);
    printf("Left Exti Press\r\n");
    DeviceW25QXX_Test();
    DeviceExtiClear(EXTI_LEFT);
}

void DeviceDownExtiProcess()
{
    DeviceExtiClear(EXTI_DOWN);
    printf("Down Exti Press\r\n");
    short tempds = 0;
    if(DeviceDS18b20ReadTemp(&tempds))
    {
        printf("DS18B20 Read Failed\r\n");
    }
    else
    {
        printf("DS18B20 Temp : %0.1f \r\n",(double)tempds/10.0);
    }
}

void DeviceUpExtiProcess()
{
    DeviceExtiClear(EXTI_UP);
    printf("Up Exti Press\r\n");
}

/**系统按键检测处理 */
void ExtiCheckLoop(void)
{
    if(DeviceExtiGetCount(EXTI_LEFT) > 0)
    {
        DeviceLeftExtiProcess();
    }
    if(DeviceExtiGetCount(EXTI_DOWN) > 0)
    {
        DeviceDownExtiProcess();
    }
    if(DeviceExtiGetCount(EXTI_UP) > 0)
    {
        DeviceUpExtiProcess();
    }
}

int main(void)
{
    SystemUtilSetIntGroup();
    CoreTickDelayInit();
    DeviceLedInit(LED_STATUS_OFF);
    /*初始化EXTI*/
    DeviceExtiInit(EXTI_LEFT);
    DeviceExtiInit(EXTI_DOWN);
    DeviceExtiInit(EXTI_UP);
    /**蜂鸣器初始化 */
    DeviceBeepInit(BEEP_OFF);
    CoreSerialUart1DMA_Init(115200,CoreSerialUart1DMA_DefaultCallBack);
    CoreRandInit();
    while(DeviceAT24CxxInit())
    {
        printf("Device AT24CXX Init Failed\r\n");
        CoreTickDelayMs(1000);
    }
    while(DeviceW25QXX_Init())
    {
        printf("Device W25QXX Init Failed\r\n");
        CoreTickDelayMs(1000);
    }
    while(DeviceDS18b20Init())
    {
        printf("Device DS18B20 Init Failed\r\n");
        CoreTickDelayMs(1000);
    }
    printf("System Init Over\r\n");
    while(1)
    {
        ExtiCheckLoop();
        CoreTickDelayMs(1000);
    }
}




