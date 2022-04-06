#include "Admin_Economics.h"
using namespace Admin;
void Economics::asyncHandleHttpRequest(const HttpRequestPtr& req, std::function<void (const HttpResponsePtr &)> &&callback)
{
    Json::Value ReqVal,RespVal;
    drogon::HttpResponsePtr Result;
    auto MyBasePtr = app().getPlugin<MyBase>();
    auto MyJsonPtr = app().getPlugin<MyJson>();
    const unordered_map<string,string>umapPara = req->getParameters();
    MyBasePtr->TRACELog("Economics::body" + string(req->getBody()), true);


    RespVal["简介"] = "管理员查看系统经济报表接口";
    MyJsonPtr->UnMapToJson(ReqVal, umapPara, "Para");
    MyJsonPtr->UnMapToJson(RespVal, umapPara, "Para");
    MyBasePtr->DEBUGLog("RespVal::" + RespVal.toStyledString(), true);

    Result=HttpResponse::newHttpJsonResponse(RespVal);
    
    Result->setStatusCode(k200OK);
    Result->setContentTypeCode(CT_TEXT_HTML);
    callback(Result);
}