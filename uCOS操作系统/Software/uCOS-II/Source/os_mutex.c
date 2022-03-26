/*
*********************************************************************************************************
*                                                uC/OS-II
*                                          The Real-Time Kernel
*                                  MUTUAL EXCLUSION SEMAPHORE MANAGEMENT
*
*                              (c) Copyright 1992-2009, Micrium, Weston, FL
*                                           All Rights Reserved
*
* File    : OS_MUTEX.C
* By      : Jean J. Labrosse
* Version : V2.91
*
* LICENSING TERMS:
* ---------------
*   uC/OS-II is provided in source form for FREE evaluation, for educational use or for peaceful research.
* If you plan on using  uC/OS-II  in a commercial product you need to contact Micri祄 to properly license
* its use in your product. We provide ALL the source code for your convenience and to help you experience
* uC/OS-II.   The fact that the  source is provided does  NOT  mean that you can use it without  paying a
* licensing fee.
*********************************************************************************************************
*/

#ifndef  OS_MASTER_FILE
#include <ucos_ii.h>
#endif


#if OS_MUTEX_EN > 0u
/*
*********************************************************************************************************
*                                            LOCAL CONSTANTS
*********************************************************************************************************
*/

#define  OS_MUTEX_KEEP_LOWER_8   ((INT16U)0x00FFu)
#define  OS_MUTEX_KEEP_UPPER_8   ((INT16U)0xFF00u)

#define  OS_MUTEX_AVAILABLE      ((INT16U)0x00FFu)

/*
*********************************************************************************************************
*                                            LOCAL CONSTANTS
*********************************************************************************************************
*/

static  void  OSMutex_RdyAtPrio(OS_TCB *ptcb, INT8U prio);

/*$PAGE*/
/*
*********************************************************************************************************
*                                   ACCEPT MUTUAL EXCLUSION SEMAPHORE
*
* Description: This  function checks the mutual exclusion semaphore to see if a resource is available.
*              Unlike OSMutexPend(), OSMutexAccept() does not suspend the calling task if the resource is
*              not available or the event did not occur.
*
* Arguments  : pevent     is a pointer to the event control block
*
*              perr       is a pointer to an error code which will be returned to your application:
*                            OS_ERR_NONE         if the call was successful.
*                            OS_ERR_EVENT_TYPE   if 'pevent' is not a pointer to a mutex
*                            OS_ERR_PEVENT_NULL  'pevent' is a NULL pointer
*                            OS_ERR_PEND_ISR     if you called this function from an ISR
*                            OS_ERR_PIP_LOWER    If the priority of the task that owns the Mutex is
*                                                HIGHER (i.e. a lower number) than the PIP.  This error
*                                                indicates that you did not set the PIP higher (lower
*                                                number) than ALL the tasks that compete for the Mutex.
*                                                Unfortunately, this is something that could not be
*                                                detected when the Mutex is created because we don't know
*                                                what tasks will be using the Mutex.
*
* Returns    : == OS_TRUE    if the resource is available, the mutual exclusion semaphore is acquired
*              == OS_FALSE   a) if the resource is not available
*                            b) you didn't pass a pointer to a mutual exclusion semaphore
*                            c) you called this function from an ISR
*
* Warning(s) : This function CANNOT be called from an ISR because mutual exclusion semaphores are
*              intended to be used by tasks only.
*********************************************************************************************************
*/
/*不等待的请求互斥信号量*/
#if OS_MUTEX_ACCEPT_EN > 0u
BOOLEAN  OSMutexAccept (OS_EVENT  *pevent,
                        INT8U     *perr)
{
    INT8U      pip;						/*临时优先级*/
#if OS_CRITICAL_METHOD == 3u			/*Allocate storage for CPU status register     */
    OS_CPU_SR  cpu_sr = 0u;
#endif
#ifdef OS_SAFETY_CRITICAL
    if (perr == (INT8U *)0)
	{
        OS_SAFETY_CRITICAL_EXCEPTION();
    }
#endif
#if OS_ARG_CHK_EN > 0u
    if (pevent == (OS_EVENT *)0)		/*Validate 'pevent'                            */
	{
        *perr = OS_ERR_PEVENT_NULL;
        return (OS_FALSE);
    }
#endif
    if (pevent->OSEventType != OS_EVENT_TYPE_MUTEX)		/*Validate event block type                    */
	{
        *perr = OS_ERR_EVENT_TYPE;
        return (OS_FALSE);
    }
    if (OSIntNesting > 0u)								/*Make sure it's not called from an ISR        */
	{
        *perr = OS_ERR_PEND_ISR;
        return (OS_FALSE);
    }
    OS_ENTER_CRITICAL();								/*Get value (0 or 1) of Mutex                  */
    pip = (INT8U)(pevent->OSEventCnt >> 8u);			/*获取PIP优先级*/
    if ((pevent->OSEventCnt & OS_MUTEX_KEEP_LOWER_8) == OS_MUTEX_AVAILABLE)/*互斥信号量没人用*/
	{
        pevent->OSEventCnt &= OS_MUTEX_KEEP_UPPER_8;	/*第八位清0，表示占用互斥信号量*/
        pevent->OSEventCnt |= OSTCBCur->OSTCBPrio;		/*本任务的优先级存到OSEventCnt低八位*/
        pevent->OSEventPtr  = (void *)OSTCBCur;			/*OSEventPtr指向当前任务TCB*/
        if (OSTCBCur->OSTCBPrio <= pip)					/*PIP优先级低*/
		{
            OS_EXIT_CRITICAL();
            *perr = OS_ERR_PIP_LOWER;
        }
		else											/*PIP优先级高*/
		{
            OS_EXIT_CRITICAL();
            *perr = OS_ERR_NONE;
        }
        return (OS_TRUE);
    }
    OS_EXIT_CRITICAL();
    *perr = OS_ERR_NONE;
    return (OS_FALSE);
}
#endif

