/*
*********************************************************************************************************
*                                                uC/OS-II
*                                          The Real-Time Kernel
*                                             TIME MANAGEMENT
*
*                              (c) Copyright 1992-2009, Micrium, Weston, FL
*                                           All Rights Reserved
*
* File    : OS_TIME.C
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

/*
*********************************************************************************************************
*                                       DELAY TASK 'n' TICKS
*
* Description: This function is called to delay execution of the currently running task until the
*              specified number of system ticks expires.  This, of course, directly equates to delaying
*              the current task for some time to expire.  No delay will result If the specified delay is
*              0.  If the specified delay is greater than 0 then, a context switch will result.
*
* Arguments  : ticks     is the time delay that the task will be suspended in number of clock 'ticks'.
*                        Note that by specifying 0, the task will not be delayed.
*
* Returns    : none
*********************************************************************************************************
*/

/*任务延时函数*/
void  OSTimeDly (INT32U ticks)
{
    INT8U      y;
#if OS_CRITICAL_METHOD == 3u								/* Allocate storage for CPU status register           */
    OS_CPU_SR  cpu_sr = 0u;
#endif
    if (OSIntNesting > 0u)									/*中断中不能阻塞任务*/
	{
        return;
    }
    if (OSLockNesting > 0u)									/*调度器上锁不能延时，因为延时后会调度任务*/
	{
        return;
    }
    if (ticks > 0u)											/*延时时间是0，直接退出*/
	{
        OS_ENTER_CRITICAL();
        y            =  OSTCBCur->OSTCBY;					/*取消当前任务在就绪组和就绪表中的标志位*/
        OSRdyTbl[y] &= (OS_PRIO)~OSTCBCur->OSTCBBitX;		/*取消当前任务在就绪组和就绪表中的标志位*/
        if (OSRdyTbl[y] == 0u)
		{
            OSRdyGrp &= (OS_PRIO)~OSTCBCur->OSTCBBitY;
        }
        OSTCBCur->OSTCBDly = ticks;							/*延时时间赋到控制块*/
        OS_EXIT_CRITICAL();
        OS_Sched();											/*调度一下*/
    }
}

/*$PAGE*/
/*
*********************************************************************************************************
*                                     DELAY TASK FOR SPECIFIED TIME
*
* Description: This function is called to delay execution of the currently running task until some time
*              expires.  This call allows you to specify the delay time in HOURS, MINUTES, SECONDS and
*              MILLISECONDS instead of ticks.
*
* Arguments  : hours     specifies the number of hours that the task will be delayed (max. is 255)
*              minutes   specifies the number of minutes (max. 59)
*              seconds   specifies the number of seconds (max. 59)
*              ms        specifies the number of milliseconds (max. 999)
*
* Returns    : OS_ERR_NONE
*              OS_ERR_TIME_INVALID_MINUTES
*              OS_ERR_TIME_INVALID_SECONDS
*              OS_ERR_TIME_INVALID_MS
*              OS_ERR_TIME_ZERO_DLY
*              OS_ERR_TIME_DLY_ISR
*
* Note(s)    : The resolution on the milliseconds depends on the tick rate.  For example, you can't do
*              a 10 mS delay if the ticker interrupts every 100 mS.  In this case, the delay would be
*              set to 0.  The actual delay is rounded to the nearest tick.
*********************************************************************************************************
*/

/*任务按秒延迟函数*/
#if OS_TIME_DLY_HMSM_EN > 0u
INT8U  OSTimeDlyHMSM (INT8U   hours,
                      INT8U   minutes,
                      INT8U   seconds,
                      INT16U  ms)
{
    INT32U ticks;
    if (OSIntNesting > 0u)									/*中断中不能延时*/
	{
        return (OS_ERR_TIME_DLY_ISR);
    }
    if (OSLockNesting > 0u)									/*调度器上锁不能延时*/
	{
        return (OS_ERR_SCHED_LOCKED);
    }
#if OS_ARG_CHK_EN > 0u										/*做个容错*/
    if (hours == 0u)
	{
        if (minutes == 0u)
		{
            if (seconds == 0u)
			{
                if (ms == 0u)
				{
                    return (OS_ERR_TIME_ZERO_DLY);
                }
            }
        }
    }
    if (minutes > 59u)
	{
        return (OS_ERR_TIME_INVALID_MINUTES);
    }
    if (seconds > 59u)
	{
        return (OS_ERR_TIME_INVALID_SECONDS);
    }
    if (ms > 999u)
	{
        return (OS_ERR_TIME_INVALID_MS);
    }
#endif
	/*算一下有多少个时间片*/
    ticks = ((INT32U)hours * 3600uL + (INT32U)minutes * 60uL + (INT32U)seconds) * OS_TICKS_PER_SEC
          + OS_TICKS_PER_SEC * ((INT32U)ms + 500uL / OS_TICKS_PER_SEC) / 1000uL;
    OSTimeDly(ticks);
    return (OS_ERR_NONE);
}
#endif
/*$PAGE*/
/*
*********************************************************************************************************
*                                         RESUME A DELAYED TASK
*
* Description: This function is used resume a task that has been delayed through a call to either
*              OSTimeDly() or OSTimeDlyHMSM().  Note that you can call this function to resume a
*              task that is waiting for an event with timeout.  This would make the task look
*              like a timeout occurred.
*
* Arguments  : prio                      specifies the priority of the task to resume
*
* Returns    : OS_ERR_NONE               Task has been resumed
*              OS_ERR_PRIO_INVALID       if the priority you specify is higher that the maximum allowed
*                                        (i.e. >= OS_LOWEST_PRIO)
*              OS_ERR_TIME_NOT_DLY       Task is not waiting for time to expire
*              OS_ERR_TASK_NOT_EXIST     The desired task has not been created or has been assigned to a Mutex.
*********************************************************************************************************
*/

