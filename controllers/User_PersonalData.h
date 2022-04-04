#pragma once
#include <drogon/HttpController.h>
#include <plugins/MyJson.h>
#include <plugins/MyBase.h>
#include <plugins/MyTools.h>
#include <plugins/MyDBService.h>
using namespace drogon;
namespace User
{
class PersonalData:public drogon::HttpController<PersonalData>
{
  public:
    METHOD_LIST_BEGIN
      METHOD_ADD(PersonalData::Search,"/Search",Get,Post,Options,"MyJsonFilter","JwtFilter","LoginFilter");
      METHOD_ADD(PersonalData::Update,"/Update",Get,Post,Options,"MyJsonFilter","JwtFilter","LoginFilter");

    METHOD_LIST_END
    void Search(const HttpRequestPtr &req,std::function<void (const HttpResponsePtr &)> &&callback) const;
    void Update(const HttpRequestPtr &req,std::function<void (const HttpResponsePtr &)> &&callback) const;
};
}
