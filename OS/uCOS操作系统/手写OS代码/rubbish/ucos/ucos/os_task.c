#include <ucos_ii.h>

/*没扩展功能创建任务 */
INT8U  OSTaskCreate(void   (*task)(void* p_arg),		/*任务代码地址 */
    void* p_arg,					/*任务参数 */
    OS_STK* ptos,						/*任务栈顶 */
    INT8U    prio);						/*任务的优先级 */
INT8U  OSTaskCreateExt(void   (*task)(void* p_arg),	/*任务代码地址 */
    void* p_arg,					/*任务参数 */
    OS_STK* ptos,					/*任务栈的栈顶 */
    INT8U    prio,					/*任务优先级 */
    INT16U   id,					/*任务ID */
    OS_STK* pbos,					/*任务栈栈底指针 */
    INT32U   stk_size,				/*任务栈大小 */
    void* pext,					    /*扩展的地址 */
    INT16U   opt);					/*任务的附加信息 */

/*堆栈清空函数 */
#if (OS_TASK_STAT_STK_CHK_EN > 0u) && (OS_TASK_CREATE_EXT_EN > 0u)
void  OS_TaskStkClr(OS_STK* pbos,
    INT32U   size,
    INT16U   opt);
#endif
void OS_Dummy();/*空函数 */
void OSTaskDelHook(ptcb);              /*删除钩子函数 */
#if OS_TASK_DEL_EN > 0u
INT8U  OSTaskDelReq(INT8U prio);       /*请求删除任务 */
#endif
#if OS_TASK_SUSPEND_EN > 0u
INT8U  OSTaskSuspend(INT8U prio);      /*挂起任务 */
#endif
#if OS_TASK_SUSPEND_EN > 0u
INT8U  OSTaskResume(INT8U prio);       /*恢复挂起任务 */
#endif







/*没扩展功能创建任务 */
#if OS_TASK_CREATE_EN > 0u
INT8U  OSTaskCreate(void   (*task)(void* p_arg),		/*任务代码地址 */
    void* p_arg,					/*任务参数 */
    OS_STK* ptos,						/*任务栈顶 */
    INT8U    prio)						/*任务的优先级 */
{
    OS_STK* psp = (OS_TCB*)0;
    INT8U      err;
#if OS_ARG_CHK_EN > 0u									/*是否进行参数检查 */
    if (prio > OS_LOWEST_PRIO) 							/*优先级超过最大值 */
    {
        return (OS_ERR_PRIO_INVALID);
    }
#endif
    OS_ENTER_CRITICAL();
    if (OSIntNesting > 0u) 								/*如果处于中断中 */
    {
        OS_EXIT_CRITICAL();
        return (OS_ERR_TASK_CREATE_ISR);				/*中断中不允许创建任务 */
    }
    if (OSTCBPrioTbl[prio] == (OS_TCB*)0) 				/*优先级指针表指向0地址，不指向0地址的话就说明该优先级被占用了 */
    {
        OSTCBPrioTbl[prio] = OS_TCB_RESERVED;			/*优先级指针表地址暂不明确，临时赋值 */
        OS_EXIT_CRITICAL();
        psp = OSTaskStkInit(task, p_arg, ptos, 0u);		/*初始化栈 */
        err = OS_TCBInit(prio, psp, (OS_STK*)0, 0u, 0u, (void*)0, 0u);/*初始化任务控制块 */
        if (err == OS_ERR_NONE)
        {
            if (OSRunning == OS_TRUE) 					/*如果多任务启动，就进行一次调度 */
            {
                OS_Sched();
            }
        }
        else
        {
            OS_ENTER_CRITICAL();
            OSTCBPrioTbl[prio] = (OS_TCB*)0;			/*没创建任务优先级清0 */
            OS_EXIT_CRITICAL();
        }
        return (err);
    }
    OS_EXIT_CRITICAL();
    return (OS_ERR_PRIO_EXIST);

}
#endif




