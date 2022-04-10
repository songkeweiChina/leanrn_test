#include "ucos_ii.h"
/*优先级判断表 */
INT8U  const  OSUnMapTbl[256] =
{
    0u, 0u, 1u, 0u, 2u, 0u, 1u, 0u, 3u, 0u, 1u, 0u, 2u, 0u, 1u, 0u, /* 0x00 to 0x0F                   */
    4u, 0u, 1u, 0u, 2u, 0u, 1u, 0u, 3u, 0u, 1u, 0u, 2u, 0u, 1u, 0u, /* 0x10 to 0x1F                   */
    5u, 0u, 1u, 0u, 2u, 0u, 1u, 0u, 3u, 0u, 1u, 0u, 2u, 0u, 1u, 0u, /* 0x20 to 0x2F                   */
    4u, 0u, 1u, 0u, 2u, 0u, 1u, 0u, 3u, 0u, 1u, 0u, 2u, 0u, 1u, 0u, /* 0x30 to 0x3F                   */
    6u, 0u, 1u, 0u, 2u, 0u, 1u, 0u, 3u, 0u, 1u, 0u, 2u, 0u, 1u, 0u, /* 0x40 to 0x4F                   */
    4u, 0u, 1u, 0u, 2u, 0u, 1u, 0u, 3u, 0u, 1u, 0u, 2u, 0u, 1u, 0u, /* 0x50 to 0x5F                   */
    5u, 0u, 1u, 0u, 2u, 0u, 1u, 0u, 3u, 0u, 1u, 0u, 2u, 0u, 1u, 0u, /* 0x60 to 0x6F                   */
    4u, 0u, 1u, 0u, 2u, 0u, 1u, 0u, 3u, 0u, 1u, 0u, 2u, 0u, 1u, 0u, /* 0x70 to 0x7F                   */
    7u, 0u, 1u, 0u, 2u, 0u, 1u, 0u, 3u, 0u, 1u, 0u, 2u, 0u, 1u, 0u, /* 0x80 to 0x8F                   */
    4u, 0u, 1u, 0u, 2u, 0u, 1u, 0u, 3u, 0u, 1u, 0u, 2u, 0u, 1u, 0u, /* 0x90 to 0x9F                   */
    5u, 0u, 1u, 0u, 2u, 0u, 1u, 0u, 3u, 0u, 1u, 0u, 2u, 0u, 1u, 0u, /* 0xA0 to 0xAF                   */
    4u, 0u, 1u, 0u, 2u, 0u, 1u, 0u, 3u, 0u, 1u, 0u, 2u, 0u, 1u, 0u, /* 0xB0 to 0xBF                   */
    6u, 0u, 1u, 0u, 2u, 0u, 1u, 0u, 3u, 0u, 1u, 0u, 2u, 0u, 1u, 0u, /* 0xC0 to 0xCF                   */
    4u, 0u, 1u, 0u, 2u, 0u, 1u, 0u, 3u, 0u, 1u, 0u, 2u, 0u, 1u, 0u, /* 0xD0 to 0xDF                   */
    5u, 0u, 1u, 0u, 2u, 0u, 1u, 0u, 3u, 0u, 1u, 0u, 2u, 0u, 1u, 0u, /* 0xE0 to 0xEF                   */
    4u, 0u, 1u, 0u, 2u, 0u, 1u, 0u, 3u, 0u, 1u, 0u, 2u, 0u, 1u, 0u  /* 0xF0 to 0xFF                   */
};


void          OS_EXIT_CRITICAL();
void          OS_ENTER_CRITICAL();
void          OSTCBInitHook(OS_TCB* ptcb);/*空函数 */
void          OSTaskCreateHook(OS_TCB          *ptcb);/*创建钩子函数 */
/*任务控制块初始化 */
INT8U  OS_TCBInit (INT8U    prio,		    /*被创建任务的优先级 */
                    OS_STK  *ptos,		    /*任务堆栈的栈顶的地址 */
                    OS_STK  *pbos,		    /*任务堆栈发栈底的地址 */
                    INT16U   id,			    /*任务ID */
                    INT32U   stk_size,	    /*任务栈大小 */
                    void    *pext,		    /*任务控制块的扩展的地址 */
                    INT16U   opt);		    /*其他选项 */

void          OSInit(void);             /*OS初始化 */
void          OSInitHookBegin(void);    /*用户可编写该函数，实现端口说明 */
void          OS_InitMisc(void);        /*初始化各种全局变量 */
void          OS_InitRdyList(void);     /*初始化就绪表 */
void          OS_InitTCBList(void);     /*初始化任务控制块，空闲链表 */
void          OS_InitEventList(void);   /*初始化空闲事件列表 */
void          OS_FlagInit();                                               /*对事件标志进行初始化 */
void          OS_MemInit();                                                /*初始化内存 */
void          OS_QInit();                                                  /*初始化消息队列 */
void          OS_InitTaskIdle();                                           /*创建空闲任务 */
void          OS_InitTaskStat();                                           /*创建统计任务 */
void          OSTmr_Init();                                                /*初始化定时器管理模块 */
void          OSDebugInit();											   /*加入调试代码 */
void          OSInitHookEnd();                                             /*用户可继续编写该函数，实现端口说明 */
void          OS_MemClr(INT8U* pdest, INT16U  size);      /*复制函数 */
/*自己加的 */
/*初始化任务栈 */
OS_STK*       OSTaskStkInit(void           (*task)(void* p_arg),   /*任务代码的地址 */
    void* pdata,			/*任务参数 */
    OS_STK* ptos,		    /*任务栈的栈顶指针 */
    INT16U    opt);		/*栈的初始化选项 */
void          OS_Sched();        /*调度 */

/*在事件等待组和表中删除该任务 */
/*取消等待事件的表和组 */
#if (OS_EVENT_EN)
void  OS_EventTaskRemove(OS_TCB* ptcb,
    OS_EVENT* pevent);
#endif

#if ((OS_EVENT_EN) && (OS_EVENT_MULTI_EN > 0u))
void  OS_EventTaskRemoveMulti(OS_TCB* ptcb,
    OS_EVENT** pevents_multi);
#endif
void  OSTimeTick(void);               /*时间片调度函数 */
void  OSTimeTickHook();               /*用户的钩子函数 */
void  OS_SchedNew(void);              /*用来确定最高优先级的就绪任务 */
void OS_TASK_SW();       /*进行任务切换 */
void  OSIntExit(void);/*时钟中断中任务切换 */
void OSIntCtxSw(void);
void  OSStart(void);/*启动多任务 */
void OSStartHighRdy(void);/*启动多任务 */
void OSTaskSwHook(void);/*钩子函数 */
void  OS_TaskIdle(void* p_arg); /*空闲任务 */
void  OSTaskIdleHook(void);/*钩子函数 */
#if OS_TASK_STAT_EN > 0u
void  OSStatInit(void); /*统计任务的初始化 */
#endif
#if OS_TASK_STAT_EN > 0u
void          OS_TaskStat(void* p_arg);			/*统计任务代码*/
#endif
void OSTaskStatHook();/*钩子函数 */
void OSIntEnter(void);/*进入中断 */
void OSTickISRuser();  /*时钟中断服务函数*/
#if (OS_EVENT_EN)
void  OS_EventWaitListInit(OS_EVENT* pevent); /*事件等待表初始化，也就是清一下ECB中的等待组+表 */
#endif
#if (OS_EVENT_EN)
void  OS_EventTaskWait(OS_EVENT* pevent); /*设置事件等待函数，将任务在ECB中登记的函数，把任务在就绪组表中取消 */
#endif

