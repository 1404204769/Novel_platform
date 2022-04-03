#include "User_Resource.h"
using namespace User;
//add definition of your processing function here

// 图书上传接口
void Resource::Upload(const HttpRequestPtr &req,std::function<void (const HttpResponsePtr &)> &&callback) const
{
    auto MyJsonPtr = app().getPlugin<MyJson>();
    auto MyBasePtr = app().getPlugin<MyBase>();
    auto MyDBSPtr = app().getPlugin<MyDBService>();
    drogon::HttpResponsePtr result;
    const unordered_map<string,string>parameters = req->getParameters();
    Json::Value RespVal;
    
    MyJsonPtr->UnMapToJson(RespVal, parameters, "parameters");
    RespVal["简介"] = "图书上传接口";
    MyBasePtr->DEBUGLog(RespVal.toStyledString(), true);
    auto ReqJson=req->getJsonObject();
    try
    {
        // 读取Json数据
        Json::Value ReqJsonValue=*ReqJson;
        MyBasePtr->DEBUGLog(ReqJsonValue.toStyledString(), true);

        // 检查数据完整性
        {    
            MyBasePtr->DEBUGLog("开始检查数据完整性", true);
            // "Book_ID"   :   0,
            // "Content"   :   "",
            // "Book_Author":"爱潜水的乌贼",//作者
            // "Book_Name":"诡秘之主", // 书名
            // "UploadType":   ""
            // 创建检查列表 key是字段名 value 是字段类型
            std::map<string,MyJson::ColType>ColMap;
            ColMap["Book_ID"]=MyJson::ColType::INT;
            ColMap["Content"]=MyJson::ColType::JSON;
            ColMap["Book_Author"]=MyJson::ColType::STRING;
            ColMap["Book_Name"]=MyJson::ColType::STRING;
            ColMap["UploadType"]=MyJson::ColType::STRING;
            MyJsonPtr->checkMemberAndTypeInMap(ReqJsonValue,RespVal,ColMap);
            MyBasePtr->DEBUGLog("检查数据完整性完成", true);
        }

        // 读取UserID LoginStatus数据
        ReqJsonValue["UserID"] = std::atoi(RespVal["parameters"]["UserID"].asString().c_str());
        auto UploadType=ReqJsonValue["UploadType"].asString();

        // 纯粹的新书，直接插入图书类即可
        if(UploadType == "new_book"){
            MyDBSPtr->User_Upload_New_Book(ReqJsonValue,RespVal);
        }
        // 已存在的书上传新的章节
        else if(UploadType == "old_book_new"){
            MyDBSPtr->User_Upload_Exist_Book_New_Chapter(ReqJsonValue,RespVal);
        }
        // 已存在的书更正章节
        else if(UploadType == "old_book_old"){
            MyDBSPtr->User_Upload_Exist_Book_Update_Chapter(ReqJsonValue,RespVal);
        }
        else{
            RespVal["ErrorMsg"] = "UploadType无效";
            throw RespVal;
        }
        result=HttpResponse::newHttpJsonResponse(RespVal);
    }
    catch(Json::Value e)
    {
        MyBasePtr->TRACELog(e["ErrorMsg"].asString(),true);
        result=HttpResponse::newHttpJsonResponse(e);
    }
    catch(const drogon::orm::DrogonDbException &e)
    {
        RespVal["ErrorMsg"] = e.base().what();
        RespVal["DrogonDbException"] = e.base().what();
        MyBasePtr->TRACELog(RespVal["ErrorMsg"].asString(),true);
        result=HttpResponse::newHttpJsonResponse(RespVal);
    }
    catch(...)
    {
        RespVal["ErrorMsg"] = "Resource::Upload::Error";
        MyBasePtr->TRACELog(RespVal["ErrorMsg"].asString(),true);
        result=HttpResponse::newHttpJsonResponse(RespVal);
    }
    
    result->setStatusCode(k200OK);
    result->setContentTypeCode(CT_TEXT_HTML);
    callback(result);
}

// 图书下载接口
void Resource::Download(const HttpRequestPtr &req,std::function<void (const HttpResponsePtr &)> &&callback) const
{
    
    auto MyJsonPtr = app().getPlugin<MyJson>();
    auto MyBasePtr = app().getPlugin<MyBase>();
    auto MyDBSPtr = app().getPlugin<MyDBService>();
    drogon::HttpResponsePtr result;
    const unordered_map<string,string>parameters = req->getParameters();
    Json::Value RespVal;
    
    MyJsonPtr->UnMapToJson(RespVal, parameters, "parameters");
    RespVal["简介"] = "图书下载接口";
    MyBasePtr->DEBUGLog(RespVal.toStyledString(), true);
    auto ReqJson=req->getJsonObject();
    try
    {
        // 读取Json数据
        Json::Value ReqJsonValue=*ReqJson;
        MyBasePtr->DEBUGLog(ReqJsonValue.toStyledString(), true);

        // 检查数据完整性
        {    
            MyBasePtr->DEBUGLog("开始检查数据完整性", true);
            // "Book_ID":0,
            // "Chapter_ID":[],
            // 创建检查列表 key是字段名 value 是字段类型
            std::map<string,MyJson::ColType>ColMap;
            ColMap["Book_ID"]=MyJson::ColType::INT;
            ColMap["Chapter_ID"]=MyJson::ColType::ARRAY;
            MyJsonPtr->checkMemberAndTypeInMap(ReqJsonValue,RespVal,ColMap);
            MyBasePtr->DEBUGLog("检查数据完整性完成", true);
        }

        // 读取UserID LoginStatus数据
        ReqJsonValue["UserID"] = std::atoi(RespVal["parameters"]["UserID"].asString().c_str());

        MyDBSPtr->User_Download(ReqJsonValue,RespVal);
        result=HttpResponse::newHttpJsonResponse(RespVal);
    }
    catch(Json::Value e)
    {
        MyBasePtr->TRACELog(e["ErrorMsg"].asString(),true);
        result=HttpResponse::newHttpJsonResponse(e);
    }
    catch(const drogon::orm::DrogonDbException &e)
    {
        RespVal["ErrorMsg"] = e.base().what();
        RespVal["DrogonDbException"] = e.base().what();
        MyBasePtr->TRACELog(RespVal["ErrorMsg"].asString(),true);
        result=HttpResponse::newHttpJsonResponse(RespVal);
    }
    catch(...)
    {
        RespVal["ErrorMsg"] = "Resource::Download::Error";
        MyBasePtr->TRACELog(RespVal["ErrorMsg"].asString(),true);
        result=HttpResponse::newHttpJsonResponse(RespVal);
    }
    
    result->setStatusCode(k200OK);
    result->setContentTypeCode(CT_TEXT_HTML);
    callback(result);
}

// 图书查找接口
void Resource::Search(const HttpRequestPtr &req,std::function<void (const HttpResponsePtr &)> &&callback) const
{
    auto MyJsonPtr = app().getPlugin<MyJson>();
    const unordered_map<string,string>parameters = req->getParameters();
    Json::Value RespVal;
    MyJsonPtr->UnMapToJson(RespVal, parameters, "parameters");
    RespVal["简介"] = "图书查找接口";
    cout << RespVal.toStyledString() << endl;
    auto resp=HttpResponse::newHttpJsonResponse(RespVal);
    resp->setStatusCode(k200OK);
    resp->setContentTypeCode(CT_TEXT_HTML);
    callback(resp);
}