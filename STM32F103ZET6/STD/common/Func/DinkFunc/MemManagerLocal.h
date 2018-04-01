#ifndef __MEM_MANAGER_LOCAL_H_
#define __MEM_MANAGER_LOCAL_H_
#include "stm32f10x.h"

//内存管理初始化函数
void MemManagerLocalInit(void);	

//内存释放
void MemManagerLocalFree(void *ptr); 

//内存分配
void *MemManagerLocalMalloc(uint32_t size);

//重新分配内存
void *MemManagerLocalRealloc(void *ptr,uint32_t size);

//设置内存
void MemManagerLocalSet(void *s,uint8_t c,uint32_t count);

//复制内存  
void MemManagerLocalCopy(void *des,void *src,uint32_t n);

//获得内存使用率
uint8_t MemManagerLocalUsed(void);	


#endif




