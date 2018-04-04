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
#include "DeviceFCPAD.h"


void FCPAD_Test()
{
    uint8_t keyValue = 0;
    keyValue = DeviceFCPAD_ReadKey();
    if(0 != keyValue)
    {
        if(keyValue&DEVICE_FCPAD_KEY_MASK_A)
        {
            printf("DEVICE_FCPAD_KEY_MASK_A\r\n");
        }
        if(keyValue&DEVICE_FCPAD_KEY_MASK_B)
        {
            printf("DEVICE_FCPAD_KEY_MASK_B\r\n");
        }
        if(keyValue&DEVICE_FCPAD_KEY_MASK_SELECT)
        {
            printf("DEVICE_FCPAD_KEY_MASK_SELECT\r\n");
        }
        if(keyValue&DEVICE_FCPAD_KEY_MASK_START)
        {
            printf("DEVICE_FCPAD_KEY_MASK_START\r\n");
        }
        if(keyValue&DEVICE_FCPAD_KEY_MASK_UP)
        {
            printf("DEVICE_FCPAD_KEY_MASK_UP\r\n");
        }
        if(keyValue&DEVICE_FCPAD_KEY_MASK_DOWN)
        {
            printf("DEVICE_FCPAD_KEY_MASK_DOWN\r\n");
        }
        if(keyValue&DEVICE_FCPAD_KEY_MASK_LEFT)
        {
            printf("DEVICE_FCPAD_KEY_MASK_LEFT\r\n");
        }
        if(keyValue&DEVICE_FCPAD_KEY_MASK_RIGHT)
        {
            printf("DEVICE_FCPAD_KEY_MASK_RIGHT\r\n");
        }

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
    DeviceFCPAD_Init();
    /**RTC初始化 */
    while(1)
    {
        FCPAD_Test();
        recvLength = CoreSerialUart1DmaGetRecv(&uartRecvDatBufferPtr);
        if(recvLength != 0)
        {
            CoreSerialUart1DmaSendBuffer(uartRecvDatBufferPtr,recvLength);
            CoreSerialUart1DmaSendString("\r\n");
            CoreSerialUart1DmaClearRx();
        }
    }
}