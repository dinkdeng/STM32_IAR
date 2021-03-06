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

void ChipFlashTest(void)
{
    if(CoreChipFlashRead(CORE_CHIP_ADDR_FLASH_SECTOR_5,(uint32_t*)testBuffer,8))
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
    if(CoreChipFlashWrite(CORE_CHIP_ADDR_FLASH_SECTOR_5,(uint32_t*)testBuffer,8))
    {
        printf("Write Buffer Failed\r\n");
        return;
    }
    else
    {
         ShowBuffer(testBuffer,32);
    }
    ClearBuffer(testBuffer,32);
    if(CoreChipFlashRead(CORE_CHIP_ADDR_FLASH_SECTOR_5,(uint32_t*)testBuffer,8))
    {
        printf("Read Buffer Failed\r\n");
        return;
    }
    printf("Read Buffer : ");
    ShowBuffer(testBuffer,32);
    ClearBuffer(testBuffer,32);
}

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
    ChipFlashTest();
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


void ShowMPU6050_Value()
{
    short tempValue;
    short gx,gy,gz;
    short ax,ay,az;
    if(DeviceMPU6050_GetTemperature(&tempValue))
    {
        printf("DeviceMPU6050_GetTemperature Failed \r\n");
    }
    else
    {
        printf("DeviceMPU6050_GetTemperature Result %0.1f \r\n",tempValue/100.0);
    }
    if(DeviceMPU6050_GetGyroscope(&gx,&gy,&gz))
    {
        printf("DeviceMPU6050_GetGyroscope Failed\r\n");
    }
    else
    {
        printf("DeviceMPU6050_GetGyroscope Result gx: %d  gy: %d   gz: %d\r\n",gx,gy,gz);
    }
    if(DeviceMPU6050_GetAccelerometer(&ax,&ay,&az))
    {
        printf("DeviceMPU6050_GetAccelerometer Failed\r\n");
    }
    else
    {
        printf("DeviceMPU6050_GetAccelerometer Result ax: %d  ay: %d   az: %d\r\n",ax,ay,az);
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
    printf("System Init Over\r\n");
    while(1)
    {
        ExtiCheckLoop();
        //ShowMPU6050_Value();
        CoreTickDelayMs(1000);
    }
}




