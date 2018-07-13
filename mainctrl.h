#ifndef __SG_MAINCTRL_H__
#define __SG_MAINCTRL_H__
#include "common/daemon.h"
#include "common/errlog.h"

enum enMainCtrlState
{
	enMainCtrl_Init,
	enMainCtrl_Running,
	enMainCtrl_Stopping,
	enMainCtrl_Done,
};


typedef int(*RoutineCheckFn)(void*);

class SGMainCtrl
{
	struct TRoutineCheckInfo
	{
		void* m_pUserdata;
		RoutineCheckFn m_pRoutineCheckFn;
		unsigned int m_uiLastCheckTime;
		unsigned int m_uiCheckIntervalSec;
	};

public:
	SGMainCtrl();
	~SGMainCtrl();

	//预备
	int PrepareRun();
	//跑
	int Run();
	//进程退出时用
	//int BeforeExit();


protected:
	void InitDaemonBlockParent();

	void ReleaseParent(int iRetVal);

	int IgnoreSignal();

	//锁文件
	int LockSvr();

	//安装信号
	int InstallSignal();

	int StartNetwork();

	//初始化
	int Initialize();

	int listenToPort(int port, int *fds, int *count);
private:

protected:

	SGDaemonWait m_stDaemonWait;	

	int m_iRunState;
	int m_listenfd;
};

extern SGMainCtrl *g_pMainCtrl;

#endif // !__SG_MAINCTRL_HPP__


