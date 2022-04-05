#include "os_cfg_r.h"


#define OS_ERR_NONE                     0u
#define OS_ERR_EVENT_TYPE               1u
#define OS_ERR_PEND_ISR                 2u/*中断中调用本函数 */
#define OS_ERR_POST_NULL_PTR            3u
#define OS_ERR_PEVENT_NULL              4u
#define OS_ERR_POST_ISR                 5u
#define OS_ERR_QUERY_ISR                6u
#define OS_ERR_INVALID_OPT              7u
#define OS_ERR_ID_INVALID               8u
#define OS_ERR_PDATA_NULL               9u

#define OS_ERR_TIMEOUT                 10u
#define OS_ERR_EVENT_NAME_TOO_LONG     11u
#define OS_ERR_PNAME_NULL              12u
#define OS_ERR_PEND_LOCKED             13u
#define OS_ERR_PEND_ABORT              14u
#define OS_ERR_DEL_ISR                 15u
#define OS_ERR_CREATE_ISR              16u
#define OS_ERR_NAME_GET_ISR            17u
#define OS_ERR_NAME_SET_ISR            18u
#define OS_ERR_ILLEGAL_CREATE_RUN_TIME 19u

#define OS_ERR_MBOX_FULL               20u

#define OS_ERR_Q_FULL                  30u
#define OS_ERR_Q_EMPTY                 31u

#define OS_ERR_PRIO_EXIST              40u
#define OS_ERR_PRIO                    41u
#define OS_ERR_PRIO_INVALID            42u

#define OS_ERR_SCHED_LOCKED            50u
#define OS_ERR_SEM_OVF                 51u

#define OS_ERR_TASK_CREATE_ISR         60u
#define OS_ERR_TASK_DEL                61u
#define OS_ERR_TASK_DEL_IDLE           62u
#define OS_ERR_TASK_DEL_REQ            63u
#define OS_ERR_TASK_DEL_ISR            64u
#define OS_ERR_TASK_NAME_TOO_LONG      65u
#define OS_ERR_TASK_NO_MORE_TCB        66u/*没有空闲的任务控制块 */
#define OS_ERR_TASK_NOT_EXIST          67u
#define OS_ERR_TASK_NOT_SUSPENDED      68u
#define OS_ERR_TASK_OPT                69u
#define OS_ERR_TASK_RESUME_PRIO        70u
#define OS_ERR_TASK_SUSPEND_IDLE       71u/*试图阻塞空闲任务 */
#define OS_ERR_TASK_SUSPEND_PRIO       72u/*阻塞任务不存在 */
#define OS_ERR_TASK_WAITING            73u

#define OS_ERR_TIME_NOT_DLY            80u
#define OS_ERR_TIME_INVALID_MINUTES    81u
#define OS_ERR_TIME_INVALID_SECONDS    82u
#define OS_ERR_TIME_INVALID_MS         83u
#define OS_ERR_TIME_ZERO_DLY           84u
#define OS_ERR_TIME_DLY_ISR            85u

#define OS_ERR_MEM_INVALID_PART        90u
#define OS_ERR_MEM_INVALID_BLKS        91u
#define OS_ERR_MEM_INVALID_SIZE        92u
#define OS_ERR_MEM_NO_FREE_BLKS        93u
#define OS_ERR_MEM_FULL                94u
#define OS_ERR_MEM_INVALID_PBLK        95u
#define OS_ERR_MEM_INVALID_PMEM        96u
#define OS_ERR_MEM_INVALID_PDATA       97u
#define OS_ERR_MEM_INVALID_ADDR        98u
#define OS_ERR_MEM_NAME_TOO_LONG       99u

#define OS_ERR_NOT_MUTEX_OWNER        100u

#define OS_ERR_FLAG_INVALID_PGRP      110u
#define OS_ERR_FLAG_WAIT_TYPE         111u
#define OS_ERR_FLAG_NOT_RDY           112u
#define OS_ERR_FLAG_INVALID_OPT       113u
#define OS_ERR_FLAG_GRP_DEPLETED      114u
#define OS_ERR_FLAG_NAME_TOO_LONG     115u

#define OS_ERR_PIP_LOWER              120u

