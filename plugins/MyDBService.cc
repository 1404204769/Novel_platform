/**
 *
 *  MyDBService.cc
 *
 */

#include "MyDBService.h"

using namespace drogon;

void MyDBService::initAndStart(const Json::Value &config)
{
    /// Initialize and start the plugin
    auto MyBasePtr = drogon::app().getPlugin<MyBase>();
    MyBasePtr->TRACELog("MyDBService initAndStart", true);
}

void MyDBService::shutdown()
{
    /// Shutdown the plugin
    auto MyBasePtr = drogon::app().getPlugin<MyBase>();
    MyBasePtr->TRACELog("MyDBService shutdown", true);
}

// 用户上传新书的接口
void MyDBService::User_Upload_New_Book(Json::Value &ReqJson, Json::Value &RespJson)
{
    auto MyJsonPtr = app().getPlugin<MyJson>();
    auto MyBasePtr = app().getPlugin<MyBase>();
    auto MyToolsPtr = app().getPlugin<MyTools>();
    MyBasePtr->DEBUGLog("User_Upload_New_Book::ReqJson: " + ReqJson.toStyledString(), true);

    auto dbclientPrt = drogon::app().getDbClient();
    Mapper<drogon_model::novel::User> UserMgr(dbclientPrt);
    Mapper<drogon_model::novel::Upload> UploadMgr(dbclientPrt);
    Mapper<drogon_model::novel::Book> BookMgr(dbclientPrt);

    int UserID,Book_ID;
    string Book_Name,Book_Author,LoginStatus;
    Json::Value ContentJson,UploadMemo,ParaJson;

    // 检查ReqJson数据
    {
        MyBasePtr->DEBUGLog("开始检查ReqJson数据", true);
        // "Book_ID"   :   0,
        // "Content"   :   "",
        // "Book_Author":"爱潜水的乌贼",//作者
        // "Book_Name":"诡秘之主", // 书名
        // "Upload_Type":   ""
        // "Para":   {}
        // 创建检查列表 key是字段名 value 是字段类型
        std::map<string, MyJson::ColType> ColMap;
        ColMap["Book_ID"] = MyJson::ColType::INT;
        ColMap["Content"] = MyJson::ColType::JSON;
        ColMap["Book_Author"] = MyJson::ColType::STRING;
        ColMap["Book_Name"] = MyJson::ColType::STRING;
        ColMap["Upload_Type"] = MyJson::ColType::STRING;
        ColMap["Para"] = MyJson::ColType::JSON;
        MyJsonPtr->checkMemberAndTypeInMap(ReqJson, RespJson, ColMap);
        MyBasePtr->DEBUGLog("检查ReqJson数据完成", true);

        MyBasePtr->DEBUGLog("开始检查Para数据", true);
        ColMap.clear();
        ParaJson = ReqJson["Para"];
        ColMap["User_ID"] = MyJson::ColType::STRING;
        ColMap["Login_Status"] = MyJson::ColType::STRING;
        MyJsonPtr->checkMemberAndTypeInMap(ParaJson, RespJson, ColMap);
        MyBasePtr->DEBUGLog("检查Para数据完成", true);

        UserID = std::atoi(ParaJson["User_ID"].asString().c_str());
        LoginStatus = ParaJson["Login_Status"].asString();
        if((LoginStatus!="user")&&(LoginStatus!="admin")&&(LoginStatus!="root"))
        {
            RespJson["ErrorMsg"] = "账户权限错误,请联系管理员";
            throw RespJson;
        }
        Book_ID = ReqJson["Book_ID"].asInt();
        ContentJson = ReqJson["Content"];
    }




    // 插入前检查content数据是否合法
    {
        MyBasePtr->DEBUGLog("开始检查content数据是否合法", true);
        // "Book_Author"   :   "",
        // "Book_Name"   :   "",
        // "Book_Publisher":   ""
        // "Book_Synopsis" : ""
        // 创建检查列表 key是字段名 value 是字段类型
        std::map<string, MyJson::ColType> ColMap;
        ColMap["Book_Name"] = MyJson::ColType::STRING;
        ColMap["Book_Author"] = MyJson::ColType::STRING;
        ColMap["Book_Publisher"] = MyJson::ColType::STRING;
        ColMap["Book_Synopsis"] = MyJson::ColType::STRING;
        MyJsonPtr->checkMemberAndTypeInMap(ContentJson, RespJson, ColMap);
        MyBasePtr->DEBUGLog("content数据合法", true);
    }

    // 核查数据合法性
    {
        MyBasePtr->DEBUGLog("开始核查数据合法性", true);
        MyJsonPtr->checkMember(ContentJson, RespJson, "Book_Name");
        MyJsonPtr->checkMember(ContentJson, RespJson, "Book_Author");
        if (ReqJson["Book_Name"].asString() != ContentJson["Book_Name"].asString())
        {
            RespJson["ErrorMsg"] = "Book_Name字段与Content/Book_Name不同";
            throw RespJson;
        }
        if (ReqJson["Book_Author"].asString() != ContentJson["Book_Author"].asString())
        {
            RespJson["ErrorMsg"] = "Book_Author字段与Content/Book_Author不同";
            throw RespJson;
        }
        MyBasePtr->DEBUGLog("核查数据合法性完成", true);
    }

    MyBasePtr->DEBUGLog("开始查询用户 : " + to_string(UserID), true);
    drogon_model::novel::User user = UserMgr.findByPrimaryKey(UserID);
    MyBasePtr->DEBUGLog("用户查询完毕 : " + user.toJson().toStyledString(), true);

    // 验证用户
    if (user.getValueOfUserId() != UserID)
    {
        RespJson["ErrorMsg"] = "登入的UserID(" + to_string(UserID) + ")与查询到的UserID(" + to_string(user.getValueOfUserId()) + ")不同";
        throw RespJson;
    }

    drogon_model::novel::Upload NewUpload;
    // 准备要处理的新数据
    {
        MyBasePtr->DEBUGLog("开始准备要处理的新数据", true);
        NewUpload.setBookId(Book_ID);
        NewUpload.setContent(ContentJson.toStyledString());
        NewUpload.setUserId(UserID);
        NewUpload.setIsmanage(false);
        NewUpload.setStatus("待处理");
        UploadMemo["Explain"] = "待处理";
        NewUpload.setMemo(UploadMemo.toStyledString());
        MyBasePtr->DEBUGLog("要处理的新数据准备完成", true);
    }
    MyBasePtr->DEBUGLog("要处理的新数据为 : " + NewUpload.toJson().toStyledString(), true);
    MyBasePtr->DEBUGLog("准备插入新数据", true);
    UploadMgr.insert(NewUpload);
    MyBasePtr->DEBUGLog("插入新数据完毕", true);
    MyBasePtr->DEBUGLog("插入的新数据为 : " + NewUpload.toJson().toStyledString(), true);

    drogon_model::novel::Book NewBook;
    Json::Value BookMemo;
    string BookName = ContentJson["Book_Name"].asString();
    string BookAuthor = ContentJson["Book_Author"].asString();
    string BookPublisher = ContentJson["Book_Publisher"].asString();
    string BookSynopsis = ContentJson["Book_Synopsis"].asString();

    // 检查图书是否已存在
    {
        MyBasePtr->DEBUGLog("开始检查图书是否已存在", true);
        Criteria BookName_cri(drogon_model::novel::Book::Cols::_Name, CompareOperator::EQ, BookName);
        Criteria BookAuthor_cri(drogon_model::novel::Book::Cols::_Author, CompareOperator::EQ, BookAuthor);
        vector<drogon_model::novel::Book> vecBook = BookMgr.findBy(BookName_cri && BookAuthor_cri);
        // 如果不是新书，则将Upload状态改为已处理且拒绝插入
        if (vecBook.size() != 0)
        {
            RespJson["ErrorMsg"] = "要新建的图书已存在(BookID = " + to_string(vecBook[0].getValueOfBookId()) + ")";
            MyBasePtr->DEBUGLog("开始更新Upload数据", true);
            NewUpload.setProcessor("System");
            NewUpload.setStatus("已拒绝");
            NewUpload.setIsmanage(true);
            UploadMemo["Explain"] = RespJson["ErrorMsg"].asString();
            NewUpload.setMemo(UploadMemo.toStyledString());
            NewUpload.setBookId(vecBook[0].getValueOfBookId());
            int count = UploadMgr.update(NewUpload);
            MyBasePtr->DEBUGLog("更新的新数据为 : " + NewUpload.toJson().toStyledString(), true);
            // 图书已存在
            if (!count)
            {
                MyBasePtr->DEBUGLog(RespJson["ErrorMsg"].asString(), true);
                RespJson["ErrorMsg"] = "更新Upload数据失败";
                throw RespJson;
            }
            MyBasePtr->DEBUGLog("更新Upload数据完毕", true);
            throw RespJson;
        }
        MyBasePtr->DEBUGLog("要插入的图书不存在", true);
    }

    // 准备要创建的新图书的数据
    {
        MyBasePtr->DEBUGLog("开始准备新图书数据", true);
        NewBook.setAuthor(BookAuthor);       // 作者
        NewBook.setName(BookName);           // 书名
        NewBook.setPublisher(BookPublisher); // 出版方
        NewBook.setStatus("连载中");         // 状态
        NewBook.setSynopsis(BookSynopsis);   // 提要
        /*
            Memo : {
                "Chapter" :[],// 数组形式记录每一卷的章节数量 第一位保存所有卷的总章节数
                "ExtraChapter":{
                    "Index" : 10000,//从10000开始的章节数都算番外
                    "Num"   : 0,//记录番外的数量
                }
            }
        */
        BookMemo["Chapter"].append(0);             // 插入总章节数
        BookMemo["Chapter"].append(0);             // 插入第一卷的章节数
        BookMemo["ExtraChapter"]["Index"] = 10000; // 插入番外章节数开始位置
        BookMemo["ExtraChapter"]["Num"] = 0;       // 插入番外的章节数

        NewBook.setMemo(BookMemo.toStyledString()); // 详细参数
        MyBasePtr->DEBUGLog("准备新图书数据完成", true);
        MyBasePtr->DEBUGLog("准备新图书数据为 : " + NewBook.toJson().toStyledString(), true);
    }

    // 准备插入图书数据
    try
    {
        MyBasePtr->DEBUGLog("准备插入新图书数据", true);
        BookMgr.insert(NewBook);
        MyBasePtr->DEBUGLog("插入新图书数据完毕", true);
        MyBasePtr->DEBUGLog("插入的新数据为 : " + NewBook.toJson().toStyledString(), true);

        // 图书创建后更新Upload信息
        {
            MyBasePtr->DEBUGLog("开始更新Upload数据", true);
            NewUpload.setProcessor("System");
            NewUpload.setStatus("已通过");
            NewUpload.setIsmanage(true);
            UploadMemo["Explain"] = "图书创建成功";
            NewUpload.setMemo(UploadMemo.toStyledString());
            NewUpload.setBookId(NewBook.getValueOfBookId());
            int count = UploadMgr.update(NewUpload);
            MyBasePtr->DEBUGLog("更新的新数据为 : " + NewUpload.toJson().toStyledString(), true);
            if (!count)
            {
                RespJson["ErrorMsg"] = "更新Upload数据失败";
                throw RespJson;
            }
            MyBasePtr->DEBUGLog("更新Upload数据完毕", true);
        }
    }
    // 新建图书失败
    catch (...)
    {
        // 图书创建失败更新Upload信息
        {
            MyBasePtr->DEBUGLog("新建图书失败", true);
            MyBasePtr->DEBUGLog("开始更新Upload数据", true);
            NewUpload.setProcessor("System");
            NewUpload.setStatus("数据异常");
            NewUpload.setIsmanage(false);
            UploadMemo["Explain"] = "图书创建失败";
            NewUpload.setMemo(UploadMemo.toStyledString());
            NewUpload.setBookId(-1);
            int count = UploadMgr.update(NewUpload);
            MyBasePtr->DEBUGLog("更新的新数据为 : " + NewUpload.toJson().toStyledString(), true);
            if (!count)
            {
                RespJson["ErrorMsg"] = "更新Upload数据失败";
                throw RespJson;
            }
            MyBasePtr->DEBUGLog("更新Upload数据完毕", true);
        }
        RespJson["ErrorMsg"] = "System 新建图书失败";
        throw RespJson;
    }
}