#if (OS_EVENT_EN)
/*将等待事件的任务就绪 */
INT8U  OS_EventTaskRdy(OS_EVENT* pevent,				/*ECB的指针 */
    void* pmsg,				/*消息指针 */
    INT8U      msk,					/*清除状态位的掩码 */
    INT8U      pend_stat);			/*等待(pend)结束 */
#endif
OS_EVENT* OSSemCreate(INT16U cnt); /*创建一个信号量 */
 /*删除一个信号量/消息 */
#if OS_SEM_DEL_EN > 0u
OS_EVENT* OSSemDel(OS_EVENT* pevent,						/*ECB地址 */
    INT8U      opt,						/*删除选项 */
    INT8U* perr);						/*返回值 */
#endif
 /*请求一个信号量，等待一个信号量，也就是任务申请一把钥匙 */
void  OSSemPend(OS_EVENT* pevent,						/*ECB地址 */
    INT32U     timeout,					/*设定的超时时间 */
    INT8U* perr);
INT8U  OSSemPost(OS_EVENT* pevent);/*提交一个信号量，发出一个信号量，也就是申请完钥匙还回去 */
#if OS_SEM_ACCEPT_EN > 0u
INT16U  OSSemAccept(OS_EVENT* pevent); /*无等待的信号量，就是说申请到信号量就拿，申请不到也不阻塞自己，继续往下执行 */
#endif
 /*放弃其他任务等待该信号量 */
#if OS_SEM_PEND_ABORT_EN > 0u
INT8U  OSSemPendAbort(OS_EVENT* pevent,					/*ECB地址 */
    INT8U      opt,						/*参数 */
    INT8U* perr);						/*返回值 */
#endif
 /*直接设置信号量的值 */
#if OS_SEM_SET_EN > 0u
void  OSSemSet(OS_EVENT* pevent,
    INT16U     cnt,							/*要设置信号量的值 */
    INT8U* perr);
#endif












#if 1

/*任务控制块初始化 */
INT8U  OS_TCBInit(INT8U    prio,		    /*被创建任务的优先级 */
    OS_STK* ptos,		    /*任务堆栈的栈顶的地址 */
    OS_STK* pbos,		    /*任务堆栈发栈底的地址 */
    INT16U   id,			    /*任务ID */
    INT32U   stk_size,	    /*任务栈大小 */
    void* pext,		    /*任务控制块的扩展的地址 */
    INT16U   opt)		    /*其他选项 */
{
    OS_TCB* ptcb;
    INT8U i;
    OS_ENTER_CRITICAL();								   /*进入临界区 */
    ptcb = OSTCBFreeList;                                  /*获取空闲的任务控制块 */
    if (ptcb != (OS_TCB*)0)
    {
        OSTCBFreeList = ptcb->OSTCBNext;        /*把OSTCBFreeList指针移到链表的下一项 */
        OS_EXIT_CRITICAL();								   /*推出临界区 */
        ptcb->OSTCBStkPtr = ptos;                   /*输入任务堆栈的栈顶的地址 */
        ptcb->OSTCBPrio = prio;                   /*输入优先级 */
        ptcb->OSTCBStat = OS_STAT_RDY;            /*刚创建的任务处于就绪态 */
        ptcb->OSTCBStatPend = OS_STAT_PEND_OK;        /*没有等待任务事件 */
        ptcb->OSTCBDly = 0u;                     /*没有设置延时 */

#if OS_TASK_CREATE_EXT_EN > 0u							   /*如果使用扩展功能 */
        ptcb->OSTCBExtPtr = pext;                   /*扩展块地址 */
        ptcb->OSTCBStkSize = stk_size;               /*堆栈大小 */
        ptcb->OSTCBStkBottom = pbos;                   /*栈底 */
        ptcb->OSTCBOpt = opt;                    /*扩展选项 */
        ptcb->OSTCBId = id;                     /*任务ID */
#else/*防止警告 */
        pext = pext;
        stk_size = stk_size;
        pbos = pbos;
        opt = opt;
        id = id;
#endif

#if OS_TASK_DEL_EN > 0u												/*如果允许删除任务 */
        ptcb->OSTCBDelReq = OS_ERR_NONE;						/*任务删除标志置NONE */
#endif

#if OS_LOWEST_PRIO <= 63u                                         
        ptcb->OSTCBY = (INT8U)(prio >> 3u);				/*找到优先级在表的第几行 */
        ptcb->OSTCBX = (INT8U)(prio & 0x07u);			/*找到优先级在表的第几列 */
#else                                                             
        ptcb->OSTCBY = (INT8U)((INT8U)(prio >> 4u) & 0xFFu);
        ptcb->OSTCBX = (INT8U)(prio & 0x0Fu);
#endif

        ptcb->OSTCBBitY = (OS_PRIO)(1uL << ptcb->OSTCBY);	/*输入优先级对应表中的位置 */
        ptcb->OSTCBBitX = (OS_PRIO)(1uL << ptcb->OSTCBX);

#if (OS_EVENT_EN)/*如果使用事件控制快 */
        ptcb->OSTCBEventPtr = (OS_EVENT*)0;                  /*初始化ECB指针 */
#if (OS_EVENT_MULTI_EN > 0u)/*如果使用多事件 */
        ptcb->OSTCBEventMultiPtr = (OS_EVENT**)0;                  /*初始化多事件指针 */
#endif
#endif

#if (OS_FLAG_EN > 0u) && (OS_MAX_FLAGS > 0u) && (OS_TASK_DEL_EN > 0u)
        ptcb->OSTCBFlagNode = (OS_FLAG_NODE*)0;					/*初始化事件标志节点 */
#endif

#if (OS_MBOX_EN > 0u) || ((OS_Q_EN > 0u) && (OS_MAX_QS > 0u))
        ptcb->OSTCBMsg = (void*)0;							/*初始化消息地址 */
#endif


#if OS_TASK_PROFILE_EN > 0u                                         /*如果使用ECB参数 */
        ptcb->OSTCBCtxSwCtr = 0uL;                               /*切换到该任务的次数 */
        ptcb->OSTCBCyclesStart = 0uL;                               /*任务运行的总的时间周期 */
        ptcb->OSTCBCyclesTot = 0uL;                               /* Snapshot of cycle counter at start of task resumption   */
        ptcb->OSTCBStkBase = (OS_STK*)0;                       /*任务堆栈的起始地址 */
        ptcb->OSTCBStkUsed = 0uL;                               /*任务堆栈中使用过的空间数 */
#endif

#if OS_TASK_NAME_EN > 0u
        ptcb->OSTCBTaskName = (INT8U*)(void*)"?";
#endif

#if OS_TASK_REG_TBL_SIZE > 0u										/*是否使用任务注册表 */
        for (i = 0u; i < OS_TASK_REG_TBL_SIZE; i++)
        {
            ptcb->OSTCBRegTbl[i] = 0u;								/*初始化任务注册表 */
        }
#endif

        OSTCBInitHook(ptcb);							   /*空函数，内容需要读者填写，和Windows的钩子函数性质相同 */
        OSTaskCreateHook(ptcb);                            /*又是个钩子函数，用户创建任务钩子 */

        OS_ENTER_CRITICAL();							   /*进入临界区 */
        OSTCBPrioTbl[prio] = ptcb;						   /*把初始化任务控制块的地址给任务优先级指针表 */
        ptcb->OSTCBNext = OSTCBList;                    /*将当前控制块添加到就绪表最前面 */
        ptcb->OSTCBPrev = (OS_TCB*)0;
        if (OSTCBList != (OS_TCB*)0)
        {
            OSTCBList->OSTCBPrev = ptcb;
        }
        OSTCBList = ptcb;




        OSRdyGrp |= ptcb->OSTCBBitY;         /*设置就绪组 */
        OSRdyTbl[ptcb->OSTCBY] |= ptcb->OSTCBBitX;		   /*设置就绪表 */
        OSTaskCtr++;                                       /*任务数+1 */
        OS_EXIT_CRITICAL();								   /*退出临界区 */
        return (OS_ERR_NONE);
    }
    OS_EXIT_CRITICAL();
    return (OS_ERR_TASK_NO_MORE_TCB);
}

