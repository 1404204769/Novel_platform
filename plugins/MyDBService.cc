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

// 管理员查看用户上传申请
void MyDBService::Admin_Search_Upload(Json::Value &ReqJson, Json::Value &RespJson)
{
    Json::Value ParaJson, ChangeTargetArray;
    auto MyJsonPtr = app().getPlugin<MyJson>();
    auto MyBasePtr = app().getPlugin<MyBase>();
    auto MyToolsPtr = app().getPlugin<MyTools>();
    MyBasePtr->INFO_Func("Admin_Search_Upload", true, ReqJson);
    auto dbclientPrt = drogon::app().getDbClient();
    Mapper<drogon_model::novel::Upload> UploadMgr(dbclientPrt);

    // 检查ReqJson数据是否合法
    {
        MyBasePtr->DEBUGLog("开始检查ReqJson数据是否合法", true);
        // "Limit"      :   0,          //  取多少条数据
        // "Offset"     :   0,          //  偏移量
        // "Finish"     :   true/false, //  是否经过审核
        // "Provider_ID":   0,          //  贡献者ID
        // "Processor"  :   "",         //  处理者
        // "Filter_Col" :   [],         //  过滤字段
        // 创建检查列表 key是字段名 value 是字段类型
        std::map<string, MyJson::ColType> ColMap;
        ColMap["Limit"] = MyJson::ColType::INT;
        ColMap["Offset"] = MyJson::ColType::INT;
        ColMap["Finish"] = MyJson::ColType::BOOL;
        ColMap["Provider_ID"] = MyJson::ColType::INT;
        ColMap["Processor"] = MyJson::ColType::STRING;
        ColMap["Filter_Col"] = MyJson::ColType::ARRAY;
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
            RespJson["ErrorMsg"].append("权限不足，请联系管理员");
            MyBasePtr->INFO_Func("Admin_Search_Upload", false, RespJson);
            throw RespJson;
        }
        MyBasePtr->DEBUGLog("操作权限检测通过", true);
    }

    int FilterSize;
    Criteria Filter_Cri;
    // 制作筛选条件
    {
        Json::Value FilterJson = ReqJson["Filter_Col"];
        FilterSize = FilterJson.size();
        MyBasePtr->DEBUGLog("一共有" + to_string(FilterSize) + "项筛选对象", true);
        for (int i = 0; i < FilterSize; i++)
        {
            MyBasePtr->DEBUGLog("开始校验筛选对象(" + FilterJson[i].asString() + ")", true);
            if (FilterJson[i].asString() == "Finish")
            {
                if (Filter_Cri)
                {
                    MyBasePtr->DEBUGLog("制作筛选对象(" + FilterJson[i].asString() + ")为Filter_Cri", true);
                    Filter_Cri = Filter_Cri && Criteria(drogon_model::novel::Upload::Cols::_IsManage, CompareOperator::EQ, ReqJson["Finish"].asBool());
                    continue;
                }
                Filter_Cri = Criteria(drogon_model::novel::Upload::Cols::_IsManage, CompareOperator::EQ, ReqJson["Finish"].asBool());
            }
            else if (FilterJson[i].asString() == "Provider_ID")
            {
                if (Filter_Cri)
                {
                    MyBasePtr->DEBUGLog("制作筛选对象(" + FilterJson[i].asString() + ")为Filter_Cri", true);
                    Filter_Cri = Filter_Cri && Criteria(drogon_model::novel::Upload::Cols::_User_ID, CompareOperator::Like, "%" + ReqJson["Provider_ID"].asString() + "%");
                    continue;
                }
                Filter_Cri = Criteria(drogon_model::novel::Upload::Cols::_User_ID, CompareOperator::Like, "%" + ReqJson["Provider_ID"].asString() + "%");
            }
            else if (FilterJson[i].asString() == "Processor")
            {
                if (Filter_Cri)
                {
                    MyBasePtr->DEBUGLog("制作筛选对象(" + FilterJson[i].asString() + ")为Filter_Cri", true);
                    Filter_Cri = Filter_Cri && Criteria(drogon_model::novel::Upload::Cols::_User_ID, CompareOperator::Like, "%" + ReqJson["Processor"].asString() + "%");
                    continue;
                }
                Filter_Cri = Criteria(drogon_model::novel::Upload::Cols::_User_ID, CompareOperator::Like, "%" + ReqJson["Processor"].asString() + "%");
            }
            else if (FilterJson[i].asString() == "Limit")
            {
                int LimitNum = ReqJson["Limit"].asInt();
                UploadMgr.limit(LimitNum);
            }
            else if (FilterJson[i].asString() == "Offset")
            {
                int OffsetNum = ReqJson["Offset"].asInt();
                UploadMgr.offset(OffsetNum);
            }
            else
            {
                RespJson["ErrorMsg"].append("Filter_Col中存在非法字段(" + FilterJson[i].asString() + ")");
                throw RespJson;
            }
        }
    }

    // 查询申请
    {
        MyBasePtr->DEBUGLog("开始查询用户申请", true);
        std::vector<drogon_model::novel::Upload> vecUpload;
        if (FilterSize > 0)
            vecUpload = UploadMgr.findBy(Filter_Cri);
        else
            vecUpload = UploadMgr.findAll();

        Json::Value vecJsonVal;
        for (auto &upload : vecUpload)
        {
            vecJsonVal.append(upload.toJson());
            // std::cout << user.toJson().toStyledString() <<std::endl;
        }
        RespJson["UploadList"] = vecJsonVal;
        MyBasePtr->DEBUGLog("用户申请查询完毕", true);
    }
    MyBasePtr->INFO_Func("Admin_Search_Upload", false, RespJson);
}

// 管理员查询用户数据
void MyDBService::Admin_Search_User(Json::Value &ReqJson, Json::Value &RespJson)
{
    auto MyBasePtr = app().getPlugin<MyBase>();
    auto MyJsonPtr = app().getPlugin<MyJson>();
    MyBasePtr->INFO_Func("Admin_Search_User", true, ReqJson);

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
            RespJson["ErrorMsg"].append("权限不足，请联系管理员");
            MyBasePtr->INFO_Func("Admin_Search_User", false, RespJson);
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
        string UserID = FilterJson["User_ID"].asInt() > 0 ? FilterJson["User_ID"].asString() : "";
        Criteria UserID_cri = Criteria(drogon_model::novel::User::Cols::_User_ID, CompareOperator::Like, "%" + UserID + "%");
        Criteria UserName_cri(drogon_model::novel::User::Cols::_Name, CompareOperator::Like, "%" + FilterJson["User_Name"].asString() + "%");
        string UserSex = ((FilterJson["User_Sex"].asString() != "男") && (FilterJson["User_Sex"].asString() != "女")) ? "" : FilterJson["User_Sex"].asString();
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
    MyBasePtr->INFO_Func("Admin_Search_User", false, RespJson);
}

// 管理员修改图书资源数据
void MyDBService::Admin_Update_Resource(Json::Value &ReqJson, Json::Value &RespJson)
{
    Json::Value ParaJson, ChangeTargetArray;
    auto MyJsonPtr = app().getPlugin<MyJson>();
    auto MyBasePtr = app().getPlugin<MyBase>();
    auto MyToolsPtr = app().getPlugin<MyTools>();
    MyBasePtr->INFO_Func("Admin_Update_Resource", true, ReqJson);

    auto dbclientPrt = drogon::app().getDbClient();
    Mapper<drogon_model::novel::User> UserMgr(dbclientPrt);
    Mapper<drogon_model::novel::Upload> UploadMgr(dbclientPrt);
    Mapper<drogon_model::novel::Book> BookMgr(dbclientPrt);
    Mapper<drogon_model::novel::Chapter> ChapterMgr(dbclientPrt);
    drogon_model::novel::Book book;
    drogon_model::novel::Chapter chapter;

    std::map<string, MyJson::ColType> ColMap, BookColMap, ChapterColMap;
    int ChangeSize, ChangeColSize, ChangeCount, ChangeIntegralNum;
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
            RespJson["ErrorMsg"].append("权限不足，请联系管理员");
            throw RespJson;
        }
        MyBasePtr->DEBUGLog("操作权限检测通过", true);
    }

    MyBasePtr->DEBUGLog("开始检查被修改的数据是否合法", true);
    ChangeTargetArray = ReqJson["Change_Target"];
    ChangeSize = ChangeTargetArray.size();
    MyBasePtr->DEBUGLog("要修改的对象一共有" + to_string(ChangeSize) + "项", true);
    Json::Value TempReqJson, TempRespJson;

    // 设置数据校验格式
    {
        ColMap.clear();
        // "Change_Type"        :   "Book/Chapter" ,
        // "Change_Content"     :   {},
        ColMap["Change_Type"] = MyJson::ColType::STRING;
        ColMap["Change_Content"] = MyJson::ColType::JSON;

        BookColMap.clear();
        // "Change_ID"          :   0,
        // "Change_Name"        :   "",
        // "Change_Status"      :   "",
        // "Change_Synopsis"    :   "",
        // "Change_Publisher"   :   "",
        // "Change_Author"      :   "",
        // "Change_Memo"        :   {}
        // "Change_Col"         :   ["Change_Name","Change_Status","Change_Synopsis","Change_Publisher","Change_Author","Change_Memo"]
        BookColMap["Change_ID"] = MyJson::ColType::INT;
        BookColMap["Change_Name"] = MyJson::ColType::STRING;
        BookColMap["Change_Status"] = MyJson::ColType::STRING;
        BookColMap["Change_Synopsis"] = MyJson::ColType::STRING;
        BookColMap["Change_Publisher"] = MyJson::ColType::STRING;
        BookColMap["Change_Author"] = MyJson::ColType::STRING;
        BookColMap["Change_Memo"] = MyJson::ColType::JSON;
        BookColMap["Change_Col"] = MyJson::ColType::ARRAY;

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
        ChapterColMap["Change_ID"] = MyJson::ColType::INT;
        ChapterColMap["Change_Book_ID"] = MyJson::ColType::INT;
        ChapterColMap["Change_Part_Num"] = MyJson::ColType::INT;
        ChapterColMap["Change_Chapter_Num"] = MyJson::ColType::INT;
        ChapterColMap["Change_Title"] = MyJson::ColType::STRING;
        ChapterColMap["Change_User_ID"] = MyJson::ColType::INT;
        ChapterColMap["Change_Valid"] = MyJson::ColType::BOOL;
        ChapterColMap["Change_Content"] = MyJson::ColType::ARRAY;
        ChapterColMap["Change_Memo"] = MyJson::ColType::JSON;
        ChapterColMap["Change_Col"] = MyJson::ColType::ARRAY;
    }

    for (int i = 0; i < ChangeSize; i++)
    {
        TempReqJson.clear();
        TempReqJson = ChangeTargetArray[i];
        TempRespJson.clear();
        try
        {
            MyBasePtr->DEBUGLog("开始检查第" + to_string(i) + "项的数据结构是否合法", true);
            MyJsonPtr->checkMemberAndTypeInMap(TempReqJson, TempRespJson, ColMap);
            MyBasePtr->DEBUGLog("第" + to_string(i) + "项的数据结构合法", true);
            if (TempReqJson["Change_Type"].asString() == "Book")
            {
                MyBasePtr->DEBUGLog("开始检查第" + to_string(i) + "项的(图书)数据是否合法", true);
                TempReqJson.clear();
                TempReqJson = ChangeTargetArray[i]["Change_Content"];
                MyJsonPtr->checkMemberAndTypeInMap(TempReqJson, TempRespJson, BookColMap);
                MyBasePtr->DEBUGLog("第" + to_string(i) + "项的(章节)数据合法", true);
            }
            else if (TempReqJson["Change_Type"].asString() == "Chapter")
            {
                MyBasePtr->DEBUGLog("开始检查第" + to_string(i) + "项的(章节)数据是否合法", true);
                TempReqJson.clear();
                TempReqJson = ChangeTargetArray[i]["Change_Content"];
                MyJsonPtr->checkMemberAndTypeInMap(TempReqJson, TempRespJson, ChapterColMap);
                MyBasePtr->DEBUGLog("第" + to_string(i) + "项的(章节)数据合法", true);
            }
            else
            {
                ChangeTargetArray[i]["Result"] = false;
                ChangeTargetArray[i]["ErrorMsg"].append("资源类型数据不合法(" + TempReqJson["Change_Type"].asString() + ")");
                continue;
            }
            MyBasePtr->DEBUGLog("第" + to_string(i) + "项的数据合法", true);
        }
        catch (Json::Value &TempRespJson)
        {
            ChangeTargetArray[i]["Result"] = false;
            ChangeTargetArray[i]["ErrorMsg"] = TempRespJson["ErrorMsg"];
            ChangeTargetArray[i]["ErrorMsg"].append("资源数据不合法");
            MyBasePtr->TRACE_ERROR(ChangeTargetArray[i]["ErrorMsg"]);
            continue;
        }
    }

    for (int i = 0; i < ChangeSize; i++)
    {
        try
        {
            /* code */

            if (ChangeTargetArray[i].isMember("Result"))
            {
                MyBasePtr->DEBUGLog("第" + to_string(i) + "项的数据不合法,跳过", true);
                continue;
            }

            MyBasePtr->DEBUGLog("第" + to_string(i) + "项的数据合法,开始准备修改", true);

            string ChangeColName;
            ChangeCount = 0;
            ChangeColSize = ChangeTargetArray[i]["Change_Content"]["Change_Col"].size();
            if (ChangeTargetArray[i]["Change_Type"].asString() == "Book")
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

                MyBasePtr->DEBUGLog("第" + to_string(i) + "项的数据修改完成", true);

                // 统计更新的字段数
                MyBasePtr->DEBUGLog("统计第" + to_string(i) + "项更新的字段数为" + to_string(ChangeCount), true);

                if (ChangeCount == 0)
                {
                    MyBasePtr->DEBUGLog("第" + to_string(i) + "项的数据无需更新", true);
                    ChangeTargetArray[i]["ErrorMsg"].append("资源数据无需更新");
                    ChangeTargetArray[i]["Result"] = false;
                    continue;
                }

                MyBasePtr->DEBUGLog("开始更新第" + to_string(i) + "项的数据", true);
                int row = BookMgr.update(book);
                if (row != 1)
                {
                    ChangeTargetArray[i]["ErrorMsg"].append("图书资料更新失败");
                    ChangeTargetArray[i]["Result"] = false;
                    continue;
                }
                MyBasePtr->DEBUGLog("第" + to_string(i) + "项数据更新完成: " + book.toJson().toStyledString(), true);

                ChangeTargetArray[i]["Result"] = true;
            }
            else if (ChangeTargetArray[i]["Change_Type"].asString() == "Chapter")
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

                MyBasePtr->DEBUGLog("第" + to_string(i) + "项的数据修改完成", true);

                // 统计更新的字段数
                MyBasePtr->DEBUGLog("统计第" + to_string(i) + "项更新的字段数为" + to_string(ChangeCount), true);

                if (ChangeCount == 0)
                {
                    MyBasePtr->DEBUGLog("第" + to_string(i) + "项的数据无需更新", true);
                    ChangeTargetArray[i]["ErrorMsg"].append("资源数据无需更新");
                    ChangeTargetArray[i]["Result"] = false;
                    continue;
                }

                MyBasePtr->DEBUGLog("开始更新第" + to_string(i) + "项的数据", true);
                int row = ChapterMgr.update(chapter);
                if (row != 1)
                {
                    ChangeTargetArray[i]["ErrorMsg"].append("图书资料更新失败");
                    ChangeTargetArray[i]["Result"] = false;
                    continue;
                }
                MyBasePtr->DEBUGLog("第" + to_string(i) + "项数据更新完成: " + chapter.toJson().toStyledString(), true);

                ChangeTargetArray[i]["Result"] = true;
            }
        }
        catch (const drogon::orm::DrogonDbException &e)
        {
            if (e.base().what() == string("0 rows found"))
            {
                ChangeTargetArray[i]["ErrorMsg"].append("要更改的资源不存在");
            }
            else if (e.base().what() == string("Found more than one row"))
            {
                ChangeTargetArray[i]["ErrorMsg"].append("要更改的资源ID重复,请联系管理员");
            }
            else
            {
                ChangeTargetArray[i]["ErrorMsg"].append(e.base().what());
            }
            MyBasePtr->TRACE_ERROR(ChangeTargetArray[i]["ErrorMsg"]);
            ChangeTargetArray[i]["Result"] = false;
            continue;
        }
    }

    RespJson["Change_Target"] = ChangeTargetArray;
    MyBasePtr->INFO_Func("Admin_Update_Resource", false, RespJson);
}

