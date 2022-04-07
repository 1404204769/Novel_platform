#include "User_Resource.h"
using namespace User;
// add definition of your processing function here

// 图书上传接口
void Resource::Upload(const HttpRequestPtr &req, std::function<void(const HttpResponsePtr &)> &&callback) const
{
    Json::Value ReqVal, RespVal;
    drogon::HttpResponsePtr Result;
    auto MyJsonPtr = app().getPlugin<MyJson>();
    auto MyBasePtr = app().getPlugin<MyBase>();
    auto MyDBSPtr = app().getPlugin<MyDBService>();
    const unordered_map<string, string> umapPara = req->getParameters();
    MyBasePtr->TRACELog("Resource::Upload::body" + string(req->getBody()), true);

    try
    {
        // 读取Json数据
        ReqVal = *req->getJsonObject();
        MyBasePtr->DEBUGLog("ReqVal::" + ReqVal.toStyledString(), true);

        RespVal["简介"] = "图书上传接口";
        MyJsonPtr->UnMapToJson(ReqVal, umapPara, "Para");
        MyJsonPtr->UnMapToJson(RespVal, umapPara, "Para");

        // 检查数据完整性
        {
            MyBasePtr->DEBUGLog("开始检查数据完整性", true);
            // "Book_ID"   :   0,
            // "Content"   :   "",
            // "Book_Author":"爱潜水的乌贼",//作者
            // "Book_Name":"诡秘之主", // 书名
            // "Upload_Type":   ""
            // "Para"       :   {}
            // 创建检查列表 key是字段名 value 是字段类型
            std::map<string, MyJson::ColType> ColMap;
            ColMap["Book_ID"] = MyJson::ColType::INT;
            ColMap["Content"] = MyJson::ColType::JSON;
            ColMap["Book_Author"] = MyJson::ColType::STRING;
            ColMap["Book_Name"] = MyJson::ColType::STRING;
            ColMap["Upload_Type"] = MyJson::ColType::STRING;
            ColMap["Para"] = MyJson::ColType::JSON;
            MyJsonPtr->checkMemberAndTypeInMap(ReqVal, RespVal, ColMap);
            MyBasePtr->DEBUGLog("检查数据完整性完成", true);
        }

        // 读取UserID LoginStatus数据
        Json::Value ExamineJson;
        // "Processor_Type"     :   "",         //审核人类型(system/admin/root)
        // "Processor_ID"       :   0,          //审核人ID
        // "Examine_Result"     :   true/false, //审核结果             
        // "Upload_ID"          :   0,          //审核对象
        // "Examine_Type"       :   "",         //审核对象类型(Book/Chapter)    
        ExamineJson["Processor_Type"] = "System";
        ExamineJson["Processor_ID"] = 0;
        auto UploadType = ReqVal["Upload_Type"].asString();
        // 纯粹的新书，直接插入图书类即可
        if (UploadType == "new_book")
        {
            MyDBSPtr->Upload_Book(ReqVal, RespVal);
            if(RespVal["Result"].asBool() == true)
            {
                ExamineJson["Examine_Type"] =   "Book";
                ExamineJson["Upload_ID"]    =   RespVal["Upload_ID"].asInt();
                ExamineJson["Examine_Result"]   =   true;
                MyDBSPtr->Examine_Upload(ExamineJson, RespVal);
                if(RespVal.isMember("Result") && RespVal["Result"].isBool() && RespVal["Result"].asBool())
                {
                    // 创建图书成功，准备创建资源贴
                    // Para中获取用户ID，Resp中Book_Data获取图书数据
                    ////----待完成
                }
            }
        }
        // 已存在的书上传新的章节
        else if (UploadType == "old_book_new")
        {
            MyDBSPtr->Upload_Chapter(ReqVal, RespVal);
            if(RespVal["Result"].asBool() == true)
            {
                ExamineJson["Examine_Type"] =   "Chapter";
                ExamineJson["Upload_ID"]    =   RespVal["Upload_ID"].asInt();
                ExamineJson["Examine_Result"]   =   true;
                MyDBSPtr->Examine_Upload(ExamineJson, RespVal);
            }
        }
        // 已存在的书更正章节
        else if (UploadType == "old_book_old")
        {
            MyDBSPtr->Upload_Chapter(ReqVal, RespVal);
            if(RespVal["Result"].asBool() == true)
            {
                ExamineJson["Examine_Type"] =   "Chapter_Update";
                ExamineJson["Upload_ID"]    =   RespVal["Upload_ID"].asInt();
                ExamineJson["Examine_Result"]   =   true;
                MyDBSPtr->Auto_Examine_Chapter_Update(ExamineJson, RespVal);
                
            }
        }
        else
        {
            RespVal["ErrorMsg"] = "Upload_Type无效";
            throw RespVal;
        }

        MyBasePtr->DEBUGLog("RespVal::" + RespVal.toStyledString(), true);

        Result = HttpResponse::newHttpJsonResponse(RespVal);
    }
    catch (Json::Value &RespVal)
    {
        MyBasePtr->TRACELog("ErrorMsg::" + RespVal["ErrorMsg"].asString(), true);

        Result = HttpResponse::newHttpJsonResponse(RespVal);
    }
    catch (const drogon::orm::DrogonDbException &e)
    {
        RespVal["ErrorMsg"] = e.base().what();
        MyBasePtr->TRACELog("ErrorMsg::" + RespVal["ErrorMsg"].asString(), true);

        Result = HttpResponse::newHttpJsonResponse(RespVal);
    }
    catch (...)
    {
        RespVal["ErrorMsg"] = "Resource::Upload::Error";
        MyBasePtr->TRACELog("ErrorMsg::" + RespVal["ErrorMsg"].asString(), true);

        Result = HttpResponse::newHttpJsonResponse(RespVal);
    }

    Result->setStatusCode(k200OK);
    Result->setContentTypeCode(CT_TEXT_HTML);
    callback(Result);
}

