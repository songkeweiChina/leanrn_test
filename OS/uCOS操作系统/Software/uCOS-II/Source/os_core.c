/*
*********************************************************************************************************
*                                                uC/OS-II
*                                          The Real-Time Kernel
*                                             CORE FUNCTIONS
*
*                              (c) Copyright 1992-2009, Micrium, Weston, FL
*                                           All Rights Reserved
*
* File    : OS_CORE.C
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
#define  OS_GLOBALS
#include <ucos_ii.h>
#endif

/*
*********************************************************************************************************
*                                       PRIORITY RESOLUTION TABLE
*
* Note: Index into table is bit pattern to resolve highest priority
*       Indexed value corresponds to highest priority bit position (i.e. 0..7)
*********************************************************************************************************
*/

INT8U  const  OSUnMapTbl[256] = {/*优先级判定表*/
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

/*$PAGE*/
/*
*********************************************************************************************************
*                                       FUNCTION PROTOTYPES
*********************************************************************************************************
*/

static  void  OS_InitEventList(void);

static  void  OS_InitMisc(void);

static  void  OS_InitRdyList(void);

static  void  OS_InitTaskIdle(void);

#if OS_TASK_STAT_EN > 0u
static  void  OS_InitTaskStat(void);
#endif

static  void  OS_InitTCBList(void);

static  void  OS_SchedNew(void);

/*$PAGE*/
/*
*********************************************************************************************************
*                         GET THE NAME OF A SEMAPHORE, MUTEX, MAILBOX or QUEUE
*
* Description: This function is used to obtain the name assigned to a semaphore, mutex, mailbox or queue.
*
* Arguments  : pevent    is a pointer to the event group.  'pevent' can point either to a semaphore,
*                        a mutex, a mailbox or a queue.  Where this function is concerned, the actual
*                        type is irrelevant.
*
*              pname     is a pointer to a pointer to an ASCII string that will receive the name of the semaphore,
*                        mutex, mailbox or queue.
*
*              perr      is a pointer to an error code that can contain one of the following values:
*
*                        OS_ERR_NONE                if the name was copied to 'pname'
*                        OS_ERR_EVENT_TYPE          if 'pevent' is not pointing to the proper event
*                                                   control block type.
*                        OS_ERR_PNAME_NULL          You passed a NULL pointer for 'pname'
*                        OS_ERR_PEVENT_NULL         if you passed a NULL pointer for 'pevent'
*                        OS_ERR_NAME_GET_ISR        if you are trying to call this function from an ISR
*
* Returns    : The length of the string or 0 if the 'pevent' is a NULL pointer.
*********************************************************************************************************
*/

#if (OS_EVENT_EN) && (OS_EVENT_NAME_EN > 0u)
INT8U  OSEventNameGet (OS_EVENT   *pevent,
                       INT8U     **pname,
                       INT8U      *perr)
{
    INT8U      len;
#if OS_CRITICAL_METHOD == 3u                     /* Allocate storage for CPU status register           */
    OS_CPU_SR  cpu_sr = 0u;
#endif



#ifdef OS_SAFETY_CRITICAL
    if (perr == (INT8U *)0) {
        OS_SAFETY_CRITICAL_EXCEPTION();
    }
#endif

#if OS_ARG_CHK_EN > 0u
    if (pevent == (OS_EVENT *)0) {               /* Is 'pevent' a NULL pointer?                        */
        *perr = OS_ERR_PEVENT_NULL;
        return (0u);
    }
    if (pname == (INT8U **)0) {                   /* Is 'pname' a NULL pointer?                         */
        *perr = OS_ERR_PNAME_NULL;
        return (0u);
    }
#endif
    if (OSIntNesting > 0u) {                     /* See if trying to call from an ISR                  */
        *perr  = OS_ERR_NAME_GET_ISR;
        return (0u);
    }
    switch (pevent->OSEventType) {
        case OS_EVENT_TYPE_SEM:
        case OS_EVENT_TYPE_MUTEX:
        case OS_EVENT_TYPE_MBOX:
        case OS_EVENT_TYPE_Q:
             break;

        default:
             *perr = OS_ERR_EVENT_TYPE;
             return (0u);
    }
    OS_ENTER_CRITICAL();
    *pname = pevent->OSEventName;
    len    = OS_StrLen(*pname);
    OS_EXIT_CRITICAL();
    *perr  = OS_ERR_NONE;
    return (len);
}
#endif

/*$PAGE*/
/*
*********************************************************************************************************
*                         ASSIGN A NAME TO A SEMAPHORE, MUTEX, MAILBOX or QUEUE
*
* Description: This function assigns a name to a semaphore, mutex, mailbox or queue.
*
* Arguments  : pevent    is a pointer to the event group.  'pevent' can point either to a semaphore,
*                        a mutex, a mailbox or a queue.  Where this function is concerned, it doesn't
*                        matter the actual type.
*
*              pname     is a pointer to an ASCII string that will be used as the name of the semaphore,
*                        mutex, mailbox or queue.
*
*              perr      is a pointer to an error code that can contain one of the following values:
*
*                        OS_ERR_NONE                if the requested task is resumed
*                        OS_ERR_EVENT_TYPE          if 'pevent' is not pointing to the proper event
*                                                   control block type.
*                        OS_ERR_PNAME_NULL          You passed a NULL pointer for 'pname'
*                        OS_ERR_PEVENT_NULL         if you passed a NULL pointer for 'pevent'
*                        OS_ERR_NAME_SET_ISR        if you called this function from an ISR
*
* Returns    : None
*********************************************************************************************************
*/

#if (OS_EVENT_EN) && (OS_EVENT_NAME_EN > 0u)
void  OSEventNameSet (OS_EVENT  *pevent,
                      INT8U     *pname,
                      INT8U     *perr)
{
#if OS_CRITICAL_METHOD == 3u                     /* Allocate storage for CPU status register           */
    OS_CPU_SR  cpu_sr = 0u;
#endif



#ifdef OS_SAFETY_CRITICAL
    if (perr == (INT8U *)0) {
        OS_SAFETY_CRITICAL_EXCEPTION();
    }
#endif

#if OS_ARG_CHK_EN > 0u
    if (pevent == (OS_EVENT *)0) {               /* Is 'pevent' a NULL pointer?                        */
        *perr = OS_ERR_PEVENT_NULL;
        return;
    }
    if (pname == (INT8U *)0) {                   /* Is 'pname' a NULL pointer?                         */
        *perr = OS_ERR_PNAME_NULL;
        return;
    }
#endif
    if (OSIntNesting > 0u) {                     /* See if trying to call from an ISR                  */
        *perr = OS_ERR_NAME_SET_ISR;
        return;
    }
    switch (pevent->OSEventType) {
        case OS_EVENT_TYPE_SEM:
        case OS_EVENT_TYPE_MUTEX:
        case OS_EVENT_TYPE_MBOX:
        case OS_EVENT_TYPE_Q:
             break;

        default:
             *perr = OS_ERR_EVENT_TYPE;
             return;
    }
    OS_ENTER_CRITICAL();
    pevent->OSEventName = pname;
    OS_EXIT_CRITICAL();
    *perr = OS_ERR_NONE;
}
#endif

/*$PAGE*/
/*
*********************************************************************************************************
*                                      PEND ON MULTIPLE EVENTS
*
* Description: This function waits for multiple events.  If multiple events are ready at the start of the
*              pend call, then all available events are returned as ready.  If the task must pend on the
*              multiple events, then only the first posted or aborted event is returned as ready.
*
* Arguments  : pevents_pend  is a pointer to a NULL-terminated array of event control blocks to wait for.
*
*              pevents_rdy   is a pointer to an array to return which event control blocks are available
*                            or ready.  The size of the array MUST be greater than or equal to the size
*                            of the 'pevents_pend' array, including terminating NULL.
*
*              pmsgs_rdy     is a pointer to an array to return messages from any available message-type
*                            events.  The size of the array MUST be greater than or equal to the size of
*                            the 'pevents_pend' array, excluding the terminating NULL.  Since NULL
*                            messages are valid messages, this array cannot be NULL-terminated.  Instead,
*                            every available message-type event returns its messages in the 'pmsgs_rdy'
*                            array at the same index as the event is returned in the 'pevents_rdy' array.
*                            All other 'pmsgs_rdy' array indices are filled with NULL messages.
*
*              timeout       is an optional timeout period (in clock ticks).  If non-zero, your task will
*                            wait for the resources up to the amount of time specified by this argument.
*                            If you specify 0, however, your task will wait forever for the specified
*                            events or, until the resources becomes available (or the events occur).
*
*              perr          is a pointer to where an error message will be deposited.  Possible error
*                            messages are:
*
*                            OS_ERR_NONE         The call was successful and your task owns the resources
*                                                or, the events you are waiting for occurred; check the
*                                                'pevents_rdy' array for which events are available.
*                            OS_ERR_PEND_ABORT   The wait on the events was aborted; check the
*                                                'pevents_rdy' array for which events were aborted.
*                            OS_ERR_TIMEOUT      The events were not received within the specified
*                                                'timeout'.
*                            OS_ERR_PEVENT_NULL  If 'pevents_pend', 'pevents_rdy', or 'pmsgs_rdy' is a
*                                                NULL pointer.
*                            OS_ERR_EVENT_TYPE   If you didn't pass a pointer to an array of semaphores,
*                                                mailboxes, and/or queues.
*                            OS_ERR_PEND_ISR     If you called this function from an ISR and the result
*                                                would lead to a suspension.
*                            OS_ERR_PEND_LOCKED  If you called this function when the scheduler is locked.
*
* Returns    : >  0          the number of events returned as ready or aborted.
*              == 0          if no events are returned as ready because of timeout or upon error.
*
* Notes      : 1) a. Validate 'pevents_pend' array as valid OS_EVENTs :
*
*                        semaphores, mailboxes, queues
*
*                 b. Return ALL available events and messages, if any
*
*                 c. Add    current task priority as pending to   each events's wait list
*                      Performed in OS_EventTaskWaitMulti()
*
*                 d. Wait on any of multiple events
*
*                 e. Remove current task priority as pending from each events's wait list
*                      Performed in OS_EventTaskRdy(), if events posted or aborted
*
*                 f. Return any event posted or aborted, if any
*                      else
*                    Return timeout
*
*              2) 'pevents_rdy' initialized to NULL PRIOR to all other validation or function handling in
*                 case of any error(s).
*********************************************************************************************************
*/
/*$PAGE*/
#if ((OS_EVENT_EN) && (OS_EVENT_MULTI_EN > 0u))
INT16U  OSEventPendMulti (OS_EVENT  **pevents_pend,
                          OS_EVENT  **pevents_rdy,
                          void      **pmsgs_rdy,
                          INT32U      timeout,
                          INT8U      *perr)
{
    OS_EVENT  **pevents;
    OS_EVENT   *pevent;
#if ((OS_Q_EN > 0u) && (OS_MAX_QS > 0u))
    OS_Q       *pq;
#endif
    BOOLEAN     events_rdy;
    INT16U      events_rdy_nbr;
    INT8U       events_stat;
#if (OS_CRITICAL_METHOD == 3u)                          /* Allocate storage for CPU status register    */
    OS_CPU_SR   cpu_sr = 0u;
#endif



#ifdef OS_SAFETY_CRITICAL
    if (perr == (INT8U *)0) {
        OS_SAFETY_CRITICAL_EXCEPTION();
    }
#endif

#if (OS_ARG_CHK_EN > 0u)
    if (pevents_pend == (OS_EVENT **)0) {               /* Validate 'pevents_pend'                     */
       *perr =  OS_ERR_PEVENT_NULL;
        return (0u);
    }
    if (*pevents_pend  == (OS_EVENT *)0) {              /* Validate 'pevents_pend'                     */
       *perr =  OS_ERR_PEVENT_NULL;
        return (0u);
    }
    if (pevents_rdy  == (OS_EVENT **)0) {               /* Validate 'pevents_rdy'                      */
       *perr =  OS_ERR_PEVENT_NULL;
        return (0u);
    }
    if (pmsgs_rdy == (void **)0) {                      /* Validate 'pmsgs_rdy'                        */
       *perr =  OS_ERR_PEVENT_NULL;
        return (0u);
    }
#endif

   *pevents_rdy = (OS_EVENT *)0;                        /* Init array to NULL in case of errors        */

    pevents     =  pevents_pend;
    pevent      = *pevents;
    while  (pevent != (OS_EVENT *)0) {
        switch (pevent->OSEventType) {                  /* Validate event block types                  */
#if (OS_SEM_EN  > 0u)
            case OS_EVENT_TYPE_SEM:
                 break;
#endif
#if (OS_MBOX_EN > 0u)
            case OS_EVENT_TYPE_MBOX:
                 break;
#endif
#if ((OS_Q_EN   > 0u) && (OS_MAX_QS > 0u))
            case OS_EVENT_TYPE_Q:
                 break;
#endif

            case OS_EVENT_TYPE_MUTEX:
            case OS_EVENT_TYPE_FLAG:
            default:
                *perr = OS_ERR_EVENT_TYPE;
                 return (0u);
        }
        pevents++;
        pevent = *pevents;
    }

    if (OSIntNesting  > 0u) {                           /* See if called from ISR ...                  */
       *perr =  OS_ERR_PEND_ISR;                        /* ... can't PEND from an ISR                  */
        return (0u);
    }
    if (OSLockNesting > 0u) {                           /* See if called with scheduler locked ...     */
       *perr =  OS_ERR_PEND_LOCKED;                     /* ... can't PEND when locked                  */
        return (0u);
    }

/*$PAGE*/
    OS_ENTER_CRITICAL();
    events_rdy     =  OS_FALSE;
    events_rdy_nbr =  0u;
    events_stat    =  OS_STAT_RDY;
    pevents        =  pevents_pend;
    pevent         = *pevents;
    while (pevent != (OS_EVENT *)0) {                   /* See if any events already available         */
        switch (pevent->OSEventType) {
#if (OS_SEM_EN > 0u)
            case OS_EVENT_TYPE_SEM:
                 if (pevent->OSEventCnt > 0u) {         /* If semaphore count > 0, resource available; */
                     pevent->OSEventCnt--;              /* ... decrement semaphore,                ... */
                    *pevents_rdy++ =  pevent;           /* ... and return available semaphore event    */
                      events_rdy   =  OS_TRUE;
                    *pmsgs_rdy++   = (void *)0;         /* NO message returned  for semaphores         */
                      events_rdy_nbr++;

                 } else {
                      events_stat |=  OS_STAT_SEM;      /* Configure multi-pend for semaphore events   */
                 }
                 break;
#endif

#if (OS_MBOX_EN > 0u)
            case OS_EVENT_TYPE_MBOX:
                 if (pevent->OSEventPtr != (void *)0) { /* If mailbox NOT empty;                   ... */
                                                        /* ... return available message,           ... */
                    *pmsgs_rdy++         = (void *)pevent->OSEventPtr;
                     pevent->OSEventPtr  = (void *)0;
                    *pevents_rdy++       =  pevent;     /* ... and return available mailbox event      */
                      events_rdy         =  OS_TRUE;
                      events_rdy_nbr++;

                 } else {
                      events_stat |= OS_STAT_MBOX;      /* Configure multi-pend for mailbox events     */
                 }
                 break;
#endif

#if ((OS_Q_EN > 0u) && (OS_MAX_QS > 0u))
            case OS_EVENT_TYPE_Q:
                 pq = (OS_Q *)pevent->OSEventPtr;
                 if (pq->OSQEntries > 0u) {             /* If queue NOT empty;                     ... */
                                                        /* ... return available message,           ... */
                    *pmsgs_rdy++ = (void *)*pq->OSQOut++;
                     if (pq->OSQOut == pq->OSQEnd) {    /* If OUT ptr at queue end, ...                */
                         pq->OSQOut  = pq->OSQStart;    /* ... wrap   to queue start                   */
                     }
                     pq->OSQEntries--;                  /* Update number of queue entries              */
                    *pevents_rdy++ = pevent;            /* ... and return available queue event        */
                      events_rdy   = OS_TRUE;
                      events_rdy_nbr++;

                 } else {
                      events_stat |= OS_STAT_Q;         /* Configure multi-pend for queue events       */
                 }
                 break;
#endif

            case OS_EVENT_TYPE_MUTEX:
            case OS_EVENT_TYPE_FLAG:
            default:
                 OS_EXIT_CRITICAL();
                *pevents_rdy = (OS_EVENT *)0;           /* NULL terminate return event array           */
                *perr        =  OS_ERR_EVENT_TYPE;
                 return (events_rdy_nbr);
        }
        pevents++;
        pevent = *pevents;
    }

    if ( events_rdy == OS_TRUE) {                       /* Return any events already available         */
       *pevents_rdy = (OS_EVENT *)0;                    /* NULL terminate return event array           */
        OS_EXIT_CRITICAL();
       *perr        =  OS_ERR_NONE;
        return (events_rdy_nbr);
    }
/*$PAGE*/
                                                        /* Otherwise, must wait until any event occurs */
    OSTCBCur->OSTCBStat     |= events_stat  |           /* Resource not available, ...                 */
                               OS_STAT_MULTI;           /* ... pend on multiple events                 */
    OSTCBCur->OSTCBStatPend  = OS_STAT_PEND_OK;
    OSTCBCur->OSTCBDly       = timeout;                 /* Store pend timeout in TCB                   */
    OS_EventTaskWaitMulti(pevents_pend);                /* Suspend task until events or timeout occurs */

    OS_EXIT_CRITICAL();
    OS_Sched();                                         /* Find next highest priority task ready       */
    OS_ENTER_CRITICAL();

    switch (OSTCBCur->OSTCBStatPend) {                  /* Handle event posted, aborted, or timed-out  */
        case OS_STAT_PEND_OK:
        case OS_STAT_PEND_ABORT:
             pevent = OSTCBCur->OSTCBEventPtr;
             if (pevent != (OS_EVENT *)0) {             /* If task event ptr != NULL, ...              */
                *pevents_rdy++ =  pevent;               /* ... return available event ...              */
                *pevents_rdy   = (OS_EVENT *)0;         /* ... & NULL terminate return event array     */
                  events_rdy_nbr++;

             } else {                                   /* Else NO event available, handle as timeout  */
                 OSTCBCur->OSTCBStatPend = OS_STAT_PEND_TO;
                 OS_EventTaskRemoveMulti(OSTCBCur, pevents_pend);
             }
			 break;

        case OS_STAT_PEND_TO:                           /* If events timed out, ...                    */
        default:                                        /* ... remove task from events' wait lists     */
             OS_EventTaskRemoveMulti(OSTCBCur, pevents_pend);
             break;
    }

    switch (OSTCBCur->OSTCBStatPend) {
        case OS_STAT_PEND_OK:
             switch (pevent->OSEventType) {             /* Return event's message                      */
#if (OS_SEM_EN > 0u)
                 case OS_EVENT_TYPE_SEM:
                     *pmsgs_rdy++ = (void *)0;          /* NO message returned for semaphores          */
                      break;
#endif

#if ((OS_MBOX_EN > 0u) ||                 \
    ((OS_Q_EN    > 0u) && (OS_MAX_QS > 0u)))
                 case OS_EVENT_TYPE_MBOX:
                 case OS_EVENT_TYPE_Q:
                     *pmsgs_rdy++ = (void *)OSTCBCur->OSTCBMsg;     /* Return received message         */
                      break;
#endif

                 case OS_EVENT_TYPE_MUTEX:
                 case OS_EVENT_TYPE_FLAG:
                 default:
                      OS_EXIT_CRITICAL();
                     *pevents_rdy = (OS_EVENT *)0;      /* NULL terminate return event array           */
                     *perr        =  OS_ERR_EVENT_TYPE;
                      return (events_rdy_nbr);
             }
            *perr = OS_ERR_NONE;
             break;

        case OS_STAT_PEND_ABORT:
            *pmsgs_rdy++ = (void *)0;                   /* NO message returned for abort               */
            *perr        =  OS_ERR_PEND_ABORT;          /* Indicate that event  aborted                */
             break;

        case OS_STAT_PEND_TO:
        default:
            *pmsgs_rdy++ = (void *)0;                   /* NO message returned for timeout             */
            *perr        =  OS_ERR_TIMEOUT;             /* Indicate that events timed out              */
             break;
    }

    OSTCBCur->OSTCBStat          =  OS_STAT_RDY;        /* Set   task  status to ready                 */
    OSTCBCur->OSTCBStatPend      =  OS_STAT_PEND_OK;    /* Clear pend  status                          */
    OSTCBCur->OSTCBEventPtr      = (OS_EVENT  *)0;      /* Clear event pointers                        */
    OSTCBCur->OSTCBEventMultiPtr = (OS_EVENT **)0;
#if ((OS_MBOX_EN > 0u) ||                 \
    ((OS_Q_EN    > 0u) && (OS_MAX_QS > 0u)))
    OSTCBCur->OSTCBMsg           = (void      *)0;      /* Clear task  message                         */
