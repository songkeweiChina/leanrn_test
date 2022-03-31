#include "ucos_ii.h"

/*优先级判断表*/
INT8U  const  OSUnMapTbl[256] = 
{
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


void          OS_EXIT_CRITICAL();
void          OS_ENTER_CRITICAL();
void          OSTCBInitHook(OS_TCB          *ptcb);/*空函数*/
// void          OSTaskCreateHook(OS_TCB          *ptcb);/*创建钩子函数*/
// /*任务控制块初始化*/
// INT8U  OS_TCBInit (INT8U    prio,		    /*被创建任务的优先级*/
//                    OS_STK  *ptos,		    /*任务堆栈的栈顶的地址*/
//                    OS_STK  *pbos,		    /*任务堆栈发栈底的地址*/
//                    INT16U   id,			    /*任务ID*/
//                    INT32U   stk_size,	    /*任务栈大小*/
//                    void    *pext,		    /*任务控制块的扩展的地址*/
//                    INT16U   opt);		    /*其他选项*/





/*任务控制块初始化*/
INT8U  OS_TCBInit (INT8U    prio,		    /*被创建任务的优先级*/
                   OS_STK  *ptos,		    /*任务堆栈的栈顶的地址*/
                   OS_STK  *pbos,		    /*任务堆栈发栈底的地址*/
                   INT16U   id,			    /*任务ID*/
                   INT32U   stk_size,	    /*任务栈大小*/
                   void    *pext,		    /*任务控制块的扩展的地址*/
                   INT16U   opt)		    /*其他选项*/
{
    OS_TCB    *ptcb;
    INT8U i;
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
        ptcb->OSTCBEventPtr      = (OS_EVENT  *)0;                  /*初始化ECB指针*/
#if (OS_EVENT_MULTI_EN > 0u)/*如果使用多事件*/
        ptcb->OSTCBEventMultiPtr = (OS_EVENT **)0;                  /*初始化多事件指针*/
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
        ptcb->OSTCBNext    = OSTCBList;                    /*将当前控制块添加到就绪表最前面*/                 
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

void          OS_EXIT_CRITICAL()
{
    /*为了编译*/
}


void          OS_ENTER_CRITICAL()
{
    /*为了编译*/
}
void          OSTCBInitHook(OS_TCB          *ptcb)/*空函数*/
{
    /*为了编译*/
}
void          OSTaskCreateHook(OS_TCB          *ptcb)/*创建钩子函数*/
{
    /*为了编译*/
}
