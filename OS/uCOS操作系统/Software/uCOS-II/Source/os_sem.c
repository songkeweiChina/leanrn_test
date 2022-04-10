/*
*********************************************************************************************************
*                                                uC/OS-II
*                                          The Real-Time Kernel
*                                          SEMAPHORE MANAGEMENT
*
*                              (c) Copyright 1992-2009, Micrium, Weston, FL
*                                           All Rights Reserved
*
* File    : OS_SEM.C
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

#if OS_SEM_EN > 0u
/*$PAGE*/
/*
*********************************************************************************************************
*                                           ACCEPT SEMAPHORE
*
* Description: This function checks the semaphore to see if a resource is available or, if an event
*              occurred.  Unlike OSSemPend(), OSSemAccept() does not suspend the calling task if the
*              resource is not available or the event did not occur.
*
* Arguments  : pevent     is a pointer to the event control block
*
* Returns    : >  0       if the resource is available or the event did not occur the semaphore is
*                         decremented to obtain the resource.
*              == 0       if the resource is not available or the event did not occur or,
*                         if 'pevent' is a NULL pointer or,
*                         if you didn't pass a pointer to a semaphore
*********************************************************************************************************
*/

/*无等待的信号量，就是说申请到信号量就拿，申请不到也不阻塞自己，继续往下执行*/
#if OS_SEM_ACCEPT_EN > 0u
INT16U  OSSemAccept (OS_EVENT *pevent)
{
    INT16U     cnt;
#if OS_CRITICAL_METHOD == 3u						/* Allocate storage for CPU status register      */
    OS_CPU_SR  cpu_sr = 0u;
#endif
#if OS_ARG_CHK_EN > 0u
    if(pevent == (OS_EVENT *)0)						/*ECB地址无效*/
	{
        return (0u);
    }
#endif
    if(pevent->OSEventType != OS_EVENT_TYPE_SEM)	/*格式不是信号量*/
	{
        return (0u);
    }
    OS_ENTER_CRITICAL();
    cnt = pevent->OSEventCnt;
    if (cnt > 0u)									/*有信号量就--。没有拉倒*/
	{
        pevent->OSEventCnt--;
    }
    OS_EXIT_CRITICAL();
    return (cnt);									/*返回信号量值*/
}
#endif

/*$PAGE*/
/*
*********************************************************************************************************
*                                           CREATE A SEMAPHORE
*
* Description: This function creates a semaphore.
*
* Arguments  : cnt           is the initial value for the semaphore.  If the value is 0, no resource is
*                            available (or no event has occurred).  You initialize the semaphore to a
*                            non-zero value to specify how many resources are available (e.g. if you have
*                            10 resources, you would initialize the semaphore to 10).
*
* Returns    : != (void *)0  is a pointer to the event control block (OS_EVENT) associated with the
*                            created semaphore
*              == (void *)0  if no event control blocks were available
*********************************************************************************************************
*/

/*创建一个信号量*/
OS_EVENT  *OSSemCreate (INT16U cnt)
{
    OS_EVENT  *pevent;
#if OS_CRITICAL_METHOD == 3u/* Allocate storage for CPU status register */
    OS_CPU_SR  cpu_sr = 0u;
#endif
#ifdef OS_SAFETY_CRITICAL_IEC61508
    if (OSSafetyCriticalStartFlag == OS_TRUE)
	{
        OS_SAFETY_CRITICAL_EXCEPTION();
    }
#endif
    if (OSIntNesting > 0u)												/*中断中不允许创建信号量*/
	{
        return ((OS_EVENT *)0);
    }
    OS_ENTER_CRITICAL();
    pevent = OSEventFreeList;											/*获取一个ECB*/
    if (OSEventFreeList != (OS_EVENT *)0)
	{
        OSEventFreeList = (OS_EVENT *)OSEventFreeList->OSEventPtr;		/*OSEventFreeList指向下一个*/
    }
    OS_EXIT_CRITICAL();
    if (pevent != (OS_EVENT *)0)
	{
        pevent->OSEventType    = OS_EVENT_TYPE_SEM;
        pevent->OSEventCnt     = cnt;
        pevent->OSEventPtr     = (void *)0;
#if OS_EVENT_NAME_EN > 0u
        pevent->OSEventName    = (INT8U *)(void *)"?";					/*这四句，配置ECB*/
#endif
        OS_EventWaitListInit(pevent);									/*清等待时间组表*/
    }
    return (pevent);
}

