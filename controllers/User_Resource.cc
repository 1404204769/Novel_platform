#include "User_Resource.h"
using namespace User;
// add definition of your processing function here

// 图书下载接口
void Resource::Download(const HttpRequestPtr &req, std::function<void(const HttpResponsePtr &)> &&callback) const
{
    Json::Value ReqVal, RespVal, ActionJson;
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
        if(!RespVal["Result"].asBool())
        {
            RespVal["ErrorMsg"].append("下载过程发生错误");
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
        RespVal["ErrorMsg"].append("Resource::Download::Error");
        MyBasePtr->TRACE_ERROR(RespVal["ErrorMsg"]);
        Result = HttpResponse::newHttpJsonResponse(RespVal);
    }

    Result->setStatusCode(k200OK);
    Result->setContentTypeCode(CT_TEXT_HTML);
    callback(Result);
}

// 图书阅读接口
void Resource::Read(const HttpRequestPtr &req,std::function<void (const HttpResponsePtr &)> &&callback) const
{

    Json::Value ReqVal, RespVal, ActionJson;
    drogon::HttpResponsePtr Result;
    auto MyJsonPtr = app().getPlugin<MyJson>();
    auto MyBasePtr = app().getPlugin<MyBase>();
    auto MyDBSPtr = app().getPlugin<MyDBService>();
    const unordered_map<string, string> umapPara = req->getParameters();
    MyBasePtr->TRACELog("Resource::Read::body" + string(req->getBody()), true);

    try
    {
        // 读取Json数据
        ReqVal = *req->getJsonObject();
        MyBasePtr->DEBUGLog("ReqVal::" + ReqVal.toStyledString(), true);


        RespVal["简介"] = "图书阅读接口";
        MyJsonPtr->UnMapToJson(ReqVal, umapPara, "Para");

        // 检查数据完整性
        {
            MyBasePtr->DEBUGLog("开始检查数据完整性", true);
            // "Book_ID":0,
            // "Chapter_Num":[],
            // 创建检查列表 key是字段名 value 是字段类型
            std::map<string, MyJson::ColType> ColMap;
            ColMap["Book_ID"] = MyJson::ColType::INT;
            ColMap["Chapter_Num"] = MyJson::ColType::INT;
            MyJsonPtr->checkMemberAndTypeInMap(ReqVal, RespVal, ColMap);
            MyBasePtr->DEBUGLog("检查数据完整性完成", true);
        }

        // 读取UserID LoginStatus数据
    
        MyDBSPtr->Read_Resource(ReqVal, RespVal);
        if(!RespVal["Result"].asBool())
        {
            RespVal["ErrorMsg"].append("查询过程发生错误");
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
        RespVal["ErrorMsg"].append("Resource::Read::Error");
        MyBasePtr->TRACE_ERROR(RespVal["ErrorMsg"]);
        Result = HttpResponse::newHttpJsonResponse(RespVal);
    }

    Result->setStatusCode(k200OK);
    Result->setContentTypeCode(CT_TEXT_HTML);
    callback(Result);
}

// 查找帖子的接口
void Resource::Search(const HttpRequestPtr &req, std::function<void(const HttpResponsePtr &)> &&callback) const
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
        MyBasePtr->DEBUGLog("开始检查数据完整性", true);
        MyJsonPtr->checkMemberAndType(ReqVal,RespVal,"Note_KeyWord",MyJson::ColType::STRING);
        MyBasePtr->DEBUGLog("检查数据完整性完成", true);
        MyDBS->Search_Note(ReqVal,RespVal);
        if(!RespVal["Result"].asBool())throw RespVal;
        RespVal["简介"] = "图书查找接口";

        MyBasePtr->DEBUGLog("RespVal::" + RespVal.toStyledString(), true);

        // 设置返回格式
        ResultData["Result"] = true;
        ResultData["Message"] = "用户查询帖子成功";
        ResultData["Data"]["Note_List"]= RespVal["Note_List"];

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


// 帖子查找评论接口
void Resource::SearchComment(const HttpRequestPtr &req,std::function<void (const HttpResponsePtr &)> &&callback) const
{
    
    Json::Value ReqVal, RespVal,ResultData;
    drogon::HttpResponsePtr Result;
    auto MyBasePtr = app().getPlugin<MyBase>();
    auto MyJsonPtr = app().getPlugin<MyJson>();
    auto MyDBS = app().getPlugin<MyDBService>();
    const unordered_map<string, string> umapPara = req->getParameters();
    MyBasePtr->TRACELog("Resource::SearchComment::body" + string(req->getBody()), true);
    
    try
    {
        // 读取Json数据
        ReqVal=*req->getJsonObject();
        // "Note_ID" : 0,
        MyBasePtr->DEBUGLog("开始检查数据完整性", true);
        MyJsonPtr->checkMemberAndType(ReqVal,RespVal,"Note_ID",MyJson::ColType::INT);
        MyBasePtr->DEBUGLog("检查数据完整性完成", true);
        MyDBS->Search_Comment(ReqVal,RespVal);
        if(!RespVal["Result"].asBool())throw RespVal;
        RespVal["简介"] = "帖子评论查找接口";

        MyBasePtr->DEBUGLog("RespVal::" + RespVal.toStyledString(), true);

        // 设置返回格式
        ResultData["Result"] = true;
        ResultData["Message"] = "用户查询帖子评论成功";
        ResultData["Data"]["Comment_List"]= RespVal["Comment_List"];
        ResultData["Data"]["Note_Data"]= RespVal["Note_Data"];

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
        RespVal["ErrorMsg"].append("Resource::SearchComment::Error");
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

// 根据BookID BookName查找目录的接口
void Resource::SearchMenu(const HttpRequestPtr &req,std::function<void (const HttpResponsePtr &)> &&callback) const
{
    Json::Value ReqVal, RespVal,ResultData;
    drogon::HttpResponsePtr Result;
    auto MyBasePtr = app().getPlugin<MyBase>();
    auto MyJsonPtr = app().getPlugin<MyJson>();
    auto MyDBS = app().getPlugin<MyDBService>();
    const unordered_map<string, string> umapPara = req->getParameters();
    MyBasePtr->TRACELog("Resource::SearchMenu::body" + string(req->getBody()), true);
    
    try
    {
        // 读取Json数据
        ReqVal=*req->getJsonObject();
        MyDBS->Search_BookMenu_By_BookIDAndName(ReqVal,RespVal);
        if(!RespVal["Result"].asBool())throw RespVal;
        RespVal["简介"] = "图书查找接口";

        MyBasePtr->DEBUGLog("RespVal::" + RespVal.toStyledString(), true);

        // 设置返回格式
        ResultData["Result"] = true;
        ResultData["Message"] = "图书目录查询成功";
        ResultData["Data"]["ChapterList"]= RespVal["Chapter_List"];
        ResultData["Data"]["BookData"]= RespVal["Book_Data"];
        Result=HttpResponse::newHttpJsonResponse(ResultData);
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
        RespVal["ErrorMsg"].append("Resource::SearchMenu::Error");
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
// 根据BookID PartNum ChapterNum查找章节数据的接口
void Resource::SearchContent(const HttpRequestPtr &req,std::function<void (const HttpResponsePtr &)> &&callback) const
{
    Json::Value ReqVal, RespVal,ResultData;
    drogon::HttpResponsePtr Result;
    auto MyBasePtr = app().getPlugin<MyBase>();
    auto MyJsonPtr = app().getPlugin<MyJson>();
    auto MyDBS = app().getPlugin<MyDBService>();
    const unordered_map<string, string> umapPara = req->getParameters();
    MyBasePtr->TRACELog("Resource::SearchContent::body" + string(req->getBody()), true);
    
    try
    {
        // 读取Json数据
        ReqVal=*req->getJsonObject();
        MyDBS->Search_ChapterContent(ReqVal,RespVal);
        if(!RespVal["Result"].asBool())throw RespVal;
        RespVal["简介"] = "图书查找接口";

        MyBasePtr->DEBUGLog("RespVal::" + RespVal.toStyledString(), true);

        // 设置返回格式
        ResultData["Result"] = true;
        ResultData["Message"] = "图书目录查询成功";
        ResultData["Data"]["ChapterContent"]= RespVal["Chapter_Content"];
        Result=HttpResponse::newHttpJsonResponse(ResultData);
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
        RespVal["ErrorMsg"].append("Resource::SearchContent::Error");
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

void MyUpload(Json::Value &ReqVal,Json::Value &RespVal)
{
    Json::Value ParaJson;
    auto MyJsonPtr = app().getPlugin<MyJson>();
    auto MyBasePtr = app().getPlugin<MyBase>();
    auto MyDBSPtr = app().getPlugin<MyDBService>();
    MyBasePtr->INFO_Func("MyUpload", true, ReqVal);
    try
    {

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

        int User_ID = atoi(ParaJson["User_ID"].asString().c_str());
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
                // 先记录一下行为数据
                MyBasePtr->DEBUGLog("上传成功,开始记录上传的行为", true);
                MyDBSPtr->Log_Resource_Upload(User_ID,RespVal["Upload_ID"].asInt(),"new_book","创建新图书");

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
                    NoteJson["User_ID"] = User_ID;
                    NoteJson["Processor_Type"] = "system";
                    NoteJson["Note_Type"] = "Resource";
                    MyBasePtr->DEBUGLog("开始获取图书数据", true);
                    book.updateByJson(RespVal["Book_Data"]);
                    MyBasePtr->DEBUGLog("获取图书数据完成", true);
                    NoteJson["Note_Title"] = book.getValueOfBookName()+"("+book.getValueOfAuthor()+")";
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


                    Json::Value CommentJson ;
                    CommentJson["Para"]["User_ID"] = ParaJson["User_ID"];
                    CommentJson["Para"]["Login_Status"] = ParaJson["Login_Status"];
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
                // 先记录一下行为数据
                MyBasePtr->DEBUGLog("上传成功,开始记录上传的行为", true);
                MyDBSPtr->Log_Resource_Upload(User_ID,RespVal["Upload_ID"].asInt(),"old_book_new","上传后续章节");

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
                // 先记录一下行为数据
                MyBasePtr->DEBUGLog("上传成功,开始记录上传的行为", true);
                MyDBSPtr->Log_Resource_Upload(User_ID,RespVal["Upload_ID"].asInt(),"old_book_old","上传更正章节");
                
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

    }
    catch (Json::Value &RespVal)
    {
        MyBasePtr->TRACE_ERROR(RespVal["ErrorMsg"]);
    }
    catch (const drogon::orm::DrogonDbException &e)
    {
        RespVal["ErrorMsg"].append(e.base().what());
        MyBasePtr->TRACE_ERROR(RespVal["ErrorMsg"]);
    }
    catch (...)
    {
        RespVal["ErrorMsg"].append("Resource::Upload::Error");
        MyBasePtr->TRACE_ERROR(RespVal["ErrorMsg"]);
    }
    
    MyBasePtr->INFO_Func("MyUpload", false, RespVal);
}

/*
Req:{
    "Book_ID":0,
    "Book_Author":"爱潜水的乌贼",//作者
    "Book_Name":"诡秘之主", // 书名
    "Upload_Type" : "",// 类型
    "Chapter_List":[
        {
            "Vol_Num":0,
            "Chapter_Num":0,
            "Chapter_Title": "",
            "Content_Array": [
                ""
            ]
        }
    ]
}
Resp:{
    "Chapter_List":[
        {
            "Vol_Num":0,
            "Chapter_Num":0,
            "Result":true/false
        }
    ]
}
*/
void MyUploadChapterList(Json::Value &ReqVal,Json::Value &RespVal)
{
    auto MyJsonPtr = app().getPlugin<MyJson>();
    auto MyBasePtr = app().getPlugin<MyBase>();
    auto MyDBSPtr = app().getPlugin<MyDBService>();
    MyBasePtr->INFO_Func("MyUploadNewChapterList", true, ReqVal);
    try
    {

        // 检查数据完整性
        {
            MyBasePtr->DEBUGLog("开始检查数据完整性", true);
            // "Book_ID":0,
            // "Book_Author":"爱潜水的乌贼",//作者
            // "Book_Name":"诡秘之主", // 书名
            // "Upload_Type" : "",// 类型
            // "Chapter_List":[]
            // 创建检查列表 key是字段名 value 是字段类型
            std::map<string, MyJson::ColType> ColMap;
            ColMap["Book_Author"] = MyJson::ColType::STRING;
            ColMap["Book_Name"] = MyJson::ColType::STRING;
            ColMap["Upload_Type"] = MyJson::ColType::STRING;
            ColMap["Book_ID"] = MyJson::ColType::INT;
            ColMap["Chapter_List"] = MyJson::ColType::ARRAY;
            MyJsonPtr->checkMemberAndTypeInMap(ReqVal, RespVal, ColMap);
            MyBasePtr->DEBUGLog("检查数据完整性完成", true);
        }
        // 先检查一遍章节内容，去掉数据不完整的
        int chaptersize = ReqVal["Chapter_List"].size();
        Json::Value chap,ChapterReq,ChapterResp;
        Json::Value TempReq,TempResp;
        
        ChapterReq["Para"] = ReqVal["Para"];
        ChapterReq["Book_ID"] = ReqVal["Book_ID"];
        ChapterReq["Book_Author"] = ReqVal["Book_Author"];
        ChapterReq["Book_Name"] = ReqVal["Book_Name"];
        ChapterReq["Upload_Type"] = ReqVal["Upload_Type"];
        for(int i = 0;i<chaptersize;i++)
        {
            TempReq.clear();
            TempResp.clear();
            ChapterResp.clear();
            TempReq = ReqVal["Chapter_List"][i];
            chap["Vol_Num"] = TempReq["Vol_Num"];
            chap["Chapter_Num"] = TempReq["Chapter_Num"];
            try
            {
                // 检查数据完整性
                {
                    MyBasePtr->DEBUGLog("开始检查数据完整性", true);
                    // "Vol_Num":0,
                    // "Chapter_Num":0,
                    // "Chapter_Title": "",
                    // "Content_Array": [""]
                    // 创建检查列表 key是字段名 value 是字段类型
                    std::map<string, MyJson::ColType> ColMap;
                    ColMap["Vol_Num"] = MyJson::ColType::INT;
                    ColMap["Chapter_Num"] = MyJson::ColType::INT;
                    ColMap["Chapter_Title"] = MyJson::ColType::STRING;
                    ColMap["Content_Array"] = MyJson::ColType::ARRAY;
                    MyJsonPtr->checkMemberAndTypeInMap(TempReq, TempResp, ColMap);
                    MyBasePtr->DEBUGLog("检查数据完整性完成", true);
                }
                chap["Result"] = true;
            }
            catch(Json::Value &e)
            {
                chap["Result"] = false;
            }
            // 如果存在缺陷，则跳过这个
            if(!chap["Result"].asBool())
            {
                RespVal["Chapter_List"].append(chap);
                continue;
            }

            ChapterReq["Content"] = TempReq;
            MyUpload(ChapterReq,ChapterResp);
            int uploadid = ChapterResp["Upload_ID"].asInt();
            TempReq.clear();
            TempResp.clear();
            TempReq["Upload_ID"] = uploadid;
            MyDBSPtr->Search_Upload_By_UploadID(TempReq,TempResp);
            if(!TempResp["Result"].asBool())
            {
                chap["Result"] = false;
            }
            if(TempResp["Upload_Data"]["Status"].asString() == "system已拒绝")
            {
                chap["Result"] = false;
            }
            if(TempResp["Upload_Data"]["Status"].asString() == "system已允许/已成功")
            {
                chap["Result"] = true;
            }

            RespVal["Chapter_List"].append(chap);
        }
    }
    catch (Json::Value &e)
    {
        MyBasePtr->TRACE_ERROR(e["ErrorMsg"]);
        // 设置返回格式
        int ErrorSize = e["ErrorMsg"].size();
        RespVal["Result"] = false;
        RespVal["Message"] = e["ErrorMsg"][ErrorSize - 1];

    }
    
    MyBasePtr->INFO_Func("MyUploadNewChapterList", false, RespVal);
}

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

    // 读取Json数据
    ReqVal = *req->getJsonObject();
    MyBasePtr->DEBUGLog("ReqVal::" + ReqVal.toStyledString(), true);

    RespVal["简介"] = "图书上传接口";
    MyJsonPtr->UnMapToJson(ReqVal, umapPara, "Para");
    MyUpload(ReqVal,RespVal);
    Result = HttpResponse::newHttpJsonResponse(RespVal);
    Result->setStatusCode(k200OK);
    Result->setContentTypeCode(CT_TEXT_HTML);
    callback(Result);
}

void Resource::UploadLN(const HttpRequestPtr &req,std::function<void (const HttpResponsePtr &)> &&callback) const
{
    //"system已拒绝"
    //"system已允许/已成功"
    Json::Value ReqVal, RespVal, ParaJson,ResultData;
    drogon::HttpResponsePtr Result;
    auto MyJsonPtr = app().getPlugin<MyJson>();
    auto MyBasePtr = app().getPlugin<MyBase>();
    auto MyDBSPtr = app().getPlugin<MyDBService>();
    const unordered_map<string, string> umapPara = req->getParameters();
    MyBasePtr->TRACELog("Resource::UploadLN::body" + string(req->getBody()), true);

    try
    {
        // 读取Json数据
        ReqVal = *req->getJsonObject();
        MyBasePtr->DEBUGLog("ReqVal::" + ReqVal.toStyledString(), true);

        RespVal["简介"] = "图书上传列表新接口";
        MyJsonPtr->UnMapToJson(ReqVal, umapPara, "Para");

        // 检查数据完整性
        {
            MyBasePtr->DEBUGLog("开始检查数据完整性", true);
            // "Book_Author":"爱潜水的乌贼",//作者
            // "Book_Name":"诡秘之主", // 书名
            // "Book_Publisher":"起点中文网",// 签约平台
            // "Book_Synopsis":"" // 提要
        
            //  "Chapter_List":[],//章节列表
            // 创建检查列表 key是字段名 value 是字段类型
            std::map<string, MyJson::ColType> ColMap;
            ColMap["Book_Author"] = MyJson::ColType::STRING;
            ColMap["Book_Name"] = MyJson::ColType::STRING;
            ColMap["Book_Publisher"] = MyJson::ColType::STRING;
            ColMap["Book_Synopsis"] = MyJson::ColType::STRING;
            ColMap["Chapter_List"] = MyJson::ColType::ARRAY;
            MyJsonPtr->checkMemberAndTypeInMap(ReqVal, RespVal, ColMap);
            MyBasePtr->DEBUGLog("检查数据完整性完成", true);
        }

        // 先判断此书是否存在
        Json::Value TempReq,TempResp;
        TempReq["Book_Name"] = ReqVal["Book_Name"];
        TempReq["Author"] = ReqVal["Book_Author"];
        TempReq["Publisher"] = ReqVal["Book_Publisher"];
        MyDBSPtr->Search_Book(TempReq,TempResp);
        int BookID = 0;
        TempReq.clear();
        MyJsonPtr->UnMapToJson(TempReq, umapPara, "Para");
        TempReq["Book_ID"] = 0;
        TempReq["Book_Author"] = ReqVal["Book_Author"];
        TempReq["Book_Name"] = ReqVal["Book_Name"];
        TempReq["Upload_Type"] = "new_book";
        TempReq["Content"]["Book_Author"] = ReqVal["Book_Author"];
        TempReq["Content"]["Book_Name"] = ReqVal["Book_Name"];
        TempReq["Content"]["Book_Publisher"] = ReqVal["Book_Publisher"];
        TempReq["Content"]["Book_Synopsis"] = ReqVal["Book_Synopsis"];

        // 根据情况查询BookID
        if(!TempResp["Result"].asBool())
        {
            // 不存在
            RespVal["ErrorMsg"].append("目标图书不存在");
            throw RespVal;
        }
        else{
            // 判读是否唯一
            if(TempResp["Book_List"].size() > 1)
            {
                // 不唯一
                RespVal["ErrorMsg"].append("目标图书不唯一");
                throw RespVal;
            }
            else if(TempResp["Book_List"].size() == 1)
            {
                BookID = TempResp["Book_List"][0]["Book_ID"].asInt();
            }
            else
            {
                TempResp.clear();
                MyUpload(TempReq,TempResp);
                int uploadid = TempResp["Upload_ID"].asInt();
                TempReq.clear();
                TempResp.clear();
                TempReq["Upload_ID"] = uploadid;
                MyDBSPtr->Search_Upload_By_UploadID(TempReq,TempResp);
                if(!TempResp["Result"].asBool())
                {
                    RespVal["ErrorMsg"].append("新图书上传ID异常");
                    throw RespVal;
                }
                if(TempResp["Upload_Data"]["Status"].asString() == "system已拒绝")
                {
                    RespVal["ErrorMsg"].append("新图书上传已被系统拒绝");
                    throw RespVal;
                }
                if(TempResp["Upload_Data"]["Status"].asString() == "system已允许/已成功")
                {
                    BookID = TempResp["Upload_Data"]["Book_ID"].asInt();
                }
            }
        }

        ReqVal["Book_ID"] = BookID;
        ReqVal["Upload_Type"] = "old_book_new";
        MyUploadChapterList(ReqVal,RespVal);

        // 设置返回格式
        ResultData["Result"] = true;
        ResultData["Message"] = "用户上传资源成功";
        ResultData["Data"]["Chapter_List"] = RespVal["Chapter_List"];

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
    catch (const drogon::orm::DrogonDbException &e)
    {
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
        RespVal["ErrorMsg"].append("Resource::UploadLN::Error");
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

void Resource::UploadLO(const HttpRequestPtr &req,std::function<void (const HttpResponsePtr &)> &&callback) const
{

    //"system已拒绝"
    //"system已允许/已成功"
    Json::Value ReqVal, RespVal, ParaJson,ResultData;
    drogon::HttpResponsePtr Result;
    auto MyJsonPtr = app().getPlugin<MyJson>();
    auto MyBasePtr = app().getPlugin<MyBase>();
    auto MyDBSPtr = app().getPlugin<MyDBService>();
    const unordered_map<string, string> umapPara = req->getParameters();
    MyBasePtr->TRACELog("Resource::UploadLO::body" + string(req->getBody()), true);

    try
    {
        // 读取Json数据
        ReqVal = *req->getJsonObject();
        MyBasePtr->DEBUGLog("ReqVal::" + ReqVal.toStyledString(), true);

        RespVal["简介"] = "图书上传列表新接口";
        MyJsonPtr->UnMapToJson(ReqVal, umapPara, "Para");

        // 检查数据完整性
        {
            MyBasePtr->DEBUGLog("开始检查数据完整性", true);
            // "Book_Author":"爱潜水的乌贼",//作者
            // "Book_Name":"诡秘之主", // 书名
            // "Book_Publisher":"起点中文网",// 签约平台
            //  "Chapter_List":[],//章节列表
            // 创建检查列表 key是字段名 value 是字段类型
            std::map<string, MyJson::ColType> ColMap;
            ColMap["Book_Author"] = MyJson::ColType::STRING;
            ColMap["Book_Name"] = MyJson::ColType::STRING;
            ColMap["Book_Publisher"] = MyJson::ColType::STRING;
            ColMap["Chapter_List"] = MyJson::ColType::ARRAY;
            MyJsonPtr->checkMemberAndTypeInMap(ReqVal, RespVal, ColMap);
            MyBasePtr->DEBUGLog("检查数据完整性完成", true);
        }

        // 先判断此书是否存在
        Json::Value TempReq,TempResp;
        TempReq["Book_Name"] = ReqVal["Book_Name"];
        TempReq["Author"] = ReqVal["Book_Author"];
        TempReq["Publisher"] = ReqVal["Book_Publisher"];
        MyDBSPtr->Search_Book(TempReq,TempResp);
        int BookID = 0;
        TempReq.clear();
        MyJsonPtr->UnMapToJson(TempReq, umapPara, "Para");

        // 根据情况查询BookID
        if(!TempResp["Result"].asBool())
        {
            // 不存在
            RespVal["ErrorMsg"].append("目标图书不存在");
            throw RespVal;
        }
        else{
            // 判读是否唯一
            if(TempResp["Book_List"].size() > 1)
            {
                // 不唯一
                RespVal["ErrorMsg"].append("目标图书不唯一");
                throw RespVal;
            }
            else if(TempResp["Book_List"].size() == 1)
            {
                BookID = TempResp["Book_List"][0]["Book_ID"].asInt();
            }
            else
            {
                // 不存在
                RespVal["ErrorMsg"].append("目标图书不存在");
                throw RespVal;
            }
        }

        ReqVal["Book_ID"] = BookID;
        ReqVal["Upload_Type"] = "old_book_old";
        MyUploadChapterList(ReqVal,RespVal);
        
        // 设置返回格式
        ResultData["Result"] = true;
        ResultData["Message"] = "用户申请更改资源成功";
        ResultData["Data"]["Chapter_List"] = RespVal["Chapter_List"];

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
    catch (const drogon::orm::DrogonDbException &e)
    {
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
        RespVal["ErrorMsg"].append("Resource::UploadLO::Error");
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