/*有扩展功能创建任务 */
#if OS_TASK_CREATE_EXT_EN > 0u
INT8U  OSTaskCreateExt(void   (*task)(void* p_arg),	/*任务代码地址 */
    void* p_arg,					/*任务参数 */
    OS_STK* ptos,					/*任务栈的栈顶 */
    INT8U    prio,					/*任务优先级 */
    INT16U   id,					/*任务ID */
    OS_STK* pbos,					/*任务栈栈底指针 */
    INT32U   stk_size,				/*任务栈大小 */
    void* pext,					    /*扩展的地址 */
    INT16U   opt)					/*任务的附加信息 */
{
    OS_STK* psp;
    INT8U      err;


#if OS_ARG_CHK_EN > 0u
    if (prio > OS_LOWEST_PRIO) /* 检查优先级           */
    {
        return (OS_ERR_PRIO_INVALID);
    }
#endif
    OS_ENTER_CRITICAL();
    if (OSIntNesting > 0u)  /*如果处于中断中 */
    {
        OS_EXIT_CRITICAL();
        return (OS_ERR_TASK_CREATE_ISR);
    }
    if (OSTCBPrioTbl[prio] == (OS_TCB*)0)/*优先级指针表指向0地址，不指向0地址的话就说明该优先级被占用了 */
    {
        OSTCBPrioTbl[prio] = OS_TCB_RESERVED;/*优先级指针表地址暂不明确，临时赋值 */
        OS_EXIT_CRITICAL();
#if (OS_TASK_STAT_STK_CHK_EN > 0u)
        OS_TaskStkClr(pbos, stk_size, opt);                    /*清空堆栈 */
#endif

        psp = OSTaskStkInit(task, p_arg, ptos, opt);           /*初始化栈 */
        err = OS_TCBInit(prio, psp, pbos, id, stk_size, pext, opt);/*初始化任务控制块 */
        if (err == OS_ERR_NONE)
        {
            if (OSRunning == OS_TRUE)/*如果多任务启动，就进行一次调度 */
            {
                OS_Sched();
            }
        }
        else
        {
            OS_ENTER_CRITICAL();
            OSTCBPrioTbl[prio] = (OS_TCB*)0;/*没创建任务优先级清0 */
            OS_EXIT_CRITICAL();
        }
        return (err);
    }
    OS_EXIT_CRITICAL();
    return (OS_ERR_PRIO_EXIST);
}
#endif

void OS_Dummy()/*空函数 */
{
    /*为了编译 */
}


/*堆栈清空函数 */
#if (OS_TASK_STAT_STK_CHK_EN > 0u) && (OS_TASK_CREATE_EXT_EN > 0u)
void  OS_TaskStkClr(OS_STK* pbos,
    INT32U   size,
    INT16U   opt)
{
    if ((opt & OS_TASK_OPT_STK_CHK) != 0x0000u) 		/* 如果配置了清空堆栈 */
    {
        if ((opt & OS_TASK_OPT_STK_CLR) != 0x0000u) 	/*如果配置了创建任务时清空堆栈 */
        {
#if OS_STK_GROWTH == 1u									/*判断堆栈方向 */
            while (size > 0u)
            {
                size--;
                *pbos++ = (OS_STK)0;
            }
#else
            while (size > 0u)
            {
                size--;
                *pbos-- = (OS_STK)0;
            }
#endif
        }
    }
}

#endif

void OSTaskDelHook(ptcb)             /*删除钩子函数 */
{
    /*删除钩子函数 */
}






