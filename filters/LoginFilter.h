/**
 *
 *  LoginFilter.h
 *
 */

#pragma once

#include <drogon/HttpFilter.h>
#include <plugins/MyJwt.h>
#include <plugins/MyBase.h>
using namespace drogon;


class LoginFilter : public HttpFilter<LoginFilter>
{
  public:
    LoginFilter() {}
    virtual void doFilter(const HttpRequestPtr &req,
                          FilterCallback &&fcb,
                          FilterChainCallback &&fccb) override;
};

