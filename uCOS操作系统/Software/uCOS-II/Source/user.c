/*创建普通信号量访问资源*/
/*Task A*/
/*任务创建信号量然后周期性访问R资源*/
OS_EVENT* MyEventSem;
void UserTaskSemA(void *pParam)
{
	/*创建信号量*/
	INT8U *perr;
	INT8U err;
	INT8U i;
	OS_SEM_DATA mysemData;
	err = 0;
	perr = &err;
	MyEventSem = OSSemCreate(2);
	if(MyEventSem == (OS_EVENT *)0)
	{
		printf("创建信号量失败\n");
		OSTaskDel(OS_PRIO_SELF);
		return;
	}
	OSSemQuery(MyEventSem,&mysemData);
	printf("时间，当前信号量的值\n",OSTimeGet(),mysemData.OSCnt);
	while(1)
	{
		OSSemQuery(MyEventSem,&mysemData);
		printf("任务A准备申请信号量，当前时间，信号量的值\n",OSTimeGet(),mysemData.OSCnt);
		OSSemPend(MyEventSem,0,perr);
		if(err != OS_ERR_NONE)
		{
			printf("任务A请求信号量失败\n");
			printf("任务错误原因\n",err);
			continue;
		}
		OSSemQuery(MyEventSem,&mysemData);
		printf("任务A申请信号量OK，当前时间，信号量的值\n",OSTimeGet(),mysemData.OSCnt);
		OSTimeDly(1000);
		printf("任务A延时完毕准备提交信号量，当前时间，信号量的值\n",OSTimeGet(),mysemData.OSCnt);
		OSSemPost(MyEventSem);
		OSSemQuery(MyEventSem,&mysemData);
		printf("任务A提交信号量完毕，当前时间，信号量的值\n",OSTimeGet(),mysemData.OSCnt);
		OSTimeDly(1000);
	}
}














/*Task B*/
/*创建A创建信号量，然后周期性访问R资源*/
void UserTaskSemB(void *pParam)
{
	INT8U *perr;
	INT8U err;
	OS_SEM_DATA mysemData;
	err = 0;
	perr = &err;
	printf("创建任务B，当前时间是\n",OSTimeGet());
	OSTimeDly(300);
	if(MyEventSem == (OS_EVENT *)0)
	{
		printf("任务A创建信号量失败\n");
		OSTaskDel(OS_PRIO_SELF);
		return;
	}
	while(1)
	{
		OSSemQuery(MyEventSem,&mysemData);
		printf("任务B准备申请信号量，当前时间，信号量的值\n",OSTimeGet(),mysemData.OSCnt);
		OSSemPend(MyEventSem,0,perr);
		if(err != OS_ERR_NONE)
		{
			printf("任务B请求信号量失败\n，错误码是",err);
			continue;
		}
		OSSemQuery(MyEventSem,&mysemData);
		printf("任务B已经申请到了信号量，时间，信号量的值\n",OSTimeGet(),mysemData.OSCnt);
		OSTimeDly(1000);
		printf("任务B干完活儿了，准备释放信号量，当前时间\n",OSTimeGet());
		OSSemPost(MyEventSem);
		OSSemQuery(MyEventSem,&mysemData);
		printf("任务B提交完信号量了");
		OSTimeDly(1000);
	}
}















/*Task C*/
void UserTaskSemC(void *pParam)
{
	INT8U *perr;
	INT8U err;
	INT8U i;
	OS_SEM_DATA mysemData;
	err = 0;
	perr = &err;
	printf("任务C创建完成，延时400个时间片");
	OSTimeDly(400);
	if(MyEventSem == (OS_EVENT *)0)
	{
		printf("任务A创建信号量失败\n");
		OSTaskDel(OS_PRIO_SELF);
		return;
	}
	while(1)
	{
		OSSemQuery(MyEventSem,&mysemData);
		printf("任务C准备申请信号量，当前时间，信号量的值\n",OSTimeGet(),mysemData.OSCnt);
		OSSemPend(MyEventSem,0,perr);
		if(err != OS_ERR_NONE)
		{
			printf("任务C请求信号量失败\n，错误码是",err);
			continue;
		}
		OSSemQuery(MyEventSem,&mysemData);
		printf("任务C已经申请到了信号量，时间，信号量的值\n",OSTimeGet(),mysemData.OSCnt);
		OSTimeDly(1000);
		printf("任务C干完活儿了，准备释放信号量，当前时间\n",OSTimeGet());
		OSSemPost(MyEventSem);
		OSSemQuery(MyEventSem,&mysemData);
		printf("任务C提交完信号量了");
		OSTimeDly(1000);
	}
}





















































