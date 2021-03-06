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
#include "DeviceMPU6050.h"
#include "CoreChipFlash.h"
#include "DeviceSRAM.h"

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

/**读取32字节 */
uint8_t testBuffer[32] = {0};

void SRAM_Test(void)
{
    if(DeviceSRAM_Read(DEVICE_SRAM_BASE_ADDR,testBuffer,32))
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
    if(DeviceSRAM_Write(DEVICE_SRAM_BASE_ADDR,testBuffer,32))
    {
        printf("Write Buffer Failed\r\n");
        return;
    }
    else
    {
         ShowBuffer(testBuffer,32);
    }
    ClearBuffer(testBuffer,32);
    if(DeviceSRAM_Read(DEVICE_SRAM_BASE_ADDR,testBuffer,32))
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
}

void DeviceDownExtiProcess()
{
    DeviceExtiClear(EXTI_DOWN);
    printf("Down Exti Press\r\n");
    SRAM_Test();
    DeviceExtiClear(EXTI_DOWN);
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
    while(DeviceMPU6050_Init())
    {
        printf("Device MPU6050 Init Failed\r\n");
        CoreTickDelayMs(1000);
    }
    while(DeviceSRAM_Init())
    {
        printf("Device SRAM Init Failed\r\n");
        CoreTickDelayMs(1000);
    }
    printf("System Init Over\r\n");
    while(1)
    {
        ExtiCheckLoop();
        CoreTickDelayMs(1000);
    }
}




