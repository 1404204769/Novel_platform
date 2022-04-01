#include "Root.h"
//add definition of your processing function here

void Root::Instructions(const HttpRequestPtr &req,std::function<void (const HttpResponsePtr &)> &&callback) const
{
    //write your application logic here
	std::cout<< "req body" << req->getBody()<<std::endl;
	Json::Value RespVal;
    drogon::HttpResponsePtr result;
    auto *MyJsonPtr = app().getPlugin<MyJson>();
    auto *MyRootPtr = app().getPlugin<MyRoot>();
	RespVal["Result"] = "false";
	auto jsonptr=req->getJsonObject();

    try{
        // 读取Json数据
        Json::Value json=*jsonptr;
        MyJsonPtr->checkMember(json,RespVal,"Instruction");
        MyRoot::Instructions instruct;
        if(json["Instruction"].asString() == "restart")
        {
            MyRootPtr->restart();
            RespVal["Result"]="success";
        }
        if(json["Instruction"].asString() == "close")
        {
            MyRootPtr->close();
            RespVal["Result"]="success";
        }
        
        result=HttpResponse::newHttpJsonResponse(RespVal);
	    std::cout << "RespVal : "<< RespVal.toStyledString() <<std::endl;
    }
    catch(Json::Value &e)
    {
        std::cout << RespVal["ErrorMsg"] <<std::endl;
        result=HttpResponse::newHttpJsonResponse(RespVal);
    }
    catch(const drogon::orm::DrogonDbException &e)
    {
        RespVal["ErrorMsg"] = e.base().what();
        std::cout << RespVal["ErrorMsg"] <<std::endl;
        result=HttpResponse::newHttpJsonResponse(RespVal);
    }

    result->setStatusCode(k200OK);
    result->setContentTypeCode(CT_TEXT_HTML);
    callback(result);
}