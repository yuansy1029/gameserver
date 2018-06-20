#include "RedisDB.h"

int RedisDB::Init(char* address, int port)
{
    m_address = address;
    m_port = port;

    struct timeval timeout = {2, 0}; //2s的超时时间
    for (int i=1; i<MAX_CONN; i++)
    {
        redisContext* conn = (redisContext*)redisConnectWithTimeout(address, port, timeout);
        if ( (NULL == conn) || (conn->err) )
        {
            if (conn_box[i])
            {
                printf("connect error:%s,%d\n",conn->errstr,i);
            }
            else
            {
                printf("connect error: can't allocate redis context.");
            }
            return -1;
        }
        conn_box.push_back(conn);
    }
    return 0;
}

void RedisDB::redisfree()
{   
    for (int i=0; i<conn_box.size(); i++)
    {
        redisFree(conn_box[i]);
    }
}

redisContext* RedisDB::get_conn()
{
    if (conn_box.size() <= 0)
    {
        struct timeval timeout = {2, 0}; //2s的超时时间
        redisContext* conn = (redisContext*)redisConnectWithTimeout(m_address, m_port, timeout);
        if ( (NULL == conn) || (conn->err) )
        {
            if (conn)
            {
                printf("connect error:%s,%d\n",conn->errstr);
            }
            else
            {
                printf("connect error: can't allocate redis context.");
            }
        }
        conn_box.push_back(conn);
    }
    redisContext* conn = conn_box[0]; 
    conn_box.erase(conn_box.begin());
    return conn;
}

void RedisDB::put_conn(redisContext* conn)
{
    conn_box.push_back(conn);
}


redisReply* RedisDB::command(char* cmdline,int db)
{
    redisContext* conn = get_conn();
    redisCommand(conn, "select %d",db); //执行命令
    redisReply *pRedisReply = (redisReply*)redisCommand(conn, cmdline); //执行命令
    printf("%d,%s\n",pRedisReply->type,pRedisReply->str);
    put_conn(conn);
    return pRedisReply;
}