/*$PAGE*/
/*
*********************************************************************************************************
*                                  CREATE A MUTUAL EXCLUSION SEMAPHORE
*
* Description: This function creates a mutual exclusion semaphore.
*
* Arguments  : prio          is the priority to use when accessing the mutual exclusion semaphore.  In
*                            other words, when the semaphore is acquired and a higher priority task
*                            attempts to obtain the semaphore then the priority of the task owning the
*                            semaphore is raised to this priority.  It is assumed that you will specify
*                            a priority that is LOWER in value than ANY of the tasks competing for the
*                            mutex.
*
*              perr          is a pointer to an error code which will be returned to your application:
*                               OS_ERR_NONE         if the call was successful.
*                               OS_ERR_CREATE_ISR   if you attempted to create a MUTEX from an ISR
*                               OS_ERR_PRIO_EXIST   if a task at the priority inheritance priority
*                                                   already exist.
*                               OS_ERR_PEVENT_NULL  No more event control blocks available.
*                               OS_ERR_PRIO_INVALID if the priority you specify is higher that the
*                                                   maximum allowed (i.e. > OS_LOWEST_PRIO)
*
* Returns    : != (void *)0  is a pointer to the event control clock (OS_EVENT) associated with the
*                            created mutex.
*              == (void *)0  if an error is detected.
*
* Note(s)    : 1) The LEAST significant 8 bits of '.OSEventCnt' are used to hold the priority number
*                 of the task owning the mutex or 0xFF if no task owns the mutex.
*
*              2) The MOST  significant 8 bits of '.OSEventCnt' are used to hold the priority number
*                 to use to reduce priority inversion.
*********************************************************************************************************
*/
/*创建一个互斥信号量*/
OS_EVENT  *OSMutexCreate (INT8U   prio,/*创建互斥信号量后调整到的优先级*/
                          INT8U  *perr)
{
    OS_EVENT  *pevent;
#if OS_CRITICAL_METHOD == 3u								/* Allocate storage for CPU status register */
    OS_CPU_SR  cpu_sr = 0u;
#endif
#ifdef OS_SAFETY_CRITICAL
    if (perr == (INT8U *)0)
	{
        OS_SAFETY_CRITICAL_EXCEPTION();
    }
#endif
#ifdef OS_SAFETY_CRITICAL_IEC61508
    if (OSSafetyCriticalStartFlag == OS_TRUE)
	{
        OS_SAFETY_CRITICAL_EXCEPTION();
    }
#endif
#if OS_ARG_CHK_EN > 0u
    if (prio >= OS_LOWEST_PRIO)								/*优先级无效*/
	{
        *perr = OS_ERR_PRIO_INVALID;
        return ((OS_EVENT *)0);
    }
#endif
    if (OSIntNesting > 0u)									/*如果再中断里面*/
	{
        *perr = OS_ERR_CREATE_ISR;							/*中断里面不允许创建信号量*/
        return ((OS_EVENT *)0);
    }
    OS_ENTER_CRITICAL();
    if (OSTCBPrioTbl[prio] != (OS_TCB *)0)					/*如果这个任务优先级存在*/
	{
        OS_EXIT_CRITICAL();
        *perr = OS_ERR_PRIO_EXIST;							/*任务优先级不能存在*/
        return ((OS_EVENT *)0);
    }
    OSTCBPrioTbl[prio] = OS_TCB_RESERVED;					/*把这优先级占上*/
    pevent             = OSEventFreeList;					/*把pevent指向空闲的EBC*/
    if (pevent == (OS_EVENT *)0)							/*如果EBC不存在*/
	{
        OSTCBPrioTbl[prio] = (OS_TCB *)0;					/*把优先级还回去*/
        OS_EXIT_CRITICAL();
        *perr              = OS_ERR_PEVENT_NULL;
        return (pevent);
    }
    OSEventFreeList        = (OS_EVENT *)OSEventFreeList->OSEventPtr;				/*更新EBC空闲链表*/
    OS_EXIT_CRITICAL();
    pevent->OSEventType    = OS_EVENT_TYPE_MUTEX;			/*配置EBC*/
    pevent->OSEventCnt     = (INT16U)((INT16U)prio << 8u) | OS_MUTEX_AVAILABLE;		/*高八位存优先级，第八位全是1*/
    pevent->OSEventPtr     = (void *)0;
#if OS_EVENT_NAME_EN > 0u
    pevent->OSEventName    = (INT8U *)(void *)"?";
#endif
    OS_EventWaitListInit(pevent);							/*清一下ECB中的等待组+表*/
    *perr                  = OS_ERR_NONE;
    return (pevent);
}

