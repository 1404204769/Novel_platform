#pragma once
#include <drogon/HttpSimpleController.h>
#include <plugins/MyJson.h>
using namespace drogon;
namespace User
{
class Feedback:public drogon::HttpSimpleController<Feedback>
{
  public:
    virtual void asyncHandleHttpRequest(const HttpRequestPtr& req, std::function<void (const HttpResponsePtr &)> &&callback) override;
    PATH_LIST_BEGIN
      PATH_ADD("/User/Feedback",Get,Post,Options,"MyJsonFilter","JwtFilter","LoginFilter","TimeFilter");
    PATH_LIST_END
};
}
