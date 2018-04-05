#include "DeviceIR.h"
#include "SystemUtil.h"


const uint8_t IR_KEY_ERROR_DESP[] = "ERROR";
const uint8_t IR_KEY_POWER_DESP[] = "POWER";
const uint8_t IR_KEY_UP_DESP[] = "UP";
const uint8_t IR_KEY_PLAY_DESP[] = "PLAY";
const uint8_t IR_KEY_ALIENTEK_DESP[] = "ALIENTEK";
const uint8_t IR_KEY_RIGHT_DESP[] = "RIGHT";
const uint8_t IR_KEY_LEFT_DESP[] = "LEFT";
const uint8_t IR_KEY_VOL_DEC_DESP[] = "VOL-";
const uint8_t IR_KEY_VOL_ADD_DESP[] = "VOL+";
const uint8_t IR_KEY_DOWN_DESP[] = "DOWN";
const uint8_t IR_KEY_1_DESP[] = "1";
const uint8_t IR_KEY_2_DESP[] = "2";
const uint8_t IR_KEY_3_DESP[] = "3";
const uint8_t IR_KEY_4_DESP[] = "4";
const uint8_t IR_KEY_5_DESP[] = "5";
const uint8_t IR_KEY_6_DESP[] = "6";
const uint8_t IR_KEY_7_DESP[] = "7";
const uint8_t IR_KEY_8_DESP[] = "8";
const uint8_t IR_KEY_9_DESP[] = "9";
const uint8_t IR_KEY_0_DESP[] = "0";
const uint8_t IR_KEY_DELETE_DESP[] = "DELETE";

#define IR_KEY_NONE		        0
#define IR_KEY_POWER	        162
#define IR_KEY_UP		        98
#define IR_KEY_PLAY		        2
#define IR_KEY_ALIENTEK	        226
#define IR_KEY_RIGHT	        194
#define IR_KEY_LEFT		        34
#define IR_KEY_VOL_DEC	        224
#define IR_KEY_VOL_ADD	        144
#define IR_KEY_DOWN		        168
#define IR_KEY_1		        104
#define IR_KEY_2		        152
#define IR_KEY_3		        176
#define IR_KEY_4		        48
#define IR_KEY_5		        24
#define IR_KEY_6		        122
#define IR_KEY_7		        16
#define IR_KEY_8		        56
#define IR_KEY_9		        90
#define IR_KEY_0		        66
#define IR_KEY_DELETE	        82


//通过按键值获取按键标识字符串
uint8_t* DeviceIR_GetDesp(uint8_t keyValue)
{
    switch (keyValue)
    {
    case IR_KEY_NONE:
        return (uint8_t*)IR_KEY_ERROR_DESP;
    case IR_KEY_POWER:
        return (uint8_t*)IR_KEY_POWER_DESP;
    case IR_KEY_UP:
        return (uint8_t*)IR_KEY_UP_DESP;
    case IR_KEY_PLAY:
        return (uint8_t*)IR_KEY_PLAY_DESP;
    case IR_KEY_ALIENTEK:
        return (uint8_t*)IR_KEY_ALIENTEK_DESP;
    case IR_KEY_RIGHT:
        return (uint8_t*)IR_KEY_RIGHT_DESP;
    case IR_KEY_LEFT:
        return (uint8_t*)IR_KEY_LEFT_DESP;
    case IR_KEY_VOL_DEC:
        return (uint8_t*)IR_KEY_VOL_DEC_DESP;
    case IR_KEY_VOL_ADD:
        return (uint8_t*)IR_KEY_VOL_ADD_DESP;
    case IR_KEY_DOWN:
        return (uint8_t*)IR_KEY_DOWN_DESP;
    case IR_KEY_1:
        return (uint8_t*)IR_KEY_1_DESP;
    case IR_KEY_2:
        return (uint8_t*)IR_KEY_2_DESP;
    case IR_KEY_3:
        return (uint8_t*)IR_KEY_3_DESP;
    case IR_KEY_4:
        return (uint8_t*)IR_KEY_4_DESP;
    case IR_KEY_5:
        return (uint8_t*)IR_KEY_5_DESP;
    case IR_KEY_6:
        return (uint8_t*)IR_KEY_6_DESP;
    case IR_KEY_7:
        return (uint8_t*)IR_KEY_7_DESP;
    case IR_KEY_8:
        return (uint8_t*)IR_KEY_8_DESP;
    case IR_KEY_9:
        return (uint8_t*)IR_KEY_9_DESP;
    case IR_KEY_0:
        return (uint8_t*)IR_KEY_0_DESP;
    case IR_KEY_DELETE:
        return (uint8_t*)IR_KEY_DELETE_DESP;
    default:
        return (uint8_t*)"None Desp";
    }
}


