// 用户上传已有的书的后续章节的接口
void MyDBService::User_Upload_Exist_Book_New_Chapter(Json::Value &ReqJson, Json::Value &RespJson)
{
    auto MyJsonPtr = app().getPlugin<MyJson>();
    auto MyBasePtr = app().getPlugin<MyBase>();
    auto MyToolsPtr = app().getPlugin<MyTools>();
    MyBasePtr->DEBUGLog("User_Upload_Exist_Book_New_Chapter::ReqJson: " + ReqJson.toStyledString(), true);

    auto dbclientPrt = drogon::app().getDbClient();
    Mapper<drogon_model::novel::User> UserMgr(dbclientPrt);
    Mapper<drogon_model::novel::Upload> UploadMgr(dbclientPrt);
    Mapper<drogon_model::novel::Book> BookMgr(dbclientPrt);
    Mapper<drogon_model::novel::Chapter> ChapterMgr(dbclientPrt);

    int UserID,Book_ID;
    string Book_Name,Book_Author,LoginStatus;
    Json::Value ContentJson,UploadMemo,ParaJson;
    drogon_model::novel::Book TBook; // 目标图书
    // 检查数据完整性
    {
        MyBasePtr->DEBUGLog("开始检查数据完整性", true);
        // "Book_ID"   :   0,
        // "Content"   :   "",
        // "Book_Author":"爱潜水的乌贼",//作者
        // "Book_Name":"诡秘之主", // 书名
        // "Upload_Type":   ""
        // "Para":   {}
        // 创建检查列表 key是字段名 value 是字段类型
        std::map<string, MyJson::ColType> ColMap;
        ColMap["Book_ID"] = MyJson::ColType::INT;
        ColMap["Content"] = MyJson::ColType::JSON;
        ColMap["Book_Author"] = MyJson::ColType::STRING;
        ColMap["Book_Name"] = MyJson::ColType::STRING;
        ColMap["Upload_Type"] = MyJson::ColType::STRING;
        ColMap["Para"] = MyJson::ColType::JSON;
        MyJsonPtr->checkMemberAndTypeInMap(ReqJson, RespJson, ColMap);
        MyBasePtr->DEBUGLog("检查数据完整性完成", true);

        MyBasePtr->DEBUGLog("开始检查Para数据", true);
        ColMap.clear();
        ParaJson = ReqJson["Para"];
        ColMap["User_ID"] = MyJson::ColType::STRING;
        ColMap["Login_Status"] = MyJson::ColType::STRING;
        MyJsonPtr->checkMemberAndTypeInMap(ParaJson, RespJson, ColMap);
        MyBasePtr->DEBUGLog("检查Para数据完成", true);

        UserID = std::atoi(ParaJson["User_ID"].asString().c_str());
        LoginStatus = ParaJson["Login_Status"].asString();
        if((LoginStatus!="user")&&(LoginStatus!="admin")&&(LoginStatus!="root"))
        {
            RespJson["ErrorMsg"] = "账户权限错误,请联系管理员";
            throw RespJson;
        }
        Book_ID     = ReqJson["Book_ID"].asInt();
        ContentJson = ReqJson["Content"];
        Book_Name   = ReqJson["Book_Name"].asString();
        Book_Author = ReqJson["Book_Author"].asString();
    }

 

    

    // 先判断传入数据的Book_Name和Book_Author是否存在 若存在则判断目标图书ID是否正确
    {
        MyBasePtr->DEBUGLog("开始检查上传新章节的图书是否已存在", true);
        Criteria BookName_cri(drogon_model::novel::Book::Cols::_Name, CompareOperator::EQ, Book_Name);
        Criteria BookAuthor_cri(drogon_model::novel::Book::Cols::_Author, CompareOperator::EQ, Book_Author);
        vector<drogon_model::novel::Book> vecBook = BookMgr.findBy(BookName_cri && BookAuthor_cri);
        // 如果不是已存在的书，则拒绝插入Upload
        if (vecBook.size() == 0)
        {
            RespJson["ErrorMsg"] = "要上传新章节的图书不存在";
            throw RespJson;
        }
        MyBasePtr->DEBUGLog("要上传新章节的图书存在", true);
        TBook = vecBook[0];
        MyBasePtr->DEBUGLog("开始判断目标图书ID是否正确", true);
        if (TBook.getValueOfBookId() != Book_ID)
        {
            RespJson["ErrorMsg"] = "要上传新章节的图书的ID与指定ID不符合,(Book_ID=" + to_string(Book_ID) + ",目标图书的ID=" + to_string(TBook.getValueOfBookId());
            throw RespJson;
        }
        MyBasePtr->DEBUGLog("目标图书ID正确", true);
    }

    // 插入前检查content数据是否合法
    {
        MyBasePtr->DEBUGLog("开始检查content数据是否合法", true);
        // "Chapter": "",
        // "ContentArray":[]
        // 创建检查列表 key是字段名 value 是字段类型

        std::map<string, MyJson::ColType> ColMap;
        ColMap["ChapterTitle"] = MyJson::ColType::STRING;
        ColMap["ContentArray"] = MyJson::ColType::ARRAY;
        MyJsonPtr->checkMemberAndTypeInMap(ContentJson, RespJson, ColMap);
        MyBasePtr->DEBUGLog("检查content数据合法", true);
    }

    MyBasePtr->DEBUGLog("开始查询用户 : " + to_string(UserID), true);
    drogon_model::novel::User user = UserMgr.findByPrimaryKey(UserID);
    MyBasePtr->DEBUGLog("用户查询完毕 : " + user.toJson().toStyledString(), true);

    // 验证用户
    if (user.getValueOfUserId() != UserID)
    {
        RespJson["ErrorMsg"] = "登入的UserID(" + to_string(UserID) + ")与查询到的UserID(" + to_string(user.getValueOfUserId()) + ")不同";
        throw RespJson;
    }

    drogon_model::novel::Upload NewUpload;
    // 准备要处理的新数据
    {
        MyBasePtr->DEBUGLog("开始准备要处理的新数据", true);
        NewUpload.setBookId(Book_ID);
        NewUpload.setContent(ContentJson.toStyledString());
        NewUpload.setUserId(UserID);
        NewUpload.setIsmanage(false);
        NewUpload.setStatus("待处理");
        UploadMemo["Explain"] = "待处理";
        NewUpload.setMemo(UploadMemo.toStyledString());
        MyBasePtr->DEBUGLog("要处理的新数据准备完成", true);
    }
    MyBasePtr->DEBUGLog("要处理的新数据为 : " + NewUpload.toJson().toStyledString(), true);
    MyBasePtr->DEBUGLog("准备插入新数据", true);
    UploadMgr.insert(NewUpload);
    MyBasePtr->DEBUGLog("插入新数据完毕", true);
    MyBasePtr->DEBUGLog("插入的新数据为 : " + NewUpload.toJson().toStyledString(), true);

    drogon_model::novel::Chapter NewChapter;
    pair<int, string> Title = MyToolsPtr->getTitleNumAndTitleStr(ContentJson["ChapterTitle"].asString());
    string ChapterTitle = Title.second;                        // 章节名
    int Part_Num;                                              // 章节卷
    int Chapter_Num = Title.first;                             // 章节数
    Json::Value Chapter_Content = ContentJson["ContentArray"]; // 章节内容
    Json::Value Chapter_Memo;                                  // 参数
    Json::Value BookMemo;
    Json::Value ChapterJson; //  Memo/Chapter

    // 章节卷需要查询Book表获取
    {
        MyBasePtr->DEBUGLog("开始查询Book/Memo获取章节卷", true);
        MyJsonPtr->JsonstrToJson(BookMemo, TBook.getValueOfMemo());

        MyBasePtr->DEBUGLog("开始检查Book/Memo数据是否合法", true);
        std::map<string, MyJson::ColType> ColMap;
        ColMap["Chapter"] = MyJson::ColType::ARRAY;
        ColMap["ExtraChapter"] = MyJson::ColType::JSON;
        MyJsonPtr->checkMemberAndTypeInMap(BookMemo, RespJson, ColMap);
        MyBasePtr->DEBUGLog("Book/Memo数据合法", true);
        ////---- 这里有可能不合法导致跳出，需要捕获然后更新Upload的值
        ChapterJson = BookMemo["Chapter"];

        int vecPartSize = ChapterJson.size();
        Part_Num = vecPartSize - 1;
        MyBasePtr->DEBUGLog("查询Book/Memo获取章节卷成功", true);
    }

    // 章节数小于0 说明是番外 ////----待处理
    if (Chapter_Num < 0)
    {
    }

    // 检查章节是否已存在
    {
        MyBasePtr->DEBUGLog("开始检查图书章节是否已存在", true);
        Criteria BookID_cri(drogon_model::novel::Chapter::Cols::_Book_ID, CompareOperator::EQ, Book_ID);
        Criteria PartNum_cri(drogon_model::novel::Chapter::Cols::_Part_Num, CompareOperator::EQ, Part_Num);
        Criteria ChapterNum_cri(drogon_model::novel::Chapter::Cols::_Chapter_Num, CompareOperator::EQ, Chapter_Num);
        Criteria ChapterTitle_cri(drogon_model::novel::Chapter::Cols::_Title, CompareOperator::EQ, ChapterTitle);
        vector<drogon_model::novel::Chapter> vecChapter = ChapterMgr.findBy(BookID_cri && PartNum_cri && ChapterNum_cri && ChapterTitle_cri);
        // 如果不是新章节，则将Upload状态改为已处理且拒绝插入
        if (vecChapter.size() != 0)
        {
            RespJson["ErrorMsg"] = "要新增的图书章节已存在(BookID = " + to_string(vecChapter[0].getValueOfBookId()) + ",Part_Num = " + to_string(vecChapter[0].getValueOfPartNum()) + ",Chapter_Num = " + to_string(vecChapter[0].getValueOfChapterNum()) + ",Title = " + vecChapter[0].getValueOfTitle() + ")";
            MyBasePtr->DEBUGLog("开始更新Upload数据", true);
            NewUpload.setProcessor("System");
            NewUpload.setStatus("已拒绝");
            NewUpload.setIsmanage(true);
            UploadMemo["Explain"] = RespJson["ErrorMsg"].asString();
            NewUpload.setMemo(UploadMemo.toStyledString());
            NewUpload.setBookId(vecChapter[0].getValueOfBookId());
            int count = UploadMgr.update(NewUpload);
            MyBasePtr->DEBUGLog("更新的新数据为 : " + NewUpload.toJson().toStyledString(), true);
            // 章节已存在
            if (!count)
            {
                MyBasePtr->DEBUGLog(RespJson["ErrorMsg"].asString(), true);
                RespJson["ErrorMsg"] = "更新Upload数据失败";
                throw RespJson;
            }
            MyBasePtr->DEBUGLog("更新Upload数据完毕", true);
            throw RespJson;
        }
        MyBasePtr->DEBUGLog("要插入的图书章节不存在", true);
    }

    // 准备要创建的新章节的数据
    {
        MyBasePtr->DEBUGLog("开始准备新章节数据", true);
        NewChapter.setBookId(Book_ID);
        NewChapter.setChapterNum(Chapter_Num);
        NewChapter.setContent(Chapter_Content.toStyledString());
        Chapter_Memo["Status"] = "normal";
        NewChapter.setMemo(Chapter_Memo.toStyledString());
        NewChapter.setPartNum(Part_Num);
        NewChapter.setTitle(ChapterTitle);
        NewChapter.setUserId(UserID);
        NewChapter.setValid(true);
        MyBasePtr->DEBUGLog("准备新章节数据完成", true);
        MyBasePtr->DEBUGLog("准备新章节数据为 : " + NewChapter.toJson().toStyledString(), true);
    }

    // 准备插入章节数据
    try
    {
        MyBasePtr->DEBUGLog("准备插入新章节数据", true);
        ChapterMgr.insert(NewChapter);
        MyBasePtr->DEBUGLog("插入新章节数据完毕", true);
        MyBasePtr->DEBUGLog("插入的新数据为 : " + NewChapter.toJson().toStyledString(), true);

        // 章节插入后需要更新Book信息
        {
            MyBasePtr->DEBUGLog("开始更新Book数据", true);
            ChapterJson[Part_Num] = ChapterJson[Part_Num].asInt() + 1;
            BookMemo["Chapter"] = ChapterJson;
            TBook.setMemo(BookMemo.toStyledString());
            int count = BookMgr.update(TBook);
            MyBasePtr->DEBUGLog("更新的新数据为 : " + TBook.toJson().toStyledString(), true);
            if (!count)
            {
                RespJson["ErrorMsg"] = "更新Book数据失败";
                throw RespJson;
            }
            MyBasePtr->DEBUGLog("更新Book数据完毕", true);
        }

        // 章节创建后更新Upload信息
        {
            MyBasePtr->DEBUGLog("开始更新Upload数据", true);
            NewUpload.setProcessor("System");
            NewUpload.setStatus("已通过");
            NewUpload.setIsmanage(true);
            
            UploadMemo["Explain"] = "上传新章节成功";
            NewUpload.setMemo(UploadMemo.toStyledString());
            int count = UploadMgr.update(NewUpload);
            MyBasePtr->DEBUGLog("更新的新数据为 : " + NewUpload.toJson().toStyledString(), true);
            if (!count)
            {
                RespJson["ErrorMsg"] = "更新Upload数据失败";
                throw RespJson;
            }
            MyBasePtr->DEBUGLog("更新Upload数据完毕", true);
        }
    }
    // 新建章节失败
    catch (...)
    {
        // 章节创建失败更新Upload信息
        {
            MyBasePtr->DEBUGLog("新建章节失败", true);
            MyBasePtr->DEBUGLog("开始更新Upload数据", true);
            NewUpload.setProcessor("System");
            NewUpload.setStatus("数据异常");
            NewUpload.setIsmanage(false);
            UploadMemo["Explain"] = "上传新章节失败";
            NewUpload.setMemo(UploadMemo.toStyledString());
            int count = UploadMgr.update(NewUpload);
            MyBasePtr->DEBUGLog("更新的新数据为 : " + NewUpload.toJson().toStyledString(), true);
            if (!count)
            {
                RespJson["ErrorMsg"] = "更新Upload数据失败";
                throw RespJson;
            }
            MyBasePtr->DEBUGLog("更新Upload数据完毕", true);
        }
        RespJson["ErrorMsg"] = "System 新建章节失败";
        throw RespJson;
    }
}