#endif

/*操作系统初始化 */
void  OSInit(void)
{
    OSInitHookBegin();                                           /*用户可编写该函数，实现端口说明 */
    OS_InitMisc();                                               /*初始化各种全局变量 */
    OS_InitRdyList();                                            /*初始化就绪表 */
    OS_InitTCBList();                                            /*初始化任务控制块，空闲链表 */
    OS_InitEventList();                                          /*初始化空闲事件列表 */
    OS_FlagInit();                                               /*对事件标志进行初始化 */
    OS_MemInit();                                                /*初始化内存 */
    OS_QInit();                                                  /*初始化消息队列 */
    OS_InitTaskIdle();                                           /*创建空闲任务 */
    OS_InitTaskStat();                                           /*创建统计任务 */
    OSTmr_Init();                                                /*初始化定时器管理模块 */
    OSInitHookEnd();                                             /*用户可继续编写该函数，实现端口说明 */
    OSDebugInit();												 /*加入调试代码 */

}

/*初始化各种全局变量 */
void  OS_InitMisc(void)
{
#if OS_TIME_GET_SET_EN > 0u
    OSTime = 0uL;                       /*清系统时间 */
#endif
    OSIntNesting = 0u;                     /*清中断嵌套计数 */
    OSLockNesting = 0u;                    /*调度锁计数 */
    OSTaskCtr = 0u;                        /*当前任务数 */
    OSRunning = OS_FALSE;                  /*关闭多任务 */
    OSCtxSwCtr = 0u;                       /*清任务切换次数 */
    OSIdleCtr = 0uL;                       /*清空闲计数器 */

#if OS_TASK_STAT_EN > 0u			       /*如果需要统计函数 */
    OSIdleCtrRun = 0uL;					   /*1秒内空闲计数值置0 */
    OSIdleCtrMax = 0uL;					   /*最大空闲计数值置0 */
    OSStatRdy = OS_FALSE;                  /*统计任务准备状态置false *//* Statistic task is not ready              */
#endif
}

/*初始化就绪组表 */
void  OS_InitRdyList(void)
{
    INT8U  i;
    OSRdyGrp = 0u;                                    /*清空就绪组 */
    for (i = 0u; i < OS_RDY_TBL_SIZE; i++)				   /*清空就绪表 */
    {
        OSRdyTbl[i] = 0u;
    }
    OSPrioCur = 0u;									   /*当前任务优先级 */
    OSPrioHighRdy = 0u;									   /*运行任务的最高优先级 */
    OSTCBHighRdy = (OS_TCB*)0;						   /*最高优先级的任务控制块的指针 */
    OSTCBCur = (OS_TCB*)0;						   /*当前运行的任务控制块的指针 */

}


/*初始化任务控制块，空闲链表 */
void  OS_InitTCBList(void)
{
    INT8U    ix;
    INT8U    ix_next;
    OS_TCB* ptcb1;
    OS_TCB* ptcb2;

    OS_MemClr((INT8U*)&OSTCBTbl[0], sizeof(OSTCBTbl));      /*清空所有的任务控制块 */
    OS_MemClr((INT8U*)&OSTCBPrioTbl[0], sizeof(OSTCBPrioTbl));  /*清空任务优先级指针表 */
    for (ix = 0u; ix < (OS_MAX_TASKS + OS_N_SYS_TASKS - 1u); ix++) /*把所有控制块都连上，连成一个链表 */
    {
        ix_next = ix + 1u;
        ptcb1 = &OSTCBTbl[ix];
        ptcb2 = &OSTCBTbl[ix_next];
        ptcb1->OSTCBNext = ptcb2;

    #if OS_TASK_NAME_EN > 0u
        ptcb1->OSTCBTaskName = (INT8U*)(void*)"?";
    #endif
    }
    ptcb1 = &OSTCBTbl[ix];
    ptcb1->OSTCBNext = (OS_TCB*)0;                       /*最后一项指向0地址 */
#if OS_TASK_NAME_EN > 0u
    ptcb1->OSTCBTaskName = (INT8U*)(void*)"?";              /* Unknown name                       */
#endif
    OSTCBList = (OS_TCB*)0;                       /*就绪链表指针指向0 */
    OSTCBFreeList = &OSTCBTbl[0];						 /*空闲链表指针指向表头 */
}

