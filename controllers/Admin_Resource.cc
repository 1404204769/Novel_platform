#include "Admin_Resource.h"
using namespace Admin;
//add definition of your processing function here


// 管理员资源查询接口
void Resource::Search(const HttpRequestPtr &req,std::function<void (const HttpResponsePtr &)> &&callback) const
{
    Json::Value ReqVal, RespVal,ResultData;
    drogon::HttpResponsePtr Result;
    auto MyBasePtr = app().getPlugin<MyBase>();
    auto MyJsonPtr = app().getPlugin<MyJson>();
    auto MyDBS = app().getPlugin<MyDBService>();
    const unordered_map<string, string> umapPara = req->getParameters();
    MyBasePtr->TRACELog("Resource::Search::body" + string(req->getBody()), true);
    
    try
    {
        // 读取Json数据
        ReqVal=*req->getJsonObject();
        // "Note_KeyWord" : "",// 关键字,在标题和内容中查找
            // 检查ReqJson数据是否合法

        MyBasePtr->DEBUGLog("开始检查ReqJson数据是否合法", true);
        std::map<string, MyJson::ColType> ColMap;
        ColMap["SearchType"] = MyJson::ColType::STRING;
        MyJsonPtr->checkMemberAndTypeInMap( ReqVal, RespVal, ColMap);
        MyBasePtr->DEBUGLog("ReqJson数据合法", true);
        string Type = ReqVal["SearchType"].asString();
        if(Type == "Book")
        {
            MyDBS->Search_Book(ReqVal,RespVal);
            RespVal["简介"] = "图书查找接口";

            MyBasePtr->DEBUGLog("RespVal::" + RespVal.toStyledString(), true);

            // 设置返回格式
            ResultData["Message"] = "图书查询成功";
            ResultData["Data"]["Book_List"]= RespVal["Book_List"];
        }
        else if(Type == "Chapter")
        {
            MyDBS->Search_Chapter_By_BookID(ReqVal,RespVal);
            if(RespVal["Result"] == false)
                throw RespVal;
            RespVal["简介"] = "图书所有章节查找接口";

            MyBasePtr->DEBUGLog("RespVal::" + RespVal.toStyledString(), true);

            // 设置返回格式
            ResultData["Message"] = "图书所有章节查询成功";
            ResultData["Data"]["Chapter_List"]= RespVal["Chapter_List"];

        }
        else if(Type == "ChapterVersion")
        {
            MyDBS->Search_Chapter_All_Version(ReqVal,RespVal);
            if(RespVal["Result"] == false)
                throw RespVal;
            RespVal["简介"] = "指定图书章节的所有版本查找接口";

            MyBasePtr->DEBUGLog("RespVal::" + RespVal.toStyledString(), true);

            // 设置返回格式
            ResultData["Message"] = "指定图书章节的所有版本查询成功";
            ResultData["Data"]["Chapter_List"]= RespVal["Chapter_List"];
        }
        else
        {
            RespVal["ErrorMsg"].append("SearchType字段内容异常");
            throw RespVal;
        }
        ResultData["Result"] = true;
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
        RespVal["ErrorMsg"].append("Resource::Search::Error");
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

// 管理员资源上传接口
void Resource::Upload(const HttpRequestPtr &req,std::function<void (const HttpResponsePtr &)> &&callback) const
{
    auto MyBasePtr = app().getPlugin<MyBase>();
    MyBasePtr->TRACELog("原始地址为：" + req->getPath(),true);
    MyBasePtr->TRACELog("准备开始重定向",true);
    req->setPath("/User/Resource/Upload");
    app().forward(req,move(callback));
    MyBasePtr->TRACELog("重定向完成",true);
   
}

// 管理员资源管理接口
void Resource::Update(const HttpRequestPtr &req,std::function<void (const HttpResponsePtr &)> &&callback) const
{
    Json::Value ReqVal, RespVal;
    drogon::HttpResponsePtr Result;
    auto MyBasePtr = app().getPlugin<MyBase>();
    auto MyJsonPtr = app().getPlugin<MyJson>();
    auto MyDBSPtr = app().getPlugin<MyDBService>();
    const unordered_map<string, string> umapPara = req->getParameters();
    MyBasePtr->TRACELog("Resource::Upload::body" + string(req->getBody()), true);
    

    Result = HttpResponse::newHttpJsonResponse(RespVal);

    try
    {
        // 读取Json数据
        ReqVal = *req->getJsonObject();
        MyBasePtr->DEBUGLog("ReqVal::" + ReqVal.toStyledString(), true);

        RespVal["简介"] = "管理员资源管理接口";
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
        
        MyDBSPtr->Admin_Update_Resource(ReqVal, RespVal);
        RespVal["Result"] = "更新图书资源数据成功";
        MyBasePtr->DEBUGLog("RespVal::" + RespVal.toStyledString(), true);

        Result = HttpResponse::newHttpJsonResponse(RespVal);
    }
    catch (Json::Value &RespVal)
    {
        RespVal["Result"] = "更新图书资源数据失败";
        MyBasePtr->TRACE_ERROR(RespVal["ErrorMsg"]);
        Result = HttpResponse::newHttpJsonResponse(RespVal);
    }
    catch (...)
    {
        RespVal["Result"] = "更新图书资源数据失败";
        RespVal["ErrorMsg"].append("Admin::Resource::Update::Error");
        MyBasePtr->TRACE_ERROR(RespVal["ErrorMsg"]);
        Result = HttpResponse::newHttpJsonResponse(RespVal);
    }

    Result->setStatusCode(k200OK);
    Result->setContentTypeCode(CT_TEXT_HTML);
    callback(Result);
}

// 管理员修改资源状态
void Resource::UBS(const HttpRequestPtr &req,std::function<void (const HttpResponsePtr &)> &&callback) const
{
    Json::Value ReqVal, RespVal,ResultData;
    drogon::HttpResponsePtr Result;
    auto MyBasePtr = app().getPlugin<MyBase>();
    auto MyJsonPtr = app().getPlugin<MyJson>();
    auto MyDBSPtr = app().getPlugin<MyDBService>();
    const unordered_map<string, string> umapPara = req->getParameters();
    MyBasePtr->TRACELog("Resource::UBS::body" + string(req->getBody()), true);
    

    Result = HttpResponse::newHttpJsonResponse(RespVal);

    try
    {
        // 读取Json数据
        ReqVal = *req->getJsonObject();
        MyBasePtr->DEBUGLog("ReqVal::" + ReqVal.toStyledString(), true);

        RespVal["简介"] = "管理员资源管理接口-图书状态管理";
        MyJsonPtr->UnMapToJson(ReqVal, umapPara, "Para");

        // 开始检查传入参数
        {
            MyBasePtr->DEBUGLog("开始检查传入参数是否合法", true);
            // "Para"           :   {"User_ID":"","Login_Status":""} 执行者
            // "Book_ID":0,
            // "Part_Num"0,
            // "Chapter_Num":0
            // "Version":0
            std::map<string, MyJson::ColType> ColMap;
            ColMap["Para"] = MyJson::ColType::JSON;
            ColMap["Book_ID"] = MyJson::ColType::INT;
            ColMap["Status"] = MyJson::ColType::STRING;
            ColMap["Explain"] = MyJson::ColType::STRING;
            MyJsonPtr->checkMemberAndTypeInMap(ReqVal, RespVal, ColMap);
            MyBasePtr->DEBUGLog("传入参数合法", true);
        }
        
        bool res = MyDBSPtr->Change_Book_Status(ReqVal, RespVal);
        if(!res) throw RespVal;
        MyBasePtr->DEBUGLog("RespVal::" + RespVal.toStyledString(), true);

        // 设置返回格式
        ResultData["Result"] = true;
        ResultData["Message"] = "修改图书状态成功";
        ResultData["Data"]["Book_Status"] = RespVal["Book_Status"];

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
        RespVal["ErrorMsg"].append("Admin::Resource::UBS::Error");
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

// 管理员指定章节版本的
void Resource::UCV(const HttpRequestPtr &req,std::function<void (const HttpResponsePtr &)> &&callback) const
{
    
    Json::Value ReqVal, RespVal,ResultData;
    drogon::HttpResponsePtr Result;
    auto MyBasePtr = app().getPlugin<MyBase>();
    auto MyJsonPtr = app().getPlugin<MyJson>();
    auto MyDBSPtr = app().getPlugin<MyDBService>();
    const unordered_map<string, string> umapPara = req->getParameters();
    MyBasePtr->TRACELog("Resource::UCV::body" + string(req->getBody()), true);
    

    Result = HttpResponse::newHttpJsonResponse(RespVal);

    try
    {
        // 读取Json数据
        ReqVal = *req->getJsonObject();
        MyBasePtr->DEBUGLog("ReqVal::" + ReqVal.toStyledString(), true);

        RespVal["简介"] = "管理员资源管理接口-章节版本管理";
        MyJsonPtr->UnMapToJson(ReqVal, umapPara, "Para");

        // 开始检查传入参数
        {
            MyBasePtr->DEBUGLog("开始检查传入参数是否合法", true);
            // "Para"           :   {"User_ID":"","Login_Status":""} 执行者
            // "Book_ID":0,
            // "Part_Num"0,
            // "Chapter_Num":0
            // "Version":0
            std::map<string, MyJson::ColType> ColMap;
            ColMap["Para"] = MyJson::ColType::JSON;
            ColMap["Book_ID"] = MyJson::ColType::INT;
            ColMap["Part_Num"] = MyJson::ColType::INT;
            ColMap["Chapter_Num"] = MyJson::ColType::INT;
            ColMap["Version"] = MyJson::ColType::INT;
            MyJsonPtr->checkMemberAndTypeInMap(ReqVal, RespVal, ColMap);
            MyBasePtr->DEBUGLog("传入参数合法", true);
        }
        
        MyDBSPtr->Select_Chapter_Version_Valid(ReqVal, RespVal);
        if(!RespVal["Result"].asBool()) throw RespVal;
        MyBasePtr->DEBUGLog("RespVal::" + RespVal.toStyledString(), true);

        // 设置返回格式
        ResultData["Result"] = true;
        ResultData["Message"] = "指定章节版本生效成功";
        ResultData["Data"] = "指定章节版本生效成功";

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
        RespVal["ErrorMsg"].append("Admin::Resource::UCV::Error");
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


// 管理员资源下载接口
void Resource::Download(const HttpRequestPtr &req,std::function<void (const HttpResponsePtr &)> &&callback) const
{
    Json::Value ReqVal,RespVal;
    drogon::HttpResponsePtr Result;
    auto MyBasePtr = app().getPlugin<MyBase>();
    auto MyJsonPtr = app().getPlugin<MyJson>();
    auto MyDBSPtr = app().getPlugin<MyDBService>();
    const unordered_map<string,string>umapPara = req->getParameters();
    MyBasePtr->TRACELog("Resource::Download::body" + string(req->getBody()), true);
    
    try
    {
        // 读取Json数据
        ReqVal = *req->getJsonObject();
        MyBasePtr->DEBUGLog("ReqVal::" + ReqVal.toStyledString(), true);


        RespVal["简介"] = "管理员资源下载接口";
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

        MyDBSPtr->Download_Resource_Public(ReqVal, RespVal);
        MyBasePtr->DEBUGLog("RespVal::" + RespVal.toStyledString(), true);

        Result = HttpResponse::newHttpJsonResponse(RespVal);
    }
    catch (Json::Value &RespVal)
    {
        MyBasePtr->TRACE_ERROR(RespVal["ErrorMsg"]);

        Result = HttpResponse::newHttpJsonResponse(RespVal);
    }
    catch (const drogon::orm::DrogonDbException &e)
    {
        RespVal["ErrorMsg"].append(e.base().what());
        MyBasePtr->TRACE_ERROR(RespVal["ErrorMsg"]);
        Result = HttpResponse::newHttpJsonResponse(RespVal);
    }
    catch (...)
    {
        RespVal["ErrorMsg"].append("Admin::Resource::Download::Error");
        MyBasePtr->TRACE_ERROR(RespVal["ErrorMsg"]);
        Result = HttpResponse::newHttpJsonResponse(RespVal);
    }

    Result->setStatusCode(k200OK);
    Result->setContentTypeCode(CT_TEXT_HTML);
    callback(Result);
}