/*
*********************************************************************************************************
*                                         DELETE A SEMAPHORE
*
* Description: This function deletes a semaphore and readies all tasks pending on the semaphore.
*
* Arguments  : pevent        is a pointer to the event control block associated with the desired
*                            semaphore.
*
*              opt           determines delete options as follows:
*                            opt == OS_DEL_NO_PEND   Delete semaphore ONLY if no task pending
*                            opt == OS_DEL_ALWAYS    Deletes the semaphore even if tasks are waiting.
*                                                    In this case, all the tasks pending will be readied.
*
*              perr          is a pointer to an error code that can contain one of the following values:
*                            OS_ERR_NONE             The call was successful and the semaphore was deleted
*                            OS_ERR_DEL_ISR          If you attempted to delete the semaphore from an ISR
*                            OS_ERR_INVALID_OPT      An invalid option was specified
*                            OS_ERR_TASK_WAITING     One or more tasks were waiting on the semaphore
*                            OS_ERR_EVENT_TYPE       If you didn't pass a pointer to a semaphore
*                            OS_ERR_PEVENT_NULL      If 'pevent' is a NULL pointer.
*
* Returns    : pevent        upon error
*              (OS_EVENT *)0 if the semaphore was successfully deleted.
*
* Note(s)    : 1) This function must be used with care.  Tasks that would normally expect the presence of
*                 the semaphore MUST check the return code of OSSemPend().
*              2) OSSemAccept() callers will not know that the intended semaphore has been deleted unless
*                 they check 'pevent' to see that it's a NULL pointer.
*              3) This call can potentially disable interrupts for a long time.  The interrupt disable
*                 time is directly proportional to the number of tasks waiting on the semaphore.
*              4) Because ALL tasks pending on the semaphore will be readied, you MUST be careful in
*                 applications where the semaphore is used for mutual exclusion because the resource(s)
*                 will no longer be guarded by the semaphore.
*********************************************************************************************************
*/

/*删除一个信号量/消息*/
#if OS_SEM_DEL_EN > 0u
OS_EVENT  *OSSemDel (OS_EVENT  *pevent,						/*ECB地址*/
                     INT8U      opt,						/*删除选项*/
                     INT8U     *perr)						/*返回值*/
{
    BOOLEAN    tasks_waiting;								/*有没有任务在等信号量*/
    OS_EVENT  *pevent_return;
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
    if (pevent == (OS_EVENT *)0)							/*如果ECB不存在*/
	{
        *perr = OS_ERR_PEVENT_NULL;
        return (pevent);
    }
#endif
    if (pevent->OSEventType != OS_EVENT_TYPE_SEM)			/*如果类型不符合*/
	{
        *perr = OS_ERR_EVENT_TYPE;
        return (pevent);
    }
    if (OSIntNesting > 0u)									/*如果在中断中*/
	{
        *perr = OS_ERR_DEL_ISR;
        return (pevent);
    }
    OS_ENTER_CRITICAL();
    if (pevent->OSEventGrp != 0u)							/*等待组不是0，表示有任务在等这信号量*/
	{
        tasks_waiting = OS_TRUE;
    }
	else
	{
        tasks_waiting = OS_FALSE;
    }
    switch (opt)
	{
        case OS_DEL_NO_PEND:								/*有任务等信号量就不能删除的情况*/
             if (tasks_waiting == OS_FALSE)
			 {
#if OS_EVENT_NAME_EN > 0u
                 pevent->OSEventName    = (INT8U *)(void *)"?";
#endif
                 pevent->OSEventType    = OS_EVENT_TYPE_UNUSED;
                 pevent->OSEventPtr     = OSEventFreeList;
                 pevent->OSEventCnt     = 0u;
                 OSEventFreeList        = pevent;			/*上面是删除信号量后配置的ECB*/
                 OS_EXIT_CRITICAL();
                 *perr                  = OS_ERR_NONE;
                 pevent_return          = (OS_EVENT *)0;	/*删除成功返回空指针*/
             }
			 else
			 {
                 OS_EXIT_CRITICAL();
                 *perr                  = OS_ERR_TASK_WAITING;
                 pevent_return          = pevent;			/*删除失败，返回当前ECB*/
             }
             break;

        case OS_DEL_ALWAYS:									/*强制删除的情况*/
             while (pevent->OSEventGrp != 0u)				/*有任务在等信号量*/
			 {
                 (void)OS_EventTaskRdy(pevent, (void *)0, OS_STAT_SEM, OS_STAT_PEND_OK);		/*把等信号量的任务就绪*/
             }
#if OS_EVENT_NAME_EN > 0u
             pevent->OSEventName    = (INT8U *)(void *)"?";
#endif
             pevent->OSEventType    = OS_EVENT_TYPE_UNUSED;
             pevent->OSEventPtr     = OSEventFreeList;
             pevent->OSEventCnt     = 0u;
             OSEventFreeList        = pevent;				/*上面是删除信号量后配置的ECB*/
             OS_EXIT_CRITICAL();
             if (tasks_waiting == OS_TRUE)					/*恢复任务后，有任务就绪了*/
			 {
                 OS_Sched();								/*调度一下*/
             }
             *perr                  = OS_ERR_NONE;
             pevent_return          = (OS_EVENT *)0;		/*删除成功返回空指针*/
             break;

        default:											/*传的删除选项opt无效*/
             OS_EXIT_CRITICAL();
             *perr                  = OS_ERR_INVALID_OPT;
             pevent_return          = pevent;
             break;
    }
    return (pevent_return);
}
#endif

