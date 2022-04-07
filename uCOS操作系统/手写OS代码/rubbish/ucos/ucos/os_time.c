#include <ucos_ii.h>



void  OSTimeDly(INT32U ticks);  /*������ʱ���� */
#if OS_TIME_GET_SET_EN > 0u
INT32U  OSTimeGet(void);/*��ȡ��ǰʱ�� */
#endif
#if OS_TIME_DLY_HMSM_EN > 0u
INT8U  OSTimeDlyHMSM(INT8U   hours, INT8U   minutes, INT8U   seconds, INT16U  ms);/*�������ӳٺ���*/
#endif
#if OS_TIME_DLY_RESUME_EN > 0u
INT8U  OSTimeDlyResume(INT8U prio);/*��ʱ�ָ�����*/
#endif
INT32U            OSTime;                   /*��ǰϵͳʱ�䣬���ȼ����� */










/*������ʱ���� */
void  OSTimeDly(INT32U ticks)
{
    INT8U      y;
    if (OSIntNesting > 0u)									/*�ж��в�����������*/
    {
        return;
    }
    if (OSLockNesting > 0u)									/*����������������ʱ����Ϊ��ʱ����������*/
    {
        return;
    }
    if (ticks > 0u)											/*��ʱʱ����0��ֱ���˳�*/
    {
        OS_ENTER_CRITICAL();
        y = OSTCBCur->OSTCBY;					/*ȡ����ǰ�����ھ�����;������еı�־λ*/
        OSRdyTbl[y] &= (OS_PRIO)~OSTCBCur->OSTCBBitX;		/*ȡ����ǰ�����ھ�����;������еı�־λ*/
        if (OSRdyTbl[y] == 0u)
        {
            OSRdyGrp &= (OS_PRIO)~OSTCBCur->OSTCBBitY;
        }
        OSTCBCur->OSTCBDly = ticks;							/*��ʱʱ�丳�����ƿ�*/
        OS_EXIT_CRITICAL();
        OS_Sched();											/*����һ��*/
    }
}
/*��ȡ��ǰʱ�� */
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


/*�������ӳٺ���*/
#if OS_TIME_DLY_HMSM_EN > 0u
INT8U  OSTimeDlyHMSM(INT8U   hours,
    INT8U   minutes,
    INT8U   seconds,
    INT16U  ms)
{
    INT32U ticks;
    if (OSIntNesting > 0u)									/*�ж��в�����ʱ*/
    {
        return (OS_ERR_TIME_DLY_ISR);
    }
    if (OSLockNesting > 0u)									/*����������������ʱ*/
    {
        return (OS_ERR_SCHED_LOCKED);
    }
#if OS_ARG_CHK_EN > 0u										/*�����ݴ�*/
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
    /*��һ���ж��ٸ�ʱ��Ƭ */
    ticks = ((INT32U)hours * 3600uL + (INT32U)minutes * 60uL + (INT32U)seconds) * OS_TICKS_PER_SEC
        + OS_TICKS_PER_SEC * ((INT32U)ms + 500uL / OS_TICKS_PER_SEC) / 1000uL;
    OSTimeDly(ticks);
    return (OS_ERR_NONE);
}
#endif


/*��ʱ�ָ�����*/
#if OS_TIME_DLY_RESUME_EN > 0u
INT8U  OSTimeDlyResume(INT8U prio)
{
    OS_TCB* ptcb;
    if (prio >= OS_LOWEST_PRIO)									/*��Ч���ȼ�*/
    {
        return (OS_ERR_PRIO_INVALID);
    }
    OS_ENTER_CRITICAL();
    ptcb = OSTCBPrioTbl[prio];									/*�ҵ����ȼ���������ƿ�*/
    if (ptcb == (OS_TCB*)0)										/*���ƿ�������*/
    {
        OS_EXIT_CRITICAL();
        return (OS_ERR_TASK_NOT_EXIST);
    }
    if (ptcb == OS_TCB_RESERVED)
    {
        OS_EXIT_CRITICAL();										/*���񲻴��ڣ�����������ƿ鱻ռ����*/
        return (OS_ERR_TASK_NOT_EXIST);
    }
    if (ptcb->OSTCBDly == 0u)									/*����û����ʱ*/
    {
        OS_EXIT_CRITICAL();
        return (OS_ERR_TIME_NOT_DLY);
    }
    ptcb->OSTCBDly = 0u;										/*���������ʱ*/
    if ((ptcb->OSTCBStat & OS_STAT_PEND_ANY) != OS_STAT_RDY)		/*��������еȴ��¼�*/
    {
        ptcb->OSTCBStat &= ~OS_STAT_PEND_ANY;				/*����ȴ���־λ*/
        ptcb->OSTCBStatPend = OS_STAT_PEND_TO;				/*����timeout*/
    }
    else
    {
        ptcb->OSTCBStatPend = OS_STAT_PEND_OK;					/*ֻ��ʱ�����񣬸�ֵΪ�����ȴ�����ǰ����*/
    }
    if ((ptcb->OSTCBStat & OS_STAT_SUSPEND) == OS_STAT_RDY)		/*��������Ǳ�ͨ��SUSPEND�����*/
    {
        OSRdyGrp |= ptcb->OSTCBBitY;				/*�������������*/
        OSRdyTbl[ptcb->OSTCBY] |= ptcb->OSTCBBitX;
        OS_EXIT_CRITICAL();
        OS_Sched();												/*����һ��*/
    }
    else
    {
        OS_EXIT_CRITICAL();										/*ͨ��SUSPEND����Ĳ������⺯���ָ���������Resume�ָ�*/
    }
    return (OS_ERR_NONE);
}
#endif