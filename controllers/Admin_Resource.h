#pragma once
#include <drogon/HttpController.h>
#include <plugins/MyJson.h>
using namespace drogon;
namespace Admin
{
class Resource:public drogon::HttpController<Resource>
{
  public:
    METHOD_LIST_BEGIN
      METHOD_ADD(Resource::Upload,"/Upload",Get,Post,Options,"MyJsonFilter","JwtFilter","AdminFilter");
      METHOD_ADD(Resource::Download,"/Download",Get,Post,Options,"MyJsonFilter","JwtFilter","AdminFilter");
      METHOD_ADD(Resource::Search,"/Search",Get,Post,Options,"MyJsonFilter","JwtFilter","AdminFilter");

    METHOD_LIST_END
    void Upload(const HttpRequestPtr &req,std::function<void (const HttpResponsePtr &)> &&callback) const;
    void Download(const HttpRequestPtr &req,std::function<void (const HttpResponsePtr &)> &&callback) const;
    void Search(const HttpRequestPtr &req,std::function<void (const HttpResponsePtr &)> &&callback) const;
};
}
