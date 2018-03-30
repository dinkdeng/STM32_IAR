#ifndef __DEVICE_ADXL345_H_
#define __DEVICE_ADXL345_H_
#include "stm32f10x.h"

/**定义是否使用错误信息打印 */
#define DEVICE_ADXL345_USE_PRINTF   0

/*设定计算基准轴的方向*/
typedef enum ADXL345_BASE_AXIS
{
    BASE_AXIS_Z = 0,
    BASE_AXIS_X = 1,
    BASE_AXIS_Y = 2,
}ADXL345_BASE_AXIS;

/*设备初始化*/
uint8_t DeviceAdxl345Init(void);

/*读取三轴数据*/
uint8_t DeviceAdxl345ReadXYZ(short *x, short *y, short *z,uint8_t times);

/*读取角度*/
uint8_t DeviceAdxl345ReadAngle(float x, float y, float z,ADXL345_BASE_AXIS baseAxis, short* angle);

/*自动校正*/
uint8_t DeviceAdxl345AutoAdjust(char *xval, char *yval, char *zval);


#endif