#if OS_TASK_DEL_EN > 0u
INT8U OSTaskDel(INT8U prio)
{
#if (OS_FLAG_EN > 0u) && (OS_MAX_FLAGS > 0u)
    OS_FLAG_NODE* pnode;
#endif
    OS_TCB* ptcb;
    if (OSIntNesting > 0u) /*中断中不能删除任务*/
    {
        return (OS_ERR_TASK_DEL_ISR);
    }
    if (prio == OS_TASK_IDLE_PRIO) /*不允许删除空闲任务*/
    {
        return (OS_ERR_TASK_DEL_IDLE);
    }
#if OS_ARG_CHK_EN > 0u          /*如果配置了参数检查*/
    if (prio >= OS_LOWEST_PRIO) /*检查优先级*/
    {
        if (prio != OS_PRIO_SELF) /*如果不是当前任务*/
        {
            return (OS_ERR_PRIO_INVALID);
        }
    }
#endif
    OS_ENTER_CRITICAL();
    if (prio == OS_PRIO_SELF) /*如果是删除自己*/
    {
        prio = OSTCBCur->OSTCBPrio; /*获取自己的优先级*/
    }
    ptcb = OSTCBPrioTbl[prio]; /*获取任务块地址*/
    if (ptcb == (OS_TCB*)0)   /*如果该任务不存在*/
    {
        OS_EXIT_CRITICAL();
        return (OS_ERR_TASK_NOT_EXIST);
    }
    if (ptcb == OS_TCB_RESERVED) /*如果该任务块被保留*/
    {
        OS_EXIT_CRITICAL();
        return (OS_ERR_TASK_DEL);
    }
    /*处理就绪组和就绪表*/
    OSRdyTbl[ptcb->OSTCBY] &= (OS_PRIO)~ptcb->OSTCBBitX;
    if (OSRdyTbl[ptcb->OSTCBY] == 0u)
    {
        OSRdyGrp &= (OS_PRIO)~ptcb->OSTCBBitY;
    }

#if (OS_EVENT_EN) /*如果操作系统使用事件操作*/
    if (ptcb->OSTCBEventPtr != (OS_EVENT*)0)
    {
        OS_EventTaskRemove(ptcb, ptcb->OSTCBEventPtr); /* Remove this task from any event   wait list */
    }
#if (OS_EVENT_MULTI_EN > 0u)                        /*如果允许任务等待多事件*/
    if (ptcb->OSTCBEventMultiPtr != (OS_EVENT**)0) /* Remove this task from any events' wait lists*/
    {
        OS_EventTaskRemoveMulti(ptcb, ptcb->OSTCBEventMultiPtr);
    }
#endif
#endif

#if (OS_FLAG_EN > 0u) && (OS_MAX_FLAGS > 0u) /*如果允许使用时间标志组管理*/
    pnode = ptcb->OSTCBFlagNode;
    if (pnode != (OS_FLAG_NODE*)0) /*如果事件再等待事件标志*/
    {
        OS_FlagUnlink(pnode); /*删除等待队列中这任务的标志，*/
    }
#endif

    ptcb->OSTCBDly = 0u;                   /*如果任务在等待延时事件到，那就不需要等待了*/
    ptcb->OSTCBStat = OS_STAT_RDY;         /*去掉等待等标志*/
    ptcb->OSTCBStatPend = OS_STAT_PEND_OK; /*所有等待都取消了*/
    if (OSLockNesting < 255u)
    {
        OSLockNesting++; /*强行将调度器上一次锁*/
    }
    OS_EXIT_CRITICAL();
    OS_Dummy();          /*空函数，给中断一定的时间*/
    OS_ENTER_CRITICAL(); /*禁止中断*/
    if (OSLockNesting > 0u)
    {
        OSLockNesting--; /*还原调度器*/
    }
    OSTaskDelHook(ptcb);              /*删除钩子函数*/
    OSTaskCtr--;                      /*任务数-1*/
    OSTCBPrioTbl[prio] = (OS_TCB*)0; /*任务优先级指针表清0*/
                                      /*把控制块从就绪链表上拿到空闲链表上*/
    if (ptcb->OSTCBPrev == (OS_TCB*)0)
    {
        ptcb->OSTCBNext->OSTCBPrev = (OS_TCB*)0;
        OSTCBList = ptcb->OSTCBNext;
    }
    else
    {
        ptcb->OSTCBPrev->OSTCBNext = ptcb->OSTCBNext;
        ptcb->OSTCBNext->OSTCBPrev = ptcb->OSTCBPrev;
    }
    ptcb->OSTCBNext = OSTCBFreeList; /* Return TCB to free TCB list                 */
    OSTCBFreeList = ptcb;
#if OS_TASK_NAME_EN > 0u
    ptcb->OSTCBTaskName = (INT8U*)(void*)"?";
#endif
    OS_EXIT_CRITICAL();       /*离开临界区，打开中断 */
    if (OSRunning == OS_TRUE) /*如果运行多任务，调度一次 */
    {
        OS_Sched();
    }
    return (OS_ERR_NONE);
}
#endif












#if OS_TASK_DEL_EN > 0u
INT8U  OSTaskDelReq(INT8U prio)
{
    INT8U      stat;
    OS_TCB* ptcb;
    if (prio == OS_TASK_IDLE_PRIO)								/*检查传入的任务优先级是否有效 */
    {
        return (OS_ERR_TASK_DEL_IDLE);
    }
#if OS_ARG_CHK_EN > 0u
    if (prio >= OS_LOWEST_PRIO)									/*继续检查传入的任务优先级是否有效 */
    {
        if (prio != OS_PRIO_SELF)
        {
            return (OS_ERR_PRIO_INVALID);
        }
    }
#endif
    if (prio == OS_PRIO_SELF)									/*如果删除自己 */
    {
        OS_ENTER_CRITICAL();                                    /*进入临界区 */
        stat = OSTCBCur->OSTCBDelReq;                           /*任务控制块中存放了是否有删除请求 */
        OS_EXIT_CRITICAL();
        return (stat);
    }
    OS_ENTER_CRITICAL();										/*进入临界区 */
    ptcb = OSTCBPrioTbl[prio];									/*获取任务控制块地址 */
    if (ptcb == (OS_TCB*)0)
    {
        OS_EXIT_CRITICAL();
        return (OS_ERR_TASK_NOT_EXIST);                         /*不存在返回错误 */
    }
    if (ptcb == OS_TCB_RESERVED)								/*如果任务控制块被保留 */
    {
        OS_EXIT_CRITICAL();
        return (OS_ERR_TASK_DEL);								/*返回错误信息 */
    }
    ptcb->OSTCBDelReq = OS_ERR_TASK_DEL_REQ;                    /*在对方任务控制块上打上删除成功的标志 */
    OS_EXIT_CRITICAL();
    return (OS_ERR_NONE);
}
#endif



