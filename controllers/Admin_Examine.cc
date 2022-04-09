#include "Admin_Examine.h"
using namespace Admin;
//add definition of your processing function here

// 管理员查看用户上传申请接口
void Examine::UpList(const HttpRequestPtr &req,std::function<void (const HttpResponsePtr &)> &&callback) const
{
    Json::Value ReqVal,RespVal;
    drogon::HttpResponsePtr Result;
    auto MyBasePtr = app().getPlugin<MyBase>();
    auto MyJsonPtr = app().getPlugin<MyJson>();
    auto MyDBSPtr = app().getPlugin<MyDBService>();
    const unordered_map<string,string>umapPara = req->getParameters();
    MyBasePtr->TRACELog("Examine::UpList::body" + string(req->getBody()), true);
    
    try
    {
        // 读取Json数据
        ReqVal = *req->getJsonObject();
        MyBasePtr->DEBUGLog("ReqVal::" + ReqVal.toStyledString(), true);

        RespVal["简介"] = "管理员查看用户上传申请接口";
        MyJsonPtr->UnMapToJson(ReqVal, umapPara, "Para");
        MyJsonPtr->UnMapToJson(RespVal, umapPara, "Para");

        MyDBSPtr->Admin_Search_Upload(ReqVal,RespVal);
        RespVal["Result"] = true;

        
        MyBasePtr->DEBUGLog("RespVal::" + RespVal.toStyledString(), true);

        Result = HttpResponse::newHttpJsonResponse(RespVal);
    }
    catch (Json::Value &RespVal)
    {
        RespVal["Result"] = false;
        MyBasePtr->TRACELog(RespVal["ErrorMsg"].asString(), true);
        Result = HttpResponse::newHttpJsonResponse(RespVal);
    }
    catch (const drogon::orm::DrogonDbException &e)
    {
        RespVal["Result"] = false;
        RespVal["ErrorMsg"].append(e.base().what());
        MyBasePtr->TRACE_ERROR(RespVal["ErrorMsg"]);

        Result = HttpResponse::newHttpJsonResponse(RespVal);
    }
    catch (...)
    {
        RespVal["Result"] = false;
        RespVal["ErrorMsg"].append("Examine::UpList::Error");
        MyBasePtr->TRACE_ERROR(RespVal["ErrorMsg"]);

        Result = HttpResponse::newHttpJsonResponse(RespVal);
    }
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

// 管理员审核接口
void Examine::Resolve(const HttpRequestPtr &req,std::function<void (const HttpResponsePtr &)> &&callback) const
{
    Json::Value ReqVal,RespVal,ParaJson;
    drogon::HttpResponsePtr Result;
    auto MyBasePtr = app().getPlugin<MyBase>();
    auto MyJsonPtr = app().getPlugin<MyJson>();
    auto MyDBSPtr = app().getPlugin<MyDBService>();
    const unordered_map<string,string>umapPara = req->getParameters();
    MyBasePtr->TRACELog("Examine::Resolve::body" + string(req->getBody()), true);
    

    Result=HttpResponse::newHttpJsonResponse(RespVal);
    

    try
    {
        // 读取Json数据
        ReqVal = *req->getJsonObject();
        MyBasePtr->DEBUGLog("ReqVal::" + ReqVal.toStyledString(), true);
        RespVal["简介"] = "管理员审核接口";
        MyJsonPtr->UnMapToJson(ReqVal, umapPara, "Para");
        MyJsonPtr->UnMapToJson(RespVal, umapPara, "Para");
        // 检查数据完整性
        {
            MyBasePtr->DEBUGLog("开始检查数据完整性", true);
            std::map<string, MyJson::ColType> ColMap;
            ColMap["Upload_ID"] = MyJson::ColType::INT;
            ColMap["Examine_Result"] = MyJson::ColType::BOOL;
            MyJsonPtr->checkMemberAndTypeInMap(ReqVal, RespVal, ColMap);
            
            if(ReqVal["Examine_Result"].asBool() == false)
            {
                ColMap.clear();
                ColMap["Examine_Explain"] = MyJson::ColType::STRING;
                MyJsonPtr->checkMemberAndTypeInMap(ReqVal, RespVal, ColMap);
            }
            MyBasePtr->DEBUGLog("检查数据完整性完成", true);
            
            MyBasePtr->DEBUGLog("开始检查Para数据", true);
            ColMap.clear();
            ParaJson = ReqVal["Para"];
            ColMap["User_ID"] = MyJson::ColType::STRING;
            ColMap["Login_Status"] = MyJson::ColType::STRING;
            MyJsonPtr->checkMemberAndTypeInMap(ParaJson, RespVal, ColMap);
            MyBasePtr->DEBUGLog("检查Para数据完成", true);


            if ((ParaJson["Login_Status"].asString() != "admin") && (ParaJson["Login_Status"].asString() != "root"))
            {
                RespVal["ErrorMsg"].append("账户权限错误,请联系管理员");
                RespVal["Result"]   = false;
                throw RespVal;
            }
        }

        
        MyBasePtr->DEBUGLog("开始设置ExamineJson", true);
        Json::Value ExamineJson;
        // "Processor_Type"     :   "",         //审核人类型(system/admin/root)
        // "Processor_ID"       :   0,          //审核人ID
        // "Examine_Result"     :   true/false, //审核结果             
        // "Upload_ID"          :   0,          //审核对象
        // "Examine_Type"       :   "",         //审核对象类型(Book/Chapter)    
        ExamineJson["Processor_Type"] = ParaJson["Login_Status"].asString();
        ExamineJson["Processor_ID"] = std::atoi(ParaJson["User_ID"].asString().c_str());
        ExamineJson["Examine_Type"] =  MyDBSPtr->Get_Upload_Type(ReqVal["Upload_ID"].asInt());
        ExamineJson["Upload_ID"]    =   ReqVal["Upload_ID"].asInt();
        ExamineJson["Examine_Result"]   =   ReqVal["Examine_Result"].asBool();
        ExamineJson["Examine_Explain"]  =   ReqVal["Examine_Explain"].asString();
        MyBasePtr->DEBUGLog("设置ExamineJson结束", true);
        
        MyBasePtr->DEBUGLog("开始处理申请::ExamineJson::" + ExamineJson.toStyledString(), true);
        MyDBSPtr->Examine_Upload(ExamineJson,RespVal);
        MyBasePtr->DEBUGLog("申请处理结束", true);

        MyBasePtr->DEBUGLog("RespVal::" + RespVal.toStyledString(), true);

        Result = HttpResponse::newHttpJsonResponse(RespVal);
    }
    catch (Json::Value &RespVal)
    {
        RespVal["Result"] = false;
        MyBasePtr->TRACE_ERROR(RespVal["ErrorMsg"]);
        Result = HttpResponse::newHttpJsonResponse(RespVal);
    }
    catch (const drogon::orm::DrogonDbException &e)
    {
        RespVal["Result"] = false;
        RespVal["ErrorMsg"].append(e.base().what());
        MyBasePtr->TRACE_ERROR(RespVal["ErrorMsg"]);
        Result = HttpResponse::newHttpJsonResponse(RespVal);
    }
    catch (...)
    {
        RespVal["Result"] = false;
        RespVal["ErrorMsg"].append("Examine::Resolve::Error");
        MyBasePtr->TRACE_ERROR(RespVal["ErrorMsg"]);
      
        Result = HttpResponse::newHttpJsonResponse(RespVal);
    }


    Result->setStatusCode(k200OK);
    Result->setContentTypeCode(CT_TEXT_HTML);
    callback(Result);
}