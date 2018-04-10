#ifndef __DEVICE_LCD_H_
#define __DEVICE_LCD_H_
#include "stm32f4xx.h"

/********************屏幕方向定义*******************/
//屏幕方向竖屏
#define LCD_DIR_VERTICAL	0
//屏幕方向横屏
#define LCD_DIR_HORIZONTAL	1
//屏幕默认初始化方向
#define LCD_DEFAULT_DIR		LCD_DIR_VERTICAL
/********************屏幕方向定义*******************/

#define DEVICE_LCD_WAIT_MS(nms)         CoreTickDelayMs(nms)

#define Device_LCD_WAIT_US(nus)         CoreTickDelayUs(nus)

/***************一些必要的操作延时时间的定义*******************/
//读取LCD寄存器的时候的等待时间 单位为US
#define  LCD_READ_REG_WAIT_TIME		5
//等待FSMC稳定时间 单位MS
#define LCD_FSMC_WAIT_START_TIME	50
//等待芯片配置时间 单位MS
#define LCD_WAIT_CONFIG_TIME		120
//id9328配置时候需要的等待时间 单位MS
#define LCD_ID9328_WAIT_CONFIG_TIME	50
//id9320配置时候需要的等待时间 单位MS
#define LCD_ID9320_WAIT_CONFIG_TIME	50
/***************一些必要的操作延时时间的定义*******************/

/***************************扫描方向定义*******************************/
//从左到右,从上到下
#define L2R_U2D  0
//从左到右,从下到上
#define L2R_D2U  1
//从右到左,从上到下
#define R2L_U2D  2
//从右到左,从下到上
#define R2L_D2U  3

//从上到下,从左到右
#define U2D_L2R  4
//从上到下,从右到左
#define U2D_R2L  5
//从下到上,从左到右
#define D2U_L2R  6
//从下到上,从右到左
#define D2U_R2L  7

//默认的扫描方向 从左到右 从上到下
#define DFT_SCAN_DIR  L2R_U2D
/***************************扫描方向定义*******************************/

/****************************画笔基础颜色宏定义*************************/
//白色
#define WHITE         	 0xFFFF
//黑色
#define BLACK         	 0x0000
//蓝色
#define BLUE         	 0x001F
//蓝红
#define BRED             0XF81F
#define GRED 			 0XFFE0
#define GBLUE			 0X07FF
//红色
#define RED           	 0xF800
#define MAGENTA       	 0xF81F
#define GREEN         	 0x07E0
#define CYAN          	 0x7FFF
//黄色
#define YELLOW        	 0xFFE0
//棕色
#define BROWN 			 0XBC40
//棕红色
#define BRRED 			 0XFC07
//灰色
#define GRAY  			 0X8430
/****************************GUI颜色*************************/
//深蓝色
#define DARKBLUE      	 0X01CF
//浅蓝色
#define LIGHTBLUE      	 0X7D7C
//灰蓝色
#define GRAYBLUE       	 0X5458
//浅绿色
#define LIGHTGREEN     	 0X841F
//浅灰色(PANNEL),窗体背景色
#define LGRAY 			 0XC618
//浅灰蓝色(中间层颜色)
#define LGRAYBLUE        0XA651
//浅棕蓝色(选择条目的反色)
#define LBBLUE           0X2B12
/****************************GUI颜色*************************/
/****************************画笔基础颜色宏定义*************************/


/****************************初始化默认背景色与前景色定义**************************/
//默认前景色  黑色
#define LCD_DEFAULT_POINT_COLOR		BLACK
//默认背景色 白色
#define LCD_DEFAULT_BACK_COLOR		WHITE
/****************************初始化默认背景色与前景色定义**************************/

/*******************LCD分辨率设置与时序参数设定**********************/
//LCD水平分辨率
#define SSD_HOR_RESOLUTION		800
//LCD垂直分辨率
#define SSD_VER_RESOLUTION		480
//LCD驱动参数设置
//水平脉宽
#define SSD_HOR_PULSE_WIDTH		1
//水平前廊
#define SSD_HOR_BACK_PORCH		46
//水平后廊
#define SSD_HOR_FRONT_PORCH		210
//垂直脉宽
#define SSD_VER_PULSE_WIDTH		1
//垂直前廊
#define SSD_VER_BACK_PORCH		23
//垂直前廊
#define SSD_VER_FRONT_PORCH		22
//如下几个参数，自动计算
#define SSD_HT	(SSD_HOR_RESOLUTION+SSD_HOR_BACK_PORCH+SSD_HOR_FRONT_PORCH)

