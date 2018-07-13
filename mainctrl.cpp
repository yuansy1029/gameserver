#include "mainctrl.h"
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <signal.h>
#include <string.h>
#include <sys/file.h>
#include <stdlib.h>

#include "common/epoll.h"
#include "common/anet.h"

SGEpollEventLoop *pTmpEventLoop;
volatile static int g_iSignalFlag = 0;
#define SIGNAL_QUIT				1
#define SIGNAL_RELOAD			2


//char g_pTempMainThreadSendBuffer[SERVERCONFIG_COMMON_SERIALIZE_TEMP_BUFF];
//int g_iTempMainThreadSendBufferLength = sizeof(g_pTempMainThreadSendBuffer);

void SigUser1Handler( int iSigVal )
{
	g_iSignalFlag = SIGNAL_RELOAD;
	signal(SIGUSR1, SigUser1Handler);
}

void SigUser2Handler( int iSigVal )
{
	g_iSignalFlag = SIGNAL_QUIT;
	signal(SIGUSR2, SigUser2Handler);
}

/////////
SGMainCtrl::SGMainCtrl()
{

}

SGMainCtrl::~SGMainCtrl()
{

}


int SGMainCtrl::InstallSignal()
{
	signal(SIGUSR1,SigUser1Handler);
	signal(SIGUSR2,SigUser2Handler);
	return 0;
}

int SGMainCtrl::IgnoreSignal()
{
	signal(SIGINT, SIG_IGN);
	signal(SIGHUP, SIG_IGN);
	signal(SIGQUIT, SIG_IGN);
	signal(SIGPIPE, SIG_IGN);
	signal(SIGTTOU, SIG_IGN);
	signal(SIGTTIN, SIG_IGN);
	signal(SIGCHLD, SIG_IGN);
	signal(SIGTERM, SIG_IGN);
	signal(SIGHUP, SIG_IGN);
	signal(SIGPIPE, SIG_IGN);

	return 0;
}


void SGMainCtrl::InitDaemonBlockParent()
{

	m_stDaemonWait.Start();

	IgnoreSignal();

	pid_t tPID;
	if ((tPID = fork()) != 0)
	{
		char szErrStr[128];
		int iExitCode = 0;
		if (m_stDaemonWait.WaitChild(szErrStr, sizeof(szErrStr)))
		{
			fprintf(stderr, "Run failed reason1111:%s\n", szErrStr);
			iExitCode = 1;
		}
		exit(iExitCode);
	}

	setsid();

	if ((tPID = fork()) != 0)
	{
		exit(0);
	}

	umask(0);
}

void SGMainCtrl::ReleaseParent(int iRetVal)
{

	m_stDaemonWait.NotifyParent(iRetVal);
}

int SGMainCtrl::PrepareRun()
{
	//InitDaemonBlockParent();

	int iRet = 0;

	iRet = Initialize();
	if (iRet)
	{
		ReleaseParent(iRet - 2000);
		exit(iRet);
	}

	ReleaseParent(0);

	return 0;
}

//开始监听网络
int SGMainCtrl::StartNetwork()
{
	ERROR_LOG("________________________________GAMESVR StartNetwork BEGIN________________________________\n");
	int iRet = 0;
	char err[256];
	SGAnet *pConn = new SGAnet();
	int count = 0;
	m_listenfd = pConn->anetTcpServer(err,9999,"127.0.0.1",1);
	printf("xxxxxxxx\n");
	if (m_listenfd == -1)
	{
		printf("listen failed,%s",err);
	}
	if (pConn->anetNonBlock(err,m_listenfd) == -1)
	{
		printf("set failed,%s",err);
	}
	printf("777777777777\n");
	//添加监听事件
	pTmpEventLoop->AddEvent(m_listenfd, EventType_Read);

	ERROR_LOG("________________________________GAMESVR StartNetwork END________________________________\n");

	return 0;
}

int InitEventLoop()
{
	pTmpEventLoop = new SGEpollEventLoop(1000);
	int iRet = pTmpEventLoop->Initialize();
	if (iRet)
	{
		printf("pTmpEventLoop->Initialize() failed got:%d\n",iRet);
		return -1;
	}

	return 0;
}

int SGMainCtrl::Initialize()
{
	ERROR_LOG("________________________________GAMESVR Initialize BEGIN________________________________\n");

	int iRet = 0;

	//注册信号
	iRet = InstallSignal();
	if (iRet)
	{
		ERROR_LOG("InstallSignal failed:%d\n", iRet);
		return -1;
	}

	//eventloop初始化
	iRet = InitEventLoop();
	if (iRet)
	{
		ERROR_LOG("pEventLoop->InitEventLoop() failed:%d\n", iRet);
		return -3;
	}

	///开始监听网络

	iRet = StartNetwork();
	if (iRet)
	{
		ERROR_LOG("StartNetwork() failed:%d\n", iRet);
		return -999;
	}
	ERROR_LOG("________________________________GAMESVR Initialize END________________________________\n");

	return 0;
}



int SGMainCtrl::Run()
{
	ERROR_LOG("________________________________GAMESVR Run BEGIN________________________________\n");

	//SGIEventLoop* pEventLoop = g_pEventObjFactory->GetEventLoop();

	m_iRunState = enMainCtrl_Running;

	while (enMainCtrl_Running == m_iRunState)
	{
		//消息检查
		pTmpEventLoop->LoopOnce(m_listenfd,100);
	}


	ERROR_LOG("________________________________GAMESVR Run END________________________________\n");


	return 0;
}


