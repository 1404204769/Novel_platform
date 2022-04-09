#include "show.h"
void show::asyncHandleHttpRequest(const HttpRequestPtr& req, std::function<void (const HttpResponsePtr &)> &&callback)
{
    Json::Value ReqVal, RespVal;
    drogon::HttpResponsePtr Result;
	auto *JWTPtr = app().getPlugin<MyJwt>();
    auto MyBasePtr = app().getPlugin<MyBase>();
	const string token = req->getHeader("Authorization");
    const unordered_map<string, string> umapPara = req->getParameters();
    MyBasePtr->TRACELog("show::body" + string(req->getBody()), true);


    try{
        ReqVal=*req->getJsonObject();

        MyBasePtr->DEBUGLog("开始解析Token", true);
        RespVal = JWTPtr->decode(token);
        MyBasePtr->DEBUGLog("解析Token成功", true);
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
        RespVal["ErrorMsg"].append("Test::SysOutLevel");
        MyBasePtr->TRACE_ERROR(RespVal["ErrorMsg"]);
        Result = HttpResponse::newHttpJsonResponse(RespVal);
    }


	Result->setStatusCode(k200OK);
    Result->setContentTypeCode(CT_TEXT_HTML);
	callback(Result);
}