/*$PAGE*/
/*
*********************************************************************************************************
*                                          DELETE A MUTEX
*
* Description: This function deletes a mutual exclusion semaphore and readies all tasks pending on the it.
*
* Arguments  : pevent        is a pointer to the event control block associated with the desired mutex.
*
*              opt           determines delete options as follows:
*                            opt == OS_DEL_NO_PEND   Delete mutex ONLY if no task pending
*                            opt == OS_DEL_ALWAYS    Deletes the mutex even if tasks are waiting.
*                                                    In this case, all the tasks pending will be readied.
*
*              perr          is a pointer to an error code that can contain one of the following values:
*                            OS_ERR_NONE             The call was successful and the mutex was deleted
*                            OS_ERR_DEL_ISR          If you attempted to delete the MUTEX from an ISR
*                            OS_ERR_INVALID_OPT      An invalid option was specified
*                            OS_ERR_TASK_WAITING     One or more tasks were waiting on the mutex
*                            OS_ERR_EVENT_TYPE       If you didn't pass a pointer to a mutex
*                            OS_ERR_PEVENT_NULL      If 'pevent' is a NULL pointer.
*
* Returns    : pevent        upon error
*              (OS_EVENT *)0 if the mutex was successfully deleted.
*
* Note(s)    : 1) This function must be used with care.  Tasks that would normally expect the presence of
*                 the mutex MUST check the return code of OSMutexPend().
*
*              2) This call can potentially disable interrupts for a long time.  The interrupt disable
*                 time is directly proportional to the number of tasks waiting on the mutex.
*
*              3) Because ALL tasks pending on the mutex will be readied, you MUST be careful because the
*                 resource(s) will no longer be guarded by the mutex.
*
*              4) IMPORTANT: In the 'OS_DEL_ALWAYS' case, we assume that the owner of the Mutex (if there
*                            is one) is ready-to-run and is thus NOT pending on another kernel object or
*                            has delayed itself.  In other words, if a task owns the mutex being deleted,
*                            that task will be made ready-to-run at its original priority.
*********************************************************************************************************
*/
/*删除一个互斥信号量*/
#if OS_MUTEX_DEL_EN > 0u
OS_EVENT  *OSMutexDel (OS_EVENT  *pevent,		/*ECB地址*/
                       INT8U      opt,			/*删除选项*/
                       INT8U     *perr)			/*结果*/
{
    BOOLEAN    tasks_waiting;								/*是否有任务再等该信号量*/
    OS_EVENT  *pevent_return;
    INT8U      pip;											/*调整到的优先级*/
    INT8U      prio;										/*信号量有没有人使用*/
    OS_TCB    *ptcb;
#if OS_CRITICAL_METHOD == 3u								/* Allocate storage for CPU status register */
    OS_CPU_SR  cpu_sr = 0u;
#endif
#ifdef OS_SAFETY_CRITICAL
    if (perr == (INT8U *)0)
	{
        OS_SAFETY_CRITICAL_EXCEPTION();
    }
#endif
#if OS_ARG_CHK_EN > 0u
    if (pevent == (OS_EVENT *)0)							/*Validate 'pevent'                        */
	{
        *perr = OS_ERR_PEVENT_NULL;
        return (pevent);
    }
#endif
    if (pevent->OSEventType != OS_EVENT_TYPE_MUTEX)			/* Validate event block type                */
	{
        *perr = OS_ERR_EVENT_TYPE;
        return (pevent);
    }
    if (OSIntNesting > 0u)									/* See if called from ISR ...               */
	{
        *perr = OS_ERR_DEL_ISR;								/* ... can't DELETE from an ISR             */
        return (pevent);
    }
    OS_ENTER_CRITICAL();
    if (pevent->OSEventGrp != 0u)							/*是否有任务再等该信号量*/
	{
        tasks_waiting = OS_TRUE;
    }
	else
	{
        tasks_waiting = OS_FALSE;
    }
    switch (opt)
	{
        case OS_DEL_NO_PEND:									/*有任务等信号量就不能删除信号量*/
             if (tasks_waiting == OS_FALSE)						/*没有任务等信号量*/
			 {
#if OS_EVENT_NAME_EN > 0u
                 pevent->OSEventName = (INT8U *)(void *)"?";
#endif
                 pip                 = (INT8U)(pevent->OSEventCnt >> 8u);		/*取出继承的优先级*/
                 OSTCBPrioTbl[pip]   = (OS_TCB *)0;				/*清一下优先级指针表*/
                 pevent->OSEventType = OS_EVENT_TYPE_UNUSED;
                 pevent->OSEventPtr  = OSEventFreeList;			/*EBC放到表头*/
                 pevent->OSEventCnt  = 0u;
                 OSEventFreeList     = pevent;					/*空闲指针指向表头*/
                 OS_EXIT_CRITICAL();
                 *perr               = OS_ERR_NONE;
                 pevent_return       = (OS_EVENT *)0;			/* Mutex has been deleted                   */
             }
			 else												/*有任务等就不删除*/
			 {
                 OS_EXIT_CRITICAL();
                 *perr               = OS_ERR_TASK_WAITING;
                 pevent_return       = pevent;
             }
             break;

        case OS_DEL_ALWAYS:										/*强制删除信号量，不管有没有任务在等*/
             pip  = (INT8U)(pevent->OSEventCnt >> 8u);			/*取出继承的优先级*/
             prio = (INT8U)(pevent->OSEventCnt & OS_MUTEX_KEEP_LOWER_8);/**/
             ptcb = (OS_TCB *)pevent->OSEventPtr;
             if (ptcb != (OS_TCB *)0)							/*有任务占用这个信号量*/
			 {
                 if (ptcb->OSTCBPrio == pip)					/*如果使用了继承优先级*/
				 {
                     OSMutex_RdyAtPrio(ptcb, prio);				/*把任务的优先级改回来*/
                 }
             }
             while (pevent->OSEventGrp != 0u)/*如果有任务在等待信号量*/
			 {
                 (void)OS_EventTaskRdy(pevent, (void *)0, OS_STAT_MUTEX, OS_STAT_PEND_OK);			/*把任务都就绪了*/
             }
#if OS_EVENT_NAME_EN > 0u
             pevent->OSEventName = (INT8U *)(void *)"?";
#endif
             pip                 = (INT8U)(pevent->OSEventCnt >> 8u);
             OSTCBPrioTbl[pip]   = (OS_TCB *)0;					/*释放优先级指针表*/
             pevent->OSEventType = OS_EVENT_TYPE_UNUSED;
             pevent->OSEventPtr  = OSEventFreeList;
             pevent->OSEventCnt  = 0u;
             OSEventFreeList     = pevent;						/*处理一下EBC*/
             OS_EXIT_CRITICAL();
             if (tasks_waiting == OS_TRUE)						/*有任务等待就调度*/
			 {
                 OS_Sched();
             }
             *perr         = OS_ERR_NONE;
             pevent_return = (OS_EVENT *)0;
             break;

        default:
             OS_EXIT_CRITICAL();
             *perr         = OS_ERR_INVALID_OPT;
             pevent_return = pevent;
             break;
    }
    return (pevent_return);
}
#endif