/*初始化空闲事件列表 */
void  OS_InitEventList(void)
{
#if (OS_EVENT_EN) && (OS_MAX_EVENTS > 0u)
#if (OS_MAX_EVENTS > 1u)
    INT16U     ix;
    INT16U     ix_next;
    OS_EVENT*  pevent1;
    OS_EVENT*  pevent2;
    OS_MemClr((INT8U*)&OSEventTbl[0], sizeof(OSEventTbl));		/*清空所有的事件控制块 */
    for (ix = 0u; ix < (OS_MAX_EVENTS - 1u); ix++)				/*初始化ECB，空闲链表连上 */
    {
        ix_next = ix + 1u;
        pevent1 = &OSEventTbl[ix];
        pevent2 = &OSEventTbl[ix_next];
        pevent1->OSEventType = OS_EVENT_TYPE_UNUSED;
        pevent1->OSEventPtr = pevent2;
#if OS_EVENT_NAME_EN > 0u
        pevent1->OSEventName = (INT8U*)(void*)"?";
#endif
    }
    pevent1 = &OSEventTbl[ix];			/*最后一个ECB赋值 */
    pevent1->OSEventType = OS_EVENT_TYPE_UNUSED;		/*最后一个ECB赋值 */
    pevent1->OSEventPtr = (OS_EVENT*)0;			/*最后一个ECB赋值 */
#if OS_EVENT_NAME_EN > 0u
    pevent1->OSEventName = (INT8U*)(void*)"?";		/*最后一个ECB赋值 */
#endif
    OSEventFreeList = &OSEventTbl[0];			/*空闲指针指向表头 */
#else
    OSEventFreeList = &OSEventTbl[0];
    OSEventFreeList->OSEventType = OS_EVENT_TYPE_UNUSED;
    OSEventFreeList->OSEventPtr = (OS_EVENT*)0;
#if OS_EVENT_NAME_EN > 0u
    OSEventFreeList->OSEventName = (INT8U*)"?";
#endif
#endif
#endif
}









void  OS_MemClr(INT8U* pdest,
    INT16U  size)
{
    while (size > 0u) {
        *pdest++ = (INT8U)0;
        size--;
    }
}


void OS_FlagInit() /*对事件标志进行初始化 */
{
    ;
}
void OS_MemInit() /*初始化内存 */
{
    ;
}
void OS_QInit() /*初始化消息队列 */
{
    ;
}
void OS_InitTaskIdle() /*创建空闲任务 */
{
    ;
}
void OS_InitTaskStat() /*创建统计任务 */
{
    ;
}
void OSTmr_Init() /*初始化定时器管理模块 */
{
    ;
}
void OSDebugInit() /*加入调试代码 */
{
    ;
}

void OSInitHookEnd()  /*用户可继续编写该函数，实现端口说明 */
{
    ;
}

void          OS_EXIT_CRITICAL()
{
    /*为了编译 */
}


void          OS_ENTER_CRITICAL()
{
    /*为了编译 */
}
void          OSTCBInitHook(OS_TCB* ptcb)/*空函数 */
{
    /*为了编译 */
}
void          OSTaskCreateHook(OS_TCB* ptcb)/*创建钩子函数 */
{
    /*为了编译 */
}

void          OSInitHookBegin(void) /*用户可编写该函数，实现端口说明 */
{
    /*为了编译 */
}
void OSTimeTickHook()               /*用户的钩子函数 */
{
    /*为了编译 */
}

/*自己加的 */
/*初始化任务栈 */
OS_STK* OSTaskStkInit(void           (*task)(void* p_arg),          /*任务代码的地址 */
    void* pdata,			/*任务参数 */
    OS_STK* ptos,		    /*任务栈的栈顶指针 */
    INT16U    opt)				/*栈的初始化选项 */
{
    /*为了编译 */
}





/*在事件等待组和表中删除该任务 */
/*取消等待事件的表和组 */
#if (OS_EVENT_EN)
void  OS_EventTaskRemove(OS_TCB* ptcb,
    OS_EVENT* pevent)
{
    INT8U  y;
    y = ptcb->OSTCBY;
    pevent->OSEventTbl[y] &= (OS_PRIO)~ptcb->OSTCBBitX;/*处理事件等待表 */
    if (pevent->OSEventTbl[y] == 0u)
    {
        pevent->OSEventGrp &= (OS_PRIO)~ptcb->OSTCBBitY;/*处理事件等待组 */
    }
}
#endif



#if ((OS_EVENT_EN) && (OS_EVENT_MULTI_EN > 0u))
void  OS_EventTaskRemoveMulti(OS_TCB* ptcb,
    OS_EVENT** pevents_multi)
{
    OS_EVENT** pevents = 0;
    OS_EVENT* pevent = 0;
    INT8U      y = 0;
    OS_PRIO    bity = 0;
    OS_PRIO    bitx = 0;


    y = ptcb->OSTCBY;
    bity = ptcb->OSTCBBitY;
    bitx = ptcb->OSTCBBitX;
    pevents = pevents_multi;
    pevent = *pevents;
    while (pevent != (OS_EVENT*)0)
    {
        pevent->OSEventTbl[y] &= (OS_PRIO)~bitx;
        if (pevent->OSEventTbl[y] == 0u)
        {
            pevent->OSEventGrp &= (OS_PRIO)~bity;
        }
        pevents++;
        pevent = *pevents;
    }
}
#endif




/*时间片调度函数 */
void  OSTimeTick(void)
{
    OS_TCB* ptcb;
#if OS_TIME_TICK_HOOK_EN > 0u
    OSTimeTickHook();                                      /*调用用户的钩子函数 */
#endif
#if OS_TIME_GET_SET_EN > 0u
    OS_ENTER_CRITICAL();
    OSTime++;											   /*调度计数器+1 */
    OS_EXIT_CRITICAL();
#endif
    if (OSRunning == OS_TRUE)							   /*如果已经启动了多任务 */
    {
#if OS_TICK_STEP_EN > 0u
        switch (OSTickStepState) {                         /* Determine whether we need to process a tick  */
        case OS_TICK_STEP_DIS:                         /* Yes, stepping is disabled                    */
            step = OS_TRUE;
            break;

        case OS_TICK_STEP_WAIT:                        /* No,  waiting for uC/OS-View to set ...       */
            step = OS_FALSE;                          /*      .. OSTickStepState to OS_TICK_STEP_ONCE */
            break;

        case OS_TICK_STEP_ONCE:                        /* Yes, process tick once and wait for next ... */
            step = OS_TRUE;                /*      ... step command from uC/OS-View        */
            OSTickStepState = OS_TICK_STEP_WAIT;
            break;

        default:                                       /* Invalid case, correct situation              */
            step = OS_TRUE;
            OSTickStepState = OS_TICK_STEP_DIS;
            break;
        }
        if (step == OS_FALSE) {                            /* Return if waiting for step command           */
            return;
        }
#endif
        ptcb = OSTCBList;                                  /*ptcb指向就绪链表的表头 */
        while (ptcb->OSTCBPrio != OS_TASK_IDLE_PRIO)	   /*如果就绪表头的任务不是空闲 */
        {
            OS_ENTER_CRITICAL();
            if (ptcb->OSTCBDly != 0u)                      /*如果该任务设置了时间延时或事件等待延时 */
            {
                ptcb->OSTCBDly--;                          /*延时-1，因为过了一个时钟滴答 */
                if (ptcb->OSTCBDly == 0u)                  /*延时到期 */
                {
                    if ((ptcb->OSTCBStat & OS_STAT_PEND_ANY) != OS_STAT_RDY)			/*如果任务有等待事件 */
                    {
                        ptcb->OSTCBStat &= (INT8U)~(INT8U)OS_STAT_PEND_ANY;			/*清任务状态 */
                        ptcb->OSTCBStatPend = OS_STAT_PEND_TO;                 			/*指示事件因超时而结束 */
                    }
                    else
                    {
                        ptcb->OSTCBStatPend = OS_STAT_PEND_OK;							/*指示延时时间到了 */
                    }
                    if ((ptcb->OSTCBStat & OS_STAT_SUSPEND) == OS_STAT_RDY)				/*如果任务不是被挂起的 */
                    {
                        OSRdyGrp |= ptcb->OSTCBBitY;             			/* No,  Make ready          */
                        OSRdyTbl[ptcb->OSTCBY] |= ptcb->OSTCBBitX;
                    }
                }
            }
            ptcb = ptcb->OSTCBNext;                        /*指向下一个TCB */
            OS_EXIT_CRITICAL();
        }
    }
}