// 管理员修改用户数据
void MyDBService::Admin_Update_User(Json::Value &ReqJson, Json::Value &RespJson)
{
    Json::Value ParaJson, ChangeTargetArray;
    auto MyJsonPtr = app().getPlugin<MyJson>();
    auto MyBasePtr = app().getPlugin<MyBase>();
    auto MyToolsPtr = app().getPlugin<MyTools>();
    MyBasePtr->INFO_Func("Admin_Update_User", true, ReqJson);

    auto dbclientPrt = drogon::app().getDbClient();
    Mapper<drogon_model::novel::User> UserMgr(dbclientPrt);
    Mapper<drogon_model::novel::Upload> UploadMgr(dbclientPrt);
    Mapper<drogon_model::novel::Book> BookMgr(dbclientPrt);
    drogon_model::novel::User user;

    int ChangeSize, ChangeColSize, ChangeCount, ChangeIntegralNum;
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
            RespJson["ErrorMsg"].append("权限不足，请联系管理员");
            throw RespJson;
        }
        MyBasePtr->DEBUGLog("操作权限检测通过", true);

        MyBasePtr->DEBUGLog("开始检查被修改者们的数据是否合法", true);
        ChangeTargetArray = ReqJson["Change_Target"];
        ChangeSize = ChangeTargetArray.size();
        MyBasePtr->DEBUGLog("要修改的对象一共有" + to_string(ChangeSize) + "位", true);
        Json::Value TempReqJson, TempRespJson;

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

        for (int i = 0; i < ChangeSize; i++)
        {
            TempReqJson.clear();
            TempReqJson = ChangeTargetArray[i];
            TempRespJson.clear();
            try
            {
                MyBasePtr->DEBUGLog("开始检查第" + to_string(i) + "位修改者的数据是否合法", true);
                MyJsonPtr->checkMemberAndTypeInMap(TempReqJson, TempRespJson, ColMap);
                MyBasePtr->DEBUGLog("第" + to_string(i) + "位修改者的数据合法", true);
            }
            catch (Json::Value &TempRespJson)
            {
                ChangeTargetArray[i]["Result"] = false;
                ChangeTargetArray[i]["ErrorMsg"] = TempRespJson["ErrorMsg"];
                ChangeTargetArray[i]["ErrorMsg"].append("用户数据不合法");
                MyBasePtr->TRACE_ERROR(ChangeTargetArray[i]["ErrorMsg"]);
                continue;
            }
        }
    }

    for (int i = 0; i < ChangeSize; i++)
    {
        try
        {
            /* code */

            if (ChangeTargetArray[i].isMember("Result"))
            {
                MyBasePtr->DEBUGLog("第" + to_string(i) + "位修改者的数据不合法,跳过", true);
                continue;
            }
            MyBasePtr->DEBUGLog("第" + to_string(i) + "位修改者的数据合法,开始准备修改", true);

            MyBasePtr->DEBUGLog("开始查询用户 : " + to_string(ChangeTargetArray[i]["Change_ID"].asInt()), true);
            user = UserMgr.findByPrimaryKey(ChangeTargetArray[i]["Change_ID"].asInt());
            MyBasePtr->DEBUGLog("用户查询完毕 : " + user.toJson().toStyledString(), true);

            ChangeCount = 0;
            ChangeColSize = ChangeTargetArray[i]["Change_Col"].size();
            string ChangeColName, ChangeIntegralType;
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
                    if (ChangeIntegralNum <= 0)
                    {
                        MyBasePtr->DEBUGLog("字段(" + ChangeColName + ")没有产生变化", true);
                        continue;
                    }

                    if (ChangeIntegralType == "Sub")
                    {
                        ChangeIntegralNum = (-1) * ChangeIntegralNum;
                    }
                    else if (ChangeIntegralType != "Add")
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
            MyBasePtr->DEBUGLog("第" + to_string(i) + "位修改者的数据修改完成", true);

            // 统计更新的字段数
            MyBasePtr->DEBUGLog("统计第" + to_string(i) + "位修改者更新的字段数为" + to_string(ChangeCount), true);

            if (ChangeCount == 0)
            {
                MyBasePtr->DEBUGLog("第" + to_string(i) + "位修改者的数据无需更新", true);
                ChangeTargetArray[i]["ErrorMsg"].append("用户数据无需更新");
                ChangeTargetArray[i]["Result"] = false;
                continue;
            }

            MyBasePtr->DEBUGLog("开始更新第" + to_string(i) + "位修改者的数据", true);
            int row = UserMgr.update(user);
            if (row != 1)
            {
                ChangeTargetArray[i]["ErrorMsg"].append("用户资料更新失败");
                ChangeTargetArray[i]["Result"] = false;
                continue;
            }
            MyBasePtr->DEBUGLog("第" + to_string(i) + "位修改者的数据更新完成: " + user.toJson().toStyledString(), true);

            ChangeTargetArray[i]["Result"] = true;
        }
        catch (const drogon::orm::DrogonDbException &e)
        {
            if (e.base().what() == string("0 rows found"))
            {
                ChangeTargetArray[i]["ErrorMsg"].append("要更改的用户不存在");
            }
            else if (e.base().what() == string("Found more than one row"))
            {
                ChangeTargetArray[i]["ErrorMsg"].append("要更改的用户ID重复,请联系管理员");
            }
            else
            {
                ChangeTargetArray[i]["ErrorMsg"].append(e.base().what());
            }

            MyBasePtr->TRACE_ERROR(ChangeTargetArray[i]["ErrorMsg"]);
            ChangeTargetArray[i]["Result"] = false;
            continue;
        }
    }

    RespJson["Change_Target"] = ChangeTargetArray;
    MyBasePtr->INFO_Func("Admin_Update_User", false, RespJson);
}

