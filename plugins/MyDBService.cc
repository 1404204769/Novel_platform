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
    MyBasePtr->TRACELog("MyDBService initAndStart",true);
}

void MyDBService::shutdown() 
{
    /// Shutdown the plugin
    auto MyBasePtr = drogon::app().getPlugin<MyBase>();
    MyBasePtr->TRACELog("MyDBService shutdown",true);
}

// 用户上传新书的接口
void MyDBService::User_Upload_New_Book(Json::Value &ReqJson, Json::Value &RespJson)
{
    auto MyJsonPtr = app().getPlugin<MyJson>();
    auto MyBasePtr = app().getPlugin<MyBase>();
    auto MyToolsPtr = app().getPlugin<MyTools>();
    MyBasePtr->DEBUGLog("User_Upload_New_Book::ReqJson: " + ReqJson.toStyledString(), true);
    
    auto dbclientPrt=drogon::app().getDbClient();
    Mapper<drogon_model::novel::User>UserMgr(dbclientPrt);
    Mapper<drogon_model::novel::Upload>UploadMgr(dbclientPrt);
    Mapper<drogon_model::novel::Book>BookMgr(dbclientPrt);
    
    int UserID  =   ReqJson["UserID"].asInt();
    int Book_ID =   ReqJson["Book_ID"].asInt();
    Json::Value JsonContent =   ReqJson["Content"];
    Json::Value UploadMemo;

    // 检查数据合法性
    {    
        MyBasePtr->DEBUGLog("开始检查数据合法性", true);
        MyJsonPtr->checkMember(JsonContent,RespJson,"Book_Name");
        MyJsonPtr->checkMember(JsonContent,RespJson,"Book_Author");
        if(ReqJson["Book_Name"].asString() != JsonContent["Book_Name"].asString())
        {
            RespJson["ErrorMsg"] = "Book_Name字段与Content/Book_Name不同";
            throw RespJson;
        }
        if(ReqJson["Book_Author"].asString() != JsonContent["Book_Author"].asString())
        {
            RespJson["ErrorMsg"] = "Book_Author字段与Content/Book_Author不同";
            throw RespJson;
        }
        MyBasePtr->DEBUGLog("检查数据合法性完成", true);
    }
    
    // 插入前检查content数据是否合法
    {    
        MyBasePtr->DEBUGLog("开始检查content数据是否合法", true);
        // "Book_Author"   :   "",
        // "Book_Name"   :   "",
        // "Book_Publisher":   ""
        // "Book_Synopsis" : ""
        // 创建检查列表 key是字段名 value 是字段类型
        std::map<string,MyJson::ColType>ColMap;
        ColMap["Book_Name"]=MyJson::ColType::STRING;
        ColMap["Book_Author"]=MyJson::ColType::STRING;
        ColMap["Book_Publisher"]=MyJson::ColType::STRING;
        ColMap["Book_Synopsis"]=MyJson::ColType::STRING;
        MyJsonPtr->checkMemberAndTypeInMap(JsonContent,RespJson,ColMap);
        MyBasePtr->DEBUGLog("content数据合法", true);
    }

    MyBasePtr->DEBUGLog("开始查询用户 : " + to_string(UserID), true);
    drogon_model::novel::User user = UserMgr.findByPrimaryKey(UserID);
    MyBasePtr->DEBUGLog("用户查询完毕 : " + user.toJson().toStyledString(), true);

    // 验证用户
    if(user.getValueOfUserId() != UserID)
    {
        RespJson["ErrorMsg"] = "登入的UserID("+ to_string(UserID) +")与查询到的UserID("+ to_string(user.getValueOfUserId()) +")不同";
        throw RespJson;
    }

    drogon_model::novel::Upload NewUpload;
    // 准备要处理的新数据
    {
        MyBasePtr->DEBUGLog("开始准备要处理的新数据", true);
        NewUpload.setBookId(Book_ID);
        NewUpload.setContent(JsonContent.toStyledString());
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
    string BookName = JsonContent["Book_Name"].asString();
    string BookAuthor = JsonContent["Book_Author"].asString();
    string BookPublisher = JsonContent["Book_Publisher"].asString();
    string BookSynopsis = JsonContent["Book_Synopsis"].asString();

    // 检查图书是否已存在
    {
        MyBasePtr->DEBUGLog("开始检查图书是否已存在", true);
        Criteria BookName_cri(drogon_model::novel::Book::Cols::_Name, CompareOperator::EQ, BookName);
        Criteria BookAuthor_cri(drogon_model::novel::Book::Cols::_Author, CompareOperator::EQ, BookAuthor);
        vector<drogon_model::novel::Book> vecBook = BookMgr.findBy(BookName_cri&&BookAuthor_cri);
        // 如果不是新书，则将Upload状态改为已处理且拒绝插入
        if(vecBook.size() != 0)
        {
            RespJson["ErrorMsg"] = "要新建的图书已存在(BookID = "+to_string(vecBook[0].getValueOfBookId()) + ")";
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
            if(!count)
            {
                MyBasePtr->DEBUGLog(RespJson["ErrorMsg"].asString(),true);   
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
        NewBook.setAuthor(BookAuthor); // 作者
        NewBook.setName(BookName); // 书名
        NewBook.setPublisher(BookPublisher); // 出版方
        NewBook.setStatus("连载中"); // 状态
        NewBook.setSynopsis(BookSynopsis); // 提要
        /*
            Memo : {
                "Chapter" :[],// 数组形式记录每一卷的章节数量 第一位保存所有卷的总章节数
                "ExtraChapter":{
                    "Index" : 10000,//从10000开始的章节数都算番外
                    "Num"   : 0,//记录番外的数量
                }
            }
        */
        BookMemo["Chapter"].append(0);// 插入总章节数
        BookMemo["Chapter"].append(0);// 插入第一卷的章节数
        BookMemo["ExtraChapter"]["Index"] = 10000;// 插入番外章节数开始位置
        BookMemo["ExtraChapter"]["Num"] = 0;// 插入番外的章节数

        NewBook.setMemo(BookMemo.toStyledString()); // 详细参数
        MyBasePtr->DEBUGLog("准备新图书数据完成", true);
        MyBasePtr->DEBUGLog("准备新图书数据为 : " + NewBook.toJson().toStyledString(), true);
    }

    // 准备插入图书数据
    try{
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
            if(!count)
            {
                RespJson["ErrorMsg"] = "更新Upload数据失败";
                throw RespJson;
            }
            MyBasePtr->DEBUGLog("更新Upload数据完毕", true);
        }
    }
    // 新建图书失败
    catch(...)
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
            if(!count)
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
    
    auto dbclientPrt=drogon::app().getDbClient();
    Mapper<drogon_model::novel::User>UserMgr(dbclientPrt);
    Mapper<drogon_model::novel::Upload>UploadMgr(dbclientPrt);
    Mapper<drogon_model::novel::Book>BookMgr(dbclientPrt);
    Mapper<drogon_model::novel::Chapter>ChapterMgr(dbclientPrt);
    
    int UserID  =   ReqJson["UserID"].asInt();
    int Book_ID =   ReqJson["Book_ID"].asInt();
    string Book_Name    =   ReqJson["Book_Name"].asString();
    string Book_Author  =   ReqJson["Book_Author"].asString();
    Json::Value JsonContent =   ReqJson["Content"];
    Json::Value UploadMemo;
    drogon_model::novel::Book TBook;// 目标图书


    // 先判断传入数据的Book_Name和Book_Author是否存在 若存在则判断目标图书ID是否正确
    {
        MyBasePtr->DEBUGLog("开始检查上传新章节的图书是否已存在", true);
        Criteria BookName_cri(drogon_model::novel::Book::Cols::_Name, CompareOperator::EQ, Book_Name);
        Criteria BookAuthor_cri(drogon_model::novel::Book::Cols::_Author, CompareOperator::EQ, Book_Author);
        vector<drogon_model::novel::Book> vecBook = BookMgr.findBy(BookName_cri&&BookAuthor_cri);
        // 如果不是已存在的书，则拒绝插入Upload
        if(vecBook.size() == 0)
        {
            RespJson["ErrorMsg"] = "要上传新章节的图书不存在";
            throw RespJson;
        }
        MyBasePtr->DEBUGLog("要上传新章节的图书存在", true);
        TBook =vecBook[0];
        MyBasePtr->DEBUGLog("开始判断目标图书ID是否正确", true);
        if(TBook.getValueOfBookId() != Book_ID)
        {
            RespJson["ErrorMsg"] = "要上传新章节的图书的ID与指定ID不符合,(Book_ID=" + to_string(Book_ID) + ",目标图书的ID="+to_string(TBook.getValueOfBookId());
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

        std::map<string,MyJson::ColType>ColMap;
        ColMap["ChapterTitle"]=MyJson::ColType::STRING;
        ColMap["ContentArray"]=MyJson::ColType::ARRAY;
        MyJsonPtr->checkMemberAndTypeInMap(JsonContent,RespJson,ColMap);
        MyBasePtr->DEBUGLog("检查content数据合法", true);
    }

    MyBasePtr->DEBUGLog("开始查询用户 : " + to_string(UserID), true);
    drogon_model::novel::User user = UserMgr.findByPrimaryKey(UserID);
    MyBasePtr->DEBUGLog("用户查询完毕 : " + user.toJson().toStyledString(), true);

    // 验证用户
    if(user.getValueOfUserId() != UserID)
    {
        RespJson["ErrorMsg"] = "登入的UserID("+ to_string(UserID) +")与查询到的UserID("+ to_string(user.getValueOfUserId()) +")不同";
        throw RespJson;
    }

    drogon_model::novel::Upload NewUpload;
    // 准备要处理的新数据
    {
        MyBasePtr->DEBUGLog("开始准备要处理的新数据", true);
        NewUpload.setBookId(Book_ID);
        NewUpload.setContent(JsonContent.toStyledString());
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
    pair<int,string>Title = MyToolsPtr->getTitleNumAndTitleStr(JsonContent["ChapterTitle"].asString());
    string ChapterTitle = Title.second;// 章节名
    int Part_Num;// 章节卷
    int Chapter_Num = Title.first;// 章节数
    Json::Value Chapter_Content = JsonContent["ContentArray"];// 章节内容
    Json::Value Chapter_Memo ;// 参数
    Json::Value BookMemo;
    Json::Value ChapterJson;//  Memo/Chapter

    // 章节卷需要查询Book表获取
    {
        MyBasePtr->DEBUGLog("开始查询Book/Memo获取章节卷", true);
        MyJsonPtr->JsonstrToJson(BookMemo,TBook.getValueOfMemo());

        MyBasePtr->DEBUGLog("开始检查Book/Memo数据是否合法", true);
        std::map<string,MyJson::ColType>ColMap;
        ColMap["Chapter"]=MyJson::ColType::ARRAY;
        ColMap["ExtraChapter"]=MyJson::ColType::JSON;
        MyJsonPtr->checkMemberAndTypeInMap(BookMemo,RespJson,ColMap);
        MyBasePtr->DEBUGLog("Book/Memo数据合法", true);
        ////---- 这里有可能不合法导致跳出，需要捕获然后更新Upload的值
        ChapterJson = BookMemo["Chapter"];

        int vecPartSize = ChapterJson.size();
        Part_Num = vecPartSize - 1;
        MyBasePtr->DEBUGLog("查询Book/Memo获取章节卷成功", true);
    }

    // 章节数小于0 说明是番外 ////----待处理
    if(Chapter_Num < 0)
    {

    }


    // 检查章节是否已存在
    {
        MyBasePtr->DEBUGLog("开始检查图书章节是否已存在", true);
        Criteria BookID_cri(drogon_model::novel::Chapter::Cols::_Book_ID, CompareOperator::EQ, Book_ID);
        Criteria PartNum_cri(drogon_model::novel::Chapter::Cols::_Part_Num, CompareOperator::EQ, Part_Num);
        Criteria ChapterNum_cri(drogon_model::novel::Chapter::Cols::_Chapter_Num, CompareOperator::EQ, Chapter_Num);
        Criteria ChapterTitle_cri(drogon_model::novel::Chapter::Cols::_Title, CompareOperator::EQ, ChapterTitle);
        vector<drogon_model::novel::Chapter> vecChapter = ChapterMgr.findBy(BookID_cri&&PartNum_cri&&ChapterNum_cri&&ChapterTitle_cri);
        // 如果不是新章节，则将Upload状态改为已处理且拒绝插入
        if(vecChapter.size() != 0)
        {
            RespJson["ErrorMsg"] = "要新增的图书章节已存在(BookID = "+ to_string(vecChapter[0].getValueOfBookId())
                                + ",Part_Num = " + to_string(vecChapter[0].getValueOfPartNum())
                                + ",Chapter_Num = " + to_string(vecChapter[0].getValueOfChapterNum())
                                + ",Title = " + vecChapter[0].getValueOfTitle() + ")";
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
            if(!count)
            {
                MyBasePtr->DEBUGLog(RespJson["ErrorMsg"].asString(),true);   
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
    try{
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
            if(!count)
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
            int count = UploadMgr.update(NewUpload);
            UploadMemo["Explain"] = "上传新章节成功";
            NewUpload.setMemo(UploadMemo.toStyledString());
            MyBasePtr->DEBUGLog("更新的新数据为 : " + NewUpload.toJson().toStyledString(), true);
            if(!count)
            {
                RespJson["ErrorMsg"] = "更新Upload数据失败";
                throw RespJson;
            }
            MyBasePtr->DEBUGLog("更新Upload数据完毕", true);
        }
    }
    // 新建章节失败
    catch(...)
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
            if(!count)
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
    
    auto dbclientPrt=drogon::app().getDbClient();
    Mapper<drogon_model::novel::User>UserMgr(dbclientPrt);
    Mapper<drogon_model::novel::Upload>UploadMgr(dbclientPrt);
    Mapper<drogon_model::novel::Book>BookMgr(dbclientPrt);
    Mapper<drogon_model::novel::Chapter>ChapterMgr(dbclientPrt);
    
    int UserID  =   ReqJson["UserID"].asInt();
    int Book_ID =   ReqJson["Book_ID"].asInt();
    string Book_Name    =   ReqJson["Book_Name"].asString();
    string Book_Author  =   ReqJson["Book_Author"].asString();
    Json::Value JsonContent =   ReqJson["Content"];
    Json::Value UploadMemo;
    drogon_model::novel::Book TBook;// 目标图书


    // 先判断传入数据的Book_Name和Book_Author是否存在 若存在则判断目标图书ID是否正确
    {
        MyBasePtr->DEBUGLog("开始检查上传新章节的图书是否已存在", true);
        Criteria BookName_cri(drogon_model::novel::Book::Cols::_Name, CompareOperator::EQ, Book_Name);
        Criteria BookAuthor_cri(drogon_model::novel::Book::Cols::_Author, CompareOperator::EQ, Book_Author);
        vector<drogon_model::novel::Book> vecBook = BookMgr.findBy(BookName_cri&&BookAuthor_cri);
        // 如果不是已存在的书，则拒绝插入Upload
        if(vecBook.size() == 0)
        {
            RespJson["ErrorMsg"] = "要上传新章节的图书不存在";
            throw RespJson;
        }
        MyBasePtr->DEBUGLog("要上传新章节的图书存在", true);
        TBook =vecBook[0];
        MyBasePtr->DEBUGLog("开始判断目标图书ID是否正确", true);
        if(TBook.getValueOfBookId() != Book_ID)
        {
            RespJson["ErrorMsg"] = "要上传新章节的图书的ID与指定ID不符合,(Book_ID=" + to_string(Book_ID) + ",目标图书的ID="+to_string(TBook.getValueOfBookId());
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

        std::map<string,MyJson::ColType>ColMap;
        ColMap["ChapterTitle"]=MyJson::ColType::STRING;
        ColMap["ContentArray"]=MyJson::ColType::ARRAY;
        MyJsonPtr->checkMemberAndTypeInMap(JsonContent,RespJson,ColMap);
        MyBasePtr->DEBUGLog("检查content数据合法", true);
    }

    MyBasePtr->DEBUGLog("开始查询用户 : " + to_string(UserID), true);
    drogon_model::novel::User user = UserMgr.findByPrimaryKey(UserID);
    MyBasePtr->DEBUGLog("用户查询完毕 : " + user.toJson().toStyledString(), true);

    // 验证用户
    if(user.getValueOfUserId() != UserID)
    {
        RespJson["ErrorMsg"] = "登入的UserID("+ to_string(UserID) +")与查询到的UserID("+ to_string(user.getValueOfUserId()) +")不同";
        throw RespJson;
    }

    drogon_model::novel::Upload NewUpload;
    // 准备要处理的新数据
    {
        MyBasePtr->DEBUGLog("开始准备要处理的新数据", true);
        NewUpload.setBookId(Book_ID);
        NewUpload.setContent(JsonContent.toStyledString());
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
    pair<int,string>Title = MyToolsPtr->getTitleNumAndTitleStr(JsonContent["ChapterTitle"].asString());
    string ChapterTitle = Title.second;// 章节名
    int Part_Num;// 章节卷
    int Chapter_Num = Title.first;// 章节数
    Json::Value Chapter_Content = JsonContent["ContentArray"];// 章节内容
    Json::Value Chapter_Memo ;// 参数
    Json::Value BookMemo;
    Json::Value ChapterJson;//  Memo/Chapter

    // 章节卷需要查询Book表获取
    {
        MyBasePtr->DEBUGLog("开始查询Book/Memo获取章节卷", true);
        MyJsonPtr->JsonstrToJson(BookMemo,TBook.getValueOfMemo());

        MyBasePtr->DEBUGLog("开始检查Book/Memo数据是否合法", true);
        std::map<string,MyJson::ColType>ColMap;
        ColMap["Chapter"]=MyJson::ColType::ARRAY;
        ColMap["ExtraChapter"]=MyJson::ColType::JSON;
        MyJsonPtr->checkMemberAndTypeInMap(BookMemo,RespJson,ColMap);
        MyBasePtr->DEBUGLog("Book/Memo数据合法", true);
        ////---- 这里有可能不合法导致跳出，需要捕获然后更新Upload的值
        ChapterJson = BookMemo["Chapter"];

        int vecPartSize = ChapterJson.size();
        Part_Num = vecPartSize - 1;
        MyBasePtr->DEBUGLog("查询Book/Memo获取章节卷成功", true);
    }

    // 章节数小于0 说明是番外 ////----待处理
    if(Chapter_Num < 0)
    {

    }


    // 检查章节是否不存在,存在则等待管理员处理，不存在则直接拒绝
    {
        MyBasePtr->DEBUGLog("开始检查图书章节是否不存在", true);
        Criteria BookID_cri(drogon_model::novel::Chapter::Cols::_Book_ID, CompareOperator::EQ, Book_ID);
        Criteria PartNum_cri(drogon_model::novel::Chapter::Cols::_Part_Num, CompareOperator::EQ, Part_Num);
        Criteria ChapterNum_cri(drogon_model::novel::Chapter::Cols::_Chapter_Num, CompareOperator::EQ, Chapter_Num);
        Criteria ChapterTitle_cri(drogon_model::novel::Chapter::Cols::_Title, CompareOperator::EQ, ChapterTitle);
        vector<drogon_model::novel::Chapter> vecChapter = ChapterMgr.findBy(BookID_cri&&PartNum_cri&&ChapterNum_cri&&ChapterTitle_cri);
        // 如果不是已存在的章节，则将Upload状态改为已处理且拒绝插入
        if(vecChapter.size() == 0)
        {
            RespJson["ErrorMsg"] = "要改正的图书章节不存在(BookID = "+ to_string(Book_ID)
                                + ",Part_Num = " + to_string(Part_Num)
                                + ",Chapter_Num = " + to_string(Chapter_Num)
                                + ",Title = " + ChapterTitle + "),请用正确的接口新增此章节";
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
            if(!count)
            {
                MyBasePtr->DEBUGLog(RespJson["ErrorMsg"].asString(),true);   
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
void MyDBService::User_Download(Json::Value &ReqJson, Json::Value &RespJson)
{
    auto MyJsonPtr = app().getPlugin<MyJson>();
    auto MyBasePtr = app().getPlugin<MyBase>();
    auto MyToolsPtr = app().getPlugin<MyTools>();
    MyBasePtr->DEBUGLog("User_Download::ReqJson: " + ReqJson.toStyledString(), true);
    
    auto dbclientPrt=drogon::app().getDbClient();
    Mapper<drogon_model::novel::User>UserMgr(dbclientPrt);
    Mapper<drogon_model::novel::Upload>UploadMgr(dbclientPrt);
    Mapper<drogon_model::novel::Book>BookMgr(dbclientPrt);
    Mapper<drogon_model::novel::Chapter>ChapterMgr(dbclientPrt);

    int UserID  =   ReqJson["UserID"].asInt();
    int Book_ID =   ReqJson["Book_ID"].asInt();
    vector<int> vecChapter_ID;
    drogon_model::novel::Book TBook;// 目标图书
    Json::Value TBookMemo;
    Json::Value TBookMemoChapter;//  Memo/Chapter
    Json::Value TBookMemoExtraChapter;//  Memo/ExtraChapter

    MyBasePtr->DEBUGLog("开始查询用户 : " + to_string(UserID), true);
    drogon_model::novel::User user = UserMgr.findByPrimaryKey(UserID);
    MyBasePtr->DEBUGLog("用户查询完毕 : " + user.toJson().toStyledString(), true);

    // 验证用户
    if(user.getValueOfUserId() != UserID)
    {
        RespJson["ErrorMsg"] = "登入的UserID("+ to_string(UserID) +")与查询到的UserID("+ to_string(user.getValueOfUserId()) +")不同";
        throw RespJson;
    }

    // 先判断传入数据的Book_ID对应的图书是否存在
    {
        MyBasePtr->DEBUGLog("开始检查要下载的图书是否已存在", true);
        Criteria BookID_cri(drogon_model::novel::Book::Cols::_Book_ID, CompareOperator::EQ, Book_ID);
        vector<drogon_model::novel::Book> vecBook = BookMgr.findBy(BookID_cri);
        // 如果不是已存在的书，则拒绝插入Upload
        if(vecBook.size() == 0)
        {
            RespJson["ErrorMsg"] = "要下载的图书不存在";
            throw RespJson;
        }
        MyBasePtr->DEBUGLog("要下载的图书存在", true);
        TBook =vecBook[0];
    }

    // 检查Book/Memo数据是否合法
    {
        MyBasePtr->DEBUGLog("开始检查Book/Memo数据是否合法", true);
        MyJsonPtr->JsonstrToJson(TBookMemo,TBook.getValueOfMemo());
        std::map<string,MyJson::ColType>ColMap;
        ColMap["Chapter"]=MyJson::ColType::ARRAY;
        ColMap["ExtraChapter"]=MyJson::ColType::JSON;
        MyJsonPtr->checkMemberAndTypeInMap(TBookMemo,RespJson,ColMap);
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
        for(int i = 1 ; i < MemoChapterSize ; i++)
        {
            RespBook["Book_Memo"]["正文"]["第"+ to_string(i) +"卷章节数"] = TBookMemoChapter[i].asInt();
        }
        RespBook["Book_Memo"]["番外"]["番外起始编号"] = TBookMemoExtraChapter["Index"];
        RespBook["Book_Memo"]["番外"]["番外数量"] = TBookMemoExtraChapter["Num"];
        RespJson["Book_Data"] = RespBook;
    }


    // 获取要下载的图书的章节ID
    {

        MyBasePtr->DEBUGLog("开始获取要下载的图书的章节ID", true);
        int ChapterSize = ReqJson["Chapter_ID"].size();
        for(int i = 0; i< ChapterSize; i++)
        {
            vecChapter_ID.push_back(ReqJson["Chapter_ID"][i].asInt());
        }
        MyBasePtr->DEBUGLog("要下载的图书的章节ID获取完成", true);
    }

    // 下载并保存章节信息
    {
        MyBasePtr->DEBUGLog("开始获取指定的图书的章节数据", true);
        Criteria ChapterID_cri(drogon_model::novel::Chapter::Cols::_Chapter_ID,CompareOperator::In,vecChapter_ID);
        Criteria Valid_cri(drogon_model::novel::Chapter::Cols::_Valid,CompareOperator::EQ,true);
        MyBasePtr->DEBUGLog("使用的SQL语句为 " + ChapterID_cri.criteriaString(), true);
        vector<drogon_model::novel::Chapter> vecChapter = ChapterMgr.findBy(ChapterID_cri&&Valid_cri);
        int ChapterSize = vecChapter.size();
        if(ChapterSize == 0)
        {
            RespJson["ErrorMsg"] = "要下载的图书的章节都不存在";
            throw RespJson;
        }
        Json::Value TChapterArray;
        Json::Value TempChapter,TempChapterJsonStr;
        for(int i = 0 ; i < ChapterSize ; i++)
        {
            TempChapter.clear();
            TempChapterJsonStr.clear();
            MyJsonPtr->JsonstrToJson(TempChapterJsonStr,vecChapter[i].getValueOfContent());
            TempChapter["Chapter_ID"] = vecChapter[i].getValueOfChapterId();
            TempChapter["Book_ID"] = vecChapter[i].getValueOfBookId();
            TempChapter["Part_Num"] = vecChapter[i].getValueOfPartNum();
            TempChapter["Chapter_Num"] = vecChapter[i].getValueOfChapterNum();
            TempChapter["Title"] = vecChapter[i].getValueOfTitle();
            TempChapter["User_ID"] = vecChapter[i].getValueOfUserId();
            TempChapter["Content"] = TempChapterJsonStr;
            MyJsonPtr->JsonstrToJson(TempChapterJsonStr,vecChapter[i].getValueOfMemo());
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

    auto dbclientPrt=drogon::app().getDbClient();
    Mapper<drogon_model::novel::User>UserMgr(dbclientPrt);

    // 检查ReqJson数据是否合法
    {    
        MyBasePtr->DEBUGLog("开始检查ReqJson数据是否合法", true);
        // "User_ID"   :   0,
        std::map<string,MyJson::ColType>ColMap;
        ColMap["User_ID"]=MyJson::ColType::INT;
        MyJsonPtr->checkMemberAndTypeInMap(ReqJson,RespJson,ColMap);
        MyBasePtr->DEBUGLog("ReqJson数据合法", true);
    }

    MyBasePtr->DEBUGLog("开始查询用户 : " + to_string(ReqJson["User_ID"].asInt()), true);
    drogon_model::novel::User user = UserMgr.findByPrimaryKey(ReqJson["User_ID"].asInt());
    MyBasePtr->DEBUGLog("用户查询完毕 : " + user.toJson().toStyledString(), true);


}

// 用户更改个人资料的接口
void MyDBService::Update_User_PersonalData(Json::Value &ReqJson, Json::Value &RespJson)
{

}

// 管理员修改用户数据
void MyDBService::Admin_Update_User(Json::Value &ReqJson, Json::Value &RespJson)
{
    auto MyJsonPtr = app().getPlugin<MyJson>();
    auto MyBasePtr = app().getPlugin<MyBase>();
    auto MyToolsPtr = app().getPlugin<MyTools>();
    MyBasePtr->DEBUGLog("Admin_Update_User::ReqJson: " + ReqJson.toStyledString(), true);
    
    auto dbclientPrt=drogon::app().getDbClient();
    Mapper<drogon_model::novel::User>UserMgr(dbclientPrt);
    Mapper<drogon_model::novel::Upload>UploadMgr(dbclientPrt);
    Mapper<drogon_model::novel::Book>BookMgr(dbclientPrt);
    
    int UserID  =   ReqJson["UserID"].asInt();
    int Book_ID =   ReqJson["Book_ID"].asInt();
    Json::Value JsonContent =   ReqJson["Content"];
    Json::Value UploadMemo;
}





