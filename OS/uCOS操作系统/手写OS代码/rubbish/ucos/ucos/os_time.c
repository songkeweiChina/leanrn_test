#include <ucos_ii.h>



void  OSTimeDly(INT32U ticks);  /*任务延时函数 */
#if OS_TIME_GET_SET_EN > 0u
INT32U  OSTimeGet(void);/*获取当前时间 */
#endif
#if OS_TIME_DLY_HMSM_EN > 0u
INT8U  OSTimeDlyHMSM(INT8U   hours, INT8U   minutes, INT8U   seconds, INT16U  ms);/*任务按秒延迟函数*/
#endif
#if OS_TIME_DLY_RESUME_EN > 0u
INT8U  OSTimeDlyResume(INT8U prio);/*延时恢复函数*/
#endif
INT32U            OSTime;                   /*当前系统时间，调度计数器 */










/*任务延时函数 */
void  OSTimeDly(INT32U ticks)
{
    INT8U      y;
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
        y = OSTCBCur->OSTCBY;					/*取消当前任务在就绪组和就绪表中的标志位*/
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
/*获取当前时间 */
#if OS_TIME_GET_SET_EN > 0u
INT32U  OSTimeGet(void)
{
    INT32U     ticks;

    OS_ENTER_CRITICAL();
    ticks = OSTime;
    OS_EXIT_CRITICAL();
    return (ticks);
}
#endif


/*任务按秒延迟函数*/
#if OS_TIME_DLY_HMSM_EN > 0u
INT8U  OSTimeDlyHMSM(INT8U   hours,
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
    /*算一下有多少个时间片 */
    ticks = ((INT32U)hours * 3600uL + (INT32U)minutes * 60uL + (INT32U)seconds) * OS_TICKS_PER_SEC
        + OS_TICKS_PER_SEC * ((INT32U)ms + 500uL / OS_TICKS_PER_SEC) / 1000uL;
    OSTimeDly(ticks);
    return (OS_ERR_NONE);
}
#endif


/*延时恢复函数*/
#if OS_TIME_DLY_RESUME_EN > 0u
INT8U  OSTimeDlyResume(INT8U prio)
{
    OS_TCB* ptcb;
    if (prio >= OS_LOWEST_PRIO)									/*无效优先级*/
    {
        return (OS_ERR_PRIO_INVALID);
    }
    OS_ENTER_CRITICAL();
    ptcb = OSTCBPrioTbl[prio];									/*找到优先级的任务控制块*/
    if (ptcb == (OS_TCB*)0)										/*控制块必须存在*/
    {
        OS_EXIT_CRITICAL();
        return (OS_ERR_TASK_NOT_EXIST);
    }
    if (ptcb == OS_TCB_RESERVED)
    {
        OS_EXIT_CRITICAL();										/*任务不存在，但是任务控制块被占用了*/
        return (OS_ERR_TASK_NOT_EXIST);
    }
    if (ptcb->OSTCBDly == 0u)									/*任务没有延时*/
    {
        OS_EXIT_CRITICAL();
        return (OS_ERR_TIME_NOT_DLY);
    }
    ptcb->OSTCBDly = 0u;										/*清除任务延时*/
    if ((ptcb->OSTCBStat & OS_STAT_PEND_ANY) != OS_STAT_RDY)		/*如果任务有等待事件*/
    {
        ptcb->OSTCBStat &= ~OS_STAT_PEND_ANY;				/*清除等待标志位*/
        ptcb->OSTCBStatPend = OS_STAT_PEND_TO;				/*表明timeout*/
    }
    else
    {
        ptcb->OSTCBStatPend = OS_STAT_PEND_OK;					/*只延时的任务，赋值为结束等待而提前结束*/
    }
    if ((ptcb->OSTCBStat & OS_STAT_SUSPEND) == OS_STAT_RDY)		/*如果任务不是被通过SUSPEND挂起的*/
    {
        OSRdyGrp |= ptcb->OSTCBBitY;				/*让任务就绪起来*/
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