// 用户上传已有的书的改正章节的接口
void MyDBService::User_Upload_Exist_Book_Update_Chapter(Json::Value &ReqJson, Json::Value &RespJson)
{
    auto MyJsonPtr = app().getPlugin<MyJson>();
    auto MyBasePtr = app().getPlugin<MyBase>();
    auto MyToolsPtr = app().getPlugin<MyTools>();
    MyBasePtr->DEBUGLog("User_Upload_Exist_Book_Update_Chapter::ReqJson: " + ReqJson.toStyledString(), true);

    auto dbclientPrt = drogon::app().getDbClient();
    Mapper<drogon_model::novel::User> UserMgr(dbclientPrt);
    Mapper<drogon_model::novel::Upload> UploadMgr(dbclientPrt);
    Mapper<drogon_model::novel::Book> BookMgr(dbclientPrt);
    Mapper<drogon_model::novel::Chapter> ChapterMgr(dbclientPrt);

    int UserID = ReqJson["User_ID"].asInt();
    int Book_ID = ReqJson["Book_ID"].asInt();
    string Book_Name = ReqJson["Book_Name"].asString();
    string Book_Author = ReqJson["Book_Author"].asString();
    Json::Value ContentJson = ReqJson["Content"];
    Json::Value UploadMemo;
    drogon_model::novel::Book TBook; // 目标图书

    // 先判断传入数据的Book_Name和Book_Author是否存在 若存在则判断目标图书ID是否正确
    {
        MyBasePtr->DEBUGLog("开始检查上传新章节的图书是否已存在", true);
        Criteria BookName_cri(drogon_model::novel::Book::Cols::_Name, CompareOperator::EQ, Book_Name);
        Criteria BookAuthor_cri(drogon_model::novel::Book::Cols::_Author, CompareOperator::EQ, Book_Author);
        vector<drogon_model::novel::Book> vecBook = BookMgr.findBy(BookName_cri && BookAuthor_cri);
        // 如果不是已存在的书，则拒绝插入Upload
        if (vecBook.size() == 0)
        {
            RespJson["ErrorMsg"] = "要上传新章节的图书不存在";
            throw RespJson;
        }
        MyBasePtr->DEBUGLog("要上传新章节的图书存在", true);
        TBook = vecBook[0];
        MyBasePtr->DEBUGLog("开始判断目标图书ID是否正确", true);
        if (TBook.getValueOfBookId() != Book_ID)
        {
            RespJson["ErrorMsg"] = "要上传新章节的图书的ID与指定ID不符合,(Book_ID=" + to_string(Book_ID) + ",目标图书的ID=" + to_string(TBook.getValueOfBookId());
            throw RespJson;
        }
        MyBasePtr->DEBUGLog("目标图书ID正确", true);
    }

    // 插入前检查content数据是否合法
    {
        MyBasePtr->DEBUGLog("开始检查content数据是否合法", true);
        // "Chapter": "",
        // "ContentArray":[]
        // 创建检查列表 key是字段名 value 是字段类型

        std::map<string, MyJson::ColType> ColMap;
        ColMap["ChapterTitle"] = MyJson::ColType::STRING;
        ColMap["ContentArray"] = MyJson::ColType::ARRAY;
        MyJsonPtr->checkMemberAndTypeInMap(ContentJson, RespJson, ColMap);
        MyBasePtr->DEBUGLog("检查content数据合法", true);
    }

    MyBasePtr->DEBUGLog("开始查询用户 : " + to_string(UserID), true);
    drogon_model::novel::User user = UserMgr.findByPrimaryKey(UserID);
    MyBasePtr->DEBUGLog("用户查询完毕 : " + user.toJson().toStyledString(), true);

    // 验证用户
    if (user.getValueOfUserId() != UserID)
    {
        RespJson["ErrorMsg"] = "登入的UserID(" + to_string(UserID) + ")与查询到的UserID(" + to_string(user.getValueOfUserId()) + ")不同";
        throw RespJson;
    }

    drogon_model::novel::Upload NewUpload;
    // 准备要处理的新数据
    {
        MyBasePtr->DEBUGLog("开始准备要处理的新数据", true);
        NewUpload.setBookId(Book_ID);
        NewUpload.setContent(ContentJson.toStyledString());
        NewUpload.setUserId(UserID);
        NewUpload.setIsmanage(false);
        NewUpload.setStatus("待处理");
        UploadMemo["Explain"] = "待处理";
        NewUpload.setMemo(UploadMemo.toStyledString());
        MyBasePtr->DEBUGLog("要处理的新数据准备完成", true);
    }
    MyBasePtr->DEBUGLog("要处理的新数据为 : " + NewUpload.toJson().toStyledString(), true);
    MyBasePtr->DEBUGLog("准备插入新数据", true);
    UploadMgr.insert(NewUpload);
    MyBasePtr->DEBUGLog("插入新数据完毕", true);
    MyBasePtr->DEBUGLog("插入的新数据为 : " + NewUpload.toJson().toStyledString(), true);

    drogon_model::novel::Chapter NewChapter;
    pair<int, string> Title = MyToolsPtr->getTitleNumAndTitleStr(ContentJson["ChapterTitle"].asString());
    string ChapterTitle = Title.second;                        // 章节名
    int Part_Num;                                              // 章节卷
    int Chapter_Num = Title.first;                             // 章节数
    Json::Value Chapter_Content = ContentJson["ContentArray"]; // 章节内容
    Json::Value Chapter_Memo;                                  // 参数
    Json::Value BookMemo;
    Json::Value ChapterJson; //  Memo/Chapter

    // 章节卷需要查询Book表获取
    {
        MyBasePtr->DEBUGLog("开始查询Book/Memo获取章节卷", true);
        MyJsonPtr->JsonstrToJson(BookMemo, TBook.getValueOfMemo());

        MyBasePtr->DEBUGLog("开始检查Book/Memo数据是否合法", true);
        std::map<string, MyJson::ColType> ColMap;
        ColMap["Chapter"] = MyJson::ColType::ARRAY;
        ColMap["ExtraChapter"] = MyJson::ColType::JSON;
        MyJsonPtr->checkMemberAndTypeInMap(BookMemo, RespJson, ColMap);
        MyBasePtr->DEBUGLog("Book/Memo数据合法", true);
        ////---- 这里有可能不合法导致跳出，需要捕获然后更新Upload的值
        ChapterJson = BookMemo["Chapter"];

        int vecPartSize = ChapterJson.size();
        Part_Num = vecPartSize - 1;
        MyBasePtr->DEBUGLog("查询Book/Memo获取章节卷成功", true);
    }

    // 章节数小于0 说明是番外 ////----待处理
    if (Chapter_Num < 0)
    {
    }

    // 检查章节是否不存在,存在则等待管理员处理，不存在则直接拒绝
    {
        MyBasePtr->DEBUGLog("开始检查图书章节是否不存在", true);
        Criteria BookID_cri(drogon_model::novel::Chapter::Cols::_Book_ID, CompareOperator::EQ, Book_ID);
        Criteria PartNum_cri(drogon_model::novel::Chapter::Cols::_Part_Num, CompareOperator::EQ, Part_Num);
        Criteria ChapterNum_cri(drogon_model::novel::Chapter::Cols::_Chapter_Num, CompareOperator::EQ, Chapter_Num);
        Criteria ChapterTitle_cri(drogon_model::novel::Chapter::Cols::_Title, CompareOperator::EQ, ChapterTitle);
        vector<drogon_model::novel::Chapter> vecChapter = ChapterMgr.findBy(BookID_cri && PartNum_cri && ChapterNum_cri && ChapterTitle_cri);
        // 如果不是已存在的章节，则将Upload状态改为已处理且拒绝插入
        if (vecChapter.size() == 0)
        {
            RespJson["ErrorMsg"] = "要改正的图书章节不存在(BookID = " + to_string(Book_ID) + ",Part_Num = " + to_string(Part_Num) + ",Chapter_Num = " + to_string(Chapter_Num) + ",Title = " + ChapterTitle + "),请用正确的接口新增此章节";
            MyBasePtr->DEBUGLog("开始更新Upload数据", true);
            NewUpload.setProcessor("System");
            NewUpload.setStatus("已拒绝");
            NewUpload.setIsmanage(true);
            UploadMemo["Explain"] = RespJson["ErrorMsg"].asString();
            NewUpload.setMemo(UploadMemo.toStyledString());
            NewUpload.setBookId(vecChapter[0].getValueOfBookId());
            int count = UploadMgr.update(NewUpload);
            MyBasePtr->DEBUGLog("更新的新数据为 : " + NewUpload.toJson().toStyledString(), true);
            // 章节已存在
            if (!count)
            {
                MyBasePtr->DEBUGLog(RespJson["ErrorMsg"].asString(), true);
                RespJson["ErrorMsg"] = "更新Upload数据失败";
                throw RespJson;
            }
            MyBasePtr->DEBUGLog("更新Upload数据完毕", true);
            throw RespJson;
        }
        MyBasePtr->DEBUGLog("要改正的图书章节存在", true);
    }

    RespJson["ErrorMsg"] = "等待管理员审核";
    throw RespJson;
}

