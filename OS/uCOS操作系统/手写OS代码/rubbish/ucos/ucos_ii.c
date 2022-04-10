#include "stdio.h"
#include "ucos_ii.h"
    OS_TCB       OSTCBTbl[OS_MAX_TASKS + OS_N_SYS_TASKS];		    /*任务控制块的结构体数组，实体化一共有多少任务控制块 */
    OS_TCB*      OSTCBFreeList;                                     /*指向空闲任务控制块链表 */
    OS_TCB*      OSTCBList;                       				    /*指向就绪任务控制块链表 */
    OS_TCB*      OSTCBPrioTbl[OS_LOWEST_PRIO + 1u];                 /*任务优先级指针表，用于获取某优先级的任务控制块地址 */
    OS_STK       Task_Stk[OS_MAX_TASKS][TASK_STK_SIZE];             /*任务堆栈 */
    OS_PRIO      OSRdyGrp;                        /*任务就绪组 */
    OS_PRIO      OSRdyTbl[OS_RDY_TBL_SIZE];       /*任务就绪表 */
    INT8U             OSTaskCtr;                       /*当前任务数 */
    INT32U            OSCtxSwCtr;               /*任务切换次数 */
    INT8U             OSIntNesting;             /*中断嵌套计数 */
    INT8U             OSLockNesting;            /*调度锁计数 */
    INT8U             OSRunning;                /*是否启动多任务 */
    INT8U             OSTaskCtr;                /*当前任务数 */
    INT32U            OSIdleCtr;                /*空闲计数器，在空闲任务中一直++ */
    INT32U            OSIdleCtrMax;             /*最大空闲计数值 */					/* Max. value that idle ctr can take in 1 sec.     */
    INT32U            OSIdleCtrRun;             /*1秒内空闲计数值 */					/* Val. reached by idle ctr at run time in 1 sec.  */
    INT8U             OSStatRdy;                /*统计任务准备状态 */				/* Flag indicating that the statistic task is rdy  */
    INT8U             OSPrioCur;                /*当前任务优先级 */					/* Priority of current task                        */
    INT8U             OSPrioHighRdy;            /*运行任务的最高优先级 */			/* Priority of highest priority task               */
    OS_TCB*           OSTCBCur;                 /*当前运行的任务控制块的指针 */
    OS_TCB*           OSTCBHighRdy;             /*最高优先级的任务控制块的指针 */
#if (OS_EVENT_EN) && (OS_MAX_EVENTS > 0u)
    OS_EVENT*         OSEventFreeList;          /*事件空闲链表指针 *//* Pointer to list of free EVENT control blocks    */
    OS_EVENT          OSEventTbl[OS_MAX_EVENTS];/*事件控制块的实体 *//* Table of EVENT control blocks                   */
#endif
#if OS_TASK_STAT_EN > 0u
    INT8U             OSCPUUsage;               /*CPU使用率 */						/* Percentage of CPU used                          */
    INT32U            OSIdleCtrMax;             /*最大空闲计数值 */					/* Max. value that idle ctr can take in 1 sec.     */
    INT32U            OSIdleCtrRun;             /*1秒内空闲计数值 */					/* Val. reached by idle ctr at run time in 1 sec.  */
    INT8U             OSStatRdy;                /*统计任务准备状态 */				/* Flag indicating that the statistic task is rdy  */
    OS_STK            OSTaskStatStk[OS_TASK_STAT_STK_SIZE];      /*统计任务栈 */		/* Statistics task stack          */
#endif
    INT8U             FlagEn;                   /*是否在临界区 */



void main()
{



    INT8U  a = 1;
    OS_STK b = 1;
    OS_STK c = 1;
    INT16U d = 1;
    INT32U e = 1;
    void* f = NULL;
    INT16U g = 1;
    OS_TCBInit(a,b,c,d,e,f,g);
    OSInit();
    /*进度       程序4.3.8*/
    printf("let's 挼 OS------\n");
}










// 运行程序: Ctrl + F5 或调试 >“开始执行(不调试)”菜单
// 调试程序: F5 或调试 >“开始调试”菜单

// 入门使用技巧: 
//   1. 使用解决方案资源管理器窗口添加/管理文件
//   2. 使用团队资源管理器窗口连接到源代码管理
//   3. 使用输出窗口查看生成输出和其他消息
//   4. 使用错误列表窗口查看错误
//   5. 转到“项目”>“添加新项”以创建新的代码文件，或转到“项目”>“添加现有项”以将现有代码文件添加到项目
//   6. 将来，若要再次打开此项目，请转到“文件”>“打开”>“项目”并选择 .sln 文件
