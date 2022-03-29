#include "show.h"
void show::asyncHandleHttpRequest(const HttpRequestPtr& req, std::function<void (const HttpResponsePtr &)> &&callback)
{
    //write your application logic here
	Json::Value RespVal;
	const std::string token = req->getHeader("Authorization");

    //std::cout<< "req header Authorization:" << req->getHeader("Authorization")<<std::endl;
    //std::cout<< "token:" << token << std::endl;

	auto *JWTPtr = app().getPlugin<MyJwt>();//获取mytools插件
	RespVal = JWTPtr->decode(token);
	auto result=HttpResponse::newHttpJsonResponse(RespVal);
	result->setStatusCode(k200OK);
    result->setContentTypeCode(CT_TEXT_HTML);
	callback(result);
}