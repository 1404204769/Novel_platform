#include "User_Feedback.h"
using namespace User;
void Feedback::asyncHandleHttpRequest(const HttpRequestPtr& req, std::function<void (const HttpResponsePtr &)> &&callback)
{    
    Json::Value ReqVal, RespVal;
    drogon::HttpResponsePtr Result;
    auto MyBasePtr = app().getPlugin<MyBase>();
    auto MyJsonPtr = app().getPlugin<MyJson>();
    auto MyDBSPtr = app().getPlugin<MyDBService>();
    const unordered_map<string, string> umapPara = req->getParameters();
    MyBasePtr->TRACELog("Admin::User::Update::body" + string(req->getBody()), true);
    
 
    try
    {
        ReqVal = *req->getJsonObject();
        RespVal["简介"] = "用户问题反馈接口";
        MyJsonPtr->UnMapToJson(ReqVal, umapPara, "Para");
        MyDBSPtr->Create_Suggestion(ReqVal,RespVal);

        MyBasePtr->DEBUGLog("RespVal::" + RespVal.toStyledString(), true);

        Result=HttpResponse::newHttpJsonResponse(RespVal);
    }
    catch (Json::Value &RespVal)
    {
        MyBasePtr->TRACE_ERROR(RespVal["ErrorMsg"]);
        Result = HttpResponse::newHttpJsonResponse(RespVal);
    }
    catch (...)
    {
        RespVal["ErrorMsg"].append("Feedback::ERROR");
        MyBasePtr->TRACE_ERROR(RespVal["ErrorMsg"]);
        Result = HttpResponse::newHttpJsonResponse(RespVal);
    }
    
    Result->setStatusCode(k200OK);
    Result->setContentTypeCode(CT_TEXT_HTML);
    callback(Result);
}