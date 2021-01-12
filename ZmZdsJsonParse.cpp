ZmZdsJsonParse::ZmZdsJsonParse()
{

}
ZmZdsJsonParse::~ZmZdsJsonParse()
{

}

// 额外解析json
void ZmZdsJsonParse::doExtern(Document& d, HttpRequestBody* stRequest)
{
    // string strJson = "{\"C\":\"BatchExeSql\",\"T\":\"\",\"L\":\"\",\"P\":null,\"D\":[{\"optid\":\"6b0e688f-a53a-155b-25c1-282ece9677f1\",\"comptype\":\"table\",\"opttype\":\"select\",\"sql\":\"SELECT * FROM  T_ZM_BackupTimes WHERE UNO = '10071847'  AND STATUS IN (2) ORDER BY ID DESC;\"}],\"F\":[{\"K\":\"clientid\",\"V\":\"zMail_client1\"},{\"K\":\"reqid\",\"V\":\"abbf6a08-0dc1-a948-127f-38e755dccc43\"},{\"K\":\"istrans\",\"V\":false},{\"K\":\"dbfile\",\"V\":\"D://BusinessSoft//zMail//zMailLocal//10071847//ClientMail.db\"}],\"VER\":\"1.0\",\"DT\":\"2019-02-28 17:47:45\"}";

    if (d.HasMember("D") && d["D"].IsArray())
    {
        Value& s = d["D"];
        for (SizeType i = 0; i < s.Size(); i++)
        {
            SqlDEntyity entity;
            if (s[i].HasMember(SQL_OPT_ID) && s[i][SQL_OPT_ID].IsString())
            {
                string value = s[i][SQL_OPT_ID].GetString();
                entity.optid = value;
            }
            if (s[i].HasMember(SQL_COMP_TYPE) && s[i][SQL_COMP_TYPE].IsString())
            {
                string value = s[i][SQL_COMP_TYPE].GetString();
                entity.comptype = value;
            }

            if (s[i].HasMember(SQL_OPT_TYPE) && s[i][SQL_OPT_TYPE].IsString())
            {
                string value = s[i][SQL_OPT_TYPE].GetString();
                entity.opttype = value;
            }

            if (s[i].HasMember(SQL_TEXT) && s[i][SQL_TEXT].IsString())
            {
                string value = s[i][SQL_TEXT].GetString();
                entity.sql = value;
            }

            if (s[i].HasMember(SQL_PARAM))
            {
                //这是一个数组
                if (s[i][SQL_PARAM].IsArray())
                {
                    Value& pa = s[i][SQL_PARAM];
                    for (SizeType j = 0; j < pa.Size(); j++)
                    {
                        SqlParameter param;
                        if (pa[j].HasMember(SQL_PARAM_TYPE) && pa[j][SQL_PARAM_TYPE].IsInt())
                        {
                            int ntype = pa[j][SQL_PARAM_TYPE].GetInt();
                            param.type = ntype;
                            // 参数是string
                            if (pa[j].HasMember(SQL_PARAM_VALUE))
                            {
                                if (pa[j][SQL_PARAM_VALUE].IsString())
                                {
                                    if (ntype == ZDS_PARAM_0)
                                    {
                                        string value = pa[j][SQL_PARAM_VALUE].GetString();
                                        param.strP = value;
                                        entity.params.emplace_back(param);
                                    }
                                    else if (ntype == ZDS_PARAM_1)
                                    {
                                        try {
                                            int value = std::atoi(pa[j][SQL_PARAM_VALUE].GetString());
                                            param.nP = value;
                                            entity.params.emplace_back(param);
                                        }
                                        catch (...)
                                        {

                                        }
                                    }
                                    else if (ntype == ZDS_PARAM_2)
                                    {
                                        try {
                                            float value = std::atof(pa[j][SQL_PARAM_VALUE].GetString());
                                            param.fP = value;
                                            entity.params.emplace_back(param);
                                        }
                                        catch (...)
                                        {

                                        }
                                    }
                                }
                                else if (pa[j][SQL_PARAM_VALUE].IsInt())
                                {
                                    if (ntype == ZDS_PARAM_0)
                                    {
                                        int value = pa[j][SQL_PARAM_VALUE].GetInt();
                                        param.strP = std::to_string(value);
                                        entity.params.emplace_back(param);
                                    }
                                    else if (ntype == ZDS_PARAM_1)
                                    {
                                        int value = pa[j][SQL_PARAM_VALUE].GetInt();
                                        param.nP = value;
                                        entity.params.emplace_back(param);

                                    }
                                }
                                else if (pa[j][SQL_PARAM_VALUE].IsFloat())
                                {
                                    if (ntype == ZDS_PARAM_0)
                                    {
                                        float value = pa[j][SQL_PARAM_VALUE].GetFloat();
                                        param.strP = std::to_string(value);
                                        entity.params.emplace_back(param);
                                    }
                                    else if (ntype == ZDS_PARAM_2)
                                    {
                                        float value = pa[j][SQL_PARAM_VALUE].GetFloat();
                                        param.fP = value;
                                        entity.params.emplace_back(param);
                                    }
                                }
                            }
                        }
                        else
                        {
                            continue;
                        }
                    }

                }
            }

            if (stRequest->D.get() == NULL)
            {
                stRequest->D = VoidPtr(new SqlD());
            }
            ((SqlD*)(stRequest->D.get()))->sqlds.emplace_back(entity);
        }
    }
}
