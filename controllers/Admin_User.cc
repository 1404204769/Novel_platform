#include "Admin_User.h"
// add definition of your processing function here
void Admin::User::List(const HttpRequestPtr &req, std::function<void(const HttpResponsePtr &)> &&callback) const
{
    drogon::HttpResponsePtr Result;
    Json::Value ReqVal, RespVal, FilterVal;
    auto MyBasePtr = app().getPlugin<MyBase>();
    auto MyJsonPtr = app().getPlugin<MyJson>();
    auto MyDBSPtr = app().getPlugin<MyDBService>();
    const unordered_map<string, string> umapPara = req->getParameters();
    MyBasePtr->TRACELog("Admin::User::List::body" + string(req->getBody()), true);

    try
    {
        // 读取Json数据
        ReqVal = *req->getJsonObject();
        MyBasePtr->TRACELog("Admin::User::List::ReqVal" + ReqVal.toStyledString(), true);

        MyJsonPtr->UnMapToJson(ReqVal, umapPara, "Para");
        MyJsonPtr->UnMapToJson(RespVal, umapPara, "Para");
        RespVal["简介"] = "管理员查询所有用户接口";

        ReqVal["Search_All"] = true;
        FilterVal["User_ID"] = 0;
        FilterVal["User_Name"] = "";
        FilterVal["User_Sex"] = "";
        ReqVal["Filter"] = FilterVal;

        MyDBSPtr->Admin_Search_User(ReqVal, RespVal);

        RespVal["Result"] = "查询所有用户成功";
        MyBasePtr->DEBUGLog("RespVal::" + RespVal.toStyledString(), true);

        Result = HttpResponse::newHttpJsonResponse(RespVal);
    }
    catch (Json::Value RespVal)
    {
        RespVal["Result"] = "查询所有用户失败";
        MyBasePtr->TRACELog("ErrorMsg::" + RespVal["ErrorMsg"].asString(), true);

        Result = HttpResponse::newHttpJsonResponse(RespVal);
    }
    catch (const drogon::orm::DrogonDbException &e)
    {
        RespVal["Result"] = "查询所有用户失败";
        RespVal["ErrorMsg"] = e.base().what();
        MyBasePtr->TRACELog("ErrorMsg::" + RespVal["ErrorMsg"].asString(), true);

        Result = HttpResponse::newHttpJsonResponse(RespVal);
    }

    Result->setStatusCode(k200OK);
    Result->setContentTypeCode(CT_TEXT_HTML);
    callback(Result);
}

void Admin::User::Search(const HttpRequestPtr &req, std::function<void(const HttpResponsePtr &)> &&callback) const
{
    Json::Value ReqVal, RespVal;
    drogon::HttpResponsePtr Result;
    auto MyBasePtr = app().getPlugin<MyBase>();
    auto MyDBSPtr = app().getPlugin<MyDBService>();
    auto MyJsonPtr = app().getPlugin<MyJson>();
    const unordered_map<string, string> umapPara = req->getParameters();
    MyBasePtr->TRACELog("Admin::User::Search::body" + string(req->getBody()), true);

    try
    {
        // 读取Json数据
        ReqVal = *req->getJsonObject();
        MyBasePtr->TRACELog("Admin::User::Search::ReqVal" + ReqVal.toStyledString(), true);

        MyJsonPtr->UnMapToJson(ReqVal, umapPara, "Para");
        MyJsonPtr->UnMapToJson(RespVal, umapPara, "Para");
        RespVal["简介"] = "管理员查询指定用户接口";
        ReqVal["Search_All"] = false;
        ReqVal["Filter"] = ReqVal["UserSearch"];

        MyDBSPtr->Admin_Search_User(ReqVal, RespVal);

        RespVal["Result"] = "查询指定用户成功";
        MyBasePtr->DEBUGLog("RespVal::" + RespVal.toStyledString(), true);

        Result = HttpResponse::newHttpJsonResponse(RespVal);
    }
    catch (Json::Value RespVal)
    {
        RespVal["Result"] = "查询指定用户失败";
        MyBasePtr->TRACELog("ErrorMsg::" + RespVal["ErrorMsg"].asString(), true);
        
        Result = HttpResponse::newHttpJsonResponse(RespVal);
    }
    catch (const drogon::orm::DrogonDbException &e)
    {
        RespVal["Result"] = "查询指定用户失败";
        RespVal["ErrorMsg"] = e.base().what();
        MyBasePtr->TRACELog("ErrorMsg::" + RespVal["ErrorMsg"].asString(), true);

        Result = HttpResponse::newHttpJsonResponse(RespVal);
    }

    Result->setStatusCode(k200OK);
    Result->setContentTypeCode(CT_TEXT_HTML);
    callback(Result);
}

void Admin::User::Update(const HttpRequestPtr &req, std::function<void(const HttpResponsePtr &)> &&callback) const
{
    Json::Value ReqVal, RespVal;
    drogon::HttpResponsePtr Result;
    auto MyBasePtr = app().getPlugin<MyBase>();
    auto MyJsonPtr = app().getPlugin<MyJson>();
    const unordered_map<string, string> umapPara = req->getParameters();
    MyBasePtr->TRACELog("Admin::User::Update::body" + string(req->getBody()), true);
    
    MyJsonPtr->UnMapToJson(RespVal, umapPara, "Para");
    MyBasePtr->DEBUGLog("RespVal::" + RespVal.toStyledString(), true);

    Result = HttpResponse::newHttpJsonResponse(RespVal);

    Result->setStatusCode(k200OK);
    Result->setContentTypeCode(CT_TEXT_HTML);
    callback(Result);
}