// 用户下载已有的书的章节
void MyDBService::Download_Resource(Json::Value &ReqJson, Json::Value &RespJson)
{
    Json::Value ParaJson;
    auto MyJsonPtr = app().getPlugin<MyJson>();
    auto MyBasePtr = app().getPlugin<MyBase>();
    auto MyToolsPtr = app().getPlugin<MyTools>();
    MyBasePtr->DEBUGLog("Download_Resource::ReqJson: " + ReqJson.toStyledString(), true);

    auto dbclientPrt = drogon::app().getDbClient();
    Mapper<drogon_model::novel::User> UserMgr(dbclientPrt);
    Mapper<drogon_model::novel::Upload> UploadMgr(dbclientPrt);
    Mapper<drogon_model::novel::Book> BookMgr(dbclientPrt);
    Mapper<drogon_model::novel::Chapter> ChapterMgr(dbclientPrt);

    int UserID,Book_ID;
    string LoginStatus;
    // 检查ReqJson数据
    {
        MyBasePtr->DEBUGLog("开始检查ReqJson数据", true);
        // "Book_ID":0,
        // "Chapter_Num":[],
        // "Para" : {}
        // 创建检查列表 key是字段名 value 是字段类型
        std::map<string, MyJson::ColType> ColMap;
        ColMap["Para"] = MyJson::ColType::JSON;
        ColMap["Book_ID"] = MyJson::ColType::INT;
        ColMap["Chapter_Num"] = MyJson::ColType::ARRAY;
        MyJsonPtr->checkMemberAndTypeInMap(ReqJson, RespJson, ColMap);
        MyBasePtr->DEBUGLog("检查ReqJson数据完成", true);

        MyBasePtr->DEBUGLog("开始检查Para数据", true);
        ColMap.clear();
        ParaJson = ReqJson["Para"];
        ColMap["User_ID"] = MyJson::ColType::STRING;
        ColMap["Login_Status"] = MyJson::ColType::STRING;
        MyJsonPtr->checkMemberAndTypeInMap(ParaJson, RespJson, ColMap);
        MyBasePtr->DEBUGLog("检查Para数据完成", true);

        UserID = std::atoi(ParaJson["User_ID"].asString().c_str());
        LoginStatus = ParaJson["Login_Status"].asString();
        if((LoginStatus!="user")&&(LoginStatus!="admin")&&(LoginStatus!="root"))
        {
            RespJson["ErrorMsg"] = "账户权限错误,请联系管理员";
            throw RespJson;
        }
        Book_ID     = ReqJson["Book_ID"].asInt();
    }


    vector<int> vecChapter_Num;
    drogon_model::novel::Book TBook; // 目标图书
    Json::Value TBookMemo;
    Json::Value TBookMemoChapter;      //  Memo/Chapter
    Json::Value TBookMemoExtraChapter; //  Memo/ExtraChapter

    MyBasePtr->DEBUGLog("开始查询用户 : " + to_string(UserID), true);
    drogon_model::novel::User user = UserMgr.findByPrimaryKey(UserID);
    MyBasePtr->DEBUGLog("用户查询完毕 : " + user.toJson().toStyledString(), true);

    // 验证用户
    if (user.getValueOfUserId() != UserID)
    {
        RespJson["ErrorMsg"] = "登入的UserID(" + to_string(UserID) + ")与查询到的UserID(" + to_string(user.getValueOfUserId()) + ")不同";
        throw RespJson;
    }

    // 先判断传入数据的Book_ID对应的图书是否存在
    {
        MyBasePtr->DEBUGLog("开始检查要下载的图书是否已存在", true);
        Criteria BookID_cri(drogon_model::novel::Book::Cols::_Book_ID, CompareOperator::EQ, Book_ID);
        vector<drogon_model::novel::Book> vecBook = BookMgr.findBy(BookID_cri);
        // 如果不是已存在的书，则拒绝插入Upload
        if (vecBook.size() == 0)
        {
            RespJson["ErrorMsg"] = "要下载的图书不存在";
            throw RespJson;
        }
        MyBasePtr->DEBUGLog("要下载的图书存在", true);
        TBook = vecBook[0];
    }

    // 检查Book/Memo数据是否合法
    {
        MyBasePtr->DEBUGLog("开始检查Book/Memo数据是否合法", true);
        MyJsonPtr->JsonstrToJson(TBookMemo, TBook.getValueOfMemo());
        std::map<string, MyJson::ColType> ColMap;
        ColMap["Chapter"] = MyJson::ColType::ARRAY;
        ColMap["ExtraChapter"] = MyJson::ColType::JSON;
        MyJsonPtr->checkMemberAndTypeInMap(TBookMemo, RespJson, ColMap);
        MyBasePtr->DEBUGLog("Book/Memo数据合法", true);

        TBookMemoChapter = TBookMemo["Chapter"];
        TBookMemoExtraChapter = TBookMemo["ExtraChapter"];
    }

    // 保存图书信息
    {
        Json::Value TBookJson;
        Json::Value RespBook;
        TBookJson = TBook.toJson();
        RespBook["Book_ID"] = TBookJson["Book_ID"].asInt();
        RespBook["Book_Name"] = TBookJson["Name"].asString();
        RespBook["Book_Status"] = TBookJson["Status"].asString();
        RespBook["Book_Synopsis"] = TBookJson["Synopsis"].asString();
        RespBook["Book_Publisher"] = TBookJson["Publisher"].asString();
        RespBook["Book_Author"] = TBookJson["Author"].asString();
        RespBook["Book_Create_time"] = TBookJson["Create_time"].asString();
        RespBook["Book_Update_time"] = TBookJson["Update_time"].asString();

        int MemoChapterSize = TBookMemoChapter.size();
        RespBook["Book_Memo"]["正文"]["章节总数量"] = TBookMemoChapter[0].asInt();
        for (int i = 1; i < MemoChapterSize; i++)
        {
            RespBook["Book_Memo"]["正文"]["第" + to_string(i) + "卷章节数"] = TBookMemoChapter[i].asInt();
        }
        RespBook["Book_Memo"]["番外"]["番外起始编号"] = TBookMemoExtraChapter["Index"];
        RespBook["Book_Memo"]["番外"]["番外数量"] = TBookMemoExtraChapter["Num"];
        RespJson["Book_Data"] = RespBook;
    }

    // 获取要下载的图书的章节ID
    {

        MyBasePtr->DEBUGLog("开始获取要下载的图书的章节ID", true);
        int ChapterSize = ReqJson["Chapter_Num"].size();
        for (int i = 0; i < ChapterSize; i++)
        {
            vecChapter_Num.push_back(ReqJson["Chapter_Num"][i].asInt());
        }
        MyBasePtr->DEBUGLog("要下载的图书的章节ID获取完成", true);
    }

    // 下载并保存章节信息
    {
        MyBasePtr->DEBUGLog("开始获取指定的图书的章节数据", true);
        Criteria ChapterNum_cri(drogon_model::novel::Chapter::Cols::_Chapter_Num, CompareOperator::In, vecChapter_Num);
        Criteria Valid_cri(drogon_model::novel::Chapter::Cols::_Valid, CompareOperator::EQ, true);
        MyBasePtr->DEBUGLog("使用的SQL语句为 " + ChapterNum_cri.criteriaString(), true);
        vector<drogon_model::novel::Chapter> vecChapter = ChapterMgr.findBy(ChapterNum_cri && Valid_cri);
        int ChapterSize = vecChapter.size();
        if (ChapterSize == 0)
        {
            RespJson["ErrorMsg"] = "要下载的图书的章节都不存在";
            throw RespJson;
        }
        Json::Value TChapterArray;
        Json::Value TempChapter, TempChapterJsonStr;
        for (int i = 0; i < ChapterSize; i++)
        {
            TempChapter.clear();
            TempChapterJsonStr.clear();
            MyJsonPtr->JsonstrToJson(TempChapterJsonStr, vecChapter[i].getValueOfContent());
            TempChapter["Chapter_ID"] = vecChapter[i].getValueOfChapterId();
            TempChapter["Book_ID"] = vecChapter[i].getValueOfBookId();
            TempChapter["Part_Num"] = vecChapter[i].getValueOfPartNum();
            TempChapter["Chapter_Num"] = vecChapter[i].getValueOfChapterNum();
            TempChapter["Title"] = vecChapter[i].getValueOfTitle();
            TempChapter["User_ID"] = vecChapter[i].getValueOfUserId();
            TempChapter["Content"] = TempChapterJsonStr;
            MyJsonPtr->JsonstrToJson(TempChapterJsonStr, vecChapter[i].getValueOfMemo());
            TempChapter["Memo"] = TempChapterJsonStr;
            TChapterArray.append(TempChapter);
        }
        RespJson["Chapter_Data"] = TChapterArray;
        MyBasePtr->DEBUGLog("获取指定的图书的章节数据完成", true);
    }
}

