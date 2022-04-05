#include <ucos_ii.h>

/*û��չ���ܴ������� */
INT8U  OSTaskCreate(void   (*task)(void* p_arg),		/*��������ַ */
    void* p_arg,					/*������� */
    OS_STK* ptos,						/*����ջ�� */
    INT8U    prio);						/*��������ȼ� */
INT8U  OSTaskCreateExt(void   (*task)(void* p_arg),	/*��������ַ */
    void* p_arg,					/*������� */
    OS_STK* ptos,					/*����ջ��ջ�� */
    INT8U    prio,					/*�������ȼ� */
    INT16U   id,					/*����ID */
    OS_STK* pbos,					/*����ջջ��ָ�� */
    INT32U   stk_size,				/*����ջ��С */
    void* pext,					    /*��չ�ĵ�ַ */
    INT16U   opt);					/*����ĸ�����Ϣ */

/*��ջ��պ��� */
#if (OS_TASK_STAT_STK_CHK_EN > 0u) && (OS_TASK_CREATE_EXT_EN > 0u)
void  OS_TaskStkClr(OS_STK* pbos,
    INT32U   size,
    INT16U   opt);
#endif
void OS_Dummy();/*�պ��� */
void OSTaskDelHook(ptcb);              /*ɾ�����Ӻ��� */
#if OS_TASK_DEL_EN > 0u
INT8U  OSTaskDelReq(INT8U prio);       /*����ɾ������ */
#endif
#if OS_TASK_SUSPEND_EN > 0u
INT8U  OSTaskSuspend(INT8U prio);      /*�������� */
#endif
#if OS_TASK_SUSPEND_EN > 0u
INT8U  OSTaskResume(INT8U prio);       /*�ָ��������� */
#endif







/*û��չ���ܴ������� */
#if OS_TASK_CREATE_EN > 0u
INT8U  OSTaskCreate(void   (*task)(void* p_arg),		/*��������ַ */
    void* p_arg,					/*������� */
    OS_STK* ptos,						/*����ջ�� */
    INT8U    prio)						/*��������ȼ� */
{
    OS_STK* psp = (OS_TCB*)0;
    INT8U      err;
#if OS_ARG_CHK_EN > 0u									/*�Ƿ���в������ */
    if (prio > OS_LOWEST_PRIO) 							/*���ȼ��������ֵ */
    {
        return (OS_ERR_PRIO_INVALID);
    }
#endif
    OS_ENTER_CRITICAL();
    if (OSIntNesting > 0u) 								/*��������ж��� */
    {
        OS_EXIT_CRITICAL();
        return (OS_ERR_TASK_CREATE_ISR);				/*�ж��в����������� */
    }
    if (OSTCBPrioTbl[prio] == (OS_TCB*)0) 				/*���ȼ�ָ���ָ��0��ַ����ָ��0��ַ�Ļ���˵�������ȼ���ռ���� */
    {
        OSTCBPrioTbl[prio] = OS_TCB_RESERVED;			/*���ȼ�ָ����ַ�ݲ���ȷ����ʱ��ֵ */
        OS_EXIT_CRITICAL();
        psp = OSTaskStkInit(task, p_arg, ptos, 0u);		/*��ʼ��ջ */
        err = OS_TCBInit(prio, psp, (OS_STK*)0, 0u, 0u, (void*)0, 0u);/*��ʼ��������ƿ� */
        if (err == OS_ERR_NONE)
        {
            if (OSRunning == OS_TRUE) 					/*����������������ͽ���һ�ε��� */
            {
                OS_Sched();
            }
        }
        else
        {
            OS_ENTER_CRITICAL();
            OSTCBPrioTbl[prio] = (OS_TCB*)0;			/*û�����������ȼ���0 */
            OS_EXIT_CRITICAL();
        }
        return (err);
    }
    OS_EXIT_CRITICAL();
    return (OS_ERR_PRIO_EXIST);

}
#endif




