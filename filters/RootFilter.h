/**
 *
 *  RootFilter.h
 *
 */

#pragma once

#include <drogon/HttpFilter.h>
#include <plugins/MyBase.h>
using namespace drogon;


class RootFilter : public HttpFilter<RootFilter>
{
  public:
    RootFilter() {}
    virtual void doFilter(const HttpRequestPtr &req,
                          FilterCallback &&fcb,
                          FilterChainCallback &&fccb) override;
};

