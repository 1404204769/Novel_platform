#include "User_Resource.h"
using namespace User;
// add definition of your processing function here

// 图书上传接口
void Resource::Upload(const HttpRequestPtr &req, std::function<void(const HttpResponsePtr &)> &&callback) const
{
    Json::Value ReqVal, RespVal, ParaJson;
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

            ParaJson = ReqVal["Para"];
            MyBasePtr->DEBUGLog("开始检查Para数据", true);
            ColMap.clear();
            ColMap["User_ID"] = MyJson::ColType::STRING;
            ColMap["Login_Status"] = MyJson::ColType::STRING;
            MyJsonPtr->checkMemberAndTypeInMap(ParaJson, RespVal, ColMap);
            MyBasePtr->DEBUGLog("检查Para数据完成", true);
        }

        // 读取UserID LoginStatus数据
        Json::Value ExamineJson;
        // "Processor_Type"     :   "",         //审核人类型(system/admin/root)
        // "Processor_ID"       :   0,          //审核人ID
        // "Examine_Result"     :   true/false, //审核结果             
        // "Upload_ID"          :   0,          //审核对象
        // "Examine_Type"       :   "",         //审核对象类型(Book/Chapter)    
        ExamineJson["Processor_Type"] = "system";
        ExamineJson["Processor_ID"] = 0;
        auto UploadType = ReqVal["Upload_Type"].asString();
        // 纯粹的新书，直接插入图书类即可
        if (UploadType == "new_book")
        {
            MyBasePtr->DEBUGLog("开始申请加入图书", true);
            MyDBSPtr->Upload_Book(ReqVal, RespVal);
            MyBasePtr->DEBUGLog("申请加入图书完成", true);
            if(RespVal["Result"].asBool() == true)
            {
                ExamineJson["Examine_Type"] =   "Book";
                ExamineJson["Upload_ID"]    =   RespVal["Upload_ID"].asInt();
                // 检查图书是否存在 存在则拒绝申请，不存在则同意
                if(MyDBSPtr->Is_Book_Exist(ReqVal,RespVal))
                {
                    ExamineJson["Examine_Result"] = false;
                    
                    ExamineJson["Examine_Explain"] = "要新增的图书已存在";
                }
                else
                {
                    ExamineJson["Examine_Result"] = true;
                }
                MyBasePtr->DEBUGLog("开始审核申请", true);
                MyDBSPtr->Examine_Upload(ExamineJson, RespVal);
                MyBasePtr->DEBUGLog("审核申请完成", true);
                if(RespVal.isMember("Book_Data") && RespVal["Book_Data"].isObject())
                {
                    // 创建图书成功，准备创建资源贴
                    // Para中获取用户ID，Resp中Book_Data获取图书数据
                    /*
                        NoteJson:{
                            "Note_Title" : "", // 帖子标题,图书资源帖标题由书名(作者名)构成
                            "Note_Content" : {
                                "Content" : "",// 如果是图书资源则内容为图书简介
                                "Book_ID" : 0,// 只有图书资源有此项 连接到图书ID
                            },
                            "User_ID" : 0,// 创建者ID，保存第一个创建图书者的ID
                            "Note_Type" : "",// 帖子类型，(Help/Resource)
                            "Processor_Type": "",// System/root/admin有效
                        }
                    */
                    // 检测是否存在图书数据
                    drogon_model::novel::Book book;
                    Json::Value NoteJson,NoteContentJson;
                    NoteJson["User_ID"] = atoi(ParaJson["User_ID"].asString().c_str());
                    NoteJson["Processor_Type"] = "system";
                    NoteJson["Note_Type"] = "Resource";
                    MyBasePtr->DEBUGLog("开始获取图书数据", true);
                    book.updateByJson(RespVal["Book_Data"]);
                    MyBasePtr->DEBUGLog("获取图书数据完成", true);
                    NoteJson["Note_Title"] = book.getValueOfName()+"("+book.getValueOfAuthor()+")";
                    NoteContentJson["Book_ID"] = book.getValueOfBookId();
                    NoteContentJson["Content"] = book.getValueOfSynopsis();
                    NoteJson["Note_Content"] = NoteContentJson;
                    MyBasePtr->DEBUGLog("开始建立资源贴", true);
                    MyDBSPtr->Create_Note(NoteJson,RespVal);
                    if(!RespVal["Result"].asBool())
                    {
                        MyBasePtr->DEBUGLog("建立资源贴失败", true);
                        throw RespVal;
                    }
                    MyBasePtr->DEBUGLog("建立资源贴完成", true);


                    Json::Value CommentJson;
                    MyJsonPtr->UnMapToJson(CommentJson, umapPara, "Para");
                    // "Note_ID" : 0, // 帖子ID
                    // "Comment_Content" : {
                    //     "Content":"",//表示内容
                    //     ["URL"]:0,//表示超链接，可以连接到别的表，用于将资源贴引用到求助帖
                    //     // url格式为Note_ID
                    // },
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
            }
        }
        // 已存在的书上传新的章节
        else if (UploadType == "old_book_new")
        {
            MyBasePtr->DEBUGLog("开始申请加入后续章节", true);
            MyDBSPtr->Upload_Chapter(ReqVal, RespVal);
            MyBasePtr->DEBUGLog("申请加入后续章节完成", true);
            if(RespVal["Result"].asBool() == true)
            {
                ExamineJson["Examine_Type"] =   "Chapter";
                ExamineJson["Upload_ID"]    =   RespVal["Upload_ID"].asInt();
                ExamineJson["Examine_Result"]   =   true;
                MyBasePtr->DEBUGLog("开始自动审核申请", true);
                MyDBSPtr->Auto_Examine_Chapter_Update(ExamineJson, RespVal);
                MyBasePtr->DEBUGLog("自动审核申请完成", true);
            }
        }
        // 已存在的书更正章节
        else if (UploadType == "old_book_old")
        {
            MyBasePtr->DEBUGLog("开始申请更正章节", true);
            MyDBSPtr->Upload_Chapter(ReqVal, RespVal);
            MyBasePtr->DEBUGLog("申请更正章节完成", true);
            
            if(RespVal["Result"].asBool() == true)
            {
                ExamineJson["Examine_Type"] =   "Chapter_Update";
                ExamineJson["Upload_ID"]    =   RespVal["Upload_ID"].asInt();
                ExamineJson["Examine_Result"]   =   true;
                MyBasePtr->DEBUGLog("开始自动审核申请", true);
                MyDBSPtr->Auto_Examine_Chapter_Update(ExamineJson, RespVal);
                MyBasePtr->DEBUGLog("自动审核申请完成", true);
            }
        }
        else
        {
            RespVal["ErrorMsg"].append("Upload_Type无效");
            throw RespVal;
        }

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
        RespVal["ErrorMsg"].append("Resource::Upload::Error");
        MyBasePtr->TRACE_ERROR(RespVal["ErrorMsg"]);
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
        RespVal["ErrorMsg"].append("Resource::Download::Error");
        MyBasePtr->TRACE_ERROR(RespVal["ErrorMsg"]);
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
    auto MyDBS = app().getPlugin<MyDBService>();
    const unordered_map<string, string> umapPara = req->getParameters();
    MyBasePtr->TRACELog("Resource::Search::body" + string(req->getBody()), true);
    
    try
    {
        // 读取Json数据
        ReqVal=*req->getJsonObject();
        // "Note_KeyWord" : "",// 关键字,在标题和内容中查找
        MyBasePtr->DEBUGLog("开始检查数据完整性", true);
        MyJsonPtr->checkMemberAndType(ReqVal,RespVal,"Note_KeyWord",MyJson::ColType::STRING);
        MyBasePtr->DEBUGLog("检查数据完整性完成", true);
        MyDBS->Search_Note(ReqVal,RespVal);
        RespVal["简介"] = "图书查找接口";

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
        RespVal["ErrorMsg"].append("Resource::Search::Error");
        MyBasePtr->TRACE_ERROR(RespVal["ErrorMsg"]);
        Result = HttpResponse::newHttpJsonResponse(RespVal);
    }

    Result->setStatusCode(k200OK);
    Result->setContentTypeCode(CT_TEXT_HTML);
    callback(Result);
}