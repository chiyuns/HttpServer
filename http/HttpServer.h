#ifndef __HTTP_SERVER_H__
#define __HTTP_SERVER_H__

#include <memory>
#include <mutex>
#include <list>
#include "muduo/base/Platform.h"
#include "muduo/base/Singleton.h"
#include "muduo/base/ConfigFileReader.h"
#include "muduo/base/AsyncLog.h"
#include "muduo/base/ThreadPool.h"
#include "muduo/net/EventLoop.h"
#include "muduo/net/EventLoopThreadPool.h"
#include "muduo/net/TcpServer.h"

#ifndef WIN32
#include <string.h>
#include "muduo/utils/DaemonRun.h"
#endif 

using namespace net;
class HttpRequest;
class HttpResponse;
class HttpServer
{
public:
    HttpServer() = default;
    ~HttpServer() = default;

    HttpServer(const HttpServer & rhs) = delete;
    HttpServer& operator =(const HttpServer & rhs) = delete;

public:
    bool init(const char* ip, short& port, EventLoop * loop);
    void uninit();
    static bool sendResponse(const TcpConnectionPtr& conn, HttpResponse& resp);

private:
    //新连接到来调用或连接断开，所以需要通过conn->connected()来判断，一般只在主loop里面调用
    void onConnected(std::shared_ptr<TcpConnection> conn);
    void onMessage(const TcpConnectionPtr& conn, Buffer* buf, Timestamp receiveTime);
    void processRequestBody(const TcpConnectionPtr& conn, HttpRequest);
    bool onRequest(const TcpConnectionPtr& conn, const HttpRequest& req, HttpResponse* resp);
    
private:
    std::unique_ptr<TcpServer>                     m_server;
    ThreadPool                                     m_threadPool;       //线程池
    std::map<int, std::string>                     threadMap_;
};

#endif
