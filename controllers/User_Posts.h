#pragma once
#include <drogon/HttpController.h>
#include <plugins/MyJson.h>
using namespace drogon;
namespace User
{
class Posts:public drogon::HttpController<Posts>
{
  public:
    METHOD_LIST_BEGIN
      METHOD_ADD(Posts::Like,"/Like",Get,Post,Options,"MyJsonFilter","JwtFilter","LoginFilter");
      METHOD_ADD(Posts::Reply,"/Reply",Get,Post,Options,"MyJsonFilter","JwtFilter","LoginFilter");

    METHOD_LIST_END
    void Like(const HttpRequestPtr &req,std::function<void (const HttpResponsePtr &)> &&callback) const;
    void Reply(const HttpRequestPtr &req,std::function<void (const HttpResponsePtr &)> &&callback) const;
};
}