#endif
    OS_EXIT_CRITICAL();

    return (events_rdy_nbr);
}
#endif

/*$PAGE*/
/*
*********************************************************************************************************
*                                             INITIALIZATION
*
* Description: This function is used to initialize the internals of uC/OS-II and MUST be called prior to
*              creating any uC/OS-II object and, prior to calling OSStart().
*
* Arguments  : none
*
* Returns    : none
*********************************************************************************************************
*/
/*操作系统初始化*/
void  OSInit (void)
{
    OSInitHookBegin();                                           /*用户可编写该函数，实现端口说明*/
    OS_InitMisc();                                               /*初始化各种全局变量*/
    OS_InitRdyList();                                            /*初始化就绪表*/
    OS_InitTCBList();                                            /*初始化任务控制块，空闲链表*/
    OS_InitEventList();                                          /*初始化空闲事件列表*/
#if (OS_FLAG_EN > 0u) && (OS_MAX_FLAGS > 0u)					 /*如果使用事件标志*/
    OS_FlagInit();                                               /*对事件标志进行初始化*/
#endif

#if (OS_MEM_EN > 0u) && (OS_MAX_MEM_PART > 0u)					 /*如果配置内存管理*/
    OS_MemInit();                                                /*初始化内存*/
#endif

#if (OS_Q_EN > 0u) && (OS_MAX_QS > 0u)							 /*如果使用消息队列*/
    OS_QInit();                                                  /*初始化消息队列*/
#endif

    OS_InitTaskIdle();                                           /*创建空闲任务*/
#if OS_TASK_STAT_EN > 0u										 /*如果有统计任务*/
    OS_InitTaskStat();                                           /*创建统计任务*/
#endif

#if OS_TMR_EN > 0u												 /*如果定时器始能*/
    OSTmr_Init();                                                /*初始化定时器管理模块*/
#endif

    OSInitHookEnd();                                             /*用户可继续编写该函数，实现端口说明*/

#if OS_DEBUG_EN > 0u											 /*如果支持调试*/
    OSDebugInit();												 /*加入调试代码*/
#endif
}
/*$PAGE*/
/*
*********************************************************************************************************
*                                              ENTER ISR
*
* Description: This function is used to notify uC/OS-II that you are about to service an interrupt
*              service routine (ISR).  This allows uC/OS-II to keep track of interrupt nesting and thus
*              only perform rescheduling at the last nested ISR.
*
* Arguments  : none
*
* Returns    : none
*
* Notes      : 1) This function should be called ith interrupts already disabled
*              2) Your ISR can directly increment OSIntNesting without calling this function because
*                 OSIntNesting has been declared 'global'.
*              3) You MUST still call OSIntExit() even though you increment OSIntNesting directly.
*              4) You MUST invoke OSIntEnter() and OSIntExit() in pair.  In other words, for every call
*                 to OSIntEnter() at the beginning of the ISR you MUST have a call to OSIntExit() at the
*                 end of the ISR.
*              5) You are allowed to nest interrupts up to 255 levels deep.
*              6) I removed the OS_ENTER_CRITICAL() and OS_EXIT_CRITICAL() around the increment because
*                 OSIntEnter() is always called with interrupts disabled.
*********************************************************************************************************
*/

