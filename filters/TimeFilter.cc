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
    Json::Value ReqVal, RespVal;
    drogon::HttpResponsePtr Result;
    trantor::Date now=trantor::Date::date();
	auto *MyBasePtr = app().getPlugin<MyBase>();
    MyBasePtr->TRACELog("TimeFilter::body" + string(req->getBody()), true);

    if(req->session()->find(VDate))
    {
        //如果不是第一次登录
        auto lastDate=req->session()->get<trantor::Date>(VDate);
        MyBasePtr->DEBUGLog("最后一次登入时间::" + lastDate.toFormattedString(false), true);
        if(now>lastDate.after(10))
        {
            //10 sec later can visit again;
            MyBasePtr->DEBUGLog("更新最后登入时间", true);
            req->session()->modify<trantor::Date>(VDate,
                                        [now](trantor::Date &vdate) {
                                            vdate = now;
                                        });
            return fccb();
        }
        else
        {
            RespVal["ErrorMsg"]="距离上次操作还没超过10s,请缓慢操作";
            RespVal["最后登入时间"]=lastDate.toFormattedString(false);
            MyBasePtr->DEBUGLog("RespVal::" + RespVal.toStyledString(), true);

            Result=HttpResponse::newHttpJsonResponse(RespVal);
            return fcb(Result);
        }
    }
    MyBasePtr->DEBUGLog("首次登入，在session加入最后登入时间(visitDate)", true);
    //LOG_DEBUG<<"first access,insert visitDate";
    req->session()->insert(VDate,now);
    return fccb();
}
