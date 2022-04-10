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
* If you plan on using  uC/OS-II  in a commercial product you need to contact Micri�m to properly license
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
/*���ȴ������󻥳��ź���*/
#if OS_MUTEX_ACCEPT_EN > 0u
BOOLEAN  OSMutexAccept (OS_EVENT  *pevent,
                        INT8U     *perr)
{
    INT8U      pip;						/*��ʱ���ȼ�*/
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
    pip = (INT8U)(pevent->OSEventCnt >> 8u);			/*��ȡPIP���ȼ�*/
    if ((pevent->OSEventCnt & OS_MUTEX_KEEP_LOWER_8) == OS_MUTEX_AVAILABLE)/*�����ź���û����*/
	{
        pevent->OSEventCnt &= OS_MUTEX_KEEP_UPPER_8;	/*�ڰ�λ��0����ʾռ�û����ź���*/
        pevent->OSEventCnt |= OSTCBCur->OSTCBPrio;		/*����������ȼ��浽OSEventCnt�Ͱ�λ*/
        pevent->OSEventPtr  = (void *)OSTCBCur;			/*OSEventPtrָ��ǰ����TCB*/
        if (OSTCBCur->OSTCBPrio <= pip)					/*PIP���ȼ���*/
		{
            OS_EXIT_CRITICAL();
            *perr = OS_ERR_PIP_LOWER;
        }
		else											/*PIP���ȼ���*/
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
/*����һ�������ź���*/
OS_EVENT  *OSMutexCreate (INT8U   prio,/*���������ź���������������ȼ�*/
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
    if (prio >= OS_LOWEST_PRIO)								/*���ȼ���Ч*/
	{
        *perr = OS_ERR_PRIO_INVALID;
        return ((OS_EVENT *)0);
    }
#endif
    if (OSIntNesting > 0u)									/*������ж�����*/
	{
        *perr = OS_ERR_CREATE_ISR;							/*�ж����治�������ź���*/
        return ((OS_EVENT *)0);
    }
    OS_ENTER_CRITICAL();
    if (OSTCBPrioTbl[prio] != (OS_TCB *)0)					/*�������������ȼ�����*/
	{
        OS_EXIT_CRITICAL();
        *perr = OS_ERR_PRIO_EXIST;							/*�������ȼ����ܴ���*/
        return ((OS_EVENT *)0);
    }
    OSTCBPrioTbl[prio] = OS_TCB_RESERVED;					/*�������ȼ�ռ��*/
    pevent             = OSEventFreeList;					/*��peventָ����е�EBC*/
    if (pevent == (OS_EVENT *)0)							/*���EBC������*/
	{
        OSTCBPrioTbl[prio] = (OS_TCB *)0;					/*�����ȼ�����ȥ*/
        OS_EXIT_CRITICAL();
        *perr              = OS_ERR_PEVENT_NULL;
        return (pevent);
    }
    OSEventFreeList        = (OS_EVENT *)OSEventFreeList->OSEventPtr;				/*����EBC��������*/
    OS_EXIT_CRITICAL();
    pevent->OSEventType    = OS_EVENT_TYPE_MUTEX;			/*����EBC*/
    pevent->OSEventCnt     = (INT16U)((INT16U)prio << 8u) | OS_MUTEX_AVAILABLE;		/*�߰�λ�����ȼ����ڰ�λȫ��1*/
    pevent->OSEventPtr     = (void *)0;
#if OS_EVENT_NAME_EN > 0u
    pevent->OSEventName    = (INT8U *)(void *)"?";
#endif
    OS_EventWaitListInit(pevent);							/*��һ��ECB�еĵȴ���+��*/
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
/*ɾ��һ�������ź���*/
#if OS_MUTEX_DEL_EN > 0u
OS_EVENT  *OSMutexDel (OS_EVENT  *pevent,		/*ECB��ַ*/
                       INT8U      opt,			/*ɾ��ѡ��*/
                       INT8U     *perr)			/*���*/
{
    BOOLEAN    tasks_waiting;								/*�Ƿ��������ٵȸ��ź���*/
    OS_EVENT  *pevent_return;
    INT8U      pip;											/*�����������ȼ�*/
    INT8U      prio;										/*�ź�����û����ʹ��*/
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
    if (pevent->OSEventGrp != 0u)							/*�Ƿ��������ٵȸ��ź���*/
	{
        tasks_waiting = OS_TRUE;
    }
	else
	{
        tasks_waiting = OS_FALSE;
    }
    switch (opt)
	{
        case OS_DEL_NO_PEND:									/*��������ź����Ͳ���ɾ���ź���*/
             if (tasks_waiting == OS_FALSE)						/*û��������ź���*/
			 {
#if OS_EVENT_NAME_EN > 0u
                 pevent->OSEventName = (INT8U *)(void *)"?";
#endif
                 pip                 = (INT8U)(pevent->OSEventCnt >> 8u);		/*ȡ���̳е����ȼ�*/
                 OSTCBPrioTbl[pip]   = (OS_TCB *)0;				/*��һ�����ȼ�ָ���*/
                 pevent->OSEventType = OS_EVENT_TYPE_UNUSED;
                 pevent->OSEventPtr  = OSEventFreeList;			/*EBC�ŵ���ͷ*/
                 pevent->OSEventCnt  = 0u;
                 OSEventFreeList     = pevent;					/*����ָ��ָ���ͷ*/
                 OS_EXIT_CRITICAL();
                 *perr               = OS_ERR_NONE;
                 pevent_return       = (OS_EVENT *)0;			/* Mutex has been deleted                   */
             }
			 else												/*������ȾͲ�ɾ��*/
			 {
                 OS_EXIT_CRITICAL();
                 *perr               = OS_ERR_TASK_WAITING;
                 pevent_return       = pevent;
             }
             break;

        case OS_DEL_ALWAYS:										/*ǿ��ɾ���ź�����������û�������ڵ�*/
             pip  = (INT8U)(pevent->OSEventCnt >> 8u);			/*ȡ���̳е����ȼ�*/
             prio = (INT8U)(pevent->OSEventCnt & OS_MUTEX_KEEP_LOWER_8);/**/
             ptcb = (OS_TCB *)pevent->OSEventPtr;
             if (ptcb != (OS_TCB *)0)							/*������ռ������ź���*/
			 {
                 if (ptcb->OSTCBPrio == pip)					/*���ʹ���˼̳����ȼ�*/
				 {
                     OSMutex_RdyAtPrio(ptcb, prio);				/*����������ȼ��Ļ���*/
                 }
             }
             while (pevent->OSEventGrp != 0u)/*����������ڵȴ��ź���*/
			 {
                 (void)OS_EventTaskRdy(pevent, (void *)0, OS_STAT_MUTEX, OS_STAT_PEND_OK);			/*�����񶼾�����*/
             }
#if OS_EVENT_NAME_EN > 0u
             pevent->OSEventName = (INT8U *)(void *)"?";
#endif
             pip                 = (INT8U)(pevent->OSEventCnt >> 8u);
             OSTCBPrioTbl[pip]   = (OS_TCB *)0;					/*�ͷ����ȼ�ָ���*/
             pevent->OSEventType = OS_EVENT_TYPE_UNUSED;
             pevent->OSEventPtr  = OSEventFreeList;
             pevent->OSEventCnt  = 0u;
             OSEventFreeList     = pevent;						/*����һ��EBC*/
             OS_EXIT_CRITICAL();
             if (tasks_waiting == OS_TRUE)						/*������ȴ��͵���*/
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
/*�ȴ�һ�������ź�����Ҳ��������һ��Կ��*/
void  OSMutexPend (OS_EVENT  *pevent,	/*ECB��ַ*/
                   INT32U     timeout,	/*��ʱʱ��*/
                   INT8U     *perr)		/*����ֵ*/
{
    INT8U      pip;											/*�����������ȼ�*/
    INT8U      mprio;										/*��ռ���ź�������������ȼ�*/
    BOOLEAN    rdy;											/*�����Ƿ����*/
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
    if (pevent == (OS_EVENT *)0)							/*ECB��Ч*/
	{
        *perr = OS_ERR_PEVENT_NULL;
        return;
    }
#endif
    if (pevent->OSEventType != OS_EVENT_TYPE_MUTEX)			/*ECB������Ч*/
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
    pip = (INT8U)(pevent->OSEventCnt >> 8u);				/*��ȡ����������ȼ�*/
    if ((INT8U)(pevent->OSEventCnt & OS_MUTEX_KEEP_LOWER_8) == OS_MUTEX_AVAILABLE)/*�Ͱ�λ��FF��֤���ź���û����*/
	{
        pevent->OSEventCnt &= OS_MUTEX_KEEP_UPPER_8;		/*�Ͱ�λ��0*/
        pevent->OSEventCnt |= OSTCBCur->OSTCBPrio;			/*�߰�λ����µ����ȼ����Ͱ�λ���þɵ����ȼ�*/
        pevent->OSEventPtr  = (void *)OSTCBCur;				/*OSEventPtrָ��ǰ�����TCB*/
        if (OSTCBCur->OSTCBPrio <= pip)						/*�µ����ȼ���ԭ���ĵ�*/
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
	/*����������ռ�õ����*/
    mprio = (INT8U)(pevent->OSEventCnt & OS_MUTEX_KEEP_LOWER_8);	/*��ȡռ�ź�������������ȼ�*/
    ptcb  = (OS_TCB *)(pevent->OSEventPtr);							/*��ȡ�ź��������ߵ�TCB��ַ*/
    if (ptcb->OSTCBPrio > pip)										/*ռ���ź������������ȼ��ȵ����������ȼ���*/
	{
        if (mprio > OSTCBCur->OSTCBPrio)
		{
            y = ptcb->OSTCBY;
            if ((OSRdyTbl[y] & ptcb->OSTCBBitX) != 0u)				/*�ź��������߾�����*/
			{
                OSRdyTbl[y] &= (OS_PRIO)~ptcb->OSTCBBitX;			/*���ź��������߾�����־ȡ��*/
                if (OSRdyTbl[y] == 0u)
				{
                    OSRdyGrp &= (OS_PRIO)~ptcb->OSTCBBitY;
                }
                rdy = OS_TRUE;
            }
			else													/*�����ź���������û����*/
			{
                pevent2 = ptcb->OSTCBEventPtr;						/*pevent2ָ�򻥳��ź�����EBC*/
                if (pevent2 != (OS_EVENT *)0)						/*������ȴ��������ɾ����ǰ����*/
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
            ptcb->OSTCBPrio = pip;									/*�����ź������ߵ����ȼ�*/
#if OS_LOWEST_PRIO <= 63u
            ptcb->OSTCBY    = (INT8U)( ptcb->OSTCBPrio >> 3u);
            ptcb->OSTCBX    = (INT8U)( ptcb->OSTCBPrio & 0x07u);
#else
            ptcb->OSTCBY    = (INT8U)((INT8U)(ptcb->OSTCBPrio >> 4u) & 0xFFu);
            ptcb->OSTCBX    = (INT8U)( ptcb->OSTCBPrio & 0x0Fu);
#endif
            ptcb->OSTCBBitY = (OS_PRIO)(1uL << ptcb->OSTCBY);
            ptcb->OSTCBBitX = (OS_PRIO)(1uL << ptcb->OSTCBX);
            if (rdy == OS_TRUE)										/*�ź��������߾����ˣ����¸���һ�����ȼ�*/
			{
                OSRdyGrp               |= ptcb->OSTCBBitY;
                OSRdyTbl[ptcb->OSTCBY] |= ptcb->OSTCBBitX;
            }
			else													/*�ź���������û����*/
			{
                pevent2 = ptcb->OSTCBEventPtr;
                if (pevent2 != (OS_EVENT *)0)
				{
                    pevent2->OSEventGrp               |= ptcb->OSTCBBitY;		/*���ź��������߼���ȴ���*/
                    pevent2->OSEventTbl[ptcb->OSTCBY] |= ptcb->OSTCBBitX;
                }
            }
            OSTCBPrioTbl[pip] = ptcb;											/*�����������ȼ�ָ���*/
        }
    }
    OSTCBCur->OSTCBStat     |= OS_STAT_MUTEX;						/*������״̬�óɵȻ����ź���*/
    OSTCBCur->OSTCBStatPend  = OS_STAT_PEND_OK;
    OSTCBCur->OSTCBDly       = timeout;								/*���ó�ʱʱ��*/
    OS_EventTaskWait(pevent);										/*��ǰ�������óɵȴ������ź������ѵ�ǰ��������*/
    OS_EXIT_CRITICAL();
    OS_Sched();														/*����һ��*/
    OS_ENTER_CRITICAL();
    switch (OSTCBCur->OSTCBStatPend)								/*�жϵȴ����*/
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
    OSTCBCur->OSTCBStat          =  OS_STAT_RDY;					/*���������*/
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
/*�ύһ�������ź��������ǰ�Կ�׻���ȥ*/
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
    pip  = (INT8U)(pevent->OSEventCnt >> 8u);						/*��ȡ��ʱ���ȼ�*/
    prio = (INT8U)(pevent->OSEventCnt & OS_MUTEX_KEEP_LOWER_8);		/*��ȡ����ԭ�е����ȼ�*/
    if (OSTCBCur != (OS_TCB *)pevent->OSEventPtr)					/*������Ǳ�����ռ�õ��ź���*/
	{
        OS_EXIT_CRITICAL();
        return (OS_ERR_NOT_MUTEX_OWNER);
    }
    if (OSTCBCur->OSTCBPrio == pip)									/*���ʹ������ʱ��ʱ���ȼ�*/
	{
        OSMutex_RdyAtPrio(OSTCBCur, prio);							/*�ָ������ԭ�����ȼ�*/
    }
    OSTCBPrioTbl[pip] = OS_TCB_RESERVED;							/*��ʱ���ȼ��Ŀ��ƿ���ռ��*/
    if (pevent->OSEventGrp != 0u)									/*�������ڵ��ź���*/
	{
        prio                = OS_EventTaskRdy(pevent, (void *)0, OS_STAT_MUTEX, OS_STAT_PEND_OK);		/*�ѵ��ź������������*/
        pevent->OSEventCnt &= OS_MUTEX_KEEP_UPPER_8;
        pevent->OSEventCnt |= prio;									/*���ź��������ȼ�����*/
        pevent->OSEventPtr  = OSTCBPrioTbl[prio];					/*ָ��TCB*/
        if (prio <= pip)											/*PIP���ȼ���*/
		{
            OS_EXIT_CRITICAL();
            OS_Sched();
            return (OS_ERR_PIP_LOWER);
        }
		else														/*PIP���ȼ���*/
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
/*��ѯ�����ź�����Ϣ*/
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
    p_mutex_data->OSMutexPIP  = (INT8U)(pevent->OSEventCnt >> 8u);						/*��ȡ��ʱ���ȼ�*/
    p_mutex_data->OSOwnerPrio = (INT8U)(pevent->OSEventCnt & OS_MUTEX_KEEP_LOWER_8);	/*��ȡ�ź������������ȼ�*/
    if (p_mutex_data->OSOwnerPrio == 0xFFu)	/*δʹ��*/
	{
        p_mutex_data->OSValue = OS_TRUE;
    }
	else									/*����ռ��*/
	{
        p_mutex_data->OSValue = OS_FALSE;
    }
    p_mutex_data->OSEventGrp  = pevent->OSEventGrp;				/*�����¼��ȴ���ͱ�*/
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
/*�޸��������ȼ�*/
static  void  OSMutex_RdyAtPrio (OS_TCB  *ptcb,	/*TCB��ַ*/
                                 INT8U    prio)	/*Ҫ�޸ĵ����ȼ�*/
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
	 	   	  		 			 	    		   		 		 	 	 			 	    		   	 			 	  	 		 				 		  			 		 					 	  	  		      		  	   		      		  	 		 	      		   		 		  	 		 	      		  		  		  
