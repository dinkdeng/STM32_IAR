#include "SystemUtil.h"

/*系统辅助函数定义*/

void SystemUtilSetIntGroup(void)
{
    /*系统NVIC中断分组初始化*/
    NVIC_PriorityGroupConfig(BOARD_DEFAULT_NVIC_GROUP);
}


