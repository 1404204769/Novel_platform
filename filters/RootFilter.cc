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
    Json::Value RespVal;
    const std::string LoginStatus = req->getParameter("LoginStatus");
    if(LoginStatus == "root")
    {
        std::cout << "RootFilter 验证成功" << std::endl;
        return fccb();
    }
    RespVal["ErrorMsg"] = "权限不足，请联系站长";
    auto res = HttpResponse::newHttpJsonResponse(RespVal);
    res->setStatusCode(k500InternalServerError);
    std::cout << "RootFilter 验证失败" << std::endl;
    // Return the response and let's tell this endpoint request was cancelled
    return fcb(res);
}
