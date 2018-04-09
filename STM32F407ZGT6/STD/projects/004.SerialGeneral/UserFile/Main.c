#include "stm32f4xx.h"
#include "SystemUtil.h"
#include "CoreTickDelay.h"
#include "DeviceLed.h"
#include "DeviceKey.h"
#include "DeviceBeep.h"
#include "CoreSerialUart1General.h"

int main(void)
{
    SystemUtilSetIntGroup();
    CoreTickDelayInit();
    DeviceLedInit(LED_STATUS_OFF);
    DeviceKeyInit();
    DeviceBeepInit(BEEP_OFF);
    CoreSerialUart1GeneralInit(115200);
    printf("System Init Over\r\n");
    while(1)
    {
        CoreTickDelayMs(1000);
        printf("System Running\r\n");
    }
}




