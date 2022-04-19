/**
 *
 *  JwtFilter.cc
 *
 */

#include "JwtFilter.h"

using namespace drogon;

void JwtFilter::doFilter(const HttpRequestPtr &req,
                         FilterCallback &&fcb,
                         FilterChainCallback &&fccb)
{
    Json::Value ReqVal, RespVal;
    drogon::HttpResponsePtr Result;
    auto *JWTPtr = app().getPlugin<MyJwt>();
	auto *MyBasePtr = app().getPlugin<MyBase>();
    MyBasePtr->TRACELog("JwtFilter::body" + string(req->getBody()), true);

    
    MyBasePtr->DEBUGLog("JwtFilter header Authorization:" + req->getHeader("Authorization"), true);

	const std::string token = req->getHeader("Authorization");
    try{
        // If authorization header is empty
        if (token.empty()) {
            RespVal["ErrorMsg"].append("No header authentication!");
            throw RespVal;
        }
        if(!JWTPtr->verify(token))
        {
            RespVal["ErrorMsg"].append("JWTFilter 验证失败!");
            throw RespVal;
        }

        JWTPtr->PayloadToJson(token, RespVal);
        req->setParameter("Login_Status",RespVal["Login_Status"].asString());
        req->setParameter("User_ID",RespVal["User_ID"].asString());


    }
    catch(const std::exception& e)
    {  
        RespVal["ErrorMsg"].append(e.what());
        MyBasePtr->DEBUGLog("RespVal::" + RespVal.toStyledString(), true);

        Result = HttpResponse::newHttpJsonResponse(RespVal);

        Result->setStatusCode(k500InternalServerError);
        // Return the response and let's tell this endpoint request was cancelled
        return fcb(Result);
    }
    catch(Json::Value &RespVal)
    {   
        MyBasePtr->DEBUGLog("RespVal::" + RespVal.toStyledString(), true);

        Result = HttpResponse::newHttpJsonResponse(RespVal);

        Result->setStatusCode(k500InternalServerError);
        // Return the response and let's tell this endpoint request was cancelled
        return fcb(Result);
    }

    MyBasePtr->TRACELog("JWTFilter 验证成功", true);
    return fccb();
}
