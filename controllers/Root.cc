#include "Root.h"
//add definition of your processing function here

void Root::Instructions(const HttpRequestPtr &req,std::function<void (const HttpResponsePtr &)> &&callback) const
{
    Json::Value ReqVal, RespVal;
    drogon::HttpResponsePtr Result;
    auto MyBasePtr = app().getPlugin<MyBase>();
    auto MyJsonPtr = app().getPlugin<MyJson>();
    auto *MyRootPtr = app().getPlugin<MyRoot>();
    const unordered_map<string, string> umapPara = req->getParameters();
    MyBasePtr->TRACELog("Root::Instructions::body" + string(req->getBody()), true);


    try{
        // 读取Json数据
        Json::Value ReqVal=*req->getJsonObject();

        MyJsonPtr->checkMember(ReqVal,RespVal,"Instruction");
        if(ReqVal["Instruction"].asString() == "restart")
        {
            MyRootPtr->restart();
            RespVal["Result"]="重启成功";
        }
        if(ReqVal["Instruction"].asString() == "close")
        {
            MyRootPtr->close();
            RespVal["Result"]="关机成功";
        }
        
        MyBasePtr->DEBUGLog("RespVal::" + RespVal.toStyledString(), true);

        Result=HttpResponse::newHttpJsonResponse(RespVal);
    }
    catch(Json::Value &RespVal)
    {
	    RespVal["Result"] = "指令错误";
        MyBasePtr->TRACELog("ErrorMsg::" + RespVal["ErrorMsg"].asString(), true);

        Result=HttpResponse::newHttpJsonResponse(RespVal);
    }
    catch(const drogon::orm::DrogonDbException &e)
    {
	    RespVal["Result"] = "指令错误";
        RespVal["ErrorMsg"] = e.base().what();
        MyBasePtr->TRACELog("ErrorMsg::" + RespVal["ErrorMsg"].asString(), true);

        Result=HttpResponse::newHttpJsonResponse(RespVal);
    }

    Result->setStatusCode(k200OK);
    Result->setContentTypeCode(CT_TEXT_HTML);
    callback(Result);
}