void  OSIntEnter (void)
{
    if (OSRunning == OS_TRUE)
	{
        if (OSIntNesting < 255u)
		{
            OSIntNesting++;/*中断嵌套数+1*/
        }
    }
}
/*$PAGE*/
/*
*********************************************************************************************************
*                                               EXIT ISR
*
* Description: This function is used to notify uC/OS-II that you have completed serviving an ISR.  When
*              the last nested ISR has completed, uC/OS-II will call the scheduler to determine whether
*              a new, high-priority task, is ready to run.
*
* Arguments  : none
*
* Returns    : none
*
* Notes      : 1) You MUST invoke OSIntEnter() and OSIntExit() in pair.  In other words, for every call
*                 to OSIntEnter() at the beginning of the ISR you MUST have a call to OSIntExit() at the
*                 end of the ISR.
*              2) Rescheduling is prevented when the scheduler is locked (see OS_SchedLock())
*********************************************************************************************************
*/

/*时钟中断中任务切换*/
void  OSIntExit (void)
{
#if OS_CRITICAL_METHOD == 3u                                /* Allocate storage for CPU status register */
    OS_CPU_SR  cpu_sr = 0u;
#endif



    if (OSRunning == OS_TRUE)								/*多任务的时候才能调度*/
	{
        OS_ENTER_CRITICAL();
        if (OSIntNesting > 0u)								/*如果当前有中断嵌套*/
		{
            OSIntNesting--;									/*退出的时候嵌套数-1*/
        }
        if (OSIntNesting == 0u)								/*当前没中断嵌套*/
		{
            if (OSLockNesting == 0u)						/*调度器未加锁*/
			{
                OS_SchedNew();								/*确定最高优先级的就绪任务的*/
                OSTCBHighRdy = OSTCBPrioTbl[OSPrioHighRdy];	/*获取优先级最高的任务控制块*/
                if (OSPrioHighRdy != OSPrioCur)				/*最高优先级的是不是当前任务*/
				{
#if OS_TASK_PROFILE_EN > 0u
                    OSTCBHighRdy->OSTCBCtxSwCtr++;
#endif
                    OSCtxSwCtr++;
                    OSIntCtxSw();							/*切换任务*/
                }
            }
        }
        OS_EXIT_CRITICAL();
    }
}
/*$PAGE*/
/*
*********************************************************************************************************
*                            INDICATE THAT IT'S NO LONGER SAFE TO CREATE OBJECTS
*
* Description: This function is called by the application code to indicate that all initialization has
*              been completed and that kernel objects are no longer allowed to be created.
*
* Arguments  : none
*
* Returns    : none
*
* Note(s)    : 1) You should call this function when you no longer want to allow application code to
*                 create kernel objects.
*              2) You need to define the macro 'OS_SAFETY_CRITICAL_IEC61508'
*********************************************************************************************************
*/

#ifdef OS_SAFETY_CRITICAL_IEC61508
void  OSSafetyCriticalStart (void)
{
    OSSafetyCriticalStartFlag = OS_TRUE;
}

#endif

/*$PAGE*/
/*
*********************************************************************************************************
*                                          PREVENT SCHEDULING
*
* Description: This function is used to prevent rescheduling to take place.  This allows your application
*              to prevent context switches until you are ready to permit context switching.
*
* Arguments  : none
*
* Returns    : none
*
* Notes      : 1) You MUST invoke OSSchedLock() and OSSchedUnlock() in pair.  In other words, for every
*                 call to OSSchedLock() you MUST have a call to OSSchedUnlock().
*********************************************************************************************************
*/

#if OS_SCHED_LOCK_EN > 0u
void  OSSchedLock (void)
{
#if OS_CRITICAL_METHOD == 3u                     /* Allocate storage for CPU status register           */
    OS_CPU_SR  cpu_sr = 0u;
#endif



    if (OSRunning == OS_TRUE) {                  /* Make sure multitasking is running                  */
        OS_ENTER_CRITICAL();
        if (OSIntNesting == 0u) {                /* Can't call from an ISR                             */
            if (OSLockNesting < 255u) {          /* Prevent OSLockNesting from wrapping back to 0      */
                OSLockNesting++;                 /* Increment lock nesting level                       */
            }
        }
        OS_EXIT_CRITICAL();
    }
}
#endif

/*$PAGE*/
/*
*********************************************************************************************************
*                                          ENABLE SCHEDULING
*
* Description: This function is used to re-allow rescheduling.
*
* Arguments  : none
*
* Returns    : none
*
* Notes      : 1) You MUST invoke OSSchedLock() and OSSchedUnlock() in pair.  In other words, for every
*                 call to OSSchedLock() you MUST have a call to OSSchedUnlock().
*********************************************************************************************************
*/

#if OS_SCHED_LOCK_EN > 0u
void  OSSchedUnlock (void)
{
#if OS_CRITICAL_METHOD == 3u                               /* Allocate storage for CPU status register */
    OS_CPU_SR  cpu_sr = 0u;
#endif



    if (OSRunning == OS_TRUE) {                            /* Make sure multitasking is running        */
        OS_ENTER_CRITICAL();
        if (OSLockNesting > 0u) {                          /* Do not decrement if already 0            */
            OSLockNesting--;                               /* Decrement lock nesting level             */
            if (OSLockNesting == 0u) {                     /* See if scheduler is enabled and ...      */
                if (OSIntNesting == 0u) {                  /* ... not in an ISR                        */
                    OS_EXIT_CRITICAL();
                    OS_Sched();                            /* See if a HPT is ready                    */
                } else {
                    OS_EXIT_CRITICAL();
                }
            } else {
                OS_EXIT_CRITICAL();
            }
        } else {
            OS_EXIT_CRITICAL();
        }
    }
}
#endif

/*$PAGE*/
/*
*********************************************************************************************************
*                                          START MULTITASKING
*
* Description: This function is used to start the multitasking process which lets uC/OS-II manages the
*              task that you have created.  Before you can call OSStart(), you MUST have called OSInit()
*              and you MUST have created at least one task.
*
* Arguments  : none
*
* Returns    : none
*
* Note       : OSStartHighRdy() MUST:
*                 a) Call OSTaskSwHook() then,
*                 b) Set OSRunning to OS_TRUE.
*                 c) Load the context of the task pointed to by OSTCBHighRdy.
*                 d_ Execute the task.
*********************************************************************************************************
*/

void  OSStart (void)
{
    if (OSRunning == OS_FALSE)						/*如果系统没有启动多任务*/
	{
        OS_SchedNew();								/*找到优先级最高的任务*/
        OSPrioCur     = OSPrioHighRdy;
        OSTCBHighRdy  = OSTCBPrioTbl[OSPrioHighRdy];
        OSTCBCur      = OSTCBHighRdy;
        OSStartHighRdy();							/*启动多任务*/
    }
}
/*$PAGE*/
/*
*********************************************************************************************************
*                                        STATISTICS INITIALIZATION
*
* Description: This function is called by your application to establish CPU usage by first determining
*              how high a 32-bit counter would count to in 1 second if no other tasks were to execute
*              during that time.  CPU usage is then determined by a low priority task which keeps track
*              of this 32-bit counter every second but this time, with other tasks running.  CPU usage is
*              determined by:
*
*                                             OSIdleCtr
*                 CPU Usage (%) = 100 * (1 - ------------)
*                                            OSIdleCtrMax
*
* Arguments  : none
*
* Returns    : none
*********************************************************************************************************
*/

