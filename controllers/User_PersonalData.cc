#include "User_PersonalData.h"
using namespace User;
// add definition of your processing function here

// 用户查询个人资料
void PersonalData::Search(const HttpRequestPtr &req, std::function<void(const HttpResponsePtr &)> &&callback) const
{

    Json::Value ReqVal, RespVal;
    drogon::HttpResponsePtr Result;
    auto MyJsonPtr = app().getPlugin<MyJson>();
    auto MyBasePtr = app().getPlugin<MyBase>();
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

        Result = HttpResponse::newHttpJsonResponse(RespVal);
    }
    catch (Json::Value &RespVal)
    {
        RespVal["Result"] = "获取失败";
        MyBasePtr->TRACELog(RespVal["ErrorMsg"].asString(), true);

        Result = HttpResponse::newHttpJsonResponse(RespVal);
    }
    catch (const drogon::orm::DrogonDbException &e)
    {

        if (e.base().what() == string("0 rows found"))
        {
            RespVal["ErrorMsg"] = "此用户不存在";
        }
        else if (e.base().what() == string("Found more than one row"))
        {
            RespVal["ErrorMsg"] = "用户ID重复,请联系管理员";
        }
        else
        {
            RespVal["ErrorMsg"] = e.base().what();
        }
        RespVal["Result"] = "获取失败";
        MyBasePtr->TRACELog("ErrorMsg::" + RespVal["ErrorMsg"].asString(), true);

        Result = HttpResponse::newHttpJsonResponse(RespVal);
    }
    catch (...)
    {
        RespVal["Result"] = "获取失败";
        RespVal["ErrorMsg"] = "PersonalData::Search::Error";
        MyBasePtr->TRACELog("ErrorMsg::" + RespVal["ErrorMsg"].asString(), true);

        Result = HttpResponse::newHttpJsonResponse(RespVal);
    }

    Result->setStatusCode(k200OK);
    Result->setContentTypeCode(CT_TEXT_HTML);
    callback(Result);
}

// 用户更新个人资料
void PersonalData::Update(const HttpRequestPtr &req, std::function<void(const HttpResponsePtr &)> &&callback) const
{
    Json::Value ReqVal, RespVal;
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
            // "Change_Name"    :   "",
            // "Change_Password":   "",
            // "Change_Sex"     :   "",
            // "Change_Col"     :   ["Change_Name","Change_Password","Change_Sex"]
            std::map<string, MyJson::ColType> ColMap;
            ColMap["Change_ID"] = MyJson::ColType::INT;
            ColMap["Change_Name"] = MyJson::ColType::STRING;
            ColMap["Change_Password"] = MyJson::ColType::STRING;
            ColMap["Change_Sex"] = MyJson::ColType::STRING;
            ColMap["Change_Col"] = MyJson::ColType::ARRAY;
            MyJsonPtr->checkMemberAndTypeInMap(ReqVal, RespVal, ColMap);
            MyBasePtr->DEBUGLog("传入参数合法", true);
        }
        ReqVal["User_ID"] = std::atoi(RespVal["Para"]["User_ID"].asString().c_str());

        // 检查是否修改的是自己的资料
        if (ReqVal["Change_ID"].asInt() != ReqVal["User_ID"].asInt())
        {
            RespVal["ErrorMsg"] = "权限不足，请联系管理员(操作用户: " 
                            + to_string(ReqVal["User_ID"].asInt()) 
                            + ",被操作用户: " 
                            + to_string(ReqVal["Change_ID"].asInt()) + ")";
            throw RespVal;
        }
        
        MyDBSPtr->Update_User_PersonalData(ReqVal, RespVal);
        RespVal["Result"] = "更新成功";
        MyBasePtr->DEBUGLog("RespVal::" + RespVal.toStyledString(), true);

        Result = HttpResponse::newHttpJsonResponse(RespVal);
    }
    catch (Json::Value &RespVal)
    {
        RespVal["Result"] = "更新失败";
        MyBasePtr->TRACELog("ErrorMsg::" + RespVal["ErrorMsg"].asString(), true);

        Result = HttpResponse::newHttpJsonResponse(RespVal);
    }
    catch (const drogon::orm::DrogonDbException &e)
    {

        if (e.base().what() == string("0 rows found"))
        {
            RespVal["ErrorMsg"] = "要更改的用户不存在";
        }
        else if (e.base().what() == string("Found more than one row"))
        {
            RespVal["ErrorMsg"] = "要更改的用户ID重复,请联系管理员";
        }
        else
        {
            RespVal["ErrorMsg"] = e.base().what();
        }
        RespVal["Result"] = "更新失败";
        RespVal["ErrorMsg"] = e.base().what();
        MyBasePtr->TRACELog("ErrorMsg::" + RespVal["ErrorMsg"].asString(), true);

        Result = HttpResponse::newHttpJsonResponse(RespVal);
    }
    catch (...)
    {
        RespVal["Result"] = "更新失败";
        RespVal["ErrorMsg"] = "PersonalData::Search::Error";
        MyBasePtr->TRACELog("ErrorMsg::" + RespVal["ErrorMsg"].asString(), true);
        
        Result = HttpResponse::newHttpJsonResponse(RespVal);
    }

    Result->setStatusCode(k200OK);
    Result->setContentTypeCode(CT_TEXT_HTML);
    callback(Result);
}