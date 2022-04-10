typedef unsigned int OS_STK;
typedef unsigned int INT32U;
typedef unsigned short INT16U;
typedef unsigned char INT8U;


#define OS_TASK_SUSPEND_EN        1u   /*是否允许挂起任务 */
#define OS_TASK_CREATE_EXT_EN     0u   /*是否使用扩展功能 */
#define OS_Q_EN                   1u   /*使用队列 */
#define OS_MBOX_EN                1u   /*是否使用邮箱 */
#define OS_MUTEX_EN               1u   /*是否使用互斥锁 */
#define OS_EVENT_MULTI_EN         1u   /*允许任务等待多事件 *//* Include code for OSEventPendMulti()                          */
#define OS_FLAG_EN                1u   /*使用事件的标志 */
#define OS_TASK_DEL_EN            1u   /*是否允许删除任务 */																										/*     Include code for OSTaskDel()                             */
#define OS_TASK_PROFILE_EN        1u   /*是否使用TCB中的用于分析的参数 */
#define OS_TASK_NAME_EN           1u   /*是否允许使用任务名 */
#define OS_TASK_REG_TBL_SIZE      1u   /*是否使用任务注册表 */
#define OS_FLAGS_NBITS           16u   /*配置事件标志大小 */
#define OS_LOWEST_PRIO           63u   /*最低优先级的任务优先级，也就是最大任务的个数 */
#define OS_TASK_STAT_EN           1u   /*是否开启统计任务 */
#define TASK_STK_SIZE           512U   /*任务堆栈大小 */
#define OS_MAX_EVENTS            10u   /*应用程序中最大事件控制块的数量  */
#define OS_MAX_FLAGS              5u   /*事件标志组的最大数量 */
#define OS_MAX_MEM_PART           5u   /* Max. number of memory partitions                             */
#define OS_MAX_QS                 4u   /*队列控制块的最大数量 */
#define OS_MAX_TASKS             20u   /*最多的用户任务数 */
#define OS_DEBUG_EN               1u   /* Enable(1) debug variables                                    */
#define OS_EVENT_NAME_EN          1u   /* Enable names for Sem, Mutex, Mbox and Q                      */
/* --------------------- MEMORY MANAGEMENT -------------------- */
#define OS_MEM_EN                 1u   /* Enable (1) or Disable (0) code generation for MEMORY MANAGER */
#define OS_MEM_NAME_EN            1u   /*     Enable memory partition names                            */
#define OS_MEM_QUERY_EN           1u   /*     Include code for OSMemQuery()                            */

                                       /* --------------------- TIMER MANAGEMENT --------------------- */
#define OS_TMR_EN                 1u   /* Enable (1) or Disable (0) code generation for TIMERS         */
#define OS_TMR_CFG_MAX           16u   /*     Maximum number of timers                                 */
#define OS_TMR_CFG_NAME_EN        1u   /*     Determine timer names                                    */
#define OS_TMR_CFG_WHEEL_SIZE     8u   /*     Size of timer wheel (#Spokes)                            */
#define OS_TMR_CFG_TICKS_PER_SEC 10u   /*     Rate at which timer management task runs (Hz)            */



/* --------------------- 时间管理 ---------------------- */
#define OS_TIME_DLY_HMSM_EN       1u   /*     Include code for OSTimeDlyHMSM()                         */
#define OS_TIME_DLY_RESUME_EN     1u   /*     Include code for OSTimeDlyResume()                       */
#define OS_TIME_GET_SET_EN        1u   /*     Include code for OSTimeGet() and OSTimeSet()             */
#define OS_TIME_TICK_HOOK_EN      1u   /*     Include code for OSTimeTickHook()                        */
#define OS_TICK_STEP_EN           0u   /* Enable tick stepping feature for uC/OS-View                  */


/* ---------------------任务管理---------------------- */
#define OS_TASK_CHANGE_PRIO_EN    1u   /*     Include code for OSTaskChangePrio()                      */
#define OS_TASK_CREATE_EN         1u   /*     Include code for OSTaskCreate()                          */

#define OS_APP_HOOKS_EN           1u   /* Application-defined hooks are called from the uC/OS-II hooks */
#define OS_ARG_CHK_EN             1u   /*是否参数检查 */
#define OS_CPU_HOOKS_EN           1u   /* uC/OS-II hooks are found in the processor port files         */
#define OS_TASK_STAT_STK_CHK_EN   0u   /*     Check task stacks from statistic task                    */
#define OS_STK_GROWTH             1u   /*栈的生长方向 */

                                       /* --------------------- TASK STACK SIZE ---------------------- */
#define OS_TASK_TMR_STK_SIZE    128u   /* Timer      task stack size (# of OS_STK wide entries)        */
#define OS_TASK_STAT_STK_SIZE   128u   /* Statistics task stack size (# of OS_STK wide entries)        */
#define OS_TASK_IDLE_STK_SIZE   128u   /* Idle       task stack size (# of OS_STK wide entries)        */
#define OS_TICK_STEP_EN           0u   /* Enable tick stepping feature for uC/OS-View                  */
#define OS_TICKS_PER_SEC        100u   /*每秒时钟中断发生的次数 *//* Set the number of ticks in one second                        */

                                       /* ------------------------ SEMAPHORES ------------------------ */
#define OS_SEM_EN                 1u   /*是否使用信号量*/
#define OS_SEM_ACCEPT_EN          1u   /*    Include code for OSSemAccept()                            */
#define OS_SEM_DEL_EN             1u   /*    Include code for OSSemDel()                               */
#define OS_SEM_PEND_ABORT_EN      1u   /*    Include code for OSSemPendAbort()                         */
#define OS_SEM_QUERY_EN           1u   /*    Include code for OSSemQuery()                             */
#define OS_SEM_SET_EN             1u   /*    Include code for OSSemSet()                               */

#define  OS_DEL_NO_PEND                 0u/*有任务等信号量就不能删除信号量 */
#define  OS_DEL_ALWAYS                  1u/*强制删除信号量，不管有没有任务在等 */

#define  OS_PEND_OPT_NONE               0u  /*只让最高优先级的退出等待 *//* NO option selected                                      */
#define  OS_PEND_OPT_BROADCAST          1u  /*让所有的任务退出等待 *//* Broadcast action to ALL tasks waiting                   */