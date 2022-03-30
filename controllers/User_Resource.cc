#include "User_Resource.h"
using namespace User;
//add definition of your processing function here

// 图书上传接口
void Resource::Upload(const HttpRequestPtr &req,std::function<void (const HttpResponsePtr &)> &&callback) const
{
    //write your application logic here
    auto resp=HttpResponse::newHttpResponse();
    resp->setStatusCode(k200OK);
    resp->setContentTypeCode(CT_TEXT_HTML);
    resp->setBody("Hello World!");
    resp->setBody("<a href=\"/General_Interface/show\">图书上传接口</a>");
    callback(resp);
}

// 图书下载接口
void Resource::Download(const HttpRequestPtr &req,std::function<void (const HttpResponsePtr &)> &&callback) const
{
    auto MyJsonPtr = app().getPlugin<MyJson>();
    const unordered_map<string,string>parameters = req->getParameters();
    Json::Value RespVal;
    MyJsonPtr->UnMapToJson(RespVal, parameters, "parameters");
    RespVal["简介"] = "图书下载接口";
    cout << RespVal.toStyledString() << endl;
    auto resp=HttpResponse::newHttpJsonResponse(RespVal);
    resp->setStatusCode(k200OK);
    resp->setContentTypeCode(CT_TEXT_HTML);
    callback(resp);
}

// 图书查找接口
void Resource::Search(const HttpRequestPtr &req,std::function<void (const HttpResponsePtr &)> &&callback) const
{
    auto MyJsonPtr = app().getPlugin<MyJson>();
    const unordered_map<string,string>parameters = req->getParameters();
    Json::Value RespVal;
    MyJsonPtr->UnMapToJson(RespVal, parameters, "parameters");
    RespVal["简介"] = "图书查找接口";
    cout << RespVal.toStyledString() << endl;
    auto resp=HttpResponse::newHttpJsonResponse(RespVal);
    resp->setStatusCode(k200OK);
    resp->setContentTypeCode(CT_TEXT_HTML);
    callback(resp);
}