// 自动审核章节更新的申请
/*
    自动审核章节更新的申请
Req:{
    "Processor_Type"          :   "",         //审核人类型(system/admin/root)
    "Processor_ID"       :   0,          //审核人ID
    "Examine_Result"     :   true/false, //审核结果
    "Upload_ID"          :   0,          //审核对象
    "Examine_Type"       :   "",         //审核对象类型
}
Resp:{
    "ErrorMsg"  :   "",         //  错误原因
    "Result"    :   true/false, //  执行结果
}
*/
void MyDBService::Auto_Examine_Chapter_Update(Json::Value &ReqJson, Json::Value &RespJson)
{
    auto MyJsonPtr = app().getPlugin<MyJson>();
    auto MyBasePtr = app().getPlugin<MyBase>();
    auto MyToolsPtr = app().getPlugin<MyTools>();
    MyBasePtr->INFO_Func("Auto_Examine_Chapter_Update", true, ReqJson);

    auto dbclientPrt = drogon::app().getDbClient();
    Mapper<drogon_model::novel::User> UserMgr(dbclientPrt);
    Mapper<drogon_model::novel::Upload> UploadMgr(dbclientPrt);
    Mapper<drogon_model::novel::Book> BookMgr(dbclientPrt);
    Mapper<drogon_model::novel::Chapter> ChapterMgr(dbclientPrt);

    // 插入前检查ReqJson是否合法
    try
    {
        MyBasePtr->DEBUGLog("插入前检查ReqJson是否合法", true);
        // "Processor_Type"          :   "",         //审核人类型(system/admin/root)
        // "Processor_ID"       :   0,          //审核人ID
        // "Examine_Result"     :   true/false, //审核结果
        // "Upload_ID"          :   0,          //审核对象
        // "Examine_Type"       :   "",         //审核对象类型
        std::map<string, MyJson::ColType> ColMap;
        ColMap["Upload_ID"] = MyJson::ColType::INT;
        ColMap["Processor_ID"] = MyJson::ColType::INT;
        ColMap["Examine_Type"] = MyJson::ColType::STRING;
        ColMap["Examine_Result"] = MyJson::ColType::BOOL;
        ColMap["Processor_Type"] = MyJson::ColType::STRING;
        MyJsonPtr->checkMemberAndTypeInMap(ReqJson, RespJson, ColMap);
        MyBasePtr->DEBUGLog("插入前检查显示ReqJson合法", true);
    }
    catch (Json::Value &e)
    {
        MyBasePtr->INFO_Func("Auto_Examine_Chapter_Update", false, RespJson);
        throw e;
    }

    drogon_model::novel::Upload ExamineUpload;
    // 获取审核对象
    try
    {
        ExamineUpload = UploadMgr.findByPrimaryKey(ReqJson["Upload_ID"].asInt());
    }
    catch (const drogon::orm::DrogonDbException &e)
    {
        if (e.base().what() == string("0 rows found"))
        {
            RespJson["ErrorMsg"].append("要处理的Upload_ID不存在");
        }
        else if (e.base().what() == string("Found more than one row"))
        {
            RespJson["ErrorMsg"].append("要处理的Upload_ID重复,请联系管理员");
        }
        else
        {
            RespJson["ErrorMsg"].append(e.base().what());
        }
        MyBasePtr->INFO_Func("Auto_Examine_Chapter_Update", false, RespJson);
        throw RespJson;
    }

    int UserID, Book_ID;
    string Book_Name, Book_Author, LoginStatus;
    Json::Value ContentJson, UploadMemo, ParaJson;
    drogon_model::novel::Book TBook;
    Book_ID = ExamineUpload.getValueOfBookId();

    try
    {
        TBook = BookMgr.findByPrimaryKey(Book_ID);
    }
    catch (...)
    {
        ReqJson["Examine_Explain"] = "章节创建失败(找不到对应的图书(ID=" + to_string(Book_ID) + "))";
        ReqJson["Examine_Result"] = false;
        Examine_Upload(ReqJson, RespJson);
        MyBasePtr->INFO_Func("Auto_Examine_Chapter_Update", false, RespJson);
        return;
    }

    MyJsonPtr->JsonstrToJson(ContentJson, ExamineUpload.getValueOfContent());
    pair<int, string> Title = MyToolsPtr->getTitleNumAndTitleStr(ContentJson["Chapter_Title"].asString());
    string ChapterTitle = Title.second;                         // 章节名
    int Part_Num;                                               // 章节卷
    int Chapter_Num = Title.first;                              // 章节数
    Json::Value Chapter_Content = ContentJson["Content_Array"]; // 章节内容
    Json::Value Chapter_Memo;                                   // 参数
    Json::Value BookMemo;
    Json::Value ChapterJson; //  Memo/Chapter

    // 章节卷需要查询Book表获取
    try
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
    catch (Json::Value &e)
    {
        ReqJson["Examine_Explain"] = e["ErrorMsg"];
        ReqJson["Examine_Result"] = false;
        Examine_Upload(ReqJson, RespJson);
        MyBasePtr->INFO_Func("Auto_Examine_Chapter_Update", false, RespJson);
        return;
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
        Criteria ChapterValid_cri(drogon_model::novel::Chapter::Cols::_Valid, CompareOperator::EQ, true);
        vector<drogon_model::novel::Chapter> vecChapter = ChapterMgr.findBy(BookID_cri && PartNum_cri && ChapterNum_cri && ChapterTitle_cri && ChapterValid_cri);
        // 如果不是已存在的章节，则将Upload状态改为已处理且拒绝插入
        if (vecChapter.size() == 0)
        {
            MyBasePtr->DEBUGLog("图书章节不存在", true);
            // 判断类型
            // 如果是新增章节则直接同意
            if(ReqJson["Examine_Type"].asString() == "Chapter")
            {
                ReqJson["Examine_Result"] = true;
                Examine_Upload(ReqJson, RespJson);
                MyBasePtr->INFO_Func("Auto_Examine_Chapter_Update", false, RespJson);
                return;
            }

            // 如果是更正章节则直接拒绝
            if(ReqJson["Examine_Type"].asString() == "Chapter_Update")
            {
                ReqJson["Examine_Explain"] = "要改正的图书章节不存在(BookID = " + to_string(Book_ID) + ",Part_Num = " + to_string(Part_Num) + ",Chapter_Num = " + to_string(Chapter_Num) + ",Title = " + ChapterTitle + "),请用正确的接口新增此章节";
                ReqJson["Examine_Result"] = false;
                Examine_Upload(ReqJson, RespJson);
                MyBasePtr->INFO_Func("Auto_Examine_Chapter_Update", false, RespJson);
                return;
            }
        }
        else
        {
            MyBasePtr->DEBUGLog("图书章节已存在", true);
            // 判断类型
            // 如果是新增章节则直接拒绝
            if(ReqJson["Examine_Type"].asString() == "Chapter")
            {
                ReqJson["Examine_Explain"] = "要上传的章节已存在";
                ReqJson["Examine_Result"] = false;
                Examine_Upload(ReqJson, RespJson);
                MyBasePtr->INFO_Func("Auto_Examine_Chapter_Update", false, RespJson);
                return;
            }

            // 如果是更正章节则等待管理员审核
            if(ReqJson["Examine_Type"].asString() == "Chapter_Update")
            {
                ReqJson["Examine_Explain"] = "等待管理员审核";
                ReqJson["Target_ID"] = vecChapter[0].getValueOfChapterId();
                ReqJson["Examine_Result"] = false;
                Examine_Upload(ReqJson, RespJson);
                MyBasePtr->INFO_Func("Auto_Examine_Chapter_Update", false, RespJson);
                return;
            }
        }
    }

    MyBasePtr->INFO_Func("Auto_Examine_Chapter_Update", false, RespJson);
}

// 创建帖子
/*
    创建帖子的接口
Req:{
    "Note_Title"    :   "", // 帖子标题,图书资源帖标题由书名(作者名)构成
    "Note_Content"  :   {
        "Content"   :   "",// 如果是图书资源则内容为图书简介
        "Book_ID"   :   0,// 只有图书资源有此项 连接到图书ID
    },
    "User_ID"       :   0,// 创建者ID，保存第一个创建图书者的ID
    "Note_Type"     :   "",// 帖子类型，(Help/Resource)
    "Processor_Type":   "",// System/root/admin有效
}
Resp:{
    "ErrorMsg"  :   "",//失败返回的错误信息
    "Result"    :   true/false,// 操作结果
    "Note_Data" :   {},// 成功返回的帖子信息
}
*/
void MyDBService::Create_Note(Json::Value &ReqJson, Json::Value &RespJson)
{
    auto MyJsonPtr = app().getPlugin<MyJson>();
    auto MyBasePtr = app().getPlugin<MyBase>();
    auto MyToolsPtr = app().getPlugin<MyTools>();
    MyBasePtr->INFO_Func("Create_Note", true, ReqJson);

    auto dbclientPrt = drogon::app().getDbClient();
    Mapper<drogon_model::novel::Note> NoteMgr(dbclientPrt);
    Mapper<drogon_model::novel::Book> BookMgr(dbclientPrt);

    int UserID;
    string NoteTitle, NoteType, ProcessorType, NoteContent;
    Json::Value NoteContentJson;
    // 插入前检查帖子数据是否合法
    try
    {
        MyBasePtr->DEBUGLog("插入前检查帖子数据是否合法", true);
        // "Note_Title"    :   "", // 帖子标题,图书资源帖标题由书名(作者名)构成
        // "Note_Content"  :   {},
        // "User_ID"       :   0,// 创建者ID，保存第一个创建图书者的ID
        // "Note_Type"     :   "",// 帖子类型，(Help/Resource)
        // "Processor_Type":   "",// System/root/admin有效
        // 创建检查列表 key是字段名 value 是字段类型
        std::map<string, MyJson::ColType> ColMap;
        ColMap["Note_Title"] = MyJson::ColType::STRING;
        ColMap["Note_Content"] = MyJson::ColType::JSON;
        ColMap["User_ID"] = MyJson::ColType::INT;
        ColMap["Note_Type"] = MyJson::ColType::STRING;
        ColMap["Processor_Type"] = MyJson::ColType::STRING;
        MyJsonPtr->checkMemberAndTypeInMap(ReqJson, RespJson, ColMap);
        MyBasePtr->DEBUGLog("插入前检查显示帖子数据合法", true);

        UserID = atoi(ReqJson["User_ID"].asString().c_str());
        NoteTitle = ReqJson["Note_Title"].asString();
        NoteType = ReqJson["Note_Type"].asString();
        ProcessorType = ReqJson["Processor_Type"].asString();
        NoteContentJson = ReqJson["Note_Content"];

        MyBasePtr->DEBUGLog("开始检查操作权限", true);

        if (ReqJson["Processor_Type"].asString() != "user" && ReqJson["Processor_Type"].asString() != "system" && ReqJson["Processor_Type"].asString() != "admin" && ReqJson["Processor_Type"].asString() != "root")
        {
            RespJson["ErrorMsg"].append("用户权限(" + ReqJson["Processor_Type"].asString() + ")不足,请联系管理员"); //失败返回的错误信息
            RespJson["Result"] = false;                                                                             // 操作结果
            throw RespJson;
        }
        MyBasePtr->DEBUGLog("操作权限检查通过", true);

        // "Note_Content"  :   {
        //     "Content"   :   "",// 如果是图书资源则内容为图书简介
        //     "Book_ID"   :   0,// 只有图书资源有此项 连接到图书ID
        // },
        MyBasePtr->DEBUGLog("开始检查NoteContentJson数据格式", true);
        ColMap.clear();
        ColMap["Content"] = MyJson::ColType::STRING;
        MyJsonPtr->checkMemberAndTypeInMap(NoteContentJson, RespJson, ColMap);
        MyBasePtr->DEBUGLog("检查NoteContentJson数据格式完成", true);

        NoteContent = NoteContentJson["Content"].asString();
        MyBasePtr->DEBUGLog("开始检查数据正确性", true);

        {
            if (NoteType != "Help" && NoteType != "Resource")
            {
                RespJson["ErrorMsg"].append("新增帖子类型(" + NoteType + ")错误,请选择[Help/Resource]"); //失败返回的错误信息
                RespJson["Result"] = false;                                                              // 操作结果
                throw RespJson;
            }

            if (NoteTitle.empty())
            {
                RespJson["ErrorMsg"].append("新增帖子标题不能为空"); //失败返回的错误信息
                RespJson["Result"] = false;                          // 操作结果
                throw RespJson;
            }

            if (NoteContent.empty())
            {
                RespJson["ErrorMsg"].append("新增帖子内容不能为空"); //失败返回的错误信息
                RespJson["Result"] = false;                          // 操作结果
                throw RespJson;
            }

            if (NoteType == "Resource")
            {
                if (!NoteContentJson.isMember("Book_ID"))
                {
                    RespJson["ErrorMsg"].append("新增资源贴没有图书ID信息"); //失败返回的错误信息
                    RespJson["Result"] = false;                              // 操作结果
                    throw RespJson;
                }

                if (!NoteContentJson["Book_ID"].isInt())
                {
                    RespJson["ErrorMsg"].append("新增资源贴的图书ID不是数字"); //失败返回的错误信息
                    RespJson["Result"] = false;                          // 操作结果
                    throw RespJson;
                }
                try
                {
                    drogon_model::novel::Book book = BookMgr.findByPrimaryKey(NoteContentJson["Book_ID"].asInt());
                    if (book.getValueOfStatus() == "禁止")
                    {
                        RespJson["ErrorMsg"].append("新增资源贴的图书ID已被禁止"); //失败返回的错误信息
                        RespJson["Result"] = false;                          // 操作结果
                        throw RespJson;
                    }
                }
                catch (...)
                {
                    RespJson["ErrorMsg"].append("新增资源贴的图书ID无效"); //失败返回的错误信息
                    RespJson["Result"] = false;                      // 操作结果
                    throw RespJson;
                }
            }
        }

        MyBasePtr->DEBUGLog("数据正确性检查通过", true);
    }
    catch (Json::Value &e)
    {
        RespJson["ErrorMsg"].append("帖子创建失败(要创建的帖子数据不合法(" + e["ErrorMsg"].asString() + "))");
        RespJson["Result"] = false;
        MyBasePtr->INFO_Func("Create_Note", false, RespJson);
        return;
    }

    drogon_model::novel::Note NewNote;

    // 检查帖子是否已存在
    {
        MyBasePtr->DEBUGLog("开始检查帖子是否已存在", true);
        Criteria NoteTitle_cri(drogon_model::novel::Note::Cols::_Title, CompareOperator::EQ, NoteTitle);
        vector<drogon_model::novel::Note> vecNote = NoteMgr.findBy(NoteTitle_cri);

        // 如果不是新书，则将Upload状态改为已处理且拒绝插入
        if (vecNote.size() != 0)
        {
            RespJson["ErrorMsg"].append("帖子创建失败(要创建的帖子已存在(NoteID = " + to_string(vecNote[0].getValueOfNoteId()) + "))");
            RespJson["Result"] = false;
            MyBasePtr->INFO_Func("Create_Note", false, RespJson);
            return;
        }

        MyBasePtr->DEBUGLog("要创建的帖子不存在", true);
    }

    // 准备要创建的新帖子的数据
    {
        MyBasePtr->DEBUGLog("开始准备新帖子数据", true);
        NewNote.setCommentCount(0);                           // 帖子评论量
        NewNote.setUserId(UserID);                            // 创建者
        NewNote.setType(NoteType);                            // 帖子类型
        NewNote.setStatus("allow");                           // 状态 allow/forbid
        NewNote.setTitle(NoteTitle);                          // 帖子标题
        NewNote.setContent(NoteContentJson.toStyledString()); // 帖子内容
        MyBasePtr->DEBUGLog("准备新帖子数据完成", true);
    }

    // 准备插入帖子数据
    try
    {
        MyBasePtr->DEBUGLog("准备插入的新帖子数据为 : " + NewNote.toJson().toStyledString(), true);
        MyBasePtr->DEBUGLog("准备插入新帖子数据", true);
        NoteMgr.insert(NewNote);
        MyBasePtr->DEBUGLog("插入新帖子数据完毕", true);
        MyBasePtr->DEBUGLog("插入的新数据为 : " + NewNote.toJson().toStyledString(), true);
        RespJson["Note_Data"] = NewNote.toJson();
        RespJson["Result"] = true;
    }
    // 新建帖子失败
    catch (const drogon::orm::DrogonDbException &e)
    {
        RespJson["Result"] = false;
        RespJson["ErrorMsg"].append("图书资源帖创建失败(" + string(e.base().what()) + ")");
        MyBasePtr->INFO_Func("Create_Note", false, RespJson);
        return;
    }
    catch (...)
    {
        RespJson["Result"] = false;
        RespJson["ErrorMsg"].append("图书资源帖创建失败(数据异常)");
        MyBasePtr->INFO_Func("Create_Note", false, RespJson);
        return;
    }
    MyBasePtr->INFO_Func("Create_Note", false, RespJson);
    return;
}