//红外数据输入脚
#define DeviceIRReadValue()           PBin(9)	

//中断处理函数,数据在中断中接收
//遥控器接收状态
//[7]:收到了引导码标志
//[6]:得到了一个按键的所有信息
//[5]:保留	
//[4]:标记上升沿是否已经被捕获								   
//[3:0]:溢出计时器
uint8_t deviceIRState = 0;
//下降沿时计数器的值
uint16_t Dval;
//红外接收到的数据
uint32_t deviceIRRecvData = 0;
//按键按下的次数
uint8_t  deviceIRDataCount = 0;

void DeviceIR_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    NVIC_InitTypeDef NVIC_InitStructure;
    TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
    TIM_ICInitTypeDef  TIM_ICInitStructure;
    //使能PORTB时钟 
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
    //TIM4 时钟使能 
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4, ENABLE);

    //PB9 输入 
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;
    //上拉输入
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPD;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOB, &GPIO_InitStructure);
    //初始化GPIOB.9
    GPIO_SetBits(GPIOB, GPIO_Pin_9);

    //设定计数器自动重装值 最大10ms溢出				  
    TIM_TimeBaseStructure.TIM_Period = 10000;
    //预分频器,1M的计数频率,1us加1. 此处不得随便修改,要根据系统定时器时钟来做
    //72分频 一个计数就是1US
    TIM_TimeBaseStructure.TIM_Prescaler = (72 - 1);
    //设置时钟分割:TDTS = Tck_tim
    TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;
    //TIM向上计数模式	
    TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
    //根据指定的参数初始化TIMx
    TIM_TimeBaseInit(TIM4, &TIM_TimeBaseStructure);

    // 选择输入端 IC4映射到TI4上
    TIM_ICInitStructure.TIM_Channel = TIM_Channel_4;
    //上升沿捕获
    TIM_ICInitStructure.TIM_ICPolarity = TIM_ICPolarity_Rising;
    TIM_ICInitStructure.TIM_ICSelection = TIM_ICSelection_DirectTI;
    //配置输入分频,不分频 
    TIM_ICInitStructure.TIM_ICPrescaler = TIM_ICPSC_DIV1;
    //IC4F=0011 配置输入滤波器 8个定时器时钟周期滤波
    TIM_ICInitStructure.TIM_ICFilter = 0x03;
    //初始化定时器输入捕获通道
    TIM_ICInit(TIM4, &TIM_ICInitStructure);

    //使能定时器4
    TIM_Cmd(TIM4, ENABLE);

    //TIM4中断
    NVIC_InitStructure.NVIC_IRQChannel = TIM4_IRQn;
    //先占优先级
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = DEVICE_IR_RECV_PREE_PRI;
    //从优先级
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = DEVICE_IR_RECV_SUB_PRI;
    //IRQ通道被使能
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    //根据NVIC_InitStruct中指定的参数初始化外设NVIC寄存器
    NVIC_Init(&NVIC_InitStructure);
    //允许更新中断 ,允许CC4IE捕获中断
    TIM_ITConfig(TIM4, TIM_IT_Update | TIM_IT_CC4, ENABLE);
}

