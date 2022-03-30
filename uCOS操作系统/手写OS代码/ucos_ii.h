#include "os_cfg_r.h"
#define  OS_EVENT_EN           /*使用事件操作*/(((OS_Q_EN > 0u) && (OS_MAX_QS > 0u)) || (OS_MBOX_EN > 0u) || (OS_SEM_EN > 0u) || (OS_MUTEX_EN > 0u))

/*配置事件标志大小*/
#if OS_FLAGS_NBITS == 8u
typedef  INT8U    OS_FLAGS;
#endif
#if OS_FLAGS_NBITS == 16u
typedef  INT16U   OS_FLAGS;
#endif
#if OS_FLAGS_NBITS == 32u
typedef  INT32U   OS_FLAGS;
#endif

/*跟据任务数量配置，事件等待组和等待表的大小*/
#if OS_LOWEST_PRIO <= 63u
typedef  INT8U    OS_PRIO;
#else
typedef  INT16U   OS_PRIO;
#endif


/*根据有没有统计任务，配置系统任务数*/
#if OS_TASK_STAT_EN > 0u								/*有没有统计任务*/
#define  OS_N_SYS_TASKS                 2u              /*系统任务数*/
#else
#define  OS_N_SYS_TASKS                 1u				/*系统任务数*/
#endif

/*事件控制块ECB*/
typedef struct os_event
{
} OS_EVENT;

/*事件标志节点*/
typedef struct os_flag_node
{ 
} OS_FLAG_NODE;




/*任务控制块*/
typedef struct os_tcb 
{
    OS_STK          *OSTCBStkPtr;           /*任务堆栈指针，指向任务栈的栈顶指针*/	
#if OS_TASK_CREATE_EXT_EN > 0u				/*是否配置扩展功能*/
    void            *OSTCBExtPtr;           /*扩展块的指针*/
    OS_STK          *OSTCBStkBottom;        /*任务堆栈的栈底地址*/
    INT32U           OSTCBStkSize;          /*任务堆栈的大小*/
    INT16U           OSTCBOpt;              /*扩展选项*/
    INT16U           OSTCBId;               /*任务ID*/
#endif

    struct os_tcb   *OSTCBNext;             /*指向上一个控制块的指针*/
    struct os_tcb   *OSTCBPrev;             /*指向下一个控制块的指针*/

#if (OS_EVENT_EN)							/*如果使用消息队列，消息邮箱，或者信号量，那么任务就要用事件控制块，*/
    OS_EVENT        *OSTCBEventPtr;         /*OSTCBEventPtr是指向事件控制块的指针，*/
#endif
#if (OS_EVENT_EN) && (OS_EVENT_MULTI_EN > 0u)
    OS_EVENT       **OSTCBEventMultiPtr;    /*多事件控制指针*/
#endif
#if ((OS_Q_EN > 0u) && (OS_MAX_QS > 0u)) || (OS_MBOX_EN > 0u)
    void            *OSTCBMsg;              /*消息地址*/
#endif

#if (OS_FLAG_EN > 0u) && (OS_MAX_FLAGS > 0u)
#if OS_TASK_DEL_EN > 0u
    OS_FLAG_NODE    *OSTCBFlagNode;         /*事件标志节点*/
#endif
    OS_FLAGS         OSTCBFlagsRdy;         /*事件标志*/
#endif
    INT32U           OSTCBDly;              /*任务延时时间*/
    INT8U            OSTCBStat;             /*任务状态标志位    = 0x00就是就绪态*/
	/*7           6     5               4               3     2         1         0*/
	/*请求多事件，未用，请求事件标志组，请求互斥信号量，挂起，请求队列，请求邮箱，请求信号量*/
    INT8U            OSTCBStatPend;         /*等待状态*//*时间等待标志*/
    INT8U            OSTCBPrio;             /*任务优先级*/
	/*这四个都和就绪表相关，表示任务优先级*/
    INT8U            OSTCBX;				/*优先级在表中的第几列，任务优先级低3位*/
    INT8U            OSTCBY;				/*优先级在表中的第几行，任务优先级的高3位，任务优先级>>3，任务优先级/8*/                
    OS_PRIO          OSTCBBitX;				/*任务优先级在对应的任务就绪表中的位置*/ 
    OS_PRIO          OSTCBBitY;				/*任务在优先级组表中的位置*/

#if OS_TASK_DEL_EN > 0u
    INT8U            OSTCBDelReq;           /*任务删除请求标志*/
#endif

#if OS_TASK_PROFILE_EN > 0u
    INT32U           OSTCBCtxSwCtr;         /*切换到该任务的次数*/
    INT32U           OSTCBCyclesTot;        /*任务运行的总的时间周期*/
    INT32U           OSTCBCyclesStart;      /* Snapshot of cycle counter at start of task resumption   */
    OS_STK          *OSTCBStkBase;          /*任务堆栈的起始地址*/
    INT32U           OSTCBStkUsed;          /*任务堆栈中使用过的空间数*/ 
#endif

#if OS_TASK_NAME_EN > 0u
    INT8U           *OSTCBTaskName;         /*任务名称*/
#endif

#if OS_TASK_REG_TBL_SIZE > 0u
    INT32U           OSTCBRegTbl[OS_TASK_REG_TBL_SIZE];/*任务注册表*/
#endif
} OS_TCB;









