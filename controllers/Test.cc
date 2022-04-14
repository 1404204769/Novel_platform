#include "Test.h"
//add definition of your processing function here
void Test::ChineseStr(const HttpRequestPtr &req,std::function<void (const HttpResponsePtr &)> &&callback) const
{
    Json::Value ReqVal, RespVal;
    drogon::HttpResponsePtr Result;
	auto *MyBasePtr = app().getPlugin<MyBase>();
	auto *MyToolPtr = app().getPlugin<MyTools>();
    const unordered_map<string, string> umapPara = req->getParameters();
    MyBasePtr->TRACELog("Test::ChineseStr::body" + string(req->getBody()), true);
    
    try
    {
        ReqVal=*req->getJsonObject();
    
        MyBasePtr->DEBUGLog("开始解析中文标题", true);
        pair<int,string> TitleVal = MyToolPtr->getTitleNumAndTitleStr(ReqVal["Title"].asString());
        RespVal["TitleNum"] = TitleVal.first;
        RespVal["TitleStr"] = TitleVal.second;
        MyBasePtr->DEBUGLog("中文标题解析完成", true);

        MyBasePtr->DEBUGLog("TitleNum : " + RespVal["TitleNum"].asString(), true);
        MyBasePtr->DEBUGLog("TitleStr : " + RespVal["TitleStr"].asString(), true);
        MyBasePtr->DEBUGLog("RespVal::" + RespVal.toStyledString(), true);

        Result=HttpResponse::newHttpJsonResponse(RespVal);
    }
    catch (Json::Value &RespVal)
    {
        MyBasePtr->TRACE_ERROR(RespVal["ErrorMsg"]);
        Result = HttpResponse::newHttpJsonResponse(RespVal);
    }
    catch (...)
    {
        RespVal["ErrorMsg"].append("Test::ChineseStr发生异常");
        MyBasePtr->TRACE_ERROR(RespVal["ErrorMsg"]);
        Result = HttpResponse::newHttpJsonResponse(RespVal);
    }
    
    Result->setStatusCode(k200OK);
    Result->setContentTypeCode(CT_TEXT_HTML);
    callback(Result);
}

// 修改系统输出级别
void Test::SysOutLevel(const HttpRequestPtr &req,std::function<void (const HttpResponsePtr &)> &&callback) const
{
    Json::Value ReqVal, RespVal;
    drogon::HttpResponsePtr Result;
	auto *MyBasePtr = app().getPlugin<MyBase>();
	auto *MyJsonPtr = app().getPlugin<MyJson>();
    const unordered_map<string, string> umapPara = req->getParameters();
    MyBasePtr->TRACELog("Test::SysOutLevel::body" + string(req->getBody()), true);

    try{
        ReqVal=*req->getJsonObject();

        // 检测参数
        {
            std::map<string,MyJson::ColType>ColMap;
            ColMap["Level"] = MyJson::ColType::STRING;
            MyJsonPtr->checkMemberAndTypeInMap(ReqVal,RespVal,ColMap);
        }

        MyBasePtr->TRACELog("开始改变输出模式", true);
        MyBasePtr->ChangeStatus(ReqVal["Level"].asString());
        MyBasePtr->TRACELog("改变输出模式成功", true);
        RespVal["DEBUG"] = MyBasePtr->IsStatus("DEBUG");
        RespVal["TRACE"] = MyBasePtr->IsStatus("TRACE");
        RespVal["INFO"] = MyBasePtr->IsStatus("INFO");
        RespVal["WARN"] = MyBasePtr->IsStatus("WARN");
        MyBasePtr->DEBUGLog("RespVal::" + RespVal.toStyledString(), true);

        Result=HttpResponse::newHttpJsonResponse(RespVal);
    }
    catch (Json::Value &RespVal)
    {
        MyBasePtr->TRACE_ERROR(RespVal["ErrorMsg"]);
        Result = HttpResponse::newHttpJsonResponse(RespVal);
    }
    catch (...)
    {
        RespVal["ErrorMsg"].append("Test::SysOutLevel");
        MyBasePtr->TRACE_ERROR(RespVal["ErrorMsg"]);
        Result = HttpResponse::newHttpJsonResponse(RespVal);
    }
    
    Result->setStatusCode(k200OK);
    Result->setContentTypeCode(CT_TEXT_HTML);
    callback(Result);
}


// 充值测试
void Test::Recharge(const HttpRequestPtr &req,std::function<void (const HttpResponsePtr &)> &&callback) const
{
    Json::Value ReqVal, RespVal;
    drogon::HttpResponsePtr Result;
	auto *MyBasePtr = app().getPlugin<MyBase>();
	auto *MyJsonPtr = app().getPlugin<MyJson>();
	auto *MyDBSPtr = app().getPlugin<MyDBService>();
    const unordered_map<string, string> umapPara = req->getParameters();
    MyBasePtr->TRACELog("Test::Recharge::body" + string(req->getBody()), true);

    try{
        ReqVal=*req->getJsonObject();
        ReqVal["User_ID"] = atoi(umapPara.at("User_ID").c_str());
        MyBasePtr->DEBUGLog("开始准备充值", true);
        MyBasePtr->DEBUGLog("ReqVal::" + ReqVal.toStyledString(), true);
        MyDBSPtr->Recharge(ReqVal,RespVal);
        if(!RespVal["Result"].asBool())
        {
            MyBasePtr->DEBUGLog("用户充值失败", true);
            RespVal["ErrorMsg"].append("用户充值失败");
            throw RespVal;
        }
        MyBasePtr->DEBUGLog("用户充值成功", true);
        MyBasePtr->DEBUGLog("RespVal::" + RespVal.toStyledString(), true);

        Result=HttpResponse::newHttpJsonResponse(RespVal);
    }
    catch (Json::Value &e)
    {
        RespVal["ErrorMsg"] = e["ErrorMsg"];
        MyBasePtr->TRACE_ERROR(RespVal["ErrorMsg"]);
        Result = HttpResponse::newHttpJsonResponse(RespVal);
    }
    catch (...)
    {
        RespVal["ErrorMsg"].append("Test::Recharge");
        MyBasePtr->TRACE_ERROR(RespVal["ErrorMsg"]);
        Result = HttpResponse::newHttpJsonResponse(RespVal);
    }
    
    Result->setStatusCode(k200OK);
    Result->setContentTypeCode(CT_TEXT_HTML);
    callback(Result);
}

