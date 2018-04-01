#include "MemManagerLocal.h"
#include "SystemUtil.h"

//内存块大小为32字节
#define MEM_MANAGER_LOCAL_BLOCK_SIZE			32
//最大管理内存 10K
#define MEM_MANAGER_LOCAL_MAX_SIZE			    10*1024
//内存表大小
#define MEM_MANAGER_LOCAL_ALLOC_TABLE_SIZE	    MEM_MANAGER_LOCAL_MAX_SIZE/MEM_MANAGER_LOCAL_BLOCK_SIZE

//内存池(32字节对齐)
#pragma pack(32)
static uint8_t memManagerLocalBuffer[MEM_MANAGER_LOCAL_MAX_SIZE];
#pragma pack()

//内存管理表,一个字节对应实际内存块的一块,表示该块内存是否已经被使用
static uint16_t memManagerLocalControlTable[MEM_MANAGER_LOCAL_ALLOC_TABLE_SIZE];

/**标识当前模块是否已经初始化 */
static uint8_t memManagerLocalReady = 0;

//复制内存
void MemManagerLocalCopy(void *des,void *src,uint32_t n)
{
    uint8_t *xdes=des;
	uint8_t *xsrc=src;
    while(n--)
        *xdes++=*xsrc++;
}

//设置内存
void MemManagerLocalSet(void *s,uint8_t c,uint32_t count)
{
    uint8_t *xs = s;
    while(count--)
        *xs++=c;
}



//内存管理初始化函数
void MemManagerLocalInit(void)
{
    /**内存池清零 */
    MemManagerLocalSet(memManagerLocalBuffer,0,MEM_MANAGER_LOCAL_MAX_SIZE);
    /**内存管理表清零 */
    MemManagerLocalSet(memManagerLocalControlTable,0,MEM_MANAGER_LOCAL_ALLOC_TABLE_SIZE*2);
    /**初始化标识为真 */
    memManagerLocalReady = 1;
}

//获得内存使用率
uint8_t MemManagerLocalUsed(void)
{
    uint32_t used = 0;
    uint32_t i = 0;
    for(i = 0; i < MEM_MANAGER_LOCAL_ALLOC_TABLE_SIZE; i++)
    {
        if(memManagerLocalControlTable[i] != 0)
            used++;
    }
    return (uint8_t)(100*(((double)used)/((double)(MEM_MANAGER_LOCAL_ALLOC_TABLE_SIZE))));
}

static uint32_t MemManagerLocalMallocUtil(uint32_t size)
{
    signed long offset=0;
    //需要的内存块数
    uint32_t nmemb = 0;
    //连续空内存块数
	uint32_t cmemb=0;
    uint32_t i = 0;
    //未初始化,先执行初始化
    if(!memManagerLocalReady)
        MemManagerLocalInit();
    //不需要分配
    if(size==0)
        return 0XFFFFFFFF;
    /**获取需要分配的连续内存块数*/
    nmemb=size/MEM_MANAGER_LOCAL_BLOCK_SIZE;
    /**不足一块的多分配一块 */
    if(size%MEM_MANAGER_LOCAL_BLOCK_SIZE)
        nmemb++;
    /**搜索整个内存控制区   */
    for(offset=MEM_MANAGER_LOCAL_ALLOC_TABLE_SIZE-1;offset>=0;offset--)
    {
        /**连续空内存块数增加*/
		if(!memManagerLocalControlTable[offset])
            cmemb++;
        //连续内存块清零
		else
            cmemb=0;
        //找到了连续nmemb个空内存块
		if(cmemb==nmemb)
		{
            //标注内存块非空
            for(i=0;i<nmemb;i++)
            {
                /**用此次分配的内存块数目来作为表示,这样释放的时候就可以直接释放指定数量的块数,加速目的 */
                memManagerLocalControlTable[offset+i]=nmemb;
            }
            //返回偏移地址
            return (offset*MEM_MANAGER_LOCAL_BLOCK_SIZE);
		}
    }
    //未找到符合分配条件的内存块
    return 0XFFFFFFFF;
}

//内存分配
void *MemManagerLocalMalloc(uint32_t size)
{
    uint32_t offset = 0;
	offset=MemManagerLocalMallocUtil(size);
    if(offset==0XFFFFFFFF)
	{
		return NULL;
    }
	else
	{
		return (void*)((uint32_t)memManagerLocalBuffer+offset);
	}
}

uint8_t MemManagerLocalFreeUtil(uint32_t offset)
{
    int i = 0;
    if(!memManagerLocalReady)
        MemManagerLocalInit();
    //偏移在内存池内.
    if(offset<MEM_MANAGER_LOCAL_MAX_SIZE)
    {
        //偏移所在内存块号码
        int index=offset/MEM_MANAGER_LOCAL_BLOCK_SIZE;
        //内存块数量
        int nmemb=memManagerLocalControlTable[index];
        //内存块清零
        for(i=0;i<nmemb;i++)
        {
            memManagerLocalControlTable[index+i]=0;
        }
        return 0;
    }
    //偏移超区了.
    else
        return 2;
}

//内存释放
void MemManagerLocalFree(void *ptr)
{
    uint32_t offset;
    /**计算偏移量 */
    offset=(uint32_t)ptr-(uint32_t)memManagerLocalBuffer;
    //释放内存
    MemManagerLocalFreeUtil(offset);
}



//重新分配内存
void *MemManagerLocalRealloc(void *ptr,uint32_t size)
{
    uint32_t offset;
    offset=MemManagerLocalMallocUtil(size);
    if(offset==0XFFFFFFFF)
	{
		return NULL;
	}
    else
    {
        //拷贝旧内存内容到新内存
	    MemManagerLocalCopy((void*)((uint32_t)memManagerLocalBuffer+offset),ptr,size);
        //释放旧内存
        MemManagerLocalFree(ptr);
        //返回新内存首地址
        return (void*)((uint32_t)memManagerLocalBuffer+offset);
    }
}





