/**
 *
 *  RootFilter.cc
 *
 */

#include "RootFilter.h"

using namespace drogon;

void RootFilter::doFilter(const HttpRequestPtr &req,
                         FilterCallback &&fcb,
                         FilterChainCallback &&fccb)
{
    Json::Value ReqVal, RespVal;
    drogon::HttpResponsePtr Result;
	auto *MyBasePtr = app().getPlugin<MyBase>();
    MyBasePtr->TRACELog("RootFilter::body" + string(req->getBody()), true);


    const std::string LoginStatus = req->getParameter("Login_Status");
    if(LoginStatus == "root")
    {
        MyBasePtr->TRACELog("RootFilter 验证成功", true);
        return fccb();
    }
    
    RespVal["ErrorMsg"] = "权限不足，请联系站长";
    MyBasePtr->TRACELog("RootFilter 验证失败", true);
    MyBasePtr->DEBUGLog("RespVal::" + RespVal.toStyledString(), true);

    Result = HttpResponse::newHttpJsonResponse(RespVal);

    Result->setStatusCode(k500InternalServerError);
    // Return the response and let's tell this endpoint request was cancelled
    return fcb(Result);
}
