#include "User_PersonalData.h"
using namespace User;
// add definition of your processing function here

// 用户查询个人资料
void PersonalData::Search(const HttpRequestPtr &req, std::function<void(const HttpResponsePtr &)> &&callback) const
{

    Json::Value ReqVal, RespVal,ResultData;
    drogon::HttpResponsePtr Result;
    auto MyJsonPtr = app().getPlugin<MyJson>();
    auto MyBasePtr = app().getPlugin<MyBase>();
    auto MyRootPtr = app().getPlugin<MyRoot>();
    auto MyDBSPtr = app().getPlugin<MyDBService>();
    const unordered_map<string, string> umapPara = req->getParameters();
    MyBasePtr->TRACELog("PersonalData::Search::body" + string(req->getBody()), true);

    try
    {
        // 读取Json数据
        ReqVal= *req->getJsonObject();
        MyBasePtr->DEBUGLog("ReqVal::" + ReqVal.toStyledString(), true);

        RespVal["简介"] = "用户查询个人资料接口";
        MyJsonPtr->UnMapToJson(ReqVal, umapPara, "Para");
        MyJsonPtr->UnMapToJson(RespVal, umapPara, "Para");

        MyDBSPtr->Search_User_PersonalData(ReqVal, RespVal);
        RespVal["Result"] = "获取成功";
        MyBasePtr->DEBUGLog("RespVal::" + RespVal.toStyledString(), true);

        // 设置返回格式
        ResultData["Message"] = RespVal["Result"];
        ResultData["Data"]["User_Data"] = RespVal["User_Data"];
        
        Json::Value TempReq,TempResp;
        
        TempReq["User_ID"] = atoi(ReqVal["Para"]["User_ID"].asString().c_str()); 
        TempReq["Para"] = ReqVal["Para"]; 
        MyDBSPtr->Search_Resource_Action_Count(TempReq, TempResp);
        if(!TempResp["Result"].asBool())
            throw TempResp;
        ResultData["Data"]["Action_Count"] = TempResp["Action_Count"];


        Json::Value LevelConfig = MyRootPtr->getCurrentLevelConfig(RespVal["User_Data"]["Total_Integral"].asInt());

        ResultData["Data"]["LevelConfig"] = LevelConfig;
        ResultData["Result"] = true;
        
        Result=HttpResponse::newHttpJsonResponse(ResultData);
    }
    catch (Json::Value &RespVal)
    {
        RespVal["Result"] = "获取失败";
        MyBasePtr->TRACE_ERROR(RespVal["ErrorMsg"]);

        
        // 设置返回格式
        int ErrorSize = RespVal["ErrorMsg"].size();
        ResultData["Result"] = false;
        ResultData["Message"] = RespVal["ErrorMsg"][ErrorSize - 1];

        Result = HttpResponse::newHttpJsonResponse(ResultData);
    }
    catch (const drogon::orm::DrogonDbException &e)
    {

        if (e.base().what() == string("0 rows found"))
        {
            RespVal["ErrorMsg"].append("此用户不存在");
        }
        else if (e.base().what() == string("Found more than one row"))
        {
            RespVal["ErrorMsg"].append("用户ID重复,请联系管理员");
        }
        else
        {
            RespVal["ErrorMsg"].append(e.base().what());
        }
        RespVal["Result"] = "获取失败";
        MyBasePtr->TRACE_ERROR(RespVal["ErrorMsg"]);
        // 设置返回格式
        int ErrorSize = RespVal["ErrorMsg"].size();
        ResultData["Result"] = false;
        ResultData["Message"] = RespVal["ErrorMsg"][ErrorSize - 1];

        Result = HttpResponse::newHttpJsonResponse(ResultData);
    }
    catch (...)
    {
        RespVal["Result"] = "获取失败";
        RespVal["ErrorMsg"].append("PersonalData::Search::Error");
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

// 用户更新个人资料
void PersonalData::Update(const HttpRequestPtr &req, std::function<void(const HttpResponsePtr &)> &&callback) const
{
    Json::Value ReqVal, RespVal,ResultData;
    drogon::HttpResponsePtr Result;
    auto MyJsonPtr = app().getPlugin<MyJson>();
    auto MyBasePtr = app().getPlugin<MyBase>();
    auto MyDBSPtr = app().getPlugin<MyDBService>();
    const unordered_map<string, string> umapPara = req->getParameters();
    MyBasePtr->TRACELog("PersonalData::Update::body" + string(req->getBody()), true);

    try
    {
        // 读取Json数据
        ReqVal = *req->getJsonObject();
        MyBasePtr->DEBUGLog("ReqVal::" + ReqVal.toStyledString(), true);

        RespVal["简介"] = "用户查询个人资料接口";
        MyJsonPtr->UnMapToJson(ReqVal, umapPara, "Para");
        MyJsonPtr->UnMapToJson(RespVal, umapPara, "Para");

        // 开始检查传入参数
        {
            MyBasePtr->DEBUGLog("开始检查传入参数是否合法", true);
            // "Change_ID"      :   0 ,
            // "Change_Col"     :   ["Change_Name","Change_Password","Change_Sex"]
            std::map<string, MyJson::ColType> ColMap;
            ColMap["Change_ID"] = MyJson::ColType::INT;
            ColMap["Change_Col"] = MyJson::ColType::ARRAY;
            MyJsonPtr->checkMemberAndTypeInMap(ReqVal, RespVal, ColMap);
            MyBasePtr->DEBUGLog("传入参数合法", true);
        }
        
        MyDBSPtr->Update_User_PersonalData(ReqVal, RespVal);
        // 设置返回格式
        ResultData["Result"] = true;
        ResultData["Message"] = "个人资料更新成功";
        ResultData["Data"] = RespVal["User_Data"];

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
    catch (const drogon::orm::DrogonDbException &e)
    {

        if (e.base().what() == string("0 rows found"))
        {
            RespVal["ErrorMsg"].append("要更改的用户不存在");
        }
        else if (e.base().what() == string("Found more than one row"))
        {
            RespVal["ErrorMsg"].append("要更改的用户ID重复,请联系管理员");
        }
        else
        {
            RespVal["ErrorMsg"].append(e.base().what());
        }
        MyBasePtr->TRACE_ERROR(RespVal["ErrorMsg"]);
        // 设置返回格式
        int ErrorSize = RespVal["ErrorMsg"].size();
        ResultData["Result"] = false;
        ResultData["Message"] = RespVal["ErrorMsg"][ErrorSize - 1];

        Result = HttpResponse::newHttpJsonResponse(ResultData);
    }
    catch (...)
    {
        RespVal["Result"] = "更新失败";
        RespVal["ErrorMsg"].append("PersonalData::Update::Error");
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