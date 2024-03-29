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
    catch (Json::Value &RespVal)
    {
        RespVal["Result"] = "查询所有用户失败";
        MyBasePtr->TRACE_ERROR(RespVal["ErrorMsg"]);
        Result = HttpResponse::newHttpJsonResponse(RespVal);
    }
    catch (const drogon::orm::DrogonDbException &e)
    {
        RespVal["Result"] = "查询所有用户失败";
        RespVal["ErrorMsg"].append(e.base().what());
        MyBasePtr->TRACE_ERROR(RespVal["ErrorMsg"]);
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
    catch (Json::Value &RespVal)
    {
        RespVal["Result"] = "查询指定用户失败";
        MyBasePtr->TRACE_ERROR(RespVal["ErrorMsg"]);
        Result = HttpResponse::newHttpJsonResponse(RespVal);
    }
    catch (const drogon::orm::DrogonDbException &e)
    {
        RespVal["Result"] = "查询指定用户失败";
        RespVal["ErrorMsg"].append(e.base().what());
        MyBasePtr->TRACE_ERROR(RespVal["ErrorMsg"]);
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
    auto MyDBSPtr = app().getPlugin<MyDBService>();
    const unordered_map<string, string> umapPara = req->getParameters();
    MyBasePtr->TRACELog("Admin::User::Update::body" + string(req->getBody()), true);
    
    try
    {
        // 读取Json数据
        ReqVal = *req->getJsonObject();
        MyBasePtr->DEBUGLog("ReqVal::" + ReqVal.toStyledString(), true);

        RespVal["简介"] = "管理员更改用户数据接口";
        MyJsonPtr->UnMapToJson(ReqVal, umapPara, "Para");
        MyJsonPtr->UnMapToJson(RespVal, umapPara, "Para");

        // 开始检查传入参数
        {
            MyBasePtr->DEBUGLog("开始检查传入参数是否合法", true);
            // "Change_Target"  :   [{},{}],被执行者们
            // "Para"           :   {"User_ID":"","Login_Status":""} 执行者
            std::map<string, MyJson::ColType> ColMap;
            ColMap["Para"] = MyJson::ColType::JSON;
            ColMap["Change_Target"] = MyJson::ColType::ARRAY;
            MyJsonPtr->checkMemberAndTypeInMap(ReqVal, RespVal, ColMap);
            MyBasePtr->DEBUGLog("传入参数合法", true);
        }
        
        MyDBSPtr->Admin_Update_User(ReqVal, RespVal);
        RespVal["Result"] = "更新用户数据成功";
        MyBasePtr->DEBUGLog("RespVal::" + RespVal.toStyledString(), true);

        Result = HttpResponse::newHttpJsonResponse(RespVal);
    }
    catch (Json::Value &RespVal)
    {
        RespVal["Result"] = "更新用户数据失败";
        MyBasePtr->TRACE_ERROR(RespVal["ErrorMsg"]);
        Result = HttpResponse::newHttpJsonResponse(RespVal);
    }
    catch (...)
    {
        RespVal["Result"] = "更新用户数据失败";
        RespVal["ErrorMsg"].append("Admin::User::Update::Error");
        MyBasePtr->TRACE_ERROR(RespVal["ErrorMsg"]);
        Result = HttpResponse::newHttpJsonResponse(RespVal);
    }

    Result->setStatusCode(k200OK);
    Result->setContentTypeCode(CT_TEXT_HTML);
    callback(Result);
}


void Admin::User::Change_User_Integral(const HttpRequestPtr &req, std::function<void(const HttpResponsePtr &)> &&callback) const
{
    Json::Value ReqVal, RespVal, ParaVal;
    drogon::HttpResponsePtr Result;
    auto MyBasePtr = app().getPlugin<MyBase>();
    auto MyJsonPtr = app().getPlugin<MyJson>();
    auto MyDBSPtr = app().getPlugin<MyDBService>();
    const unordered_map<string, string> umapPara = req->getParameters();
    MyBasePtr->TRACELog("Admin::User::Change_User_Integral::body" + string(req->getBody()), true);
    
    try
    {
        // 读取Json数据
        ReqVal = *req->getJsonObject();
        MyBasePtr->DEBUGLog("ReqVal::" + ReqVal.toStyledString(), true);

        RespVal["简介"] = "管理员更改用户积分数据接口";
        MyJsonPtr->UnMapToJson(ParaVal, umapPara, "Para");

        // 开始检查传入参数
        {
            MyBasePtr->DEBUGLog("开始检查传入参数是否合法", true);
            // "Change_ID" : 0,
            // "Change_Num" : 0,
            // "Change_Type" : "Add/Sub",
            // "Change_Explain" : ""
            std::map<string, MyJson::ColType> ColMap;
            ColMap["Change_ID"] = MyJson::ColType::INT;
            ColMap["Change_Num"] = MyJson::ColType::INT;
            ColMap["Change_Type"] = MyJson::ColType::STRING;
            ColMap["Change_Explain"] = MyJson::ColType::STRING;
            MyJsonPtr->checkMemberAndTypeInMap(ReqVal, RespVal, ColMap);
            MyBasePtr->DEBUGLog("传入参数合法", true);

            
            MyBasePtr->DEBUGLog("开始检查Para数据是否合法", true);
            ColMap.clear();
            ColMap["User_ID"] = MyJson::ColType::STRING;
            ColMap["Login_Status"] = MyJson::ColType::STRING;
            MyJsonPtr->checkMemberAndTypeInMap(ParaVal["Para"], RespVal, ColMap);
            MyBasePtr->DEBUGLog("Para数据合法", true);
        }

        ReqVal["User_ID"] = atoi(ParaVal["Para"]["User_ID"].asString().c_str());
        ReqVal["Processor"] = ParaVal["Para"]["Login_Status"].asString();
        if(!MyDBSPtr->Change_User_Integral(ReqVal,RespVal))
        {
            throw RespVal;
        }
        
        RespVal["Result"] = "更新用户数据成功";
        MyBasePtr->DEBUGLog("RespVal::" + RespVal.toStyledString(), true);

        Result = HttpResponse::newHttpJsonResponse(RespVal);
    }
    catch (Json::Value &RespVal)
    {
        RespVal["Result"] = "更新用户数据失败";
        MyBasePtr->TRACE_ERROR(RespVal["ErrorMsg"]);
        Result = HttpResponse::newHttpJsonResponse(RespVal);
    }
    catch (...)
    {
        RespVal["Result"] = "更新用户数据失败";
        RespVal["ErrorMsg"].append("Admin::User::Change_User_Integral::Error");
        MyBasePtr->TRACE_ERROR(RespVal["ErrorMsg"]);
        Result = HttpResponse::newHttpJsonResponse(RespVal);
    }

    Result->setStatusCode(k200OK);
    Result->setContentTypeCode(CT_TEXT_HTML);
    callback(Result);
}


void Admin::User::Change_User_Status(const HttpRequestPtr &req, std::function<void(const HttpResponsePtr &)> &&callback) const
{
    Json::Value ReqVal, RespVal, ParaVal;
    drogon::HttpResponsePtr Result;
    auto MyBasePtr = app().getPlugin<MyBase>();
    auto MyJsonPtr = app().getPlugin<MyJson>();
    auto MyDBSPtr = app().getPlugin<MyDBService>();
    const unordered_map<string, string> umapPara = req->getParameters();
    MyBasePtr->TRACELog("Admin::User::Change_User_Status::body" + string(req->getBody()), true);
    
    try
    {
        // 读取Json数据
        ReqVal = *req->getJsonObject();
        MyBasePtr->DEBUGLog("ReqVal::" + ReqVal.toStyledString(), true);

        RespVal["简介"] = "管理员更改用户状态数据接口";
        MyJsonPtr->UnMapToJson(ParaVal, umapPara, "Para");

        // 开始检查传入参数
        {
            MyBasePtr->DEBUGLog("开始检查传入参数是否合法", true);
            // "Change_ID" : 0,
            // "Limit_Time" : 0,//封号的时候使用
            // "Change_Type" : "Ban/Unseal",
            // "Change_Explain" : ""

            std::map<string, MyJson::ColType> ColMap;
            ColMap["Change_ID"] = MyJson::ColType::INT;
            ColMap["Change_Type"] = MyJson::ColType::STRING;
            ColMap["Change_Explain"] = MyJson::ColType::STRING;
            MyJsonPtr->checkMemberAndTypeInMap(ReqVal, RespVal, ColMap);
            MyBasePtr->DEBUGLog("传入参数合法", true);

            
            MyBasePtr->DEBUGLog("开始检查Para数据是否合法", true);
            ColMap.clear();
            ColMap["User_ID"] = MyJson::ColType::STRING;
            ColMap["Login_Status"] = MyJson::ColType::STRING;
            MyJsonPtr->checkMemberAndTypeInMap(ParaVal["Para"], RespVal, ColMap);
            MyBasePtr->DEBUGLog("Para数据合法", true);
        }

        ReqVal["User_ID"] = atoi(ParaVal["Para"]["User_ID"].asString().c_str());
        ReqVal["Processor"] = ParaVal["Para"]["Login_Status"].asString();
        if(!MyDBSPtr->Change_User_Status(ReqVal,RespVal))
        {
            throw RespVal;
        }
        
        RespVal["Result"] = "更新用户数据成功";
        MyBasePtr->DEBUGLog("RespVal::" + RespVal.toStyledString(), true);

        Result = HttpResponse::newHttpJsonResponse(RespVal);
    }
    catch (Json::Value &RespVal)
    {
        RespVal["Result"] = "更新用户数据失败";
        MyBasePtr->TRACE_ERROR(RespVal["ErrorMsg"]);
        Result = HttpResponse::newHttpJsonResponse(RespVal);
    }
    catch (...)
    {
        RespVal["Result"] = "更新用户数据失败";
        RespVal["ErrorMsg"].append("Admin::User::Change_User_Status::Error");
        MyBasePtr->TRACE_ERROR(RespVal["ErrorMsg"]);
        Result = HttpResponse::newHttpJsonResponse(RespVal);
    }

    Result->setStatusCode(k200OK);
    Result->setContentTypeCode(CT_TEXT_HTML);
    callback(Result);
}
