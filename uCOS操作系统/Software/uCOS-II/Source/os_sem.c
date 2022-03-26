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
* If you plan on using  uC/OS-II  in a commercial product you need to contact Micri�m to properly license
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
/*�޵ȴ����ź���������˵���뵽�ź������ã����벻��Ҳ�������Լ�����������ִ��*/
#if OS_SEM_ACCEPT_EN > 0u
INT16U  OSSemAccept (OS_EVENT *pevent)
{
    INT16U     cnt;
#if OS_CRITICAL_METHOD == 3u						/* Allocate storage for CPU status register      */
    OS_CPU_SR  cpu_sr = 0u;
#endif
#if OS_ARG_CHK_EN > 0u
    if(pevent == (OS_EVENT *)0)						/*ECB��ַ��Ч*/
	{
        return (0u);
    }
#endif
    if(pevent->OSEventType != OS_EVENT_TYPE_SEM)	/*��ʽ�����ź���*/
	{
        return (0u);
    }
    OS_ENTER_CRITICAL();
    cnt = pevent->OSEventCnt;
    if (cnt > 0u)									/*���ź�����--��û������*/
	{
        pevent->OSEventCnt--;
    }
    OS_EXIT_CRITICAL();
    return (cnt);									/*�����ź���ֵ*/
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
/*����һ���ź���*/
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
    if (OSIntNesting > 0u)												/*�ж��в��������ź���*/
	{
        return ((OS_EVENT *)0);
    }
    OS_ENTER_CRITICAL();
    pevent = OSEventFreeList;											/*��ȡһ��ECB*/
    if (OSEventFreeList != (OS_EVENT *)0)
	{
        OSEventFreeList = (OS_EVENT *)OSEventFreeList->OSEventPtr;		/*OSEventFreeListָ����һ��*/
    }
    OS_EXIT_CRITICAL();
    if (pevent != (OS_EVENT *)0)
	{
        pevent->OSEventType    = OS_EVENT_TYPE_SEM;
        pevent->OSEventCnt     = cnt;
        pevent->OSEventPtr     = (void *)0;
#if OS_EVENT_NAME_EN > 0u
        pevent->OSEventName    = (INT8U *)(void *)"?";					/*���ľ䣬����ECB*/
#endif
        OS_EventWaitListInit(pevent);									/*��ȴ�ʱ�����*/
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
/*ɾ��һ���ź���/��Ϣ*/
#if OS_SEM_DEL_EN > 0u
OS_EVENT  *OSSemDel (OS_EVENT  *pevent,						/*ECB��ַ*/
                     INT8U      opt,						/*ɾ��ѡ��*/
                     INT8U     *perr)						/*����ֵ*/
{
    BOOLEAN    tasks_waiting;								/*��û�������ڵ��ź���*/
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
    if (pevent == (OS_EVENT *)0)							/*���ECB������*/
	{
        *perr = OS_ERR_PEVENT_NULL;
        return (pevent);
    }
#endif
    if (pevent->OSEventType != OS_EVENT_TYPE_SEM)			/*������Ͳ�����*/
	{
        *perr = OS_ERR_EVENT_TYPE;
        return (pevent);
    }
    if (OSIntNesting > 0u)									/*������ж���*/
	{
        *perr = OS_ERR_DEL_ISR;
        return (pevent);
    }
    OS_ENTER_CRITICAL();
    if (pevent->OSEventGrp != 0u)							/*�ȴ��鲻��0����ʾ�������ڵ����ź���*/
	{
        tasks_waiting = OS_TRUE;
    }
	else
	{
        tasks_waiting = OS_FALSE;
    }
    switch (opt)
	{
        case OS_DEL_NO_PEND:								/*��������ź����Ͳ���ɾ�������*/
             if (tasks_waiting == OS_FALSE)
			 {
#if OS_EVENT_NAME_EN > 0u
                 pevent->OSEventName    = (INT8U *)(void *)"?";
#endif
                 pevent->OSEventType    = OS_EVENT_TYPE_UNUSED;
                 pevent->OSEventPtr     = OSEventFreeList;
                 pevent->OSEventCnt     = 0u;
                 OSEventFreeList        = pevent;			/*������ɾ���ź��������õ�ECB*/
                 OS_EXIT_CRITICAL();
                 *perr                  = OS_ERR_NONE;
                 pevent_return          = (OS_EVENT *)0;	/*ɾ���ɹ����ؿ�ָ��*/
             }
			 else
			 {
                 OS_EXIT_CRITICAL();
                 *perr                  = OS_ERR_TASK_WAITING;
                 pevent_return          = pevent;			/*ɾ��ʧ�ܣ����ص�ǰECB*/
             }
             break;

        case OS_DEL_ALWAYS:									/*ǿ��ɾ�������*/
             while (pevent->OSEventGrp != 0u)				/*�������ڵ��ź���*/
			 {
                 (void)OS_EventTaskRdy(pevent, (void *)0, OS_STAT_SEM, OS_STAT_PEND_OK);		/*�ѵ��ź������������*/
             }
#if OS_EVENT_NAME_EN > 0u
             pevent->OSEventName    = (INT8U *)(void *)"?";
#endif
             pevent->OSEventType    = OS_EVENT_TYPE_UNUSED;
             pevent->OSEventPtr     = OSEventFreeList;
             pevent->OSEventCnt     = 0u;
             OSEventFreeList        = pevent;				/*������ɾ���ź��������õ�ECB*/
             OS_EXIT_CRITICAL();
             if (tasks_waiting == OS_TRUE)					/*�ָ�����������������*/
			 {
                 OS_Sched();								/*����һ��*/
             }
             *perr                  = OS_ERR_NONE;
             pevent_return          = (OS_EVENT *)0;		/*ɾ���ɹ����ؿ�ָ��*/
             break;

        default:											/*����ɾ��ѡ��opt��Ч*/
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
/*����һ���ź������ȴ�һ���ź�����Ҳ������������һ��Կ��*/
void  OSSemPend (OS_EVENT  *pevent,						/*ECB��ַ*/
                 INT32U     timeout,					/*�趨�ĳ�ʱʱ��*/
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
    if (pevent == (OS_EVENT *)0)						/*ECB��ַΪ��*/
	{
        *perr = OS_ERR_PEVENT_NULL;
        return;
    }
#endif
    if (pevent->OSEventType != OS_EVENT_TYPE_SEM)		/*������Ч*/
	{
        *perr = OS_ERR_EVENT_TYPE;
        return;
    }
    if (OSIntNesting > 0u)								/*�ж��ﲻ����*/
	{
        *perr = OS_ERR_PEND_ISR;
        return;
    }
    if (OSLockNesting > 0u)								/*������������*/
	{
        *perr = OS_ERR_PEND_LOCKED;
        return;
    }
    OS_ENTER_CRITICAL();
    if (pevent->OSEventCnt > 0u)						/*�ź�����ֵ�������0*/
	{
        pevent->OSEventCnt--;							/*����һ���ź���*/
        OS_EXIT_CRITICAL();
        *perr = OS_ERR_NONE;
        return;											/*�ɹ������ź�����*/
    }
	/*��ǰû���ź����ˣ��͵ĵȴ�*/
    OSTCBCur->OSTCBStat     |= OS_STAT_SEM;				/*�ڵ�ǰ���ƿ��д��ϵȴ��ź����ı��*/
    OSTCBCur->OSTCBStatPend  = OS_STAT_PEND_OK;			/*�ȴ�״̬��ֵ*/
    OSTCBCur->OSTCBDly       = timeout;					/*��ʱʱ�丳��*/
    OS_EventTaskWait(pevent);							/*������ȣ���ECB�д��ǣ��ھ�����&����ȡ�������ı��*/
    OS_EXIT_CRITICAL();
    OS_Sched();											/*����һ��*/
    OS_ENTER_CRITICAL();
    switch (OSTCBCur->OSTCBStatPend)					/*OS_Sched���������������ˣ����ǻ�����ʱ��*/
	{
        case OS_STAT_PEND_OK:							/*�¼�������*/
             *perr = OS_ERR_NONE;
             break;
        case OS_STAT_PEND_ABORT:						/*�¼���ʱ��*/
             *perr = OS_ERR_PEND_ABORT;
             break;
        case OS_STAT_PEND_TO:
        default:
             OS_EventTaskRemove(OSTCBCur, pevent);		/*�Լ����������ECB�еĵȴ��¼�����еı�־*/
             *perr = OS_ERR_TIMEOUT;
             break;
    }
	/*����һ��TCB*/
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
/*������������ȴ��ź���*/
#if OS_SEM_PEND_ABORT_EN > 0u
INT8U  OSSemPendAbort (OS_EVENT  *pevent,					/*ECB��ַ*/
                       INT8U      opt,						/*����*/
                       INT8U     *perr)						/*����ֵ*/
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
    if (pevent->OSEventGrp != 0u)							/*�����������ź���*/
	{
        nbr_tasks = 0u;
        switch (opt)
		{
            case OS_PEND_OPT_BROADCAST:						/*�����е������˳��ȴ�*/
                 while (pevent->OSEventGrp != 0u)
				 {
                     (void)OS_EventTaskRdy(pevent, (void *)0, OS_STAT_SEM, OS_STAT_PEND_ABORT);
                     nbr_tasks++;
                 }
                 break;
            case OS_PEND_OPT_NONE:							/*ֻ��������ȼ����˳��ȴ�*/
            default:
                 (void)OS_EventTaskRdy(pevent, (void *)0, OS_STAT_SEM, OS_STAT_PEND_ABORT);
                 nbr_tasks++;
                 break;
        }
        OS_EXIT_CRITICAL();
        OS_Sched();											/*����һ��*/
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
/*�ύһ���ź���������һ���ź�����Ҳ����������Կ�׻���ȥ*/
INT8U  OSSemPost (OS_EVENT *pevent)
{
#if OS_CRITICAL_METHOD == 3u							/*Allocate storage for CPU status register      */
    OS_CPU_SR  cpu_sr = 0u;
#endif
#if OS_ARG_CHK_EN > 0u
    if (pevent == (OS_EVENT *)0)						/*ECB��Ч*/
	{
        return (OS_ERR_PEVENT_NULL);
    }
#endif
    if (pevent->OSEventType != OS_EVENT_TYPE_SEM)		/*ECB������Ч*/
	{
        return (OS_ERR_EVENT_TYPE);
    }
    OS_ENTER_CRITICAL();
    if (pevent->OSEventGrp != 0u)						/*�������ڵ��ź���*/
	{
        (void)OS_EventTaskRdy(pevent, (void *)0, OS_STAT_SEM, OS_STAT_PEND_OK);			/*���������*/
        OS_EXIT_CRITICAL();
        OS_Sched();										/*����һ��*/
        return (OS_ERR_NONE);
    }
    if (pevent->OSEventCnt < 65535u)					/*����ź���û������*/
	{
        pevent->OSEventCnt++;							/*���ź�������ȥ*/
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
/*��ѯ�ź�������Ϣ�����ǰ�ECB�еĶ���ȫ��ֵ��os_sem_data����ṹ������*/
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
    p_sem_data->OSEventGrp = pevent->OSEventGrp;			/*�����¼��ȴ���*/
    psrc                   = &pevent->OSEventTbl[0];		/*psrcָ���¼��ȴ���*/
    pdest                  = &p_sem_data->OSEventTbl[0];	/*pdestָ�򿽱���Ŀ�ĵ�*/
    for (i = 0u; i < OS_EVENT_TBL_SIZE; i++)				/*��ECB������p_sem_data*/
	{
        *pdest++ = *psrc++;
    }
    p_sem_data->OSCnt = pevent->OSEventCnt;					/*�����ź�����ֵ*/
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
/*ֱ�������ź�����ֵ*/
#if OS_SEM_SET_EN > 0u
void  OSSemSet (OS_EVENT  *pevent,
                INT16U     cnt,							/*Ҫ�����ź�����ֵ*/
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
    if (pevent->OSEventCnt > 0u)						/*�ź�����ֵԭ����>0*/
	{
        pevent->OSEventCnt = cnt;						/*�ź����ĳ���ֵ*/
    }
	else
	{
        if (pevent->OSEventGrp == 0u)					/*���û����ȴ�*/
		{
            pevent->OSEventCnt = cnt;					/*�ź����ĳ���ֵ*/
        }
		else
		{
            *perr              = OS_ERR_TASK_WAITING;	/*������ȴ����������޸�*/
        }
    }
    OS_EXIT_CRITICAL();
}
#endif

#endif                                                /* OS_SEM_EN                                     */
	 	   	  		 			 	    		   		 		 	 	 			 	    		   	 			 	  	 		 				 		  			 		 					 	  	  		      		  	   		      		  	 		 	      		   		 		  	 		 	      		  		  		  