/*用来确定最高优先级的就绪任务 */
 void  OS_SchedNew(void)
{
#if OS_LOWEST_PRIO <= 63u                        /* See if we support up to 64 tasks                   */
    INT8U   y;
    y = OSUnMapTbl[OSRdyGrp];
    OSPrioHighRdy = (INT8U)((y << 3u) + OSUnMapTbl[OSRdyTbl[y]]);
#else                                            /* We support up to 256 tasks                         */
    INT8U     y;
    OS_PRIO* ptbl;
    if ((OSRdyGrp & 0xFFu) != 0u) {
        y = OSUnMapTbl[OSRdyGrp & 0xFFu];
    }
    else {
        y = OSUnMapTbl[(OS_PRIO)(OSRdyGrp >> 8u) & 0xFFu] + 8u;
    }
    ptbl = &OSRdyTbl[y];
    if ((*ptbl & 0xFFu) != 0u) {
        OSPrioHighRdy = (INT8U)((y << 4u) + OSUnMapTbl[(*ptbl & 0xFFu)]);
    }
    else {
        OSPrioHighRdy = (INT8U)((y << 4u) + OSUnMapTbl[(OS_PRIO)(*ptbl >> 8u) & 0xFFu] + 8u);
    }
#endif
}

 void OS_Sched()             /*调度函数 */
 {
     OS_ENTER_CRITICAL();
     if (OSIntNesting == 0u)								/*调度任务时必须没在中断里 */
     {
         if (OSLockNesting == 0u)						/*调度器没有上锁 */
         {
             OS_SchedNew();								/*找到最高优先级的任务*/
             OSTCBHighRdy = OSTCBPrioTbl[OSPrioHighRdy];	/*获取优先级最高任务控制块地址 */
             if (OSPrioHighRdy != OSPrioCur)				/*如果不是当前任务，就要进行切换 */
             {
#if OS_TASK_PROFILE_EN > 0u
                 OSTCBHighRdy->OSTCBCtxSwCtr++;			/*任务调度次数++ */
#endif
                 OSCtxSwCtr++;							/*操作系统切换任务次数++ */
                 OS_TASK_SW();							/*进行任务切换 */
             }
         }
     }
     OS_EXIT_CRITICAL();
 }


 void OS_TASK_SW()       /*进行任务切换 */
 {
     /*为了编译 */
 }


 /*时钟中断中任务切换 */
 void  OSIntExit(void)
 {
     if (OSRunning == OS_TRUE)								/*多任务的时候才能调度 */
     {
         OS_ENTER_CRITICAL();
         if (OSIntNesting > 0u)								/*如果当前有中断嵌套 */
         {
             OSIntNesting--;									/*退出的时候嵌套数-1 */
         }
         if (OSIntNesting == 0u)								/*当前没中断嵌套 */
         {
             if (OSLockNesting == 0u)						/*调度器未加锁 */
             {
                 OS_SchedNew();								/*确定最高优先级的就绪任务的 */
                 OSTCBHighRdy = OSTCBPrioTbl[OSPrioHighRdy];	/*获取优先级最高的任务控制块 */
                 if (OSPrioHighRdy != OSPrioCur)				/*最高优先级的是不是当前任务 */
                 {
#if OS_TASK_PROFILE_EN > 0u
                     OSTCBHighRdy->OSTCBCtxSwCtr++;
#endif
                     OSCtxSwCtr++;
                     OSIntCtxSw();							/*切换任务 */
                 }
             }
         }
         OS_EXIT_CRITICAL();
     }
 }

 void OSIntCtxSw(void)/*切换任务 */
 {
     /*入栈存CPU值 */
     /*全局变量赋值 */
     /*出栈恢复CPU值 */
 }

 void  OSStart(void)/*启动多任务 */
 {
     if (OSRunning == OS_FALSE)						/*如果系统没有启动多任务 */
     {
         OS_SchedNew();								/*找到优先级最高的任务 */
         OSPrioCur = OSPrioHighRdy;
         OSTCBHighRdy = OSTCBPrioTbl[OSPrioHighRdy];
         OSTCBCur = OSTCBHighRdy;
         OSStartHighRdy();							/*启动多任务 */
     }
 }


 void OSStartHighRdy(void)/*启动多任务 */
 {
     OSTaskSwHook();					/*钩子函数 */
     OSRunning = OS_TRUE;			/*标志位变成启动多任务 */
     //_asm
     //{
         /*任务栈地址赋值给ebx*/
         /*esp指向任务堆栈*/
         /*恢复所有通用寄存器*/
         /*恢复标志寄存器*/
         /*在堆栈中取出任务地址并开始任务 */
     //}
 }

 void OSTaskSwHook(void)/*钩子函数 */
 {
     /*钩子函数 */;
 }


 /*空闲任务 */
 void  OS_TaskIdle(void* p_arg)
 {
     p_arg = p_arg;								/*防止报错 */
     for (;;)
     {
         OS_ENTER_CRITICAL();
         OSIdleCtr++;							/*空闲计数器++ */
         OS_EXIT_CRITICAL();
         OSTaskIdleHook();						/*钩子函数 */
     }
 }

 void OSTaskIdleHook(void)/*钩子函数 */
 {
     /*钩子函数 */;
 }
 void OSTaskStatHook()/*钩子函数 */
 {
     ;
 }

#if OS_TASK_STAT_EN > 0u
 /*统计任务的初始化 */
 /*目的是获取系统空闲计数的最大值,该函数在用户任务中被调用，这函数在一直过程中命名为TaskStart，优先级是0，在一直部分可以看到*/
 /*这是系统没有运行其他任务，统计任务初始化函数将自己阻塞2个时钟周期，在系统时钟中断2次后，有调度器恢复运行，叫做时钟同步*/
 void  OSStatInit(void)
 {
     OSTimeDly(2u);								/*延时两个时钟周期，目的是与时钟同步 */
     OS_ENTER_CRITICAL();
     OSIdleCtr = 0uL;							/*空闲计数器清0*/
     OS_EXIT_CRITICAL();
     OSTimeDly(OS_TICKS_PER_SEC / 10u);			/*延时100ms，此时空闲任务一直OSIdleCtr++*/
     OS_ENTER_CRITICAL();
     OSIdleCtrMax = OSIdleCtr;					/*获取最大空闲计数值*/
     OSStatRdy = OS_TRUE;						/*统计任务准备状态OK */
     OS_EXIT_CRITICAL();
 }
