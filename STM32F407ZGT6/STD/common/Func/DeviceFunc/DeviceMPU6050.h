#ifndef __DEVICE_MPU6050_H_
#define __DEVICE_MPU6050_H_
#include "stm32f4xx.h"


#define DEVICE_MPU6050_WAIT_MS(nms)          CoreTickDelayMs(nms)

//得到温度值
//得到的温度是实际温度*100
uint8_t DeviceMPU6050_GetTemperature(short* temp);

//得到陀螺仪值(原始值)
uint8_t DeviceMPU6050_GetGyroscope(short *gx,short *gy,short *gz);

//得到加速度值(原始值)
uint8_t DeviceMPU6050_GetAccelerometer(short *ax,short *ay,short *az);

//初始化MPU6050
uint8_t DeviceMPU6050_Init(void);


//提供给EMPL的库
//IIC写一个字节
uint8_t DeviceMPU6050_WriteByte(uint8_t reg,uint8_t data);	
//IIC读一个字节
uint8_t DeviceMPU6050_ReadByte(uint8_t reg,uint8_t* readData);
//IIC连续写
uint8_t DeviceMPU6050_WriteLen(uint8_t addr,uint8_t reg,uint8_t len,uint8_t *buf);
//IIC连续读
uint8_t DeviceMPU6050_ReadLen(uint8_t addr,uint8_t reg,uint8_t len,uint8_t *buf);



#endif





