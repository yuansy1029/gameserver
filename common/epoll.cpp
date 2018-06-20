#include "epoll.h"
#include <string.h>
#include <stdio.h>


SGEpollEventLoop::SGEpollEventLoop(int iEpollEventSize) :
	m_iEpollFD(-1),
	m_iEpollEventSize(iEpollEventSize),
	m_pstEpollEvent(NULL)
{
	printf("5555555555\n");
}


SGEpollEventLoop::~SGEpollEventLoop()
{

}


int SGEpollEventLoop::Initialize()
{
	m_iEpollFD = epoll_create(m_iEpollEventSize);
	if (m_iEpollFD < 0)
	{
		return -1;
	}

	m_pstEpollEvent = new epoll_event[m_iEpollEventSize];
	if (!m_pstEpollEvent)
	{
		return -2;
	}

	return 0;
}

int SGEpollEventLoop::AddEvent(int iFD, int iEventType)
{
	printf("1234");
	int iRet = 0;
	
	epoll_event  stOneEpollEvent;
	stOneEpollEvent.events = EPOLLERR | EPOLLHUP;
	stOneEpollEvent.data.u64 = iFD;

	//可读事件
	if (iEventType & EventType_Read)
	{
		stOneEpollEvent.events |= EPOLLIN;
	}

	//可写事件
	if (iEventType & EventType_Write)
	{
		stOneEpollEvent.events |= EPOLLOUT;
	}
	printf("1234");
	printf("%d\n",m_iEpollFD);
	printf("12345");
	//ִ添加监听
	iRet = epoll_ctl(m_iEpollFD, EPOLL_CTL_ADD, iFD, &stOneEpollEvent);

	if (iRet < 0)
	{
		printf("epoll_ctl(%d) fd(%d) failed got:%d\n", m_iEpollFD, iFD, iRet);
		return -1;
	}

	return 0;
}

int SGEpollEventLoop::ChgEvent(int iFD, TObjMixID iObjID, int iEventType)
{
	int iRet = 0;
	uint64_t ui64FD = iFD;

	epoll_event  stOneEpollEvent;
	stOneEpollEvent.events = EPOLLERR | EPOLLHUP;
	stOneEpollEvent.data.u64 = ui64FD << 32 | iObjID;

	//可读事件
	if (iEventType & EventType_Read)
	{
		stOneEpollEvent.events |= EPOLLIN;
	}

	//可写事件
	if (iEventType & EventType_Write)
	{
		stOneEpollEvent.events |= EPOLLOUT;
	}

	//ִ修改事件类型
	iRet = epoll_ctl(m_iEpollFD, EPOLL_CTL_MOD, iFD, &stOneEpollEvent);
	if (iRet < 0)
	{
		ERROR_LOG("epoll_ctl(%d) fd(%d) failed got:%d\n", m_iEpollFD, iFD, iRet);
		return -1;
	}

	return 0;
}

int SGEpollEventLoop::DelDevent(int iFD)
{
	int iRet = 0;
	epoll_event  stOneEpollEvent;
	iRet = epoll_ctl(m_iEpollFD, EPOLL_CTL_DEL, iFD, &stOneEpollEvent);
	if (iRet < 0)
	{
		ERROR_LOG("epoll_ctl(%d) fd(%d) failed got:%d\n", m_iEpollFD, iFD, iRet);
		return -1;
	}

	return 0;
}

int SGEpollEventLoop::LoopOnce(int iTimeoutMS)
{
	//ERROR_LOG("THERE is a message arrive111111111111111 %d\n",2);
	int iEpollEventNumber;
	iEpollEventNumber = epoll_wait(m_iEpollFD, m_pstEpollEvent, m_iEpollEventSize, iTimeoutMS);
	//ERROR_LOG("sssssssss%d\n",iEpollEventNumber);
	if (iEpollEventNumber < 0)
	{
	//	if(SGUtility::GetLastErrorNO() != EINTR)
	//	{
			ERROR_LOG("epoll_wait(%d) failed got:%d\n", m_iEpollFD, iEpollEventNumber);
	//	}
	//	else
	//	{
	//		ERROR_LOG("epoll_wait(%d) failed got:%d\n", m_iEpollFD, iEpollEventNumber);
	//	}
		return -1;
	}

	for (int i = 0; i < iEpollEventNumber; ++i)
	{
		printf("THERE is a message arrive %d\n",2);
		uint64_t ui64FD = m_pstEpollEvent[i].data.u64;
		unsigned int uiEpollEvent = m_pstEpollEvent[i].events;
		int iRealFD = ui64FD;
		TObjMixID iObjID = static_cast<TObjMixID>(ui64FD);

		if ((EPOLLERR | EPOLLHUP) & uiEpollEvent)
		{
		//	OnError(iRealFD, this,iObjID,uiEpollEvent);
		}
		else
		{
			if ((EPOLLIN)& uiEpollEvent)
			{
				ERROR_LOG("THERE is a message arrive %d\n",2);
				OnRead(iRealFD, this);
			}
			if ((EPOLLOUT)& uiEpollEvent)
			{
				ERROR_LOG("THERE is a message arrive %d\n",3);
				OnWrite(iRealFD, this);
			}
		}
	}

	return 0;
}

int SGEpollEventLoop::OnRead(int fd,char *buf)
{
	int nread;
    nread = read(fd,buf,MAXSIZE);
        if (nread == -1)
    {
        ERROR_LOG("read error:");
        close(fd);
    }
    else if (nread == 0)
    {
        fprintf(stderr,"server close.\n");
        close(fd);
    }
    else
    {
        if (fd == STDIN_FILENO)
            add_event(m_iEpollFD,sockfd,EPOLLOUT);
        else
        {
            delete_event(m_iEpollFD,sockfd,EPOLLIN);
            add_event(m_iEpollFD,STDOUT_FILENO,EPOLLOUT);
        }
	}
	return 0;
}

int SGEpollEventLoop::OnWrite(int fd,char *buf)
{
	int nwrite;
    nwrite = write(fd,buf,strlen(buf));
    if (nwrite == -1)
    {
        ERROR_LOG("write error:");
        close(fd);
    }
    else
    {
        if (fd == STDOUT_FILENO)
            delete_event(epollfd,fd,EPOLLOUT);
        else
            modify_event(epollfd,fd,EPOLLIN);
    }
    memset(buf,0,MAXSIZE);
}

int SGEpollEventLoop::GetLoopErrorMsg(char *pErrMsg, int iMsgStrLen)
{
	pErrMsg[0] = 0;
	return 0;
}