/*����չ���ܴ������� */
#if OS_TASK_CREATE_EXT_EN > 0u
INT8U  OSTaskCreateExt(void   (*task)(void* p_arg),	/*��������ַ */
    void* p_arg,					/*������� */
    OS_STK* ptos,					/*����ջ��ջ�� */
    INT8U    prio,					/*�������ȼ� */
    INT16U   id,					/*����ID */
    OS_STK* pbos,					/*����ջջ��ָ�� */
    INT32U   stk_size,				/*����ջ��С */
    void* pext,					    /*��չ�ĵ�ַ */
    INT16U   opt)					/*����ĸ�����Ϣ */
{
    OS_STK* psp;
    INT8U      err;


#if OS_ARG_CHK_EN > 0u
    if (prio > OS_LOWEST_PRIO) /* ������ȼ�           */
    {
        return (OS_ERR_PRIO_INVALID);
    }
#endif
    OS_ENTER_CRITICAL();
    if (OSIntNesting > 0u)  /*��������ж��� */
    {
        OS_EXIT_CRITICAL();
        return (OS_ERR_TASK_CREATE_ISR);
    }
    if (OSTCBPrioTbl[prio] == (OS_TCB*)0)/*���ȼ�ָ���ָ��0��ַ����ָ��0��ַ�Ļ���˵�������ȼ���ռ���� */
    {
        OSTCBPrioTbl[prio] = OS_TCB_RESERVED;/*���ȼ�ָ����ַ�ݲ���ȷ����ʱ��ֵ */
        OS_EXIT_CRITICAL();
#if (OS_TASK_STAT_STK_CHK_EN > 0u)
        OS_TaskStkClr(pbos, stk_size, opt);                    /*��ն�ջ */
#endif

        psp = OSTaskStkInit(task, p_arg, ptos, opt);           /*��ʼ��ջ */
        err = OS_TCBInit(prio, psp, pbos, id, stk_size, pext, opt);/*��ʼ��������ƿ� */
        if (err == OS_ERR_NONE)
        {
            if (OSRunning == OS_TRUE)/*����������������ͽ���һ�ε��� */
            {
                OS_Sched();
            }
        }
        else
        {
            OS_ENTER_CRITICAL();
            OSTCBPrioTbl[prio] = (OS_TCB*)0;/*û�����������ȼ���0 */
            OS_EXIT_CRITICAL();
        }
        return (err);
    }
    OS_EXIT_CRITICAL();
    return (OS_ERR_PRIO_EXIST);
}
#endif

void OS_Dummy()/*�պ��� */
{
    /*Ϊ�˱��� */
}