#define OS_ERR_TMR_INVALID_DLY        130u
#define OS_ERR_TMR_INVALID_PERIOD     131u
#define OS_ERR_TMR_INVALID_OPT        132u
#define OS_ERR_TMR_INVALID_NAME       133u
#define OS_ERR_TMR_NON_AVAIL          134u
#define OS_ERR_TMR_INACTIVE           135u
#define OS_ERR_TMR_INVALID_DEST       136u
#define OS_ERR_TMR_INVALID_TYPE       137u
#define OS_ERR_TMR_INVALID            138u
#define OS_ERR_TMR_ISR                139u
#define OS_ERR_TMR_NAME_TOO_LONG      140u
#define OS_ERR_TMR_INVALID_STATE      141u
#define OS_ERR_TMR_STOPPED            142u
#define OS_ERR_TMR_NO_CALLBACK        143u

#define  OS_FALSE                       0u
#define  OS_TRUE                        1u

#define  OS_EVENT_EN           /*使用事件操作 */(((OS_Q_EN > 0u) && (OS_MAX_QS > 0u)) || (OS_MBOX_EN > 0u) || (OS_SEM_EN > 0u) || (OS_MUTEX_EN > 0u))

#define  OS_TCB_RESERVED        ((OS_TCB *)1)

/*配置事件标志大小 */
#if OS_FLAGS_NBITS == 8u
typedef  INT8U    OS_FLAGS;
#endif
#if OS_FLAGS_NBITS == 16u
typedef  INT16U   OS_FLAGS;
#endif
#if OS_FLAGS_NBITS == 32u
typedef  INT32U   OS_FLAGS;
#endif

/*跟据任务数量配置，事件等待组和等待表的大小 */
#if OS_LOWEST_PRIO <= 63u
typedef  INT8U    OS_PRIO;
#else
typedef  INT16U   OS_PRIO;
#endif


/*根据任务数量配置事件等待组和表的类型 */
#if OS_LOWEST_PRIO <= 63u
#define  OS_EVENT_TBL_SIZE ((OS_LOWEST_PRIO) / 8u + 1u) /* Size of event table                         */
#define  OS_RDY_TBL_SIZE   ((OS_LOWEST_PRIO) / 8u + 1u) /* Size of ready table                         */
#else
#define  OS_EVENT_TBL_SIZE ((OS_LOWEST_PRIO) / 16u + 1u)/* Size of event table                         */
#define  OS_RDY_TBL_SIZE   ((OS_LOWEST_PRIO) / 16u + 1u)/* Size of ready table                         */
#endif


/*根据有没有统计任务，配置系统任务数 */
#if OS_TASK_STAT_EN > 0u								/*有没有统计任务 */
#define  OS_N_SYS_TASKS                 2u              /*系统任务数 */
#else
#define  OS_N_SYS_TASKS                 1u				/*系统任务数 */
#endif




/*TASK的状态 */
#define  OS_STAT_RDY                 0x00u  /*就绪态，任务未等待事件且未挂起 */
#define  OS_STAT_SEM                 0x01u  /*，任务等待信号量 */														/* Pending on semaphore                                    */
#define  OS_STAT_MBOX                0x02u  /*，任务等待邮箱 */															/* Pending on mailbox                                      */
#define  OS_STAT_Q                   0x04u  /*，任务等待消息队列 */														/* Pending on queue                                        */
#define  OS_STAT_SUSPEND             0x08u  /*，任务挂起 */																/* Task is suspended                                       */
#define  OS_STAT_MUTEX               0x10u  /*，任务等待互斥信号量 */													/* Pending on mutual exclusion semaphore                   */
#define  OS_STAT_FLAG                0x20u  /*，任务等待事件标志 */														/* Pending on event flag group                             */
#define  OS_STAT_MULTI               0x80u  /*，等待多事件 */																/* Pending on multiple events                              */

#define  OS_STAT_PEND_ANY                   /*等待时间结合体 */(OS_STAT_SEM | OS_STAT_MBOX | OS_STAT_Q | OS_STAT_MUTEX | OS_STAT_FLAG)

/*Task Pend状态 */
#define  OS_STAT_PEND_OK                0u/*事件发生了 */
#define  OS_STAT_PEND_TO                1u/*事件等待失败了 */
#define  OS_STAT_PEND_ABORT             2u/*事件超时了 */

/* 事件类型 */
#define  OS_EVENT_TYPE_UNUSED           0u/*未使用 */
#define  OS_EVENT_TYPE_MBOX             1u/*消息邮箱 */
#define  OS_EVENT_TYPE_Q                2u/*消息队列 */
#define  OS_EVENT_TYPE_SEM              3u/*信号量 */
#define  OS_EVENT_TYPE_MUTEX            4u/*互斥信号量 */
#define  OS_EVENT_TYPE_FLAG             5u/*事件标志组被使用 */

