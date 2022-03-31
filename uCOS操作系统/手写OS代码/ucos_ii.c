#include "stdio.h"
#include "ucos_ii.h"
void main()
{
    OS_TCB       OSTCBTbl[OS_MAX_TASKS + OS_N_SYS_TASKS];		    /*任务控制块的结构体数组，实体化一共有多少任务控制块*/
    OS_TCB      *OSTCBFreeList;                                     /*指向空闲任务控制块链表*/
    OS_TCB      *OSTCBList;                       				    /*指向就绪任务控制块链表*/
    OS_TCB      *OSTCBPrioTbl[OS_LOWEST_PRIO + 1u];                 /*任务优先级指针表，用于获取某优先级的任务控制块地址*/
    OS_STK       Task_Stk[OS_MAX_TASKS][TASK_STK_SIZE];             /*任务堆栈*/
    OS_PRIO      OSRdyGrp;                        /*任务就绪组*/
    OS_PRIO      OSRdyTbl[OS_RDY_TBL_SIZE];       /*任务就绪表*/
    INT8U        OSTaskCtr;                       /*当前任务数*/


    INT8U  a;
    OS_STK b;
    OS_STK c;
    INT16U d;
    INT32U e;
    void*  f;
    INT16U g;
    //编不过
    OS_TCBInit(a,b,c,d,e,f,g);
    /*进度2.3*/
    printf("let's 挼 OS------\n");
}