#include "User_Note.h"
using namespace User;
//add definition of your processing function here

// 用户点赞接口
void Note::Like(const HttpRequestPtr &req,std::function<void (const HttpResponsePtr &)> &&callback) const
{
    Json::Value ReqVal, RespVal;
    drogon::HttpResponsePtr Result;
    auto MyBasePtr = app().getPlugin<MyBase>();
    auto MyJsonPtr = app().getPlugin<MyJson>();
    auto MyDBS     = app().getPlugin<MyDBService>();
    const unordered_map<string, string> umapPara = req->getParameters();
    MyBasePtr->TRACELog("Note::Like::body" + string(req->getBody()), true);
    
    try
    {
        // 读取Json数据
        ReqVal=*req->getJsonObject();
        // "Note_ID" : 0,// 帖子ID
        // "Floor_ID" : 0,// 楼层ID
        // "Agree_Result" : true/false,//用户点赞操作
        MyBasePtr->DEBUGLog("点赞前检查点赞数据是否合法", true);
        std::map<string, MyJson::ColType> ColMap;
        ColMap["Note_ID"] = MyJson::ColType::INT;
        ColMap["Floor_ID"] = MyJson::ColType::INT;
        ColMap["Agree_Result"] = MyJson::ColType::BOOL;
        MyJsonPtr->checkMemberAndTypeInMap(ReqVal, RespVal, ColMap);
        MyBasePtr->DEBUGLog("点赞前检查点赞数据显示合法", true);
        // "User_ID" : 0,// 用户ID
        
        ReqVal["User_ID"] = atoi(umapPara.at("User_ID").c_str());
        
        MyDBS->Agree(ReqVal,RespVal);
        if(!RespVal["Result"].asBool())
        {
            MyBasePtr->DEBUGLog("点赞相关操作失败", true);
            RespVal["ErrorMsg"].append("点赞相关操作失败");
            throw RespVal;
        }
        MyBasePtr->DEBUGLog("点赞相关操作成功", true);  
        RespVal["简介"] = "用户点赞接口";
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
        RespVal["ErrorMsg"].append("Note::Like::Error");
        MyBasePtr->TRACE_ERROR(RespVal["ErrorMsg"]);
        Result = HttpResponse::newHttpJsonResponse(RespVal);
    }

    Result->setStatusCode(k200OK);
    Result->setContentTypeCode(CT_TEXT_HTML);
    callback(Result);
}

// 用户评论接口
void Note::Comment(const HttpRequestPtr &req,std::function<void (const HttpResponsePtr &)> &&callback) const
{
    Json::Value ReqVal, RespVal;
    drogon::HttpResponsePtr Result;
    auto MyBasePtr = app().getPlugin<MyBase>();
    auto MyJsonPtr = app().getPlugin<MyJson>();
    auto MyDBS = app().getPlugin<MyDBService>();
    const unordered_map<string, string> umapPara = req->getParameters();
    MyBasePtr->TRACELog("Note::Comment::body" + string(req->getBody()), true);
    
    try
    {
        ReqVal = *req->getJsonObject();
        MyBasePtr->DEBUGLog("创建评论前检查评论数据是否合法", true);
        std::map<string, MyJson::ColType> ColMap;
        ColMap["Note_ID"] = MyJson::ColType::INT;
        ColMap["Comment_Content"] = MyJson::ColType::JSON;
        MyJsonPtr->checkMemberAndTypeInMap(ReqVal, RespVal, ColMap);
        if(ReqVal.isMember("Reply_Floor_ID"))
        {
            RespVal["ErrorMsg"].append("回复别人的评论请使用回复接口");
            throw RespVal;
        }
        MyBasePtr->DEBUGLog("创建评论前检查评论数据显示合法", true);
        RespVal["简介"] = "用户评论接口";
        MyJsonPtr->UnMapToJson(ReqVal, umapPara, "Para");
        MyDBS->Create_Comment(ReqVal,RespVal);
        if(RespVal["Result"].asBool())
        {
            RespVal["Msg"] = "评论成功";
            // 更新帖子评论数量
        }
        else
        {
            RespVal["Msg"] = "评论失败";
        }
        MyBasePtr->DEBUGLog("RespVal::" + RespVal.toStyledString(), true);

        Result=HttpResponse::newHttpJsonResponse(RespVal);
    }
    catch (Json::Value &RespVal)
    {
        RespVal["Result"] = false;
        MyBasePtr->TRACE_ERROR(RespVal["ErrorMsg"]);
        Result = HttpResponse::newHttpJsonResponse(RespVal);
    }
    catch (...)
    {
        RespVal["Result"] = false;
        RespVal["ErrorMsg"].append("Note::Reply::ERROR");
        MyBasePtr->TRACE_ERROR(RespVal["ErrorMsg"]);
        Result = HttpResponse::newHttpJsonResponse(RespVal);
    }

    Result->setStatusCode(k200OK);
    Result->setContentTypeCode(CT_TEXT_HTML);
    callback(Result);
}

