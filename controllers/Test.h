#pragma once
#include <drogon/HttpController.h>
#include <plugins/MyTools.h>
#include <plugins/MyBase.h>
using namespace drogon;
class Test:public drogon::HttpController<Test>
{
  public:
    METHOD_LIST_BEGIN
      METHOD_ADD(Test::ChineseStr,"/ChineseStr",Get,Post,Options,"MyJsonFilter");
      METHOD_ADD(Test::SysOutLevel,"/SysOutLevel",Get,Post,Options,"MyJsonFilter");
    METHOD_LIST_END
    void ChineseStr(const HttpRequestPtr &req,std::function<void (const HttpResponsePtr &)> &&callback) const;
    void SysOutLevel(const HttpRequestPtr &req,std::function<void (const HttpResponsePtr &)> &&callback) const;
};
