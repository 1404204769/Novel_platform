#pragma once
#include <drogon/HttpController.h>
#include <plugins/MyRoot.h>
#include <plugins/MyBase.h>
#include <plugins/MyJson.h>
#include <plugins/MyDBService.h>
using namespace drogon;
class Root:public drogon::HttpController<Root>
{  
  public:
    METHOD_LIST_BEGIN
      METHOD_ADD(Root::Instructions,"/Instructions",Get,Post,Options,"MyJsonFilter","JwtFilter","RootFilter");
      METHOD_ADD(Root::AdminControl,"/AdminControl",Get,Post,Options,"MyJsonFilter","JwtFilter","RootFilter");
      METHOD_ADD(Root::SysOutLevelSearch,"/SysOutLevel/Search",Get,Post,Options,"MyJsonFilter","JwtFilter","RootFilter");
      METHOD_ADD(Root::SysOutLevelUpdate,"/SysOutLevel/Update",Get,Post,Options,"MyJsonFilter","JwtFilter","RootFilter");
    METHOD_LIST_END
    void Instructions(const HttpRequestPtr &req,std::function<void (const HttpResponsePtr &)> &&callback) const;
    void AdminControl(const HttpRequestPtr &req,std::function<void (const HttpResponsePtr &)> &&callback) const;
    void SysOutLevelSearch(const HttpRequestPtr &req,std::function<void (const HttpResponsePtr &)> &&callback) const;
    void SysOutLevelUpdate(const HttpRequestPtr &req,std::function<void (const HttpResponsePtr &)> &&callback) const;
    
  };
