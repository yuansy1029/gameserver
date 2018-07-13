#ifndef __SG_EPOLLEVENTLOOP_HPP__
#define __SG_EPOLLEVENTLOOP_HPP__

#include <sys/epoll.h>
#include "errlog.h"
#include "object.h"

enum SGIEVENT_TYPE
{
	EventType_Read	= 1,	
	EventType_Write = 2,
};

class SGEpollEventLoop
{
public:

	SGEpollEventLoop(int iEpollEventSize);

	~SGEpollEventLoop();

	int Initialize();

	int AddEvent(int iFD, int iEventType);

	int ChgEvent(int iFD, TObjMixID iObjID, int iEventType);

	int DelDevent(int iFD);

	int LoopOnce(int listenfd,int iTimeoutMS);

	int GetLoopErrorMsg(char *pErrMsg, int iMsgStrLen);

	int OnRead(int fd,char *buf);
//	int OnWrite(int fd,char *buf);
	//int OnError(int fd,char *buf,int reason);
protected:

	int m_iEpollFD;
	int m_iEpollEventSize;
	epoll_event* m_pstEpollEvent;

};


#endif
