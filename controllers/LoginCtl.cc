#include "LoginCtl.h"
void LoginCtl::asyncHandleHttpRequest(const HttpRequestPtr &req, std::function<void(const HttpResponsePtr &)> &&callback)
{
	//write your application logic here
	std::cout<< "req body" << req->getBody()<<std::endl;
	std::cout << "req getQuery:  "<<req->getQuery() <<std::endl;
	std::cout << "req getPath:  "<<req->getPath()<<std::endl;
	std::cout << "req UserID "<<req->getParameter("UserId")<<std::endl;
	std::cout << "req UserPwd "<<req->getParameter("UserPwd")<<std::endl;
	std::cout << "req getJsonError "<<req->getJsonError()<<std::endl;
	Json::Value RespVal;
	RespVal["result"] = "false";
	auto jsonptr=req->getJsonObject();
	if(jsonptr){
		Json::Value json=*jsonptr;
		std::cout<< json.toStyledString()<<std::endl;
		auto UserId=json["UserId"].asString();
		auto UserPwd=json["UserPwd"].asString();
		if (UserId == "admin" && UserPwd  == "admin")
		{
			RespVal["result"] = "true";
			auto *JWTPtr = app().getPlugin<MyJwt>();//获取MyJwt插件
			Json::Value param;
			param["user"] = "admin";
			param["result"] = "success";
			RespVal["Token"] = JWTPtr->encode(param);
		}
		std::cout << "jsonptr true"<<std::endl;
		
	}else{
		std::cout << "jsonptr false"<<std::endl;
	}
	std::cout << "login result "<<RespVal["result"].asString()<<std::endl;
	auto result=HttpResponse::newHttpJsonResponse(RespVal);
	result->setStatusCode(k200OK);
    result->setContentTypeCode(CT_TEXT_HTML);
	callback(result);
}