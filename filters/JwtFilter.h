/**
 *
 *  JwtFilter.h
 *
 */

#pragma once

#include <drogon/HttpFilter.h>
#include <plugins/MyJwt.h>
#include <plugins/MyBase.h>

using namespace drogon;

class JwtFilter : public HttpFilter<JwtFilter>
{
  public:
    JwtFilter() {}
    virtual void doFilter(const HttpRequestPtr &req,
                          FilterCallback &&fcb,
                          FilterChainCallback &&fccb) override;
};

