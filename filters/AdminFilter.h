/**
 *
 *  AdminFilter.h
 *
 */

#pragma once

#include <drogon/HttpFilter.h>
#include <plugins/MyBase.h>
using namespace drogon;


class AdminFilter : public HttpFilter<AdminFilter>
{
  public:
    AdminFilter() {}
    virtual void doFilter(const HttpRequestPtr &req,
                          FilterCallback &&fcb,
                          FilterChainCallback &&fccb) override;
};

