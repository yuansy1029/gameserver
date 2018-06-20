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



/*int SGMainCtrl::LockSvr()
{
	char szPidFile[128];
	char szPidBuffer[16];

	sprintf(szPidFile, "%s.pid", m_stSvrArgs.m_acSvrName);

	int iPidFileFD = open(szPidFile, O_WRONLY | O_CREAT, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);
	if (iPidFileFD < 0)
	{
		ERROR_LOG("Open pid file %s fail !\n", szPidFile);
		return -1;
	}
	if (flock(iPidFileFD, LOCK_EX | LOCK_NB) < 0)
	{
		ERROR_LOG("Server is already Running!\n");
		return -2;
	}
	if (ftruncate(iPidFileFD, 0) < 0)
	{
		ERROR_LOG("truncate pid file %s fail !\n", szPidFile);
		return -3;
	}

	int iPidLen = snprintf(szPidBuffer, sizeof(szPidBuffer), "%u\n", (int)getpid());
	if (write(iPidFileFD, szPidBuffer, strlen(szPidBuffer)) != iPidLen)
	{
		ERROR_LOG("write pid file %s fail !\n", szPidFile);
		return -4;
	}

	int val = fcntl(iPidFileFD, F_GETFD, 0);
	if (val < 0)
	{
		ERROR_LOG("fcntl F_GETFD pid file %s fail !\n", szPidFile);
		return -5;
	}

	val |= FD_CLOEXEC;

	if (fcntl(iPidFileFD, F_SETFD, val) < 0)
	{
		ERROR_LOG("fcntl F_SETFD pid file %s fail !\n", szPidFile);
		return -6;
	}

	return 0;
}*/

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

	/*iRet = LockSvr();
	if (iRet)
	{
		ReleaseParent(iRet - 1000);
		exit(iRet);
	}*/

	iRet = Initialize();
	if (iRet)
	{
		ReleaseParent(iRet - 2000);
		exit(iRet);
	}

	ReleaseParent(0);

	return 0;
}

int SGMainCtrl::listenToPort(int port, int *fds, int *count) {
    int j;
	#if 0
    /* Force binding of 0.0.0.0 if no bind address is specified, always
     * entering the loop if j == 0. */
    if (server.bindaddr_count == 0) server.bindaddr[0] = NULL;
    for (j = 0; j < server.bindaddr_count || j == 0; j++) {
        if (server.bindaddr[j] == NULL) {
            int unsupported = 0;
            /* Bind * for both IPv6 and IPv4, we enter here only if
             * server.bindaddr_count == 0. */
            fds[*count] = anetTcp6Server(server.neterr,port,NULL,
                server.tcp_backlog);
            if (fds[*count] != ANET_ERR) {
                anetNonBlock(NULL,fds[*count]);
                (*count)++;
            } else if (errno == EAFNOSUPPORT) {
                unsupported++;
                serverLog(LL_WARNING,"Not listening to IPv6: unsupproted");
            }

            if (*count == 1 || unsupported) {
                /* Bind the IPv4 address as well. */
                fds[*count] = anetTcpServer(server.neterr,port,NULL,
                    server.tcp_backlog);
                if (fds[*count] != ANET_ERR) {
                    anetNonBlock(NULL,fds[*count]);
                    (*count)++;
                } else if (errno == EAFNOSUPPORT) {
                    unsupported++;
                    serverLog(LL_WARNING,"Not listening to IPv4: unsupproted");
                }
            }
            /* Exit the loop if we were able to bind * on IPv4 and IPv6,
             * otherwise fds[*count] will be ANET_ERR and we'll print an
             * error and return to the caller with an error. */
            if (*count + unsupported == 2) break;
        } else if (strchr(server.bindaddr[j],':')) {
            /* Bind IPv6 address. */
            fds[*count] = anetTcp6Server(server.neterr,port,server.bindaddr[j],
                server.tcp_backlog);
        } else {
            /* Bind IPv4 address. */
            fds[*count] = anetTcpServer(server.neterr,port,server.bindaddr[j],
                server.tcp_backlog);
        }
        if (fds[*count] == ANET_ERR) {
            serverLog(LL_WARNING,
                "Creating Server TCP listening socket %s:%d: %s",
                server.bindaddr[j] ? server.bindaddr[j] : "*",
                port, server.neterr);
            return C_ERR;
        }
        anetNonBlock(NULL,fds[*count]);
        (*count)++;
    }
	return C_OK;
	#endif

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
	int fd = pConn->anetTcpServer(err,8888,"127.0.0.1",1);
	printf("xxxxxxxx\n");
	if (fd == -1)
	{
		printf("listen failed,%s",err);
	}
	if (pConn->anetNonBlock(err,fd) == -1)
	{
		printf("set failed,%s",err);
	}
	printf("777777777777\n");
	//添加监听事件
	pTmpEventLoop->AddEvent(fd, EventType_Read);

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
        //检查信号
		//HandleSignalCheck();
		//例行检查
		//HandleRoutineCheck();
		//消息检查
		pTmpEventLoop->LoopOnce(100);
	}


	ERROR_LOG("________________________________GAMESVR Run END________________________________\n");


	return 0;
}
/*
int SGMainCtrl::HandleSignalCheck()
{
	if(SIGNAL_RELOAD == g_iSignalFlag)
	{
		g_pLocalConfig->LoadConfig(true);
		g_pLocalConfig->TraceConfig();
		g_pLuaEngine->RunMainFile(g_pLocalConfig->m_acLuaGameMainFile, true);
	}
	else if(SIGNAL_QUIT == g_iSignalFlag)
	{
		//ͣ��
		m_iRunState = enMainCtrl_Stopping;
	}

	g_iSignalFlag = 0;

	return 0;
}
*/
/*unsigned int SGMainCtrl::GetGameTime()
{
#ifdef _DEBUG
	return g_pNowTime->GetNowTime() + m_uiAheadTimeSetting;
#else
	return g_pNowTime->GetNowTime();
#endif
}

int SGMainCtrl::SetGameTime(unsigned int uiGameTime)
{
	if (uiGameTime <= g_pNowTime->GetNowTime())
	{
		return -1;
	}
#ifdef _DEBUG
	m_uiAheadTimeSetting = uiGameTime - g_pNowTime->GetNowTime();
#endif
	return 0;
}


long SGMainCtrl::GetRealTimeMS()
{
	timeval tv = g_pNowTime->GetNowTimeVal();
	long lRealTime = tv.tv_sec * 1000 + tv.tv_usec / 1000;
	return lRealTime;
}


int SGMainCtrl::InitGlobalData()
{
	return 0;
}
*/
/*int SGMainCtrl::GetOnlineNum() const
{
	return g_pGameLogicHandler->GetOnlineConnNum();
}*/