// 用户下载已有的书的章节
void MyDBService::Download_Resource(Json::Value &ReqJson, Json::Value &RespJson)
{
    Json::Value ParaJson;
    auto MyJsonPtr = app().getPlugin<MyJson>();
    auto MyBasePtr = app().getPlugin<MyBase>();
    auto MyToolsPtr = app().getPlugin<MyTools>();
    MyBasePtr->INFO_Func("Download_Resource", true, ReqJson);

    auto dbclientPrt = drogon::app().getDbClient();
    Mapper<drogon_model::novel::User> UserMgr(dbclientPrt);
    Mapper<drogon_model::novel::Upload> UploadMgr(dbclientPrt);
    Mapper<drogon_model::novel::Book> BookMgr(dbclientPrt);
    Mapper<drogon_model::novel::Chapter> ChapterMgr(dbclientPrt);

    int UserID, Book_ID;
    string LoginStatus;
    // 检查ReqJson数据
    try
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
        if ((LoginStatus != "user") && (LoginStatus != "admin") && (LoginStatus != "root"))
        {
            RespJson["ErrorMsg"].append("账户权限错误,请联系管理员");
            throw RespJson;
        }
        Book_ID = ReqJson["Book_ID"].asInt();
    }
    catch (Json::Value &e)
    {
        MyBasePtr->INFO_Func("Download_Resource", false, RespJson);
        throw e;
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
        RespJson["ErrorMsg"].append("登入的UserID(" + to_string(UserID) + ")与查询到的UserID(" + to_string(user.getValueOfUserId()) + ")不同");
        MyBasePtr->INFO_Func("Download_Resource", false, RespJson);
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
            RespJson["ErrorMsg"].append("要下载的图书不存在");
            MyBasePtr->INFO_Func("Download_Resource", false, RespJson);
            throw RespJson;
        }
        MyBasePtr->DEBUGLog("要下载的图书存在", true);
        TBook = vecBook[0];
    }

    // 检查Book/Memo数据是否合法
    try
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
    catch (Json::Value &e)
    {
        MyBasePtr->INFO_Func("Download_Resource", false, RespJson);
        throw e;
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
            RespJson["ErrorMsg"].append("要下载的图书的章节都不存在");
            MyBasePtr->INFO_Func("Download_Resource", false, RespJson);
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
    MyBasePtr->INFO_Func("Download_Resource", false, RespJson);
}

// 系统/管理员审核用户上传申请的接口
/*
    系统/管理员审核用户上传申请的接口
Req:{
    "Processor_Type"     :   "",         //审核人类型(system/admin/root)
    "Processor_ID"       :   0,          //审核人ID
    "Examine_Result"     :   true/false, //审核结果
    "Upload_ID"          :   0,          //审核对象
    "Examine_Type"       :   "",         //审核对象类型(Book/Chapter)
    "Examine_Explain"    :   "",         //审核拒绝的备注
}
// 最终结果为Processor_Type:Examine_Result/实际处理情况
Resp:{
    "ErrorMsg"  :   "",         //  错误原因
    "Result"    :   true/false, //  执行结果
}
*/
void MyDBService::Examine_Upload(Json::Value &ReqJson, Json::Value &RespJson)
{
    Json::Value UploadMemo;
    auto MyBasePtr = app().getPlugin<MyBase>();
    auto MyJsonPtr = app().getPlugin<MyJson>();
    MyBasePtr->INFO_Func("Examine_Upload", true, ReqJson);

    auto dbclientPrt = drogon::app().getDbClient();
    Mapper<drogon_model::novel::Upload> UploadMgr(dbclientPrt);
    drogon_model::novel::Upload ExamineUpload;

    // 插入前检查ReqJson是否合法
    try
    {
        MyBasePtr->DEBUGLog("插入前检查ReqJson是否合法", true);
        // "Processor_Type"          :   "",         //审核人类型(system/admin/root)
        // "Processor_ID"       :   0,          //审核人ID
        // "Examine_Result"     :   true/false, //审核结果
        // "Upload_ID"          :   0,          //审核对象
        // "Examine_Type"       :   "",         //审核对象类型
        std::map<string, MyJson::ColType> ColMap;
        ColMap["Upload_ID"] = MyJson::ColType::INT;
        ColMap["Processor_ID"] = MyJson::ColType::INT;
        ColMap["Examine_Type"] = MyJson::ColType::STRING;
        ColMap["Examine_Result"] = MyJson::ColType::BOOL;
        ColMap["Processor_Type"] = MyJson::ColType::STRING;
        MyJsonPtr->checkMemberAndTypeInMap(ReqJson, RespJson, ColMap);
        if (ReqJson["Examine_Result"].asBool() == false)
        {
            ColMap.clear();
            // "Examine_Explain"    :   "",         //审核拒绝的备注
            ColMap["Examine_Explain"] = MyJson::ColType::STRING;
            MyJsonPtr->checkMemberAndTypeInMap(ReqJson, RespJson, ColMap);
        }
        MyBasePtr->DEBUGLog("插入前检查显示ReqJson合法", true);
    }
    catch (Json::Value &e)
    {
        MyBasePtr->INFO_Func("Examine_Upload", false, RespJson);
        throw e;
    }

    // 获取审核对象
    try
    {
        ExamineUpload = UploadMgr.findByPrimaryKey(ReqJson["Upload_ID"].asInt());
    }
    catch (const drogon::orm::DrogonDbException &e)
    {
        if (e.base().what() == string("0 rows found"))
        {
            RespJson["ErrorMsg"].append("要处理的Upload_ID不存在");
        }
        else if (e.base().what() == string("Found more than one row"))
        {
            RespJson["ErrorMsg"].append("要处理的Upload_ID重复,请联系管理员");
        }
        else
        {
            RespJson["ErrorMsg"].append(e.base().what());
        }
        RespJson["Result"] = false;
        MyBasePtr->INFO_Func("Examine_Upload", false, RespJson);
        throw RespJson;
    }

    if (ExamineUpload.getValueOfIsmanage() == true)
    {
        RespJson["ErrorMsg"].append("这个申请已经被处理过了");
        RespJson["Result"] = false;
        throw RespJson;
    }
    string Processor;

    // 获取审核人信息
    {

        if (ReqJson["Processor_Type"] == "system")
        {
            Processor = "system";
        }
        else if (ReqJson["Processor_Type"] == "root")
        {
            Processor = "root(" + ReqJson["Processor_ID"].asString() + ")";
        }
        else if (ReqJson["Processor_Type"] == "admin")
        {
            Processor = "admin";
        }
        else
        {
            RespJson["ErrorMsg"].append("Processor_Type字段类型错误");
            RespJson["Result"] = false;
            MyBasePtr->INFO_Func("Examine_Upload", false, RespJson);
            throw RespJson;
        }
    }

    bool IsBook, InsertResult, RespResult, ExamineResult;
    Json::Value ContentJson, MemoJson, TempJson, TempArrayJson, TempColArrayJson, TempMemoJson;
    MyJsonPtr->JsonstrToJson(ContentJson, ExamineUpload.getValueOfContent());
    MyJsonPtr->JsonstrToJson(MemoJson, ExamineUpload.getValueOfMemo());

    ExamineResult = ReqJson["Examine_Result"].asBool();
    /*
        修改判断流程
        如果是不允许，
            插入新书或新章节则直接拒绝，
            插入更正章节则判断审核人，如果是system则修改为等待管理员审核，否则拒绝
        如果是允许
            分别调用函数执行
            返回bool，参数自带一个bool
            返回值代表操作执行正常，参数代表数据修改是否完成
            00  程序执行异常，数据修改失败，等待管理员二次处理
            01  程序执行异常，数据修改完成，正常情况下不会出现此情况
            10  程序执行正常，数据修改失败，可能是数据格式不对
            11  程序执行正常，数据修改完成，完美
    */
    // 如果审核结果是允许，则开始审核等待结果
    ExamineUpload.setProcessor(Processor);
    if (ExamineResult)
    {
        if (ReqJson["Examine_Type"].asString() == "Book")
        {
            InsertResult = Insert_Book(ContentJson, RespJson);
        }
        else if (ReqJson["Examine_Type"].asString() == "Chapter")
        {
            ContentJson["Book_ID"] = ExamineUpload.getValueOfBookId();
            ContentJson["User_ID"] = ExamineUpload.getValueOfUserId();
            InsertResult = Insert_Chapter(ContentJson, RespJson);
        }
        else if (ReqJson["Examine_Type"].asString() == "Chapter_Update")
        {
            TempJson.clear();
            if (!MemoJson.isMember("Target_ID"))
            {
                // 错误,没有指向要修改的对象
                RespJson["Result"] = false;
                RespJson["ErrorMsg"].append("申请更新章节的备注中没有包含Target_ID字段");
                MyBasePtr->INFO_Func("Examine_Upload", false, RespJson);
                throw RespJson;
            }

            TempJson["Change_ID"] = MemoJson["Target_ID"].asInt();
            TempJson["Processor_Type"] = ReqJson["Processor_Type"].asString();
            TempJson["Change_Valid"] = false;
            TempMemoJson["Status"] = "老版本";
            TempJson["Change_Memo"] = TempMemoJson;

            bool UpdateRes = Update_Chapter(TempJson, RespJson);

            // 更新成功
            if (UpdateRes)
            {
                ContentJson["Book_ID"] = ExamineUpload.getValueOfBookId();
                ContentJson["User_ID"] = ExamineUpload.getValueOfUserId();
                ContentJson["Version"] = RespJson["Chapter_Data"]["Version"].asInt() + 1;
                InsertResult = Insert_Chapter(ContentJson, RespJson);
            }
        }
        else
        {
            RespJson["ErrorMsg"].append("Examine_Type字段类型错误");
            RespJson["Result"] = false;
            MyBasePtr->INFO_Func("Examine_Upload", false, RespJson);
            throw RespJson;
        }

        RespResult = RespJson["Result"].asBool();

        // 图书创建后更新Upload信息
        MyBasePtr->DEBUGLog("操作完成,开始更新Upload数据", true);

        UploadMemo["Explain"] = RespJson["Explain"].asString();
        ExamineUpload.setMemo(UploadMemo.toStyledString());

        // 审核不允许则直接完成 或者 审核允许且操作正常 才完结
        // InsertResult代表操作是否正常 "Result"字段代表是否成功插入
        if (InsertResult)
        {
            if (RespResult)
            {
                MyBasePtr->DEBUGLog("操作过程顺利," + UploadMemo["Explain"].asString(), true);
                ExamineUpload.setIsmanage(true);
                // 如果处理的是创建图书的任务，则要更新一下图书ID
                if (ReqJson["Examine_Type"].asString() == "Book")
                    ExamineUpload.setBookId(RespJson["Book_Data"]["Book_ID"].asInt());
            }
            else
            {
                MyBasePtr->DEBUGLog("操作过程出现问题::" + UploadMemo["Explain"].asString(), true);
                ExamineUpload.setIsmanage(false);
            }
        }
        // 程序异常情况下，设置为未处理，等待管理员二次处理
        else
        {
            ExamineUpload.setIsmanage(false);
        }
        ExamineUpload.setStatus(Processor + "已允许/" + RespJson["Upload_Status"].asString());
    }
    else
    {
        UploadMemo["Explain"] = ReqJson["Examine_Explain"].asString();
        ExamineUpload.setStatus(Processor + "已拒绝");
        ExamineUpload.setIsmanage(true);
        if (ReqJson["Examine_Type"].asString() == "Chapter_Update" && ReqJson.isMember("Target_ID"))
        {
            UploadMemo["Target_ID"] = ReqJson["Target_ID"].asInt();
            ExamineUpload.setIsmanage(false);
            ExamineUpload.setStatus("待处理");
        }
        ExamineUpload.setMemo(UploadMemo.toStyledString());
    }

    int UpdateCount = UploadMgr.update(ExamineUpload);

    MyBasePtr->DEBUGLog("更新的新数据为 : " + ExamineUpload.toJson().toStyledString(), true);
    if (!UpdateCount)
    {
        RespJson["Result"] = false;
        RespJson["ErrorMsg"].append("更新Upload数据失败");
        MyBasePtr->INFO_Func("Examine_Upload", false, RespJson);
        throw RespJson;
    }
    MyBasePtr->DEBUGLog("更新Upload数据完毕", true);
    MyBasePtr->INFO_Func("Examine_Upload", false, RespJson);
}

