/**
 *
 *  MyJsonFilter.h
 *
 */

#pragma once

#include <drogon/HttpFilter.h>
#include <plugins/MyBase.h>
using namespace drogon;


class MyJsonFilter : public HttpFilter<MyJsonFilter>
{
  public:
    MyJsonFilter() {}
    virtual void doFilter(const HttpRequestPtr &req,
                          FilterCallback &&fcb,
                          FilterChainCallback &&fccb) override;
};

