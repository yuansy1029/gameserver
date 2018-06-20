#include <hiredis/hiredis.h>
#include <stdio.h>
#include <vector>

#define REDIS_REPLY_STRING 1   //返回字符串，查看str,len字段
#define REDIS_REPLY_ARRAY 2    //返回一个数组，查看elements的值（数组个数），通过element[index]的方式访问数组元素，每个数组元素是一个redisReply对象的指针
#define REDIS_REPLY_INTEGER 3  //返回整数，从integer字段获取值
#define REDIS_REPLY_NIL 4      //没有数据返回
#define REDIS_REPLY_STATUS 5   //表示状态，内容通过str字段查看，字符串长度是len字段
#define REDIS_REPLY_ERROR 6    //表示出错，查看出错信息，如上的str,len字段

using namespace std;
#define MAX_CONN 10
class RedisDB
{
    public:
        int Init(char* address = "127.0.0.1",int port = 6379);
        void redisfree();
        redisReply* command(char* cmdline,int db);
        redisContext* get_conn();
        void put_conn(redisContext* conn);
    private:    
        vector<redisContext*> conn_box;
        char *m_address;
        int m_port;
};
