#include "stm32f10x.h"
#include "SystemUtil.h"
#include "CoreTickDelay.h"
#include "DeviceLed.h"
#include "DeviceExti.h"
#include "CoreSerialUart1Dma.h"
#include "DeviceBeep.h"
#include "DeviceSRAM.h"
#include "DeviceSD.h"
#include "ff.h"
#include "DeviceAudioSwitch.h"
#include "DeviceVS10xx.h"

FATFS fs;
FRESULT fresult;
FIL fil;
uint32_t readCount = 0;

uint8_t musicName1[] = "0:/music/fukua.mp3";
uint8_t musicName2[] = "0:/music/gulongqunxiazhuan.mp3";
uint8_t musicName3[] = "0:/music/juebieshi.mp3";
uint8_t musicName4[] = "0:/music/naoshazimanao.mp3";
uint8_t musicName5[] = "0:/music/tianzishisansha.mp3";
uint8_t musicName6[] = "0:/music/wuzibei.mp3";
uint8_t musicName7[] = "0:/music/yueguang.mp3";
uint8_t musicName8[] = "0:/music/zhangyingxiong.mp3";

uint8_t* musicNamePtrs[8] = {musicName1,musicName2,musicName3,musicName4,
                        musicName5,musicName6,musicName7,musicName8};

uint8_t fatfsReadBuffer[512] = {0};


int8_t musicIndex = -1;
uint32_t bufferDataResidue = 0;

void SystemMusicSwitch()
{
    if(musicIndex >= 0)
    {
        musicIndex++;
        if(musicIndex > 7)
            musicIndex = 0;
        f_close(&fil);
        /**直接打开文件 */
        fresult = f_open(&fil, (char const*)(musicNamePtrs[musicIndex]), FA_READ | FA_OPEN_ALWAYS);
        bufferDataResidue = 0;
        if(fresult == FR_OK)
        {
            /**切歌 */
            DeviceVS10xxRestartPlay();
            DeviceVS10xxSetAllDefault();
            printf("Current Music : %s\r\n",(char const*)musicNamePtrs[musicIndex]);
        }
        else
        {
            printf("f_open music failed : %d\r\n",0);
            musicIndex = -1;
        }
    }
    else
    {
        /**直接打开文件 */
        fresult = f_open(&fil, (char const*)musicNamePtrs[0], FA_READ | FA_OPEN_ALWAYS);
        if(fresult == FR_OK)
        {
            /**切歌 */
            DeviceVS10xxRestartPlay();
            DeviceVS10xxSetAllDefault();
            musicIndex = 0;
            bufferDataResidue = 0;
            printf("Current Music : %s\r\n",(char const*)musicNamePtrs[musicIndex]);
        }
        else
        {
            printf("f_open music failed : %d\r\n",0);
            musicIndex = -1;
        }
    }
}

void SystemExti_Task()
{
    if(DeviceExtiGetCount(EXTI_LEFT) > 0)
    {
        DeviceExtiClear(EXTI_LEFT);
        printf("Left Key Event\r\n");
        SystemMusicSwitch();
        CoreTickDelayMs(1000);
        DeviceExtiClear(EXTI_LEFT);
    }
    if(DeviceExtiGetCount(EXTI_RIGHT) > 0)
    {
        DeviceExtiClear(EXTI_RIGHT);
        printf("Right Key Event\r\n");
    }
    if(DeviceExtiGetCount(EXTI_DOWN) > 0)
    {
        DeviceExtiClear(EXTI_DOWN);
        printf("Down Key Event\r\n");

    }
    if(DeviceExtiGetCount(EXTI_UP) > 0)
    {
        DeviceExtiClear(EXTI_UP);
        printf("Up Key Event\r\n");
    }
}


void SystemMusic_Task()
{
    if(musicIndex < 0)
    {
        bufferDataResidue = 0;
        return;
    }
    if(musicIndex >= 0 && bufferDataResidue == 0)
    {
        fresult = f_read(&fil,fatfsReadBuffer,512,&bufferDataResidue);
        if(fresult)
        {
            printf("fread Error\r\n");
        }
        if(bufferDataResidue == 0)
        {
            musicIndex++;
            if(musicIndex > 7)
                musicIndex = 0;
            f_close(&fil);
            /**直接打开文件 */
            fresult = f_open(&fil, (char const*)musicNamePtrs[musicIndex], FA_READ);
            if(fresult == FR_OK)
            {
                /**切歌 */
                DeviceVS10xxRestartPlay();
                DeviceVS10xxSetAllDefault();
                fresult = f_read(&fil,fatfsReadBuffer,512,&bufferDataResidue);
                if(fresult)
                {
                    printf("fread Error\r\n");
                }
                printf("Current Music : %s\r\n",musicNamePtrs[musicIndex]);
            }
            else
            {
                printf("f_open music failed : %d\r\n",0);
                musicIndex = -1;
            }
        }
    }
    else
    {
        if(0 == DeviceVS10xxSendMusicData(fatfsReadBuffer+512-bufferDataResidue))
        {
            bufferDataResidue -= 32;
            if(bufferDataResidue > 512)
                bufferDataResidue = 0;
        }
    }
}

int main(void)
{

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
    /**初始化Beep */
    DeviceBeepInit(BEEP_OFF);
    /**初始化sram*/
    while(0 != DeviceSRAMInit())
    {
        printf("SRAM Init Failed\r\n");
        CoreTickDelayMs(500);
    }
    /**文件系统加载 */
    do
    {
        fresult = f_mount(&fs,"0:/",1);
        if(fresult != FR_OK)
        {
            printf("f_mount Failed : %d\r\n",fresult);
            CoreTickDelayMs(500);
        }
    }while(fresult != FR_OK);
    /**Audio Switch切换 */
    DeviceAudioSwitchInit(DEVICE_AUDIO_SWITCH_MP3);
    /**VS1053 Init */
    while(DeviceVS1053Init())
    {
        printf("DeviceVS1053Init Failed \r\n");
        CoreTickDelayMs(500);
    }
    /**每次复位自动打印该数据 */
    CoreSerialUart1DmaSendString("SystemInit\r\n");
    while(1)
    {
        SystemExti_Task();
        SystemMusic_Task();
    }
}