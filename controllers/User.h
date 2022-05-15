#pragma once
#include <drogon/HttpController.h>
#include <plugins/MyJson.h>
#include <plugins/MyDBService.h>
using namespace drogon;
class User:public drogon::HttpController<User>
{
  public:
    METHOD_LIST_BEGIN
    METHOD_ADD(User::Report,"/Report",Get,Post,Options,"MyJsonFilter","JwtFilter","LoginFilter");
    METHOD_ADD(User::Feedback,"/Feedback",Get,Post,Options,"MyJsonFilter","JwtFilter","LoginFilter");
    METHOD_ADD(User::Action,"/Action",Get,Post,Options,"MyJsonFilter","JwtFilter","LoginFilter");
    METHOD_LIST_END
    void Report(const HttpRequestPtr &req,std::function<void (const HttpResponsePtr &)> &&callback) const;
    void Feedback(const HttpRequestPtr &req,std::function<void (const HttpResponsePtr &)> &&callback) const;
    void Action(const HttpRequestPtr &req,std::function<void (const HttpResponsePtr &)> &&callback) const;
};