// 图书下载接口
void Resource::Download(const HttpRequestPtr &req, std::function<void(const HttpResponsePtr &)> &&callback) const
{
    Json::Value ReqVal, RespVal;
    drogon::HttpResponsePtr Result;
    auto MyJsonPtr = app().getPlugin<MyJson>();
    auto MyBasePtr = app().getPlugin<MyBase>();
    auto MyDBSPtr = app().getPlugin<MyDBService>();
    const unordered_map<string, string> umapPara = req->getParameters();
    MyBasePtr->TRACELog("Resource::Download::body" + string(req->getBody()), true);

    try
    {
        // 读取Json数据
        ReqVal = *req->getJsonObject();
        MyBasePtr->DEBUGLog("ReqVal::" + ReqVal.toStyledString(), true);


        RespVal["简介"] = "图书下载接口";
        MyJsonPtr->UnMapToJson(ReqVal, umapPara, "Para");
        MyJsonPtr->UnMapToJson(RespVal, umapPara, "Para");

        // 检查数据完整性
        {
            MyBasePtr->DEBUGLog("开始检查数据完整性", true);
            // "Book_ID":0,
            // "Chapter_Num":[],
            // 创建检查列表 key是字段名 value 是字段类型
            std::map<string, MyJson::ColType> ColMap;
            ColMap["Book_ID"] = MyJson::ColType::INT;
            ColMap["Chapter_Num"] = MyJson::ColType::ARRAY;
            MyJsonPtr->checkMemberAndTypeInMap(ReqVal, RespVal, ColMap);
            MyBasePtr->DEBUGLog("检查数据完整性完成", true);
        }

        // 读取UserID LoginStatus数据

        MyDBSPtr->Download_Resource(ReqVal, RespVal);
        MyBasePtr->DEBUGLog("RespVal::" + RespVal.toStyledString(), true);

        Result = HttpResponse::newHttpJsonResponse(RespVal);
    }
    catch (Json::Value &RespVal)
    {
        MyBasePtr->TRACELog("ErrorMsg::" + RespVal["ErrorMsg"].asString(), true);

        Result = HttpResponse::newHttpJsonResponse(RespVal);
    }
    catch (const drogon::orm::DrogonDbException &e)
    {
        RespVal["ErrorMsg"] = e.base().what();
        MyBasePtr->TRACELog("ErrorMsg::" + RespVal["ErrorMsg"].asString(), true);

        Result = HttpResponse::newHttpJsonResponse(RespVal);
    }
    catch (...)
    {
        RespVal["ErrorMsg"] = "Resource::Download::Error";
        MyBasePtr->TRACELog("ErrorMsg::" + RespVal["ErrorMsg"].asString(), true);

        Result = HttpResponse::newHttpJsonResponse(RespVal);
    }

    Result->setStatusCode(k200OK);
    Result->setContentTypeCode(CT_TEXT_HTML);
    callback(Result);
}

// 图书查找接口
void Resource::Search(const HttpRequestPtr &req, std::function<void(const HttpResponsePtr &)> &&callback) const
{
    Json::Value ReqVal, RespVal;
    drogon::HttpResponsePtr Result;
    auto MyBasePtr = app().getPlugin<MyBase>();
    auto MyJsonPtr = app().getPlugin<MyJson>();
    const unordered_map<string, string> umapPara = req->getParameters();
    MyBasePtr->TRACELog("Resource::Search::body" + string(req->getBody()), true);
    
    try
    {
        // 读取Json数据
        ReqVal=*req->getJsonObject();
        MyBasePtr->DEBUGLog("ReqVal::" + ReqVal.toStyledString(), true);

        RespVal["简介"] = "图书查找接口";
        MyJsonPtr->UnMapToJson(ReqVal, umapPara, "Para");
        MyJsonPtr->UnMapToJson(RespVal, umapPara, "Para");

        MyBasePtr->DEBUGLog("RespVal::" + RespVal.toStyledString(), true);

        Result=HttpResponse::newHttpJsonResponse(RespVal);
    }
    catch (Json::Value RespVal)
    {
        MyBasePtr->TRACELog("ErrorMsg::" + RespVal["ErrorMsg"].asString(), true);
        
        Result = HttpResponse::newHttpJsonResponse(RespVal);
    }
    catch (...)
    {
        RespVal["ErrorMsg"] = "Resource::Search::Error";
        MyBasePtr->TRACELog("ErrorMsg::" + RespVal["ErrorMsg"].asString(), true);

        Result = HttpResponse::newHttpJsonResponse(RespVal);
    }

    Result->setStatusCode(k200OK);
    Result->setContentTypeCode(CT_TEXT_HTML);
    callback(Result);
}