#define SSD_HPS	(SSD_HOR_BACK_PORCH)

#define SSD_VT 	(SSD_VER_RESOLUTION+SSD_VER_BACK_PORCH+SSD_VER_FRONT_PORCH)

#define SSD_VPS (SSD_VER_BACK_PORCH)
/*******************LCD分辨率设置与时序参数设定**********************/



//LCD重要参数集
typedef struct
{
	//LCD 宽度
	u16 width;
	//LCD 高度
	u16 height;
	//LCD ID
	u16 id;
	//横屏还是竖屏控制：
	//LCD_DIR_VERTICAL竖屏 LCD_DIR_HORIZONTAL 横屏。
	u8  dir;
	//开始写gram指令
	u16	wramcmd;
	//设置x坐标指令
	u16  setxcmd;
	//设置y坐标指令
	u16  setycmd;
}_lcd_dev;


//管理LCD重要参数 LCD设备参数
extern _lcd_dev lcddev;

//前景色
extern u16  POINT_COLOR;

//背景颜色
extern u16  BACK_COLOR;

//LCD背光    		 PB15
#define	LCD_LED PBout(15)

//LCD地址结构体
typedef struct
{
	//寄存器地址映射
	vu16 LCD_REG;
	//数据RAM地址映射
	vu16 LCD_RAM;
} LCD_TypeDef;

//使用NOR/SRAM的 Bank1.sector4,地址位HADDR[27,26]=11
//A6作为数据命令区分线
//注意设置时STM32内部会右移一位对其! 111 1110=0X7E
//LCD基地址
#define LCD_BASE        ((u32)(0x6C000000 | 0x0000007E))
//通过基地址定义寄存器写入地址和缓冲区地址
#define LCD             ((LCD_TypeDef *) LCD_BASE)


/*************************LCD编程API*************************/
//初始化
void LCD_Init(void);

//开显示
void LCD_DisplayOn(void);

//关显示
void LCD_DisplayOff(void);

//清屏
void LCD_Clear(u16 Color);

//设置光标
void LCD_SetCursor(u16 Xpos, u16 Ypos);

//画点
void LCD_DrawPoint(u16 x,u16 y);

//快速画点
void LCD_Fast_DrawPoint(u16 x,u16 y,u16 color);

//读点
u16  LCD_ReadPoint(u16 x,u16 y);

//画圆
void LCD_Draw_Circle(u16 x0,u16 y0,u8 r);

//画线
void LCD_DrawLine(u16 x1, u16 y1, u16 x2, u16 y2);

//画矩形
void LCD_DrawRectangle(u16 x1, u16 y1, u16 x2, u16 y2);

//填充单色
void LCD_Fill(u16 sx,u16 sy,u16 ex,u16 ey,u16 color);

//填充指定颜色
void LCD_Color_Fill(u16 sx,u16 sy,u16 ex,u16 ey,u16 *color);

//显示一个字符
void LCD_ShowChar(u16 x,u16 y,u8 num,u8 size,u8 mode);

//显示一个数字
void LCD_ShowNum(u16 x,u16 y,u32 num,u8 len,u8 size);

//显示数字
void LCD_ShowxNum(u16 x,u16 y,u32 num,u8 len,u8 size,u8 mode);

//显示一个字符串,12/16字体
void LCD_ShowString(u16 x,u16 y,u16 width,u16 height,u8 size,u8 *p);

//内置函数一般不作为外部API给客户调用
//写寄存器
void LCD_WriteReg(u16 LCD_Reg, u16 LCD_RegValue);

//读寄存器
u16 LCD_ReadReg(u16 LCD_Reg);

//写RAM前置准备工作
void LCD_WriteRAM_Prepare(void);

//写RAM操作
void LCD_WriteRAM(u16 RGB_Code);

//SSD1963 背光控制
void LCD_SSD_BackLightSet(u8 pwm);

//设置屏扫描方向
void LCD_Scan_Dir(u8 dir);

//设置屏幕显示方向
void LCD_Display_Dir(u8 dir);

//设置窗口大小
void LCD_Set_Window(u16 sx,u16 sy,u16 width,u16 height);

/*************************LCD编程API*************************/




#endif




