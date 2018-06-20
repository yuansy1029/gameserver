#ifndef __SG_DAEMONWAIT_HPP__
#define __SG_DAEMONWAIT_HPP__

class SGDaemonWait
{
public:
	SGDaemonWait();
	~SGDaemonWait();

	int Start();
	bool Valid();
	int WaitChild(char* pszError,int iErrStrLen);
	int NotifyParent(int iRetVal);
	void CloseAll();

protected:
	int m_aiFD[2];
};



#endif // !__SG_DAEMONWAIT_HPP__