#pragma once
#include <drogon/HttpController.h>
#include <plugins/MyJwt.h>
#include <models/User.h>
using namespace drogon;
class Gpi:public drogon::HttpController<Gpi>
{
  public:
    METHOD_LIST_BEGIN
      METHOD_ADD(Gpi::Login,"/Login",Get,Post,Options,"MyJsonFilter");
      METHOD_ADD(Gpi::Register,"/Register",Get,Post,Options,"MyJsonFilter");
    METHOD_LIST_END
    void Login(const HttpRequestPtr &req,std::function<void (const HttpResponsePtr &)> &&callback) const;
    void Register(const HttpRequestPtr &req,std::function<void (const HttpResponsePtr &)> &&callback) const;
};
