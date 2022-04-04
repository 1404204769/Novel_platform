#include "Introduce.h"
void Introduce::asyncHandleHttpRequest(const HttpRequestPtr& req, std::function<void (const HttpResponsePtr &)> &&callback)
{
    drogon::HttpResponsePtr Result;
    Json::Value ReqVal, RespVal;
    auto MyBasePtr = app().getPlugin<MyBase>();
    MyBasePtr->TRACELog("Introduce::body" + string(req->getBody()), true);

    Result=HttpResponse::newHttpResponse();

    Result->setBody("Hello World!");
    Result->setBody("<a href=\"/General_Interface/show\">show</a>");
    
    Result->setStatusCode(k200OK);
    Result->setContentTypeCode(CT_TEXT_HTML);
    callback(Result);
}