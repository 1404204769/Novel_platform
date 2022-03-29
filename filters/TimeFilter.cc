/**
 *
 *  TimeFilter.cc
 *
 */

#include "TimeFilter.h"
#include <trantor/utils/Date.h>
#include <trantor/utils/Logger.h>
#define VDate "visitDate"
using namespace drogon;

void TimeFilter::doFilter(const HttpRequestPtr &req,
                         FilterCallback &&fcb,
                         FilterChainCallback &&fccb)
{
    trantor::Date now=trantor::Date::date();
    LOG_TRACE<<"";
    if(req->session()->find(VDate))
    {
        //如果不是第一次登录
        auto lastDate=req->session()->get<trantor::Date>(VDate);
        LOG_TRACE<<"last:"<<lastDate.toFormattedString(false);
        if(now>lastDate.after(10))
        {
            //10 sec later can visit again;
            LOG_TRACE<<"update visitDate";
            req->session()->modify<trantor::Date>(VDate,
                                        [now](trantor::Date &vdate) {
                                            vdate = now;
                                        });
            fccb();
            return;
        }
        else
        {
            Json::Value json;
            json["result"]="error";
            json["message"]="Access interval should be at least 10 seconds";
            json["lastDate"]=lastDate.toFormattedString(false);
            json["提示"]="距离上次操作还没超过10s,请缓慢操作";
            auto res=HttpResponse::newHttpJsonResponse(json);
            fcb(res);
            return;
        }
    }
    LOG_TRACE<<"first access,insert visitDate";
    //LOG_DEBUG<<"first access,insert visitDate";
    req->session()->insert(VDate,now);
    fccb();
}