#endif

#if OS_TASK_STAT_EN > 0u
 /*统计任务代码 */
 void  OS_TaskStat(void* p_arg)
 {
     {
         p_arg = p_arg;									/*防止编译报错*/
         while (OSStatRdy == OS_FALSE)					/*统计任务没准备好*/
         {
             OSTimeDly(2u * OS_TICKS_PER_SEC / 10u);		/*延时0.2s，等她准备好*/
         }
         OSIdleCtrMax /= 100uL;
         if (OSIdleCtrMax == 0uL)						/*计数太少了，证明系统比较忙 */
         {
             OSCPUUsage = 0u;
#if OS_TASK_SUSPEND_EN > 0u
             (void)OSTaskSuspend(OS_PRIO_SELF);			/*挂起自己 */
#else
             for (;;)
             {
                 OSTimeDly(OS_TICKS_PER_SEC);
             }
#endif
         }
         for (;;)
         {
             OS_ENTER_CRITICAL();
             OSIdleCtrRun = OSIdleCtr;					/*获取过去100ms的空闲计数 */
             OSIdleCtr = 0uL;							/*清空闲计数*/
             OS_EXIT_CRITICAL();
             OSCPUUsage = (INT8U)(100uL - OSIdleCtrRun / OSIdleCtrMax);
             OSTaskStatHook();							/*钩子函数 */
#if (OS_TASK_STAT_STK_CHK_EN > 0u) && (OS_TASK_CREATE_EXT_EN > 0u)
             OS_TaskStatStkChk();						/*堆栈检查 */
#endif
             OSTimeDly(OS_TICKS_PER_SEC / 10u);			/*延时0.1s，为了下一个100ms累计OSIdleCtr */
         }
     }
 }
#endif

 /*进入中断 */
 void  OSIntEnter(void)
 {
     if (OSRunning == OS_TRUE)
     {
         if (OSIntNesting < 255u)
         {
             OSIntNesting++;/*中断嵌套数+1 */
         }
     }
 }

 /*时钟中断服务函数 */
 void OSTickISRuser()
 {
     OSTime++;
     if (!FlagEn)/*当前中断被屏蔽或者处于临界区*/
     {
         return;
     }
     //SuspendThread(mainhandle);/*中止主线程运行，模拟中断产生*/
     //GetThreadContext(mainhandle, &Context);/*获取主线程的上下文*/
     OSIntEnter();/*嵌套数+1*/
     //OSTCBCur->OSTCBStkPtr = (OS_STK*)Context.Esp;/*把当前任务的堆栈地址存到任务控制块*/
     OSTimeTick();/*找到优先级最高的任务*/
     OSIntExit();/*嵌套数-1*/
     //ResumeThread(mainhandle);/*主线程继续执行，模拟中断返回*/
 }



 /*事件等待表初始化，也就是清一下ECB中的等待组+表 */
#if (OS_EVENT_EN)
 void  OS_EventWaitListInit(OS_EVENT* pevent)
 {
     INT8U  i;
     pevent->OSEventGrp = 0u;						/*清事件等待组 */
     for (i = 0u; i < OS_EVENT_TBL_SIZE; i++)		/*清事件等待表 */
     {
         pevent->OSEventTbl[i] = 0u;
     }
 }
#endif


 /*设置事件等待函数，将任务在ECB中登记的函数，把任务在就绪组表中取消 */
#if (OS_EVENT_EN)
 void  OS_EventTaskWait(OS_EVENT* pevent)
 {
     INT8U  y;
     OSTCBCur->OSTCBEventPtr = pevent;						/*让任务控制块的指针指向事件控制块，这样就可以直接调用TCB来控制ECB了 */
     pevent->OSEventTbl[OSTCBCur->OSTCBY] |= OSTCBCur->OSTCBBitX;		/*ECB等待表中添加任务优先级编号 */
     pevent->OSEventGrp |= OSTCBCur->OSTCBBitY;		/*ECB等待组中添加任务优先级编号 */
     y = OSTCBCur->OSTCBY;									/*任务等待事件后需要阻塞当前任务，处理一下就绪表和就绪组 */
     OSRdyTbl[y] &= (OS_PRIO)~OSTCBCur->OSTCBBitX;
     if (OSRdyTbl[y] == 0u)
     {
         OSRdyGrp &= (OS_PRIO)~OSTCBCur->OSTCBBitY;
     }
 }
#endif




 /*将等待事件的任务就绪 */
#if (OS_EVENT_EN)
 INT8U  OS_EventTaskRdy(OS_EVENT* pevent,				/*ECB的指针 */
     void* pmsg,				/*消息指针 */
     INT8U      msk,					/*清除状态位的掩码 */
     INT8U      pend_stat)			/*等待(pend)结束 */
 {
     OS_TCB* ptcb;
     INT8U     y;
     INT8U     x;
     INT8U     prio;
#if OS_LOWEST_PRIO > 63u
     OS_PRIO* ptbl;
#endif
#if OS_LOWEST_PRIO <= 63u
     y = OSUnMapTbl[pevent->OSEventGrp];
     x = OSUnMapTbl[pevent->OSEventTbl[y]];
     prio = (INT8U)((y << 3u) + x);						/*根据事件等待组和表，找到正在等待事件的任务中最高优先级的 */
#else
     if ((pevent->OSEventGrp & 0xFFu) != 0u)				/* Find HPT waiting for message*/
     {
         y = OSUnMapTbl[pevent->OSEventGrp & 0xFFu];
     }
     else
     {
         y = OSUnMapTbl[(OS_PRIO)(pevent->OSEventGrp >> 8u) & 0xFFu] + 8u;
     }
     ptbl = &pevent->OSEventTbl[y];
     if ((*ptbl & 0xFFu) != 0u)
     {
         x = OSUnMapTbl[*ptbl & 0xFFu];
     }
     else
     {
         x = OSUnMapTbl[(OS_PRIO)(*ptbl >> 8u) & 0xFFu] + 8u;
     }
     prio = (INT8U)((y << 4u) + x);						/* Find priority of task getting the msg       */
#endif
     ptcb = OSTCBPrioTbl[prio];		/*找到任务控制块 */
     ptcb->OSTCBDly = 0u;						/*延时时间清0，如果不是0每个时钟周期会--，会产生冲突 */
#if ((OS_Q_EN > 0u) && (OS_MAX_QS > 0u)) || (OS_MBOX_EN > 0u)
     ptcb->OSTCBMsg = pmsg;						/*TCB中指针指向ECB */
#else
     pmsg = pmsg;
#endif
     ptcb->OSTCBStat &= (INT8U)~msk;				/*清除任务控制块的任务状态标志 */
     ptcb->OSTCBStatPend = pend_stat;					/*设置等待状态 */
                                                         /* See if task is ready (could be susp'd)      */
     if ((ptcb->OSTCBStat & OS_STAT_SUSPEND) == OS_STAT_RDY)			/*如果任务没有挂起 */
     {
         OSRdyGrp |= ptcb->OSTCBBitY;			/*把任务就绪了 */
         OSRdyTbl[y] |= ptcb->OSTCBBitX;
     }
     OS_EventTaskRemove(ptcb, pevent);					/*事件等待表和组中删除该任务 */
#if (OS_EVENT_MULTI_EN > 0u)
     if (ptcb->OSTCBEventMultiPtr != (OS_EVENT**)0)		/* Remove this task from events' wait lists    */
     {
         OS_EventTaskRemoveMulti(ptcb, ptcb->OSTCBEventMultiPtr);
         ptcb->OSTCBEventPtr = (OS_EVENT*)pevent;/* Return event as first multi-pend event ready */
     }
#endif
     return (prio);
 }