#define  OS_TASK_OPT_NONE          0x0000u  /*啥也没选 */
#define  OS_TASK_OPT_STK_CHK       0x0001u  /*进行堆栈检查 */
#define  OS_TASK_OPT_STK_CLR       0x0002u  /*创建任务时清空堆栈 */
#define  OS_TASK_OPT_SAVE_FP       0x0004u  /*保存浮点寄存器内容 */

#define  OS_TASK_STAT_PRIO  (OS_LOWEST_PRIO - 1u)       /* Statistic task priority                     */
#define  OS_TASK_IDLE_PRIO  (OS_LOWEST_PRIO)            /*空闲任务的优先级 */
#define  OS_PRIO_SELF                0xFFu              /*删除任务优先级置FF */


/*事件控制块ECB */
#if (OS_EVENT_EN) && (OS_MAX_EVENTS > 0u)
typedef struct os_event
{
    INT8U    OSEventType;							/*事件控制块的类型 */
    void*    OSEventPtr;							/*指向下一个ECB */
    INT16U   OSEventCnt;							/*信号量计数值，互斥信号量中高八位表示优先级第八位表示有没有人使用 */
    OS_PRIO  OSEventGrp;							/*事件等待组 */
    OS_PRIO  OSEventTbl[OS_EVENT_TBL_SIZE];			/*等待事件的任务表 */
                                                    /*事件等待表和事件等待组和任务的类似，但是任务的是独立的，事件的在控制块里 */
#if OS_EVENT_NAME_EN > 0u
    INT8U* OSEventName;							    /*事件名称 */
#endif
} OS_EVENT;
#endif


/*事件标志节点 */
typedef struct os_flag_node
{
	int a;
} OS_FLAG_NODE;


/*任务控制块 */
typedef struct os_tcb
{
	OS_STK* OSTCBStkPtr;           /*任务堆栈指针，指向任务栈的栈顶指针 */
#if OS_TASK_CREATE_EXT_EN > 0u				/*是否配置扩展功能 */
	void* OSTCBExtPtr;           /*扩展块的指针 */
	OS_STK* OSTCBStkBottom;        /*任务堆栈的栈底地址 */
	INT32U           OSTCBStkSize;          /*任务堆栈的大小 */
	INT16U           OSTCBOpt;              /*扩展选项 */
	INT16U           OSTCBId;               /*任务ID */
#endif

	struct os_tcb* OSTCBNext;             /*指向上一个控制块的指针 */
	struct os_tcb* OSTCBPrev;             /*指向下一个控制块的指针 */
#if (OS_EVENT_EN)							/*如果使用消息队列，消息邮箱，或者信号量，那么任务就要用事件控制块，*/
	OS_EVENT* OSTCBEventPtr;         /*OSTCBEventPtr是指向事件控制块的指针，*/
#endif
#if (OS_EVENT_EN) && (OS_EVENT_MULTI_EN > 0u)
	OS_EVENT** OSTCBEventMultiPtr;    /*多事件控制指针 */
#endif
#if ((OS_Q_EN > 0u) && (OS_MAX_QS > 0u)) || (OS_MBOX_EN > 0u)
	void* OSTCBMsg;              /*消息地址 */
#endif

#if (OS_FLAG_EN > 0u) && (OS_MAX_FLAGS > 0u)
#if OS_TASK_DEL_EN > 0u
    OS_FLAG_NODE* OSTCBFlagNode;         /*事件标志节点 */
#endif
    OS_FLAGS         OSTCBFlagsRdy;         /*事件标志 */
#endif
    INT32U           OSTCBDly;              /*任务延时时间 */
    INT8U            OSTCBStat;             /*任务状态标志位    = 0x00就是就绪态*/
    /*7           6     5               4               3     2         1         0*/
    /*请求多事件，未用，请求事件标志组，请求互斥信号量，挂起，请求队列，请求邮箱，请求信号量 */
    INT8U            OSTCBStatPend;         /*等待状态 *//*时间等待标志 */
    INT8U            OSTCBPrio;             /*任务优先级 */
    /*这四个都和就绪表相关，表示任务优先级 */
    INT8U            OSTCBX;				/*优先级在表中的第几列，任务优先级低3位 */
    INT8U            OSTCBY;				/*优先级在表中的第几行，任务优先级的高3位，任务优先级>>3，任务优先级/8 */
    OS_PRIO          OSTCBBitX;				/*任务优先级在对应的任务就绪表中的位置 */
    OS_PRIO          OSTCBBitY;				/*任务在优先级组表中的位置 */

#if OS_TASK_DEL_EN > 0u
    INT8U            OSTCBDelReq;           /*任务删除请求标志 */
#endif

#if OS_TASK_PROFILE_EN > 0u
    INT32U           OSTCBCtxSwCtr;         /*切换到该任务的次数 */
    INT32U           OSTCBCyclesTot;        /*任务运行的总的时间周期 */
    INT32U           OSTCBCyclesStart;      /* Snapshot of cycle counter at start of task resumption   */
    OS_STK* OSTCBStkBase;          /*任务堆栈的起始地址 */
    INT32U           OSTCBStkUsed;          /*任务堆栈中使用过的空间数 */
#endif

#if OS_TASK_NAME_EN > 0u
    INT8U* OSTCBTaskName;         /*任务名称 */
#endif

#if OS_TASK_REG_TBL_SIZE > 0u
    INT32U           OSTCBRegTbl[OS_TASK_REG_TBL_SIZE];/*任务注册表 */
#endif
}OS_TCB;