/*
*********************************************************************************************************
*                                  PEND ON MUTUAL EXCLUSION SEMAPHORE
*
* Description: This function waits for a mutual exclusion semaphore.
*
* Arguments  : pevent        is a pointer to the event control block associated with the desired
*                            mutex.
*
*              timeout       is an optional timeout period (in clock ticks).  If non-zero, your task will
*                            wait for the resource up to the amount of time specified by this argument.
*                            If you specify 0, however, your task will wait forever at the specified
*                            mutex or, until the resource becomes available.
*
*              perr          is a pointer to where an error message will be deposited.  Possible error
*                            messages are:
*                               OS_ERR_NONE        The call was successful and your task owns the mutex
*                               OS_ERR_TIMEOUT     The mutex was not available within the specified 'timeout'.
*                               OS_ERR_PEND_ABORT  The wait on the mutex was aborted.
*                               OS_ERR_EVENT_TYPE  If you didn't pass a pointer to a mutex
*                               OS_ERR_PEVENT_NULL 'pevent' is a NULL pointer
*                               OS_ERR_PEND_ISR    If you called this function from an ISR and the result
*                                                  would lead to a suspension.
*                               OS_ERR_PIP_LOWER   If the priority of the task that owns the Mutex is
*                                                  HIGHER (i.e. a lower number) than the PIP.  This error
*                                                  indicates that you did not set the PIP higher (lower
*                                                  number) than ALL the tasks that compete for the Mutex.
*                                                  Unfortunately, this is something that could not be
*                                                  detected when the Mutex is created because we don't know
*                                                  what tasks will be using the Mutex.
*                               OS_ERR_PEND_LOCKED If you called this function when the scheduler is locked
*
* Returns    : none
*
* Note(s)    : 1) The task that owns the Mutex MUST NOT pend on any other event while it owns the mutex.
*
*              2) You MUST NOT change the priority of the task that owns the mutex
*********************************************************************************************************
*/
/*等待一个互斥信号量，也就是申请一把钥匙*/
void  OSMutexPend (OS_EVENT  *pevent,	/*ECB地址*/
                   INT32U     timeout,	/*超时时间*/
                   INT8U     *perr)		/*返回值*/
{
    INT8U      pip;											/*存调整完的优先级*/
    INT8U      mprio;										/*存占用信号量的任务的优先级*/
    BOOLEAN    rdy;											/*任务是否就绪*/
    OS_TCB    *ptcb;
    OS_EVENT  *pevent2;
    INT8U      y;
#if OS_CRITICAL_METHOD == 3u								/* Allocate storage for CPU status register */
    OS_CPU_SR  cpu_sr = 0u;
#endif
#ifdef OS_SAFETY_CRITICAL
    if (perr == (INT8U *)0)
	{
        OS_SAFETY_CRITICAL_EXCEPTION();
    }
#endif

#if OS_ARG_CHK_EN > 0u
    if (pevent == (OS_EVENT *)0)							/*ECB无效*/
	{
        *perr = OS_ERR_PEVENT_NULL;
        return;
    }
#endif
    if (pevent->OSEventType != OS_EVENT_TYPE_MUTEX)			/*ECB类型无效*/
	{
        *perr = OS_ERR_EVENT_TYPE;
        return;
    }
    if (OSIntNesting > 0u)
	{
        *perr = OS_ERR_PEND_ISR;
        return;
    }
    if (OSLockNesting > 0u)
	{
        *perr = OS_ERR_PEND_LOCKED;
        return;
    }
    OS_ENTER_CRITICAL();
    pip = (INT8U)(pevent->OSEventCnt >> 8u);				/*获取调整完的优先级*/
    if ((INT8U)(pevent->OSEventCnt & OS_MUTEX_KEEP_LOWER_8) == OS_MUTEX_AVAILABLE)/*低八位是FF，证明信号量没人用*/
	{
        pevent->OSEventCnt &= OS_MUTEX_KEEP_UPPER_8;		/*低八位置0*/
        pevent->OSEventCnt |= OSTCBCur->OSTCBPrio;			/*高八位设成新的优先级，低八位设置旧的优先级*/
        pevent->OSEventPtr  = (void *)OSTCBCur;				/*OSEventPtr指向当前任务的TCB*/
        if (OSTCBCur->OSTCBPrio <= pip)						/*新的优先级比原来的低*/
		{
            OS_EXIT_CRITICAL();
            *perr = OS_ERR_PIP_LOWER;
        }
		else
		{
            OS_EXIT_CRITICAL();
            *perr = OS_ERR_NONE;
        }
        return;
    }
	/*下面是有人占用的情况*/
    mprio = (INT8U)(pevent->OSEventCnt & OS_MUTEX_KEEP_LOWER_8);	/*获取占信号量的任务的优先级*/
    ptcb  = (OS_TCB *)(pevent->OSEventPtr);							/*获取信号量所有者的TCB地址*/
    if (ptcb->OSTCBPrio > pip)										/*占用信号量的任务优先级比调整到的优先级低*/
	{
        if (mprio > OSTCBCur->OSTCBPrio)
		{
            y = ptcb->OSTCBY;
            if ((OSRdyTbl[y] & ptcb->OSTCBBitX) != 0u)				/*信号量所有者就绪了*/
			{
                OSRdyTbl[y] &= (OS_PRIO)~ptcb->OSTCBBitX;			/*把信号量所有者就绪标志取消*/
                if (OSRdyTbl[y] == 0u)
				{
                    OSRdyGrp &= (OS_PRIO)~ptcb->OSTCBBitY;
                }
                rdy = OS_TRUE;
            }
			else													/*互斥信号量所有者没就绪*/
			{
                pevent2 = ptcb->OSTCBEventPtr;						/*pevent2指向互斥信号量的EBC*/
                if (pevent2 != (OS_EVENT *)0)						/*在任务等待表和组中删除当前任务*/
				{
                    y = ptcb->OSTCBY;
                    pevent2->OSEventTbl[y] &= (OS_PRIO)~ptcb->OSTCBBitX;
                    if (pevent2->OSEventTbl[y] == 0u)
					{
                        pevent2->OSEventGrp &= (OS_PRIO)~ptcb->OSTCBBitY;
                    }
                }
                rdy = OS_FALSE;
            }
            ptcb->OSTCBPrio = pip;									/*更新信号所有者的优先级*/
#if OS_LOWEST_PRIO <= 63u
            ptcb->OSTCBY    = (INT8U)( ptcb->OSTCBPrio >> 3u);
            ptcb->OSTCBX    = (INT8U)( ptcb->OSTCBPrio & 0x07u);
#else
            ptcb->OSTCBY    = (INT8U)((INT8U)(ptcb->OSTCBPrio >> 4u) & 0xFFu);
            ptcb->OSTCBX    = (INT8U)( ptcb->OSTCBPrio & 0x0Fu);
#endif
            ptcb->OSTCBBitY = (OS_PRIO)(1uL << ptcb->OSTCBY);
            ptcb->OSTCBBitX = (OS_PRIO)(1uL << ptcb->OSTCBX);
            if (rdy == OS_TRUE)										/*信号量所有者就绪了，重新更新一下优先级*/
			{
                OSRdyGrp               |= ptcb->OSTCBBitY;
                OSRdyTbl[ptcb->OSTCBY] |= ptcb->OSTCBBitX;
            }
			else													/*信号量所有者没就绪*/
			{
                pevent2 = ptcb->OSTCBEventPtr;
                if (pevent2 != (OS_EVENT *)0)
				{
                    pevent2->OSEventGrp               |= ptcb->OSTCBBitY;		/*把信号量所有者加入等待表*/
                    pevent2->OSEventTbl[ptcb->OSTCBY] |= ptcb->OSTCBBitX;
                }
            }
            OSTCBPrioTbl[pip] = ptcb;											/*更新任务优先级指针表*/
        }
    }
    OSTCBCur->OSTCBStat     |= OS_STAT_MUTEX;						/*本任务状态置成等互斥信号量*/
    OSTCBCur->OSTCBStatPend  = OS_STAT_PEND_OK;
    OSTCBCur->OSTCBDly       = timeout;								/*设置超时时间*/
    OS_EventTaskWait(pevent);										/*当前任务设置成等待互斥信号量，把当前任务阻塞*/
    OS_EXIT_CRITICAL();
    OS_Sched();														/*调度一下*/
    OS_ENTER_CRITICAL();
    switch (OSTCBCur->OSTCBStatPend)								/*判断等待结果*/
	{
        case OS_STAT_PEND_OK:
             *perr = OS_ERR_NONE;
             break;

        case OS_STAT_PEND_ABORT:
             *perr = OS_ERR_PEND_ABORT;
             break;

        case OS_STAT_PEND_TO:
        default:
             OS_EventTaskRemove(OSTCBCur, pevent);
             *perr = OS_ERR_TIMEOUT;
             break;
    }
    OSTCBCur->OSTCBStat          =  OS_STAT_RDY;					/*给任务就绪*/
    OSTCBCur->OSTCBStatPend      =  OS_STAT_PEND_OK;
    OSTCBCur->OSTCBEventPtr      = (OS_EVENT  *)0;
#if (OS_EVENT_MULTI_EN > 0u)
    OSTCBCur->OSTCBEventMultiPtr = (OS_EVENT **)0;
#endif
    OS_EXIT_CRITICAL();
}
/*$PAGE*/
/*
*********************************************************************************************************
*                                  POST TO A MUTUAL EXCLUSION SEMAPHORE
*
* Description: This function signals a mutual exclusion semaphore
*
* Arguments  : pevent              is a pointer to the event control block associated with the desired
*                                  mutex.
*
* Returns    : OS_ERR_NONE             The call was successful and the mutex was signaled.
*              OS_ERR_EVENT_TYPE       If you didn't pass a pointer to a mutex
*              OS_ERR_PEVENT_NULL      'pevent' is a NULL pointer
*              OS_ERR_POST_ISR         Attempted to post from an ISR (not valid for MUTEXes)
*              OS_ERR_NOT_MUTEX_OWNER  The task that did the post is NOT the owner of the MUTEX.
*              OS_ERR_PIP_LOWER        If the priority of the new task that owns the Mutex is
*                                      HIGHER (i.e. a lower number) than the PIP.  This error
*                                      indicates that you did not set the PIP higher (lower
*                                      number) than ALL the tasks that compete for the Mutex.
*                                      Unfortunately, this is something that could not be
*                                      detected when the Mutex is created because we don't know
*                                      what tasks will be using the Mutex.
*********************************************************************************************************
*/
/*提交一个互斥信号量，就是把钥匙还回去*/
INT8U  OSMutexPost (OS_EVENT *pevent)
{
    INT8U      pip;
    INT8U      prio;
#if OS_CRITICAL_METHOD == 3u/* Allocate storage for CPU status register      */
    OS_CPU_SR  cpu_sr = 0u;
#endif
    if (OSIntNesting > 0u)
	{
        return (OS_ERR_POST_ISR);
    }
#if OS_ARG_CHK_EN > 0u
    if (pevent == (OS_EVENT *)0)
	{
        return (OS_ERR_PEVENT_NULL);
    }
#endif
    if (pevent->OSEventType != OS_EVENT_TYPE_MUTEX)
	{
        return (OS_ERR_EVENT_TYPE);
    }
    OS_ENTER_CRITICAL();
    pip  = (INT8U)(pevent->OSEventCnt >> 8u);						/*获取临时优先级*/
    prio = (INT8U)(pevent->OSEventCnt & OS_MUTEX_KEEP_LOWER_8);		/*获取任务原有的优先级*/
    if (OSTCBCur != (OS_TCB *)pevent->OSEventPtr)					/*如果不是本任务占用的信号量*/
	{
        OS_EXIT_CRITICAL();
        return (OS_ERR_NOT_MUTEX_OWNER);
    }
    if (OSTCBCur->OSTCBPrio == pip)									/*如果使用了临时临时优先级*/
	{
        OSMutex_RdyAtPrio(OSTCBCur, prio);							/*恢复任务的原来优先级*/
    }
    OSTCBPrioTbl[pip] = OS_TCB_RESERVED;							/*临时优先级的控制块先占着*/
    if (pevent->OSEventGrp != 0u)									/*有任务在等信号量*/
	{
        prio                = OS_EventTaskRdy(pevent, (void *)0, OS_STAT_MUTEX, OS_STAT_PEND_OK);		/*把等信号量的任务就绪*/
        pevent->OSEventCnt &= OS_MUTEX_KEEP_UPPER_8;
        pevent->OSEventCnt |= prio;									/*等信号量的优先级存上*/
        pevent->OSEventPtr  = OSTCBPrioTbl[prio];					/*指向TCB*/
        if (prio <= pip)											/*PIP优先级低*/
		{
            OS_EXIT_CRITICAL();
            OS_Sched();
            return (OS_ERR_PIP_LOWER);
        }
		else														/*PIP优先级高*/
		{
            OS_EXIT_CRITICAL();
            OS_Sched();
            return (OS_ERR_NONE);
        }
    }
    pevent->OSEventCnt |= OS_MUTEX_AVAILABLE;         /* No,  Mutex is now available                   */
    pevent->OSEventPtr  = (void *)0;
    OS_EXIT_CRITICAL();
    return (OS_ERR_NONE);
}
/*$PAGE*/
/*
*********************************************************************************************************
*                                     QUERY A MUTUAL EXCLUSION SEMAPHORE
*
* Description: This function obtains information about a mutex
*
* Arguments  : pevent          is a pointer to the event control block associated with the desired mutex
*
*              p_mutex_data    is a pointer to a structure that will contain information about the mutex
*
* Returns    : OS_ERR_NONE          The call was successful and the message was sent
*              OS_ERR_QUERY_ISR     If you called this function from an ISR
*              OS_ERR_PEVENT_NULL   If 'pevent'       is a NULL pointer
*              OS_ERR_PDATA_NULL    If 'p_mutex_data' is a NULL pointer
*              OS_ERR_EVENT_TYPE    If you are attempting to obtain data from a non mutex.
*********************************************************************************************************
*/
/*查询互斥信号量信息*/
#if OS_MUTEX_QUERY_EN > 0u
INT8U  OSMutexQuery (OS_EVENT       *pevent,
                     OS_MUTEX_DATA  *p_mutex_data)
{
    INT8U       i;
    OS_PRIO    *psrc;
    OS_PRIO    *pdest;
#if OS_CRITICAL_METHOD == 3u
    OS_CPU_SR   cpu_sr = 0u;
#endif
    if (OSIntNesting > 0u)
	{
        return (OS_ERR_QUERY_ISR);
    }
#if OS_ARG_CHK_EN > 0u
    if (pevent == (OS_EVENT *)0)
	{
        return (OS_ERR_PEVENT_NULL);
    }
    if (p_mutex_data == (OS_MUTEX_DATA *)0)
	{
        return (OS_ERR_PDATA_NULL);
    }
#endif
    if (pevent->OSEventType != OS_EVENT_TYPE_MUTEX)
	{
        return (OS_ERR_EVENT_TYPE);
    }
    OS_ENTER_CRITICAL();
    p_mutex_data->OSMutexPIP  = (INT8U)(pevent->OSEventCnt >> 8u);						/*获取临时优先级*/
    p_mutex_data->OSOwnerPrio = (INT8U)(pevent->OSEventCnt & OS_MUTEX_KEEP_LOWER_8);	/*获取信号量所有者优先级*/
    if (p_mutex_data->OSOwnerPrio == 0xFFu)	/*未使用*/
	{
        p_mutex_data->OSValue = OS_TRUE;
    }
	else									/*有人占用*/
	{
        p_mutex_data->OSValue = OS_FALSE;
    }
    p_mutex_data->OSEventGrp  = pevent->OSEventGrp;				/*拷贝事件等待组和表*/
    psrc                      = &pevent->OSEventTbl[0];
    pdest                     = &p_mutex_data->OSEventTbl[0];
    for (i = 0u; i < OS_EVENT_TBL_SIZE; i++)
	{
        *pdest++ = *psrc++;
    }
    OS_EXIT_CRITICAL();
    return (OS_ERR_NONE);
}
#endif