#if OS_TASK_STAT_EN > 0u
/*统计任务的初始化*/
/*目的是获取系统空闲计数的最大值,该函数在用户任务中被调用，这函数在一直过程中命名为TaskStart，优先级是0，在一直部分可以看到*/
/*这是系统没有运行其他任务，统计任务初始化函数将自己阻塞2个时钟周期，在系统时钟中断2次后，有调度器恢复运行，叫做时钟同步*/
void  OSStatInit (void)
{
#if OS_CRITICAL_METHOD == 3u                     /* Allocate storage for CPU status register           */
    OS_CPU_SR  cpu_sr = 0u;
#endif
    OSTimeDly(2u);								/*延时两个时钟周期，目的是与时钟同步*/
    OS_ENTER_CRITICAL();
    OSIdleCtr    = 0uL;							/*空闲计数器清0*/
    OS_EXIT_CRITICAL();
    OSTimeDly(OS_TICKS_PER_SEC / 10u);			/*延时100ms，此时空闲任务一直OSIdleCtr++*/
    OS_ENTER_CRITICAL();
    OSIdleCtrMax = OSIdleCtr;					/*获取最大空闲计数值*/
    OSStatRdy    = OS_TRUE;						/*统计任务准备状态OK*/
    OS_EXIT_CRITICAL();
}
#endif
/*$PAGE*/
/*
*********************************************************************************************************
*                                         PROCESS SYSTEM TICK
*
* Description: This function is used to signal to uC/OS-II the occurrence of a 'system tick' (also known
*              as a 'clock tick').  This function should be called by the ticker ISR but, can also be
*              called by a high priority task.
*
* Arguments  : none
*
* Returns    : none
*********************************************************************************************************
*/

/*时间片调度函数 */
void  OSTimeTick (void)
{
    OS_TCB    *ptcb;
#if OS_TICK_STEP_EN > 0u
    BOOLEAN    step;
#endif
#if OS_CRITICAL_METHOD == 3u                               /* Allocate storage for CPU status register     */
    OS_CPU_SR  cpu_sr = 0u;
#endif
#if OS_TIME_TICK_HOOK_EN > 0u
    OSTimeTickHook();                                      /*调用用户的钩子函数*/
#endif
#if OS_TIME_GET_SET_EN > 0u
    OS_ENTER_CRITICAL();
    OSTime++;											   /*调度计数器+1*/
    OS_EXIT_CRITICAL();
#endif
    if (OSRunning == OS_TRUE)							   /*如果已经启动了多任务*/
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
                 step            = OS_TRUE;                /*      ... step command from uC/OS-View        */
                 OSTickStepState = OS_TICK_STEP_WAIT;
                 break;

            default:                                       /* Invalid case, correct situation              */
                 step            = OS_TRUE;
                 OSTickStepState = OS_TICK_STEP_DIS;
                 break;
        }
        if (step == OS_FALSE) {                            /* Return if waiting for step command           */
            return;
        }
#endif
        ptcb = OSTCBList;                                  /*ptcb指向就绪链表的表头*/
        while (ptcb->OSTCBPrio != OS_TASK_IDLE_PRIO)	   /*如果就绪表头的任务不是空闲*/
		{
            OS_ENTER_CRITICAL();
            if (ptcb->OSTCBDly != 0u)                      /*如果该任务设置了时间延时或事件等待延时*/
			{
                ptcb->OSTCBDly--;                          /*延时-1，因为过了一个时钟滴答*/
                if (ptcb->OSTCBDly == 0u)                  /*延时到期*/
				{
                    if ((ptcb->OSTCBStat & OS_STAT_PEND_ANY) != OS_STAT_RDY)			/*如果任务有等待事件*/
					{
                        ptcb->OSTCBStat  &= (INT8U)~(INT8U)OS_STAT_PEND_ANY;			/*清任务状态*/
                        ptcb->OSTCBStatPend = OS_STAT_PEND_TO;                 			/*指示事件因超时而结束*/
                    }
					else
					{
                        ptcb->OSTCBStatPend = OS_STAT_PEND_OK;							/*指示延时时间到了*/
                    }
                    if ((ptcb->OSTCBStat & OS_STAT_SUSPEND) == OS_STAT_RDY)				/*如果任务不是被挂起的*/
					{
                        OSRdyGrp               |= ptcb->OSTCBBitY;             			/* No,  Make ready          */
                        OSRdyTbl[ptcb->OSTCBY] |= ptcb->OSTCBBitX;
                    }
                }
            }
            ptcb = ptcb->OSTCBNext;                        /*指向下一个TCB*/
            OS_EXIT_CRITICAL();
        }
    }
}

/*
*********************************************************************************************************
*                                             GET VERSION
*
* Description: This function is used to return the version number of uC/OS-II.  The returned value
*              corresponds to uC/OS-II's version number multiplied by 100.  In other words, version 2.00
*              would be returned as 200.
*
* Arguments  : none
*
* Returns    : the version number of uC/OS-II multiplied by 100.
*********************************************************************************************************
*/

INT16U  OSVersion (void)
{
    return (OS_VERSION);
}

/*$PAGE*/
/*
*********************************************************************************************************
*                                            DUMMY FUNCTION
*
* Description: This function doesn't do anything.  It is called by OSTaskDel().
*
* Arguments  : none
*
* Returns    : none
*********************************************************************************************************
*/

#if OS_TASK_DEL_EN > 0u
/*空函数*/
void  OS_Dummy (void)
{
}
#endif

/*$PAGE*/
/*
*********************************************************************************************************
*                             MAKE TASK READY TO RUN BASED ON EVENT OCCURING
*
* Description: This function is called by other uC/OS-II services and is used to ready a task that was
*              waiting for an event to occur.
*
* Arguments  : pevent      is a pointer to the event control block corresponding to the event.
*
*              pmsg        is a pointer to a message.  This pointer is used by message oriented services
*                          such as MAILBOXEs and QUEUEs.  The pointer is not used when called by other
*                          service functions.
*
*              msk         is a mask that is used to clear the status byte of the TCB.  For example,
*                          OSSemPost() will pass OS_STAT_SEM, OSMboxPost() will pass OS_STAT_MBOX etc.
*
*              pend_stat   is used to indicate the readied task's pending status:
*
*                          OS_STAT_PEND_OK      Task ready due to a post (or delete), not a timeout or
*                                               an abort.
*                          OS_STAT_PEND_ABORT   Task ready due to an abort.
*
* Returns    : none
*
* Note       : This function is INTERNAL to uC/OS-II and your application should not call it.
*********************************************************************************************************
*/

/*将等待事件的任务就绪*/
#if (OS_EVENT_EN)
INT8U  OS_EventTaskRdy (OS_EVENT  *pevent,				/*ECB的指针*/
                        void      *pmsg,				/*消息指针*/
                        INT8U      msk,					/*清除状态位的掩码*/
                        INT8U      pend_stat)			/*等待(pend)结束*/
{
    OS_TCB   *ptcb;
    INT8U     y;
    INT8U     x;
    INT8U     prio;
#if OS_LOWEST_PRIO > 63u
    OS_PRIO  *ptbl;
#endif
#if OS_LOWEST_PRIO <= 63u
    y    = OSUnMapTbl[pevent->OSEventGrp];
    x    = OSUnMapTbl[pevent->OSEventTbl[y]];
    prio = (INT8U)((y << 3u) + x);						/*根据事件等待组和表，找到正在等待事件的任务中最高优先级的*/
#else
    if((pevent->OSEventGrp & 0xFFu) != 0u)				/* Find HPT waiting for message*/
	{
        y = OSUnMapTbl[ pevent->OSEventGrp & 0xFFu];
    }
	else
    {
        y = OSUnMapTbl[(OS_PRIO)(pevent->OSEventGrp >> 8u) & 0xFFu] + 8u;
    }
    ptbl = &pevent->OSEventTbl[y];
    if((*ptbl & 0xFFu) != 0u)
	{
        x = OSUnMapTbl[*ptbl & 0xFFu];
    }
	else
	{
        x = OSUnMapTbl[(OS_PRIO)(*ptbl >> 8u) & 0xFFu] + 8u;
    }
    prio = (INT8U)((y << 4u) + x);						/* Find priority of task getting the msg       */
#endif
    ptcb                  =  OSTCBPrioTbl[prio];		/*找到任务控制块*/
    ptcb->OSTCBDly        =  0u;						/*延时时间清0，如果不是0每个时钟周期会--，会产生冲突*/
#if ((OS_Q_EN > 0u) && (OS_MAX_QS > 0u)) || (OS_MBOX_EN > 0u)
    ptcb->OSTCBMsg        =  pmsg;						/*TCB中指针指向ECB*/
#else
    pmsg                  =  pmsg;
#endif
    ptcb->OSTCBStat      &= (INT8U)~msk;				/*清除任务控制块的任务状态标志*/
    ptcb->OSTCBStatPend   =  pend_stat;					/*设置等待状态*/
                                                        /* See if task is ready (could be susp'd)      */
    if ((ptcb->OSTCBStat &   OS_STAT_SUSPEND) == OS_STAT_RDY)			/*如果任务没有挂起*/
	{
        OSRdyGrp         |=  ptcb->OSTCBBitY;			/*把任务就绪了*/
        OSRdyTbl[y]      |=  ptcb->OSTCBBitX;
    }
    OS_EventTaskRemove(ptcb, pevent);					/*事件等待表和组中删除该任务*/
#if (OS_EVENT_MULTI_EN > 0u)
    if (ptcb->OSTCBEventMultiPtr != (OS_EVENT **)0)		/* Remove this task from events' wait lists    */
	{
        OS_EventTaskRemoveMulti(ptcb, ptcb->OSTCBEventMultiPtr);
        ptcb->OSTCBEventPtr       = (OS_EVENT  *)pevent;/* Return event as first multi-pend event ready*/
    }
#endif
    return (prio);
}
#endif

