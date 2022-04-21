/**
 *
 *  LoginFilter.cc
 *
 */

#include "LoginFilter.h"

using namespace drogon;

void LoginFilter::doFilter(const HttpRequestPtr &req,
                         FilterCallback &&fcb,
                         FilterChainCallback &&fccb)
{
    Json::Value ReqVal, RespVal, ResultData;
    drogon::HttpResponsePtr Result;
	auto *MyBasePtr = app().getPlugin<MyBase>();
	auto *MyJsonPtr = app().getPlugin<MyJson>();
    const unordered_map<string, string> umapPara = req->getParameters();
    MyBasePtr->TRACELog("LoginFilter::body" + string(req->getBody()), true);

    const std::string LoginStatus = umapPara.at("Login_Status");
    if((LoginStatus == "user") || (LoginStatus == "admin") || (LoginStatus == "root"))
    {
        MyBasePtr->TRACELog("LoginFilter 验证成功", true);
        return fccb();
    }
    RespVal["Login_Status"] = LoginStatus;
    RespVal["ErrorMsg"].append("游客权限不足，请注册账号");
    MyBasePtr->TRACELog("LoginFilter 验证失败", true);
    MyBasePtr->DEBUGLog("RespVal::" + RespVal.toStyledString(), true);



    // 设置返回格式
    int ErrorSize = RespVal["ErrorMsg"].size();
    ResultData["Result"] = false;
    ResultData["Message"] = RespVal["ErrorMsg"][ErrorSize - 1];
    Result = HttpResponse::newHttpJsonResponse(ResultData);

    Result->setStatusCode(k500InternalServerError);
    return fcb(Result);
}
