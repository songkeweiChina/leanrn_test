/*������ͨ�ź���������Դ*/
/*Task A*/
/*���񴴽��ź���Ȼ�������Է���R��Դ*/
OS_EVENT* MyEventSem;
void UserTaskSemA(void *pParam)
{
	/*�����ź���*/
	INT8U *perr;
	INT8U err;
	INT8U i;
	OS_SEM_DATA mysemData;
	err = 0;
	perr = &err;
	MyEventSem = OSSemCreate(2);
	if(MyEventSem == (OS_EVENT *)0)
	{
		printf("�����ź���ʧ��\n");
		OSTaskDel(OS_PRIO_SELF);
		return;
	}
	OSSemQuery(MyEventSem,&mysemData);
	printf("ʱ�䣬��ǰ�ź�����ֵ\n",OSTimeGet(),mysemData.OSCnt);
	while(1)
	{
		OSSemQuery(MyEventSem,&mysemData);
		printf("����A׼�������ź�������ǰʱ�䣬�ź�����ֵ\n",OSTimeGet(),mysemData.OSCnt);
		OSSemPend(MyEventSem,0,perr);
		if(err != OS_ERR_NONE)
		{
			printf("����A�����ź���ʧ��\n");
			printf("�������ԭ��\n",err);
			continue;
		}
		OSSemQuery(MyEventSem,&mysemData);
		printf("����A�����ź���OK����ǰʱ�䣬�ź�����ֵ\n",OSTimeGet(),mysemData.OSCnt);
		OSTimeDly(1000);
		printf("����A��ʱ���׼���ύ�ź�������ǰʱ�䣬�ź�����ֵ\n",OSTimeGet(),mysemData.OSCnt);
		OSSemPost(MyEventSem);
		OSSemQuery(MyEventSem,&mysemData);
		printf("����A�ύ�ź�����ϣ���ǰʱ�䣬�ź�����ֵ\n",OSTimeGet(),mysemData.OSCnt);
		OSTimeDly(1000);
	}
}














