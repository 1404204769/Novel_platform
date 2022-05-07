#include "Admin_Examine.h"
using namespace Admin;
//add definition of your processing function here

// 管理员查看用户上传申请接口
void Examine::UploadSearchList(const HttpRequestPtr &req,std::function<void (const HttpResponsePtr &)> &&callback) const
{
    Json::Value ReqVal,RespVal,ResultData;
    drogon::HttpResponsePtr Result;
    auto MyBasePtr = app().getPlugin<MyBase>();
    auto MyJsonPtr = app().getPlugin<MyJson>();
    auto MyDBSPtr = app().getPlugin<MyDBService>();
    const unordered_map<string,string>umapPara = req->getParameters();
    MyBasePtr->TRACELog("Examine::UploadSearchList::body" + string(req->getBody()), true);
    
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

        // 设置返回格式
        ResultData["Result"] = true;
        ResultData["Message"] = "用户申请查询成功";
        ResultData["Data"]["ApplicationList"]= RespVal["UploadList"];

        Result = HttpResponse::newHttpJsonResponse(ResultData);
    }
    catch (Json::Value &RespVal)
    {
        RespVal["Result"] = false;
        MyBasePtr->TRACELog(RespVal["ErrorMsg"].asString(), true);

        // 设置返回格式
        int ErrorSize = RespVal["ErrorMsg"].size();
        ResultData["Result"] = false;
        ResultData["Message"] = RespVal["ErrorMsg"][ErrorSize - 1];

        Result = HttpResponse::newHttpJsonResponse(ResultData);
    }
    catch (const drogon::orm::DrogonDbException &e)
    {
        RespVal["Result"] = false;
        RespVal["ErrorMsg"].append(e.base().what());
        MyBasePtr->TRACE_ERROR(RespVal["ErrorMsg"]);

        // 设置返回格式
        int ErrorSize = RespVal["ErrorMsg"].size();
        ResultData["Result"] = false;
        ResultData["Message"] = RespVal["ErrorMsg"][ErrorSize - 1];

        Result = HttpResponse::newHttpJsonResponse(ResultData);
    }
    catch (...)
    {
        RespVal["Result"] = false;
        RespVal["ErrorMsg"].append("Examine::UploadSearchList::Error");
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


// 管理员查看指定申请ID的内容
void Examine::UploadSearchByID(const HttpRequestPtr &req,std::function<void (const HttpResponsePtr &)> &&callback) const
{
    Json::Value ReqVal,RespVal,ResultData;
    drogon::HttpResponsePtr Result;
    auto MyBasePtr = app().getPlugin<MyBase>();
    auto MyJsonPtr = app().getPlugin<MyJson>();
    auto MyDBSPtr = app().getPlugin<MyDBService>();
    const unordered_map<string,string>umapPara = req->getParameters();
    MyBasePtr->TRACELog("Examine::UploadSearchByID::body" + string(req->getBody()), true);
    
    try
    {
        // 读取Json数据
        ReqVal = *req->getJsonObject();
        MyBasePtr->DEBUGLog("ReqVal::" + ReqVal.toStyledString(), true);

        RespVal["简介"] = "管理员查看指定用户申请ID接口";
        MyDBSPtr->Search_Upload_By_UploadID(ReqVal,RespVal);
        if(!RespVal["Result"].asBool())throw RespVal;
        MyBasePtr->DEBUGLog("RespVal::" + RespVal.toStyledString(), true);

        // 设置返回格式
        ResultData["Result"] = true;
        ResultData["Message"] = "指定用户申请ID查询成功";
        ResultData["Data"] = RespVal["Upload_Data"];
        Result = HttpResponse::newHttpJsonResponse(ResultData);

    }
    catch (Json::Value &RespVal)
    {
        RespVal["Result"] = false;
        MyBasePtr->TRACELog(RespVal["ErrorMsg"].asString(), true);

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


// 管理员查看用户意见接口
void Examine::IdeaList(const HttpRequestPtr &req,std::function<void (const HttpResponsePtr &)> &&callback) const
{
    Json::Value ReqVal,RespVal, ResultData;
    drogon::HttpResponsePtr Result;
    auto MyBasePtr = app().getPlugin<MyBase>();
    auto MyJsonPtr = app().getPlugin<MyJson>();
    auto MyDBSPtr = app().getPlugin<MyDBService>();
    const unordered_map<string,string>umapPara = req->getParameters();
    MyBasePtr->TRACELog("Examine::IdeaList::body" + string(req->getBody()), true);
    
    try
    {
        // 读取Json数据
        ReqVal = *req->getJsonObject();
        MyBasePtr->DEBUGLog("ReqVal::" + ReqVal.toStyledString(), true);

        RespVal["简介"] = "管理员查看用户意见接口";
        MyJsonPtr->UnMapToJson(ReqVal, umapPara, "Para");

        MyDBSPtr->Admin_Search_Idea(ReqVal,RespVal);

        MyBasePtr->DEBUGLog("RespVal::" + RespVal.toStyledString(), true);


        // 设置返回格式
        ResultData["Result"] = true;
        ResultData["Message"] = "用户意见查询成功";
        ResultData["Data"]["FeedbackList"]= RespVal["Idea_List"];

        Result = HttpResponse::newHttpJsonResponse(ResultData);
    }
    catch (Json::Value &RespVal)
    {
        RespVal["Result"] = false;
        MyBasePtr->TRACELog(RespVal["ErrorMsg"].asString(), true);
        
        // 设置返回格式
        int ErrorSize = RespVal["ErrorMsg"].size();
        ResultData["Result"] = false;
        ResultData["Message"] = RespVal["ErrorMsg"][ErrorSize - 1];

        Result = HttpResponse::newHttpJsonResponse(ResultData);
    }
    catch (const drogon::orm::DrogonDbException &e)
    {
        RespVal["Result"] = false;
        RespVal["ErrorMsg"].append(e.base().what());
        MyBasePtr->TRACE_ERROR(RespVal["ErrorMsg"]);
        
        // 设置返回格式
        int ErrorSize = RespVal["ErrorMsg"].size();
        ResultData["Result"] = false;
        ResultData["Message"] = RespVal["ErrorMsg"][ErrorSize - 1];

        Result = HttpResponse::newHttpJsonResponse(ResultData);
    }
    catch (...)
    {
        RespVal["Result"] = false;
        RespVal["ErrorMsg"].append("Examine::UpList::Error");
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

// 管理员审核申请接口
void Examine::UploadExamine(const HttpRequestPtr &req,std::function<void (const HttpResponsePtr &)> &&callback) const
{
    Json::Value ReqVal,RespVal,ParaJson,ResultData;
    drogon::HttpResponsePtr Result;
    auto MyBasePtr = app().getPlugin<MyBase>();
    auto MyJsonPtr = app().getPlugin<MyJson>();
    auto MyDBSPtr = app().getPlugin<MyDBService>();
    const unordered_map<string,string>umapPara = req->getParameters();
    MyBasePtr->TRACELog("Examine::UploadExamine::body" + string(req->getBody()), true);
    

    Result=HttpResponse::newHttpJsonResponse(RespVal);
    

    try
    {
        // 读取Json数据
        ReqVal = *req->getJsonObject();
        MyBasePtr->DEBUGLog("ReqVal::" + ReqVal.toStyledString(), true);
        RespVal["简介"] = "管理员审核申请接口";
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
        if(!RespVal["Result"].asBool())throw RespVal;
        RespVal.clear();
        MyDBSPtr->Search_Upload_By_UploadID(ReqVal,RespVal);
        if(!RespVal["Result"].asBool())throw RespVal;
        // 设置返回格式
        ResultData["Result"] = true;
        ResultData["Message"] = "用户申请审核成功";
        ResultData["Data"] = RespVal["Upload_Data"];

        Result = HttpResponse::newHttpJsonResponse(ResultData);

        MyBasePtr->DEBUGLog("RespVal::" + ResultData.toStyledString(), true);
    }
    catch (Json::Value &RespVal)
    {
        RespVal["Result"] = false;
        MyBasePtr->TRACE_ERROR(RespVal["ErrorMsg"]);
        // 设置返回格式
        int ErrorSize = RespVal["ErrorMsg"].size();
        ResultData["Result"] = false;
        ResultData["Message"] = RespVal["ErrorMsg"][ErrorSize - 1];

        Result = HttpResponse::newHttpJsonResponse(ResultData);
    }
    catch (const drogon::orm::DrogonDbException &e)
    {
        RespVal["Result"] = false;
        RespVal["ErrorMsg"].append(e.base().what());
        MyBasePtr->TRACE_ERROR(RespVal["ErrorMsg"]);
        // 设置返回格式
        int ErrorSize = RespVal["ErrorMsg"].size();
        ResultData["Result"] = false;
        ResultData["Message"] = RespVal["ErrorMsg"][ErrorSize - 1];

        Result = HttpResponse::newHttpJsonResponse(ResultData);
    }
    catch (...)
    {
        RespVal["Result"] = false;
        RespVal["ErrorMsg"].append("Examine::UploadExamine::Error");
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

// 管理员审核意见接口
void Examine::ExamineIdea(const HttpRequestPtr &req,std::function<void (const HttpResponsePtr &)> &&callback) const
{
    Json::Value ReqVal,RespVal,ParaJson;
    drogon::HttpResponsePtr Result;
    auto MyBasePtr = app().getPlugin<MyBase>();
    auto MyJsonPtr = app().getPlugin<MyJson>();
    auto MyDBSPtr = app().getPlugin<MyDBService>();
    const unordered_map<string,string>umapPara = req->getParameters();
    MyBasePtr->TRACELog("Examine::ExamineIdea::body" + string(req->getBody()), true);
    

    Result=HttpResponse::newHttpJsonResponse(RespVal);
    

    try
    {
        // 读取Json数据
        ReqVal = *req->getJsonObject();
        MyBasePtr->DEBUGLog("ReqVal::" + ReqVal.toStyledString(), true);
        RespVal["简介"] = "管理员审核意见接口";
        MyJsonPtr->UnMapToJson(ReqVal, umapPara, "Para");
        // 检查数据完整性
        {
            MyBasePtr->DEBUGLog("开始检查数据完整性", true);
            std::map<string, MyJson::ColType> ColMap;
            ColMap["Idea_ID"] = MyJson::ColType::INT;
            ColMap["Examine_Result"] = MyJson::ColType::BOOL;
            ColMap["Para"] = MyJson::ColType::JSON;
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

        ExamineJson["Idea_ID"] = ReqVal["Idea_ID"].asInt();
        ExamineJson["IsManage"] = true;
        ExamineJson["Processor"] = ParaJson["Login_Status"].asString() + "("+ParaJson["User_ID"].asString()+")";
        if(ReqVal["Examine_Result"].asBool() == false)
        {
            ExamineJson["Status"] = "已拒绝";
        }else{
            ExamineJson["Status"] = "已完成";
        }
        MyBasePtr->DEBUGLog("设置ExamineJson结束", true);
        
        MyBasePtr->DEBUGLog("开始处理意见::ExamineJson::" + ExamineJson.toStyledString(), true);
        MyDBSPtr->Change_Idea_Status(ExamineJson,RespVal);
        MyBasePtr->DEBUGLog("意见处理结束", true);

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
        RespVal["ErrorMsg"].append("Examine::ExamineIdea::Error");
        MyBasePtr->TRACE_ERROR(RespVal["ErrorMsg"]);
      
        Result = HttpResponse::newHttpJsonResponse(RespVal);
    }


    Result->setStatusCode(k200OK);
    Result->setContentTypeCode(CT_TEXT_HTML);
    callback(Result);
}