/*$PAGE*/
/*
*********************************************************************************************************
*                                   MAKE TASK WAIT FOR EVENT TO OCCUR
*
* Description: This function is called by other uC/OS-II services to suspend a task because an event has
*              not occurred.
*
* Arguments  : pevent   is a pointer to the event control block for which the task will be waiting for.
*
* Returns    : none
*
* Note       : This function is INTERNAL to uC/OS-II and your application should not call it.
*********************************************************************************************************
*/
/*设置事件等待函数，将任务在ECB中登记的函数，把任务在就绪组表中取消*/
#if (OS_EVENT_EN)
void  OS_EventTaskWait (OS_EVENT *pevent)
{
    INT8U  y;
    OSTCBCur->OSTCBEventPtr               = pevent;						/*让任务控制块的指针指向事件控制块，这样就可以直接调用TCB来控制ECB了*/
    pevent->OSEventTbl[OSTCBCur->OSTCBY] |= OSTCBCur->OSTCBBitX;		/*ECB等待表中添加任务优先级编号*/
    pevent->OSEventGrp                   |= OSTCBCur->OSTCBBitY;		/*ECB等待组中添加任务优先级编号*/
    y             =  OSTCBCur->OSTCBY;									/*任务等待事件后需要阻塞当前任务，处理一下就绪表和就绪组*/
    OSRdyTbl[y]  &= (OS_PRIO)~OSTCBCur->OSTCBBitX;
    if (OSRdyTbl[y] == 0u)
	{
        OSRdyGrp &= (OS_PRIO)~OSTCBCur->OSTCBBitY;
    }
}
#endif
/*
*********************************************************************************************************
*                          MAKE TASK WAIT FOR ANY OF MULTIPLE EVENTS TO OCCUR
*
* Description: This function is called by other uC/OS-II services to suspend a task because any one of
*              multiple events has not occurred.
*
* Arguments  : pevents_wait     is a pointer to an array of event control blocks, NULL-terminated, for
*                               which the task will be waiting for.
*
* Returns    : none.
*
* Note       : This function is INTERNAL to uC/OS-II and your application should not call it.
*********************************************************************************************************
*/
#if ((OS_EVENT_EN) && (OS_EVENT_MULTI_EN > 0u))
void  OS_EventTaskWaitMulti (OS_EVENT **pevents_wait)
{
    OS_EVENT **pevents;
    OS_EVENT  *pevent;
    INT8U      y;


    OSTCBCur->OSTCBEventPtr      = (OS_EVENT  *)0;
    OSTCBCur->OSTCBEventMultiPtr = (OS_EVENT **)pevents_wait;       /* Store ptr to ECBs in TCB        */

    pevents =  pevents_wait;
    pevent  = *pevents;
    while (pevent != (OS_EVENT *)0) {                               /* Put task in waiting lists       */
        pevent->OSEventTbl[OSTCBCur->OSTCBY] |= OSTCBCur->OSTCBBitX;
        pevent->OSEventGrp                   |= OSTCBCur->OSTCBBitY;
        pevents++;
        pevent = *pevents;
    }

    y             =  OSTCBCur->OSTCBY;            /* Task no longer ready                              */
    OSRdyTbl[y]  &= (OS_PRIO)~OSTCBCur->OSTCBBitX;
    if (OSRdyTbl[y] == 0u) {                      /* Clear event grp bit if this was only task pending */
        OSRdyGrp &= (OS_PRIO)~OSTCBCur->OSTCBBitY;
    }
}
#endif
/*$PAGE*/
/*
*********************************************************************************************************
*                                   REMOVE TASK FROM EVENT WAIT LIST
*
* Description: Remove a task from an event's wait list.
*
* Arguments  : ptcb     is a pointer to the task to remove.
*
*              pevent   is a pointer to the event control block.
*
* Returns    : none
*
* Note       : This function is INTERNAL to uC/OS-II and your application should not call it.
*********************************************************************************************************
*/
/*在事件等待组和表中删除该任务*/
/*取消等待事件的表和组*/
#if (OS_EVENT_EN)
void  OS_EventTaskRemove (OS_TCB   *ptcb,
                          OS_EVENT *pevent)
{
    INT8U  y;
    y                       =  ptcb->OSTCBY;
    pevent->OSEventTbl[y]  &= (OS_PRIO)~ptcb->OSTCBBitX;/*处理事件等待表*/
    if (pevent->OSEventTbl[y] == 0u)
	{
        pevent->OSEventGrp &= (OS_PRIO)~ptcb->OSTCBBitY;/*处理事件等待组*/
    }
}
#endif
/*
*********************************************************************************************************
*                             REMOVE TASK FROM MULTIPLE EVENTS WAIT LISTS
*
* Description: Remove a task from multiple events' wait lists.
*
* Arguments  : ptcb             is a pointer to the task to remove.
*
*              pevents_multi    is a pointer to the array of event control blocks, NULL-terminated.
*
* Returns    : none
*
* Note       : This function is INTERNAL to uC/OS-II and your application should not call it.
*********************************************************************************************************
*/
#if ((OS_EVENT_EN) && (OS_EVENT_MULTI_EN > 0u))
void  OS_EventTaskRemoveMulti (OS_TCB    *ptcb,
                               OS_EVENT **pevents_multi)
{
    OS_EVENT **pevents;
    OS_EVENT  *pevent;
    INT8U      y;
    OS_PRIO    bity;
    OS_PRIO    bitx;


    y       =  ptcb->OSTCBY;
    bity    =  ptcb->OSTCBBitY;
    bitx    =  ptcb->OSTCBBitX;
    pevents =  pevents_multi;
    pevent  = *pevents;
    while (pevent != (OS_EVENT *)0) {                   /* Remove task from all events' wait lists     */
        pevent->OSEventTbl[y]  &= (OS_PRIO)~bitx;
        if (pevent->OSEventTbl[y] == 0u) {
            pevent->OSEventGrp &= (OS_PRIO)~bity;
        }
        pevents++;
        pevent = *pevents;
    }
}
#endif
/*$PAGE*/
/*
*********************************************************************************************************
*                                 INITIALIZE EVENT CONTROL BLOCK'S WAIT LIST
*
* Description: This function is called by other uC/OS-II services to initialize the event wait list.
*
* Arguments  : pevent    is a pointer to the event control block allocated to the event.
*
* Returns    : none
*
* Note       : This function is INTERNAL to uC/OS-II and your application should not call it.
*********************************************************************************************************
*/
/*事件等待表初始化，也就是清一下ECB中的等待组+表*/
#if (OS_EVENT_EN)
void  OS_EventWaitListInit (OS_EVENT *pevent)
{
    INT8U  i;
    pevent->OSEventGrp = 0u;						/*清事件等待组*/
    for (i = 0u; i < OS_EVENT_TBL_SIZE; i++)		/*清事件等待表*/
	{
        pevent->OSEventTbl[i] = 0u;
    }
}
#endif
/*$PAGE*/
/*
*********************************************************************************************************
*                                             INITIALIZATION
*                           INITIALIZE THE FREE LIST OF EVENT CONTROL BLOCKS
*
* Description: This function is called by OSInit() to initialize the free list of event control blocks.
*
* Arguments  : none
*
* Returns    : none
*********************************************************************************************************
*/

/*初始化空闲事件列表*/
static  void  OS_InitEventList (void)
{
#if (OS_EVENT_EN) && (OS_MAX_EVENTS > 0u)
#if (OS_MAX_EVENTS > 1u)
    INT16U     ix;
    INT16U     ix_next;
    OS_EVENT  *pevent1;
    OS_EVENT  *pevent2;
    OS_MemClr((INT8U *)&OSEventTbl[0], sizeof(OSEventTbl));		/*清空所有的事件控制块*/
    for (ix = 0u; ix < (OS_MAX_EVENTS - 1u); ix++)				/*初始化ECB，空闲链表连上*/
	{
        ix_next = ix + 1u;
        pevent1 = &OSEventTbl[ix];
        pevent2 = &OSEventTbl[ix_next];
        pevent1->OSEventType    = OS_EVENT_TYPE_UNUSED;
        pevent1->OSEventPtr     = pevent2;
#if OS_EVENT_NAME_EN > 0u
        pevent1->OSEventName    = (INT8U *)(void *)"?";
#endif
    }
    pevent1                         = &OSEventTbl[ix];			/*最后一个ECB赋值*/
    pevent1->OSEventType            = OS_EVENT_TYPE_UNUSED;		/*最后一个ECB赋值*/
    pevent1->OSEventPtr             = (OS_EVENT *)0;			/*最后一个ECB赋值*/
#if OS_EVENT_NAME_EN > 0u
    pevent1->OSEventName            = (INT8U *)(void *)"?";		/*最后一个ECB赋值*/
#endif
    OSEventFreeList                 = &OSEventTbl[0];			/*空闲指针指向表头*/
#else
    OSEventFreeList                 = &OSEventTbl[0];
    OSEventFreeList->OSEventType    = OS_EVENT_TYPE_UNUSED;
    OSEventFreeList->OSEventPtr     = (OS_EVENT *)0;
#if OS_EVENT_NAME_EN > 0u
    OSEventFreeList->OSEventName    = (INT8U *)"?";
#endif
#endif
#endif
}

/*$PAGE*/
/*
*********************************************************************************************************
*                                             INITIALIZATION
*                                    INITIALIZE MISCELLANEOUS VARIABLES
*
* Description: This function is called by OSInit() to initialize miscellaneous variables.
*
* Arguments  : none
*
* Returns    : none
*********************************************************************************************************
*/
/*初始化各种全局变量*/
static  void  OS_InitMisc (void)
{
#if OS_TIME_GET_SET_EN > 0u
    OSTime                    = 0uL;                       /*清系统时间*/
#endif

    OSIntNesting              = 0u;                        /*清中断嵌套计数*/
    OSLockNesting             = 0u;                        /*调度锁计数*/
    OSTaskCtr                 = 0u;                        /*当前任务数*/
    OSRunning                 = OS_FALSE;                  /*关闭多任务*/
    OSCtxSwCtr                = 0u;                        /*清任务切换次数*/
    OSIdleCtr                 = 0uL;                       /*清空闲计数器*/

#if OS_TASK_STAT_EN > 0u								   /*如果需要统计函数*/
    OSIdleCtrRun              = 0uL;					   /*1秒内空闲计数值置0*/
    OSIdleCtrMax              = 0uL;					   /*最大空闲计数值置0*/
    OSStatRdy                 = OS_FALSE;                  /*统计任务准备状态置false*//* Statistic task is not ready              */
#endif

#ifdef OS_SAFETY_CRITICAL_IEC61508
    OSSafetyCriticalStartFlag = OS_FALSE;                  /* Still allow creation of objects          */
#endif
}
/*$PAGE*/
/*
*********************************************************************************************************
*                                             INITIALIZATION
*                                       INITIALIZE THE READY LIST
*
* Description: This function is called by OSInit() to initialize the Ready List.
*
* Arguments  : none
*
* Returns    : none
*********************************************************************************************************
*/
/*初始化就绪表*/
static  void  OS_InitRdyList (void)
{
    INT8U  i;
    OSRdyGrp      = 0u;                                    /*清空就绪组*/
    for (i = 0u; i < OS_RDY_TBL_SIZE; i++)				   /*清空就绪表*/
	{
        OSRdyTbl[i] = 0u;
    }
    OSPrioCur     = 0u;									   /*当前任务优先级*/
    OSPrioHighRdy = 0u;									   /*运行任务的最高优先级*/
    OSTCBHighRdy  = (OS_TCB *)0;						   /*最高优先级的任务控制块的指针*/
    OSTCBCur      = (OS_TCB *)0;						   /*当前运行的任务控制块的指针*/
}

