#include "ZmJsonParse.h"
#include "rapidjson/writer.h"
#include "rapidjson/stringbuffer.h"
#include <iostream>
using namespace rapidjson;



ZmJsonParse::ZmJsonParse()
{

}
ZmJsonParse::~ZmJsonParse()
{

}

// 解析json
bool ZmJsonParse::parseJson(const string& strJson, HttpRequestBody* stRequest)
{
    if (stRequest == NULL)
    {
        return false;
    }
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

        if (d.HasMember("C"))
        {
            Value& s = d["C"];
            if (s.IsString())
            {
                stRequest->Command = s.GetString();
            }
        }

        if (d.HasMember("T"))
        {
            Value& s = d["T"];
            if (s.IsString())
            {
                stRequest->Token = s.GetString();
            }
        }

        if (d.HasMember("F") && d["F"].IsArray())
        {
            Value& s = d["F"];

            for (SizeType i = 0; i < s.Size(); i++)
            {
                if (s[i].HasMember("K") && s[i].HasMember("V"))
                {
                    string key = s[i]["K"].GetString();
                    string value;
                    if (s[i]["V"].IsString())
                    {
                        value = s[i]["V"].GetString();
                    }
                    else if (s[i]["V"].IsBool())
                    {
                        bool b = s[i]["V"].GetBool();
                        if (b)
                        {
                            value = "TRUE";
                        }
                        else
                        {
                            value = "FALSE";
                        }
                    }
                    else if (s[i]["V"].IsInt())
                    {
                        int b = s[i]["V"].GetInt();
                        value = std::to_string(b);
                    }
                    else if (s[i]["V"].IsDouble())
                    {
                        double b = s[i]["V"].GetDouble();
                        value = std::to_string(b);
                    }
                    else if (s[i]["V"].IsFloat())
                    {
                        float b = s[i]["V"].GetFloat();
                        value = std::to_string(b);
                    }

                    stRequest->BodyMap[key] = value;
                }
            }
        }

        doExtern(d, stRequest);

        if (d.HasMember("VER") && d["VER"].IsString())
        {
            Value& s = d["VER"];
            stRequest->VER = s.GetString();
        }

        if (d.HasMember("DT") && d["DT"].IsString())
        {
            Value& s = d["DT"];
            stRequest->DT = s.GetString();
        }
    }
    catch (...)
    {
        return false;
    }

    return true;
}

// 额外解析json
void ZmJsonParse::doExtern(Document& d, HttpRequestBody* stRequest)
{ 
    string  strTxt;
    if (d.HasMember("D"))
    {
        Value& s = d["D"];
        if (s.IsString())
        {
            strTxt = s.GetString();
        }
        if (stRequest->D.get() == NULL)
        {
            stRequest->D = VoidPtr(new ZfsD());
        }
        ((ZfsD*)(stRequest->D.get()))->txt = strTxt;
    }
}