/*$PAGE*/
/*
*********************************************************************************************************
*                                           PEND ON SEMAPHORE
*
* Description: This function waits for a semaphore.
*
* Arguments  : pevent        is a pointer to the event control block associated with the desired
*                            semaphore.
*
*              timeout       is an optional timeout period (in clock ticks).  If non-zero, your task will
*                            wait for the resource up to the amount of time specified by this argument.
*                            If you specify 0, however, your task will wait forever at the specified
*                            semaphore or, until the resource becomes available (or the event occurs).
*
*              perr          is a pointer to where an error message will be deposited.  Possible error
*                            messages are:
*
*                            OS_ERR_NONE         The call was successful and your task owns the resource
*                                                or, the event you are waiting for occurred.
*                            OS_ERR_TIMEOUT      The semaphore was not received within the specified
*                                                'timeout'.
*                            OS_ERR_PEND_ABORT   The wait on the semaphore was aborted.
*                            OS_ERR_EVENT_TYPE   If you didn't pass a pointer to a semaphore.
*                            OS_ERR_PEND_ISR     If you called this function from an ISR and the result
*                                                would lead to a suspension.
*                            OS_ERR_PEVENT_NULL  If 'pevent' is a NULL pointer.
*                            OS_ERR_PEND_LOCKED  If you called this function when the scheduler is locked
*
* Returns    : none
*********************************************************************************************************
*/

