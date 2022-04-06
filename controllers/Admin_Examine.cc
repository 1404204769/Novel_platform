#include "Admin_Examine.h"
using namespace Admin;
//add definition of your processing function here

// 管理员审核用户上传接口
void Examine::UpList(const HttpRequestPtr &req,std::function<void (const HttpResponsePtr &)> &&callback) const
{
    Json::Value ReqVal,RespVal;
    drogon::HttpResponsePtr Result;
    auto MyBasePtr = app().getPlugin<MyBase>();
    auto MyJsonPtr = app().getPlugin<MyJson>();
    const unordered_map<string,string>umapPara = req->getParameters();
    MyBasePtr->TRACELog("Examine::UpList::body" + string(req->getBody()), true);
    
    RespVal["简介"] = "管理员审核用户上传接口";
    MyJsonPtr->UnMapToJson(ReqVal, umapPara, "Para");
    MyJsonPtr->UnMapToJson(RespVal, umapPara, "Para");
    MyBasePtr->DEBUGLog("RespVal::" + RespVal.toStyledString(), true);

    Result=HttpResponse::newHttpJsonResponse(RespVal);

    Result->setStatusCode(k200OK);
    Result->setContentTypeCode(CT_TEXT_HTML);
    callback(Result);
}

// 管理员查看用户意见接口
void Examine::IdeaList(const HttpRequestPtr &req,std::function<void (const HttpResponsePtr &)> &&callback) const
{
    Json::Value ReqVal,RespVal;
    drogon::HttpResponsePtr Result;
    auto MyBasePtr = app().getPlugin<MyBase>();
    auto MyJsonPtr = app().getPlugin<MyJson>();
    const unordered_map<string,string>umapPara = req->getParameters();
    MyBasePtr->TRACELog("Examine::IdeaList::body" + string(req->getBody()), true);
    
    RespVal["简介"] = "管理员查看用户意见接口";
    MyJsonPtr->UnMapToJson(ReqVal, umapPara, "Para");
    MyJsonPtr->UnMapToJson(RespVal, umapPara, "Para");
    MyBasePtr->DEBUGLog("RespVal::" + RespVal.toStyledString(), true);

    Result=HttpResponse::newHttpJsonResponse(RespVal);
    
    Result->setStatusCode(k200OK);
    Result->setContentTypeCode(CT_TEXT_HTML);
    callback(Result);
}