/*$PAGE*/
/*
*********************************************************************************************************
*                                             INITIALIZATION
*                                         CREATING THE IDLE TASK
*
* Description: This function creates the Idle Task.
*
* Arguments  : none
*
* Returns    : none
*********************************************************************************************************
*/
/*创建空闲任务*/
static  void  OS_InitTaskIdle (void)
{
#if OS_TASK_NAME_EN > 0u
    INT8U  err;														 /*在任务使用任务名的配置下，定义整型局部变量err*/
#endif
#if OS_TASK_CREATE_EXT_EN > 0u										 /*如果使用扩展功能*/
    #if OS_STK_GROWTH == 1u											 /*堆栈的生长方向*/
    (void)OSTaskCreateExt(OS_TaskIdle,
                          (void *)0,                                 /* No arguments passed to OS_TaskIdle() */
                          &OSTaskIdleStk[OS_TASK_IDLE_STK_SIZE - 1u],/* Set Top-Of-Stack                     */
                          OS_TASK_IDLE_PRIO,                         /*空闲任务的优先级*/
                          OS_TASK_IDLE_ID,
                          &OSTaskIdleStk[0],                         /* Set Bottom-Of-Stack                  */
                          OS_TASK_IDLE_STK_SIZE,
                          (void *)0,                                 /* No TCB extension                     */
                          OS_TASK_OPT_STK_CHK | OS_TASK_OPT_STK_CLR);/* Enable stack checking + clear stack  */
    #else
    (void)OSTaskCreateExt(OS_TaskIdle,
                          (void *)0,                                 /* No arguments passed to OS_TaskIdle() */
                          &OSTaskIdleStk[0],                         /* Set Top-Of-Stack                     */
                          OS_TASK_IDLE_PRIO,                         /* Lowest priority level                */
                          OS_TASK_IDLE_ID,
                          &OSTaskIdleStk[OS_TASK_IDLE_STK_SIZE - 1u],/* Set Bottom-Of-Stack                  */
                          OS_TASK_IDLE_STK_SIZE,
                          (void *)0,                                 /* No TCB extension                     */
                          OS_TASK_OPT_STK_CHK | OS_TASK_OPT_STK_CLR);/* Enable stack checking + clear stack  */
    #endif
#else
    #if OS_STK_GROWTH == 1u
    (void)OSTaskCreate(OS_TaskIdle,
                       (void *)0,
                       &OSTaskIdleStk[OS_TASK_IDLE_STK_SIZE - 1u],
                       OS_TASK_IDLE_PRIO);
    #else
    (void)OSTaskCreate(OS_TaskIdle,
                       (void *)0,
                       &OSTaskIdleStk[0],
                       OS_TASK_IDLE_PRIO);
    #endif
#endif

#if OS_TASK_NAME_EN > 0u
    OSTaskNameSet(OS_TASK_IDLE_PRIO, (INT8U *)(void *)"uC/OS-II Idle", &err);
#endif
}
/*$PAGE*/
/*
*********************************************************************************************************
*                                             INITIALIZATION
*                                      CREATING THE STATISTIC TASK
*
* Description: This function creates the Statistic Task.
*
* Arguments  : none
*
* Returns    : none
*********************************************************************************************************
*/
/*创建统计任务*/
#if OS_TASK_STAT_EN > 0u
static  void  OS_InitTaskStat (void)
{
#if OS_TASK_NAME_EN > 0u
    INT8U  err;
#endif


#if OS_TASK_CREATE_EXT_EN > 0u
    #if OS_STK_GROWTH == 1u
    (void)OSTaskCreateExt(OS_TaskStat,
                          (void *)0,                                   /* No args passed to OS_TaskStat()*/
                          &OSTaskStatStk[OS_TASK_STAT_STK_SIZE - 1u],  /* Set Top-Of-Stack               */
                          OS_TASK_STAT_PRIO,                           /* One higher than the idle task  */
                          OS_TASK_STAT_ID,
                          &OSTaskStatStk[0],                           /* Set Bottom-Of-Stack            */
                          OS_TASK_STAT_STK_SIZE,
                          (void *)0,                                   /* No TCB extension               */
                          OS_TASK_OPT_STK_CHK | OS_TASK_OPT_STK_CLR);  /* Enable stack checking + clear  */
    #else
    (void)OSTaskCreateExt(OS_TaskStat,
                          (void *)0,                                   /* No args passed to OS_TaskStat()*/
                          &OSTaskStatStk[0],                           /* Set Top-Of-Stack               */
                          OS_TASK_STAT_PRIO,                           /* One higher than the idle task  */
                          OS_TASK_STAT_ID,
                          &OSTaskStatStk[OS_TASK_STAT_STK_SIZE - 1u],  /* Set Bottom-Of-Stack            */
                          OS_TASK_STAT_STK_SIZE,
                          (void *)0,                                   /* No TCB extension               */
                          OS_TASK_OPT_STK_CHK | OS_TASK_OPT_STK_CLR);  /* Enable stack checking + clear  */
    #endif
#else
    #if OS_STK_GROWTH == 1u
    (void)OSTaskCreate(OS_TaskStat,
                       (void *)0,                                      /* No args passed to OS_TaskStat()*/
                       &OSTaskStatStk[OS_TASK_STAT_STK_SIZE - 1u],     /* Set Top-Of-Stack               */
                       OS_TASK_STAT_PRIO);                             /* One higher than the idle task  */
    #else
    (void)OSTaskCreate(OS_TaskStat,
                       (void *)0,                                      /* No args passed to OS_TaskStat()*/
                       &OSTaskStatStk[0],                              /* Set Top-Of-Stack               */
                       OS_TASK_STAT_PRIO);                             /* One higher than the idle task  */
    #endif
#endif

#if OS_TASK_NAME_EN > 0u
    OSTaskNameSet(OS_TASK_STAT_PRIO, (INT8U *)(void *)"uC/OS-II Stat", &err);
#endif
}
#endif
/*$PAGE*/
/*
*********************************************************************************************************
*                                             INITIALIZATION
*                            INITIALIZE THE FREE LIST OF TASK CONTROL BLOCKS
*
* Description: This function is called by OSInit() to initialize the free list of OS_TCBs.
*
* Arguments  : none
*
* Returns    : none
*********************************************************************************************************
*/
/*初始化任务控制块，空闲链表*/
static  void  OS_InitTCBList (void)
{
    INT8U    ix;
    INT8U    ix_next;
    OS_TCB  *ptcb1;
    OS_TCB  *ptcb2;

    OS_MemClr((INT8U *)&OSTCBTbl[0],     sizeof(OSTCBTbl));      /*清空所有的任务控制块*/
    OS_MemClr((INT8U *)&OSTCBPrioTbl[0], sizeof(OSTCBPrioTbl));  /*清空任务优先级指针表*/
    for (ix = 0u; ix < (OS_MAX_TASKS + OS_N_SYS_TASKS - 1u); ix++) /*把所有控制块都连上，连成一个链表*/
	{    
        ix_next =  ix + 1u;
        ptcb1   = &OSTCBTbl[ix];
        ptcb2   = &OSTCBTbl[ix_next];
        ptcb1->OSTCBNext = ptcb2;
#if OS_TASK_NAME_EN > 0u
        ptcb1->OSTCBTaskName = (INT8U *)(void *)"?";             /* Unknown name                       */
#endif
    }

	
    ptcb1                   = &OSTCBTbl[ix];
    ptcb1->OSTCBNext        = (OS_TCB *)0;                       /*最后一项指向0地址*/
#if OS_TASK_NAME_EN > 0u
    ptcb1->OSTCBTaskName    = (INT8U *)(void *)"?";              /* Unknown name                       */
#endif
    OSTCBList               = (OS_TCB *)0;                       /*就绪链表指针指向0*/
    OSTCBFreeList           = &OSTCBTbl[0];						 /*空闲链表指针指向表头*/
}


/*$PAGE*/
/*
*********************************************************************************************************
*                                        CLEAR A SECTION OF MEMORY
*
* Description: This function is called by other uC/OS-II services to clear a contiguous block of RAM.
*
* Arguments  : pdest    is the start of the RAM to clear (i.e. write 0x00 to)
*
*              size     is the number of bytes to clear.
*
* Returns    : none
*
* Notes      : 1) This function is INTERNAL to uC/OS-II and your application should not call it.
*              2) Note that we can only clear up to 64K bytes of RAM.  This is not an issue because none
*                 of the uses of this function gets close to this limit.
*              3) The clear is done one byte at a time since this will work on any processor irrespective
*                 of the alignment of the destination.
*********************************************************************************************************
*/

void  OS_MemClr (INT8U  *pdest,
                 INT16U  size)
{
    while (size > 0u) {
        *pdest++ = (INT8U)0;
        size--;
    }
}
/*$PAGE*/
/*
*********************************************************************************************************
*                                        COPY A BLOCK OF MEMORY
*
* Description: This function is called by other uC/OS-II services to copy a block of memory from one
*              location to another.
*
* Arguments  : pdest    is a pointer to the 'destination' memory block
*
*              psrc     is a pointer to the 'source'      memory block
*
*              size     is the number of bytes to copy.
*
* Returns    : none
*
* Notes      : 1) This function is INTERNAL to uC/OS-II and your application should not call it.  There is
*                 no provision to handle overlapping memory copy.  However, that's not a problem since this
*                 is not a situation that will happen.
*              2) Note that we can only copy up to 64K bytes of RAM
*              3) The copy is done one byte at a time since this will work on any processor irrespective
*                 of the alignment of the source and destination.
*********************************************************************************************************
*/

