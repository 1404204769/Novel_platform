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
    Json::Value RespVal;
    const std::string LoginStatus = req->getParameter("LoginStatus");
    if((LoginStatus == "admin") || (LoginStatus == "root"))
    {
        std::cout << "AdminFilter 验证成功" << std::endl;
        return fccb();
    }
    RespVal["ErrorMsg"] = "用户权限不足，请联系管理员";
    auto res = HttpResponse::newHttpJsonResponse(RespVal);
    res->setStatusCode(k500InternalServerError);
    std::cout << "AdminFilter 验证失败" << std::endl;
    // Return the response and let's tell this endpoint request was cancelled
    return fcb(res);
}
