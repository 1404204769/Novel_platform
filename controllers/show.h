#pragma once
#include <drogon/HttpSimpleController.h>
#include <plugins/MyJwt.h>
using namespace drogon;
class show:public drogon::HttpSimpleController<show>
{
  public:
    virtual void asyncHandleHttpRequest(const HttpRequestPtr& req, std::function<void (const HttpResponsePtr &)> &&callback) override;
    PATH_LIST_BEGIN
    //list path definitions here;
    //PATH_ADD("/path","filter1","filter2",HttpMethod1,HttpMethod2...);
    PATH_ADD("/show",Get,Post,Options,"MyJsonFilter","JwtFilter","LoginFilter");
    PATH_LIST_END
};
