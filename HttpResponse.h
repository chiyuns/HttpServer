#ifndef _HTTPRESPONSE_H_
#define _HTTPRESPONSE_H_

#include "muduo/net/Buffer.h"
#include <map>

using namespace net;
using namespace std;
class HttpResponse
{
public:
    enum HttpStatusCode
    {
        kUnknown,
        k200Ok = 200,
        k301MovedPermanently = 301,
        k400BadRequest = 400,
        k404NotFound = 404,
    };

    explicit HttpResponse(bool close)
        : statusCode_(kUnknown),
        closeConnection_(close)
    {
    }

    void setStatusCode(HttpStatusCode code)
    {
        statusCode_ = code;
    }

    void setStatusMessage(const string& message)
    {
        statusMessage_ = message;
    }

    void setCloseConnection(bool on)
    {
        closeConnection_ = on;
    }

    bool closeConnection() const
    {
        return closeConnection_;
    }

    void setContentType(const string& contentType)
    {
        addHeader("Content-Type", contentType);
        addHeader("Access-Control-Allow-Origin", "*");
        addHeader("Access-Control-Allow-Methods", "POST,GET,OPTIONS,DELETE");
        addHeader("Access-Control-Allow-Headers", "x-zmail-resid,x-emp-no,x-zmail-istest");
    }

    // FIXME: replace string with StringPiece
    void addHeader(const string& key, const string& value)
    {
        headers_[key] = value;
    }


    void setBody(const string& body)
    {
        body_ = body;
    }

    void appendToBuffer(Buffer* output) const;

private:
    std::map<string, string> headers_;
    HttpStatusCode statusCode_;

    string statusMessage_;
    bool closeConnection_;
    string body_;
};

#endif 