#if OS_TASK_SUSPEND_EN > 0u
INT8U  OSTaskSuspend(INT8U prio)
{
    INT8U    self;
    OS_TCB* ptcb;
    INT8U      y;

#if OS_ARG_CHK_EN > 0u
    if (prio == OS_TASK_IDLE_PRIO)				/*不允许阻塞空闲任务*/
    {
        return (OS_ERR_TASK_SUSPEND_IDLE);
    }
    if (prio >= OS_LOWEST_PRIO)					/*优先级无效*/
    {
        if (prio != OS_PRIO_SELF)				/*不是自己*/
        {
            return (OS_ERR_PRIO_INVALID);
        }
    }
#endif
    OS_ENTER_CRITICAL();
    if (prio == OS_PRIO_SELF)					/*如果阻塞自己*/
    {
        prio = OSTCBCur->OSTCBPrio;				/*获取自己真正优先级*/
        self = OS_TRUE;
    }
    else if (prio == OSTCBCur->OSTCBPrio)		/*阻塞任务是当前任务*/
    {
        self = OS_TRUE;
    }
    else
    {
        self = OS_FALSE;
    }
    ptcb = OSTCBPrioTbl[prio];					/*获取当前任务控制块地址*/
    if (ptcb == (OS_TCB*)0)					/*当前控制块不存在*/
    {
        OS_EXIT_CRITICAL();
        return (OS_ERR_TASK_SUSPEND_PRIO);
    }
    if (ptcb == OS_TCB_RESERVED)				/*控制块需要保留*/
    {
        OS_EXIT_CRITICAL();
        return (OS_ERR_TASK_NOT_EXIST);
    }
    /*取消就绪表和就绪组中的标志位，操作一下*/
    y = ptcb->OSTCBY;
    OSRdyTbl[y] &= (OS_PRIO)~ptcb->OSTCBBitX;
    if (OSRdyTbl[y] == 0u)
    {
        OSRdyGrp &= (OS_PRIO)~ptcb->OSTCBBitY;
    }
    ptcb->OSTCBStat |= OS_STAT_SUSPEND;			/*标记任务被挂起了*/
    OS_EXIT_CRITICAL();
    if (self == OS_TRUE)						/*如果挂起的是自己就调度一下 */
    {
        OS_Sched();
    }
    return (OS_ERR_NONE);
}
#endif


#if OS_TASK_SUSPEND_EN > 0u
INT8U  OSTaskResume(INT8U prio)
{
    OS_TCB* ptcb;
#if OS_CRITICAL_METHOD == 3u                                  /* Storage for CPU status register       */
    OS_CPU_SR  cpu_sr = 0u;
#endif
#if OS_ARG_CHK_EN > 0u
    if (prio >= OS_LOWEST_PRIO)
    {
        return (OS_ERR_PRIO_INVALID);
    }
#endif
    OS_ENTER_CRITICAL();
    ptcb = OSTCBPrioTbl[prio];									/*获取控制块地址*/
    if (ptcb == (OS_TCB*)0)									/*控制块不存在*/
    {
        OS_EXIT_CRITICAL();
        return (OS_ERR_TASK_RESUME_PRIO);
    }
    if (ptcb == OS_TCB_RESERVED)								/*控制块被保留*/
    {
        OS_EXIT_CRITICAL();
        return (OS_ERR_TASK_NOT_EXIST);
    }
    if ((ptcb->OSTCBStat & OS_STAT_SUSPEND) != OS_STAT_RDY)		/*恢复的任务不是SUSPEND状态*/
    {
        ptcb->OSTCBStat &= (INT8U)~(INT8U)OS_STAT_SUSPEND;		/*移除SUSPEND状态*/
        if (ptcb->OSTCBStat == OS_STAT_RDY)						/*如果是就绪态*/
        {
            if (ptcb->OSTCBDly == 0u)							/*如果没有延时*/
            {
                OSRdyGrp |= ptcb->OSTCBBitY;		/*设置就绪组*/
                OSRdyTbl[ptcb->OSTCBY] |= ptcb->OSTCBBitX;		/*设置就绪表*/
                OS_EXIT_CRITICAL();
                if (OSRunning == OS_TRUE)						/*调度一下*/
                {
                    OS_Sched();
                }
            }
            else
            {
                OS_EXIT_CRITICAL();
            }
        }
        else
        {
            OS_EXIT_CRITICAL();
        }
        return (OS_ERR_NONE);
    }
    OS_EXIT_CRITICAL();
    return (OS_ERR_TASK_NOT_SUSPENDED);
}
#endif