// 用户查询用户个人资料的接口
void MyDBService::Search_User_PersonalData(Json::Value &ReqJson, Json::Value &RespJson)
{
    auto MyBasePtr = app().getPlugin<MyBase>();
    auto MyJsonPtr = app().getPlugin<MyJson>();
    MyBasePtr->DEBUGLog("Search_User_PersonalData::ReqJson: " + ReqJson.toStyledString(), true);

    auto dbclientPrt = drogon::app().getDbClient();
    Mapper<drogon_model::novel::User> UserMgr(dbclientPrt);

    int UserID;
    string LoginStatus;
    Json::Value ParaJson;
    // 检查ReqJson数据是否合法
    {
        MyBasePtr->DEBUGLog("开始检查ReqJson数据是否合法", true);
        // "Para"   :   {"User_ID":"","Login_Status":""},
        std::map<string, MyJson::ColType> ColMap;
        ColMap["Para"] = MyJson::ColType::JSON;
        MyJsonPtr->checkMemberAndTypeInMap(ReqJson, RespJson, ColMap);
        MyBasePtr->DEBUGLog("ReqJson数据合法", true);

        MyBasePtr->DEBUGLog("开始检查Para数据", true);
        ColMap.clear();
        ParaJson = ReqJson["Para"];
        ColMap["User_ID"] = MyJson::ColType::STRING;
        ColMap["Login_Status"] = MyJson::ColType::STRING;
        MyJsonPtr->checkMemberAndTypeInMap(ParaJson, RespJson, ColMap);
        MyBasePtr->DEBUGLog("检查Para数据完成", true);

        UserID = std::atoi(ParaJson["User_ID"].asString().c_str());
        LoginStatus = ParaJson["Login_Status"].asString();
        if((LoginStatus!="user")&&(LoginStatus!="admin")&&(LoginStatus!="root"))
        {
            RespJson["ErrorMsg"] = "账户权限错误,请联系管理员";
            throw RespJson;
        }
    }

    MyBasePtr->DEBUGLog("开始查询用户 : " + ParaJson["User_ID"].asString(), true);
    drogon_model::novel::User user = UserMgr.findByPrimaryKey(UserID);
    MyBasePtr->DEBUGLog("用户查询完毕 : " + user.toJson().toStyledString(), true);

    RespJson["User_Data"] = user.toJson();
}

// 更改用户资料的接口
void MyDBService::Update_User_PersonalData(Json::Value &ReqJson, Json::Value &RespJson)
{
    Json::Value ParaJson;
    auto MyBasePtr = app().getPlugin<MyBase>();
    auto MyJsonPtr = app().getPlugin<MyJson>();
    MyBasePtr->DEBUGLog("Update_User_PersonalData::ReqJson: " + ReqJson.toStyledString(), true);

    auto dbclientPrt = drogon::app().getDbClient();
    Mapper<drogon_model::novel::User> UserMgr(dbclientPrt);

    // 检查ReqJson数据是否合法
    {
        MyBasePtr->DEBUGLog("开始检查ReqJson数据是否合法", true);
        // "Para"    :   {},
        // "Change_ID"  :   0,被执行者
        // "Change_Name"    :   "",
        // "Change_Password":   "",
        // "Change_Sex"     :   "",
        // "Change_Col"     :   ["Change_Name","Change_Password","Change_Sex"]
                    
        std::map<string, MyJson::ColType> ColMap;
        ColMap["Para"] = MyJson::ColType::JSON;
        ColMap["Change_ID"] = MyJson::ColType::INT;
        ColMap["Change_Name"] = MyJson::ColType::STRING;
        ColMap["Change_Password"] = MyJson::ColType::STRING;
        ColMap["Change_Sex"] = MyJson::ColType::STRING;
        ColMap["Change_Col"] = MyJson::ColType::ARRAY;
        MyJsonPtr->checkMemberAndTypeInMap(ReqJson, RespJson, ColMap);
        MyBasePtr->DEBUGLog("ReqJson数据合法", true);

        MyBasePtr->DEBUGLog("开始检查Para数据是否合法", true);
        ColMap.clear();
        ParaJson = ReqJson["Para"];
        ColMap["User_ID"] = MyJson::ColType::STRING;
        ColMap["Login_Status"] = MyJson::ColType::STRING;
        MyJsonPtr->checkMemberAndTypeInMap(ParaJson, RespJson, ColMap);
        MyBasePtr->DEBUGLog("Para数据合法", true);

        MyBasePtr->DEBUGLog("开始检查操作权限", true);
        string LoginStatus = ParaJson["Login_Status"].asString();
        if (LoginStatus != "admin" && LoginStatus != "root")
        {
            RespJson["ErrorMsg"] = "权限不足，请联系管理员";
            throw RespJson;
        }
        MyBasePtr->DEBUGLog("操作权限检测通过", true);

    }

    // 检查是否修改的是自己的资料
    {
        if (ReqJson["Change_ID"].asInt() != ParaJson["User_ID"].asInt())
        {
            RespJson["ErrorMsg"] = "权限不足，请联系管理员(操作用户: " 
                        + to_string(ParaJson["User_ID"].asInt()) 
                        + ",被操作用户: " 
                        + to_string(ReqJson["Change_ID"].asInt()) + ")";
            throw RespJson;
        }
    }

    // Name  Password  Sex  用户有权修改这三项
    MyBasePtr->DEBUGLog("开始查询用户 : " + to_string(ReqJson["Change_ID"].asInt()), true);
    drogon_model::novel::User user = UserMgr.findByPrimaryKey(ReqJson["Change_ID"].asInt());
    MyBasePtr->DEBUGLog("用户查询完毕 : " + user.toJson().toStyledString(), true);

    MyBasePtr->DEBUGLog("开始修改用户数据", true);
    Json::Value Change_Col = ReqJson["Change_Col"];
    int Change_size = Change_Col.size(), Change_count = 0;
    for (int i = 0; i < Change_size; i++)
    {
        MyBasePtr->DEBUGLog("开始修改用户数据(" + Change_Col[i].asString() + ")", true);
        if (Change_Col[i].asString() == "Change_Name")
        {
            if (user.getValueOfName() == ReqJson["Change_Name"].asString())
            {
                MyBasePtr->DEBUGLog("字段(" + Change_Col[i].asString() + ")没有产生变化", true);
                continue;
            }
            user.setName(ReqJson["Change_Name"].asString());
            Change_count++;
        }
        else if (Change_Col[i].asString() == "Change_Password")
        {
            if (user.getValueOfPassword() == ReqJson["Change_Password"].asString())
            {
                MyBasePtr->DEBUGLog("字段(" + Change_Col[i].asString() + ")没有产生变化", true);
                continue;
            }
            user.setPassword(ReqJson["Change_Password"].asString());
            Change_count++;
        }
        else if (Change_Col[i].asString() == "Change_Sex")
        {
            if (user.getValueOfSex() == ReqJson["Change_Sex"].asString())
            {
                MyBasePtr->DEBUGLog("字段(" + Change_Col[i].asString() + ")没有产生变化", true);
                continue;
            }
            user.setSex(ReqJson["Change_Sex"].asString());
            Change_count++;
        }
        else
        {
            MyBasePtr->DEBUGLog("Change_Col包含错误字段(" + Change_Col[i].asString() + ")", true);
        }
    }
    MyBasePtr->DEBUGLog("用户数据修改完成", true);

    // 统计更新的字段数
    if (Change_count == 0)
    {
        RespJson["ErrorMsg"] = "用户数据无需更新";
        return;
    }

    MyBasePtr->DEBUGLog("开始更新用户数据", true);
    int row = UserMgr.update(user);
    if (row != 1)
    {
        RespJson["ErrorMsg"] = "用户资料更新失败";
        throw RespJson;
    }
    MyBasePtr->DEBUGLog("用户数据更新完成: " + user.toJson().toStyledString(), true);
}

// 管理员查询用户数据
void MyDBService::Admin_Search_User(Json::Value &ReqJson, Json::Value &RespJson)
{
    auto MyBasePtr = app().getPlugin<MyBase>();
    auto MyJsonPtr = app().getPlugin<MyJson>();
    MyBasePtr->DEBUGLog("Admin_Search_User::ReqJson: " + ReqJson.toStyledString(), true);

    auto dbclientPrt = drogon::app().getDbClient();
    Mapper<drogon_model::novel::User> UserMgr(dbclientPrt);

    Json::Value FilterJson, ParaJson;
    // 检查ReqJson数据是否合法
    {
        MyBasePtr->DEBUGLog("开始检查ReqJson数据是否合法", true);
        // "Search_All"   :   true/false,
        // "Filter":{"User_ID":0,"User_Name":"","User_Sex":""}
        // "Para"   : {"User_ID":"","Login_Status":""}
        std::map<string, MyJson::ColType> ColMap;
        ColMap["Search_All"] = MyJson::ColType::BOOL;
        ColMap["Filter"] = MyJson::ColType::JSON;
        ColMap["Para"] = MyJson::ColType::JSON;
        MyJsonPtr->checkMemberAndTypeInMap(ReqJson, RespJson, ColMap);
        MyBasePtr->DEBUGLog("ReqJson数据合法", true);

        MyBasePtr->DEBUGLog("开始检查Filter数据是否合法", true);
        ColMap.clear();
        FilterJson = ReqJson["Filter"];
        ColMap["User_ID"] = MyJson::ColType::INT;
        ColMap["User_Name"] = MyJson::ColType::STRING;
        ColMap["User_Sex"] = MyJson::ColType::STRING;
        MyJsonPtr->checkMemberAndTypeInMap(FilterJson, RespJson, ColMap);
        MyBasePtr->DEBUGLog("Filter数据合法", true);

        MyBasePtr->DEBUGLog("开始检查Para数据是否合法", true);
        ColMap.clear();
        ParaJson = ReqJson["Para"];
        ColMap["User_ID"] = MyJson::ColType::STRING;
        ColMap["Login_Status"] = MyJson::ColType::STRING;
        MyJsonPtr->checkMemberAndTypeInMap(ParaJson, RespJson, ColMap);
        MyBasePtr->DEBUGLog("Para数据合法", true);

        MyBasePtr->DEBUGLog("开始检查操作权限", true);
        string LoginStatus = ParaJson["Login_Status"].asString();
        if (LoginStatus != "admin" && LoginStatus != "root")
        {
            RespJson["ErrorMsg"] = "权限不足，请联系管理员";
            throw RespJson;
        }
        MyBasePtr->DEBUGLog("操作权限检测通过", true);
    }

    // 如果是查询所有
    if (ReqJson["Search_All"].asBool())
    {
        MyBasePtr->DEBUGLog("开始查询所有用户", true);
        std::vector<drogon_model::novel::User> vecUser = UserMgr.findAll();
        Json::Value vecJsonVal, TempUserJson;
        for (auto &user : vecUser)
        {
            TempUserJson.clear();
            TempUserJson = user.toJson();
            TempUserJson.removeMember("Password");
            vecJsonVal.append(TempUserJson);
            // std::cout << user.toJson().toStyledString() <<std::endl;
        }
        RespJson["UserList"] = vecJsonVal;
        MyBasePtr->DEBUGLog("查询所有用户完毕", true);
        return;
    }

    // 查询指定用户
    {
        // 制作筛选条件
        string UserID = FilterJson["User_ID"].asInt() > 0 ? FilterJson["User_ID"].asString(): "";
        Criteria UserID_cri = Criteria(drogon_model::novel::User::Cols::_User_ID, CompareOperator::Like, "%" + UserID + "%");
        Criteria UserName_cri(drogon_model::novel::User::Cols::_Name, CompareOperator::Like, "%" + FilterJson["User_Name"].asString() + "%");
        string UserSex = ((FilterJson["User_Sex"].asString() != "男")&&(FilterJson["User_Sex"].asString() != "女")) ? "" : FilterJson["User_Sex"].asString();
        Criteria UserSex_cri(drogon_model::novel::User::Cols::_Sex, CompareOperator::Like, "%" + UserSex + "%");

        MyBasePtr->DEBUGLog("开始查询指定用户", true);
        std::vector<drogon_model::novel::User> vecUser = UserMgr.findBy(UserID_cri && UserName_cri && UserSex_cri);
        Json::Value vecJsonVal, TempUserJson;
        for (auto &user : vecUser)
        {
            TempUserJson.clear();
            TempUserJson = user.toJson();
            TempUserJson.removeMember("Password");
            vecJsonVal.append(TempUserJson);
            // std::cout << user.toJson().toStyledString() <<std::endl;
        }
        RespJson["UserList"] = vecJsonVal;
        MyBasePtr->DEBUGLog("查询指定用户完毕", true);
    }
}