extern  OS_TCB       OSTCBTbl[OS_MAX_TASKS + OS_N_SYS_TASKS];		    /*任务控制块的结构体数组，实体化一共有多少任务控制块 */

extern  OS_TCB*      OSTCBFreeList;                  				    /*指向空闲任务控制块链表 */
extern  OS_TCB*      OSTCBList;                       			    	/*指向就绪任务控制块链表 */
extern  OS_TCB*      OSTCBPrioTbl[OS_LOWEST_PRIO + 1u];    		        /*任务优先级指针表，用于获取某优先级的任务控制块地址 */

extern  OS_STK            Task_Stk[OS_MAX_TASKS][TASK_STK_SIZE];        /* 任务堆栈 */
extern  OS_PRIO           OSRdyGrp;                        /*任务就绪组 */
extern  OS_PRIO           OSRdyTbl[OS_RDY_TBL_SIZE];       /*任务就绪表 */


extern  INT8U   const     OSUnMapTbl[256];                 /*优先级判定表 */
extern  INT8U             OSTaskCtr;                       /*当前任务数 */
#if OS_TIME_GET_SET_EN > 0u
extern  INT32U            OSTime;                   /*当前系统时间，调度计数器 *//* Current value of system time (in ticks)         */
#endif
extern  INT32U            OSCtxSwCtr;               /*任务切换次数 */
extern  INT8U             OSIntNesting;             /*中断嵌套计数 */
extern  INT8U             OSLockNesting;            /*调度锁计数 */
extern  INT8U             OSRunning;                /*是否启动多任务 */
extern  INT8U             OSTaskCtr;                /*当前任务数 */
extern  INT32U            OSIdleCtr;                /*空闲计数器，在空闲任务中一直++ */

extern  INT32U            OSIdleCtrMax;             /*最大空闲计数值 */					/* Max. value that idle ctr can take in 1 sec.     */
extern  INT32U            OSIdleCtrRun;             /*1秒内空闲计数值 */					/* Val. reached by idle ctr at run time in 1 sec.  */
extern  INT8U             OSStatRdy;                  /*统计任务准备状态 */				/* Flag indicating that the statistic task is rdy  */
extern  INT8U             OSPrioCur;                /*当前任务优先级 */					/* Priority of current task                        */
extern  INT8U             OSPrioHighRdy;            /*运行任务的最高优先级 */			/* Priority of highest priority task               */
extern  OS_TCB*           OSTCBCur;                 /*当前运行的任务控制块的指针 */
extern  OS_TCB*           OSTCBHighRdy;             /*最高优先级的任务控制块的指针 */
#if (OS_EVENT_EN) && (OS_MAX_EVENTS > 0u)
extern OS_EVENT*          OSEventFreeList;          /*事件空闲链表指针 *//* Pointer to list of free EVENT control blocks    */
extern OS_EVENT           OSEventTbl[OS_MAX_EVENTS];/*事件控制块的实体 *//* Table of EVENT control blocks                   */
#endif







