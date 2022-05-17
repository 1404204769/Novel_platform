#include "User.h"
//add definition of your processing function here
void User::Report(const HttpRequestPtr &req,std::function<void (const HttpResponsePtr &)> &&callback) const
{
    Json::Value ReqVal, RespVal,ResultData;
    drogon::HttpResponsePtr Result;
    auto MyBasePtr = app().getPlugin<MyBase>();
    auto MyJsonPtr = app().getPlugin<MyJson>();
    auto MyDBSPtr = app().getPlugin<MyDBService>();
    const unordered_map<string, string> umapPara = req->getParameters();
    MyBasePtr->TRACELog("Admin::User::Report::body" + string(req->getBody()), true);
    
 
    try
    {
        ReqVal = *req->getJsonObject();
        MyJsonPtr->UnMapToJson(ReqVal, umapPara, "Para");
        MyDBSPtr->Create_Report(ReqVal,RespVal);

        if(!RespVal["Result"].asBool())
        {
            throw RespVal;
        }

        // 设置返回格式
        RespVal["简介"] = "用户举报接口";
        ResultData["Result"] = true;
        ResultData["Message"] = "用户举报成功";
        ResultData["Data"]["Report_Data"] = RespVal["Idea_Data"];

        MyBasePtr->DEBUGLog("RespVal::" + RespVal.toStyledString(), true);
        Result = HttpResponse::newHttpJsonResponse(ResultData);
    }
    catch (Json::Value &RespVal)
    {
        MyBasePtr->TRACE_ERROR(RespVal["ErrorMsg"]);
        // 设置返回格式
        int ErrorSize = RespVal["ErrorMsg"].size();
        ResultData["Result"] = false;
        ResultData["Message"] = RespVal["ErrorMsg"][ErrorSize - 1];

        Result = HttpResponse::newHttpJsonResponse(ResultData);
    }
    catch (...)
    {
        RespVal["ErrorMsg"].append("Report::ERROR");
        MyBasePtr->TRACE_ERROR(RespVal["ErrorMsg"]);
        // 设置返回格式
        int ErrorSize = RespVal["ErrorMsg"].size();
        ResultData["Result"] = false;
        ResultData["Message"] = RespVal["ErrorMsg"][ErrorSize - 1];

        Result = HttpResponse::newHttpJsonResponse(ResultData);
    }
    
    Result->setStatusCode(k200OK);
    Result->setContentTypeCode(CT_TEXT_HTML);
    callback(Result);
}

void User::Feedback(const HttpRequestPtr &req,std::function<void (const HttpResponsePtr &)> &&callback) const
{
    Json::Value ReqVal, RespVal,ResultData;
    drogon::HttpResponsePtr Result;
    auto MyBasePtr = app().getPlugin<MyBase>();
    auto MyJsonPtr = app().getPlugin<MyJson>();
    auto MyDBSPtr = app().getPlugin<MyDBService>();
    const unordered_map<string, string> umapPara = req->getParameters();
    MyBasePtr->TRACELog("User::Feedback::body" + string(req->getBody()), true);
    
 
    try
    {
        ReqVal = *req->getJsonObject();
        RespVal["简介"] = "用户问题反馈接口";
        MyJsonPtr->UnMapToJson(ReqVal, umapPara, "Para");
        MyDBSPtr->Create_Suggestion(ReqVal,RespVal);
        
        if(!RespVal["Result"].asBool())
        {
            throw RespVal;
        }

        // 设置返回格式
        ResultData["Result"] = true;
        ResultData["Message"] = "用户举报成功";
        ResultData["Data"]["Report_Data"] = RespVal["Idea_Data"];

        Result = HttpResponse::newHttpJsonResponse(ResultData);
        MyBasePtr->DEBUGLog("RespVal::" + RespVal.toStyledString(), true);
    }
    catch (Json::Value &RespVal)
    {
        MyBasePtr->TRACE_ERROR(RespVal["ErrorMsg"]);
        // 设置返回格式
        int ErrorSize = RespVal["ErrorMsg"].size();
        ResultData["Result"] = false;
        ResultData["Message"] = RespVal["ErrorMsg"][ErrorSize - 1];

        Result = HttpResponse::newHttpJsonResponse(ResultData);
    }
    catch (...)
    {
        RespVal["ErrorMsg"].append("Feedback::ERROR");
        MyBasePtr->TRACE_ERROR(RespVal["ErrorMsg"]);
        // 设置返回格式
        int ErrorSize = RespVal["ErrorMsg"].size();
        ResultData["Result"] = false;
        ResultData["Message"] = RespVal["ErrorMsg"][ErrorSize - 1];

        Result = HttpResponse::newHttpJsonResponse(ResultData);
    }
    
    Result->setStatusCode(k200OK);
    Result->setContentTypeCode(CT_TEXT_HTML);
    callback(Result);

}

void User::Action(const HttpRequestPtr &req,std::function<void (const HttpResponsePtr &)> &&callback) const
{

    Json::Value ReqVal, RespVal,ResultData;
    drogon::HttpResponsePtr Result;
    auto MyBasePtr = app().getPlugin<MyBase>();
    auto MyJsonPtr = app().getPlugin<MyJson>();
    auto MyDBSPtr = app().getPlugin<MyDBService>();
    const unordered_map<string, string> umapPara = req->getParameters();
    MyBasePtr->TRACELog("User::Action::body" + string(req->getBody()), true);
    
 
    try
    {
        ReqVal = *req->getJsonObject();
        RespVal["简介"] = "用户行为查询接口";
        MyJsonPtr->UnMapToJson(ReqVal, umapPara, "Para");
        MyDBSPtr->Search_Action_By_UserID(ReqVal,RespVal);
        
        if(!RespVal["Result"].asBool())
        {
            throw RespVal;
        }

        // 设置返回格式
        ResultData["Result"] = true;
        ResultData["Message"] = "用户查询行为信息成功";
        ResultData["Data"]["Action_List"] = RespVal["Action_List"];

        Result = HttpResponse::newHttpJsonResponse(ResultData);
        MyBasePtr->DEBUGLog("RespVal::" + RespVal.toStyledString(), true);
    }
    catch (Json::Value &RespVal)
    {
        MyBasePtr->TRACE_ERROR(RespVal["ErrorMsg"]);
        // 设置返回格式
        int ErrorSize = RespVal["ErrorMsg"].size();
        ResultData["Result"] = false;
        ResultData["Message"] = RespVal["ErrorMsg"][ErrorSize - 1];

        Result = HttpResponse::newHttpJsonResponse(ResultData);
    }
    catch (...)
    {
        RespVal["ErrorMsg"].append("Action::ERROR");
        MyBasePtr->TRACE_ERROR(RespVal["ErrorMsg"]);
        // 设置返回格式
        int ErrorSize = RespVal["ErrorMsg"].size();
        ResultData["Result"] = false;
        ResultData["Message"] = RespVal["ErrorMsg"][ErrorSize - 1];

        Result = HttpResponse::newHttpJsonResponse(ResultData);
    }
    
    Result->setStatusCode(k200OK);
    Result->setContentTypeCode(CT_TEXT_HTML);
    callback(Result);
}