void  OS_MemCopy (INT8U  *pdest,
                  INT8U  *psrc,
                  INT16U  size)
{
    while (size > 0u) {
        *pdest++ = *psrc++;
        size--;
    }
}
/*$PAGE*/
/*
*********************************************************************************************************
*                                              SCHEDULER
*
* Description: This function is called by other uC/OS-II services to determine whether a new, high
*              priority task has been made ready to run.  This function is invoked by TASK level code
*              and is not used to reschedule tasks from ISRs (see OSIntExit() for ISR rescheduling).
*
* Arguments  : none
*
* Returns    : none
*
* Notes      : 1) This function is INTERNAL to uC/OS-II and your application should not call it.
*              2) Rescheduling is prevented when the scheduler is locked (see OS_SchedLock())
*********************************************************************************************************
*/
/*任务调度*/
void  OS_Sched (void)
{
#if OS_CRITICAL_METHOD == 3u							/* Allocate storage for CPU status register     */
    OS_CPU_SR  cpu_sr = 0u;
#endif
    OS_ENTER_CRITICAL();
    if (OSIntNesting == 0u)								/*调度任务时必须没在中断里*/
	{
        if (OSLockNesting == 0u)						/*调度器没有上锁*/
		{
            OS_SchedNew();								/*找到最高优先级的任务*/
            OSTCBHighRdy = OSTCBPrioTbl[OSPrioHighRdy];	/*获取优先级最高任务控制块地址*/
            if (OSPrioHighRdy != OSPrioCur)				/*如果不是当前任务，就要进行切换*/
			{
#if OS_TASK_PROFILE_EN > 0u
                OSTCBHighRdy->OSTCBCtxSwCtr++;			/*任务调度次数++*/
#endif
                OSCtxSwCtr++;							/*操作系统切换任务次数++*/
                OS_TASK_SW();							/*进行任务切换*/
            }
        }
    }
    OS_EXIT_CRITICAL();
}


/*
*********************************************************************************************************
*                              FIND HIGHEST PRIORITY TASK READY TO RUN
*
* Description: This function is called by other uC/OS-II services to determine the highest priority task
*              that is ready to run.  The global variable 'OSPrioHighRdy' is changed accordingly.
*
* Arguments  : none
*
* Returns    : none
*
* Notes      : 1) This function is INTERNAL to uC/OS-II and your application should not call it.
*              2) Interrupts are assumed to be disabled when this function is called.
*********************************************************************************************************
*/
/*用来确定最高优先级的就绪任务*/
static  void  OS_SchedNew (void)
{
#if OS_LOWEST_PRIO <= 63u                        /* See if we support up to 64 tasks                   */
    INT8U   y;
    y             = OSUnMapTbl[OSRdyGrp];
    OSPrioHighRdy = (INT8U)((y << 3u) + OSUnMapTbl[OSRdyTbl[y]]);
#else                                            /* We support up to 256 tasks                         */
    INT8U     y;
    OS_PRIO  *ptbl;
    if ((OSRdyGrp & 0xFFu) != 0u) {
        y = OSUnMapTbl[OSRdyGrp & 0xFFu];
    } else {
        y = OSUnMapTbl[(OS_PRIO)(OSRdyGrp >> 8u) & 0xFFu] + 8u;
    }
    ptbl = &OSRdyTbl[y];
    if ((*ptbl & 0xFFu) != 0u) {
        OSPrioHighRdy = (INT8U)((y << 4u) + OSUnMapTbl[(*ptbl & 0xFFu)]);
    } else {
        OSPrioHighRdy = (INT8U)((y << 4u) + OSUnMapTbl[(OS_PRIO)(*ptbl >> 8u) & 0xFFu] + 8u);
    }
#endif
}

/*$PAGE*/
/*
*********************************************************************************************************
*                                DETERMINE THE LENGTH OF AN ASCII STRING
*
* Description: This function is called by other uC/OS-II services to determine the size of an ASCII string
*              (excluding the NUL character).
*
* Arguments  : psrc     is a pointer to the string for which we need to know the size.
*
* Returns    : The size of the string (excluding the NUL terminating character)
*
* Notes      : 1) This function is INTERNAL to uC/OS-II and your application should not call it.
*              2) The string to check must be less than 255 characters long.
*********************************************************************************************************
*/

#if (OS_EVENT_NAME_EN > 0u) || (OS_FLAG_NAME_EN > 0u) || (OS_MEM_NAME_EN > 0u) || (OS_TASK_NAME_EN > 0u) || (OS_TMR_CFG_NAME_EN > 0u)
INT8U  OS_StrLen (INT8U *psrc)
{
    INT8U  len;


    len = 0u;
    while (*psrc != OS_ASCII_NUL) {
        psrc++;
        len++;
    }
    return (len);
}
#endif
/*$PAGE*/
/*
*********************************************************************************************************
*                                              IDLE TASK
*
* Description: This task is internal to uC/OS-II and executes whenever no other higher priority tasks
*              executes because they are ALL waiting for event(s) to occur.
*
* Arguments  : none
*
* Returns    : none
*
* Note(s)    : 1) OSTaskIdleHook() is called after the critical section to ensure that interrupts will be
*                 enabled for at least a few instructions.  On some processors (ex. Philips XA), enabling
*                 and then disabling interrupts didn't allow the processor enough time to have interrupts
*                 enabled before they were disabled again.  uC/OS-II would thus never recognize
*                 interrupts.
*              2) This hook has been added to allow you to do such things as STOP the CPU to conserve
*                 power.
*********************************************************************************************************
*/
/*空闲任务*/
void  OS_TaskIdle (void *p_arg)
{
#if OS_CRITICAL_METHOD == 3u                     /* Allocate storage for CPU status register           */
    OS_CPU_SR  cpu_sr = 0u;
#endif



    p_arg = p_arg;								/*防止报错*/
    for (;;)
	{
        OS_ENTER_CRITICAL();
        OSIdleCtr++;							/*空闲计数器++*/
        OS_EXIT_CRITICAL();
        OSTaskIdleHook();						/*钩子函数*/
    }
}
/*$PAGE*/
/*
*********************************************************************************************************
*                                            STATISTICS TASK
*
* Description: This task is internal to uC/OS-II and is used to compute some statistics about the
*              multitasking environment.  Specifically, OS_TaskStat() computes the CPU usage.
*              CPU usage is determined by:
*
*                                          OSIdleCtr
*                 OSCPUUsage = 100 * (1 - ------------)     (units are in %)
*                                         OSIdleCtrMax
*
* Arguments  : parg     this pointer is not used at this time.
*
* Returns    : none
*
* Notes      : 1) This task runs at a priority level higher than the idle task.  In fact, it runs at the
*                 next higher priority, OS_TASK_IDLE_PRIO-1.
*              2) You can disable this task by setting the configuration #define OS_TASK_STAT_EN to 0.
*              3) You MUST have at least a delay of 2/10 seconds to allow for the system to establish the
*                 maximum value for the idle counter.
*********************************************************************************************************
*/

#if OS_TASK_STAT_EN > 0u
/*统计任务代码*/
void  OS_TaskStat (void *p_arg)
{
#if OS_CRITICAL_METHOD == 3u						/* Allocate storage for CPU status register           */
    OS_CPU_SR  cpu_sr = 0u;
#endif
    p_arg = p_arg;									/*防止编译报错*/
    while (OSStatRdy == OS_FALSE)					/*统计任务没准备好*/
	{
        OSTimeDly(2u * OS_TICKS_PER_SEC / 10u);		/*延时0.2s，等她准备好*/
    }
    OSIdleCtrMax /= 100uL;
    if (OSIdleCtrMax == 0uL)						/*计数太少了，证明系统比较忙*/
	{
        OSCPUUsage = 0u;
#if OS_TASK_SUSPEND_EN > 0u
        (void)OSTaskSuspend(OS_PRIO_SELF);			/*挂起自己*/
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
        OSIdleCtrRun = OSIdleCtr;					/*获取过去100ms的空闲计数*/
        OSIdleCtr    = 0uL;							/*清空闲计数*/
        OS_EXIT_CRITICAL();
        OSCPUUsage   = (INT8U)(100uL - OSIdleCtrRun / OSIdleCtrMax);
        OSTaskStatHook();							/*钩子函数*/
#if (OS_TASK_STAT_STK_CHK_EN > 0u) && (OS_TASK_CREATE_EXT_EN > 0u)
        OS_TaskStatStkChk();						/*堆栈检查*/
#endif
        OSTimeDly(OS_TICKS_PER_SEC / 10u);			/*延时0.1s，为了下一个100ms累计OSIdleCtr*/
    }
}
#endif
/*
*********************************************************************************************************
*                                      CHECK ALL TASK STACKS
*
* Description: This function is called by OS_TaskStat() to check the stacks of each active task.
*
* Arguments  : none
*
* Returns    : none
*********************************************************************************************************
*/