extern  void          OS_EXIT_CRITICAL();
extern  void          OSTCBInitHook(OS_TCB* ptcb);      /*空函数 */
extern  void          OSTaskCreateHook(OS_TCB* ptcb);   /*创建钩子函数 */
extern  void          OS_ENTER_CRITICAL();
extern  void          OSTCBInitHook(OS_TCB* ptcb);      /*空函数 */
extern  void          OSTaskCreateHook(OS_TCB* ptcb);   /*创建钩子函数 */
/*任务控制块初始化 */
extern  INT8U  OS_TCBInit(INT8U    prio,		        /*被创建任务的优先级 */
                          OS_STK*  ptos,		        /*任务堆栈的栈顶的地址 */
                          OS_STK*  pbos,		        /*任务堆栈发栈底的地址 */
                          INT16U   id,			    /*任务ID */
                          INT32U   stk_size,	        /*任务栈大小 */
                          void*    pext,		            /*任务控制块的扩展的地址 */
                          INT16U   opt);		        /*其他选项 */

extern void          OSInit(void);             /*OS初始化 */
extern void          OSInitHookBegin(void);    /*用户可编写该函数，实现端口说明 */
extern void          OS_InitMisc(void);        /*初始化各种全局变量 */
extern void          OS_InitRdyList(void);     /*初始化就绪表 */
extern void          OS_InitTCBList(void);     /*初始化任务控制块，空闲链表 */
extern void          OS_InitEventList(void);   /*初始化空闲事件列表 */
extern void          OS_FlagInit();            /*对事件标志进行初始化 */
extern void          OS_MemInit();             /*初始化内存 */
extern void          OS_QInit();               /*初始化消息队列 */
extern void          OS_InitTaskIdle();        /*创建空闲任务 */
extern void          OS_InitTaskStat();        /*创建统计任务 */
extern void          OSTmr_Init();             /*初始化定时器管理模块 */
extern void          OSDebugInit();			   /*加入调试代码 */
extern void          OSInitHookEnd();          /*用户可继续编写该函数，实现端口说明 */

extern void          OS_MemClr(INT8U* pdest, INT16U  size);          /*复制函数*/

/*没扩展功能创建任务 */
extern INT8U         OSTaskCreate(void   (*task)(void* p_arg),		/*任务代码地址 */
                                  void* p_arg,				    	/*任务参数 */
                                  OS_STK* ptos,						/*任务栈顶 */
                                  INT8U    prio);				    /*任务的优先级 */
extern INT8U  OSTaskCreateExt(void   (*task)(void* p_arg),	        /*任务代码地址 */
    void* p_arg,					/*任务参数 */
    OS_STK* ptos,					/*任务栈的栈顶 */
    INT8U    prio,					/*任务优先级 */
    INT16U   id,					/*任务ID */
    OS_STK* pbos,					/*任务栈栈底指针 */
    INT32U   stk_size,				/*任务栈大小 */
    void* pext,					    /*扩展的地址 */
    INT16U   opt);					/*任务的附加信息 */

/*自己加的 */
/*初始化任务栈 */
extern OS_STK* OSTaskStkInit(void           (*task)(void* p_arg),   /*任务代码的地址 */
                                             void* pdata,			/*任务参数 */
                                             OS_STK* ptos,		    /*任务栈的栈顶指针 */
                                             INT16U    opt);		/*栈的初始化选项 */
extern void         OS_Sched();                                      /*调度 */
/*堆栈清空函数 */
#if (OS_TASK_STAT_STK_CHK_EN > 0u) && (OS_TASK_CREATE_EXT_EN > 0u)
extern void  OS_TaskStkClr(OS_STK* pbos,
    INT32U   size,
    INT16U   opt);
#endif

/*在事件等待组和表中删除该任务 */
/*取消等待事件的表和组 */
#if (OS_EVENT_EN)
extern void  OS_EventTaskRemove(OS_TCB* ptcb,
    OS_EVENT* pevent);
#endif


#if ((OS_EVENT_EN) && (OS_EVENT_MULTI_EN > 0u))
extern void  OS_EventTaskRemoveMulti(OS_TCB* ptcb,
    OS_EVENT** pevents_multi);
#endif
extern void  OS_FlagUnlink(OS_FLAG_NODE* pnode);
extern void OS_Dummy();/*空函数 */
extern void OSTaskDelHook(ptcb);              /*删除钩子函数 */
#if OS_TASK_DEL_EN > 0u
extern INT8U  OSTaskDelReq(INT8U prio);       /*请求删除任务 */
#endif
#if OS_TASK_SUSPEND_EN > 0u
extern INT8U  OSTaskSuspend(INT8U prio);      /*挂起任务 */
#endif
#if OS_TASK_SUSPEND_EN > 0u
extern INT8U  OSTaskResume(INT8U prio);       /*恢复挂起任务 */
#endif
extern void  OSTimeTick(void);               /*时间片调度函数 */
extern void  OSTimeTickHook();               /*用户的钩子函数 */