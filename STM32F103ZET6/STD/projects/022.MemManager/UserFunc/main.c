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

uint32_t* mallocPtr = NULL;
uint32_t size = 10;

uint32_t* mallocPtrSRAM = NULL;
uint32_t sizeSRAM = 10;

void MemManagerFuncTest()
{
    if(DeviceExtiGetCount(EXTI_LEFT) > 0)
    {
        DeviceExtiClear(EXTI_LEFT);
        printf("Left Key Event\r\n");
        size *= 2;
        mallocPtr = (uint32_t*)MemManagerLocalMalloc(size);
        printf("Size = %d mallocPtr Value :0x%p , used %d % \r\n",size,mallocPtr,MemManagerLocalUsed());
        MemManagerLocalFree(mallocPtr);
    }
    if(DeviceExtiGetCount(EXTI_RIGHT) > 0)
    {
        DeviceExtiClear(EXTI_RIGHT);
        printf("Right Key Event\r\n");
        size /= 2;
        mallocPtr = (uint32_t*)MemManagerLocalMalloc(size);
        printf("Size = %d mallocPtr Value :0x%p , used %d % \r\n",size,mallocPtr,MemManagerLocalUsed());
        MemManagerLocalFree(mallocPtr);
    }
    if(DeviceExtiGetCount(EXTI_UP) > 0)
    {
        DeviceExtiClear(EXTI_UP);
        printf("Up Key Event\r\n");

        sizeSRAM *= 2;
        mallocPtrSRAM = (uint32_t*)MemManagerSRAMMalloc(sizeSRAM);
        printf("sizeSRAM = %d mallocPtrSRAM Value :0x%p , used %d % \r\n",sizeSRAM,mallocPtrSRAM,MemManagerSRAMUsed());
        MemManagerSRAMFree(mallocPtrSRAM);
    }
    if(DeviceExtiGetCount(EXTI_DOWN) > 0)
    {
        DeviceExtiClear(EXTI_DOWN);
        printf("Down Key Event\r\n");

        sizeSRAM /= 2;
        mallocPtrSRAM = (uint32_t*)MemManagerSRAMMalloc(sizeSRAM);
        printf("sizeSRAM = %d mallocPtrSRAM Value :0x%p , used %d % \r\n",sizeSRAM,mallocPtrSRAM,MemManagerSRAMUsed());
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