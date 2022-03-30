#pragma once
#include <drogon/HttpSimpleController.h>
#include <plugins/MyJson.h>
using namespace drogon;
namespace Admin
{
class Economics:public drogon::HttpSimpleController<Economics>
{
  public:
    virtual void asyncHandleHttpRequest(const HttpRequestPtr& req, std::function<void (const HttpResponsePtr &)> &&callback) override;
    PATH_LIST_BEGIN
      PATH_ADD("/Admin/Economics",Get,Post,Options,"MyJsonFilter","JwtFilter","AdminFilter");
    PATH_LIST_END
};
}