#if (OS_TASK_STAT_STK_CHK_EN > 0u) && (OS_TASK_CREATE_EXT_EN > 0u)
/*堆栈检查*/
void  OS_TaskStatStkChk (void)
{
    OS_TCB      *ptcb;
    OS_STK_DATA  stk_data;
    INT8U        err;
    INT8U        prio;


    for (prio = 0u; prio <= OS_TASK_IDLE_PRIO; prio++) {
        err = OSTaskStkChk(prio, &stk_data);
        if (err == OS_ERR_NONE) {
            ptcb = OSTCBPrioTbl[prio];
            if (ptcb != (OS_TCB *)0) {                               /* Make sure task 'ptcb' is ...   */
                if (ptcb != OS_TCB_RESERVED) {                       /* ... still valid.               */
#if OS_TASK_PROFILE_EN > 0u
                    #if OS_STK_GROWTH == 1u
                    ptcb->OSTCBStkBase = ptcb->OSTCBStkBottom + ptcb->OSTCBStkSize;
                    #else
                    ptcb->OSTCBStkBase = ptcb->OSTCBStkBottom - ptcb->OSTCBStkSize;
                    #endif
                    ptcb->OSTCBStkUsed = stk_data.OSUsed;            /* Store the number of bytes used */
#endif
                }
            }
        }
    }
}
#endif
/*$PAGE*/
/*
*********************************************************************************************************
*                                            INITIALIZE TCB
*
* Description: This function is internal to uC/OS-II and is used to initialize a Task Control Block when
*              a task is created (see OSTaskCreate() and OSTaskCreateExt()).
*
* Arguments  : prio          is the priority of the task being created
*
*              ptos          is a pointer to the task's top-of-stack assuming that the CPU registers
*                            have been placed on the stack.  Note that the top-of-stack corresponds to a
*                            'high' memory location is OS_STK_GROWTH is set to 1 and a 'low' memory
*                            location if OS_STK_GROWTH is set to 0.  Note that stack growth is CPU
*                            specific.
*
*              pbos          is a pointer to the bottom of stack.  A NULL pointer is passed if called by
*                            'OSTaskCreate()'.
*
*              id            is the task's ID (0..65535)
*
*              stk_size      is the size of the stack (in 'stack units').  If the stack units are INT8Us
*                            then, 'stk_size' contains the number of bytes for the stack.  If the stack
*                            units are INT32Us then, the stack contains '4 * stk_size' bytes.  The stack
*                            units are established by the #define constant OS_STK which is CPU
*                            specific.  'stk_size' is 0 if called by 'OSTaskCreate()'.
*
*              pext          is a pointer to a user supplied memory area that is used to extend the task
*                            control block.  This allows you to store the contents of floating-point
*                            registers, MMU registers or anything else you could find useful during a
*                            context switch.  You can even assign a name to each task and store this name
*                            in this TCB extension.  A NULL pointer is passed if called by OSTaskCreate().
*
*              opt           options as passed to 'OSTaskCreateExt()' or,
*                            0 if called from 'OSTaskCreate()'.
*
* Returns    : OS_ERR_NONE         if the call was successful
*              OS_ERR_TASK_NO_MORE_TCB  if there are no more free TCBs to be allocated and thus, the task cannot
*                                  be created.
*
* Note       : This function is INTERNAL to uC/OS-II and your application should not call it.
*********************************************************************************************************
*/
/*任务控制块初始化*/
INT8U  OS_TCBInit (INT8U    prio,		/*被创建任务的优先级*/
                   OS_STK  *ptos,		    /*任务堆栈的栈顶的地址*/
                   OS_STK  *pbos,		    /*任务堆栈发栈底的地址*/
                   INT16U   id,			    /*任务ID*/
                   INT32U   stk_size,	    /*任务栈大小*/
                   void    *pext,		    /*任务控制块的扩展的地址*/
                   INT16U   opt)		    /*其他选项*/
{
    OS_TCB    *ptcb;
#if OS_CRITICAL_METHOD == 3u                               /* Allocate storage for CPU status register */
    OS_CPU_SR  cpu_sr = 0u;
#endif
#if OS_TASK_REG_TBL_SIZE > 0u
    INT8U      i;
#endif

    OS_ENTER_CRITICAL();								   /*进入临界区*/
    ptcb = OSTCBFreeList;                                  /*获取空闲的任务控制块*/
    if (ptcb != (OS_TCB *)0) 
	{
        OSTCBFreeList            = ptcb->OSTCBNext;        /*把OSTCBFreeList指针移到链表的下一项*/
        OS_EXIT_CRITICAL();								   /*推出临界区*/
        ptcb->OSTCBStkPtr        = ptos;                   /*输入任务堆栈的栈顶的地址*/
        ptcb->OSTCBPrio          = prio;                   /*输入优先级*/
        ptcb->OSTCBStat          = OS_STAT_RDY;            /*刚创建的任务处于就绪态*/
        ptcb->OSTCBStatPend      = OS_STAT_PEND_OK;        /*没有等待任务事件*/
        ptcb->OSTCBDly           = 0u;                     /*没有设置延时*/

#if OS_TASK_CREATE_EXT_EN > 0u							   /*如果使用扩展功能*/
        ptcb->OSTCBExtPtr        = pext;                   /*扩展块地址*/
        ptcb->OSTCBStkSize       = stk_size;               /*堆栈大小*/
        ptcb->OSTCBStkBottom     = pbos;                   /*栈底*/
        ptcb->OSTCBOpt           = opt;                    /*扩展选项*/
        ptcb->OSTCBId            = id;                     /*任务ID*/
#else/*防止警告*/
        pext                     = pext;                   
        stk_size                 = stk_size;
        pbos                     = pbos;
        opt                      = opt;
        id                       = id;
#endif

#if OS_TASK_DEL_EN > 0u												/*如果允许删除任务*/
        ptcb->OSTCBDelReq        = OS_ERR_NONE;						/*任务删除标志置NONE*/
#endif

#if OS_LOWEST_PRIO <= 63u                                         
        ptcb->OSTCBY             = (INT8U)(prio >> 3u);				/*找到优先级在表的第几行*/
        ptcb->OSTCBX             = (INT8U)(prio & 0x07u);			/*找到优先级在表的第几列*/
#else                                                             
        ptcb->OSTCBY             = (INT8U)((INT8U)(prio >> 4u) & 0xFFu);
        ptcb->OSTCBX             = (INT8U) (prio & 0x0Fu);
#endif
                                                                  
        ptcb->OSTCBBitY          = (OS_PRIO)(1uL << ptcb->OSTCBY);	/*输入优先级对应表中的位置*/
        ptcb->OSTCBBitX          = (OS_PRIO)(1uL << ptcb->OSTCBX);


#if (OS_EVENT_EN)/*如果使用事件控制快*/
        ptcb->OSTCBEventPtr      = (OS_EVENT  *)0;					/*初始化ECB指针*/
#if (OS_EVENT_MULTI_EN > 0u)/*如果使用多事件*/
        ptcb->OSTCBEventMultiPtr = (OS_EVENT **)0;					/*初始化多事件指针*/
#endif
#endif



#if (OS_FLAG_EN > 0u) && (OS_MAX_FLAGS > 0u) && (OS_TASK_DEL_EN > 0u)
        ptcb->OSTCBFlagNode  = (OS_FLAG_NODE *)0;					/*初始化事件标志节点*/
#endif

#if (OS_MBOX_EN > 0u) || ((OS_Q_EN > 0u) && (OS_MAX_QS > 0u))
        ptcb->OSTCBMsg       = (void *)0;							/*初始化消息地址*/
#endif

#if OS_TASK_PROFILE_EN > 0u                                         /*如果使用ECB参数*/
        ptcb->OSTCBCtxSwCtr    = 0uL;                               /*切换到该任务的次数*/
        ptcb->OSTCBCyclesStart = 0uL;                               /*任务运行的总的时间周期*/
        ptcb->OSTCBCyclesTot   = 0uL;                               /* Snapshot of cycle counter at start of task resumption   */
        ptcb->OSTCBStkBase     = (OS_STK *)0;                       /*任务堆栈的起始地址*/
        ptcb->OSTCBStkUsed     = 0uL;                               /*任务堆栈中使用过的空间数*/
#endif

#if OS_TASK_NAME_EN > 0u
        ptcb->OSTCBTaskName    = (INT8U *)(void *)"?";
#endif

#if OS_TASK_REG_TBL_SIZE > 0u										/*是否使用任务注册表*/
        for (i = 0u; i < OS_TASK_REG_TBL_SIZE; i++)
		{
            ptcb->OSTCBRegTbl[i] = 0u;								/*初始化任务注册表*/
        }
#endif

        OSTCBInitHook(ptcb);							   /*空函数，内容需要读者填写，和Windows的钩子函数性质相同*/
        OSTaskCreateHook(ptcb);                            /*又是个钩子函数，用户创建任务钩子*/

		OS_ENTER_CRITICAL();							   /*进入临界区*/
        OSTCBPrioTbl[prio] = ptcb;						   /*把初始化任务控制块的地址给任务优先级指针表*/
		
        ptcb->OSTCBNext    = OSTCBList;						/*将当前控制块添加到就绪表最前面*/                 
        ptcb->OSTCBPrev    = (OS_TCB *)0;
        if (OSTCBList != (OS_TCB *)0) 
		{
            OSTCBList->OSTCBPrev = ptcb;
        }
        OSTCBList               = ptcb;



		
        OSRdyGrp               |= ptcb->OSTCBBitY;         /*设置就绪组*/
        OSRdyTbl[ptcb->OSTCBY] |= ptcb->OSTCBBitX;		   /*设置就绪表*/
        OSTaskCtr++;                                       /*任务数+1*/
        OS_EXIT_CRITICAL();								   /*退出临界区*/
        return (OS_ERR_NONE);
    }
    OS_EXIT_CRITICAL();
    return (OS_ERR_TASK_NO_MORE_TCB);
}

/*自定义区*/
INT8U FlagEn;






OS_TASK_SW(void)
{
	_asm							/*嵌入汇编代码，将各寄存器的值保存到堆栈中，为换出的环境做准备*/
	{
		lea eax,nextstart			/*将nextstart的地址送入EAX寄存器*/
		push eax					/*将nextstart地址入栈*/
		pushfd						/*标志寄存器值入栈*/
		pushad						/*依次把很多寄存器的值入栈*/
		mov ebx,[OSTCBCur]			/*当前任务控制块地址放入寄存器ebx*/
		mov [ebx],esp				/*把当前任务的栈顶指针保存到TCB结构中，因为TCB的第一项就是栈顶地址*/
	}								/*前面的一段汇编程序实现了把当前任务的返回地址入栈，寄存器的值入栈，把任务栈的地址保存到任务控制块，实现了运行环境的保存，以便之后恢复*/
	OSTaskSwHook();					/*钩子函数默认为空*/
	OSTCBCur = OSTCBHighRdy;		/*更新当前任务控制块指针指向的任务快*/
	OSPrioCur = y;					/*将当前任务的优先级转化为新任务的优先级*/

	_asm/*恢复运行环境*/
	{
		mov ebx,[OSTCBCur]			/*将目标任务堆栈的地址送入ebx*/
		mov esp,[ebx]				/*获取目标任务上次保存的esp*/
		popad						/*恢复通用寄存器的值*/
		popfd						/*恢复标志寄存器*/
		ret							/*跳转到指定任务运行，因为不能直接给指令寄存器直接赋值，所以要跳转到nextstart只能用ret，ret指令是将cs和ip赋堆栈里面存的值，这样程序就能从nextstart继续执行了*/
	}
	nextstart:						/*任务切换回来的运行地址*/
		return;

}



void OSIntCtxSw(void)
{
	/*入栈存CPU值*/
	/*全局变量赋值*/
	/*出栈恢复CPU值*/
}


void OSStartHighRdy(void)/*启动多任务*/
{
	OSTaskSwHook();					/*钩子函数*/
	OSRunning = OS_TRUE;			/*标志位变成启动多任务*/
	_asm
	{
		/*任务栈地址赋值给ebx*/
		/*esp指向任务堆栈*/
		/*恢复所有通用寄存器*/
		/*恢复标志寄存器*/
		/*在堆栈中取出任务地址并开始任务*/
	}
}

/*时钟中断服务函数*/
void OSTickISRuser()
{
	OSTime++;
	if(!FlagEn)/*当前中断被屏蔽或者处于临界区*/
	{
		return;
	}
	SuspendThread(mainhandle);/*中止主线程运行，模拟中断产生*/
	GetThreadContext(mainhandle,&Context);/*获取主线程的上下文*/
	OSIntEnter();/*嵌套数+1*/
	OSTCBCur->OSTCBStkPtr = (OS_STK *)Context.Esp;/*把当前任务的堆栈地址存到任务控制块*/
	OSTimeTick();/*找到优先级最高的任务*/
	OSIntExit();/*嵌套数-1*/
	ResumeThread(mainhandle);/*主线程继续执行，模拟中断返回*/
}

















