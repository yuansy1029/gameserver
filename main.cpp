#include "redis/RedisDB.h"
#include <unistd.h>
#include "common/anet.h"
#include <stdio.h>
#include "mainctrl.h"
SGMainCtrl *g_pMainCtrl = NULL;

int main()
{

    
    //RedisDB *redis = new RedisDB();
    //redis->Init();

    g_pMainCtrl = new SGMainCtrl();

    g_pMainCtrl->PrepareRun();
    g_pMainCtrl->Run();
    //SGAnet *pConn = new SGAnet();
    //char err[256];
	//int fd = pConn->anetTcpServer(err,8888,"127.0.0.1",1);
    
    
    //printf("fd is %d\n",fd);
    //redisReply* reply = redis->command("get qq",1);
    //printf("%s\n",reply->str);
    //ERROR_LOG("sddd%s",reply->str);
    //while(1)
    //{
    //    ;
    //}
    //redis->redisfree();
    return 0;
}