// 用户回复接口
void Note::Reply(const HttpRequestPtr &req,std::function<void (const HttpResponsePtr &)> &&callback) const
{
    Json::Value ReqVal, RespVal;
    drogon::HttpResponsePtr Result;
    auto MyBasePtr = app().getPlugin<MyBase>();
    auto MyJsonPtr = app().getPlugin<MyJson>();
    auto MyDBS     = app().getPlugin<MyDBService>();
    const unordered_map<string, string> umapPara = req->getParameters();
    MyBasePtr->TRACELog("Note::Reply::body" + string(req->getBody()), true);
    
    try
    {
        ReqVal = *req->getJsonObject();
        MyBasePtr->DEBUGLog("创建评论前检查评论数据是否合法", true);
        if(!ReqVal.isMember("Reply_Floor_ID"))
        {
            RespVal["ErrorMsg"].append("发表与他人无关的评论请不要携带别的评论的楼层ID并使用评论接口");
            throw RespVal;
        }
        std::map<string, MyJson::ColType> ColMap;
        ColMap["Note_ID"] = MyJson::ColType::INT;
        ColMap["Reply_Floor_ID"] = MyJson::ColType::INT;
        ColMap["Comment_Content"] = MyJson::ColType::JSON;
        MyJsonPtr->checkMemberAndTypeInMap(ReqVal, RespVal, ColMap);

        MyBasePtr->DEBUGLog("创建回复前检查回复数据显示合法", true);
        RespVal["简介"] = "用户回复接口";
        MyJsonPtr->UnMapToJson(ReqVal, umapPara, "Para");
        MyDBS->Create_Comment(ReqVal,RespVal);
        if(RespVal["Result"].asBool())
        {
            RespVal["Msg"] = "回复成功";
        }
        else
        {
            RespVal["Msg"] = "回复失败";
        }

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
        RespVal["ErrorMsg"].append("Note::Reply::ERROR");
        MyBasePtr->TRACE_ERROR(RespVal["ErrorMsg"]);
        Result = HttpResponse::newHttpJsonResponse(RespVal);
    }

    Result->setStatusCode(k200OK);
    Result->setContentTypeCode(CT_TEXT_HTML);
    callback(Result);
}


