#ifndef HTTPCONTEXT_H
#define HTTPCONTEXT_H

#include "muduo/net/Buffer.h"
#include "muduo/base/Timestamp.h"
#include "muduo/net/TcpConnection.h"
#include "HttpRequest.h"
#include "HttpResponse.h"

using namespace net;

class HttpContext 
{
public:
    enum HttpRequestParseState
    {
        kExpectRequestLine,
        kExpectHeaders,
        kExpectBody,
        kGotAll,   //这里有逗号 is ok
    };

    HttpContext()
        : m_state(kExpectRequestLine)
    {
    }

  // default copy-ctor, dtor and assignment are fine

  // return false if any error
    bool parseRequest(Buffer* buf, Timestamp receiveTime);

    bool gotAll() const
    { return m_state == kGotAll; }

    void reset()
    {
        m_state = kExpectRequestLine;
        HttpRequest dummy;
        m_request.swap(dummy);
    }

    const HttpRequest& request() const
    { return m_request; }

    HttpRequest& request()
    { return m_request; }

private:
    bool processRequestLine(const char* begin, const char* end);

    HttpRequestParseState m_state;
    HttpRequest m_request;
};

#endif  //
