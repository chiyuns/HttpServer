#include "muduo/net/Buffer.h"
#include "HttpContext.h"
#include <assert.h>

bool HttpContext::processRequestLine(const char* begin, const char* end)
{
    bool succeed = false;
    const char* start = begin;
    const char* space = std::find(start, end, ' ');
    if (space != end && m_request.setMethod(start, space))
    {
        start = space+1;
        space = std::find(start, end, ' ');
        if (space != end)
        {
            const char* question = std::find(start, space, '?');
            if (question != space)
            {
                m_request.setPath(start, question);
                m_request.setQuery(question, space);
            }
            else
            {
                m_request.setPath(start, space);
            }
            start = space+1;
            succeed = end-start == 8 && std::equal(start, end-1, "HTTP/1.");
            if (succeed)
            {
                if (*(end-1) == '1')
                {
                    m_request.setVersion(HttpRequest::kHttp11);
                }
                else if (*(end-1) == '0')
                {
                    m_request.setVersion(HttpRequest::kHttp10);
                }
                else
                {
                    succeed = false;
                }
            }
        }
    }
    return succeed;
}

//这个是对外的接口
// return false if any error
bool HttpContext::parseRequest(Buffer* buf, Timestamp receiveTime)
{
    bool ok = true;
    bool hasMore = true;
    while (hasMore)
    {
        if (m_state == kExpectRequestLine)   //一开始 m_state = kExpectRequestLine
        {
            const char* crlf = buf->findCRLF();
            if (crlf)
            {
                ok = processRequestLine(buf->peek(), crlf);
                if (ok)
                {
                    m_request.setReceiveTime(receiveTime);
                    buf->retrieveUntil(crlf + 2);
                    m_state = kExpectHeaders;
                }
                else
                {
                    hasMore = false;
                }
            }
            else
            {
                hasMore = false;
            }
        }
        else if (m_state == kExpectHeaders)
        {
            const char* crlf = buf->findCRLF
            ();
            if (crlf)
            {
                const char* colon = std::find(buf->peek(), crlf, ':');
                if (colon != crlf)
                {
                    m_request.addHeader(buf->peek(), colon, crlf);
                    buf->retrieveUntil(crlf + 2);  //ps
                }
                else
                {
                // empty line, end of header
                    buf->retrieveUntil(crlf + 2);  //ps 
                    if (m_request.getContentLength() != 0)
                    {
                        m_state = kExpectBody;
                    }
                    else
                    {
                        m_state = kGotAll;
                        printf("kGotAll!\n");
                        hasMore = false;
                    } 
                } 
            }
            else
            {
                hasMore = false;
            }
        }
        else if (m_state == kExpectBody)
        {
            // FIXME:
            unsigned long len = m_request.getContentLength();
            assert(len > 0);
            if (buf->readableBytes() >= len)
            {
                m_request.setBody(buf->peek(), buf->peek() + static_cast<int>(len));
                buf->retrieve(len);
                m_state = kGotAll;
            }
            hasMore = false;
        }
    }
    return ok;
}