//根据Upload_ID返回审核类型
string MyDBService::Get_Upload_Type(int Upload_ID)
{
    Json::Value UploadMemo, UploadContent;
    auto MyBasePtr = app().getPlugin<MyBase>();
    auto MyJsonPtr = app().getPlugin<MyJson>();

    auto dbclientPrt = drogon::app().getDbClient();
    Mapper<drogon_model::novel::Upload> UploadMgr(dbclientPrt);
    drogon_model::novel::Upload ExamineUpload;

    MyBasePtr->DEBUGLog("查询Upload(ID = " + to_string(Upload_ID) + ")类型", true);

    // 获取审核对象

    ExamineUpload = UploadMgr.findByPrimaryKey(Upload_ID);
    MyJsonPtr->JsonstrToJson(UploadMemo, ExamineUpload.getValueOfMemo());
    MyJsonPtr->JsonstrToJson(UploadContent, ExamineUpload.getValueOfContent());
    if (UploadMemo.isMember("Target_ID"))
        return "Chapter_Update";
    if (UploadContent.isMember("Chapter_Title"))
        return "Chapter";
    if (UploadContent.isMember("Book_Author"))
        return "Book";
    return "Error";
}


// 根据书名和作者名查询图书是否存在
/*
    根据书名和作者名查询图书是否存在  
Req:{
    "Book_Author"   :   "",
    "Book_Name"     :   ""
}
Resp:{
    "Search_Book_ID"    :   0,// 找到的图书ID
}
*/
bool MyDBService::Is_Book_Exist(Json::Value &ReqJson, Json::Value &RespJson)
{
    auto MyJsonPtr = app().getPlugin<MyJson>();
    auto MyBasePtr = app().getPlugin<MyBase>();
    auto MyToolsPtr = app().getPlugin<MyTools>();
    MyBasePtr->INFO_Func("Is_Book_Exist", true, ReqJson);

    auto dbclientPrt = drogon::app().getDbClient();
    Mapper<drogon_model::novel::Book> BookMgr(dbclientPrt);
    // 插入前检查图书数据是否合法

    MyBasePtr->DEBUGLog("搜索前检查图书数据是否合法", true);
    // "Book_Author"   :   "",
    // "Book_Name"   :   "",
    // 创建检查列表 key是字段名 value 是字段类型
    std::map<string, MyJson::ColType> ColMap;
    ColMap["Book_Name"] = MyJson::ColType::STRING;
    ColMap["Book_Author"] = MyJson::ColType::STRING;
    MyJsonPtr->checkMemberAndTypeInMap(ReqJson, RespJson, ColMap);
    MyBasePtr->DEBUGLog("搜索前检查显示图书数据合法", true);


    string BookName = ReqJson["Book_Name"].asString();
    string BookAuthor = ReqJson["Book_Author"].asString();
    // 检查图书是否已存在
    MyBasePtr->DEBUGLog("开始检查图书是否已存在", true);
    Criteria BookName_cri(drogon_model::novel::Book::Cols::_Name, CompareOperator::EQ, BookName);
    Criteria BookAuthor_cri(drogon_model::novel::Book::Cols::_Author, CompareOperator::EQ, BookAuthor);
    vector<drogon_model::novel::Book> vecBook = BookMgr.findBy(BookName_cri && BookAuthor_cri);

    // 如果不是新书，则将Upload状态改为已处理且拒绝插入
    if (vecBook.size() != 0)
    {
        RespJson["Search_Book_ID"] = vecBook[0].getValueOfBookId();
        MyBasePtr->INFO_Func("Is_Book_Exist", false, RespJson);
        return true;
    }
    MyBasePtr->INFO_Func("Is_Book_Exist", false, RespJson);
    return false;
}


// 搜索帖子
/*
    搜索帖子的接口
Req:{
    "Note_KeyWord"  :   "",// 关键字,在标题和内容中查找
    "User_ID"       :   "",// 创建者ID，保存第一个创建图书者的ID
    "Note_Type"     :   "",// 帖子类型，(Help/Resource)
    "Limit"         :   0, //
    "Offset"        :   0, //
}
Resp:{
    "ErrorMsg"  :   "",         // 失败返回的错误信息
    "Result"    :   true/false, // 操作结果
    "Note_List" :   [],         // 成功返回的帖子信息
}
*/
void MyDBService::Search_Note(Json::Value &ReqJson, Json::Value &RespJson)
{
    auto MyBasePtr = app().getPlugin<MyBase>();
    auto MyJsonPtr = app().getPlugin<MyJson>();
    MyBasePtr->INFO_Func("Search_Note", true, ReqJson);

    auto dbclientPrt = drogon::app().getDbClient();
    Mapper<drogon_model::novel::Note> NoteMgr(dbclientPrt);

    int limit, offset;
    string KeyWord, NoteType, UserID;

    // 检查ReqJson数据是否合法
    try
    {
        MyBasePtr->DEBUGLog("开始检查ReqJson数据是否合法", true);
        // "Note_KeyWord"  :   "",// 关键字,在标题和内容中查找
        // "User_ID"       :   "",// 创建者ID，保存第一个创建图书者的ID
        // "Note_Type"     :   "",// 帖子类型，(Help/Resource)
        // "Limit"         :   0, //
        // "Offset"        :   0, //
        std::map<string, MyJson::ColType> ColMap;
        ColMap["Limit"] = MyJson::ColType::INT;
        ColMap["Offset"] = MyJson::ColType::INT;
        ColMap["User_ID"] = MyJson::ColType::STRING;
        ColMap["Note_Type"] = MyJson::ColType::STRING;
        ColMap["Note_KeyWord"] = MyJson::ColType::STRING;
        MyJsonPtr->checkMemberAndTypeInMap(ReqJson, RespJson, ColMap);
        MyBasePtr->DEBUGLog("ReqJson数据合法", true);

        limit = ReqJson["Limit"].asInt();
        offset = ReqJson["Offset"].asInt();
        UserID = ReqJson["User_ID"].asString();
        NoteType = ReqJson["Note_Type"].asString();
        KeyWord = ReqJson["Note_KeyWord"].asString();
    }
    catch (Json::Value &RespJson)
    {
        RespJson["Result"] = false;
        MyBasePtr->INFO_Func("Search_Note", false, RespJson);
        return;
    }
    catch (...)
    {
        RespJson["Result"] = false;
        RespJson["ErrorMsg"].append("MyDBService::Search_Note::Error");
        MyBasePtr->INFO_Func("Search_Note", false, RespJson);
        return;
    }

    // 查询指定帖子
    try
    {
        // 制作筛选条件
        Criteria UserID_cri = Criteria(drogon_model::novel::Note::Cols::_User_ID, CompareOperator::Like, "%" + UserID + "%");
        Criteria NoteTitle_cri = Criteria(drogon_model::novel::Note::Cols::_Title, CompareOperator::Like, "%" + KeyWord + "%");
        Criteria NoteContent_cri = Criteria(drogon_model::novel::Note::Cols::_Content, CompareOperator::Like, "%" + KeyWord + "%");
        Criteria NoteType_cri(drogon_model::novel::Note::Cols::_Type, CompareOperator::Like, "%" + NoteType + "%");

        if (offset > 0)
        {
            NoteMgr.offset(offset);
        }

        if (limit > 0)
        {
            NoteMgr.limit(limit);
        }

        MyBasePtr->DEBUGLog("开始查询指定帖子", true);
        std::vector<drogon_model::novel::Note> vecNote = NoteMgr.findBy(UserID_cri && (NoteTitle_cri || NoteContent_cri) && NoteType_cri);
        Json::Value vecJsonVal, TempNoteJson;
        for (auto &note : vecNote)
        {
            TempNoteJson.clear();
            TempNoteJson = note.toJson();
            TempNoteJson.removeMember("Status");
            vecJsonVal.append(TempNoteJson);
            // std::cout << user.toJson().toStyledString() <<std::endl;
        }
        MyBasePtr->DEBUGLog("查询指定帖子完毕", true);
        RespJson["Result"] = true;
        RespJson["Note_List"] = vecJsonVal;
        MyBasePtr->INFO_Func("Search_Note", false, RespJson);
        return;
    }
    catch (...)
    {
        RespJson["Result"] = false;
        RespJson["ErrorMsg"].append("帖子查询失败");
        MyBasePtr->INFO_Func("Search_Note", false, RespJson);
        return;
    }
}

