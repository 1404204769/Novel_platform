#pragma once
#include <drogon/HttpController.h>
#include <plugins/MyRoot.h>
#include <plugins/MyBase.h>
using namespace drogon;
class Root:public drogon::HttpController<Root>
{  
  public:
    METHOD_LIST_BEGIN
      METHOD_ADD(Root::Instructions,"/Instructions",Get,Post,Options,"MyJsonFilter","JwtFilter","RootFilter");
    METHOD_LIST_END
    void Instructions(const HttpRequestPtr &req,std::function<void (const HttpResponsePtr &)> &&callback) const;
    
  };
