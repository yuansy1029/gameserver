#ifndef __ERRLOG_H__
#define __ERRLOG_H__

#include <syslog.h>
#include <sys/time.h>
#include <time.h>
#include <stdio.h>
#include <stdarg.h>
#define MAX_DATE_LENGTH 32

static int ConvertUnixTimeToAccurateString(timeval tvTime, char* szAccurateString, int iStrMaxLen)
{
	if (szAccurateString == NULL)
	{
		return -1;
	}

	szAccurateString[0] = '\0';

	time_t tTime = tvTime.tv_sec;
	struct tm stTempTm;
	struct tm *pTempTm = localtime_r(&tTime, &stTempTm);

	if (pTempTm == NULL)
	{
		return -2;
	}

	snprintf(szAccurateString, iStrMaxLen, "%04d-%02d-%02d %02d:%02d:%02d.%06d",
		pTempTm->tm_year + 1900, pTempTm->tm_mon + 1, pTempTm->tm_mday,
		pTempTm->tm_hour, pTempTm->tm_min, pTempTm->tm_sec,static_cast<int>(tvTime.tv_usec));

	return 0;
}

static timeval GetNowTimeVal()
{
    timeval tvTimeVal;
	gettimeofday(&tvTimeVal, NULL);
	return tvTimeVal;
}


static void errlog(const char* pszFile, 
						 int iLine, const char* pszFunc, const char *pcContent, ...)
{
    char szCurDate[MAX_DATE_LENGTH];
    timeval tNow = GetNowTimeVal();

	ConvertUnixTimeToAccurateString(tNow, szCurDate, MAX_DATE_LENGTH);
    char buffer[1024];
    char str_tmp[512];
    //打印...
    va_list args;
    va_start(args, pcContent);
    
    int i = vsnprintf(str_tmp, 512, pcContent, args);
    va_end(args);

    sprintf(buffer,"[%s] <%s:%d %s>  %s", szCurDate,pszFile,iLine,pszFunc,str_tmp);
    syslog(LOG_ERR | LOG_USER,"test - %m\n");   
	openlog("mysvr",LOG_NDELAY | LOG_PID,0);  
	syslog(LOG_NOTICE,"%s",buffer);
}

#define ERROR_LOG(_FORMAT_STR, ...) errlog(__FILE__, __LINE__, __FUNCTION__, _FORMAT_STR, ##__VA_ARGS__)

#endif