// 管理员修改用户数据
void MyDBService::Admin_Update_User(Json::Value &ReqJson, Json::Value &RespJson)
{
    Json::Value ParaJson,ChangeTargetArray;
    auto MyJsonPtr = app().getPlugin<MyJson>();
    auto MyBasePtr = app().getPlugin<MyBase>();
    auto MyToolsPtr = app().getPlugin<MyTools>();
    MyBasePtr->DEBUGLog("Admin_Update_User::ReqJson: " + ReqJson.toStyledString(), true);

    auto dbclientPrt = drogon::app().getDbClient();
    Mapper<drogon_model::novel::User> UserMgr(dbclientPrt);
    Mapper<drogon_model::novel::Upload> UploadMgr(dbclientPrt);
    Mapper<drogon_model::novel::Book> BookMgr(dbclientPrt);
    drogon_model::novel::User user;

    int ChangeSize,ChangeColSize,ChangeCount,ChangeIntegralNum;
    // 检查ReqJson数据是否合法
    {
        MyBasePtr->DEBUGLog("开始检查ReqJson数据是否合法", true);
        // "Change_Target"  :   [{},{}],被执行者们
        // "Para"           :   {"User_ID":"","Login_Status":""} 执行者
        std::map<string, MyJson::ColType> ColMap;
        ColMap["Para"] = MyJson::ColType::JSON;
        ColMap["Change_Target"] = MyJson::ColType::ARRAY;
        MyJsonPtr->checkMemberAndTypeInMap(ReqJson, RespJson, ColMap);
        MyBasePtr->DEBUGLog("ReqJson数据合法", true);

        MyBasePtr->DEBUGLog("开始检查Para数据是否合法", true);
        ColMap.clear();
        ParaJson = ReqJson["Para"];
        ColMap["User_ID"] = MyJson::ColType::STRING;
        ColMap["Login_Status"] = MyJson::ColType::STRING;
        MyJsonPtr->checkMemberAndTypeInMap(ParaJson, RespJson, ColMap);
        MyBasePtr->DEBUGLog("Para数据合法", true);

        MyBasePtr->DEBUGLog("开始检查操作权限", true);
        string LoginStatus = ParaJson["Login_Status"].asString();
        if (LoginStatus != "admin" && LoginStatus != "root")
        {
            RespJson["ErrorMsg"] = "权限不足，请联系管理员";
            throw RespJson;
        }
        MyBasePtr->DEBUGLog("操作权限检测通过", true);

        
        MyBasePtr->DEBUGLog("开始检查被修改者们的数据是否合法", true);
        ChangeTargetArray = ReqJson["Change_Target"];
        ChangeSize = ChangeTargetArray.size();
        MyBasePtr->DEBUGLog("要修改的对象一共有"+ to_string(ChangeSize) +"位", true);
        Json::Value TempReqJson,TempRespJson;

        
        ColMap.clear();
        // "Change_ID"              :   0 ,
        // "Change_Name"            :   "",
        // "Change_Password"        :   "",
        // "Change_Sex"             :   "",
        // "Change_Integral_Type"   :   "Add/Sub",
        // "Change_Integral_Num"    :   0
        // "Change_Col"             :   ["Change_Name","Change_Password","Change_Sex","Change_Integral"]
        ColMap["Change_ID"] = MyJson::ColType::INT;
        ColMap["Change_Col"] = MyJson::ColType::ARRAY;
        ColMap["Change_Sex"] = MyJson::ColType::STRING;
        ColMap["Change_Name"] = MyJson::ColType::STRING;
        ColMap["Change_Password"] = MyJson::ColType::STRING;
        ColMap["Change_Integral_Num"] = MyJson::ColType::INT;
        ColMap["Change_Integral_Type"] = MyJson::ColType::STRING;

        for(int i = 0; i < ChangeSize; i++)
        {
            TempReqJson.clear();
            TempReqJson = ChangeTargetArray[i];
            TempRespJson.clear();
            try{
                MyBasePtr->DEBUGLog("开始检查第"+to_string(i)+"位修改者的数据是否合法", true);
                MyJsonPtr->checkMemberAndTypeInMap(TempReqJson, TempRespJson, ColMap);
                MyBasePtr->DEBUGLog("第"+to_string(i)+"位修改者的数据合法", true);
            }
            catch(Json::Value &TempRespJson)
            {
                ChangeTargetArray[i]["Result"] = false;
                ChangeTargetArray[i]["ErrorMsg"] = "用户数据不合法::" + TempRespJson["ErrorMsg"].asString();
                MyBasePtr->DEBUGLog(ChangeTargetArray[i]["ErrorMsg"].asString(), true);
                continue;
            }
        }
    }

    for(int i = 0;i < ChangeSize; i++)
    {
        try
        {
            /* code */
        
            if(ChangeTargetArray[i].isMember("Result"))
            {
                MyBasePtr->DEBUGLog("第"+to_string(i)+"位修改者的数据不合法,跳过", true);
                continue;
            }
            MyBasePtr->DEBUGLog("第"+to_string(i)+"位修改者的数据合法,开始准备修改", true);

            MyBasePtr->DEBUGLog("开始查询用户 : " + to_string(ChangeTargetArray[i]["Change_ID"].asInt()), true);
            user = UserMgr.findByPrimaryKey(ChangeTargetArray[i]["Change_ID"].asInt());
            MyBasePtr->DEBUGLog("用户查询完毕 : " + user.toJson().toStyledString(), true);

            ChangeCount = 0;
            ChangeColSize = ChangeTargetArray[i]["Change_Col"].size();
            string ChangeColName,ChangeIntegralType;
            for (int j = 0; j < ChangeColSize; j++)
            {
                ChangeColName = ChangeTargetArray[i]["Change_Col"][j].asString();
                MyBasePtr->DEBUGLog("开始修改用户数据(" + ChangeColName + ")", true);
                if (ChangeColName == "Change_Name")
                {
                    if (user.getValueOfName() == ChangeTargetArray[i]["Change_Name"].asString())
                    {
                        MyBasePtr->DEBUGLog("字段(" + ChangeColName + ")没有产生变化", true);
                        continue;
                    }
                    user.setName(ChangeTargetArray[i]["Change_Name"].asString());
                    ChangeCount++;
                }
                else if (ChangeColName == "Change_Password")
                {
                    if (user.getValueOfPassword() == ChangeTargetArray[i]["Change_Password"].asString())
                    {
                        MyBasePtr->DEBUGLog("字段(" + ChangeColName + ")没有产生变化", true);
                        continue;
                    }
                    user.setPassword(ChangeTargetArray[i]["Change_Password"].asString());
                    ChangeCount++;
                }
                else if (ChangeColName == "Change_Sex")
                {
                    if (user.getValueOfSex() == ChangeTargetArray[i]["Change_Sex"].asString())
                    {
                        MyBasePtr->DEBUGLog("字段(" + ChangeColName + ")没有产生变化", true);
                        continue;
                    }
                    user.setSex(ChangeTargetArray[i]["Change_Sex"].asString());
                    ChangeCount++;
                }
                else if (ChangeColName == "Change_Integral")
                {
                    ChangeIntegralType = ChangeTargetArray[i]["Change_Integral_Type"].asString();
                    ChangeIntegralNum = ChangeTargetArray[i]["Change_Integral_Num"].asInt();
                    if(ChangeIntegralNum <= 0)
                    {
                        MyBasePtr->DEBUGLog("字段(" + ChangeColName + ")没有产生变化", true);
                        continue;
                    }
                    
                    if(ChangeIntegralType == "Sub")
                    {
                        ChangeIntegralNum = (-1) * ChangeIntegralNum;
                    }
                    else if(ChangeIntegralType != "Add")
                    {
                        MyBasePtr->DEBUGLog("字段Change_Integral(" + ChangeIntegralType + ")无效", true);
                        continue;
                    }
                    user.setIntegral(user.getValueOfIntegral() + ChangeIntegralNum);
                    user.setTotalIntegral(user.getValueOfTotalIntegral() + ChangeIntegralNum);
                    ChangeCount++;
                }
                else
                {
                    MyBasePtr->DEBUGLog("Change_Col包含错误字段(" + ChangeColName + ")", true);
                }
            }
            MyBasePtr->DEBUGLog("第"+to_string(i)+"位修改者的数据修改完成", true);

            // 统计更新的字段数
            MyBasePtr->DEBUGLog("统计第"+to_string(i)+"位修改者更新的字段数为" + to_string(ChangeCount), true);

            if (ChangeCount == 0)
            {
                MyBasePtr->DEBUGLog("第"+to_string(i)+"位修改者的数据无需更新", true);
                ChangeTargetArray[i]["ErrorMsg"] = "用户数据无需更新";
                ChangeTargetArray[i]["Result"] = false;
                continue;
            }

            MyBasePtr->DEBUGLog("开始更新第"+to_string(i)+"位修改者的数据", true);
            int row = UserMgr.update(user);
            if (row != 1)
            {
                ChangeTargetArray[i]["ErrorMsg"] = "用户资料更新失败";
                ChangeTargetArray[i]["Result"] = false;
                continue;
            }
            MyBasePtr->DEBUGLog("第"+to_string(i)+"位修改者的数据更新完成: " + user.toJson().toStyledString(), true);

            ChangeTargetArray[i]["Result"] = true;
        }
        catch(const drogon::orm::DrogonDbException &e)
        {
            if (e.base().what() == string("0 rows found"))
            {
                ChangeTargetArray[i]["ErrorMsg"] = "要更改的用户不存在";
            }
            else if (e.base().what() == string("Found more than one row"))
            {
                ChangeTargetArray[i]["ErrorMsg"] = "要更改的用户ID重复,请联系管理员";
            }
            else
            {
                ChangeTargetArray[i]["ErrorMsg"] = e.base().what();
            }

            MyBasePtr->DEBUGLog("ErrorMsg" + ChangeTargetArray[i]["ErrorMsg"].toStyledString(), true);
            ChangeTargetArray[i]["Result"] = false;
            continue;
        }
    }

    RespJson["Change_Target"] = ChangeTargetArray;
}


