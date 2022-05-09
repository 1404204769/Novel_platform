#pragma once
#include <drogon/HttpController.h>
#include <plugins/MyJson.h>
#include <plugins/MyDBService.h>
using namespace drogon;
namespace Admin
{
class Resource:public drogon::HttpController<Resource>
{
  public:
    METHOD_LIST_BEGIN
      METHOD_ADD(Resource::Search,"/Search",Get,Post,Options,"MyJsonFilter","JwtFilter","AdminFilter");
      METHOD_ADD(Resource::Upload,"/Upload",Get,Post,Options,"MyJsonFilter","JwtFilter","AdminFilter");
      METHOD_ADD(Resource::Update,"/Update",Get,Post,Options,"MyJsonFilter","JwtFilter","AdminFilter");
      METHOD_ADD(Resource::UCV,"/Update/Chapter/Valid",Get,Post,Options,"MyJsonFilter","JwtFilter","AdminFilter");
      METHOD_ADD(Resource::Download,"/Download",Get,Post,Options,"MyJsonFilter","JwtFilter","AdminFilter");

    METHOD_LIST_END
    void Search(const HttpRequestPtr &req,std::function<void (const HttpResponsePtr &)> &&callback) const;
    void Upload(const HttpRequestPtr &req,std::function<void (const HttpResponsePtr &)> &&callback) const;
    void Update(const HttpRequestPtr &req,std::function<void (const HttpResponsePtr &)> &&callback) const;
    void UCV(const HttpRequestPtr &req,std::function<void (const HttpResponsePtr &)> &&callback) const;
    void Download(const HttpRequestPtr &req,std::function<void (const HttpResponsePtr &)> &&callback) const;
};
}
