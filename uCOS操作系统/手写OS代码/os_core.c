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