#endif



 /*创建一个信号量 */
 OS_EVENT* OSSemCreate(INT16U cnt)
 {
     OS_EVENT* pevent;
     if (OSIntNesting > 0u)												/*中断中不允许创建信号量 */
     {
         return ((OS_EVENT*)0);
     }
     OS_ENTER_CRITICAL();
     pevent = OSEventFreeList;											/*获取一个ECB */
     if (OSEventFreeList != (OS_EVENT*)0)
     {
         OSEventFreeList = (OS_EVENT*)OSEventFreeList->OSEventPtr;		/*OSEventFreeList指向下一个 */
     }
     OS_EXIT_CRITICAL();
     if (pevent != (OS_EVENT*)0)
     {
         pevent->OSEventType = OS_EVENT_TYPE_SEM;
         pevent->OSEventCnt = cnt;
         pevent->OSEventPtr = (void*)0;
#if OS_EVENT_NAME_EN > 0u
         pevent->OSEventName = (INT8U*)(void*)"?";					/*这四句，配置ECB */
#endif
         OS_EventWaitListInit(pevent);									/*清等待事件组表 */
     }
     return (pevent);
 }


 /*删除一个信号量/消息 */
#if OS_SEM_DEL_EN > 0u
 OS_EVENT* OSSemDel(OS_EVENT* pevent,						/*ECB地址 */
     INT8U      opt,						/*删除选项 */
     INT8U* perr)						/*返回值 */
 {
     INT8U    tasks_waiting;								/*有没有任务在等信号量 */
     OS_EVENT* pevent_return;

#if OS_ARG_CHK_EN > 0u
     if (pevent == (OS_EVENT*)0)							/*如果ECB不存在 */
     {
         *perr = OS_ERR_PEVENT_NULL;
         return (pevent);
     }
#endif
     if (pevent->OSEventType != OS_EVENT_TYPE_SEM)			/*如果类型不符合 */
     {
         *perr = OS_ERR_EVENT_TYPE;
         return (pevent);
     }
     if (OSIntNesting > 0u)									/*如果在中断中 */
     {
         *perr = OS_ERR_DEL_ISR;
         return (pevent);
     }
     OS_ENTER_CRITICAL();
     if (pevent->OSEventGrp != 0u)							/*等待组不是0，表示有任务在等这信号量 */
     {
         tasks_waiting = OS_TRUE;
     }
     else
     {
         tasks_waiting = OS_FALSE;
     }
     switch (opt)
     {
     case OS_DEL_NO_PEND:								/*有任务等信号量就不能删除的情况 */
         if (tasks_waiting == OS_FALSE)
         {
#if OS_EVENT_NAME_EN > 0u
             pevent->OSEventName = (INT8U*)(void*)"?";
#endif
             pevent->OSEventType = OS_EVENT_TYPE_UNUSED;
             pevent->OSEventPtr = OSEventFreeList;
             pevent->OSEventCnt = 0u;
             OSEventFreeList = pevent;			/*上面是删除信号量后配置的ECB */
             OS_EXIT_CRITICAL();
             *perr = OS_ERR_NONE;
             pevent_return = (OS_EVENT*)0;	/*删除成功返回空指针 */
         }
         else
         {
             OS_EXIT_CRITICAL();
             *perr = OS_ERR_TASK_WAITING;
             pevent_return = pevent;			/*删除失败，返回当前ECB */
         }
         break;

     case OS_DEL_ALWAYS:									/*强制删除的情况 */
         while (pevent->OSEventGrp != 0u)				/*有任务在等信号量 */
         {
             (void)OS_EventTaskRdy(pevent, (void*)0, OS_STAT_SEM, OS_STAT_PEND_OK);		/*把等信号量的任务就绪 */
         }
#if OS_EVENT_NAME_EN > 0u
         pevent->OSEventName = (INT8U*)(void*)"?";
#endif
         pevent->OSEventType = OS_EVENT_TYPE_UNUSED;
         pevent->OSEventPtr = OSEventFreeList;
         pevent->OSEventCnt = 0u;
         OSEventFreeList = pevent;				/*上面是删除信号量后配置的ECB */
         OS_EXIT_CRITICAL();
         if (tasks_waiting == OS_TRUE)					/*恢复任务后，有任务就绪了 */
         {
             OS_Sched();								/*调度一下 */
         }
         *perr = OS_ERR_NONE;
         pevent_return = (OS_EVENT*)0;		/*删除成功返回空指针 */
         break;

     default:											/*传的删除选项opt无效 */
         OS_EXIT_CRITICAL();
         *perr = OS_ERR_INVALID_OPT;
         pevent_return = pevent;
         break;
     }
     return (pevent_return);
 }