uint8_t DeviceIR_GetKeyValue(uint8_t* keyValue)
{
    uint8_t sta = 0;
    uint8_t t1, t2;
    //得到一个按键的所有信息了
    if (deviceIRState&(1 << 6))
    {
        //得到地址码
        t1 = deviceIRRecvData >> 24;
        //得到地址反码 
        t2 = (deviceIRRecvData >> 16) & 0xff;
        //检验遥控识别码(ID)及地址
        if ((t1 == (u8)~t2) && t1 == REMOTE_ID)
        {
            t1 = deviceIRRecvData >> 8;
            t2 = deviceIRRecvData;
            //键值正确
            if (t1 == (u8)~t2)
                sta = t1;
        }
        //按键数据错误/遥控已经没有按下了
        if ((sta == 0) || ((deviceIRState & 0X80) == 0))
        {
            //清除接收到有效按键标识
            deviceIRState &= ~(1 << 6);
            //清除按键次数计数器
            deviceIRDataCount = 0;
        }
        *keyValue = sta;
        if (deviceIRDataCount == 0)
            return 1;
        else
            return deviceIRDataCount;
    }
    else
    {
        return 0;
    }
}


void TIM4_IRQHandler(void)
{
    if (TIM_GetITStatus(TIM4, TIM_IT_Update) != RESET)
    {
        //上次有数据被接收到了
        if (deviceIRState & 0x80)
        {
            //取消上升沿已经被捕获标记
            deviceIRState &= ~0X10;
            //标记已经完成一次按键的键值信息采集
            if ((deviceIRState & 0X0F) == 0X00)
                deviceIRState |= 1 << 6;
            if ((deviceIRState & 0X0F) < 14)
                deviceIRState++;
            else
            {
                //清空引导标识
                deviceIRState &= ~(1 << 7);
                //清空计数器
                deviceIRState &= 0XF0;
            }
        }
    }
    if (TIM_GetITStatus(TIM4, TIM_IT_CC4) != RESET)
    {
        //上升沿捕获
        if (DeviceIRReadValue())
        {
            //CC1P=1 设置为下降沿捕获	
            TIM_OC4PolarityConfig(TIM4, TIM_ICPolarity_Falling);
            //清空定时器值
            TIM_SetCounter(TIM4, 0);
            //标记上升沿已经被捕获
            deviceIRState |= 0X10;
        }
        //下降沿捕获
        else
        {
            //读取CCR1也可以清CC1IF标志位
            Dval = TIM_GetCapture4(TIM4);
            //CC4P=0	设置为上升沿捕获
            TIM_OC4PolarityConfig(TIM4, TIM_ICPolarity_Rising);
            //完成一次高电平捕获 
            if (deviceIRState & 0X10)
            {
                //接收到了引导码
                if (deviceIRState & 0X80)
                {
                    //560为标准值,560us
                    if (Dval > 300 && Dval < 800)
                    {
                        //左移一位.
                        deviceIRRecvData <<= 1;
                        //接收到0
                        deviceIRRecvData |= 0;
                    }
                    //1680为标准值,1680us
                    else if (Dval > 1400 && Dval < 1800)
                    {
                        //左移一位.
                        deviceIRRecvData <<= 1;
                        //接收到1
                        deviceIRRecvData |= 1;
                    }
                    //得到按键键值增加的信息 2500为标准值2.5ms
                    else if (Dval > 2200 && Dval < 2600)
                    {
                        //按键次数增加1次
                        deviceIRDataCount++;
                        //清空计时器
                        deviceIRState &= 0XF0;
                    }
                }
                //4500为标准值4.5ms
                else if (Dval > 4200 && Dval < 4700)
                {
                    //标记成功接收到了引导码
                    deviceIRState |= 1 << 7;
                    //清除按键次数计数器
                    deviceIRDataCount = 0;
                }
            }
            deviceIRState &= ~(1 << 4);
        }
    }
    TIM_ClearFlag(TIM4, TIM_IT_Update | TIM_IT_CC4);
}

















