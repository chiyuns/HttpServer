    class ZmZdsJsonParse : public ZmJsonParse
    {
    public:
        ZmZdsJsonParse();
        virtual ~ZmZdsJsonParse();

        // 额外解析json
        virtual void doExtern(Document& d, HttpRequestBody* stRequest);
    };