/*��ջ��պ��� */
#if (OS_TASK_STAT_STK_CHK_EN > 0u) && (OS_TASK_CREATE_EXT_EN > 0u)
void  OS_TaskStkClr(OS_STK* pbos,
    INT32U   size,
    INT16U   opt)
{
    if ((opt & OS_TASK_OPT_STK_CHK) != 0x0000u) 		/* �����������ն�ջ */
    {
        if ((opt & OS_TASK_OPT_STK_CLR) != 0x0000u) 	/*��������˴�������ʱ��ն�ջ */
        {
#if OS_STK_GROWTH == 1u									/*�ж϶�ջ���� */
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

void OSTaskDelHook(ptcb)             /*ɾ�����Ӻ��� */
{
    /*ɾ�����Ӻ��� */
}






#if OS_TASK_DEL_EN > 0u
INT8U OSTaskDel(INT8U prio)
{
#if (OS_FLAG_EN > 0u) && (OS_MAX_FLAGS > 0u)
    OS_FLAG_NODE* pnode;
#endif
    OS_TCB* ptcb;
    if (OSIntNesting > 0u) /*�ж��в���ɾ������*/
    {
        return (OS_ERR_TASK_DEL_ISR);
    }
    if (prio == OS_TASK_IDLE_PRIO) /*������ɾ����������*/
    {
        return (OS_ERR_TASK_DEL_IDLE);
    }
#if OS_ARG_CHK_EN > 0u          /*��������˲������*/
    if (prio >= OS_LOWEST_PRIO) /*������ȼ�*/
    {
        if (prio != OS_PRIO_SELF) /*������ǵ�ǰ����*/
        {
            return (OS_ERR_PRIO_INVALID);
        }
    }
#endif
    OS_ENTER_CRITICAL();
    if (prio == OS_PRIO_SELF) /*�����ɾ���Լ�*/
    {
        prio = OSTCBCur->OSTCBPrio; /*��ȡ�Լ������ȼ�*/
    }
    ptcb = OSTCBPrioTbl[prio]; /*��ȡ������ַ*/
    if (ptcb == (OS_TCB*)0)   /*��������񲻴���*/
    {
        OS_EXIT_CRITICAL();
        return (OS_ERR_TASK_NOT_EXIST);
    }
    if (ptcb == OS_TCB_RESERVED) /*���������鱻����*/
    {
        OS_EXIT_CRITICAL();
        return (OS_ERR_TASK_DEL);
    }
    /*���������;�����*/
    OSRdyTbl[ptcb->OSTCBY] &= (OS_PRIO)~ptcb->OSTCBBitX;
    if (OSRdyTbl[ptcb->OSTCBY] == 0u)
    {
        OSRdyGrp &= (OS_PRIO)~ptcb->OSTCBBitY;
    }

#if (OS_EVENT_EN) /*�������ϵͳʹ���¼�����*/
    if (ptcb->OSTCBEventPtr != (OS_EVENT*)0)
    {
        OS_EventTaskRemove(ptcb, ptcb->OSTCBEventPtr); /* Remove this task from any event   wait list */
    }
#if (OS_EVENT_MULTI_EN > 0u)                        /*�����������ȴ����¼�*/
    if (ptcb->OSTCBEventMultiPtr != (OS_EVENT**)0) /* Remove this task from any events' wait lists*/
    {
        OS_EventTaskRemoveMulti(ptcb, ptcb->OSTCBEventMultiPtr);
    }
#endif
#endif

#if (OS_FLAG_EN > 0u) && (OS_MAX_FLAGS > 0u) /*�������ʹ��ʱ���־�����*/
    pnode = ptcb->OSTCBFlagNode;
    if (pnode != (OS_FLAG_NODE*)0) /*����¼��ٵȴ��¼���־*/
    {
        OS_FlagUnlink(pnode); /*ɾ���ȴ�������������ı�־��*/
    }
#endif

    ptcb->OSTCBDly = 0u;                   /*��������ڵȴ���ʱ�¼������ǾͲ���Ҫ�ȴ���*/
    ptcb->OSTCBStat = OS_STAT_RDY;         /*ȥ���ȴ��ȱ�־*/
    ptcb->OSTCBStatPend = OS_STAT_PEND_OK; /*���еȴ���ȡ����*/
    if (OSLockNesting < 255u)
    {
        OSLockNesting++; /*ǿ�н���������һ����*/
    }
    OS_EXIT_CRITICAL();
    OS_Dummy();          /*�պ��������ж�һ����ʱ��*/
    OS_ENTER_CRITICAL(); /*��ֹ�ж�*/
    if (OSLockNesting > 0u)
    {
        OSLockNesting--; /*��ԭ������*/
    }
    OSTaskDelHook(ptcb);              /*ɾ�����Ӻ���*/
    OSTaskCtr--;                      /*������-1*/
    OSTCBPrioTbl[prio] = (OS_TCB*)0; /*�������ȼ�ָ�����0*/
                                      /*�ѿ��ƿ�Ӿ����������õ�����������*/
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
    OS_EXIT_CRITICAL();       /*�뿪�ٽ��������ж� */
    if (OSRunning == OS_TRUE) /*������ж����񣬵���һ�� */
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
    if (prio == OS_TASK_IDLE_PRIO)								/*��鴫����������ȼ��Ƿ���Ч */
    {
        return (OS_ERR_TASK_DEL_IDLE);
    }
#if OS_ARG_CHK_EN > 0u
    if (prio >= OS_LOWEST_PRIO)									/*������鴫����������ȼ��Ƿ���Ч */
    {
        if (prio != OS_PRIO_SELF)
        {
            return (OS_ERR_PRIO_INVALID);
        }
    }
#endif
    if (prio == OS_PRIO_SELF)									/*���ɾ���Լ� */
    {
        OS_ENTER_CRITICAL();                                    /*�����ٽ��� */
        stat = OSTCBCur->OSTCBDelReq;                           /*������ƿ��д�����Ƿ���ɾ������ */
        OS_EXIT_CRITICAL();
        return (stat);
    }
    OS_ENTER_CRITICAL();										/*�����ٽ��� */
    ptcb = OSTCBPrioTbl[prio];									/*��ȡ������ƿ��ַ */
    if (ptcb == (OS_TCB*)0)
    {
        OS_EXIT_CRITICAL();
        return (OS_ERR_TASK_NOT_EXIST);                         /*�����ڷ��ش��� */
    }
    if (ptcb == OS_TCB_RESERVED)								/*���������ƿ鱻���� */
    {
        OS_EXIT_CRITICAL();
        return (OS_ERR_TASK_DEL);								/*���ش�����Ϣ */
    }
    ptcb->OSTCBDelReq = OS_ERR_TASK_DEL_REQ;                    /*�ڶԷ�������ƿ��ϴ���ɾ���ɹ��ı�־ */
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
    if (prio == OS_TASK_IDLE_PRIO)				/*������������������*/
    {
        return (OS_ERR_TASK_SUSPEND_IDLE);
    }
    if (prio >= OS_LOWEST_PRIO)					/*���ȼ���Ч*/
    {
        if (prio != OS_PRIO_SELF)				/*�����Լ�*/
        {
            return (OS_ERR_PRIO_INVALID);
        }
    }
#endif
    OS_ENTER_CRITICAL();
    if (prio == OS_PRIO_SELF)					/*��������Լ�*/
    {
        prio = OSTCBCur->OSTCBPrio;				/*��ȡ�Լ��������ȼ�*/
        self = OS_TRUE;
    }
    else if (prio == OSTCBCur->OSTCBPrio)		/*���������ǵ�ǰ����*/
    {
        self = OS_TRUE;
    }
    else
    {
        self = OS_FALSE;
    }
    ptcb = OSTCBPrioTbl[prio];					/*��ȡ��ǰ������ƿ��ַ*/
    if (ptcb == (OS_TCB*)0)					/*��ǰ���ƿ鲻����*/
    {
        OS_EXIT_CRITICAL();
        return (OS_ERR_TASK_SUSPEND_PRIO);
    }
    if (ptcb == OS_TCB_RESERVED)				/*���ƿ���Ҫ����*/
    {
        OS_EXIT_CRITICAL();
        return (OS_ERR_TASK_NOT_EXIST);
    }
    /*ȡ��������;������еı�־λ������һ��*/
    y = ptcb->OSTCBY;
    OSRdyTbl[y] &= (OS_PRIO)~ptcb->OSTCBBitX;
    if (OSRdyTbl[y] == 0u)
    {
        OSRdyGrp &= (OS_PRIO)~ptcb->OSTCBBitY;
    }
    ptcb->OSTCBStat |= OS_STAT_SUSPEND;			/*������񱻹�����*/
    OS_EXIT_CRITICAL();
    if (self == OS_TRUE)						/*�����������Լ��͵���һ�� */
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
    ptcb = OSTCBPrioTbl[prio];									/*��ȡ���ƿ��ַ*/
    if (ptcb == (OS_TCB*)0)									/*���ƿ鲻����*/
    {
        OS_EXIT_CRITICAL();
        return (OS_ERR_TASK_RESUME_PRIO);
    }
    if (ptcb == OS_TCB_RESERVED)								/*���ƿ鱻����*/
    {
        OS_EXIT_CRITICAL();
        return (OS_ERR_TASK_NOT_EXIST);
    }
    if ((ptcb->OSTCBStat & OS_STAT_SUSPEND) != OS_STAT_RDY)		/*�ָ���������SUSPEND״̬*/
    {
        ptcb->OSTCBStat &= (INT8U)~(INT8U)OS_STAT_SUSPEND;		/*�Ƴ�SUSPEND״̬*/
        if (ptcb->OSTCBStat == OS_STAT_RDY)						/*����Ǿ���̬*/
        {
            if (ptcb->OSTCBDly == 0u)							/*���û����ʱ*/
            {
                OSRdyGrp |= ptcb->OSTCBBitY;		/*���þ�����*/
                OSRdyTbl[ptcb->OSTCBY] |= ptcb->OSTCBBitX;		/*���þ�����*/
                OS_EXIT_CRITICAL();
                if (OSRunning == OS_TRUE)						/*����һ��*/
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