// 用户发布求助帖接口
void Note::Appeal(const HttpRequestPtr &req,std::function<void (const HttpResponsePtr &)> &&callback) const
{
    Json::Value ReqVal, RespVal, ParaVal;
    drogon::HttpResponsePtr Result;
    auto MyBasePtr = app().getPlugin<MyBase>();
    auto MyJsonPtr = app().getPlugin<MyJson>();
    auto MyDBSPtr = app().getPlugin<MyDBService>();
    const unordered_map<string, string> umapPara = req->getParameters();
    MyBasePtr->TRACELog("Note::Appeal::body" + string(req->getBody()), true);
    
        
    try
    {
        RespVal["简介"] = "用户发布求助帖接口";
        ReqVal = *req->getJsonObject();
        MyJsonPtr->UnMapToJson(ReqVal, umapPara, "Para");
         // 插入前检查帖子数据是否合法

        MyBasePtr->DEBUGLog("插入前检查帖子数据是否合法", true);
        // "Note_Title"    :   "", // 帖子标题,图书资源帖标题由书名(作者名)构成
        // "Note_Content"  :   {
        //     "Content"   :   "",// 如果是图书资源则内容为图书简介
        //     "Book_ID"   :   0,// 只有图书资源有此项 连接到图书ID 
        // }, 
        // "Note_Type"     :   "",// 帖子类型，(Help/Resource)
        // 创建检查列表 key是字段名 value 是字段类型
        std::map<string, MyJson::ColType> ColMap;
        ColMap["Note_Title"] = MyJson::ColType::STRING;
        ColMap["Note_Content"] = MyJson::ColType::JSON;
        ColMap["Note_Type"] = MyJson::ColType::STRING;
        ColMap["Para"]  = MyJson::ColType::JSON;
        MyJsonPtr->checkMemberAndTypeInMap(ReqVal, RespVal, ColMap);
        MyBasePtr->DEBUGLog("插入前检查显示帖子数据合法", true);

        ParaVal = ReqVal["Para"];
        MyBasePtr->DEBUGLog("开始检查Para数据", true);
        ColMap.clear();
        ColMap["User_ID"] = MyJson::ColType::STRING;
        ColMap["Login_Status"] = MyJson::ColType::STRING;
        MyJsonPtr->checkMemberAndTypeInMap(ParaVal, RespVal, ColMap);
        MyBasePtr->DEBUGLog("检查Para数据完成", true);

        ReqVal["User_ID"] = atoi(ParaVal["User_ID"].asString().c_str());
        ReqVal["Processor_Type"] = ParaVal["Login_Status"].asString();
        
        MyDBSPtr->Create_Note(ReqVal,RespVal);
        
        if(RespVal["Result"].asBool())
        {
            MyBasePtr->DEBUGLog("帖子发布成功::Note::" + RespVal["Note_Data"].toStyledString(), true);
            Json::Value CommentJson;
            MyJsonPtr->UnMapToJson(CommentJson, umapPara, "Para");
            CommentJson["Note_ID"] = RespVal["Note_Data"]["Note_ID"];
            CommentJson["Comment_Content"]["Content"] = "欢迎大家积极留言,共同努力打造一个良好的阅读环境";
            MyBasePtr->DEBUGLog("开始发布楼主评论", true);
            MyDBSPtr->Create_Comment(CommentJson,RespVal);
            if(!RespVal["Result"].asBool())
            {
                MyBasePtr->DEBUGLog("建立楼主评论失败", true);
                throw RespVal;
            }
            MyBasePtr->DEBUGLog("建立楼主评论完成", true);
        }
        else
        {
            RespVal["ErrorMsg"].append("帖子发布失败");
            throw RespVal;
        }

        Result=HttpResponse::newHttpJsonResponse(RespVal);
    }
    catch (Json::Value &RespVal)
    {
        MyBasePtr->DEBUGLog("RespVal::" + RespVal.toStyledString(), true);
        MyBasePtr->TRACE_ERROR(RespVal["ErrorMsg"]);
        Result = HttpResponse::newHttpJsonResponse(RespVal);
    }
    catch (...)
    {
        RespVal["ErrorMsg"].append("Note::Appeal::Error");
        MyBasePtr->TRACE_ERROR(RespVal["ErrorMsg"]);
        Result = HttpResponse::newHttpJsonResponse(RespVal);
    }

    Result->setStatusCode(k200OK);
    Result->setContentTypeCode(CT_TEXT_HTML);
    callback(Result);
}
