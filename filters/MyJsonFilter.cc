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
    Json::Value RespVal,ResultData;
    drogon::HttpResponsePtr Result;
	auto *MyBasePtr = app().getPlugin<MyBase>();
    MyBasePtr->TRACELog("MyJsonFilter::body" + string(req->getBody()), true);
    // 判断是否存在Json数据 若存在Json数据则允许进入下一个地址
    if(req->getJsonObject())
    {
        MyBasePtr->TRACELog("MyJsonFilter 验证成功", true);
        return fccb();
    }

    RespVal["ErrorMsg"].append("不存在Json数据");
    MyBasePtr->TRACELog("MyJsonFilter 验证失败", true);
    MyBasePtr->DEBUGLog("RespVal::" + RespVal.toStyledString(), true);


    // 设置返回格式
    int ErrorSize = RespVal["ErrorMsg"].size();
    ResultData["Result"] = false;
    ResultData["Message"] = RespVal["ErrorMsg"][ErrorSize - 1];
    auto res = HttpResponse::newHttpJsonResponse(ResultData);

    res->setStatusCode(k500InternalServerError);
    return fcb(res);
}
