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
    Json::Value RespVal;
    const std::string LoginStatus = req->getParameter("LoginStatus");
    if((LoginStatus == "user") || (LoginStatus == "admin") || (LoginStatus == "root"))
    {
        std::cout << "LoginFilter 验证成功" << std::endl;
        return fccb();
    }
    RespVal["LoginStatus"] = LoginStatus;
    RespVal["ErrorMsg"] = "游客权限不足，请注册账号";
    auto res = HttpResponse::newHttpJsonResponse(RespVal);
    res->setStatusCode(k500InternalServerError);
    std::cout << "LoginFilter 验证失败" << std::endl;
    // Return the response and let's tell this endpoint request was cancelled
    return fcb(res);
}
