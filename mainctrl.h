#ifndef __SG_MAINCTRL_H__
#define __SG_MAINCTRL_H__
#include "common/daemon.h"
#include "common/errlog.h"
//当前服务器的版本
const int CURR_GAMESVR_VERSION = 1100;

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

	//获取服务器版本号
	static int GetVersion()
	{
		return CURR_GAMESVR_VERSION;
	}

	//int AddRoutineCheck(RoutineCheckFn pRoutineCheckFn,void* pUserdata,unsigned int uiCheckInterval);

	//获取游戏时间!!!!这个函数必须保证是多线程安全!!!!
	//unsigned int GetGameTime();
	//int SetGameTime(unsigned int uiGameTime);

	//long GetRealTimeMS();


protected:
	void InitDaemonBlockParent();

	void ReleaseParent(int iRetVal);

	int IgnoreSignal();

	//锁文件
	int LockSvr();

	//安装信号
	int InstallSignal();

	int StartNetwork();

	//跨服相关服务的网络
	//int StartServiceNetworks();

	//初始化
	int Initialize();

	//lula
	//int StartLuaModule();

	//int HandleRoutineCheck();
	//int HandleSignalCheck();

	//int HandleExitCheck(unsigned int uiNowTime, unsigned int& uiLastCheckTime);

	int listenToPort(int port, int *fds, int *count);
private:
	//int DestroyDBResource();
	//int DestroyPTResource();
protected:

	SGDaemonWait m_stDaemonWait;
	//char m_acRunPath[MAX_PATH_LENGTH];	//运行路径


	//例行检查
	//SGArray<TRoutineCheckInfo, SERVERCONFIG_ROUTINE_CHECK_NUM> m_astRoutineCheck;

	//SGArray<SGIRankingList*, SERVERCONFIG_MAX_RANKINGLIST_TYPE> m_apRankingList;

	//可以通过GM设置这个前进时间
	//unsigned int m_uiAheadTimeSetting;	

	int m_iRunState;

	//这里管理所有的离线需要保存数据的接口(INTERFACE)
	//SGISaveDataOnStop* m_apSaveDataOnStop[enSaveData_MaxModule];

	//其它服务的处理接口
	//SGInternalConnHandler* m_apInternalServiceHandlers[enServiceType_Max];
//private:
//	friend class SGTestClient;
};

extern SGMainCtrl *g_pMainCtrl;

#endif // !__SG_MAINCTRL_HPP__