/*$PAGE*/
/*
*********************************************************************************************************
*                                RESTORE A TASK BACK TO ITS ORIGINAL PRIORITY
*
* Description: This function makes a task ready at the specified priority
*
* Arguments  : ptcb            is a pointer to OS_TCB of the task to make ready
*
*              prio            is the desired priority
*
* Returns    : none
*********************************************************************************************************
*/
/*修改任务优先级*/
static  void  OSMutex_RdyAtPrio (OS_TCB  *ptcb,	/*TCB地址*/
                                 INT8U    prio)	/*要修改的优先级*/
{
    INT8U  y;
    y            =  ptcb->OSTCBY;                          /* Remove owner from ready list at 'pip'    */
    OSRdyTbl[y] &= (OS_PRIO)~ptcb->OSTCBBitX;
    if (OSRdyTbl[y] == 0u)
	{
        OSRdyGrp &= (OS_PRIO)~ptcb->OSTCBBitY;
    }
    ptcb->OSTCBPrio         = prio;
    OSPrioCur               = prio;                        /* The current task is now at this priority */
#if OS_LOWEST_PRIO <= 63u
    ptcb->OSTCBY            = (INT8U)((INT8U)(prio >> 3u) & 0x07u);
    ptcb->OSTCBX            = (INT8U)(prio & 0x07u);
#else
    ptcb->OSTCBY            = (INT8U)((INT8U)(prio >> 4u) & 0x0Fu);
    ptcb->OSTCBX            = (INT8U) (prio & 0x0Fu);
#endif
    ptcb->OSTCBBitY         = (OS_PRIO)(1uL << ptcb->OSTCBY);
    ptcb->OSTCBBitX         = (OS_PRIO)(1uL << ptcb->OSTCBX);
    OSRdyGrp               |= ptcb->OSTCBBitY;             /* Make task ready at original priority     */
    OSRdyTbl[ptcb->OSTCBY] |= ptcb->OSTCBBitX;
    OSTCBPrioTbl[prio]      = ptcb;

}


#endif                                                     /* OS_MUTEX_EN                              */
	 	   	  		 			 	    		   		 		 	 	 			 	    		   	 			 	  	 		 				 		  			 		 					 	  	  		      		  	   		      		  	 		 	      		   		 		  	 		 	      		  		  		  