/*使用互斥信号量，使用优先级反转方法*/
/*Task  TaskMultx1*/
OS_EVENT *myMutex;
void TaskMultx1(void *pParam)
{
	INT8U *perr;
	INT8U err,i;
	INT32U j;
	perr = &err;
	err = OS_ERR_NONE;

	myMutex = OSMutexCreate(3,perr);			/*创建信号量，临时优先级设为3*/
	if(myMutex == (OS_EVENT *)0)				/*如果创建失败*/
	{
		printf("Task TaskMultx1创建信号量失败");
		OSTaskDel(OS_PRIO_SELF);				/*创建失败删除自己*/
		return;
	}
	printf("创建高优先级任务成功。创建互斥信号量成功");
	OSTimeDly(100);								/*等1秒然后申请信号量*/
	printf("高优先级任务准备申请互斥信号量");
	OSMutexPend(myMutex,0,perr);				/*等信号量*/
	if(*perr == OS_ERR_NONE)
	{
		for(i = 0;i < 5;i++)
		{
			printf("操作串口");					/*操作IO*/
			for(j = 0;j<99999999;j--);			/*假装操作串口*/
		}
	}
	else
	{
		printf("申请信号量失败");
	}
	OSMutexPost();								/*提交信号量*/
	for(i = 0;i < 5;i++)
	{
		printf("高优先级任务执行完信号量后，做其他操作");
		for(j = 0;j<99999999;j--);				/*模拟做其他工作*/
	}
	printf("高优先级任务结束，准备注销自己");
	OSTaskDel(OS_PRIO_SELF);
	return;
}
/*Task  TaskMultx2*/ 
void TaskMultx2(void *pParam)
{
	 INT8U *perr;
	 INT8U err,i;
	 INT32U j;
	 perr = &err;
	 err = OS_ERR_NONE;

	 if(myMutex == (OS_EVENT *)0)				/*如果信号量没被创建*/
	 {
	 	printf("互斥信号量创建失败");
		OSTaskDel(OS_PRIO_SELF);
		return;
	 }
	 OSTimeDly(90);
	 OSMutexPend(myMutex,0,perr);				/*等待互斥信号量*/
	 if(*perr == OS_ERR_NONE)
	 {
	 	printf("任务TaskMultx2申请互斥信号量成功");
		for(i = 0;i < 5;i++)
		{
			printf("操作串口");
			for(j = 0;j<99999999;j--);			/*模拟操作串口*/
		}
	 }
	 else
	 {
	 	printf("请求信号量失败");
	 }
	 OSMutexPost();								/*提交信号量*/
	 for(i = 0;i < 5;i++)
	 {
		 printf("中优先级任务执行完信号量后，做其他操作");
		 for(j = 0;j<99999999;j--); 			 /*模拟做其他工作*/
	 }
	 printf("高优先级任务结束，准备注销自己");
	 OSTaskDel(OS_PRIO_SELF);
	 return;
}
/*Task  打印任务*/ 
void TaskPrint(void *pParam)
{
	INT8U *perr;
	INT8U err,i;
 	INT32U j;
 	perr = &err;
 	err = OS_ERR_NONE;
	i = 0;
	OSTimeDly(95);
	/*略*/
}
































 /*消息邮箱实验*/
OS_EVENT *myMBox;
 void TaskMessageSen(void *pParam)
 {
 	INT8U *perr;
	INT8U err,i;
	INT32U j;
	INT32U scount;
	perr = &err;
	err = OS_ERR_NONE;
	scount = 0;
	myMBox = OSMboxCreate(&scount);
	if(myMBox == (OS_EVENT *)0)		/*创建失败*/
	{
		printf("创建邮箱失败");
		OSTaskDel(OS_PRIO_SELF);
		return;
	}
	printf("创建成功");
	while(1)
	{
	}
 }

 void TaskMessageRec(void *pParam)
 {
 }













































































 
 printf(void)
 {
	 /*打印*/
 }

 
