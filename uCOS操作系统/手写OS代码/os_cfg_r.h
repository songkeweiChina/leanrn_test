typedef unsigned int OS_STK;
typedef unsigned int INT32U;
typedef unsigned short INT16U;
typedef unsigned char INT8U;


#define OS_TASK_CREATE_EXT_EN     1u   /*是否使用扩展功能*/
#define OS_Q_EN                   1u   /*使用队列*/
#define OS_MAX_QS                 4u   /*队列控制块的最大数量*/
#define OS_MBOX_EN                1u   /*是否使用邮箱*/
#define OS_SEM_EN                 1u   /*是否使用信号量*/
#define OS_MUTEX_EN               1u   /*是否使用互斥锁*/
#define OS_EVENT_MULTI_EN         1u   /*允许任务等待多事件*//* Include code for OSEventPendMulti()                          */
#define OS_FLAG_EN                1u   /*使用事件的标志*/
#define OS_MAX_FLAGS              5u   /*事件标志组的最大数量*/
#define OS_TASK_DEL_EN            1u   /*是否允许删除任务*/																										/*     Include code for OSTaskDel()                             */
#define OS_TASK_PROFILE_EN        1u   /*是否使用TCB中的用于分析的参数*/
#define OS_TASK_NAME_EN           1u   /*是否允许使用任务名*/
#define OS_TASK_REG_TBL_SIZE      1u   /*是否使用任务注册表*/
#define OS_FLAGS_NBITS           16u   /*配置事件标志大小*/
#define OS_LOWEST_PRIO           63u   /*最低优先级的任务优先级，也就是最大任务的个数*/
#define OS_MAX_TASKS             20u   /*最多的用户任务数*/
#define OS_TASK_STAT_EN           1u   /*是否开启统计任务*/
#define TASK_STK_SIZE           512U   /*任务堆栈大小*/


