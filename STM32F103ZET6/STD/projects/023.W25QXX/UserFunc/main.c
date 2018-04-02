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

uint8_t* mallocPtr = NULL;
uint8_t* mallocPtrSRAM = NULL;

void ShowW25QXXBuffer(uint8_t* ptr)
{
    uint32_t i = 0;
    printf("Buffer Ptr : 0x%p \t Read Buffer: \r\n",ptr);
    for(i = 0; i < 255; i++)
    {
        printf("0X%02X  ",ptr[i]);
    }
    printf("\r\n");
}

void MemManagerFuncTest()
{
    if(DeviceExtiGetCount(EXTI_LEFT) > 0)
    {
        DeviceExtiClear(EXTI_LEFT);
        printf("Left Key Event\r\n");
        mallocPtr = (uint8_t*)MemManagerLocalMalloc(255);

        if(0 != DeviceW25QXXRead(mallocPtr,0x1234,255))
        {
            printf("DeviceW25QXXRead Failed\r\n");
        }
        else
        {
            ShowW25QXXBuffer(mallocPtr);
        }
        MemManagerLocalFree(mallocPtr);
    }
    if(DeviceExtiGetCount(EXTI_RIGHT) > 0)
    {
        DeviceExtiClear(EXTI_RIGHT);
        printf("Right Key Event\r\n");
        mallocPtrSRAM = (uint8_t*)MemManagerSRAMMalloc(255);

        if(0 != DeviceW25QXXRead(mallocPtrSRAM,0x1234,255))
        {
            printf("DeviceW25QXXRead Failed\r\n");
        }
        else
        {
            ShowW25QXXBuffer(mallocPtrSRAM);
        }

        MemManagerSRAMFree(mallocPtrSRAM);
    }
    if(DeviceExtiGetCount(EXTI_UP) > 0)
    {
        DeviceExtiClear(EXTI_UP);
        printf("Up Key Event\r\n");
        mallocPtr = (uint8_t*)MemManagerLocalMalloc(255);
        for(uint16_t i= 0; i< 255; i++)
            mallocPtr[i] = i;
        if(0 != DeviceW25QXXWrite(mallocPtr,0x1234,255))
        {
            printf("DeviceW25QXXWrite Failed\r\n");
        }
        else
        {
            printf("DeviceW25QXXWrite Success\r\n");
        }
        MemManagerLocalFree(mallocPtr);

    }
    if(DeviceExtiGetCount(EXTI_DOWN) > 0)
    {
        DeviceExtiClear(EXTI_DOWN);
        printf("Down Key Event\r\n");

        mallocPtrSRAM = (uint8_t*)MemManagerSRAMMalloc(255);
        for(uint16_t i= 0; i< 255; i++)
            mallocPtrSRAM[i] = 255-i;

        if(0 != DeviceW25QXXWrite(mallocPtrSRAM,0x1234,255))
        {
            printf("DeviceW25QXXWrite Failed\r\n");
        }
        else
        {
            printf("DeviceW25QXXWrite Success\r\n");
        }

        MemManagerSRAMFree(mallocPtrSRAM);
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
    while(1)
    {
        MemManagerFuncTest();
        recvLength = CoreSerialUart1DmaGetRecv(&uartRecvDatBufferPtr);
        if(recvLength != 0)
        {
            CoreSerialUart1DmaSendBuffer(uartRecvDatBufferPtr,recvLength);
            CoreSerialUart1DmaSendString("\r\n");
            CoreSerialUart1DmaClearRx();
        }
    }
}