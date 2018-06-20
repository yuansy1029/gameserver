#include "daemon.h"
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>


SGDaemonWait::SGDaemonWait()
{
	m_aiFD[0] = m_aiFD[1] = -1;
}

SGDaemonWait::~SGDaemonWait()
{

}

int SGDaemonWait::Start()
{
	if (pipe(m_aiFD) < 0)
	{
		return -1;
	}

	return 0;
}

bool SGDaemonWait::Valid()
{
	return m_aiFD[0] >= 0 && m_aiFD[1] >= 0;
}

int SGDaemonWait::WaitChild(char* pszError, int iErrStrLen)
{
	if (!Valid())
	{
		return 0;
	}

	int iRetVal = 0;
	//block in here
	read(m_aiFD[0], &iRetVal, sizeof(iRetVal));
	if (iRetVal)
	{
		snprintf(pszError, iErrStrLen, "Wait Child Daemon Run Failed1:%d", iRetVal);
	}

	CloseAll();
	printf("%d\n",iRetVal);
	return iRetVal;
}
int SGDaemonWait::NotifyParent(int iRetVal)
{
	if (!Valid())
	{
		return 0;
	}

	write(m_aiFD[1], &iRetVal, sizeof(iRetVal));

	CloseAll();
	return 0;
}

void SGDaemonWait::CloseAll()
{
	if (m_aiFD[0] >= 0)
	{
		close(m_aiFD[0]);
		m_aiFD[0] = -1;
	}
	if (m_aiFD[1] >= 0)
	{
		close(m_aiFD[1]);
		m_aiFD[1] = -1;
	}
}