// 管理员修改图书资源数据
void MyDBService::Admin_Update_Resource(Json::Value &ReqJson, Json::Value &RespJson)
{
    Json::Value ParaJson,ChangeTargetArray;
    auto MyJsonPtr = app().getPlugin<MyJson>();
    auto MyBasePtr = app().getPlugin<MyBase>();
    auto MyToolsPtr = app().getPlugin<MyTools>();
    MyBasePtr->DEBUGLog("Admin_Update_Resource::ReqJson: " + ReqJson.toStyledString(), true);

    auto dbclientPrt = drogon::app().getDbClient();
    Mapper<drogon_model::novel::User> UserMgr(dbclientPrt);
    Mapper<drogon_model::novel::Upload> UploadMgr(dbclientPrt);
    Mapper<drogon_model::novel::Book> BookMgr(dbclientPrt);
    Mapper<drogon_model::novel::Chapter> ChapterMgr(dbclientPrt);
    drogon_model::novel::Book book;
    drogon_model::novel::Chapter chapter;
    
    std::map<string, MyJson::ColType> ColMap,BookColMap,ChapterColMap;
    int ChangeSize,ChangeColSize,ChangeCount,ChangeIntegralNum;
    // 检查ReqJson数据是否合法
    {
        MyBasePtr->DEBUGLog("开始检查ReqJson数据是否合法", true);
        // "Change_Target"  :   [{},{}],被执行者们
        // "Para"           :   {"User_ID":"","Login_Status":""} 执行者
        ColMap["Para"] = MyJson::ColType::JSON;
        ColMap["Change_Target"] = MyJson::ColType::ARRAY;
        MyJsonPtr->checkMemberAndTypeInMap(ReqJson, RespJson, ColMap);
        MyBasePtr->DEBUGLog("ReqJson数据合法", true);

        MyBasePtr->DEBUGLog("开始检查Para数据是否合法", true);
        ColMap.clear();
        ParaJson = ReqJson["Para"];
        ColMap["User_ID"] = MyJson::ColType::STRING;
        ColMap["Login_Status"] = MyJson::ColType::STRING;
        MyJsonPtr->checkMemberAndTypeInMap(ParaJson, RespJson, ColMap);
        MyBasePtr->DEBUGLog("Para数据合法", true);

        MyBasePtr->DEBUGLog("开始检查操作权限", true);
        string LoginStatus = ParaJson["Login_Status"].asString();
        if (LoginStatus != "admin" && LoginStatus != "root")
        {
            RespJson["ErrorMsg"] = "权限不足，请联系管理员";
            throw RespJson;
        }
        MyBasePtr->DEBUGLog("操作权限检测通过", true);

        
    }
        
        
    MyBasePtr->DEBUGLog("开始检查被修改的数据是否合法", true);
    ChangeTargetArray = ReqJson["Change_Target"];
    ChangeSize = ChangeTargetArray.size();
    MyBasePtr->DEBUGLog("要修改的对象一共有"+ to_string(ChangeSize) +"项", true);
    Json::Value TempReqJson,TempRespJson;

    // 设置数据校验格式
    {
        ColMap.clear();
        // "Change_Type"        :   "Book/Chapter" ,
        // "Change_Content"     :   {},
        ColMap["Change_Type"]   =   MyJson::ColType::STRING;
        ColMap["Change_Content"]=   MyJson::ColType::JSON;
        
        BookColMap.clear();
        // "Change_ID"          :   0,
        // "Change_Name"        :   "",
        // "Change_Status"      :   "",
        // "Change_Synopsis"    :   "",
        // "Change_Publisher"   :   "",
        // "Change_Author"      :   "",
        // "Change_Memo"        :   {}
        // "Change_Col"         :   ["Change_Name","Change_Status","Change_Synopsis","Change_Publisher","Change_Author","Change_Memo"]
        BookColMap["Change_ID"]         =   MyJson::ColType::INT;
        BookColMap["Change_Name"]       =   MyJson::ColType::STRING;
        BookColMap["Change_Status"]     =   MyJson::ColType::STRING;
        BookColMap["Change_Synopsis"]   =   MyJson::ColType::STRING;
        BookColMap["Change_Publisher"]  =   MyJson::ColType::STRING;
        BookColMap["Change_Author"]     =   MyJson::ColType::STRING;
        BookColMap["Change_Memo"]       =   MyJson::ColType::JSON;
        BookColMap["Change_Col"]        =   MyJson::ColType::ARRAY;


        ChapterColMap.clear();
        // "Change_ID"          :   0,
        // "Change_Book_ID"     :   0,
        // "Change_Part_Num"    :   0,
        // "Change_Chapter_Num" :   0,
        // "Change_Title"       :   "",
        // "Change_User_ID"     :   0,
        // "Change_Valid"       :   true/false,
        // "Change_Content"     :   [],
        // "Change_Memo"        :   {}
        // "Change_Col"         :   ["Change_Name","Change_Status","Change_Synopsis","Change_Publisher","Change_Author","Change_Memo"]
        ChapterColMap["Change_ID"]          =   MyJson::ColType::INT;
        ChapterColMap["Change_Book_ID"]     =   MyJson::ColType::INT;
        ChapterColMap["Change_Part_Num"]    =   MyJson::ColType::INT;
        ChapterColMap["Change_Chapter_Num"] =   MyJson::ColType::INT;
        ChapterColMap["Change_Title"]       =   MyJson::ColType::STRING;
        ChapterColMap["Change_User_ID"]     =   MyJson::ColType::INT;
        ChapterColMap["Change_Valid"]       =   MyJson::ColType::BOOL;
        ChapterColMap["Change_Content"]     =   MyJson::ColType::ARRAY;
        ChapterColMap["Change_Memo"]        =   MyJson::ColType::JSON;
        ChapterColMap["Change_Col"]         =   MyJson::ColType::ARRAY;

    }
    
    for(int i = 0; i < ChangeSize; i++)
    {
        TempReqJson.clear();
        TempReqJson = ChangeTargetArray[i];
        TempRespJson.clear();
        try{
            MyBasePtr->DEBUGLog("开始检查第"+to_string(i)+"项的数据结构是否合法", true);
            MyJsonPtr->checkMemberAndTypeInMap(TempReqJson, TempRespJson, ColMap);
            MyBasePtr->DEBUGLog("第"+to_string(i)+"项的数据结构合法", true);
            if(TempReqJson["Change_Type"].asString() == "Book")
            {
                MyBasePtr->DEBUGLog("开始检查第"+to_string(i)+"项的(图书)数据是否合法", true);
                TempReqJson.clear();
                TempReqJson = ChangeTargetArray[i]["Change_Content"];
                MyJsonPtr->checkMemberAndTypeInMap(TempReqJson, TempRespJson, BookColMap);
                MyBasePtr->DEBUGLog("第"+to_string(i)+"项的(章节)数据合法", true);
            }
            else if(TempReqJson["Change_Type"].asString() == "Chapter")
            {
                MyBasePtr->DEBUGLog("开始检查第"+to_string(i)+"项的(章节)数据是否合法", true);
                TempReqJson.clear();
                TempReqJson = ChangeTargetArray[i]["Change_Content"];
                MyJsonPtr->checkMemberAndTypeInMap(TempReqJson, TempRespJson, ChapterColMap);
                MyBasePtr->DEBUGLog("第"+to_string(i)+"项的(章节)数据合法", true);
            }
            else
            {
                ChangeTargetArray[i]["Result"] = false;
                ChangeTargetArray[i]["ErrorMsg"] = "资源类型数据不合法("+TempReqJson["Change_Type"].asString()+")";
                continue;
            }
            MyBasePtr->DEBUGLog("第"+to_string(i)+"项的数据合法", true);
        }
        catch(Json::Value &TempRespJson)
        {
            ChangeTargetArray[i]["Result"] = false;
            ChangeTargetArray[i]["ErrorMsg"] = "资源数据不合法::" + TempRespJson["ErrorMsg"].asString();
            MyBasePtr->DEBUGLog(ChangeTargetArray[i]["ErrorMsg"].asString(), true);
            continue;
        }
    }

    for(int i = 0;i < ChangeSize; i++)
    {
        try
        {
            /* code */
        
            if(ChangeTargetArray[i].isMember("Result"))
            {
                MyBasePtr->DEBUGLog("第"+to_string(i)+"项的数据不合法,跳过", true);
                continue;
            }
            
            MyBasePtr->DEBUGLog("第"+to_string(i)+"项的数据合法,开始准备修改", true);

            string ChangeColName;
            ChangeCount = 0;
            ChangeColSize = ChangeTargetArray[i]["Change_Content"]["Change_Col"].size();
            if(ChangeTargetArray[i]["Change_Type"].asString() == "Book")
            {
                MyBasePtr->DEBUGLog("开始查询图书 : " + to_string(ChangeTargetArray[i]["Change_Content"]["Change_ID"].asInt()), true);
                book = BookMgr.findByPrimaryKey(ChangeTargetArray[i]["Change_Content"]["Change_ID"].asInt());
                MyBasePtr->DEBUGLog("图书查询完毕 : " + book.toJson().toStyledString(), true);

                for (int j = 0; j < ChangeColSize; j++)
                {
                    ChangeColName = ChangeTargetArray[i]["Change_Content"]["Change_Col"][j].asString();
                    MyBasePtr->DEBUGLog("开始修改图书数据(" + ChangeColName + ")", true);
                    // "Change_Name"        :   "",
                    // "Change_Status"      :   "",
                    // "Change_Synopsis"    :   "",
                    // "Change_Publisher"   :   "",
                    // "Change_Author"      :   "",
                    // "Change_Memo"        :   {}

                    // 根据Change_Col改变状态值
                    {
                        if (ChangeColName == "Change_Name")
                        {
                            if (book.getValueOfName() == ChangeTargetArray[i]["Change_Content"]["Change_Name"].asString())
                            {
                                MyBasePtr->DEBUGLog("字段(" + ChangeColName + ")没有产生变化", true);
                                continue;
                            }
                            book.setName(ChangeTargetArray[i]["Change_Content"]["Change_Name"].asString());
                            ChangeCount++;
                        }
                        else if (ChangeColName == "Change_Status")
                        {
                            if (book.getValueOfStatus() == ChangeTargetArray[i]["Change_Content"]["Change_Status"].asString())
                            {
                                MyBasePtr->DEBUGLog("字段(" + ChangeColName + ")没有产生变化", true);
                                continue;
                            }
                            book.setStatus(ChangeTargetArray[i]["Change_Content"]["Change_Status"].asString());
                            ChangeCount++;
                        }
                        else if (ChangeColName == "Change_Synopsis")
                        {
                            if (book.getValueOfSynopsis() == ChangeTargetArray[i]["Change_Content"]["Change_Synopsis"].asString())
                            {
                                MyBasePtr->DEBUGLog("字段(" + ChangeColName + ")没有产生变化", true);
                                continue;
                            }
                            book.setSynopsis(ChangeTargetArray[i]["Change_Content"]["Change_Synopsis"].asString());
                            ChangeCount++;
                        }
                        else if (ChangeColName == "Change_Publisher")
                        {
                            if (book.getValueOfPublisher() == ChangeTargetArray[i]["Change_Content"]["Change_Publisher"].asString())
                            {
                                MyBasePtr->DEBUGLog("字段(" + ChangeColName + ")没有产生变化", true);
                                continue;
                            }
                            book.setPublisher(ChangeTargetArray[i]["Change_Content"]["Change_Publisher"].asString());
                            ChangeCount++;
                        }
                        else if (ChangeColName == "Change_Author")
                        {
                            if (book.getValueOfAuthor() == ChangeTargetArray[i]["Change_Content"]["Change_Author"].asString())
                            {
                                MyBasePtr->DEBUGLog("字段(" + ChangeColName + ")没有产生变化", true);
                                continue;
                            }
                            book.setAuthor(ChangeTargetArray[i]["Change_Content"]["Change_Author"].asString());
                            ChangeCount++;
                        }
                        else if (ChangeColName == "Change_Memo")
                        {
                            if (book.getValueOfMemo() == ChangeTargetArray[i]["Change_Content"]["Change_Memo"].toStyledString())
                            {
                                MyBasePtr->DEBUGLog("字段(" + ChangeColName + ")没有产生变化", true);
                                continue;
                            }
                            book.setMemo(ChangeTargetArray[i]["Change_Content"]["Change_Memo"].toStyledString());
                            ChangeCount++;
                        }
                        else
                        {
                            MyBasePtr->DEBUGLog("Change_Col包含错误字段(" + ChangeColName + ")", true);
                        }
                    }
                    
                }
                
                MyBasePtr->DEBUGLog("第"+to_string(i)+"项的数据修改完成", true);
                
                // 统计更新的字段数
                MyBasePtr->DEBUGLog("统计第"+to_string(i)+"项更新的字段数为" + to_string(ChangeCount), true);

                if (ChangeCount == 0)
                {
                    MyBasePtr->DEBUGLog("第"+to_string(i)+"项的数据无需更新", true);
                    ChangeTargetArray[i]["ErrorMsg"] = "资源数据无需更新";
                    ChangeTargetArray[i]["Result"] = false;
                    continue;
                }

                MyBasePtr->DEBUGLog("开始更新第"+to_string(i)+"项的数据", true);
                int row = BookMgr.update(book);
                if (row != 1)
                {
                    ChangeTargetArray[i]["ErrorMsg"] = "图书资料更新失败";
                    ChangeTargetArray[i]["Result"] = false;
                    continue;
                }
                MyBasePtr->DEBUGLog("第"+to_string(i)+"项数据更新完成: " + book.toJson().toStyledString(), true);

                ChangeTargetArray[i]["Result"] = true;
            }
            else if(ChangeTargetArray[i]["Change_Type"].asString() == "Chapter")
            {
                MyBasePtr->DEBUGLog("开始查询章节 : " + to_string(ChangeTargetArray[i]["Change_Content"]["Change_ID"].asInt()), true);
                chapter = ChapterMgr.findByPrimaryKey(ChangeTargetArray[i]["Change_Content"]["Change_ID"].asInt());
                MyBasePtr->DEBUGLog("章节查询完毕 : " + chapter.toJson().toStyledString(), true);
                
                for (int j = 0; j < ChangeColSize; j++)
                {
                    ChangeColName = ChangeTargetArray[i]["Change_Content"]["Change_Col"][j].asString();
                    MyBasePtr->DEBUGLog("开始修改章节数据(" + ChangeColName + ")", true);
                    // "Change_Book_ID"     :   0,
                    // "Change_Part_Num"    :   0,
                    // "Change_Chapter_Num" :   0,
                    // "Change_Title"       :   "",
                    // "Change_User_ID"     :   0,
                    // "Change_Valid"       :   true/false,
                    // "Change_Content"     :   {},
                    // "Change_Memo"        :   {}

                    // 根据Change_Col改变状态值
                    {
                        if (ChangeColName == "Change_Book_ID")
                        {
                            if (chapter.getValueOfBookId() == ChangeTargetArray[i]["Change_Content"]["Change_Book_ID"].asInt())
                            {
                                MyBasePtr->DEBUGLog("字段(" + ChangeColName + ")没有产生变化", true);
                                continue;
                            }
                            chapter.setBookId(ChangeTargetArray[i]["Change_Content"]["Change_Book_ID"].asInt());
                            ChangeCount++;
                        }
                        else if (ChangeColName == "Change_Part_Num")
                        {
                            if (chapter.getValueOfPartNum() == ChangeTargetArray[i]["Change_Content"]["Change_Part_Num"].asInt())
                            {
                                MyBasePtr->DEBUGLog("字段(" + ChangeColName + ")没有产生变化", true);
                                continue;
                            }
                            chapter.setPartNum(ChangeTargetArray[i]["Change_Content"]["Change_Part_Num"].asInt());
                            ChangeCount++;
                        }
                        else if (ChangeColName == "Change_Chapter_Num")
                        {
                            if (chapter.getValueOfChapterNum() == ChangeTargetArray[i]["Change_Content"]["Change_Chapter_Num"].asInt())
                            {
                                MyBasePtr->DEBUGLog("字段(" + ChangeColName + ")没有产生变化", true);
                                continue;
                            }
                            chapter.setChapterNum(ChangeTargetArray[i]["Change_Content"]["Change_Chapter_Num"].asInt());
                            ChangeCount++;
                        }
                        else if (ChangeColName == "Change_Title")
                        {
                            if (chapter.getValueOfTitle() == ChangeTargetArray[i]["Change_Content"]["Change_Title"].asString())
                            {
                                MyBasePtr->DEBUGLog("字段(" + ChangeColName + ")没有产生变化", true);
                                continue;
                            }
                            chapter.setTitle(ChangeTargetArray[i]["Change_Content"]["Change_Title"].asString());
                            ChangeCount++;
                        }
                        else if (ChangeColName == "Change_User_ID")
                        {
                            if (chapter.getValueOfUserId() == ChangeTargetArray[i]["Change_Content"]["Change_User_ID"].asInt())
                            {
                                MyBasePtr->DEBUGLog("字段(" + ChangeColName + ")没有产生变化", true);
                                continue;
                            }
                            chapter.setUserId(ChangeTargetArray[i]["Change_Content"]["Change_User_ID"].asInt());
                            ChangeCount++;
                        }
                        else if (ChangeColName == "Change_Valid")
                        {
                            if (chapter.getValueOfValid() == ChangeTargetArray[i]["Change_Content"]["Change_Valid"].asBool())
                            {
                                MyBasePtr->DEBUGLog("字段(" + ChangeColName + ")没有产生变化", true);
                                continue;
                            }
                            chapter.setValid(ChangeTargetArray[i]["Change_Content"]["Change_Valid"].asBool());
                            ChangeCount++;
                        }
                        else if (ChangeColName == "Change_Content")
                        {
                            if (chapter.getValueOfContent() == ChangeTargetArray[i]["Change_Content"]["Change_Content"].toStyledString())
                            {
                                MyBasePtr->DEBUGLog("字段(" + ChangeColName + ")没有产生变化", true);
                                continue;
                            }
                            chapter.setContent(ChangeTargetArray[i]["Change_Content"]["Change_Content"].toStyledString());
                            ChangeCount++;
                        }
                        else if (ChangeColName == "Change_Memo")
                        {
                            if (chapter.getValueOfMemo() == ChangeTargetArray[i]["Change_Content"]["Change_Memo"].toStyledString())
                            {
                                MyBasePtr->DEBUGLog("字段(" + ChangeColName + ")没有产生变化", true);
                                continue;
                            }
                            chapter.setMemo(ChangeTargetArray[i]["Change_Content"]["Change_Memo"].toStyledString());
                            ChangeCount++;
                        }
                        else
                        {
                            MyBasePtr->DEBUGLog("Change_Col包含错误字段(" + ChangeColName + ")", true);
                        }
                    }
                    
                }
                
                MyBasePtr->DEBUGLog("第"+to_string(i)+"项的数据修改完成", true);
                
                // 统计更新的字段数
                MyBasePtr->DEBUGLog("统计第"+to_string(i)+"项更新的字段数为" + to_string(ChangeCount), true);

                if (ChangeCount == 0)
                {
                    MyBasePtr->DEBUGLog("第"+to_string(i)+"项的数据无需更新", true);
                    ChangeTargetArray[i]["ErrorMsg"] = "资源数据无需更新";
                    ChangeTargetArray[i]["Result"] = false;
                    continue;
                }

                MyBasePtr->DEBUGLog("开始更新第"+to_string(i)+"项的数据", true);
                int row = ChapterMgr.update(chapter);
                if (row != 1)
                {
                    ChangeTargetArray[i]["ErrorMsg"] = "图书资料更新失败";
                    ChangeTargetArray[i]["Result"] = false;
                    continue;
                }
                MyBasePtr->DEBUGLog("第"+to_string(i)+"项数据更新完成: " + chapter.toJson().toStyledString(), true);

                ChangeTargetArray[i]["Result"] = true;

            }

        }
        catch(const drogon::orm::DrogonDbException &e)
        {
            if (e.base().what() == string("0 rows found"))
            {
                ChangeTargetArray[i]["ErrorMsg"] = "要更改的资源不存在";
            }
            else if (e.base().what() == string("Found more than one row"))
            {
                ChangeTargetArray[i]["ErrorMsg"] = "要更改的资源ID重复,请联系管理员";
            }
            else
            {
                ChangeTargetArray[i]["ErrorMsg"] = e.base().what();
            }

            MyBasePtr->DEBUGLog("ErrorMsg::" + ChangeTargetArray[i]["ErrorMsg"].toStyledString(), true);
            ChangeTargetArray[i]["Result"] = false;
            continue;
        }
    }

    RespJson["Change_Target"] = ChangeTargetArray;
}

