/**
 *
 *  AdminFilter.cc
 *
 */

#include "AdminFilter.h"

using namespace drogon;

void AdminFilter::doFilter(const HttpRequestPtr &req,
                         FilterCallback &&fcb,
                         FilterChainCallback &&fccb)
{
    Json::Value ReqVal, RespVal;
    drogon::HttpResponsePtr Result;
	auto *MyBasePtr = app().getPlugin<MyBase>();
    MyBasePtr->TRACELog("AdminFilter::body" + string(req->getBody()), true);


    const std::string LoginStatus = req->getParameter("Login_Status");
    if((LoginStatus == "admin") || (LoginStatus == "root"))
    {
        MyBasePtr->DEBUGLog("AdminFilter 验证成功", true);
        return fccb();
    }
    
    RespVal["ErrorMsg"] = "用户权限不足，请联系管理员";
    MyBasePtr->TRACELog("AdminFilter 验证失败", true);
    MyBasePtr->DEBUGLog("RespVal::" + RespVal.toStyledString(), true);

    Result = HttpResponse::newHttpJsonResponse(RespVal);
    Result->setStatusCode(k500InternalServerError);
    // Return the response and let's tell this endpoint request was cancelled
    return fcb(Result);
}
