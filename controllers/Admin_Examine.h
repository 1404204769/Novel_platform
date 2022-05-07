#pragma once
#include <drogon/HttpController.h>
#include <plugins/MyJson.h>
#include <plugins/MyDBService.h>
using namespace drogon;
namespace Admin
{
class Examine:public drogon::HttpController<Examine>
{
  public:
    METHOD_LIST_BEGIN
      METHOD_ADD(Examine::UploadSearchList,"/Upload/Search/List",Get,Post,Options,"MyJsonFilter","JwtFilter","AdminFilter");
      METHOD_ADD(Examine::UploadSearchByID,"/Upload/Search/ID",Get,Post,Options,"MyJsonFilter","JwtFilter","AdminFilter");
      METHOD_ADD(Examine::UploadExamine,"/Upload/Examine",Get,Post,Options,"MyJsonFilter","JwtFilter","AdminFilter");

      METHOD_ADD(Examine::IdeaList,"/IdeaList",Get,Post,Options,"MyJsonFilter","JwtFilter","AdminFilter");
      METHOD_ADD(Examine::ExamineIdea,"/Idea/Examine",Get,Post,Options,"MyJsonFilter","JwtFilter","AdminFilter");

    METHOD_LIST_END
    void UploadSearchList(const HttpRequestPtr &req,std::function<void (const HttpResponsePtr &)> &&callback) const;
    void UploadSearchByID(const HttpRequestPtr &req,std::function<void (const HttpResponsePtr &)> &&callback) const;
    void UploadExamine(const HttpRequestPtr &req,std::function<void (const HttpResponsePtr &)> &&callback) const;
    void IdeaList(const HttpRequestPtr &req,std::function<void (const HttpResponsePtr &)> &&callback) const;
    void ExamineIdea(const HttpRequestPtr &req,std::function<void (const HttpResponsePtr &)> &&callback) const;

    
};
}
