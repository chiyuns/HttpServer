#pragma once

#include "../stdafx.h"
#include "./zds/SqlDef.h"
#include "./rapidjson/document.h"
#include <memory>
using namespace rapidjson;

namespace zchs
{

#define     ZFS_OPT_TXT         "txt"

#define     KEY_REMAINTIME        "remaintime"
#define     KEY_STATUS            "status"
#define     KEY_PORCESS           "process"
#define     KEY_ERRRDATA          "errordata"
#define     KEY_STARTTIME         "starttime"
#define     KEY_ENDTIME           "endtime"

    typedef enum ZDS_PARAMS
    {
        ZDS_PARAM_0 = 0,
        ZDS_PARAM_1 = 1,
        ZDS_PARAM_2 = 2,
    }ZDS_PARAMS;

    typedef std::shared_ptr<void> VoidPtr;

    // 请求体
    typedef struct HttpRequestBody
    {
        HttpRequestBody() {  }
        ~HttpRequestBody() {
            BodyMap.clear();
        }
        std::string ID;                   // 32位GUID
        std::string Command;              // 命令
        std::string Token;                // TOKEN
        std::map<string, string> BodyMap;
        VoidPtr D;
        string DT;                   // 时间
        string VER;                  //  版本
    } HttpRequestBody;



    struct SqlDEntyity {
        SqlDEntyity()
        {

        }
        ~SqlDEntyity()
        {
            params.clear();
        }
        string optid;
        string comptype;
        string opttype;
        string sql;
        list<SqlParameter> params;
    };

    struct SqlD {
        SqlD()
        {

        }
        ~SqlD()
        {
            sqlds.clear();
        }
        vector<SqlDEntyity> sqlds;
    };

    struct ZfsD {
        ZfsD()
        {
                
        }
        ~ZfsD()
        {

        }
        string txt;
    };

    class CHttpEnity
    {
    public:
        CHttpEnity() {}
        virtual ~CHttpEnity() {}

        virtual void ToJson(rapidjson::Writer<rapidjson::StringBuffer>& writer) const  //转换为json结构
        {

        }
    };

    class CHttpFileEnity : public CHttpEnity
    {
    public:
        CHttpFileEnity() {}
        virtual ~CHttpFileEnity() {}

        virtual void ToJson(rapidjson::Writer<rapidjson::StringBuffer>& writer) const  //转换为json结构
        {
            writer.Key("D");
            writer.String(value_.c_str());
        }
    public:
        string  value_;
    };


    class CHttpTaskEnity : public CHttpEnity
    {
    public:
        CHttpTaskEnity() {}
        virtual ~CHttpTaskEnity() {}

        virtual void ToJson(rapidjson::Writer<rapidjson::StringBuffer>& writer) const  //转换为json结构
        {
            writer.Key("D");
            writer.StartObject();

            writer.Key(KEY_STARTTIME);
            writer.String(starttime.c_str());

            writer.Key(KEY_ENDTIME);
            writer.String(endtime.c_str());


            writer.Key(KEY_STATUS);
            writer.String(status.c_str());

            writer.Key(KEY_PORCESS);
            writer.String(process.c_str());

            writer.Key(KEY_REMAINTIME);
            writer.String(remaintime.c_str());

            writer.Key(KEY_ERRRDATA);
            writer.String(errordata.c_str());

            writer.EndObject();
        }

        bool parseJson(const string& strJson)
        {
            std::size_t fEnd = strJson.find("{");
            if (fEnd == std::string::npos)
            {
                return false;
            }
            const char* json = strJson.c_str();
            Document d;
            d.Parse(json);

            if (d.HasParseError())
            {
                return false;
            }

            try {


                if (d.HasMember(KEY_STARTTIME))
                {
                    Value& s = d[KEY_STARTTIME];
                    if (s.IsString())
                    {
                        starttime = s.GetString();
                    }
                }


                if (d.HasMember(KEY_REMAINTIME))
                {
                    Value& s = d[KEY_REMAINTIME];
                    if (s.IsString())
                    {
                        remaintime = s.GetString();
                    }
                }

                if (d.HasMember(KEY_PORCESS))
                {
                    Value& s = d[KEY_PORCESS];
                    if (s.IsString())
                    {
                        process = s.GetString();
                    }
                }

                if (d.HasMember(KEY_STATUS))
                {
                    Value& s = d[KEY_STATUS];
                    if (s.IsString())
                    {
                        status = s.GetString();
                    }
                }

                if (d.HasMember(KEY_ENDTIME))
                {
                    Value& s = d[KEY_ENDTIME];
                    if (s.IsString())
                    {
                        endtime = s.GetString();
                    }
                }

                if (d.HasMember(KEY_ERRRDATA))
                {
                    Value& s = d[KEY_ERRRDATA];
                    if (s.IsString())
                    {
                        errordata = s.GetString();
                    }
                }

            }
            catch (...)
            {
                return false;
            }

            return true;


        }


    public:
        std::string  status;
        std::string  process;
        
        std::string  remaintime;
        std::string  errordata;

        std::string  starttime;
        std::string  endtime;
    };


    // 响应体
    class HttpResponseBody
    {
    public:
        HttpResponseBody() {
            S = false;
            D = NULL;
        }
        virtual ~HttpResponseBody() { 
            if (D != NULL)
            {
                delete D;
                D = NULL;
            }
            FD.clear();
        }
        bool S;
        string M;
        string C;
        CHttpEnity* D;
        std::map<string, string> FD;
        std::string ToJson(void) const  //转换为json结构
        {
            rapidjson::StringBuffer strBuf;
            rapidjson::Writer<rapidjson::StringBuffer> writer(strBuf);
            writer.StartObject();
            writer.Key("S");
            writer.Bool(S);
            if (!C.empty())
            {
                writer.Key("C");
                writer.String(C.c_str());
            }
            if (!M.empty())
            {
                writer.Key("M");
                writer.String(M.c_str());
            }

            if (D != NULL)
            {
                D->ToJson(writer);
            }

            if (FD.size() > 0)
            {
                writer.Key("FD");
                writer.StartArray();
                for (auto& kv : FD)
                {
                    writer.StartObject();
                    writer.Key("K");
                    writer.String(kv.first.c_str());
                    writer.Key("V");
                    writer.String(kv.second.c_str());
                    writer.EndObject();
                }
                writer.EndArray();
            }

            writer.EndObject();
            return strBuf.GetString();
        }
    };


    // 解析json
    class ZmJsonParse
    {
    public:
        ZmJsonParse();
        virtual ~ZmJsonParse();
        // 通用解析json
        virtual bool parseJson(const string& strJson, HttpRequestBody* stRequest);

        // 额外解析json
        virtual void doExtern(Document& d, HttpRequestBody* stRequest);
    };

    void jsonTest();
}
