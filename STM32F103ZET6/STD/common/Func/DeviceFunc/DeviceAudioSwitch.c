#include "DeviceAudioSwitch.h"
#include "SystemUtil.h"

/**使用74HC4052来控制切换 */

//4052控制设置端口
#define DeviceAudioSwitchSetPinA(n)	        (PBout(7) = n)
#define DeviceAudioSwitchSetPinB(n)	        (PDout(7) = n)

void DeviceAudioSwitchInit(DEVICE_AUDIO_SWITCH_CHANNEL initChannel)
{
    GPIO_InitTypeDef  GPIO_InitStructure;
 	
    /**使能端口时钟 */
 	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB|RCC_APB2Periph_GPIOD, ENABLE);	 
	
 	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7;		
    /**PB.7 推挽输出 */		 
 	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
 	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
 	GPIO_Init(GPIOB, &GPIO_InitStructure);
 
    /**PD.7 推挽输出 */
 	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7;
	GPIO_Init(GPIOD, &GPIO_InitStructure); 

    DeviceAudioSwitchSet(DEVICE_AUDIO_SWITCH_NONE);
}

void DeviceAudioSwitchSet(DEVICE_AUDIO_SWITCH_CHANNEL setChannel)
{
    switch(setChannel)
    {
    case DEVICE_AUDIO_SWITCH_MP3:
        DeviceAudioSwitchSetPinA(0);
        DeviceAudioSwitchSetPinB(0);
        break;
    case DEVICE_AUDIO_SWITCH_RADIO:
        DeviceAudioSwitchSetPinA(1);
        DeviceAudioSwitchSetPinB(0);
        break;
    case DEVICE_AUDIO_SWITCH_PWM:
        DeviceAudioSwitchSetPinA(0);
        DeviceAudioSwitchSetPinB(1);
        break;
    case DEVICE_AUDIO_SWITCH_NONE:
        DeviceAudioSwitchSetPinA(1);
        DeviceAudioSwitchSetPinB(1);
        break;
    default:
        return;
    }
}