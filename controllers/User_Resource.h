#pragma once
#include <drogon/HttpController.h>
#include <models/User.h>
#include <models/Upload.h>
#include <models/Book.h>
#include <models/Chapter.h>
#include <plugins/MyJson.h>
#include <plugins/MyBase.h>
#include <plugins/MyTools.h>
#include <plugins/MyDBService.h>
#include <regex>
using namespace drogon;
using namespace drogon_model::novel;
namespace User
{
class Resource:public drogon::HttpController<Resource>
{
  public:
    METHOD_LIST_BEGIN
      METHOD_ADD(Resource::Download,"/Download",Get,Post,Options,"MyJsonFilter","JwtFilter","LoginFilter");
      METHOD_ADD(Resource::Read,"/Read",Get,Post,Options,"MyJsonFilter","JwtFilter","LoginFilter");
      METHOD_ADD(Resource::Search,"/Search",Get,Post,Options,"MyJsonFilter","JwtFilter","LoginFilter");
      METHOD_ADD(Resource::SearchMenu,"/Search/Menu",Get,Post,Options,"MyJsonFilter","JwtFilter","LoginFilter");
      METHOD_ADD(Resource::SearchContent,"/Search/Content",Get,Post,Options,"MyJsonFilter","JwtFilter","LoginFilter");
      METHOD_ADD(Resource::Upload,"/Upload",Get,Post,Options,"MyJsonFilter","JwtFilter","LoginFilter");
    METHOD_LIST_END
    void Download(const HttpRequestPtr &req,std::function<void (const HttpResponsePtr &)> &&callback) const;
    void Read(const HttpRequestPtr &req,std::function<void (const HttpResponsePtr &)> &&callback) const;
    void Search(const HttpRequestPtr &req,std::function<void (const HttpResponsePtr &)> &&callback) const;
    void SearchMenu(const HttpRequestPtr &req,std::function<void (const HttpResponsePtr &)> &&callback) const;
    void SearchContent(const HttpRequestPtr &req,std::function<void (const HttpResponsePtr &)> &&callback) const;
    void Upload(const HttpRequestPtr &req,std::function<void (const HttpResponsePtr &)> &&callback) const;
};
}
