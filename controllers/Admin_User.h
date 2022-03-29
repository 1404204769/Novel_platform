#pragma once
#include <drogon/HttpController.h>
#include <plugins/MyTools.h>
using namespace drogon;
using namespace drogon_model::novel;
namespace Admin
{
class User:public drogon::HttpController<User>
{
  public:
    METHOD_LIST_BEGIN
      METHOD_ADD(User::List,"/List",Get,Post,Options);
      METHOD_ADD(User::Search,"/Search",Get,Post,Options,"MyJsonFilter");

    METHOD_LIST_END
    void List(const HttpRequestPtr &req,std::function<void (const HttpResponsePtr &)> &&callback) const;
    void Search(const HttpRequestPtr &req,std::function<void (const HttpResponsePtr &)> &&callback) const;
};
}
