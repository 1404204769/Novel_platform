#include "Admin_Resource.h"
using namespace Admin;
//add definition of your processing function here

// 管理员资源上传接口
void Resource::Upload(const HttpRequestPtr &req,std::function<void (const HttpResponsePtr &)> &&callback) const
{
    auto MyJsonPtr = app().getPlugin<MyJson>();
    const unordered_map<string,string>parameters = req->getParameters();
    Json::Value RespVal;
    MyJsonPtr->UnMapToJson(RespVal, parameters, "parameters");
    RespVal["简介"] = "管理员资源上传接口";
    cout << RespVal.toStyledString() << endl;
    auto resp=HttpResponse::newHttpJsonResponse(RespVal);
    resp->setStatusCode(k200OK);
    resp->setContentTypeCode(CT_TEXT_HTML);
    callback(resp);
}

// 管理员资源下载接口
void Resource::Download(const HttpRequestPtr &req,std::function<void (const HttpResponsePtr &)> &&callback) const
{
    auto MyJsonPtr = app().getPlugin<MyJson>();
    const unordered_map<string,string>parameters = req->getParameters();
    Json::Value RespVal;
    MyJsonPtr->UnMapToJson(RespVal, parameters, "parameters");
    RespVal["简介"] = "管理员资源下载接口";
    cout << RespVal.toStyledString() << endl;
    auto resp=HttpResponse::newHttpJsonResponse(RespVal);
    resp->setStatusCode(k200OK);
    resp->setContentTypeCode(CT_TEXT_HTML);
    callback(resp);
}

// 管理员资源查询接口
void Resource::Search(const HttpRequestPtr &req,std::function<void (const HttpResponsePtr &)> &&callback) const
{
    auto MyJsonPtr = app().getPlugin<MyJson>();
    const unordered_map<string,string>parameters = req->getParameters();
    Json::Value RespVal;
    MyJsonPtr->UnMapToJson(RespVal, parameters, "parameters");
    RespVal["简介"] = "管理员资源查询接口";
    cout << RespVal.toStyledString() << endl;
    auto resp=HttpResponse::newHttpJsonResponse(RespVal);
    resp->setStatusCode(k200OK);
    resp->setContentTypeCode(CT_TEXT_HTML);
    callback(resp);
}