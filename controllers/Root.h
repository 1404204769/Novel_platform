#pragma once
#include <drogon/HttpController.h>
#include <plugins/MyTools.h>
using namespace drogon;
class Root:public drogon::HttpController<Root>
{  
  public:
    METHOD_LIST_BEGIN
      METHOD_ADD(Root::Instructions,"/Instructions",Get,Post,Options,"MyJsonFilter");
    METHOD_LIST_END
    void Instructions(const HttpRequestPtr &req,std::function<void (const HttpResponsePtr &)> &&callback) const;
    
  };