#endif


 /*请求一个信号量，等待一个信号量，也就是任务申请一把钥匙 */
 void  OSSemPend(OS_EVENT* pevent,						/*ECB地址 */
     INT32U     timeout,					/*设定的超时时间 */
     INT8U* perr)
 {
#if OS_ARG_CHK_EN > 0u
     if (pevent == (OS_EVENT*)0)						/*ECB地址为空 */
     {
         *perr = OS_ERR_PEVENT_NULL;
         return;
     }
#endif
     if (pevent->OSEventType != OS_EVENT_TYPE_SEM)		/*类型无效 */
     {
         *perr = OS_ERR_EVENT_TYPE;
         return;
     }
     if (OSIntNesting > 0u)								/*中断里不可以 */
     {
         *perr = OS_ERR_PEND_ISR;
         return;
     }
     if (OSLockNesting > 0u)								/*调度器上锁了 */
     {
         *perr = OS_ERR_PEND_LOCKED;
         return;
     }
     OS_ENTER_CRITICAL();
     if (pevent->OSEventCnt > 0u)						/*信号量的值必须大于0 */
     {
         pevent->OSEventCnt--;							/*拿走一个信号量 */
         OS_EXIT_CRITICAL();
         *perr = OS_ERR_NONE;
         return;											/*成功请求到信号量了 */
     }
     /*当前没有信号量了，就的等待 */
     OSTCBCur->OSTCBStat |= OS_STAT_SEM;				/*在当前控制块中打上等待信号量的标记 */
     OSTCBCur->OSTCBStatPend = OS_STAT_PEND_OK;			/*等待状态赋值 */
     OSTCBCur->OSTCBDly = timeout;					/*超时时间赋上 */
     OS_EventTaskWait(pevent);							/*让任务等，在ECB中打标记，在就绪组&表中取消就绪的标记 */
     OS_EXIT_CRITICAL();
     OS_Sched();											/*调度一下 */
     OS_ENTER_CRITICAL();
     switch (OSTCBCur->OSTCBStatPend)					/*OS_Sched函数把任务切走了，这是回来的时候 */
     {
     case OS_STAT_PEND_OK:							/*事件发生了 */
         *perr = OS_ERR_NONE;
         break;
     case OS_STAT_PEND_ABORT:						/*事件超时了 */
         *perr = OS_ERR_PEND_ABORT;
         break;
     case OS_STAT_PEND_TO:
     default:
         OS_EventTaskRemove(OSTCBCur, pevent);		/*自己调函数清除ECB中的等待事件组表中的标志 */
         *perr = OS_ERR_TIMEOUT;
         break;
     }
     /*处理一下TCB */
     OSTCBCur->OSTCBStat = OS_STAT_RDY;
     OSTCBCur->OSTCBStatPend = OS_STAT_PEND_OK;
     OSTCBCur->OSTCBEventPtr = (OS_EVENT*)0;
#if (OS_EVENT_MULTI_EN > 0u)
     OSTCBCur->OSTCBEventMultiPtr = (OS_EVENT**)0;
#endif
     OS_EXIT_CRITICAL();
 }




 /*提交一个信号量，发出一个信号量，也就是申请完钥匙还回去 */
 INT8U  OSSemPost(OS_EVENT* pevent)
 {
#if OS_ARG_CHK_EN > 0u
     if (pevent == (OS_EVENT*)0)						/*ECB无效 */
     {
         return (OS_ERR_PEVENT_NULL);
     }
#endif
     if (pevent->OSEventType != OS_EVENT_TYPE_SEM)		/*ECB类型无效 */
     {
         return (OS_ERR_EVENT_TYPE);
     }
     OS_ENTER_CRITICAL();
     if (pevent->OSEventGrp != 0u)						/*有任务在等信号量 */
     {
         (void)OS_EventTaskRdy(pevent, (void*)0, OS_STAT_SEM, OS_STAT_PEND_OK);			/*把任务就绪 */
         OS_EXIT_CRITICAL();
         OS_Sched();										/*调度一下 */
         return (OS_ERR_NONE);
     }
     if (pevent->OSEventCnt < 65535u)					/*如果信号量没超限制 */
     {
         pevent->OSEventCnt++;							/*把信号量还回去 */
         OS_EXIT_CRITICAL();
         return (OS_ERR_NONE);
     }
     OS_EXIT_CRITICAL();
     return (OS_ERR_SEM_OVF);
 }



 /*无等待的信号量，就是说申请到信号量就拿，申请不到也不阻塞自己，继续往下执行 */
#if OS_SEM_ACCEPT_EN > 0u
 INT16U  OSSemAccept(OS_EVENT* pevent)
 {
     INT16U     cnt;
#if OS_ARG_CHK_EN > 0u
     if (pevent == (OS_EVENT*)0)						/*ECB地址无效 */
     {
         return (0u);
     }
#endif
     if (pevent->OSEventType != OS_EVENT_TYPE_SEM)	/*格式不是信号量 */
     {
         return (0u);
     }
     OS_ENTER_CRITICAL();
     cnt = pevent->OSEventCnt;
     if (cnt > 0u)									/*有信号量就--。没有拉倒 */
     {
         pevent->OSEventCnt--;
     }
     OS_EXIT_CRITICAL();
     return (cnt);									/*返回信号量值 */
 }
#endif



 /*放弃其他任务等待该信号量 */
#if OS_SEM_PEND_ABORT_EN > 0u
 INT8U  OSSemPendAbort(OS_EVENT* pevent,					/*ECB地址 */
     INT8U      opt,						/*参数 */
     INT8U* perr)						/*返回值 */
 {
     INT8U      nbr_tasks;
#if OS_ARG_CHK_EN > 0u
     if (pevent == (OS_EVENT*)0)
     {
         *perr = OS_ERR_PEVENT_NULL;
         return (0u);
     }
#endif
     if (pevent->OSEventType != OS_EVENT_TYPE_SEM)			/* Validate event block type                     */
     {
         *perr = OS_ERR_EVENT_TYPE;
         return (0u);
     }
     OS_ENTER_CRITICAL();
     if (pevent->OSEventGrp != 0u)							/*如果有任务等信号量 */
     {
         nbr_tasks = 0u;
         switch (opt)
         {
         case OS_PEND_OPT_BROADCAST:						/*让所有的任务退出等待 */
             while (pevent->OSEventGrp != 0u)
             {
                 (void)OS_EventTaskRdy(pevent, (void*)0, OS_STAT_SEM, OS_STAT_PEND_ABORT);
                 nbr_tasks++;
             }
             break;
         case OS_PEND_OPT_NONE:							/*只让最高优先级的退出等待 */
         default:
             (void)OS_EventTaskRdy(pevent, (void*)0, OS_STAT_SEM, OS_STAT_PEND_ABORT);
             nbr_tasks++;
             break;
         }
         OS_EXIT_CRITICAL();
         OS_Sched();											/*调度一下 */
         *perr = OS_ERR_PEND_ABORT;
         return (nbr_tasks);
     }
     OS_EXIT_CRITICAL();
     *perr = OS_ERR_NONE;
     return (0u);
 }
#endif



 /*直接设置信号量的值 */
#if OS_SEM_SET_EN > 0u
 void  OSSemSet(OS_EVENT* pevent,
     INT16U     cnt,							/*要设置信号量的值 */
     INT8U* perr)
 {
#if OS_ARG_CHK_EN > 0u
     if (pevent == (OS_EVENT*)0)
     {
         *perr = OS_ERR_PEVENT_NULL;
         return;
     }
#endif
     if (pevent->OSEventType != OS_EVENT_TYPE_SEM)
     {
         *perr = OS_ERR_EVENT_TYPE;
         return;
     }
     OS_ENTER_CRITICAL();
     *perr = OS_ERR_NONE;
     if (pevent->OSEventCnt > 0u)						/*信号量的值原来就>0 */
     {
         pevent->OSEventCnt = cnt;						/*信号量改成新值 */
     }
     else
     {
         if (pevent->OSEventGrp == 0u)					/*如果没任务等待 */
         {
             pevent->OSEventCnt = cnt;					/*信号量改成新值 */
         }
         else
         {
             *perr = OS_ERR_TASK_WAITING;	/*有任务等待，不允许修改 */
         }
     }
     OS_EXIT_CRITICAL();
 }
#endif

