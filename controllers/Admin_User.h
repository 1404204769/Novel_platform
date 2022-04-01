#pragma once
#include <drogon/HttpController.h>
#include <models/User.h>
#include <plugins/MyJson.h>
using namespace drogon;
using namespace drogon_model::novel;
namespace Admin
{
class User:public drogon::HttpController<User>
{
  public:
    METHOD_LIST_BEGIN
      METHOD_ADD(User::List,"/List",Get,Post,Options,"MyJsonFilter","JwtFilter","LoginFilter","AdminFilter");
      METHOD_ADD(User::Search,"/Search",Get,Post,Options,"MyJsonFilter","JwtFilter","LoginFilter","AdminFilter");
      METHOD_ADD(User::Update,"/Update",Get,Post,Options,"MyJsonFilter","JwtFilter","LoginFilter","AdminFilter");

    METHOD_LIST_END
    void List(const HttpRequestPtr &req,std::function<void (const HttpResponsePtr &)> &&callback) const;
    void Search(const HttpRequestPtr &req,std::function<void (const HttpResponsePtr &)> &&callback) const;
    void Update(const HttpRequestPtr &req,std::function<void (const HttpResponsePtr &)> &&callback) const;
};
}
