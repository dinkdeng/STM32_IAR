#ifndef __DEVICE_LCD_H_
#define __DEVICE_LCD_H_
#include "stm32f4xx.h"

/**延时接口 */
/**毫秒延时 */
#define DEVICE_LCD_WAIT_MS(nms)         CoreTickDelayMs(nms)
/**微秒延时 */
#define Device_LCD_WAIT_US(nus)         CoreTickDelayUs(nus)

/***************一些必要的操作延时时间的定义*******************/
//读取LCD寄存器的时候的等待时间 单位为US
#define  LCD_READ_REG_WAIT_TIME		            5
//等待FSMC稳定时间 单位MS
#define LCD_FSMC_WAIT_START_TIME	            50
//等待芯片配置时间 单位MS
#define LCD_WAIT_CONFIG_TIME		            120
//id9328配置时候需要的等待时间 单位MS
#define LCD_ID9328_WAIT_CONFIG_TIME	            50
//id9320配置时候需要的等待时间 单位MS
#define LCD_ID9320_WAIT_CONFIG_TIME	            50
/***************一些必要的操作延时时间的定义*******************/

/**芯片ID定义,只支持该芯片的驱动 */
#define DEVICE_LCD_DRIVER_IC_ID                 0X5510

/**屏幕宽高设置 */
#define DEVICE_LCD_HEIGHT                       800
#define DEVICE_LCD_WIDTH                        480

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
/**红绿 */
#define GRED 			 0XFFE0
/**蓝黄 */
#define GBLUE			 0X07FF
//红色
#define RED           	 0xF800
/**紫色 */
#define MAGENTA       	 0xF81F
/**绿色 */
#define GREEN         	 0x07E0
/**灰色 */
#define CYAN          	 0x7FFF
//黄色
#define YELLOW        	 0xFFE0
//棕色
#define BROWN 			 0XBC40 
//棕红色
#define BRRED 			 0XFC07 
//灰色
#define GRAY  			 0X8430 
/****************************画笔基础颜色宏定义*************************/


/**LCD初始化 */
uint8_t DeviceLCD_Init(void);

void DeviceLCD_Clear(uint16_t color);

//LCD开启显示
void DeviceLCD_DisplayOn(void);

//LCD关闭显示
void DeviceLCD_DisplayOff(void);

//画点
//x,y:坐标
//POINT_COLOR:此点的颜色
void DeviceLCD_DrawPoint(uint16_t x,uint16_t y,uint16_t color);

//快速画点
//x,y:坐标
//color:颜色
void DeviceLCD_FastDrawPoint(uint16_t x,uint16_t y,uint16_t color);

//在指定区域内填充单个颜色
//(sx,sy),(ex,ey):填充矩形对角坐标,区域大小为:(ex-sx+1)*(ey-sy+1)   
//color:要填充的颜色
void DeviceLCD_FillSingleColor(uint16_t sx,uint16_t sy,uint16_t ex,uint16_t ey,uint16_t color);

//在指定区域内填充指定颜色块			 
//(sx,sy),(ex,ey):填充矩形对角坐标,区域大小为:(ex-sx+1)*(ey-sy+1)   
//color:要填充的颜色
void DeviceLCD_FillColorBuffer(uint16_t sx,uint16_t sy,uint16_t ex,uint16_t ey,uint16_t *color);

//画线
//x1,y1:起点坐标
//x2,y2:终点坐标  
void DeviceLCD_DrawLine(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2,uint16_t color);

//画矩形	  
//(x1,y1),(x2,y2):矩形的对角坐标
void DeviceLCD_DrawRectangle(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2,uint16_t color);

//在指定位置显示一个字符
//x,y:起始坐标
//num:要显示的字符:" "--->"~",默认1206字体
void DeviceLCD_ShowChar(uint16_t x,uint16_t y,uint8_t num);

//显示数字,高位为0,则不显示
//x,y :起点坐标
//len :数字的位数
//num:数值(0~4294967295);
void DeviceLCD_ShowNum(uint16_t x,uint16_t y,uint32_t num,uint8_t len);

//显示字符串
//x,y:起点坐标
//*p:字符串起始地址		  
void DeviceLCD_ShowString(uint16_t x,uint16_t y,uint8_t *p);

#endif




