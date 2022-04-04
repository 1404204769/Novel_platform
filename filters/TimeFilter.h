/**
 *
 *  TimeFilter.h
 *
 */

#pragma once

#include <drogon/HttpFilter.h>
#include <plugins/MyBase.h>
using namespace drogon;

class TimeFilter : public HttpFilter<TimeFilter>
{
  public:
    TimeFilter() {}
    virtual void doFilter(const HttpRequestPtr &req,
                          FilterCallback &&fcb,
                          FilterChainCallback &&fccb) override;
};

