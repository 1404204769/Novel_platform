/**
 *
 *  TimeFilter.cc
 *
 */

#include <trantor/utils/Date.h>
#include <trantor/utils/Logger.h>
#include <trantor/utils/Funcs.h>
#include <drogon/*>
#define VDate "visitDate"
using namespace drogon;

void doFilter(const HttpRequestPtr &req,
                         FilterCallback &&fcb,
                         FilterChainCallback &&fccb)
{
    trantor::Date now=trantor::Date::date();
    LOG_TRACE<<"";
    if(req->session()->find(VDate))
    {
        auto lastDate=req->session()->get<trantor::Date>(VDate);
        LOG_TRACE<<"last:"<<lastDate.toFormattedString(false);
        req->session()->modify<trantor::Date>(VDate,
                                        [now](trantor::Date &vdate) {
                                            vdate = now;
                                        });
        LOG_TRACE<<"update visitDate";
        if(now>lastDate.after(10))
        {
            //10 sec later can visit again;
            fccb();
            return;
        }
        else
        {
            Json::Value json;
            json["result"]="error";
            json["message"]="Access interval should be at least 10 seconds";
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