// 用户查询用户个人资料的接口
void MyDBService::Search_User_PersonalData(Json::Value &ReqJson, Json::Value &RespJson)
{
    auto MyBasePtr = app().getPlugin<MyBase>();
    auto MyJsonPtr = app().getPlugin<MyJson>();
    MyBasePtr->INFO_Func("Search_User_PersonalData", true, ReqJson);

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
        if ((LoginStatus != "user") && (LoginStatus != "admin") && (LoginStatus != "root"))
        {
            RespJson["ErrorMsg"].append("账户权限错误,请联系管理员");
            MyBasePtr->INFO_Func("Search_User_PersonalData", false, RespJson);
            throw RespJson;
        }
    }

    MyBasePtr->DEBUGLog("开始查询用户 : " + ParaJson["User_ID"].asString(), true);
    drogon_model::novel::User user = UserMgr.findByPrimaryKey(UserID);
    MyBasePtr->DEBUGLog("用户查询完毕 : " + user.toJson().toStyledString(), true);

    RespJson["User_Data"] = user.toJson();
    MyBasePtr->INFO_Func("Search_User_PersonalData", false, RespJson);
}

// 用户上传新书的接口
/*
    用户上传新书的接口
Req:{
    // "Content"    :   "", // 插入的内容
    // "Book_Author":   "", // 作者
    // "Book_Name"  :   "", // 书名
    // "Para"       :   {}  // 用户Token携带的信息
}
Resp:{
    "ErrorMsg"  :   [],         //  错误原因
    "Result"    :   true/false, //  执行结果
    "Upload_ID" :   0,          //  新插入的申请的ID(只有在成功的时候有)
}
*/
void MyDBService::Upload_Book(Json::Value &ReqJson, Json::Value &RespJson)
{
    auto MyJsonPtr = app().getPlugin<MyJson>();
    auto MyBasePtr = app().getPlugin<MyBase>();
    auto MyToolsPtr = app().getPlugin<MyTools>();
    MyBasePtr->INFO_Func("Upload_Book", true, ReqJson);

    auto dbclientPrt = drogon::app().getDbClient();
    Mapper<drogon_model::novel::User> UserMgr(dbclientPrt);
    Mapper<drogon_model::novel::Upload> UploadMgr(dbclientPrt);

    int UserID;
    string LoginStatus;
    Json::Value ContentJson, UploadMemo, ParaJson;

    // 检查ReqJson数据
    try
    {
        MyBasePtr->DEBUGLog("开始检查ReqJson数据", true);
        // "Content"   :   "",
        // "Book_Author":"爱潜水的乌贼",//作者
        // "Book_Name":"诡秘之主", // 书名
        // "Para":   {}
        // 创建检查列表 key是字段名 value 是字段类型
        std::map<string, MyJson::ColType> ColMap;
        ColMap["Content"] = MyJson::ColType::JSON;
        ColMap["Book_Author"] = MyJson::ColType::STRING;
        ColMap["Book_Name"] = MyJson::ColType::STRING;
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
        if ((LoginStatus != "user") && (LoginStatus != "admin") && (LoginStatus != "root"))
        {
            RespJson["ErrorMsg"].append("账户权限错误,请联系管理员");
            RespJson["Result"] = false;
            MyBasePtr->INFO_Func("Upload_Book", false, RespJson);
            throw RespJson;
        }
        ContentJson = ReqJson["Content"];
    }
    catch (Json::Value &e)
    {
        MyBasePtr->INFO_Func("Upload_Book", false, RespJson);
        throw e;
    }

    // 插入前检查数据是否合法
    try
    {
        // 检查content数据是否合法
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
                RespJson["ErrorMsg"].append("Book_Name字段与Content/Book_Name不同");
                MyBasePtr->INFO_Func("Upload_Book", false, RespJson);
                throw RespJson;
            }
            if (ReqJson["Book_Author"].asString() != ContentJson["Book_Author"].asString())
            {
                RespJson["ErrorMsg"].append("Book_Author字段与Content/Book_Author不同");
                MyBasePtr->INFO_Func("Upload_Book", false, RespJson);
                throw RespJson;
            }
            MyBasePtr->DEBUGLog("核查数据合法性完成", true);
        }
    }
    catch (Json::Value &e)
    {
        RespJson["ErrorMsg"].append("content字段数据不合法");
        RespJson["Result"] = false;
        MyBasePtr->INFO_Func("Upload_Book", false, RespJson);
        throw RespJson;
    }

    MyBasePtr->DEBUGLog("开始查询用户 : " + to_string(UserID), true);
    drogon_model::novel::User user = UserMgr.findByPrimaryKey(UserID);
    MyBasePtr->DEBUGLog("用户查询完毕 : " + user.toJson().toStyledString(), true);

    // 验证用户
    if (user.getValueOfUserId() != UserID)
    {
        RespJson["ErrorMsg"].append("登入的UserID(" + to_string(UserID) + ")与查询到的UserID(" + to_string(user.getValueOfUserId()) + ")不同");
        RespJson["Result"] = false;
        MyBasePtr->INFO_Func("Upload_Book", false, RespJson);
        throw RespJson;
    }

    drogon_model::novel::Upload NewUpload;

    try
    {
        // 准备要处理的新数据
        {
            MyBasePtr->DEBUGLog("开始准备要处理的新数据", true);
            NewUpload.setBookId(0);
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
    }
    catch (const drogon::orm::DrogonDbException &e)
    {
        RespJson["ErrorMsg"].append("Upload插入失败(" + string(e.base().what()) + ")");
        RespJson["Result"] = false;
        MyBasePtr->INFO_Func("Upload_Book", false, RespJson);
        throw RespJson;
    }

    RespJson["Result"] = true;
    RespJson["Upload_ID"] = NewUpload.getValueOfUploadId();
    MyBasePtr->INFO_Func("Upload_Book", false, RespJson);
}

// 用户上传已有的书的后续章节的接口
/*
    用户上传已有的书的后续章节的接口
Req:{
    "Book_ID"    :   0,  // 插入章节对应的图书ID
    "Content"    :   "", // 插入的内容
    "Book_Author":   "", // 作者
    "Book_Name"  :   "", // 书名
    "Para"       :   {}  // 用户Token携带的信息
}

Resp:{
    "ErrorMsg"  :   [],         //  错误原因
    "Result"    :   true/false, //  执行结果
    "Upload_ID" :   0,          //  新插入的申请的ID(只有在成功的时候有)
}
*/
void MyDBService::Upload_Chapter(Json::Value &ReqJson, Json::Value &RespJson)
{
    auto MyJsonPtr = app().getPlugin<MyJson>();
    auto MyBasePtr = app().getPlugin<MyBase>();
    auto MyToolsPtr = app().getPlugin<MyTools>();
    MyBasePtr->INFO_Func("Upload_Chapter", true, ReqJson);

    auto dbclientPrt = drogon::app().getDbClient();
    Mapper<drogon_model::novel::User> UserMgr(dbclientPrt);
    Mapper<drogon_model::novel::Upload> UploadMgr(dbclientPrt);
    Mapper<drogon_model::novel::Book> BookMgr(dbclientPrt);
    Mapper<drogon_model::novel::Chapter> ChapterMgr(dbclientPrt);

    int UserID, Book_ID;
    string Book_Name, Book_Author, LoginStatus;
    Json::Value ContentJson, UploadMemo, ParaJson;
    // 检查数据完整性
    try
    {
        MyBasePtr->DEBUGLog("开始检查数据完整性", true);
        // "Book_ID"   :   0,
        // "Content"   :   "",
        // "Book_Author":"爱潜水的乌贼",//作者
        // "Book_Name":"诡秘之主", // 书名
        // "Para":   {}
        // 创建检查列表 key是字段名 value 是字段类型
        std::map<string, MyJson::ColType> ColMap;
        ColMap["Book_ID"] = MyJson::ColType::INT;
        ColMap["Content"] = MyJson::ColType::JSON;
        ColMap["Book_Author"] = MyJson::ColType::STRING;
        ColMap["Book_Name"] = MyJson::ColType::STRING;
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
        if ((LoginStatus != "user") && (LoginStatus != "admin") && (LoginStatus != "root"))
        {
            RespJson["ErrorMsg"].append("账户权限错误,请联系管理员");
            RespJson["Result"] = false;
            MyBasePtr->INFO_Func("Upload_Chapter", false, RespJson);
            throw RespJson;
        }
        Book_ID = ReqJson["Book_ID"].asInt();
        ContentJson = ReqJson["Content"];
        Book_Name = ReqJson["Book_Name"].asString();
        Book_Author = ReqJson["Book_Author"].asString();
    }
    catch (Json::Value &e)
    {
        MyBasePtr->INFO_Func("Upload_Chapter", false, RespJson);
        throw e;
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
            RespJson["ErrorMsg"].append("要上传新章节的图书不存在");
            RespJson["Result"] = false;
            MyBasePtr->INFO_Func("Upload_Chapter", false, RespJson);
            throw RespJson;
        }
        MyBasePtr->DEBUGLog("要上传新章节的图书存在", true);
        MyBasePtr->DEBUGLog("开始判断目标图书ID是否正确", true);
        if (vecBook[0].getValueOfBookId() != Book_ID)
        {
            RespJson["ErrorMsg"].append("要上传新章节的图书的ID与指定ID不符合,(Book_ID=" + to_string(Book_ID) + ",目标图书的ID=" + to_string(vecBook[0].getValueOfBookId()) + ")");
            RespJson["Result"] = false;
            MyBasePtr->INFO_Func("Upload_Chapter", false, RespJson);
            throw RespJson;
        }
        MyBasePtr->DEBUGLog("目标图书ID正确", true);
    }

    // 插入前检查content数据是否合法
    try
    {
        MyBasePtr->DEBUGLog("开始检查content数据是否合法", true);
        // "Chapter_Title"   :   "", //  章节标题
        // "Content_Array"   :   []  //  章节内容
        // 创建检查列表 key是字段名 value 是字段类型

        std::map<string, MyJson::ColType> ColMap;
        ColMap["Chapter_Title"] = MyJson::ColType::STRING;
        ColMap["Content_Array"] = MyJson::ColType::ARRAY;
        MyJsonPtr->checkMemberAndTypeInMap(ContentJson, RespJson, ColMap);
        MyBasePtr->DEBUGLog("检查content数据合法", true);
    }
    catch (Json::Value &RespJson)
    {
        RespJson["ErrorMsg"].append("content字段数据不合法");
        RespJson["Result"] = false;
        MyBasePtr->INFO_Func("Upload_Chapter", false, RespJson);
        throw RespJson;
    }

    MyBasePtr->DEBUGLog("开始查询用户 : " + to_string(UserID), true);
    drogon_model::novel::User user = UserMgr.findByPrimaryKey(UserID);
    MyBasePtr->DEBUGLog("用户查询完毕 : " + user.toJson().toStyledString(), true);

    // 验证用户
    if (user.getValueOfUserId() != UserID)
    {
        RespJson["ErrorMsg"].append("登入的UserID(" + to_string(UserID) + ")与查询到的UserID(" + to_string(user.getValueOfUserId()) + ")不同");
        RespJson["Result"] = false;
        MyBasePtr->INFO_Func("Upload_Chapter", false, RespJson);
        throw RespJson;
    }

    drogon_model::novel::Upload NewUpload;
    try
    {
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
    }
    catch (const drogon::orm::DrogonDbException &e)
    {
        RespJson["ErrorMsg"].append("Upload插入失败(" + string(e.base().what()) + ")");
        RespJson["Result"] = false;
        MyBasePtr->INFO_Func("Upload_Chapter", false, RespJson);
        throw RespJson;
    }

    RespJson["Result"] = true;
    RespJson["Upload_ID"] = NewUpload.getValueOfUploadId();
    MyBasePtr->INFO_Func("Upload_Chapter", false, RespJson);
}