/*延时恢复函数*/
#if OS_TIME_DLY_RESUME_EN > 0u
INT8U  OSTimeDlyResume (INT8U prio)
{
    OS_TCB    *ptcb;
#if OS_CRITICAL_METHOD == 3u/* Storage for CPU status register      */
    OS_CPU_SR  cpu_sr = 0u;
#endif
    if(prio >= OS_LOWEST_PRIO)									/*无效优先级*/
	{
        return (OS_ERR_PRIO_INVALID);
    }
    OS_ENTER_CRITICAL();
    ptcb = OSTCBPrioTbl[prio];									/*找到优先级的任务控制块*/
    if(ptcb == (OS_TCB *)0)										/*控制块必须存在*/
	{
        OS_EXIT_CRITICAL();
        return (OS_ERR_TASK_NOT_EXIST);
    }
    if(ptcb == OS_TCB_RESERVED)
	{
        OS_EXIT_CRITICAL();										/*任务不存在，但是任务控制块被占用了*/
        return (OS_ERR_TASK_NOT_EXIST);
    }
    if(ptcb->OSTCBDly == 0u)									/*任务没有延时*/
	{
        OS_EXIT_CRITICAL();
        return (OS_ERR_TIME_NOT_DLY);
    }
    ptcb->OSTCBDly = 0u;										/*清除任务延时*/
    if((ptcb->OSTCBStat & OS_STAT_PEND_ANY) != OS_STAT_RDY)		/*如果任务有等待事件*/
	{
        ptcb->OSTCBStat     &= ~OS_STAT_PEND_ANY;				/*清除等待标志位*/
        ptcb->OSTCBStatPend  =  OS_STAT_PEND_TO;				/*表明timeout*/
    }
	else
	{
        ptcb->OSTCBStatPend = OS_STAT_PEND_OK;					/*只延时的任务，赋值为结束等待而提前结束*/
    }
    if ((ptcb->OSTCBStat & OS_STAT_SUSPEND) == OS_STAT_RDY)		/*如果任务不是被通过SUSPEND挂起的*/
	{
        OSRdyGrp               |= ptcb->OSTCBBitY;				/*让任务就绪起来*/
        OSRdyTbl[ptcb->OSTCBY] |= ptcb->OSTCBBitX;
        OS_EXIT_CRITICAL();
        OS_Sched();												/*调度一下*/
    }
	else
	{
        OS_EXIT_CRITICAL();										/*通过SUSPEND挂起的不能用这函数恢复，必须用Resume恢复*/
    }
    return (OS_ERR_NONE);
}
#endif
/*$PAGE*/
/*
*********************************************************************************************************
*                                         GET CURRENT SYSTEM TIME
*
* Description: This function is used by your application to obtain the current value of the 32-bit
*              counter which keeps track of the number of clock ticks.
*
* Arguments  : none
*
* Returns    : The current value of OSTime
*********************************************************************************************************
*/
/*获取当前时间*/
#if OS_TIME_GET_SET_EN > 0u
INT32U  OSTimeGet (void)
{
    INT32U     ticks;
#if OS_CRITICAL_METHOD == 3u                     /* Allocate storage for CPU status register           */
    OS_CPU_SR  cpu_sr = 0u;
#endif



    OS_ENTER_CRITICAL();
    ticks = OSTime;
    OS_EXIT_CRITICAL();
    return (ticks);
}
#endif

/*
*********************************************************************************************************
*                                            SET SYSTEM CLOCK
*
* Description: This function sets the 32-bit counter which keeps track of the number of clock ticks.
*
* Arguments  : ticks      specifies the new value that OSTime needs to take.
*
* Returns    : none
*********************************************************************************************************
*/

#if OS_TIME_GET_SET_EN > 0u
void  OSTimeSet (INT32U ticks)
{
#if OS_CRITICAL_METHOD == 3u                     /* Allocate storage for CPU status register           */
    OS_CPU_SR  cpu_sr = 0u;
#endif



    OS_ENTER_CRITICAL();
    OSTime = ticks;
    OS_EXIT_CRITICAL();
}
#endif
	 	   	  		 			 	    		   		 		 	 	 			 	    		   	 			 	  	 		 				 		  			 		 					 	  	  		      		  	   		      		  	 		 	      		   		 		  	 		 	      		  		  		  
