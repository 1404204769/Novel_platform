#include "Admin_Examine.h"
using namespace Admin;
//add definition of your processing function here

// 管理员审核用户上传接口
void Examine::UpList(const HttpRequestPtr &req,std::function<void (const HttpResponsePtr &)> &&callback) const
{
    auto MyJsonPtr = app().getPlugin<MyJson>();
    const unordered_map<string,string>parameters = req->getParameters();
    Json::Value RespVal;
    MyJsonPtr->UnMapToJson(RespVal, parameters, "parameters");
    RespVal["简介"] = "管理员审核用户上传接口";
    cout << RespVal.toStyledString() << endl;
    auto resp=HttpResponse::newHttpJsonResponse(RespVal);
    resp->setStatusCode(k200OK);
    resp->setContentTypeCode(CT_TEXT_HTML);
    callback(resp);
}

// 管理员查看用户意见接口
void Examine::IdeaList(const HttpRequestPtr &req,std::function<void (const HttpResponsePtr &)> &&callback) const
{
    auto MyJsonPtr = app().getPlugin<MyJson>();
    const unordered_map<string,string>parameters = req->getParameters();
    Json::Value RespVal;
    MyJsonPtr->UnMapToJson(RespVal, parameters, "parameters");
    RespVal["简介"] = "管理员查看用户意见接口";
    cout << RespVal.toStyledString() << endl;
    auto resp=HttpResponse::newHttpJsonResponse(RespVal);
    resp->setStatusCode(k200OK);
    resp->setContentTypeCode(CT_TEXT_HTML);
    callback(resp);
}