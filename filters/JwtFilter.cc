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
    Json::Value RespVal;
    //std::cout<< "JwtFilter header Authorization:" << req->getHeader("Authorization")<<std::endl;
	const std::string token = req->getHeader("Authorization");
    auto *JWTPtr = app().getPlugin<MyJwt>();//获取mytools插件
    try{
            // If authorization header is empty
        if (token.empty()) {
            RespVal["ErrorMsg"] = "No header authentication!";
            throw RespVal;
        }
        if(!JWTPtr->verify(token))
        {
            RespVal["ErrorMsg"] = "JWTFilter 验证失败!";
            throw RespVal;
        }

        JWTPtr->PayloadToJson(token, RespVal);
        req->setParameter("LoginStatus",RespVal["LoginStatus"].asString());
        req->setParameter("UserID",RespVal["UserID"].asString());
    }
    catch(const std::exception& e)
    {  
        RespVal["ErrorMsg"] = e.what();
        auto res = HttpResponse::newHttpJsonResponse(RespVal);
        res->setStatusCode(k500InternalServerError);

        // Return the response and let's tell this endpoint request was cancelled
        return fcb(res);
    }
    catch(Json::Value &RespVal)
    {       
        auto res = HttpResponse::newHttpJsonResponse(RespVal);
        res->setStatusCode(k500InternalServerError);

        // Return the response and let's tell this endpoint request was cancelled
        return fcb(res);
    }
    std::cout << "JWTFilter 验证成功" << std::endl;
    return fccb();
}
