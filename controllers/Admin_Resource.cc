#include "Admin_Resource.h"
using namespace Admin;
//add definition of your processing function here

// 管理员资源上传接口
void Resource::Upload(const HttpRequestPtr &req,std::function<void (const HttpResponsePtr &)> &&callback) const
{
    Json::Value RespVal;
    drogon::HttpResponsePtr Result;
    auto MyBasePtr = app().getPlugin<MyBase>();
    auto MyJsonPtr = app().getPlugin<MyJson>();
    const unordered_map<string,string>umapPara = req->getParameters();
    MyBasePtr->TRACELog("Resource::Upload::body" + string(req->getBody()), true);

    RespVal["简介"] = "管理员资源上传接口";
    MyJsonPtr->UnMapToJson(RespVal, umapPara, "Para");
    MyBasePtr->DEBUGLog("RespVal::" + RespVal.toStyledString(), true);

    Result=HttpResponse::newHttpJsonResponse(RespVal);

    Result->setStatusCode(k200OK);
    Result->setContentTypeCode(CT_TEXT_HTML);
    callback(Result);
}

// 管理员资源下载接口
void Resource::Download(const HttpRequestPtr &req,std::function<void (const HttpResponsePtr &)> &&callback) const
{
    Json::Value RespVal;
    drogon::HttpResponsePtr Result;
    auto MyBasePtr = app().getPlugin<MyBase>();
    auto MyJsonPtr = app().getPlugin<MyJson>();
    const unordered_map<string,string>umapPara = req->getParameters();
    MyBasePtr->TRACELog("Resource::Download::body" + string(req->getBody()), true);
    
    RespVal["简介"] = "管理员资源下载接口";
    MyJsonPtr->UnMapToJson(RespVal, umapPara, "Para");
    MyBasePtr->DEBUGLog("RespVal::" + RespVal.toStyledString(), true);

    Result=HttpResponse::newHttpJsonResponse(RespVal);

    Result->setStatusCode(k200OK);
    Result->setContentTypeCode(CT_TEXT_HTML);
    callback(Result);
}

// 管理员资源查询接口
void Resource::Search(const HttpRequestPtr &req,std::function<void (const HttpResponsePtr &)> &&callback) const
{
    Json::Value RespVal;
    drogon::HttpResponsePtr Result;
    auto MyBasePtr = app().getPlugin<MyBase>();
    auto MyJsonPtr = app().getPlugin<MyJson>();
    const unordered_map<string,string>umapPara = req->getParameters();
    MyBasePtr->TRACELog("Resource::Search::body" + string(req->getBody()), true);
    
    RespVal["简介"] = "管理员资源查询接口";
    MyJsonPtr->UnMapToJson(RespVal, umapPara, "Para");
    MyBasePtr->DEBUGLog("RespVal::" + RespVal.toStyledString(), true);

    Result=HttpResponse::newHttpJsonResponse(RespVal);
    
    Result->setStatusCode(k200OK);
    Result->setContentTypeCode(CT_TEXT_HTML);
    callback(Result);
}