/*请求一个信号量，等待一个信号量，也就是任务申请一把钥匙*/
void  OSSemPend (OS_EVENT  *pevent,						/*ECB地址*/
                 INT32U     timeout,					/*设定的超时时间*/
                 INT8U     *perr)
{
#if OS_CRITICAL_METHOD == 3u							/* Allocate storage for CPU status register      */
    OS_CPU_SR  cpu_sr = 0u;
#endif
#ifdef OS_SAFETY_CRITICAL
    if (perr == (INT8U *)0)
	{
        OS_SAFETY_CRITICAL_EXCEPTION();
    }
#endif
#if OS_ARG_CHK_EN > 0u
    if (pevent == (OS_EVENT *)0)						/*ECB地址为空*/
	{
        *perr = OS_ERR_PEVENT_NULL;
        return;
    }
#endif
    if (pevent->OSEventType != OS_EVENT_TYPE_SEM)		/*类型无效*/
	{
        *perr = OS_ERR_EVENT_TYPE;
        return;
    }
    if (OSIntNesting > 0u)								/*中断里不可以*/
	{
        *perr = OS_ERR_PEND_ISR;
        return;
    }
    if (OSLockNesting > 0u)								/*调度器上锁了*/
	{
        *perr = OS_ERR_PEND_LOCKED;
        return;
    }
    OS_ENTER_CRITICAL();
    if (pevent->OSEventCnt > 0u)						/*信号量的值必须大于0*/
	{
        pevent->OSEventCnt--;							/*拿走一个信号量*/
        OS_EXIT_CRITICAL();
        *perr = OS_ERR_NONE;
        return;											/*成功请求到信号量了*/
    }
	/*当前没有信号量了，就的等待*/
    OSTCBCur->OSTCBStat     |= OS_STAT_SEM;				/*在当前控制块中打上等待信号量的标记*/
    OSTCBCur->OSTCBStatPend  = OS_STAT_PEND_OK;			/*等待状态赋值*/
    OSTCBCur->OSTCBDly       = timeout;					/*超时时间赋上*/
    OS_EventTaskWait(pevent);							/*让任务等，在ECB中打标记，在就绪组&表中取消就绪的标记*/
    OS_EXIT_CRITICAL();
    OS_Sched();											/*调度一下*/
    OS_ENTER_CRITICAL();
    switch (OSTCBCur->OSTCBStatPend)					/*OS_Sched函数把任务切走了，这是回来的时候*/
	{
        case OS_STAT_PEND_OK:							/*事件发生了*/
             *perr = OS_ERR_NONE;
             break;
        case OS_STAT_PEND_ABORT:						/*事件超时了*/
             *perr = OS_ERR_PEND_ABORT;
             break;
        case OS_STAT_PEND_TO:
        default:
             OS_EventTaskRemove(OSTCBCur, pevent);		/*自己调函数清除ECB中的等待事件组表中的标志*/
             *perr = OS_ERR_TIMEOUT;
             break;
    }
	/*处理一下TCB*/
    OSTCBCur->OSTCBStat          =  OS_STAT_RDY;
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
*                                      ABORT WAITING ON A SEMAPHORE
*
* Description: This function aborts & readies any tasks currently waiting on a semaphore.  This function
*              should be used to fault-abort the wait on the semaphore, rather than to normally signal
*              the semaphore via OSSemPost().
*
* Arguments  : pevent        is a pointer to the event control block associated with the desired
*                            semaphore.
*
*              opt           determines the type of ABORT performed:
*                            OS_PEND_OPT_NONE         ABORT wait for a single task (HPT) waiting on the
*                                                     semaphore
*                            OS_PEND_OPT_BROADCAST    ABORT wait for ALL tasks that are  waiting on the
*                                                     semaphore
*
*              perr          is a pointer to where an error message will be deposited.  Possible error
*                            messages are:
*
*                            OS_ERR_NONE         No tasks were     waiting on the semaphore.
*                            OS_ERR_PEND_ABORT   At least one task waiting on the semaphore was readied
*                                                and informed of the aborted wait; check return value
*                                                for the number of tasks whose wait on the semaphore
*                                                was aborted.
*                            OS_ERR_EVENT_TYPE   If you didn't pass a pointer to a semaphore.
*                            OS_ERR_PEVENT_NULL  If 'pevent' is a NULL pointer.
*
* Returns    : == 0          if no tasks were waiting on the semaphore, or upon error.
*              >  0          if one or more tasks waiting on the semaphore are now readied and informed.
*********************************************************************************************************
*/

/*放弃其他任务等待信号量*/
#if OS_SEM_PEND_ABORT_EN > 0u
INT8U  OSSemPendAbort (OS_EVENT  *pevent,					/*ECB地址*/
                       INT8U      opt,						/*参数*/
                       INT8U     *perr)						/*返回值*/
{
    INT8U      nbr_tasks;
#if OS_CRITICAL_METHOD == 3u								/* Allocate storage for CPU status register      */
    OS_CPU_SR  cpu_sr = 0u;
#endif
#ifdef OS_SAFETY_CRITICAL
    if (perr == (INT8U *)0)
	{
        OS_SAFETY_CRITICAL_EXCEPTION();
    }
#endif

#if OS_ARG_CHK_EN > 0u
    if (pevent == (OS_EVENT *)0)
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
    if (pevent->OSEventGrp != 0u)							/*如果有任务等信号量*/
	{
        nbr_tasks = 0u;
        switch (opt)
		{
            case OS_PEND_OPT_BROADCAST:						/*让所有的任务退出等待*/
                 while (pevent->OSEventGrp != 0u)
				 {
                     (void)OS_EventTaskRdy(pevent, (void *)0, OS_STAT_SEM, OS_STAT_PEND_ABORT);
                     nbr_tasks++;
                 }
                 break;
            case OS_PEND_OPT_NONE:							/*只让最高优先级的退出等待*/
            default:
                 (void)OS_EventTaskRdy(pevent, (void *)0, OS_STAT_SEM, OS_STAT_PEND_ABORT);
                 nbr_tasks++;
                 break;
        }
        OS_EXIT_CRITICAL();
        OS_Sched();											/*调度一下*/
        *perr = OS_ERR_PEND_ABORT;
        return (nbr_tasks);
    }
    OS_EXIT_CRITICAL();
    *perr = OS_ERR_NONE;
    return (0u);
}
#endif

/*$PAGE*/
/*
*********************************************************************************************************
*                                         POST TO A SEMAPHORE
*
* Description: This function signals a semaphore
*
* Arguments  : pevent        is a pointer to the event control block associated with the desired
*                            semaphore.
*
* Returns    : OS_ERR_NONE         The call was successful and the semaphore was signaled.
*              OS_ERR_SEM_OVF      If the semaphore count exceeded its limit.  In other words, you have
*                                  signalled the semaphore more often than you waited on it with either
*                                  OSSemAccept() or OSSemPend().
*              OS_ERR_EVENT_TYPE   If you didn't pass a pointer to a semaphore
*              OS_ERR_PEVENT_NULL  If 'pevent' is a NULL pointer.
*********************************************************************************************************
*/

/*提交一个信号量，发出一个信号量，也就是申请完钥匙还回去*/
INT8U  OSSemPost (OS_EVENT *pevent)
{
#if OS_CRITICAL_METHOD == 3u							/*Allocate storage for CPU status register      */
    OS_CPU_SR  cpu_sr = 0u;
#endif
#if OS_ARG_CHK_EN > 0u
    if (pevent == (OS_EVENT *)0)						/*ECB无效*/
	{
        return (OS_ERR_PEVENT_NULL);
    }
#endif
    if (pevent->OSEventType != OS_EVENT_TYPE_SEM)		/*ECB类型无效*/
	{
        return (OS_ERR_EVENT_TYPE);
    }
    OS_ENTER_CRITICAL();
    if (pevent->OSEventGrp != 0u)						/*有任务在等信号量*/
	{
        (void)OS_EventTaskRdy(pevent, (void *)0, OS_STAT_SEM, OS_STAT_PEND_OK);			/*把任务就绪*/
        OS_EXIT_CRITICAL();
        OS_Sched();										/*调度一下*/
        return (OS_ERR_NONE);
    }
    if (pevent->OSEventCnt < 65535u)					/*如果信号量没超限制*/
	{
        pevent->OSEventCnt++;							/*把信号量还回去*/
        OS_EXIT_CRITICAL();
        return (OS_ERR_NONE);
    }
    OS_EXIT_CRITICAL();
    return (OS_ERR_SEM_OVF);
}

/*$PAGE*/
/*
*********************************************************************************************************
*                                          QUERY A SEMAPHORE
*
* Description: This function obtains information about a semaphore
*
* Arguments  : pevent        is a pointer to the event control block associated with the desired
*                            semaphore
*
*              p_sem_data    is a pointer to a structure that will contain information about the
*                            semaphore.
*
* Returns    : OS_ERR_NONE         The call was successful and the message was sent
*              OS_ERR_EVENT_TYPE   If you are attempting to obtain data from a non semaphore.
*              OS_ERR_PEVENT_NULL  If 'pevent'     is a NULL pointer.
*              OS_ERR_PDATA_NULL   If 'p_sem_data' is a NULL pointer
*********************************************************************************************************
*/
/*查询信号量的信息，就是把ECB中的东西全赋值到os_sem_data这个结构体里面*/
#if OS_SEM_QUERY_EN > 0u
INT8U  OSSemQuery (OS_EVENT     *pevent,
                   OS_SEM_DATA  *p_sem_data)
{
    INT8U       i;
    OS_PRIO    *psrc;
    OS_PRIO    *pdest;
#if OS_CRITICAL_METHOD == 3u								/*Allocate storage for CPU status register */
    OS_CPU_SR   cpu_sr = 0u;
#endif
#if OS_ARG_CHK_EN > 0u
    if (pevent == (OS_EVENT *)0)
	{
        return (OS_ERR_PEVENT_NULL);
    }
    if (p_sem_data == (OS_SEM_DATA *)0)
	{
        return (OS_ERR_PDATA_NULL);
    }
#endif
    if (pevent->OSEventType != OS_EVENT_TYPE_SEM)
	{
        return (OS_ERR_EVENT_TYPE);
    }
    OS_ENTER_CRITICAL();
    p_sem_data->OSEventGrp = pevent->OSEventGrp;			/*复制事件等待组*/
    psrc                   = &pevent->OSEventTbl[0];		/*psrc指向事件等待表*/
    pdest                  = &p_sem_data->OSEventTbl[0];	/*pdest指向拷贝的目的地*/
    for (i = 0u; i < OS_EVENT_TBL_SIZE; i++)				/*把ECB拷贝到p_sem_data*/
	{
        *pdest++ = *psrc++;
    }
    p_sem_data->OSCnt = pevent->OSEventCnt;					/*拷贝信号量的值*/
    OS_EXIT_CRITICAL();
    return (OS_ERR_NONE);
}
#endif

/*$PAGE*/
/*
*********************************************************************************************************
*                                              SET SEMAPHORE
*
* Description: This function sets the semaphore count to the value specified as an argument.  Typically,
*              this value would be 0.
*
*              You would typically use this function when a semaphore is used as a signaling mechanism
*              and, you want to reset the count value.
*
* Arguments  : pevent     is a pointer to the event control block
*
*              cnt        is the new value for the semaphore count.  You would pass 0 to reset the
*                         semaphore count.
*
*              perr       is a pointer to an error code returned by the function as follows:
*
*                            OS_ERR_NONE          The call was successful and the semaphore value was set.
*                            OS_ERR_EVENT_TYPE    If you didn't pass a pointer to a semaphore.
*                            OS_ERR_PEVENT_NULL   If 'pevent' is a NULL pointer.
*                            OS_ERR_TASK_WAITING  If tasks are waiting on the semaphore.
*********************************************************************************************************
*/

/*直接设置信号量的值*/
#if OS_SEM_SET_EN > 0u
void  OSSemSet (OS_EVENT  *pevent,
                INT16U     cnt,							/*要设置信号量的值*/
                INT8U     *perr)
{
#if OS_CRITICAL_METHOD == 3u							/* Allocate storage for CPU status register      */
    OS_CPU_SR  cpu_sr = 0u;
#endif
#ifdef OS_SAFETY_CRITICAL
    if (perr == (INT8U *)0)
	{
        OS_SAFETY_CRITICAL_EXCEPTION();
    }
#endif
#if OS_ARG_CHK_EN > 0u
    if (pevent == (OS_EVENT *)0)
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
    if (pevent->OSEventCnt > 0u)						/*信号量的值原来就>0*/
	{
        pevent->OSEventCnt = cnt;						/*信号量改成新值*/
    }
	else
	{
        if (pevent->OSEventGrp == 0u)					/*如果没任务等待*/
		{
            pevent->OSEventCnt = cnt;					/*信号量改成新值*/
        }
		else
		{
            *perr              = OS_ERR_TASK_WAITING;	/*有任务等待，不允许修改*/
        }
    }
    OS_EXIT_CRITICAL();
}
#endif

#endif                                                /* OS_SEM_EN                                     */
	 	   	  		 			 	    		   		 		 	 	 			 	    		   	 			 	  	 		 				 		  			 		 					 	  	  		      		  	   		      		  	 		 	      		   		 		  	 		 	      		  		  		  
