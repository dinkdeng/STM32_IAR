#include "stm32f10x.h"
#include "SystemUtil.h"
#include "CoreTickDelay.h"
#include "DeviceLed.h"
#include "DeviceExti.h"
#include "CoreSerialUart1Dma.h"
#include "DeviceBeep.h"
#include "DeviceSRAM.h"
#include "DeviceAudioSwitch.h"
#include "DeviceRDA5820.h"

/**设置基础频率 */
uint16_t baseFreq = 8700;

void SystemRDA5820_AutoScan(void)
{
    uint16_t currentSingleStrength = 0;
    printf("Auto Scan Start\r\n");
    baseFreq += 100;
    for(;baseFreq < 10800;baseFreq+=10)
    {
        DeviceRDA520_SetFreq(baseFreq);
        CoreTickDelayMs(200);
        currentSingleStrength = DeviceRDA5820_GetSingleStrength();
        printf("Current Freq : %0.2f , currentSingleStrength : %d \r\n",baseFreq/100.0,currentSingleStrength);
        if(DeviceRDA5820_CurrentFreqIsAState())
        {
            DeviceRDA520_SetFreq(baseFreq);
            break;
        }
    }
    printf("Auto Scan End\r\n");
}

void SystemRDA5820_Test()
{
    if(DeviceExtiGetCount(EXTI_LEFT) > 0)
    {
        DeviceExtiClear(EXTI_LEFT);
        printf("Left Key Event\r\n");
        baseFreq += 10;
        if(baseFreq > 10800)
            baseFreq = 10800;
        DeviceRDA520_SetFreq(baseFreq);
        CoreTickDelayMs(20);
        printf("Current Freq %0.2f , Single Strength %d\r\n",baseFreq/100.0,DeviceRDA5820_GetSingleStrength());
    }
    if(DeviceExtiGetCount(EXTI_RIGHT) > 0)
    {
        DeviceExtiClear(EXTI_RIGHT);
        printf("Right Key Event\r\n");
        baseFreq = 8700;
        DeviceRDA520_SetFreq(baseFreq);
        CoreTickDelayMs(20);
        printf("Current Freq %0.2f , Single Strength %d\r\n",baseFreq/100.0,DeviceRDA5820_GetSingleStrength());
    }
    if(DeviceExtiGetCount(EXTI_DOWN) > 0)
    {
        DeviceExtiClear(EXTI_DOWN);
        printf("Down Key Event\r\n");
        SystemRDA5820_AutoScan();
        DeviceExtiClear(EXTI_DOWN);
    }
    if(DeviceExtiGetCount(EXTI_UP) > 0)
    {
        DeviceExtiClear(EXTI_UP);
        printf("Up Key Event\r\n");

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
    DeviceAudioSwitchInit(DEVICE_AUDIO_SWITCH_RADIO);
    /**初始化sram*/
    while(0 != DeviceSRAMInit())
    {
        printf("SRAM Init Failed\r\n");
        CoreTickDelayMs(500);
    }
    /**RDA5820初始化 */
    while(DeviceRDA5820_Init(RDA5820_BAUD_87_108MHZ,RDA5820_RESOLUTION_100K,9600,15) != 0)
    {
        printf("DeviceRDA5820_Init Failed\r\n");
        CoreTickDelayMs(500);
    }
    while(1)
    {
        SystemRDA5820_Test();
        recvLength = CoreSerialUart1DmaGetRecv(&uartRecvDatBufferPtr);
        if(recvLength != 0)
        {
            CoreSerialUart1DmaSendBuffer(uartRecvDatBufferPtr,recvLength);
            CoreSerialUart1DmaSendString("\r\n");
            CoreSerialUart1DmaClearRx();
        }
    }
}