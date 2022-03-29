#pragma once
#include <drogon/HttpSimpleController.h>
#include <plugins/MyJwt.h>
using namespace drogon;
class LoginCtl:public drogon::HttpSimpleController<LoginCtl>
{
  public:
    virtual void asyncHandleHttpRequest(const HttpRequestPtr& req, std::function<void (const HttpResponsePtr &)> &&callback) override;
    PATH_LIST_BEGIN
    //list path definitions here;
    PATH_ADD("/login",Get,Post,Options);
    PATH_LIST_END
};
