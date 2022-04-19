#pragma once
#include <drogon/HttpController.h>
#include <plugins/MyDBService.h>
using namespace drogon;
namespace User
{
  class Note : public drogon::HttpController<Note>
  {
  public:
    METHOD_LIST_BEGIN
    METHOD_ADD(Note::Appeal, "/Appeal", Get, Post, Options, "MyJsonFilter", "JwtFilter", "LoginFilter");
    METHOD_ADD(Note::Comment, "/Comment", Get, Post, Options, "MyJsonFilter", "JwtFilter", "LoginFilter");
    METHOD_ADD(Note::Like, "/Like", Get, Post, Options, "MyJsonFilter", "JwtFilter", "LoginFilter");
    METHOD_ADD(Note::Reply, "/Reply", Get, Post, Options, "MyJsonFilter", "JwtFilter", "LoginFilter");

    METHOD_LIST_END
    void Appeal(const HttpRequestPtr &req, std::function<void(const HttpResponsePtr &)> &&callback) const;
    void Comment(const HttpRequestPtr &req, std::function<void(const HttpResponsePtr &)> &&callback) const;
    void Like(const HttpRequestPtr &req, std::function<void(const HttpResponsePtr &)> &&callback) const;
    void Reply(const HttpRequestPtr &req, std::function<void(const HttpResponsePtr &)> &&callback) const;
  };
}
