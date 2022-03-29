/**
 *
 *  MyJsonFilter.cc
 *
 */

#include "MyJsonFilter.h"

using namespace drogon;

void MyJsonFilter::doFilter(const HttpRequestPtr &req,
                         FilterCallback &&fcb,
                         FilterChainCallback &&fccb)
{
    // 获取Json数据指针
	auto jsonptr=req->getJsonObject();
    // 判断是否存在Json数据
    if(jsonptr)
    {
        // 若存在Json数据则允许进入下一个地址
        fccb();
        return;
    }
    Json::Value RespVal;
    RespVal["ErrorMsg"] = "不存在Json数据";
    auto res = HttpResponse::newHttpJsonResponse(RespVal);
    res->setStatusCode(k500InternalServerError);
    fcb(res);
}
