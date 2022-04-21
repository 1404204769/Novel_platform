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
    Json::Value ReqVal, RespVal,ResultData;
    drogon::HttpResponsePtr Result;
	auto *MyBasePtr = app().getPlugin<MyBase>();
    MyBasePtr->TRACELog("RootFilter::body" + string(req->getBody()), true);


    const std::string LoginStatus = req->getParameter("Login_Status");
    if(LoginStatus == "root")
    {
        MyBasePtr->TRACELog("RootFilter 验证成功", true);
        return fccb();
    }
    
    RespVal["ErrorMsg"].append("权限不足，请联系站长");
    MyBasePtr->TRACELog("RootFilter 验证失败", true);
    MyBasePtr->DEBUGLog("RespVal::" + RespVal.toStyledString(), true);



    // 设置返回格式
    int ErrorSize = RespVal["ErrorMsg"].size();
    ResultData["Result"] = false;
    ResultData["Message"] = RespVal["ErrorMsg"][ErrorSize - 1];
    Result = HttpResponse::newHttpJsonResponse(ResultData);

    Result->setStatusCode(k500InternalServerError);
    // Return the response and let's tell this endpoint request was cancelled
    return fcb(Result);
}
