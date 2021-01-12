#include "muduo/net/InetAddress.h"
#include "HttpServer.h"
#include "HttpContext.h"
#include "ZmJsonParse.h"
#include "../zfs/File.h"
#include "../utility/Util.h"
#include "../ControlService.h"

#ifdef WIN32
#include "windows.h"
#else
#include "unistd.h"
#include "sys/sysinfo.h"
#endif

//http服务器
bool HttpServer::init(const char* ip, short& port, EventLoop* loop)
{
    bool bBind = false;

    int nRetry = 0;

    while (!bBind && nRetry < 10)
    {
        InetAddress addr(ip, port);

        m_server.reset( new TcpServer(loop, addr, "ZTE-HTTPSERVER", TcpServer::kReusePort));
        bBind = m_server->getBind();
        if (!bBind)
        {
            port = port + 1;
        }
        nRetry++;
    }
    m_server->setConnectionCallback(std::bind(&HttpServer::onConnected, this, std::placeholders::_1));
    m_server->setMessageCallback(std::bind(&HttpServer::onMessage, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3));
    m_threadPool.start(9);
    m_server->start(9);
    return bBind;
}


void HttpServer::uninit()
{
    if (m_server)
        m_server->stop();
    m_threadPool.stop();
}

//新连接到来调用或连接断开，所以需要通过conn->connected()来判断，一般只在主loop里面调用
void HttpServer::onConnected(std::shared_ptr<TcpConnection> conn)
{
    if (conn->connected())
    {
        conn->refreshAliveTime();
        LOGI("HttpServer onConnected:%s", conn->peerAddress().toIpPort().c_str());
        conn->setContext(HttpContext());  //这个HttpContext后面还会用到
    }
}


void HttpServer::onMessage(const TcpConnectionPtr& conn, Buffer* buf, Timestamp receiveTime)
{
    HttpContext* context = any_cast<HttpContext>(conn->getMutableContext()); //conn->getMutableContext()返回的是引用

    if (!context->parseRequest(buf, receiveTime))
    {
        conn->send("HTTP/1.1 400 Bad Request\r\n\r\n");
        conn->shutdown();
    }

    if (context->gotAll())
    {
        //放到线程池里处理
       m_threadPool.run(std::bind(static_cast<void (HttpServer::*)(const TcpConnectionPtr&, HttpRequest)>(&HttpServer::processRequestBody),
                                           this,     // FIXME
                                           conn,
                                           context->request()));
        context->reset();   //会把request清空
    }
}

//1.问题:为什么一定要用static function,下面是不用static function后的报错
//ISO C++ forbids taking the address 
//of an unqualified or parenthesized non-static member function to form a pointer to member function
//2.注意点:类的静态函数,这里定义不要再在前面加static了
void HttpServer::processRequestBody(const TcpConnectionPtr& conn,HttpRequest req) 
{
    try
    {
        const string& connection = req.getHeader("Connection");
        bool close = connection == "close" || (req.getVersion() == HttpRequest::kHttp10 && connection != "Keep-Alive");
        HttpResponse resp(close);
        bool bRet = onRequest(conn, req, &resp);
        if (bRet)
        {
            sendResponse(conn, resp);
        }
        else
        {
            //失败 
            conn->send("HTTP/1.1 400 Bad Request\r\n\r\n");
            conn->shutdown();
        }
    }
    catch (const std::exception&e)
    {
        LOGI("processRequestBody: exec exception:%s", e.what());
        conn->send("HTTP/1.1 400 Bad Request\r\n\r\n");
        conn->shutdown();
    }   
}


bool HttpServer::onRequest(const TcpConnectionPtr& conn, const HttpRequest& req, HttpResponse* resp)  //回复的业务逻辑在此
{
    bool  bRet = false;
    string url = req.path();
    string sControlName = CUtil::StrReplace(url, "/", "");
    IControl* pControl = CControlService::GetInstance()->getControl(sControlName);
    if (pControl == NULL)
    {
        pControl = CControlService::GetInstance()->getDefaultControl();
    }

    if (pControl != NULL)
    {
        bRet = pControl->doHttpReceive(conn, req, resp);
    }
    return bRet;
}


bool HttpServer::sendResponse(const TcpConnectionPtr& conn, HttpResponse& resp)
{
    Buffer buf;
    resp.setContentType("application/json;charset=utf-8");
    resp.setStatusCode(HttpResponse::k200Ok);
    resp.setStatusMessage("OK");
    resp.setCloseConnection(true);
    resp.appendToBuffer(&buf);
    conn->send(&buf);
    if (resp.closeConnection())
    {
        conn->shutdown();
    }
    return true;
}
