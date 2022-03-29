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
    Json::Value resultJson;
    //std::cout<< "JwtFilter header Authorization:" << req->getHeader("Authorization")<<std::endl;
	const std::string token = req->getHeader("Authorization");

    // If authorization header is empty
    if (token.empty()) {
        resultJson["error"] = "No header authentication!";
        resultJson["status"] = "false";

        auto res = HttpResponse::newHttpJsonResponse(resultJson);
        res->setStatusCode(k401Unauthorized);

        // Return the response and let's tell this endpoint request was cancelled
        return fcb(res);
    }

	auto *JWTPtr = app().getPlugin<MyJwt>();//获取mytools插件
    if(!JWTPtr->verify(token))
    {
        resultJson["error"] = "JWT verify false!";
        resultJson["status"] = "false";
        auto res = HttpResponse::newHttpJsonResponse(resultJson);
        res->setStatusCode(k500InternalServerError);

        // Return the response and let's tell this endpoint request was cancelled
        return fcb(res);
    }

    fccb();
}
