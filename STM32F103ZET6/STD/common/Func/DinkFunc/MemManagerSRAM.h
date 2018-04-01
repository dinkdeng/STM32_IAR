#ifndef __MEM_MANAGER_SRAM_H_
#define __MEM_MANAGER_SRAM_H_
#include "stm32f10x.h"

//内存管理初始化函数
void MemManagerSRAMInit(void);	

//内存释放
void MemManagerSRAMFree(void *ptr); 

//内存分配
void *MemManagerSRAMMalloc(uint32_t size);

//重新分配内存
void *MemManagerSRAMRealloc(void *ptr,uint32_t size);

//设置内存
void MemManagerSRAMSet(void *s,uint8_t c,uint32_t count);

//复制内存  
void MemManagerSRAMCopy(void *des,void *src,uint32_t n);

//获得内存使用率
uint8_t MemManagerSRAMUsed(void);	



#endif