// 更改用户资料的接口
void MyDBService::Update_User_PersonalData(Json::Value &ReqJson, Json::Value &RespJson)
{
    Json::Value ParaJson;
    auto MyBasePtr = app().getPlugin<MyBase>();
    auto MyJsonPtr = app().getPlugin<MyJson>();
    MyBasePtr->INFO_Func("Update_User_PersonalData", true, ReqJson);

    auto dbclientPrt = drogon::app().getDbClient();
    Mapper<drogon_model::novel::User> UserMgr(dbclientPrt);

    // 检查ReqJson数据是否合法
    try
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
        if (LoginStatus != "user" && LoginStatus != "admin" && LoginStatus != "root")
        {
            RespJson["ErrorMsg"].append("权限不足，请联系管理员");
            throw RespJson;
        }
        MyBasePtr->DEBUGLog("操作权限检测通过", true);
    }
    catch (Json::Value &e)
    {
        MyBasePtr->INFO_Func("Update_User_PersonalData", false, RespJson);
        throw e;
    }

    // 检查是否修改的是自己的资料
    {
        if (ReqJson["Change_ID"].asInt() != std::atoi(ParaJson["User_ID"].asString().c_str()))
        {
            RespJson["ErrorMsg"].append("权限不足，请联系管理员(操作用户: " + to_string(ParaJson["User_ID"].asInt()) + ",被操作用户: " + to_string(ReqJson["Change_ID"].asInt()) + ")");
            MyBasePtr->INFO_Func("Update_User_PersonalData", false, RespJson);
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
        RespJson["ErrorMsg"].append("用户数据无需更新");
        MyBasePtr->INFO_Func("Update_User_PersonalData", false, RespJson);
        return;
    }

    MyBasePtr->DEBUGLog("开始更新用户数据", true);
    int row = UserMgr.update(user);
    if (row != 1)
    {
        RespJson["ErrorMsg"].append("用户资料更新失败");
        MyBasePtr->INFO_Func("Update_User_PersonalData", false, RespJson);
        throw RespJson;
    }
    MyBasePtr->DEBUGLog("用户数据更新完成: " + user.toJson().toStyledString(), true);
    MyBasePtr->INFO_Func("Update_User_PersonalData", false, RespJson);
}

// 数据库插入新书
/*
    数据库插入新书  return bool // 代表插入过程是否正常，数据异常时返回false
Req:{
    "Book_Author"   :   "",
    "Book_Name"     :   "",
    "Book_Publisher":   "",
    "Book_Synopsis" :   ""
}
Resp:{
    "ErrorMsg"[失败]    :   [],//失败原因(数据异常/要创建的图书已存在)
    "Upload_Status"     :   "",//要更改的Upload的状态(已成功/已失败/待处理)
    "Book_Data"[成功]   :   {},//新建图书的数据
    "Explain"           :   "",//用于UPload的Memo字段更新
    "Result"            :   true/false,//只有成功插入才是true否则都是false
}
*/
bool MyDBService::Insert_Book(Json::Value &ReqJson, Json::Value &RespJson)
{
    string ErrorMsg;
    auto MyJsonPtr = app().getPlugin<MyJson>();
    auto MyBasePtr = app().getPlugin<MyBase>();
    auto MyToolsPtr = app().getPlugin<MyTools>();
    MyBasePtr->INFO_Func("Insert_Book", true, ReqJson);

    auto dbclientPrt = drogon::app().getDbClient();
    Mapper<drogon_model::novel::Book> BookMgr(dbclientPrt);

    // 插入前检查图书数据是否合法
    try
    {
        MyBasePtr->DEBUGLog("插入前检查图书数据是否合法", true);
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
        MyJsonPtr->checkMemberAndTypeInMap(ReqJson, RespJson, ColMap);
        MyBasePtr->DEBUGLog("插入前检查显示图书数据合法", true);
    }
    catch (Json::Value &e)
    {
        ErrorMsg = "要创建的图书数据不合法";
        RespJson["Result"] = false;
        RespJson["Explain"] = ErrorMsg;
        RespJson["Upload_Status"] = "已失败";
        RespJson["ErrorMsg"].append(ErrorMsg);
        MyBasePtr->INFO_Func("Insert_Book", false, RespJson);
        return true;
    }

    drogon_model::novel::Book NewBook;
    Json::Value BookMemo;
    string BookName = ReqJson["Book_Name"].asString();
    string BookAuthor = ReqJson["Book_Author"].asString();
    string BookPublisher = ReqJson["Book_Publisher"].asString();
    string BookSynopsis = ReqJson["Book_Synopsis"].asString();

    // 检查图书是否已存在
    {
        MyBasePtr->DEBUGLog("开始检查图书是否已存在", true);
        Criteria BookName_cri(drogon_model::novel::Book::Cols::_Name, CompareOperator::EQ, BookName);
        Criteria BookAuthor_cri(drogon_model::novel::Book::Cols::_Author, CompareOperator::EQ, BookAuthor);
        vector<drogon_model::novel::Book> vecBook = BookMgr.findBy(BookName_cri && BookAuthor_cri);

        // 如果不是新书，则将Upload状态改为已处理且拒绝插入
        if (vecBook.size() != 0)
        {
            ErrorMsg = "图书创建失败(要创建的图书已存在(BookID = " + to_string(vecBook[0].getValueOfBookId()) + "))";
            RespJson["ErrorMsg"].append(ErrorMsg);
            RespJson["Upload_Status"] = "已失败";
            RespJson["Explain"] = ErrorMsg;
            RespJson["Result"] = false;
            MyBasePtr->INFO_Func("Insert_Book", false, RespJson);
            return true;
        }

        MyBasePtr->DEBUGLog("要创建的图书不存在", true);
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
    }

    // 准备插入图书数据
    try
    {
        MyBasePtr->DEBUGLog("准备插入的新图书数据为 : " + NewBook.toJson().toStyledString(), true);
        MyBasePtr->DEBUGLog("准备插入新图书数据", true);
        BookMgr.insert(NewBook);
        MyBasePtr->DEBUGLog("插入新图书数据完毕", true);
        MyBasePtr->DEBUGLog("插入的新数据为 : " + NewBook.toJson().toStyledString(), true);
    }
    // 新建图书失败
    catch (...)
    {
        ErrorMsg = "图书创建失败(数据异常)";
        RespJson["Result"] = false;
        RespJson["Explain"] = ErrorMsg;
        RespJson["Upload_Status"] = "待处理";
        RespJson["ErrorMsg"].append(ErrorMsg);
        MyBasePtr->INFO_Func("Insert_Book", false, RespJson);
        return false;
    }
    RespJson["Result"] = true;
    RespJson["Explain"] = "图书创建成功";
    RespJson["Upload_Status"] = "已通过";
    RespJson["Book_Data"] = NewBook.toJson();
    MyBasePtr->INFO_Func("Insert_Book", false, RespJson);
    return true;
}

// 数据库插入新章节
/*
数据库插入新章节  return bool
Req:{
    "User_ID"         :   0,  //  贡献者ID
    "Book_ID"         :   0,  //  目标图书的ID
    "Chapter_Title"   :   "", //  章节标题
    "Content_Array"   :   []  //  章节内容
    ["Version"]       :   0,  //  可选参数，默认为1，传入时以其为准
}
Resp:{
    "Result"            :   true/false,//结果(图书创建失败/成功)
    "ErrorMsg"[失败]    :   [],//失败原因(数据异常/要创建的图书已存在)
    "Upload_Status"     :   "",//要更改的Upload的状态(已成功/已失败/待处理)
    "Chapter_Data"[成功]:   {},//新建图书的数据(只有在成功情况下有此字段)
    "Explain"           :   "",//用于UPload的Memo字段更新
}
*/
bool MyDBService::Insert_Chapter(Json::Value &ReqJson, Json::Value &RespJson)
{
    string ErrorMsg;
    auto MyJsonPtr = app().getPlugin<MyJson>();
    auto MyBasePtr = app().getPlugin<MyBase>();
    auto MyToolsPtr = app().getPlugin<MyTools>();
    MyBasePtr->INFO_Func("Insert_Chapter", true, ReqJson);

    auto dbclientPrt = drogon::app().getDbClient();
    Mapper<drogon_model::novel::User> UserMgr(dbclientPrt);
    Mapper<drogon_model::novel::Upload> UploadMgr(dbclientPrt);
    Mapper<drogon_model::novel::Book> BookMgr(dbclientPrt);
    Mapper<drogon_model::novel::Chapter> ChapterMgr(dbclientPrt);
    drogon_model::novel::Book TBook; // 目标图书

    int BookID, UserID;
    // 插入前检查章节数据是否合法
    try
    {
        MyBasePtr->DEBUGLog("插入前检查章节数据是否合法", true);
        // "User_ID"         :   0,  //  贡献者ID
        // "Book_ID"         :   0,  //  目标图书的ID
        // "Chapter_Title"   :   "", //  章节标题
        // "Content_Array"   :   []  //  章节内容
        // 创建检查列表 key是字段名 value 是字段类型
        std::map<string, MyJson::ColType> ColMap;
        ColMap["User_ID"] = MyJson::ColType::INT;
        ColMap["Book_ID"] = MyJson::ColType::INT;
        ColMap["Chapter_Title"] = MyJson::ColType::STRING;
        ColMap["Content_Array"] = MyJson::ColType::ARRAY;
        MyJsonPtr->checkMemberAndTypeInMap(ReqJson, RespJson, ColMap);
        MyBasePtr->DEBUGLog("插入前检查显示章节数据合法", true);
        UserID = ReqJson["User_ID"].asInt();
        BookID = ReqJson["Book_ID"].asInt();
    }
    catch (Json::Value &e)
    {
        ErrorMsg = "要创建的章节数据不合法";
        RespJson["Result"] = false;
        RespJson["Explain"] = ErrorMsg;
        RespJson["Upload_Status"] = "已失败";
        RespJson["ErrorMsg"].append(ErrorMsg);
        MyBasePtr->INFO_Func("Insert_Chapter", false, RespJson);
        return true;
    }

    // 查找要插入的图书
    try
    {
        TBook = BookMgr.findByPrimaryKey(ReqJson["Book_ID"].asInt());
    }
    catch (...)
    {
        ErrorMsg = "章节创建失败(找不到对应的图书(ID=" + ReqJson["Book_ID"].asString() + "))";
        RespJson["Result"] = false;
        RespJson["Explain"] = ErrorMsg;
        RespJson["Upload_Status"] = "已失败";
        RespJson["ErrorMsg"].append(ErrorMsg);
        MyBasePtr->INFO_Func("Insert_Chapter", false, RespJson);
        return true;
    }

    drogon_model::novel::Chapter NewChapter;
    pair<int, string> Title = MyToolsPtr->getTitleNumAndTitleStr(ReqJson["Chapter_Title"].asString());
    string ChapterTitle = Title.second;                     // 章节名
    int Part_Num;                                           // 章节卷
    int Chapter_Num = Title.first;                          // 章节数
    Json::Value Chapter_Content = ReqJson["Content_Array"]; // 章节内容
    Json::Value Chapter_Memo;                               // 参数
    Json::Value BookMemo;
    Json::Value ChapterJson; //  Memo/Chapter

    // 章节卷需要查询Book表获取
    try
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

        Part_Num = ChapterJson.size() - 1;
        MyBasePtr->DEBUGLog("查询Book/Memo获取章节卷成功", true);
    }
    catch (Json::Value &e)
    {
        ErrorMsg = "章节创建失败(Book/Memo数据不合法)";
        RespJson["Result"] = false;
        RespJson["Explain"] = ErrorMsg;
        RespJson["Upload_Status"] = "已失败";
        RespJson["ErrorMsg"].append(ErrorMsg);
        MyBasePtr->INFO_Func("Insert_Chapter", false, RespJson);
        return true;
    }

    // 章节数小于0 说明是番外 ////----待处理
    if (Chapter_Num < 0)
    {
    }

    // 检查章节是否已存在
    {
        MyBasePtr->DEBUGLog("开始检查图书章节是否已存在", true);
        Criteria BookID_cri(drogon_model::novel::Chapter::Cols::_Book_ID, CompareOperator::EQ, BookID);
        Criteria PartNum_cri(drogon_model::novel::Chapter::Cols::_Part_Num, CompareOperator::EQ, Part_Num);
        Criteria ChapterNum_cri(drogon_model::novel::Chapter::Cols::_Chapter_Num, CompareOperator::EQ, Chapter_Num);
        Criteria ChapterTitle_cri(drogon_model::novel::Chapter::Cols::_Title, CompareOperator::EQ, ChapterTitle);
        Criteria ChapterValid_cri(drogon_model::novel::Chapter::Cols::_Valid, CompareOperator::EQ, true);
        vector<drogon_model::novel::Chapter> vecChapter = ChapterMgr.findBy(BookID_cri && PartNum_cri && ChapterNum_cri && ChapterTitle_cri && ChapterValid_cri);
        // 如果不是新章节，则将Upload状态改为已处理且拒绝插入
        if (vecChapter.size() != 0)
        {
            ErrorMsg = "章节创建失败(要新增的图书章节已存在(BookID = " + to_string(vecChapter[0].getValueOfBookId()) + ",Part_Num = " + to_string(vecChapter[0].getValueOfPartNum()) + ",Chapter_Num = " + to_string(vecChapter[0].getValueOfChapterNum()) + ",Title = " + vecChapter[0].getValueOfTitle() + "))";
            RespJson["Result"] = false;
            RespJson["Explain"] = ErrorMsg;
            RespJson["Upload_Status"] = "已失败";
            RespJson["ErrorMsg"].append(ErrorMsg);
            MyBasePtr->INFO_Func("Insert_Chapter", false, RespJson);
            return true;
        }
        MyBasePtr->DEBUGLog("要插入的图书章节不存在", true);
    }

    // 准备要创建的新章节的数据
    {
        MyBasePtr->DEBUGLog("开始准备新章节数据", true);
        NewChapter.setBookId(BookID);
        NewChapter.setUserId(UserID);
        NewChapter.setChapterNum(Chapter_Num);
        NewChapter.setContent(Chapter_Content.toStyledString());
        Chapter_Memo["Status"] = "normal";
        NewChapter.setMemo(Chapter_Memo.toStyledString());
        NewChapter.setPartNum(Part_Num);
        NewChapter.setTitle(ChapterTitle);
        NewChapter.setValid(true);
        if (ReqJson.isMember("Version"))
        {
            if (!ReqJson["Version"].isInt())
            {
                ErrorMsg = "传入的Version字段不是int类型";
                RespJson["Result"] = false;
                RespJson["Explain"] = ErrorMsg;
                RespJson["Upload_Status"] = "已失败";
                RespJson["ErrorMsg"].append(ErrorMsg);
                MyBasePtr->INFO_Func("Insert_Chapter", false, RespJson);
                return true;
            }
            NewChapter.setVersion(ReqJson["Version"].asInt());
        }
        else
        {
            NewChapter.setVersion(1);
        }
        MyBasePtr->DEBUGLog("准备新章节数据完成", true);
    }

    try
    {
        MyBasePtr->DEBUGLog("准备新章节数据为 : " + NewChapter.toJson().toStyledString(), true);
        MyBasePtr->DEBUGLog("准备插入新章节数据", true);
        ChapterMgr.insert(NewChapter);
        MyBasePtr->DEBUGLog("插入新章节数据完毕", true);
        MyBasePtr->DEBUGLog("插入的新数据为 : " + NewChapter.toJson().toStyledString(), true);

        // 章节插入后需要更新Book信息
        {
            MyBasePtr->DEBUGLog("开始更新Book数据", true);
            ChapterJson[Part_Num] = ChapterJson[Part_Num].asInt() + 1;
            ChapterJson[0] = ChapterJson[0].asInt() + 1;
            BookMemo["Chapter"] = ChapterJson;
            TBook.setMemo(BookMemo.toStyledString());
            int count = BookMgr.update(TBook);
            MyBasePtr->DEBUGLog("更新的新数据为 : " + TBook.toJson().toStyledString(), true);
            if (!count)
            {
                MyBasePtr->DEBUGLog("更新Book数据失败", true);
            }
            else
            {
                MyBasePtr->DEBUGLog("更新Book数据完毕", true);
            }
        }
    }
    // 新建图书失败
    catch (...)
    {
        ErrorMsg = "章节创建失败(数据异常)";
        RespJson["Result"] = false;
        RespJson["Explain"] = ErrorMsg;
        RespJson["Upload_Status"] = "待处理";
        RespJson["ErrorMsg"].append(ErrorMsg);
        MyBasePtr->INFO_Func("Insert_Chapter", false, RespJson);
        return false;
    }

    RespJson["Result"] = true;
    RespJson["Explain"] = "章节创建成功";
    RespJson["Upload_Status"] = "已成功";
    RespJson["Chapter_Data"] = NewChapter.toJson();
    MyBasePtr->INFO_Func("Insert_Chapter", false, RespJson);
    return true;
}

