#pragma once
#include <drogon/HttpController.h>
#include <plugins/MyJson.h>
using namespace drogon;
namespace Admin
{
class Examine:public drogon::HttpController<Examine>
{
  public:
    METHOD_LIST_BEGIN
      METHOD_ADD(Examine::UpList,"/UpList",Get,Post,Options,"MyJsonFilter","JwtFilter","AdminFilter");
      METHOD_ADD(Examine::IdeaList,"/IdeaList",Get,Post,Options,"MyJsonFilter","JwtFilter","AdminFilter");

    METHOD_LIST_END
    void UpList(const HttpRequestPtr &req,std::function<void (const HttpResponsePtr &)> &&callback) const;
    void IdeaList(const HttpRequestPtr &req,std::function<void (const HttpResponsePtr &)> &&callback) const;
};
}