/*Task B*/
/*����A�����ź�����Ȼ�������Է���R��Դ*/
void UserTaskSemB(void *pParam)
{
	INT8U *perr;
	INT8U err;
	OS_SEM_DATA mysemData;
	err = 0;
	perr = &err;
	printf("��������B����ǰʱ����\n",OSTimeGet());
	OSTimeDly(300);
	if(MyEventSem == (OS_EVENT *)0)
	{
		printf("����A�����ź���ʧ��\n");
		OSTaskDel(OS_PRIO_SELF);
		return;
	}
	while(1)
	{
		OSSemQuery(MyEventSem,&mysemData);
		printf("����B׼�������ź�������ǰʱ�䣬�ź�����ֵ\n",OSTimeGet(),mysemData.OSCnt);
		OSSemPend(MyEventSem,0,perr);
		if(err != OS_ERR_NONE)
		{
			printf("����B�����ź���ʧ��\n����������",err);
			continue;
		}
		OSSemQuery(MyEventSem,&mysemData);
		printf("����B�Ѿ����뵽���ź�����ʱ�䣬�ź�����ֵ\n",OSTimeGet(),mysemData.OSCnt);
		OSTimeDly(1000);
		printf("����B�������ˣ�׼���ͷ��ź�������ǰʱ��\n",OSTimeGet());
		OSSemPost(MyEventSem);
		OSSemQuery(MyEventSem,&mysemData);
		printf("����B�ύ���ź�����");
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
	printf("����C������ɣ���ʱ400��ʱ��Ƭ");
	OSTimeDly(400);
	if(MyEventSem == (OS_EVENT *)0)
	{
		printf("����A�����ź���ʧ��\n");
		OSTaskDel(OS_PRIO_SELF);
		return;
	}
	while(1)
	{
		OSSemQuery(MyEventSem,&mysemData);
		printf("����C׼�������ź�������ǰʱ�䣬�ź�����ֵ\n",OSTimeGet(),mysemData.OSCnt);
		OSSemPend(MyEventSem,0,perr);
		if(err != OS_ERR_NONE)
		{
			printf("����C�����ź���ʧ��\n����������",err);
			continue;
		}
		OSSemQuery(MyEventSem,&mysemData);
		printf("����C�Ѿ����뵽���ź�����ʱ�䣬�ź�����ֵ\n",OSTimeGet(),mysemData.OSCnt);
		OSTimeDly(1000);
		printf("����C�������ˣ�׼���ͷ��ź�������ǰʱ��\n",OSTimeGet());
		OSSemPost(MyEventSem);
		OSSemQuery(MyEventSem,&mysemData);
		printf("����C�ύ���ź�����");
		OSTimeDly(1000);
	}
}





















































/*ʹ�û����ź�����ʹ�����ȼ���ת����*/
/*Task  TaskMultx1*/
OS_EVENT *myMutex;
void TaskMultx1(void *pParam)
{
	INT8U *perr;
	INT8U err,i;
	INT32U j;
	perr = &err;
	err = OS_ERR_NONE;

	myMutex = OSMutexCreate(3,perr);			/*�����ź�������ʱ���ȼ���Ϊ3*/
	if(myMutex == (OS_EVENT *)0)				/*�������ʧ��*/
	{
		printf("Task TaskMultx1�����ź���ʧ��");
		OSTaskDel(OS_PRIO_SELF);				/*����ʧ��ɾ���Լ�*/
		return;
	}
	printf("���������ȼ�����ɹ������������ź����ɹ�");
	OSTimeDly(100);								/*��1��Ȼ�������ź���*/
	printf("�����ȼ�����׼�����뻥���ź���");
	OSMutexPend(myMutex,0,perr);				/*���ź���*/
	if(*perr == OS_ERR_NONE)
	{
		for(i = 0;i < 5;i++)
		{
			printf("��������");					/*����IO*/
			for(j = 0;j<99999999;j--);			/*��װ��������*/
		}
	}
	else
	{
		printf("�����ź���ʧ��");
	}
	OSMutexPost();								/*�ύ�ź���*/
	for(i = 0;i < 5;i++)
	{
		printf("�����ȼ�����ִ�����ź���������������");
		for(j = 0;j<99999999;j--);				/*ģ������������*/
	}
	printf("�����ȼ����������׼��ע���Լ�");
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

	 if(myMutex == (OS_EVENT *)0)				/*����ź���û������*/
	 {
	 	printf("�����ź�������ʧ��");
		OSTaskDel(OS_PRIO_SELF);
		return;
	 }
	 OSTimeDly(90);
	 OSMutexPend(myMutex,0,perr);				/*�ȴ������ź���*/
	 if(*perr == OS_ERR_NONE)
	 {
	 	printf("����TaskMultx2���뻥���ź����ɹ�");
		for(i = 0;i < 5;i++)
		{
			printf("��������");
			for(j = 0;j<99999999;j--);			/*ģ���������*/
		}
	 }
	 else
	 {
	 	printf("�����ź���ʧ��");
	 }
	 OSMutexPost();								/*�ύ�ź���*/
	 for(i = 0;i < 5;i++)
	 {
		 printf("�����ȼ�����ִ�����ź���������������");
		 for(j = 0;j<99999999;j--); 			 /*ģ������������*/
	 }
	 printf("�����ȼ����������׼��ע���Լ�");
	 OSTaskDel(OS_PRIO_SELF);
	 return;
}
/*Task  ��ӡ����*/ 
void TaskPrint(void *pParam)
{
	INT8U *perr;
	INT8U err,i;
 	INT32U j;
 	perr = &err;
 	err = OS_ERR_NONE;
	i = 0;
	OSTimeDly(95);
	/*��*/
}
































 /*��Ϣ����ʵ��*/
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
	if(myMBox == (OS_EVENT *)0)		/*����ʧ��*/
	{
		printf("��������ʧ��");
		OSTaskDel(OS_PRIO_SELF);
		return;
	}
	printf("�����ɹ�");
	while(1)
	{
	}
 }

 void TaskMessageRec(void *pParam)
 {
 }













































































 
 printf(void)
 {
	 /*��ӡ*/
 }

 