// 数据库更新章节
/*
    数据库更新新章节  return bool // 结果(图书更新失败/成功)
Req:{
    "Processor_Type"    :  "",// 权限
    "Change_ID"  :   0,被更改的章节ID
    带[]的是可选参数即要修改的参数
    ["Change_Book_ID"] :   0,//修改章节所属的图书ID
    ["Change_Part_Num"]:   0,//修改章节所属的卷数
    ["Change_Chapter_Num"]:0,//修改章节数
    ["Change_Title"]   :   "",//修改标题
    ["Change_User_ID"] :   0,//修改章节贡献者ID
    ["Change_Version"] :   0,//修改章节版本
    ["Change_Valid"]   :   true/false,//修改章节有效性
    ["Change_Content"] :   [],//修改章节内容
    ["Change_Memo"]    :   {},//修改章节参数
}
Resp:{
    "ErrorMsg"      :   [],//失败原因(..)    (只有在失败情况下有此字段)
    "Chapter_Data"  :   {},//新建图书的数据(只有在成功情况下有此字段)
}
*/
bool MyDBService::Update_Chapter(Json::Value &ReqJson, Json::Value &RespJson)
{
    string ErrorMsg;
    auto MyBasePtr = app().getPlugin<MyBase>();
    auto MyJsonPtr = app().getPlugin<MyJson>();
    MyBasePtr->INFO_Func("Update_Chapter", true, ReqJson);

    auto dbclientPrt = drogon::app().getDbClient();
    Mapper<drogon_model::novel::Chapter> ChapterMgr(dbclientPrt);

    // 检查ReqJson数据是否合法
    try
    {
        MyBasePtr->DEBUGLog("开始检查ReqJson数据是否合法", true);
        // "Processor_Type"    :  "",// 权限
        // "Change_ID"  :   0,被更改的章节ID

        std::map<string, MyJson::ColType> ColMap;
        ColMap["Processor_Type"] = MyJson::ColType::STRING;
        ColMap["Change_ID"] = MyJson::ColType::INT;
        MyJsonPtr->checkMemberAndTypeInMap(ReqJson, RespJson, ColMap);
        MyBasePtr->DEBUGLog("ReqJson数据合法", true);

        MyBasePtr->DEBUGLog("开始检查操作权限", true);
        string LoginStatus = ReqJson["Processor_Type"].asString();
        if (LoginStatus != "admin" && LoginStatus != "root" && LoginStatus != "system")
        {
            ErrorMsg = "章节更新失败(权限不足，请联系管理员)";
            RespJson["ErrorMsg"].append(ErrorMsg);
            MyBasePtr->INFO_Func("Update_Chapter", false, RespJson);
            return false;
        }
        MyBasePtr->DEBUGLog("操作权限检测通过", true);
    }
    catch (Json::Value &e)
    {
        ErrorMsg = "要更新的章节数据不合法";
        RespJson["ErrorMsg"].append(ErrorMsg);
        MyBasePtr->INFO_Func("Update_Chapter", false, RespJson);
        return false;
    }

    // 查找要更新的章节
    drogon_model::novel::Chapter chapter;
    try
    {
        // Name  Password  Sex  用户有权修改这三项
        MyBasePtr->DEBUGLog("开始查询章节ID : " + to_string(ReqJson["Change_ID"].asInt()), true);
        chapter = ChapterMgr.findByPrimaryKey(ReqJson["Change_ID"].asInt());
        MyBasePtr->DEBUGLog("章节ID查询完毕 : " + chapter.toJson().toStyledString(), true);

        if (chapter.getValueOfValid() == false)
        {
            ErrorMsg = "章节更新失败(章节ID已失效(ID=" + to_string(ReqJson["Change_ID"].asInt()) + "))";
            RespJson["ErrorMsg"].append(ErrorMsg);
            MyBasePtr->INFO_Func("Update_Chapter", false, RespJson);
            return false;
        }
    }
    catch (...)
    {
        ErrorMsg = "章节更新失败(找不到对应的章节(ID=" + ReqJson["Change_ID"].asString() + ")";
        RespJson["ErrorMsg"].append(ErrorMsg);
        MyBasePtr->INFO_Func("Update_Chapter", false, RespJson);
        return false;
    }

    MyBasePtr->DEBUGLog("开始修改章节数据", true);

    int Change_count = 0;
    try
    {
        if (ReqJson.isMember("Change_Book_ID"))
        {
            MyJsonPtr->checkColType(ReqJson, RespJson, "Change_Book_ID", MyJson::ColType::INT);
            chapter.setBookId(ReqJson["Change_Book_ID"].asInt());
            Change_count++;
        }
        if (ReqJson.isMember("Change_Part_Num"))
        {
            MyJsonPtr->checkColType(ReqJson, RespJson, "Change_Part_Num", MyJson::ColType::INT);
            chapter.setPartNum(ReqJson["Change_Part_Num"].asInt());
            Change_count++;
        }
        if (ReqJson.isMember("Change_Chapter_Num"))
        {
            MyJsonPtr->checkColType(ReqJson, RespJson, "Change_Chapter_Num", MyJson::ColType::INT);
            chapter.setChapterNum(ReqJson["Change_Chapter_Num"].asInt());
            Change_count++;
        }
        if (ReqJson.isMember("Change_Title"))
        {
            MyJsonPtr->checkColType(ReqJson, RespJson, "Change_Title", MyJson::ColType::STRING);
            chapter.setTitle(ReqJson["Change_Title"].asString());
            Change_count++;
        }
        if (ReqJson.isMember("Change_User_ID"))
        {
            MyJsonPtr->checkColType(ReqJson, RespJson, "Change_User_ID", MyJson::ColType::INT);
            chapter.setUserId(ReqJson["Change_User_ID"].asInt());
            Change_count++;
        }
        if (ReqJson.isMember("Change_Version"))
        {
            MyJsonPtr->checkColType(ReqJson, RespJson, "Change_Version", MyJson::ColType::INT);
            chapter.setVersion(ReqJson["Change_Version"].asInt());
            Change_count++;
        }
        if (ReqJson.isMember("Change_Valid"))
        {
            MyJsonPtr->checkColType(ReqJson, RespJson, "Change_Valid", MyJson::ColType::BOOL);
            chapter.setValid(ReqJson["Change_Valid"].asBool());
            Change_count++;
        }
        if (ReqJson.isMember("Change_Content"))
        {
            MyJsonPtr->checkColType(ReqJson, RespJson, "Change_Content", MyJson::ColType::JSON);
            chapter.setContent(ReqJson["Change_Content"].toStyledString());
            Change_count++;
        }
        if (ReqJson.isMember("Change_Memo"))
        {
            MyJsonPtr->checkColType(ReqJson, RespJson, "Change_Memo", MyJson::ColType::JSON);
            chapter.setMemo(ReqJson["Change_Memo"].toStyledString());
            Change_count++;
        }
    }
    catch (Json::Value &e)
    {
        ErrorMsg = "字段数据格式错误";
        RespJson["ErrorMsg"].append(ErrorMsg);
        MyBasePtr->INFO_Func("Update_Chapter", false, RespJson);
        return false;
    }
    MyBasePtr->DEBUGLog("章节数据修改完成", true);

    // 统计更新的字段数
    if (Change_count == 0)
    {
        ErrorMsg = "章节更新失败(章节数据无更新字段)";
        RespJson["ErrorMsg"].append(ErrorMsg);
        MyBasePtr->INFO_Func("Update_Chapter", false, RespJson);
        return false;
    }

    MyBasePtr->DEBUGLog("开始更新章节数据", true);
    int row = ChapterMgr.update(chapter);
    if (row != 1)
    {
        ErrorMsg = "章节更新失败(章节数据更新失败)";
        RespJson["ErrorMsg"].append(ErrorMsg);
        MyBasePtr->INFO_Func("Update_Chapter", false, RespJson);
        return false;
    }
    MyBasePtr->DEBUGLog("章节数据更新完成: " + chapter.toJson().toStyledString(), true);
    RespJson["Chapter_Data"] = chapter.toJson();
    MyBasePtr->INFO_Func("Update_Chapter", false, RespJson);
    return true;
}
