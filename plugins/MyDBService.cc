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


// 管理员查看用户意见
/*
Req:{
    "Limit"      :   0,          //  取多少条数据
    "Offset"     :   0,          //  偏移量
    "Finish"     :   true/false, //  是否经过审核
    "Provider_ID":   0,          //  贡献者ID
    "Processor"  :   "",         //  处理者
    "Filter_Col" :   [],         //  过滤字段
    "Para":{},// 管理员信息
}
Resp:{
    "Result":true/false,
    "ErrorMsg":[],
    "Idea_List":[]
}
*/
void MyDBService::Admin_Search_Idea(Json::Value &ReqJson, Json::Value &RespJson)
{
    Json::Value ParaJson, ChangeTargetArray;
    auto MyJsonPtr = app().getPlugin<MyJson>();
    auto MyBasePtr = app().getPlugin<MyBase>();
    auto MyToolsPtr = app().getPlugin<MyTools>();
    MyBasePtr->INFO_Func("Admin_Search_Idea", true, ReqJson);
    auto dbclientPrt = drogon::app().getDbClient();
    Mapper<drogon_model::novel::Idea> IdeaMgr(dbclientPrt);

    // 检查ReqJson数据是否合法
    try
    {
        MyBasePtr->DEBUGLog("开始检查ReqJson数据是否合法", true);
        // "Limit"      :   0,          //  取多少条数据
        // "Offset"     :   0,          //  偏移量
        // "Finish"     :   true/false, //  是否经过审核
        // "Provider_ID":   0,          //  贡献者ID
        // "Processor"  :   "",         //  处理者
        // "Filter_Col" :   [],         //  过滤字段
        // "Para"       :   {},         //  管理员信息
        // 创建检查列表 key是字段名 value 是字段类型
        std::map<string, MyJson::ColType> ColMap;
        ColMap["Limit"] = MyJson::ColType::INT;
        ColMap["Offset"] = MyJson::ColType::INT;
        ColMap["Finish"] = MyJson::ColType::BOOL;
        ColMap["Provider_ID"] = MyJson::ColType::INT;
        ColMap["Processor"] = MyJson::ColType::STRING;
        ColMap["Filter_Col"] = MyJson::ColType::ARRAY;
        ColMap["Para"] = MyJson::ColType::JSON;
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
    catch(Json::Value &e)
    {
        RespJson["Result"] = false;
        RespJson["ErrorMsg"] = e["ErrorMsg"];
        RespJson["ErrorMsg"].append("ReqJson数据检查未通过");
        MyBasePtr->INFO_Func("Admin_Search_Idea", false, RespJson);
        return;
    }

    int FilterSize;
    Criteria Filter_Cri;
    // 制作筛选条件
    try
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
                    Filter_Cri = Filter_Cri && Criteria(drogon_model::novel::Idea::Cols::_IsManage, CompareOperator::EQ, ReqJson["Finish"].asBool());
                    continue;
                }
                Filter_Cri = Criteria(drogon_model::novel::Idea::Cols::_IsManage, CompareOperator::EQ, ReqJson["Finish"].asBool());
            }
            else if (FilterJson[i].asString() == "Provider_ID")
            {
                if (Filter_Cri)
                {
                    MyBasePtr->DEBUGLog("制作筛选对象(" + FilterJson[i].asString() + ")为Filter_Cri", true);
                    Filter_Cri = Filter_Cri && Criteria(drogon_model::novel::Idea::Cols::_User_ID, CompareOperator::Like, "%" + ReqJson["Provider_ID"].asString() + "%");
                    continue;
                }
                Filter_Cri = Criteria(drogon_model::novel::Idea::Cols::_User_ID, CompareOperator::Like, "%" + ReqJson["Provider_ID"].asString() + "%");
            }
            else if (FilterJson[i].asString() == "Processor")
            {
                if (Filter_Cri)
                {
                    MyBasePtr->DEBUGLog("制作筛选对象(" + FilterJson[i].asString() + ")为Filter_Cri", true);
                    Filter_Cri = Filter_Cri && Criteria(drogon_model::novel::Idea::Cols::_Processor, CompareOperator::Like, "%" + ReqJson["Processor"].asString() + "%");
                    continue;
                }
                Filter_Cri = Criteria(drogon_model::novel::Idea::Cols::_Processor, CompareOperator::Like, "%" + ReqJson["Processor"].asString() + "%");
            }
            else if (FilterJson[i].asString() == "Limit")
            {
                int LimitNum = ReqJson["Limit"].asInt();
                IdeaMgr.limit(LimitNum);
            }
            else if (FilterJson[i].asString() == "Offset")
            {
                int OffsetNum = ReqJson["Offset"].asInt();
                IdeaMgr.offset(OffsetNum);
            }
            else
            {
                RespJson["ErrorMsg"].append("Filter_Col中存在非法字段(" + FilterJson[i].asString() + ")");
                throw RespJson;
            }
        }
    }
    catch(...)
    {
        RespJson["Result"] = false;
        RespJson["ErrorMsg"].append("制作筛选条件发生异常错误");
        MyBasePtr->INFO_Func("Admin_Search_Idea", false, RespJson);
        return;
    }

    // 查询意见信息
    try
    {
        MyBasePtr->DEBUGLog("开始查询用户意见信息", true);
        vector<drogon_model::novel::Idea> vecIdea;
        
        MyBasePtr->DEBUGLog("SQL : " + Filter_Cri.criteriaString(), true);
        if (FilterSize > 0)
            vecIdea = IdeaMgr.findBy(Filter_Cri);
        else
            vecIdea = IdeaMgr.findAll();

        MyBasePtr->DEBUGLog("用户意见信息共" + to_string(vecIdea.size()) + "个", true);
        Json::Value vecJsonVal;
        for (auto &idea : vecIdea)
        {
            vecJsonVal.append(idea.toJson());
            // std::cout << user.toJson().toStyledString() <<std::endl;
        }
        RespJson["Idea_List"] = vecJsonVal;
        MyBasePtr->DEBUGLog("用户意见信息查询完毕", true);
    }
    catch(const drogon::orm::DrogonDbException &e)
    {
	    RespJson["Result"] = false;
        RespJson["ErrorMsg"].append("查询用户意见信息发生数据库异常");
        RespJson["ErrorMsg"].append(e.base().what());
        MyBasePtr->INFO_Func("Admin_Search_Idea", false, RespJson);
        return;
    }
    catch(...)
    {
        RespJson["Result"] = false;
        RespJson["ErrorMsg"].append("查询用户意见信息发生异常错误");
        MyBasePtr->INFO_Func("Admin_Search_Idea", false, RespJson);
        return;
    }
    RespJson["Result"] = true;
    MyBasePtr->INFO_Func("Admin_Search_Idea", false, RespJson);
    return;
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
            Json::Value UpJson = upload.toJson();
            UpJson.removeMember("Content");
            vecJsonVal.append(UpJson);
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
                            if (book.getValueOfBookName() == ChangeTargetArray[i]["Change_Content"]["Change_Name"].asString())
                            {
                                MyBasePtr->DEBUGLog("字段(" + ChangeColName + ")没有产生变化", true);
                                continue;
                            }
                            book.setBookName(ChangeTargetArray[i]["Change_Content"]["Change_Name"].asString());
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
/*

Req:{
    "Change_Target"  :   [{},{}],被执行者们
    "Para"           :   {"User_ID":"","Login_Status":""} 执行者
}

*/
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

    int UserID;
    string Processor;
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
        UserID = atoi(ParaJson["User_ID"].asString().c_str());
        Processor = ParaJson["Login_Status"].asString();
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
        // "Change_Col"             :   ["Change_Name","Change_Password","Change_Sex","Change_Integral"]
        ColMap["Change_ID"] = MyJson::ColType::INT;
        ColMap["Change_Col"] = MyJson::ColType::ARRAY;
        ColMap["Change_Sex"] = MyJson::ColType::STRING;
        ColMap["Change_Name"] = MyJson::ColType::STRING;
        ColMap["Change_Password"] = MyJson::ColType::STRING;

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

    Json::Value UpdateJson,TempJson;
    for (int i = 0; i < ChangeSize; i++)
    {
        /* code */
        TempJson.clear();
        if (ChangeTargetArray[i].isMember("Result"))
        {
            MyBasePtr->DEBUGLog("第" + to_string(i) + "位修改者的数据不合法,跳过", true);
            TempJson["Changed_User_ID"] = ChangeTargetArray[i]["Change_ID"];
            TempJson["Result"] = false;
            TempJson["ErrorMsg"] = ChangeTargetArray[i]["ErrorMsg"];
            RespJson["Change_Target"].append(TempJson);
            continue;
        }
        MyBasePtr->DEBUGLog("第" + to_string(i) + "位修改者的数据合法,开始准备修改", true);
        UpdateJson.clear();
        TempJson.clear();
        UpdateJson;
        // Req:{
        //     "User_ID" : 0,
        //     "Processor" : "",
        //     "Change_ID" : 0 ,
        //     "Change_Name" : "",
        //     "Change_Password" : "",
        //     "Change_Sex" : "",
        //     "Change_Col" : ["Change_Name","Change_Password","Change_Sex"]
        // }
        UpdateJson["User_ID"] = UserID;
        UpdateJson["Processor"] = Processor;
        UpdateJson["Change_ID"] = ChangeTargetArray[i]["Change_ID"].asInt();
        UpdateJson["Change_Name"] = ChangeTargetArray[i]["Change_Name"].asString();
        UpdateJson["Change_Password"] = ChangeTargetArray[i]["Change_Password"].asString();
        UpdateJson["Change_Sex"] = ChangeTargetArray[i]["Change_Sex"].asString();
        UpdateJson["Change_Col"] = ChangeTargetArray[i]["Change_Col"];
        if(!Update_User(UpdateJson,TempJson))
        {
            UpdateJson.clear();
            MyBasePtr->DEBUGLog("第" + to_string(i) + "位修改者的数据更新失败", true);
            UpdateJson["Changed_User_ID"] = ChangeTargetArray[i]["Change_ID"];
            UpdateJson["Result"] = false;
            UpdateJson["ErrorMsg"] = TempJson["ErrorMsg"];
            RespJson["Change_Target"].append(UpdateJson);
            continue;
        }
        else
        {
            UpdateJson.clear();
            MyBasePtr->DEBUGLog("第" + to_string(i) + "位修改者的数据更新成功", true);
            UpdateJson["Changed_User_ID"] = ChangeTargetArray[i]["Change_ID"];
            UpdateJson["Result"] = true;
            RespJson["Change_Target"].append(UpdateJson);
            continue;
        }
    }
    MyBasePtr->INFO_Func("Admin_Update_User", false, RespJson);
}

// 点赞接口
/*
    点赞接口
Req:{
    "User_ID"       :   0,// 用户ID
    "Note_ID"       :   0,// 帖子ID
    "Floor_ID"    :   0,// 楼层ID// 0 代表楼主即帖子的点赞, 其余表示评论的点赞
    "Agree_Result"  :   true/false,//用户点赞操作
}
Resp:{
    "ErrorMsg"  :   [],//失败返回的错误信息
    "Result"    :   true/false,// 操作结果
    "Agree_Data" :   {},// 成功返回的点赞信息
}
*/
void MyDBService::Agree(Json::Value &ReqJson, Json::Value &RespJson)
{
    auto MyJsonPtr = app().getPlugin<MyJson>();
    auto MyBasePtr = app().getPlugin<MyBase>();
    auto MyToolsPtr = app().getPlugin<MyTools>();
    MyBasePtr->INFO_Func("Agree", true, ReqJson);

    auto dbclientPrt = drogon::app().getDbClient();
    Mapper<drogon_model::novel::Agree> AgreeMgr(dbclientPrt);
    drogon_model::novel::Agree Agree;

    Json::Value AgreeJson, NoteJson, UserJson, CommentJson;
    // 检查点赞数据是否合法
    try
    {
        MyBasePtr->DEBUGLog("点赞前检查点赞数据是否合法", true);
        // "User_ID"       :   0,// 用户ID
        // "Note_ID"       :   0,// 帖子ID
        // "Floor_ID"    :   0,// 楼层ID// 0 代表楼主即帖子的点赞, 其余表示评论的点赞
        // "Agree_Result"  :   true/false,//用户点赞操作
        std::map<string, MyJson::ColType> ColMap;
        ColMap["User_ID"] = MyJson::ColType::INT;
        ColMap["Note_ID"] = MyJson::ColType::INT;
        ColMap["Floor_ID"] = MyJson::ColType::INT;
        ColMap["Agree_Result"] = MyJson::ColType::BOOL;
        MyJsonPtr->checkMemberAndTypeInMap(ReqJson, RespJson, ColMap);
        MyBasePtr->DEBUGLog("点赞前检查点赞数据显示合法", true);
    }
    catch (Json::Value &e)
    {
        RespJson["ErrorMsg"] = e["ErrorMsg"];
        RespJson["ErrorMsg"].append("点赞失败(点赞数据不合法)");
        RespJson["Result"] = false;
        MyBasePtr->INFO_Func("Agree", false, RespJson);
        return;
    }

    MyBasePtr->DEBUGLog("开始检查用户是否存在", true);
    if (!Is_User_Exist(ReqJson, UserJson))
    {
        RespJson["ErrorMsg"] = UserJson["ErrorMsg"];
        RespJson["ErrorMsg"].append("点赞失败(用户数据异常)");
        RespJson["Result"] = false;
        MyBasePtr->INFO_Func("Agree", false, RespJson);
        return;
    }
    MyBasePtr->DEBUGLog("开始检查帖子是否存在", true);
    Search_Note_By_NoteID(ReqJson, NoteJson);
    if (!NoteJson["Result"].asBool())
    {
        RespJson["ErrorMsg"] = NoteJson["ErrorMsg"];
        RespJson["ErrorMsg"].append("点赞失败(帖子数据异常)");
        RespJson["Result"] = false;
        MyBasePtr->INFO_Func("Agree", false, RespJson);
        return;
    }

    MyBasePtr->DEBUGLog("开始检查评论是否存在", true);
    if (!Is_Comment_Exist(ReqJson, CommentJson))
    {
        RespJson["ErrorMsg"] = CommentJson["ErrorMsg"];
        RespJson["ErrorMsg"].append("点赞失败(评论数据异常)");
        RespJson["Result"] = false;
        MyBasePtr->INFO_Func("Agree", false, RespJson);
        return;
    }

    MyBasePtr->DEBUGLog("开始检查点赞信息是否存在", true);
    Search_Agree(ReqJson, AgreeJson);
    if (!AgreeJson.isMember("Result") || !AgreeJson.isMember("Agree"))
    {
        MyBasePtr->DEBUGLog("数据异常", true);
        RespJson["ErrorMsg"] = AgreeJson["ErrorMsg"];
        RespJson["ErrorMsg"].append("Search_Agree函数返回的数据异常");
        RespJson["Result"] = false;
        MyBasePtr->INFO_Func("Agree", false, RespJson);
        return;
    }
    if (!AgreeJson["Result"].isBool() || !AgreeJson["Agree"].isBool())
    {
        MyBasePtr->DEBUGLog("数据异常", true);
        RespJson["ErrorMsg"] = AgreeJson["ErrorMsg"];
        RespJson["ErrorMsg"].append("Search_Agree函数返回的数据类型异常");
        RespJson["Result"] = false;
        MyBasePtr->INFO_Func("Agree", false, RespJson);
        return;
    }
    // 如果Result假Agree假则表示程序错误
    // 如果Result假Agree真则表示没有此数据，需要插入
    // 如果Result真Agree假/真 表示用户点赞状态
    if (!AgreeJson["Result"].asBool() && !AgreeJson["Agree"].asBool())
    {
        MyBasePtr->DEBUGLog("数据异常", true);
        RespJson["ErrorMsg"] = AgreeJson["ErrorMsg"];
        RespJson["Result"] = false;
        MyBasePtr->INFO_Func("Agree", false, RespJson);
        return;
    }
    else if (!AgreeJson["Result"].asBool() && AgreeJson["Agree"].asBool())
    {
        MyBasePtr->DEBUGLog("点赞信息不存在", true);
        MyBasePtr->DEBUGLog("开始新增点赞信息", true);
        Agree.setUserId(ReqJson["User_ID"].asInt());
        Agree.setNoteId(ReqJson["Note_ID"].asInt());
        Agree.setFloorId(ReqJson["Floor_ID"].asInt());
        Agree.setStatus(ReqJson["Agree_Result"].asBool());
        try
        {
            AgreeMgr.insert(Agree);
        }
        catch (const drogon::orm::DrogonDbException &e)
        {
            MyBasePtr->DEBUGLog("点赞信息新增失败", true);
            RespJson["ErrorMsg"].append(e.base().what());
            RespJson["ErrorMsg"].append("点赞信息新增失败");
            RespJson["Result"] = false;
            MyBasePtr->INFO_Func("Agree", false, RespJson);
            return;
        }
        MyBasePtr->DEBUGLog("点赞信息新增成功", true);
    }
    else
    {
        MyBasePtr->DEBUGLog("点赞信息存在", true);
        MyBasePtr->DEBUGLog("开始更新点赞信息", true);
        Agree.setAgreeId(AgreeJson["Agree_ID"].asInt());
        Agree.setUserId(ReqJson["User_ID"].asInt());
        Agree.setNoteId(ReqJson["Note_ID"].asInt());
        Agree.setFloorId(ReqJson["Floor_ID"].asInt());
        Agree.setStatus(ReqJson["Agree_Result"].asBool());

        try
        {
            AgreeMgr.update(Agree);
        }
        catch (const drogon::orm::DrogonDbException &e)
        {
            MyBasePtr->DEBUGLog("点赞信息更新失败", true);
            RespJson["ErrorMsg"].append(e.base().what());
            RespJson["ErrorMsg"].append("点赞信息更新失败");
            RespJson["Result"] = false;
            MyBasePtr->INFO_Func("Agree", false, RespJson);
            return;
        }
        MyBasePtr->DEBUGLog("点赞信息更新成功", true);
    }
    RespJson["Result"] = true;
    RespJson["Agree_Data"] = Agree.toJson();
    MyBasePtr->INFO_Func("Agree", false, RespJson);
    return;
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
    string ChapterTitle = ContentJson["Chapter_Title"].asString();// 章节名
    int Part_Num = ContentJson["Vol_Num"].asInt();             // 章节卷
    int Chapter_Num = ContentJson["Chapter_Num"].asInt();       // 章节数
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
        // ChapterJson = BookMemo["Chapter"];

        // int vecPartSize = ChapterJson.size();
        // Part_Num = vecPartSize - 1;
        // MyBasePtr->DEBUGLog("查询Book/Memo获取章节卷成功", true);
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
            if (ReqJson["Examine_Type"].asString() == "Chapter")
            {
                ReqJson["Examine_Result"] = true;
                Examine_Upload(ReqJson, RespJson);
                MyBasePtr->INFO_Func("Auto_Examine_Chapter_Update", false, RespJson);
                return;
            }

            // 如果是更正章节则直接拒绝
            if (ReqJson["Examine_Type"].asString() == "Chapter_Update")
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
            if (ReqJson["Examine_Type"].asString() == "Chapter")
            {
                ReqJson["Examine_Explain"] = "要上传的章节已存在";
                ReqJson["Examine_Result"] = false;
                Examine_Upload(ReqJson, RespJson);
                MyBasePtr->INFO_Func("Auto_Examine_Chapter_Update", false, RespJson);
                return;
            }

            // 如果是更正章节则等待管理员审核
            if (ReqJson["Examine_Type"].asString() == "Chapter_Update")
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


// 修改用户积分数据
/*
    修改用户数据
Req:{
    "User_ID"           :   0,
    "Processor"         :   "",
    "Change_ID"         :   0,
    "Change_Num"        :   0,
    "Change_Type"       :   "Add/Sub/Deduction/Return",
    "Change_Explain"    :   ""
}

Resp:{
    "User_Data":{},
    "Action_Data":{},
    "ErrorMsg" : []
}
*/
bool MyDBService::Change_User_Integral(Json::Value &ReqJson, Json::Value &RespJson)
{
    auto MyJsonPtr = app().getPlugin<MyJson>();
    auto MyBasePtr = app().getPlugin<MyBase>();
    auto MyToolsPtr = app().getPlugin<MyTools>();
    auto MyRootPtr = app().getPlugin<MyRoot>();
    MyBasePtr->INFO_Func("Change_User_Integral", true, ReqJson);

    auto dbclientPrt = drogon::app().getDbClient();
    Mapper<drogon_model::novel::User> UserMgr(dbclientPrt);
    Mapper<drogon_model::novel::Upload> UploadMgr(dbclientPrt);
    Mapper<drogon_model::novel::Book> BookMgr(dbclientPrt);
    drogon_model::novel::User user;

    // 检查ReqJson数据是否合法
    
    try
    {
        MyBasePtr->DEBUGLog("开始检查ReqJson数据是否合法", true);
        // "User_ID"           :   0,
        // "Processor"         :   "",
        // "Change_ID"         :   0,
        // "Change_Num"        :   0,
        // "Change_Type"       :   "Add/Sub",
        // "Change_Explain"    :   ""
        std::map<string, MyJson::ColType> ColMap;
        ColMap["User_ID"] = MyJson::ColType::INT;
        ColMap["Processor"] = MyJson::ColType::STRING;
        ColMap["Change_ID"] = MyJson::ColType::INT;
        ColMap["Change_Num"] = MyJson::ColType::INT;
        ColMap["Change_Type"] = MyJson::ColType::STRING;
        ColMap["Change_Explain"] = MyJson::ColType::STRING;
        MyJsonPtr->checkMemberAndTypeInMap(ReqJson, RespJson, ColMap);
        MyBasePtr->DEBUGLog("ReqJson数据合法", true);

        if(ReqJson["Change_Type"].asString() != "Sub" && ReqJson["Change_Type"].asString() != "Add")
        {
            RespJson["ErrorMsg"].append("Change_Type字段类型错误");
            throw RespJson;
        }
    }
    catch (Json::Value &e)
    {
        RespJson["ErrorMsg"] = e["ErrorMsg"];
        RespJson["ErrorMsg"].append("ReqJson数据不合法");
        MyBasePtr->INFO_Func("Change_User_Integral", false, RespJson);
        return false;
    }

    int UserID,ChangeID,ChangeNum;
    string ChangeType, Processor, ChangeExplain;
    UserID = ReqJson["User_ID"].asInt();
    ChangeID = ReqJson["Change_ID"].asInt();
    ChangeNum = ReqJson["Change_Num"].asInt();
    Processor = ReqJson["Processor"].asString();
    ChangeType = ReqJson["Change_Type"].asString();
    ChangeExplain = ReqJson["Change_Explain"].asString();

    try
    {
        MyBasePtr->DEBUGLog("开始查询用户 : " + to_string(ChangeID), true);
        user = UserMgr.findByPrimaryKey(ChangeID);
        MyBasePtr->DEBUGLog("用户查询完毕 : " + user.toJson().toStyledString(), true);

        
        
        Json::Value ActionJson;
        ActionJson["Action_Memo"]["Old_Data"]["Integral"] = user.getValueOfIntegral();
        ActionJson["Action_Memo"]["Old_Data"]["TotalIntegral"] = user.getValueOfTotalIntegral();
        if(ChangeType == "Sub")
        {
            // 表示积分的正常使用
            user.setIntegral(user.getValueOfIntegral() - ChangeNum);
        }
        else if(ChangeType == "Add")
        {
            // 表示积分的正常获取
            user.setIntegral(user.getValueOfIntegral() + ChangeNum);
            user.setTotalIntegral(user.getValueOfTotalIntegral() + ChangeNum);
            Json::Value data = MyRootPtr->getCurrentLevelConfig(user.getValueOfTotalIntegral());
            if(data["Level"].asInt()>user.getValueOfLevel())
            {
                MyBasePtr->INFOLog("用户("+to_string(user.getValueOfUserId())+")升级了",true);
                user.setPower(data["Power"].asInt());// 更新权限
                user.setLevel(data["Level"].asInt());// 更新等级
            }
        }
        else if(ChangeType == "Return")
        {
            // 表示积分的错误扣除后的返还
            user.setIntegral(user.getValueOfIntegral() + ChangeNum);
        }
        else if(ChangeType == "Deduction")
        {
            // 表示积分的惩罚性扣除
            user.setIntegral(user.getValueOfIntegral() - ChangeNum);
            user.setTotalIntegral(user.getValueOfTotalIntegral() - ChangeNum);
        }

        MyBasePtr->DEBUGLog("开始更新用户(ID=" + to_string(ChangeID) + ")积分数据", true);
        int row = UserMgr.update(user);
        if (row != 1)
        {
            RespJson["ErrorMsg"].append("用户积分数据更新失败");
            MyBasePtr->INFO_Func("Change_User_Integral", false, RespJson);
            return false;
        }
        MyBasePtr->DEBUGLog("用户积分数据更新完成: " + user.toJson().toStyledString(), true);
        RespJson["User_Data"]["Integral"] = user.getValueOfIntegral();
        RespJson["User_Data"]["TotalIntegral"] = user.getValueOfTotalIntegral();
        MyBasePtr->DEBUGLog("开始记录行为数据", true);
        // Memo:{
        //     Explain:"",
        //     Num:0,
        //     Processor:“system/admin:xxx”,
        //     Type:"Add/Sub",
        //     Old_Data:0,
        //     New_Data:0
        // }
        ActionJson["User_ID"] = ChangeID;
        ActionJson["Action_Type"] = "User_Integral";
        ActionJson["Action_Memo"]["Explain"] = ChangeExplain;
        ActionJson["Action_Memo"]["Processor"] = Processor + "("+ ReqJson["User_ID"].asString() +")";
        ActionJson["Action_Memo"]["Type"] = ChangeType;
        ActionJson["Action_Memo"]["Num"] = ChangeNum;
        ActionJson["Action_Memo"]["New_Data"]["Integral"] = user.getValueOfIntegral();
        ActionJson["Action_Memo"]["New_Data"]["TotalIntegral"] = user.getValueOfTotalIntegral();
        if (!Insert_Action(ActionJson, RespJson))
        {
            LOG_DEBUG << "行为数据记录失败";
            MyBasePtr->DEBUGLog("行为数据记录失败", true);
            MyBasePtr->INFO_Func("Change_User_Integral", false, RespJson);
        }
        MyBasePtr->DEBUGLog("行为数据记录完毕", true);
    }
    catch (const drogon::orm::DrogonDbException &e)
    {
        if (e.base().what() == string("0 rows found"))
        {
            RespJson["ErrorMsg"].append("要更改的用户不存在");
        }
        else if (e.base().what() == string("Found more than one row"))
        {
            RespJson["ErrorMsg"].append("要更改的用户ID重复,请联系管理员");
        }
        else
        {
            RespJson["ErrorMsg"].append(e.base().what());
        }
        MyBasePtr->INFO_Func("Change_User_Integral", false, RespJson);
        return false;
    }

    MyBasePtr->INFO_Func("Change_User_Integral", false, RespJson);
    return true;
}


// 修改用户状态  封号/解封
/*
    修改用户状态  封号/解封
Req:{
    "User_ID"           :   0,
    "Processor"         :   "",
    "Change_ID"         :   0,
    "Change_Type"       :   "Ban/Unseal",
    ["Limit_Time"]      :   0,// Type为Ban时代表封多少天 
    "Change_Explain"    :   ""
}

Resp:{
    "ErrorMsg" : [],
    "Ban_Time" : "",// Type为Unseal时且失败的情况下存在此字段，代表实际封号截止时间
    "User_Status":"",// 更新后 用户的状态
}
*/
bool MyDBService::Change_User_Status(Json::Value &ReqJson, Json::Value &RespJson)
{
    auto MyJsonPtr = app().getPlugin<MyJson>();
    auto MyBasePtr = app().getPlugin<MyBase>();
    auto MyToolsPtr = app().getPlugin<MyTools>();
    MyBasePtr->INFO_Func("Change_User_Status", true, ReqJson);

    auto dbclientPrt = drogon::app().getDbClient();
    Mapper<drogon_model::novel::User> UserMgr(dbclientPrt);
    Mapper<drogon_model::novel::Upload> UploadMgr(dbclientPrt);
    Mapper<drogon_model::novel::Book> BookMgr(dbclientPrt);
    drogon_model::novel::User user;

    // 检查ReqJson数据是否合法
    
    try
    {
        MyBasePtr->DEBUGLog("开始检查ReqJson数据是否合法", true);
        // "User_ID"           :   0,
        // "Processor"         :   "",
        // "Change_ID"         :   0,
        // "Limit_Time"       :   0,
        // "Change_Type"       :   "Add/Sub",
        // "Change_Explain"    :   ""
        std::map<string, MyJson::ColType> ColMap;
        ColMap["User_ID"] = MyJson::ColType::INT;
        ColMap["Processor"] = MyJson::ColType::STRING;
        ColMap["Change_ID"] = MyJson::ColType::INT;
        ColMap["Change_Type"] = MyJson::ColType::STRING;
        ColMap["Change_Explain"] = MyJson::ColType::STRING;
        MyJsonPtr->checkMemberAndTypeInMap(ReqJson, RespJson, ColMap);
        
        if(ReqJson["Change_Type"].asString() != "Ban" && ReqJson["Change_Type"].asString() != "Unseal")
        {
            RespJson["ErrorMsg"].append("Change_Type字段类型错误");
            throw RespJson;
        }
        if(ReqJson["Change_Type"].asString() == "Ban")
        {
            ColMap.clear();
            ColMap["Limit_Time"] = MyJson::ColType::INT;
            MyJsonPtr->checkMemberAndTypeInMap(ReqJson, RespJson, ColMap);
        }
        MyBasePtr->DEBUGLog("ReqJson数据合法", true);

    }
    catch (Json::Value &e)
    {
        RespJson["ErrorMsg"] = e["ErrorMsg"];
        RespJson["ErrorMsg"].append("ReqJson数据不合法");
        MyBasePtr->INFO_Func("Change_User_Status", false, RespJson);
        return false;
    }

    bool NeedUpdate = true;// 是否需要更新
    int UserID,ChangeID,LimitTime;
    string ChangeType, Processor, ChangeExplain,Ban_Time;
    UserID = ReqJson["User_ID"].asInt();
    ChangeID = ReqJson["Change_ID"].asInt();
    Processor = ReqJson["Processor"].asString();
    ChangeType = ReqJson["Change_Type"].asString();
    ChangeExplain = ReqJson["Change_Explain"].asString();

    if(ReqJson["Change_Type"].asString() == "Ban")
    {
        LimitTime = ReqJson["Limit_Time"].asInt();
    }
    else if(ReqJson["Change_Type"].asString() == "Unseal")
    {
        Ban_Time = ReqJson["Ban_Time"].asString();
    }

    try
    {
        /*
            {
                string time = "";
                trantor::Date now,after;
                now = trantor::Date::now();
                after = now.after(60*60*24*LimitTime);

                MyBasePtr->DEBUGLog("now.secondsSinceEpoch: \t\t\t\t"+ to_string(now.secondsSinceEpoch()) , true);
                MyBasePtr->DEBUGLog("now.secondsSinmicroSecondsSinceEpochceEpoch: \t"+ to_string(now.microSecondsSinceEpoch()) , true);
                MyBasePtr->DEBUGLog("now.toDbStringLocal: \t\t\t\t"+now.toDbStringLocal(), true);
                MyBasePtr->DEBUGLog("now.toFormattedString: \t\t\t\t"+now.toFormattedString(false), true);
                MyBasePtr->DEBUGLog("now.toFormattedStringLocal: \t\t\t"+now.toFormattedStringLocal(false), true);
                MyBasePtr->DEBUGLog("now.toCustomedFormattedString: \t\t\t"+now.toCustomedFormattedString("%Y-%m-%d %H:%M:%S",false), true);
                MyBasePtr->DEBUGLog("now.toCustomedFormattedStringLocal: \t\t"+now.toCustomedFormattedStringLocal("%Y-%m-%d %H:%M:%S",false), true);

                MyBasePtr->DEBUGLog("after.secondsSinceEpoch: \t\t\t\t"+ to_string(after.secondsSinceEpoch()) , true);
                MyBasePtr->DEBUGLog("after.secondsSinmicroSecondsSinceEpochceEpoch: \t"+ to_string(after.microSecondsSinceEpoch()) , true);
                MyBasePtr->DEBUGLog("after.toDbStringLocal: \t\t\t\t"+after.toDbStringLocal(), true);
                MyBasePtr->DEBUGLog("after.toFormattedString: \t\t\t\t"+after.toFormattedString(false), true);
                MyBasePtr->DEBUGLog("after.toFormattedStringLocal: \t\t\t"+after.toFormattedStringLocal(false), true);
                MyBasePtr->DEBUGLog("after.toCustomedFormattedString: \t\t\t"+after.toCustomedFormattedString("%Y-%m-%d %H:%M:%S",false), true);
                MyBasePtr->DEBUGLog("after.toCustomedFormattedStringLocal: \t\t"+after.toCustomedFormattedStringLocal("%Y-%m-%d %H:%M:%S",false), true);

                MyBasePtr->DEBUGLog("trantor::Date::now().after(60*60*24*LimitTime).secondsSinceEpoch: \t\t\t\t"+ to_string(trantor::Date::now().after(60*60*24*LimitTime).secondsSinceEpoch()) , true);
                MyBasePtr->DEBUGLog("trantor::Date::now().after(60*60*24*LimitTime).secondsSinmicroSecondsSinceEpochceEpoch: \t"+ to_string(trantor::Date::now().after(60*60*24*LimitTime).microSecondsSinceEpoch()) , true);
                MyBasePtr->DEBUGLog("trantor::Date::now().after(60*60*24*LimitTime).toDbStringLocal: \t\t\t\t"+trantor::Date::now().after(60*60*24*LimitTime).toDbStringLocal(), true);
                MyBasePtr->DEBUGLog("trantor::Date::now().after(60*60*24*LimitTime).toFormattedString: \t\t\t\t"+trantor::Date::now().after(60*60*24*LimitTime).toFormattedString(false), true);
                MyBasePtr->DEBUGLog("trantor::Date::now().after(60*60*24*LimitTime).toFormattedStringLocal: \t\t\t"+trantor::Date::now().after(60*60*24*LimitTime).toFormattedStringLocal(false), true);
                MyBasePtr->DEBUGLog("trantor::Date::now().after(60*60*24*LimitTime).toCustomedFormattedString: \t\t\t"+trantor::Date::now().after(60*60*24*LimitTime).toCustomedFormattedString("%Y-%m-%d %H:%M:%S",false), true);
                MyBasePtr->DEBUGLog("trantor::Date::now().after(60*60*24*LimitTime).toCustomedFormattedStringLocal: \t\t"+trantor::Date::now().after(60*60*24*LimitTime).toCustomedFormattedStringLocal("%Y-%m-%d %H:%M:%S",false), true);
            
                now.secondsSinceEpoch:                          1649816487
                now.secondsSinmicroSecondsSinceEpochceEpoch:    1649816487028409
                now.toDbStringLocal:                            2022-04-13 10:21:27.028409
                now.toFormattedString:                          20220413 02:21:27
                now.toFormattedStringLocal:                     20220413 10:21:27
                now.toCustomedFormattedString:                  2022-04-13 02:21:27
                now.toCustomedFormattedStringLocal:             2022-04-13 10:21:27

                after.secondsSinceEpoch:                        1649902887
                after.secondsSinmicroSecondsSinceEpochceEpoch:  1649902887028409
                after.toDbStringLocal:                          2022-04-14 10:21:27.028409
                after.toFormattedString:                        20220414 02:21:27
                after.toFormattedStringLocal:                   20220414 10:21:27
                after.toCustomedFormattedString:                2022-04-14 02:21:27
                after.toCustomedFormattedStringLocal:           2022-04-14 10:21:27

                trantor::Date::now().after(60*60*24*LimitTime).secondsSinceEpoch:                               1649902887
                trantor::Date::now().after(60*60*24*LimitTime).secondsSinmicroSecondsSinceEpochceEpoch:         1649902887028543
                trantor::Date::now().after(60*60*24*LimitTime).toDbStringLocal:                                 2022-04-14 10:21:27.028560
                trantor::Date::now().after(60*60*24*LimitTime).toFormattedString:                               20220414 02:21:27
                trantor::Date::now().after(60*60*24*LimitTime).toFormattedStringLocal:                          20220414 10:21:27
                trantor::Date::now().after(60*60*24*LimitTime).toCustomedFormattedString:                       2022-04-14 02:21:27
                trantor::Date::now().after(60*60*24*LimitTime).toCustomedFormattedStringLocal:                  2022-04-14 10:21:27
            
            }
        */

        MyBasePtr->DEBUGLog("开始查询用户 : " + to_string(ChangeID), true);
        user = UserMgr.findByPrimaryKey(ChangeID);
        MyBasePtr->DEBUGLog("用户查询完毕 : " + user.toJson().toStyledString(), true);

        Json::Value ActionJson,TempReq,TempResp;
        // 封号
        TempReq["User_ID"]=ChangeID;
        if(ChangeType == "Ban")
        {
            trantor::Date now = trantor::Date::date();
            MyBasePtr->DEBUGLog("开始准备封号,当前时间为:"+now.toDbStringLocal(), true);
            
            // 如果用户已经处于封号时间则延长其时间
            if(user.getValueOfStatus() == "Ban")
            {
                NeedUpdate = false;
                MyBasePtr->DEBUGLog("用户当前处于封号状态", true);
                MyBasePtr->DEBUGLog("开始查询用户封号截止时间", true);
                Search_User_Ban_Time(TempReq,TempResp);
                if(!TempResp["Result"].asBool())
                {
                    RespJson["ErrorMsg"] = TempResp["ErrorMsg"];
                    RespJson["ErrorMsg"].append("查询用户封号截止时间发生错误");
                    MyBasePtr->INFO_Func("Change_User_Status", false, RespJson);
                    return false;
                }
                now = trantor::Date::fromDbStringLocal(TempResp["Ban_Time"].asString());
                MyBasePtr->DEBUGLog("用户封号截止时间为"+now.toDbStringLocal(), true);
            }else
            {
                user.setStatus("Ban");
            }
            MyBasePtr->DEBUGLog("准备封号"+to_string(LimitTime)+"天", true);
            now = now.after(60*60*24*LimitTime);
            MyBasePtr->DEBUGLog("封号截止时间设置为:"+now.toDbStringLocal(), true);
            ActionJson["Action_Memo"]["Ban_Time"] = now.toDbStringLocal();
        }
        // 解封
        else if(ChangeType == "Unseal")
        {

            if(user.getValueOfStatus() == "Normal")
            {
                RespJson["ErrorMsg"].append("用户本来就是正常状态，无需解封");
                MyBasePtr->INFO_Func("Change_User_Status", false, RespJson);
                return false;
            }
            user.setStatus("Normal");
        }

        MyBasePtr->DEBUGLog("开始更新用户(ID=" + to_string(ChangeID) + ")状态数据", true);
        if(NeedUpdate)
        {
            int row = UserMgr.update(user);
            if (row != 1)
            {
                RespJson["ErrorMsg"].append("用户状态数据更新失败");
                MyBasePtr->INFO_Func("Change_User_Status", false, RespJson);
                return false;
            }
        }
        MyBasePtr->DEBUGLog("用户状态数据更新完成: " + user.toJson().toStyledString(), true);
        RespJson["User_Status"] = user.getValueOfStatus();
        MyBasePtr->DEBUGLog("开始记录行为数据", true);
        // Memo:{
        //     Explain:"",
        //     Processor:"system/admin:xxx",
        //     Type:"Ban/Unseal",
        //     Limit_Time:yyyy_mm_dd
        //}
        ActionJson["User_ID"] = ChangeID;
        ActionJson["Action_Type"] = "User_Status";
        ActionJson["Action_Memo"]["Explain"] = ChangeExplain;
        ActionJson["Action_Memo"]["Processor"] = Processor + "("+ ReqJson["User_ID"].asString() +")";
        ActionJson["Action_Memo"]["Type"] = ChangeType;
        
        if (!Insert_Action(ActionJson, RespJson))
        {
            LOG_DEBUG << "行为数据记录失败";
            MyBasePtr->DEBUGLog("行为数据记录失败", true);
            MyBasePtr->INFO_Func("Change_User_Status", false, RespJson);
        }
        MyBasePtr->DEBUGLog("行为数据记录完毕", true);
    }
    catch (const drogon::orm::DrogonDbException &e)
    {
        if (e.base().what() == string("0 rows found"))
        {
            RespJson["ErrorMsg"].append("要更改的用户不存在");
        }
        else if (e.base().what() == string("Found more than one row"))
        {
            RespJson["ErrorMsg"].append("要更改的用户ID重复,请联系管理员");
        }
        else
        {
            RespJson["ErrorMsg"].append(e.base().what());
        }
        MyBasePtr->INFO_Func("Change_User_Status", false, RespJson);
        return false;
    }

    MyBasePtr->INFO_Func("Change_User_Status", false, RespJson);
    return true;
}


// 修改意见状态
/*
    修改用户状态  封号/解封
Req:{
    "Idea_ID":0,
    "IsManage":true/false,
    "Processor":"",
    "Status":""
}

Resp:{
    "ErrorMsg" : [],
    "Idea_Data":{}
}
*/
bool MyDBService::Change_Idea_Status(Json::Value &ReqJson, Json::Value &RespJson)
{
    auto MyJsonPtr = app().getPlugin<MyJson>();
    auto MyBasePtr = app().getPlugin<MyBase>();
    auto MyToolsPtr = app().getPlugin<MyTools>();
    MyBasePtr->INFO_Func("Change_Idea_Status", true, ReqJson);

    auto dbclientPrt = drogon::app().getDbClient();
    Mapper<drogon_model::novel::Idea> IdeaMgr(dbclientPrt);
    drogon_model::novel::Idea idea;

    // 检查ReqJson数据是否合法
    
    try
    {
        MyBasePtr->DEBUGLog("开始检查ReqJson数据是否合法", true);
        // "Idea_ID":0,
        // "IsManage":0,
        // "Processor":"",
        // "Status":""
        std::map<string, MyJson::ColType> ColMap;
        ColMap["Idea_ID"] = MyJson::ColType::INT;
        ColMap["IsManage"] = MyJson::ColType::BOOL;
        ColMap["Processor"] = MyJson::ColType::STRING;
        ColMap["Status"] = MyJson::ColType::STRING;
        MyJsonPtr->checkMemberAndTypeInMap(ReqJson, RespJson, ColMap);
        MyBasePtr->DEBUGLog("ReqJson数据合法", true);

    }
    catch (Json::Value &e)
    {
        RespJson["ErrorMsg"] = e["ErrorMsg"];
        RespJson["ErrorMsg"].append("ReqJson数据不合法");
        MyBasePtr->INFO_Func("Change_Idea_Status", false, RespJson);
        return false;
    }

    try
    {
        MyBasePtr->DEBUGLog("开始根据意见ID查询数据 : " + ReqJson["Idea_ID"].asString(), true);
        idea = IdeaMgr.findByPrimaryKey(ReqJson["Idea_ID"].asInt());
        MyBasePtr->DEBUGLog("意见查询完毕 : " + idea.toJson().toStyledString(), true);
    }
    catch (const drogon::orm::DrogonDbException &e)
    {
        if (e.base().what() == string("0 rows found"))
        {
            RespJson["ErrorMsg"].append("要更改的意见不存在");
        }
        else if (e.base().what() == string("Found more than one row"))
        {
            RespJson["ErrorMsg"].append("要更改的意见ID重复,请联系管理员");
        }
        else
        {
            RespJson["ErrorMsg"].append(e.base().what());
        }
        MyBasePtr->INFO_Func("Change_Idea_Status", false, RespJson);
        return false;
    }

    try
    {
        if(ReqJson.isMember("Explain"))
        {
            Json::Value MemoJson;
            MyJsonPtr->JsonstrToJson(MemoJson,idea.getValueOfMemo());
            MemoJson["Explain"] = ReqJson["Explain"].asString();
            idea.setMemo(MemoJson.toStyledString());
        }
        
        idea.setStatus(ReqJson["Status"].asString());
        idea.setIsmanage(ReqJson["IsManage"].asBool());
        idea.setProcessor(ReqJson["Processor"].asString());
        MyBasePtr->DEBUGLog("开始更新数据 : " + idea.toJson().toStyledString(), true);
        int row = IdeaMgr.update(idea);
        if(row == 0)
        {
            RespJson["ErrorMsg"].append("数据更新失败(数据无需更新)");
            MyBasePtr->INFO_Func("Change_Idea_Status", false, RespJson);
            return false;
        }
        MyBasePtr->DEBUGLog("数据更新完成"  + idea.toJson().toStyledString(), true);
    }
    catch (const drogon::orm::DrogonDbException &e)
    {
        string error =e.base().what();
        RespJson["ErrorMsg"].append("数据更新失败:"+error);
        MyBasePtr->INFO_Func("Change_Idea_Status", false, RespJson);
        return false;
    }
    catch (Json::Value &e)
    {
        RespJson["ErrorMsg"] = e["ErrorMsg"];
        MyBasePtr->INFO_Func("Change_Idea_Status", false, RespJson);
        return false;
    }
    catch(...)
    {
        RespJson["ErrorMsg"].append("数据更新发生异常");
        MyBasePtr->INFO_Func("Change_Idea_Status", false, RespJson);
        return false;
    }
    RespJson["Idea_Data"] = idea.toJson();
    MyJsonPtr->JsonstrToJson(RespJson["Idea_Data"]["Memo"],RespJson["Idea_Data"]["Memo"].asString());
    MyBasePtr->INFO_Func("Change_Idea_Status", false, RespJson);
    return true;
}



// 创建评论
/*
    创建评论
Req:{
    "Note_ID"    :   0, // 帖子ID
    "Comment_Content"  :  {
        "Content":"",//表示内容
        ["URL"]:0,//表示超链接，可以连接到别的表，用于将资源贴引用到求助帖
        // url格式为Note_ID
    },
    "Para"      :{},// 用户携带的Token数据
    ["Reply_Floor_ID"]  : 0,// 回复上层评论，必须是本贴下的评论
}
Resp:{
    "ErrorMsg"  :   [],//失败返回的错误信息
    "Result"    :   true/false,// 操作结果
    "Comment_Data" :   {},// 成功返回的评论信息
}
*/
void MyDBService::Create_Comment(Json::Value &ReqJson, Json::Value &RespJson)
{
    auto MyJsonPtr = app().getPlugin<MyJson>();
    auto MyBasePtr = app().getPlugin<MyBase>();
    auto MyToolsPtr = app().getPlugin<MyTools>();
    MyBasePtr->INFO_Func("Create_Comment", true, ReqJson);

    auto dbclientPrt = drogon::app().getDbClient();
    Mapper<drogon_model::novel::Comment> CommentMgr(dbclientPrt);

    int UserID;
    string LoginStatus;
    Json::Value ParaJson, CommentContentJson, TempJson;
    // 插入前检查帖子数据是否合法
    try
    {
        MyBasePtr->DEBUGLog("创建评论前检查评论数据是否合法", true);
        // "Note_ID"    :   0, // 帖子标题,图书资源帖标题由书名(作者名)构成
        // "Comment_Content"  :  {
        //     "Content":"",//表示内容
        //     ["URL"]:0,//表示超链接，可以连接到别的表，用于将资源贴引用到求助帖
        //     // url格式为Note_ID
        // },
        // "Para"      :{},// 用户携带的Token数据
        std::map<string, MyJson::ColType> ColMap;
        ColMap["Note_ID"] = MyJson::ColType::INT;
        ColMap["Comment_Content"] = MyJson::ColType::JSON;
        ColMap["Para"] = MyJson::ColType::JSON;
        MyJsonPtr->checkMemberAndTypeInMap(ReqJson, RespJson, ColMap);
        MyBasePtr->DEBUGLog("创建评论前检查评论数据显示合法", true);

        MyBasePtr->DEBUGLog("开始检查评论的帖子是否存在", true);
        if (!Is_Note_Exist(ReqJson, RespJson))
        {
            RespJson["ErrorMsg"].append("评论的帖子不存在");
            throw RespJson;
        }
        MyBasePtr->DEBUGLog("评论的帖子存在", true);

        MyBasePtr->DEBUGLog("开始检查Para数据", true);
        ColMap.clear();
        ParaJson = ReqJson["Para"];
        ColMap["User_ID"] = MyJson::ColType::STRING;
        ColMap["Login_Status"] = MyJson::ColType::STRING;
        MyJsonPtr->checkMemberAndTypeInMap(ParaJson, RespJson, ColMap);
        MyBasePtr->DEBUGLog("检查Para数据完成", true);

        MyBasePtr->DEBUGLog("开始检查操作权限", true);
        UserID = std::atoi(ParaJson["User_ID"].asString().c_str());
        LoginStatus = ParaJson["Login_Status"].asString();
        if ((LoginStatus != "user") && (LoginStatus != "admin") && (LoginStatus != "root") && (LoginStatus != "system"))
        {
            RespJson["ErrorMsg"].append("账户权限错误,请联系管理员");
            throw RespJson;
        }
        MyBasePtr->DEBUGLog("检查操作权限通过", true);

        // "Comment_Content"  :  {
        //     "Content":"",//表示内容
        //     ["URL"]:0,//表示超链接，可以连接到别的表，用于将资源贴引用到求助帖
        //     // url格式为Note_ID
        // },
        MyBasePtr->DEBUGLog("开始检查Comment_Content数据格式", true);
        CommentContentJson = ReqJson["Comment_Content"];
        ColMap.clear();
        ColMap["Content"] = MyJson::ColType::STRING;
        MyJsonPtr->checkMemberAndTypeInMap(CommentContentJson, RespJson, ColMap);
        MyBasePtr->DEBUGLog("检查Comment_Content数据格式完成", true);

        if (CommentContentJson.isMember("URL"))
        {
            MyBasePtr->DEBUGLog("开始检查引用的帖子是否存在", true);
            if (!CommentContentJson["URL"].isInt())
            {
                RespJson["ErrorMsg"].append("引用数据错误,URL不是int类型的");
                throw RespJson;
            }
            TempJson.clear();
            TempJson["Note_ID"] = ReqJson["URL"].asInt();
            if (!Is_Note_Exist(TempJson, RespJson))
            {
                RespJson["ErrorMsg"].append("引用的帖子不存在");
                throw RespJson;
            }
            MyBasePtr->DEBUGLog("引用的帖子存在", true);
        }

        if (ReqJson.isMember("Reply_Floor_ID"))
        {
            MyBasePtr->DEBUGLog("开始检查回复的评论是否存在", true);
            if (!ReqJson["Reply_Floor_ID"].isInt())
            {
                RespJson["ErrorMsg"].append("回复评论数据错误,Reply_Floor_ID不是int类型的");
                throw RespJson;
            }
            TempJson.clear();
            TempJson["Note_ID"] = ReqJson["Note_ID"].asInt();
            TempJson["Floor_ID"] = ReqJson["Reply_Floor_ID"].asInt();
            if (!Is_Comment_Exist(TempJson, RespJson))
            {
                RespJson["ErrorMsg"].append("回复的评论不存在");
                throw RespJson;
            }
            MyBasePtr->DEBUGLog("回复的评论存在", true);
        }
    }
    catch (Json::Value &e)
    {
        RespJson["ErrorMsg"] = e["ErrorMsg"];
        RespJson["ErrorMsg"].append("评论失败(要创建的评论数据不合法)");
        RespJson["Result"] = false;
        MyBasePtr->INFO_Func("Create_Comment", false, RespJson);
        return;
    }

    drogon_model::novel::Comment NewComment;

    // 准备要创建的评论的数据
    {
        MyBasePtr->DEBUGLog("开始准备新评论数据", true);

        NewComment.setUserId(UserID);
        NewComment.setNoteId(ReqJson["Note_ID"].asInt());
        NewComment.setCommentContent(CommentContentJson.toStyledString());
        if (ReqJson.isMember("Reply_Floor_ID"))
        {
            NewComment.setReplyFloorId(ReqJson["Reply_Floor_ID"].asInt());
        }

        MyBasePtr->DEBUGLog("准备新评论数据完成", true);
    }

    // 准备插入评论数据
    try
    {
        MyBasePtr->DEBUGLog("准备插入的新帖子数据为 : " + NewComment.toJson().toStyledString(), true);
        MyBasePtr->DEBUGLog("准备插入新帖子数据", true);
        CommentMgr.insert(NewComment);
        MyBasePtr->DEBUGLog("插入新帖子数据完毕", true);
        MyBasePtr->DEBUGLog("插入的新数据为 : " + NewComment.toJson().toStyledString(), true);
    }
    // 新建评论失败
    catch (const drogon::orm::DrogonDbException &e)
    {
        RespJson["Result"] = false;
        RespJson["ErrorMsg"].append("创建新评论失败(" + string(e.base().what()) + ")");
        MyBasePtr->INFO_Func("Create_Comment", false, RespJson);
        return;
    }
    catch (...)
    {
        RespJson["Result"] = false;
        RespJson["ErrorMsg"].append("创建新评论失败(数据异常)");
        MyBasePtr->INFO_Func("Create_Comment", false, RespJson);
        return;
    }
    RespJson["Result"] = true;
    RespJson["Comment_Data"] = NewComment.toJson();
    
    MyJsonPtr->JsonstrToJson(RespJson["Comment_Data"]["Comment_Content"],RespJson["Comment_Data"]["Comment_Content"].asString());

    MyBasePtr->INFO_Func("Create_Comment", false, RespJson);
    return;
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
    "ErrorMsg"  :   [],//失败返回的错误信息
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

        if (NoteType != "Help" && NoteType != "Resource")
        {
            RespJson["ErrorMsg"].append("字段Note_Type错误"); //失败返回的错误信息
            RespJson["Result"] = false;                       // 操作结果
            throw RespJson;
        }

        if (ProcessorType != "user" && ProcessorType != "system" && ProcessorType != "admin" && ProcessorType != "root")
        {
            RespJson["ErrorMsg"].append("用户权限(" + ProcessorType + ")不足,请联系管理员"); //失败返回的错误信息
            RespJson["Result"] = false;                                                      // 操作结果
            throw RespJson;
        }

        if (ProcessorType == "user" && NoteType == "Resource")
        {
            RespJson["ErrorMsg"].append("用户权限(" + ProcessorType + ")不足,请联系管理员"); //失败返回的错误信息
            RespJson["Result"] = false;                                                      // 操作结果
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
                    RespJson["Result"] = false;                                // 操作结果
                    throw RespJson;
                }
                try
                {
                    drogon_model::novel::Book book = BookMgr.findByPrimaryKey(NoteContentJson["Book_ID"].asInt());
                    if (book.getValueOfStatus() == "禁止")
                    {
                        RespJson["ErrorMsg"].append("新增资源贴的图书ID已被禁止"); //失败返回的错误信息
                        RespJson["Result"] = false;                                // 操作结果
                        throw RespJson;
                    }
                }
                catch (...)
                {
                    RespJson["ErrorMsg"].append("新增资源贴的图书ID无效"); //失败返回的错误信息
                    RespJson["Result"] = false;                            // 操作结果
                    throw RespJson;
                }
            }
        }

        MyBasePtr->DEBUGLog("数据正确性检查通过", true);
    }
    catch (Json::Value &e)
    {
        RespJson["ErrorMsg"] = e["ErrorMsg"];
        RespJson["ErrorMsg"].append("帖子创建失败(要创建的帖子数据不合法)");
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


// 创建App改进意见
/*
    创建App改进意见
Req:{
    "Para"    : {}
    "Content"    : {},// 建议内容
Resp:{
    "ErrorMsg"  :   [],//失败返回的错误信息
    "Result"    :   true/false,// 操作结果
    "Idea_Data" :   {},// 成功返回的评论信息
}
*/
void MyDBService::Create_Suggestion(Json::Value &ReqJson, Json::Value &RespJson)
{
    
    auto MyJsonPtr = app().getPlugin<MyJson>();
    auto MyBasePtr = app().getPlugin<MyBase>();
    auto MyToolsPtr = app().getPlugin<MyTools>();
    MyBasePtr->INFO_Func("Create_Suggestion", true, ReqJson);

    string LoginStatus;
    Json::Value ParaJson;
    // 插入前检查数据是否合法
    try
    {
        MyBasePtr->DEBUGLog("创建意见前检查意见数据是否合法", true);
        // "Content"    : {},// 建议内容
        // "Para"      :{},// 用户携带的Token数据
        std::map<string, MyJson::ColType> ColMap;
        ColMap["Content"] = MyJson::ColType::STRING;
        ColMap["Para"] = MyJson::ColType::JSON;
        MyJsonPtr->checkMemberAndTypeInMap(ReqJson, RespJson, ColMap);
        MyBasePtr->DEBUGLog("创建意见前检查意见数据显示合法", true);

        MyBasePtr->DEBUGLog("开始检查Para数据", true);
        ColMap.clear();
        ParaJson = ReqJson["Para"];
        ColMap["User_ID"] = MyJson::ColType::STRING;
        ColMap["Login_Status"] = MyJson::ColType::STRING;
        MyJsonPtr->checkMemberAndTypeInMap(ParaJson, RespJson, ColMap);
        MyBasePtr->DEBUGLog("检查Para数据完成", true);

        MyBasePtr->DEBUGLog("开始检查操作权限", true);
        LoginStatus = ParaJson["Login_Status"].asString();
        if ((LoginStatus != "user") && (LoginStatus != "admin") && (LoginStatus != "root") && (LoginStatus != "system"))
        {
            RespJson["ErrorMsg"].append("账户权限错误,请联系管理员");
            throw RespJson;
        }
        MyBasePtr->DEBUGLog("检查操作权限通过", true);
    }
    catch (Json::Value &e)
    {
        RespJson["ErrorMsg"] = e["ErrorMsg"];
        RespJson["ErrorMsg"].append("提交改进意见失败(数据格式不合法)");
        RespJson["Result"] = false;
        MyBasePtr->INFO_Func("Create_Suggestion", false, RespJson);
        return;
    }

    ReqJson["Memo"]["Content"] = ReqJson["Content"];
    ReqJson["Type"] = "App";
    Insert_Idea(ReqJson,RespJson);
    if(!RespJson["Result"].asBool())
    {
        RespJson["ErrorMsg"].append("提交改进意见失败(数据插入异常)");
    }

    MyBasePtr->INFO_Func("Create_Suggestion", false, RespJson);
    return;
}


// 创建举报信息
/*
    创建举报信息
Req:{
    "Type"      :"User_Report"/"Report",//举报类型
    "Para"    : {},
    "Content"    : {},// 举报内容
    "Target":{},// 举报对象
Resp:{
    "ErrorMsg"  :   [],//失败返回的错误信息
    "Result"    :   true/false,// 操作结果
    "Idea_Data" :   {},// 成功返回的评论信息
}
*/
void MyDBService::Create_Report(Json::Value &ReqJson, Json::Value &RespJson)
{
    
    auto MyJsonPtr = app().getPlugin<MyJson>();
    auto MyBasePtr = app().getPlugin<MyBase>();
    auto MyToolsPtr = app().getPlugin<MyTools>();
    MyBasePtr->INFO_Func("Create_Report", true, ReqJson);

    string LoginStatus;
    Json::Value ParaJson,TargetJson;
    // 插入前检查数据是否合法
    try
    {
        MyBasePtr->DEBUGLog("创建举报前检查举报数据是否合法", true);
        // "Content"    : {},// 建议内容
        // "Para"      :{},// 用户携带的Token数据
        // "Type"      :"Comment_Report"/"Chapter_Report",//举报类型
        // "Target":{},// 举报对象
        std::map<string, MyJson::ColType> ColMap;
        ColMap["Content"] = MyJson::ColType::STRING;
        ColMap["Type"] = MyJson::ColType::STRING;
        ColMap["Para"] = MyJson::ColType::JSON;
        ColMap["Target"] = MyJson::ColType::JSON;
        MyJsonPtr->checkMemberAndTypeInMap(ReqJson, RespJson, ColMap);
        // 根据举报类型检查举报对象是否存在
        TargetJson = ReqJson["Target"];
        if(ReqJson["Type"].asString() == "Comment_Report")
        {
            // 如果是Comment_Report，说明举报了用户的帖子发言
            // 需要Comment_ID
            ColMap.clear();
            ColMap["Comment_ID"] = MyJson::ColType::INT;
            MyJsonPtr->checkMemberAndTypeInMap(TargetJson, RespJson, ColMap);
            // 开始检查Comment是否存在
            Json::Value TempResp;
            bool res = Is_CommentID_Exist(TargetJson,TempResp);
            if(!res)
            {
                RespJson["ErrorMsg"].append("举报对象不存在");
                throw RespJson;
            }
        }
        else if(ReqJson["Type"].asString() == "Chapter_Report")
        {
            // 如果是Chapter_Report，说明举报了资源
            // 需要Chapter_ID
            ColMap.clear();
            ColMap["Chapter_ID"] = MyJson::ColType::INT;
            Json::Value TempResp;
            MyJsonPtr->checkMemberAndTypeInMap(TargetJson, RespJson, ColMap);
            // 开始检查需要Chapter是否存在
            Search_Chapter_By_ChapterID(TargetJson,TempResp);
            cout << "TempResp :: " << TempResp.toStyledString() << endl;
            if(!TempResp["Result"].asBool())
            {
                RespJson["ErrorMsg"].append("举报对象不存在");
                throw RespJson;
            }
        }
        else 
        {
            RespJson["ErrorMsg"].append("举报类型错误");
            throw RespJson;
        }
        MyBasePtr->DEBUGLog("创建举报前检查举报数据显示合法", true);

        MyBasePtr->DEBUGLog("开始检查Para数据", true);
        ColMap.clear();
        ParaJson = ReqJson["Para"];
        ColMap["User_ID"] = MyJson::ColType::STRING;
        ColMap["Login_Status"] = MyJson::ColType::STRING;
        MyJsonPtr->checkMemberAndTypeInMap(ParaJson, RespJson, ColMap);
        MyBasePtr->DEBUGLog("检查Para数据完成", true);

        MyBasePtr->DEBUGLog("开始检查操作权限", true);
        LoginStatus = ParaJson["Login_Status"].asString();
        if ((LoginStatus != "user") && (LoginStatus != "admin") && (LoginStatus != "root") && (LoginStatus != "system"))
        {
            RespJson["ErrorMsg"].append("账户权限错误,请联系管理员");
            throw RespJson;
        }
        MyBasePtr->DEBUGLog("检查操作权限通过", true);

    }
    catch (Json::Value &e)
    {
        int size = e["ErrorMsg"].size();
        RespJson["ErrorMsg"] = e["ErrorMsg"];
        RespJson["ErrorMsg"].append("举报失败(" + e["ErrorMsg"][size - 1].asString() +")");
        RespJson["Result"] = false;
        MyBasePtr->INFO_Func("Create_Report", false, RespJson);
        return;
    }

    ReqJson["Memo"]["Explain"] = ReqJson["Content"];
    ReqJson["Memo"]["Target"] = TargetJson;
    ReqJson["Type"] = ReqJson["Type"];
    Insert_Idea(ReqJson,RespJson);
    if(!RespJson["Result"].asBool())
    {
        RespJson["ErrorMsg"].append("提交改进意见失败(数据插入异常)");
    }

    MyBasePtr->INFO_Func("Create_Report", false, RespJson);
    return;
}

// 用户下载已有的书的章节
/*

Req:{
    "Book_ID":0,
    "Chapter_Num":[],
    "Para" : {}
}
Resp:{
    "Result":true/false,//是否成功下载
    ""
}
*/
void MyDBService::Download_Resource_Public(Json::Value &ReqJson, Json::Value &RespJson)
{
    Json::Value ParaJson,TempReq,TempResp;
    auto MyBasePtr = app().getPlugin<MyBase>();
    auto MyJsonPtr = app().getPlugin<MyJson>();
    auto MyRootPtr = app().getPlugin<MyRoot>();
    MyBasePtr->INFO_Func("Download_Resource_Public", true, ReqJson);
    
    int UserID, Book_ID;
    string LoginStatus;
    Json::Value data = MyRootPtr->getIntegralConfig();
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
        RespJson["Result"] = false;
        RespJson["ErrorMsg"] = e["ErrorMsg"];
        RespJson["ErrorMsg"].append("数据校验失败");
        MyBasePtr->INFO_Func("Download_Resource_Public", false, RespJson);
        return;
    }


    MyBasePtr->DEBUGLog("开始记录下载行为数据", true);
    TempReq.clear();
    TempResp.clear();
    TempReq["User_ID"] = atoi(ParaJson["User_ID"].asString().c_str());
    TempReq["Action_Type"] = "Resource_Download";
    // Memo:{Explain:"",Book_ID:0,Chapter_Num:0}
    TempReq["Action_Memo"]["Explain"] = "用户下载图书资源";
    TempReq["Action_Memo"]["Book_ID"] = ReqJson["Book_ID"].asInt();
    TempReq["Action_Memo"]["Chapter_Num"] = ReqJson["Chapter_Num"];
    if (!Insert_Action(TempReq, TempResp))
    {
        RespJson["Result"] = false;
        LOG_DEBUG << "下载行为数据记录失败";
        RespJson["ErrorMsg"] = TempResp["ErrorMsg"];
        RespJson["ErrorMsg"].append("下载行为数据记录失败");
        MyBasePtr->DEBUGLog("下载行为数据记录失败", true);
        MyBasePtr->INFO_Func("Download_Resource_Public", false, RespJson);
        return;
    }
    MyBasePtr->DEBUGLog("下载行为数据记录完毕", true);


    MyBasePtr->DEBUGLog("开始记录下载行为导致的扣除积分行为数据", true);
    // Req:{
    // "User_ID" : 0,
    // "Processor" : "",
    // "Change_ID" : 0,
    // "Change_Num" : 0,
    // "Change_Type" : "Add/Sub",
    // "Change_Explain" : ""
    // }
    TempReq.clear();
    TempResp.clear();
    TempReq["User_ID"] = 0;
    TempReq["Processor"] = "system";
    TempReq["Change_ID"] = atoi(ParaJson["User_ID"].asString().c_str());
    TempReq["Change_Num"] = ReqJson["Chapter_Num"].size()*data["Download"].asInt();
    int PreNum = TempReq["Change_Num"].size();
    TempReq["Change_Type"] = "Sub";
    TempReq["Change_Explain"] = "用户下载图书资源消耗积分(每章"+to_string(data["Download"].asInt())+"积分)";
    if (!Change_User_Integral(TempReq, TempResp))
    {
        RespJson["Result"] = false;
        LOG_DEBUG << "积分扣除行为数据记录失败";
        RespJson["ErrorMsg"] = TempResp["ErrorMsg"];
        RespJson["ErrorMsg"].append("积分扣除行为数据记录失败");
        MyBasePtr->DEBUGLog("积分扣除行为数据记录失败", true);
        MyBasePtr->INFO_Func("Download_Resource_Public", false, RespJson);
        return;
    }
    MyBasePtr->DEBUGLog("积分扣除行为数据记录完毕", true);



    MyBasePtr->DEBUGLog("开始获取图书资源数据", true);
    if(!Download_Resource_Private(ReqJson,RespJson))
    {
        RespJson["Result"] = false;
        MyBasePtr->DEBUGLog("获取图书资源数据失败", true);
        RespJson["ErrorMsg"].append("获取图书资源数据失败");
        MyBasePtr->INFO_Func("Download_Resource_Public", false, RespJson);
        return;
    }
    MyBasePtr->DEBUGLog("获取图书资源数据成功", true);

    int AfterNum = RespJson["Chapter_Data"].size()*data["Download"].asInt();
    MyBasePtr->DEBUGLog("预计下载"+to_string(PreNum)+"章,实际下载"+to_string(AfterNum)+"章", true);
    if(PreNum > AfterNum)
    {
        MyBasePtr->DEBUGLog("开始记录积分误扣除导致的返还积分的行为数据", true);
        // Req:{
        // "User_ID" : 0,
        // "Processor" : "",
        // "Change_ID" : 0,
        // "Change_Num" : 0,
        // "Change_Type" : "Add/Sub",
        // "Change_Explain" : ""
        // }
        TempReq.clear();
        TempResp.clear();
        TempReq["User_ID"] = 0;
        TempReq["Processor"] = "system";
        TempReq["Change_ID"] = atoi(ParaJson["User_ID"].asString().c_str());
        TempReq["Change_Num"] = PreNum - AfterNum;
        TempReq["Change_Type"] = "Return";
        TempReq["Change_Explain"] = "返还部分不存在章节预先扣除的积分";
        if (!Change_User_Integral(TempReq, TempResp))
        {
            RespJson["Result"] = false;
            LOG_DEBUG << "返还积分的行为数据记录失败";
            RespJson["ErrorMsg"] = TempResp["ErrorMsg"];
            RespJson["ErrorMsg"].append("返还积分的行为数据记录失败");
            MyBasePtr->DEBUGLog("返还积分的行为数据记录失败", true);
            MyBasePtr->INFO_Func("Download_Resource_Public", false, RespJson);
            return;
        }
        MyBasePtr->DEBUGLog("返还积分的行为数据记录完毕", true);
    }

    RespJson["Result"] = true;
    MyBasePtr->INFO_Func("Download_Resource_Public", false, RespJson);
    return;
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
    "ErrorMsg"  :   [],         //  错误原因
    "Result"    :   true/false, //  执行结果
}
*/
void MyDBService::Examine_Upload(Json::Value &ReqJson, Json::Value &RespJson)
{
    Json::Value UploadMemo,ActionJson,data;
    auto MyBasePtr = app().getPlugin<MyBase>();
    auto MyJsonPtr = app().getPlugin<MyJson>();
    auto MyRootPtr = app().getPlugin<MyRoot>();
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
        else if (ReqJson["Processor_Type"] == "admin")
        {
            Processor = "admin(" + ReqJson["Processor_ID"].asString() + ")";
        }
        else if (ReqJson["Processor_Type"] == "root")
        {
            Processor = "root";
        }
        else
        {
            RespJson["ErrorMsg"].append("Processor_Type字段类型错误");
            RespJson["Result"] = false;
            MyBasePtr->INFO_Func("Examine_Upload", false, RespJson);
            throw RespJson;
        }
    }

    // Type:User_Upload,
    // Memo:{
    //      Refuse_Explain:"",
    //      Processor:“system/admin(xxx)”,
    //      Upload_ID:0,
    //      Type:"refuse/pass"}
    // 开始记录审核行为
    MyBasePtr->DEBUGLog("开始记录审核行为数据", true);
    ActionJson.clear();
    ActionJson["User_ID"] = ExamineUpload.getValueOfUserId();
    ActionJson["Action_Type"] = "Examine_Upload";
    if (ReqJson["Examine_Result"].asBool() == false)
    {
        ActionJson["Action_Memo"]["Explain"] = "审核未通过("+ReqJson["Examine_Explain"].asString()+")";
        ActionJson["Action_Memo"]["Type"] = "refuse";
    }else{
        ActionJson["Action_Memo"]["Explain"] = "审核通过";
        ActionJson["Action_Memo"]["Type"] = "pass";
    }
    ActionJson["Action_Memo"]["Processor"] = ReqJson["Processor_Type"].asString()+"("+ReqJson["Processor_ID"].asString()+")";
    ActionJson["Action_Memo"]["Upload_ID"] = ExamineUpload.getValueOfUploadId();
    if (!Insert_Action(ActionJson, RespJson))
    {
        RespJson["Result"] = false;
        LOG_DEBUG << "审核行为数据记录失败";
        RespJson["ErrorMsg"].append("审核行为数据记录失败");
        MyBasePtr->DEBUGLog("审核行为数据记录失败", true);
        MyBasePtr->INFO_Func("Recharge", false, RespJson);
        return;
    }
    MyBasePtr->DEBUGLog("审核行为数据记录完毕", true);



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
                // 如果成功，则给与用户积分奖励
                data = MyRootPtr->getIntegralConfig();
                ActionJson.clear();
                ActionJson["User_ID"] = atoi(ReqJson["Processor_ID"].asString().c_str());
                ActionJson["Processor"] = Processor;
                ActionJson["Change_ID"] = ExamineUpload.getValueOfUserId();
                ActionJson["Change_Num"] = data["Upload"].asInt();
                ActionJson["Change_Type"] = "Add";
                ActionJson["Change_Explain"] = "用户上传资源成功,1章奖励"+to_string(data["Upload"].asInt())+"积分";
                if(!Change_User_Integral(ActionJson,RespJson))
                {
                    RespJson["ErrorMsg"] = "用户上传成功,奖励积分失败";
                }

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
        ExamineUpload.setStatus(ReqJson["Processor_Type"].asString() + "已允许/" + RespJson["Upload_Status"].asString());
    }
    else
    {
        UploadMemo["Explain"] = ReqJson["Examine_Explain"].asString();
        ExamineUpload.setStatus(ReqJson["Processor_Type"].asString() + "已拒绝");
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
    try
    {
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
        Criteria BookName_cri(drogon_model::novel::Book::Cols::_Book_Name, CompareOperator::EQ, BookName);
        Criteria BookAuthor_cri(drogon_model::novel::Book::Cols::_Author, CompareOperator::EQ, BookAuthor);
        vector<drogon_model::novel::Book> vecBook = BookMgr.findBy(BookName_cri && BookAuthor_cri);

        // 如果不是新书，则将Upload状态改为已处理且拒绝插入
        if (vecBook.size() != 0)
        {
            MyBasePtr->DEBUGLog("图书已存在", true);
            RespJson["Search_Book_ID"] = vecBook[0].getValueOfBookId();
            MyBasePtr->INFO_Func("Is_Book_Exist", false, RespJson);
            return true;
        }
        MyBasePtr->DEBUGLog("图书不存在", true);
        MyBasePtr->INFO_Func("Is_Book_Exist", false, RespJson);
    }
    catch (Json::Value &e)
    {
        MyBasePtr->DEBUGLog("图书不存在", true);
        RespJson["ErrorMsg"] = e["ErrorMsg"];
        MyBasePtr->INFO_Func("Is_Book_Exist", false, RespJson);
        return false;
    }
    catch (const drogon::orm::DrogonDbException &e)
    {
        MyBasePtr->DEBUGLog("图书不存在", true);
        RespJson["ErrorMsg"].append(e.base().what());
        RespJson["ErrorMsg"].append("查询图书失败");
        MyBasePtr->INFO_Func("Is_Book_Exist", false, RespJson);
        return false;
    }

    return false;
}

// 根据帖子ID、评论楼层查询评论是否存在
/*
    根据帖子ID、评论楼层查询评论是否存在
Req:{
    "Note_ID"   :   0,// 帖子ID
    "Floor_ID"  :   0 // 评论楼层
}
Resp:{
    "ErrorMsg"  :   "",// 错误提示
}
*/
bool MyDBService::Is_Comment_Exist(Json::Value &ReqJson, Json::Value &RespJson)
{
    auto MyJsonPtr = app().getPlugin<MyJson>();
    auto MyBasePtr = app().getPlugin<MyBase>();
    auto MyToolsPtr = app().getPlugin<MyTools>();
    MyBasePtr->INFO_Func("Is_Comment_Exist", true, ReqJson);

    auto dbclientPrt = drogon::app().getDbClient();
    Mapper<drogon_model::novel::Comment> CommentMgr(dbclientPrt);

    try
    {
        // 插入前检查图书数据是否合法

        MyBasePtr->DEBUGLog("搜索前检查评论数据是否合法", true);
        // "Note_ID"   :   0,// 帖子ID
        // "Floor_ID"  :   0 // 评论楼层
        std::map<string, MyJson::ColType> ColMap;
        ColMap["Note_ID"] = MyJson::ColType::INT;
        ColMap["Floor_ID"] = MyJson::ColType::INT;
        MyJsonPtr->checkMemberAndTypeInMap(ReqJson, RespJson, ColMap);
        MyBasePtr->DEBUGLog("搜索前检查显示评论数据合法", true);

        // 检查图书是否已存在
        MyBasePtr->DEBUGLog("开始检查评论是否已存在", true);
        Criteria NoteID_cri(drogon_model::novel::Comment::Cols::_Note_ID, CompareOperator::EQ, ReqJson["Note_ID"].asInt());
        Criteria FloorID_cri(drogon_model::novel::Comment::Cols::_Floor_ID, CompareOperator::EQ, ReqJson["Floor_ID"].asInt());
        vector<drogon_model::novel::Comment> vecComment = CommentMgr.findBy(NoteID_cri && FloorID_cri);
        if (vecComment.size() == 0)
        {
            RespJson["ErrorMsg"].append("查询评论不存在");
            MyBasePtr->INFO_Func("Is_Comment_Exist", false, RespJson);
            return false;
        }
    }
    catch (Json::Value &e)
    {
        RespJson["ErrorMsg"] = e["ErrorMsg"];
        MyBasePtr->INFO_Func("Is_Comment_Exist", false, RespJson);
        return false;
    }
    catch (const drogon::orm::DrogonDbException &e)
    {
        RespJson["ErrorMsg"].append(e.base().what());
        RespJson["ErrorMsg"].append("查询评论失败");
        MyBasePtr->INFO_Func("Is_Comment_Exist", false, RespJson);
        return false;
    }

    MyBasePtr->INFO_Func("Is_Comment_Exist", false, RespJson);
    return true;
}

// 根据Comment_ID查询评论是否存在
/*
    根据Comment_ID查询评论是否存在
Req:{
    "Comment_ID"   :   0,// 评论ID
}
Resp:{
    "ErrorMsg"  :   "",// 错误提示
}
*/
bool MyDBService::Is_CommentID_Exist(Json::Value &ReqJson, Json::Value &RespJson)
{
    auto MyJsonPtr = app().getPlugin<MyJson>();
    auto MyBasePtr = app().getPlugin<MyBase>();
    auto MyToolsPtr = app().getPlugin<MyTools>();
    MyBasePtr->INFO_Func("Is_CommentID_Exist", true, ReqJson);

    auto dbclientPrt = drogon::app().getDbClient();
    Mapper<drogon_model::novel::Comment> CommentMgr(dbclientPrt);

    try
    {
        // 插入前检查图书数据是否合法

        MyBasePtr->DEBUGLog("搜索前检查评论数据是否合法", true);
        // "Comment_ID"   :   0,// 评论ID
        std::map<string, MyJson::ColType> ColMap;
        ColMap["Comment_ID"] = MyJson::ColType::INT;
        MyJsonPtr->checkMemberAndTypeInMap(ReqJson, RespJson, ColMap);
        MyBasePtr->DEBUGLog("搜索前检查显示评论数据合法", true);

        // 检查图书是否已存在
        MyBasePtr->DEBUGLog("开始检查评论是否已存在", true);
        Criteria CommentID_cri(drogon_model::novel::Comment::Cols::_Comment_ID, CompareOperator::EQ, ReqJson["Comment_ID"].asInt());
        vector<drogon_model::novel::Comment> vecComment = CommentMgr.findBy(CommentID_cri);
        if (vecComment.size() == 0)
        {
            RespJson["ErrorMsg"].append("查询评论不存在");
            MyBasePtr->INFO_Func("Is_CommentID_Exist", false, RespJson);
            return false;
        }
    }
    catch (Json::Value &e)
    {
        RespJson["ErrorMsg"] = e["ErrorMsg"];
        MyBasePtr->INFO_Func("Is_CommentID_Exist", false, RespJson);
        return false;
    }
    catch (const drogon::orm::DrogonDbException &e)
    {
        RespJson["ErrorMsg"].append(e.base().what());
        RespJson["ErrorMsg"].append("查询评论失败");
        MyBasePtr->INFO_Func("Is_CommentID_Exist", false, RespJson);
        return false;
    }

    MyBasePtr->INFO_Func("Is_CommentID_Exist", false, RespJson);
    return true;
}

// 根据帖子ID查询帖子是否存在
/*
    根据帖子ID查询帖子是否存在
Req:{
    "Note_ID"   :   0
}
Resp:{
    "ErrorMsg"  :   "",// 错误提示
}
*/
bool MyDBService::Is_Note_Exist(Json::Value &ReqJson, Json::Value &RespJson)
{
    auto MyJsonPtr = app().getPlugin<MyJson>();
    auto MyBasePtr = app().getPlugin<MyBase>();
    auto MyToolsPtr = app().getPlugin<MyTools>();
    MyBasePtr->INFO_Func("Is_Note_Exist", true, ReqJson);

    auto dbclientPrt = drogon::app().getDbClient();
    Mapper<drogon_model::novel::Note> NoteMgr(dbclientPrt);

    try
    {
        // 插入前检查图书数据是否合法

        MyBasePtr->DEBUGLog("搜索前检查帖子数据是否合法", true);
        // "Note_ID"   :   0
        std::map<string, MyJson::ColType> ColMap;
        ColMap["Note_ID"] = MyJson::ColType::INT;
        MyJsonPtr->checkMemberAndTypeInMap(ReqJson, RespJson, ColMap);
        MyBasePtr->DEBUGLog("搜索前检查显示帖子数据合法", true);

        // 检查图书是否已存在
        MyBasePtr->DEBUGLog("开始检查帖子是否已存在", true);
        NoteMgr.findByPrimaryKey(ReqJson["Note_ID"].asInt());
    }
    catch (Json::Value &e)
    {
        RespJson["ErrorMsg"] = e["ErrorMsg"];
        MyBasePtr->INFO_Func("Is_Book_Exist", false, RespJson);
        return false;
    }
    catch (const drogon::orm::DrogonDbException &e)
    {
        if (e.base().what() == string("0 rows found"))
        {
            RespJson["ErrorMsg"].append("要查询的帖子不存在");
        }
        else if (e.base().what() == string("Found more than one row"))
        {
            RespJson["ErrorMsg"].append("要查询的帖子ID重复,请联系管理员");
        }
        else
        {
            RespJson["ErrorMsg"].append(e.base().what());
        }
        RespJson["ErrorMsg"].append("查询帖子失败");
        MyBasePtr->INFO_Func("Is_Book_Exist", false, RespJson);
        return false;
    }

    MyBasePtr->INFO_Func("Is_Book_Exist", false, RespJson);
    return true;
}

// 通过UserID查询是否存在
/*
    通过NoteID查找Note数据
Req:{
    "User_ID"  :   0,
}
Resp:{
    "ErrorMsg"  :   [],         // 失败返回的错误信息
}
*/
bool MyDBService::Is_User_Exist(Json::Value &ReqJson, Json::Value &RespJson)
{
    auto MyJsonPtr = app().getPlugin<MyJson>();
    auto MyBasePtr = app().getPlugin<MyBase>();
    auto MyToolsPtr = app().getPlugin<MyTools>();
    MyBasePtr->INFO_Func("Is_User_Exist", true, ReqJson);

    auto dbclientPrt = drogon::app().getDbClient();
    Mapper<drogon_model::novel::User> UserMgr(dbclientPrt);

    try
    {
        // 插入前检查用户数据是否合法

        MyBasePtr->DEBUGLog("搜索前检查ReqJson数据是否合法", true);
        // "User_ID"   :   0
        std::map<string, MyJson::ColType> ColMap;
        ColMap["User_ID"] = MyJson::ColType::INT;
        MyJsonPtr->checkMemberAndTypeInMap(ReqJson, RespJson, ColMap);
        MyBasePtr->DEBUGLog("搜索前检查显示ReqJson数据合法", true);

        // 检查用户是否已存在
        MyBasePtr->DEBUGLog("开始检查用户是否已存在", true);

        UserMgr.findByPrimaryKey(ReqJson["User_ID"].asInt());
    }
    catch (Json::Value &e)
    {
        RespJson["ErrorMsg"] = e["ErrorMsg"];
        MyBasePtr->INFO_Func("Is_User_Exist", false, RespJson);
        return false;
    }
    catch (const drogon::orm::DrogonDbException &e)
    {
        if (e.base().what() == string("0 rows found"))
        {
            RespJson["ErrorMsg"].append("要查询的用户不存在");
        }
        else if (e.base().what() == string("Found more than one row"))
        {
            RespJson["ErrorMsg"].append("要查询的用户ID重复,请联系管理员");
        }
        else
        {
            RespJson["ErrorMsg"].append(e.base().what());
        }
        RespJson["ErrorMsg"].append("查询用户失败");
        MyBasePtr->INFO_Func("Is_User_Exist", false, RespJson);
        return false;
    }

    MyBasePtr->INFO_Func("Is_User_Exist", false, RespJson);
    return true;
}


// 用户上传资源行为记录,有问题则抛异常，&Upload_Type表示资源类型,&Upload_ID表示上传申请的ID
void MyDBService::Log_Resource_Upload(const int &User_ID,const int &Upload_ID,const string &Upload_Type,const string &Explain)
{
    Json::Value ActionJson,RespJson;
    auto MyJsonPtr = app().getPlugin<MyJson>();
    auto MyBasePtr = app().getPlugin<MyBase>();
    auto MyToolsPtr = app().getPlugin<MyTools>();

    // Type:Resource_Upload,
    // Memo:{
    //      Explain:""
    //      Upload_Type:"new_book/old_book_new/old_book_old",
    //      Upload_ID:“system/admin(xxx)”,
    //}
    // 开始记录上传行为
    MyBasePtr->DEBUGLog("开始记录上传行为数据", true);
    ActionJson.clear();
    ActionJson["User_ID"] = User_ID;
    ActionJson["Action_Type"] = "Resource_Upload";
    ActionJson["Action_Memo"]["Explain"] = Explain;
    ActionJson["Action_Memo"]["Processor"] = "system(0)";
    ActionJson["Action_Memo"]["Upload_ID"] = Upload_ID;
    ActionJson["Action_Memo"]["Upload_Type"] = Upload_Type;
    
    MyBasePtr->INFO_Func("Log_Resource_Upload", true, ActionJson);


    if (!Insert_Action(ActionJson, RespJson))
    {
        RespJson["Result"] = false;
        LOG_DEBUG << "上传行为数据记录失败";
        RespJson["ErrorMsg"].append("上传行为数据记录失败");
        MyBasePtr->DEBUGLog("上传行为数据记录失败", true);
        MyBasePtr->INFO_Func("Log_Resource_Upload", false, RespJson);
        throw RespJson;
    }
    RespJson["Result"] = true;
    MyBasePtr->DEBUGLog("上传行为数据记录完毕", true);
    MyBasePtr->INFO_Func("Log_Resource_Upload", false, RespJson);

}

// 用户阅读接口
/*
Req:{
    "Book_ID":0,// 所属图书ID
    "Chapter_Num":0// 章节数
    "Para":{},// 用户参数
    ["Version"]:0,//章节版本号
}
Resp:{
    "Result":true/false,// 结果
    "Chapter_Data":{}, // 章节内容
    "ErrorMsg":[] // 错误信息
}
*/
void MyDBService::Read_Resource(Json::Value &ReqJson, Json::Value &RespJson)
{
    Json::Value ParaJson;
    auto MyJsonPtr = app().getPlugin<MyJson>();
    auto MyBasePtr = app().getPlugin<MyBase>();
    auto MyToolsPtr = app().getPlugin<MyTools>();
    MyBasePtr->INFO_Func("Read_Resource", true, ReqJson);

    auto dbclientPrt = drogon::app().getDbClient();
    Mapper<drogon_model::novel::User> UserMgr(dbclientPrt);
    Mapper<drogon_model::novel::Upload> UploadMgr(dbclientPrt);
    Mapper<drogon_model::novel::Book> BookMgr(dbclientPrt);
    Mapper<drogon_model::novel::Chapter> ChapterMgr(dbclientPrt);

    int UserID, Book_ID, ChapterNum;
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
        ColMap["Chapter_Num"] = MyJson::ColType::INT;
        MyJsonPtr->checkMemberAndTypeInMap(ReqJson, RespJson, ColMap);
        MyBasePtr->DEBUGLog("检查ReqJson数据完成", true);

        MyBasePtr->DEBUGLog("开始检查Para数据", true);
        ColMap.clear();
        ParaJson = ReqJson["Para"];
        ColMap["User_ID"] = MyJson::ColType::STRING;
        ColMap["Login_Status"] = MyJson::ColType::STRING;
        MyJsonPtr->checkMemberAndTypeInMap(ParaJson, RespJson, ColMap);
        MyBasePtr->DEBUGLog("检查Para数据完成", true);
        ChapterNum = ReqJson["Chapter_Num"].asInt();
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
        RespJson["Result"] = false;
        RespJson["ErrorMsg"] = e["ErrorMsg"];
        RespJson["ErrorMsg"].append("数据校验失败");
        MyBasePtr->INFO_Func("Read_Resource", false, RespJson);
        return;
    }


    // 查询章节信息
    try
    {
        MyBasePtr->DEBUGLog("开始查询章节信息", true);
        Criteria BookID_cri(drogon_model::novel::Chapter::Cols::_Book_ID, CompareOperator::EQ, Book_ID);
        Criteria ChapterNum_cri(drogon_model::novel::Chapter::Cols::_Chapter_Num, CompareOperator::EQ, ChapterNum);
        Criteria Valid_cri;
        if(ReqJson.isMember("Version"))
        {
            MyJsonPtr->checkColType(ReqJson,RespJson,"Version",MyJson::ColType::INT);
            Valid_cri = Criteria(drogon_model::novel::Chapter::Cols::_Version, CompareOperator::EQ, ReqJson["Version"].asInt());
        }
        else
        {
            Valid_cri = Criteria(drogon_model::novel::Chapter::Cols::_Valid, CompareOperator::EQ, true);
        }
        vector<drogon_model::novel::Chapter> vecChapter = ChapterMgr.findBy(BookID_cri && ChapterNum_cri && Valid_cri);
        int ChapterSize = vecChapter.size();
        if (ChapterSize == 0)
        {
            RespJson["ErrorMsg"].append("要查询章节不存在");
            throw RespJson;
        }else if(ChapterSize > 1)
        {
            RespJson["ErrorMsg"].append("查询到的章节过多,数据异常，请联系管理员");
            throw RespJson;
        }
        Json::Value TempChapter, TempChapterJsonStr;

        MyJsonPtr->JsonstrToJson(TempChapterJsonStr, vecChapter[0].getValueOfContent());
        TempChapter["Chapter_ID"] = vecChapter[0].getValueOfChapterId();
        TempChapter["Book_ID"] = vecChapter[0].getValueOfBookId();
        TempChapter["Part_Num"] = vecChapter[0].getValueOfPartNum();
        TempChapter["Chapter_Num"] = vecChapter[0].getValueOfChapterNum();
        TempChapter["Title"] = vecChapter[0].getValueOfTitle();
        TempChapter["User_ID"] = vecChapter[0].getValueOfUserId();
        TempChapter["Version"] = vecChapter[0].getValueOfVersion();
        TempChapter["Content"] = TempChapterJsonStr;
        MyJsonPtr->JsonstrToJson(TempChapterJsonStr, vecChapter[0].getValueOfMemo());
        TempChapter["Memo"] = TempChapterJsonStr;

        RespJson["Chapter_Data"] = TempChapter;
        MyBasePtr->DEBUGLog("查询章节信息完成", true);
    }
    catch(Json::Value &e)
    {
        RespJson["Result"] = false;
        RespJson["ErrorMsg"] = e["ErrorMsg"];
        RespJson["ErrorMsg"].append("查询章节信息发生错误");
        MyBasePtr->INFO_Func("Read_Resource", false, RespJson);
        return ;
    }
    RespJson["Result"] = true;
    MyBasePtr->INFO_Func("Read_Resource", false, RespJson);
    return ;

}

// 充值接口
/*
Req:{
    "User_ID":0,// 被充值的用户
    "Money_Num":0,// 充值金额
}
Resp:{
    "ErrorMsg":[],
    "Result" : true/false
}
*/
void MyDBService::Recharge(Json::Value &ReqJson, Json::Value &RespJson)
{
    auto MyBasePtr = app().getPlugin<MyBase>();
    auto MyJsonPtr = app().getPlugin<MyJson>();
    auto MyRootPtr = app().getPlugin<MyRoot>();
    MyBasePtr->INFO_Func("Recharge", true, ReqJson);

    // 检查ReqJson数据是否合法
    try
    {
        MyBasePtr->DEBUGLog("检查ReqJson数据是否合法", true);
        // "User_ID":0,// 被充值的用户
        // "Money_Num":0,// 充值金额
        std::map<string, MyJson::ColType> ColMap;
        ColMap["User_ID"] = MyJson::ColType::INT;
        ColMap["Money_Num"] = MyJson::ColType::INT;
        MyJsonPtr->checkMemberAndTypeInMap(ReqJson, RespJson, ColMap);
        MyBasePtr->DEBUGLog("ReqJson数据合法", true);
    }
    catch (Json::Value &e)
    {
        RespJson["ErrorMsg"] = e["ErrorMsg"];
        RespJson["ErrorMsg"].append("ReqJson数据不合法");
        RespJson["Result"] = false;
        MyBasePtr->INFO_Func("Recharge", false, RespJson);
        return;
    }
    catch (...)
    {
        RespJson["Result"] = false;
        RespJson["ErrorMsg"].append("MyDBService::Recharge::Error");
        MyBasePtr->INFO_Func("Recharge", false, RespJson);
        return;
    }

    Json::Value TempReq,TempResp,ActionJson;
    Json::Value data = MyRootPtr->getIntegralConfig();
    int Integral_Num = ReqJson["Money_Num"].asInt() * data["Recharge"].asInt();

    // "User_ID"           :   0,
    // "Processor"         :   "",
    // "Change_ID"         :   0,
    // "Change_Num"        :   0,
    // "Change_Type"       :   "Add/Sub",
    // "Change_Explain"    :   ""
    // 
    // 开始记录
    // Type:Money,Memo:{Money_Num:0,Integral_Num:0}
        
    // Memo:{
    //     Explain:"",
    //     Num:0,
    //     Processor:“system/admin:xxx”,
    //     Type:"Add/Sub",
    //     Old_Data:0,
    //     New_Data:0
    // }
    MyBasePtr->DEBUGLog("开始记录充值行为数据", true);
    ActionJson.clear();
    ActionJson["User_ID"] = ReqJson["User_ID"].asInt();
    ActionJson["Action_Type"] = "Money";
    Json::Value MemoJson ;
    MemoJson["Explain"] = "用户充值获得积分(积分 = 金额 * "+to_string(data["Recharge"].asInt())+")";
    MemoJson["Processor"] = "system(0)";
    MemoJson["Integral_Num"] = Integral_Num;
    MemoJson["Money_Num"] = ReqJson["Money_Num"].asInt();
    ActionJson["Action_Memo"] = MemoJson;
    if (!Insert_Action(ActionJson, RespJson))
    {
        RespJson["Result"] = false;
        LOG_DEBUG << "充值行为数据记录失败";
        RespJson["ErrorMsg"].append("充值行为数据记录失败");
        MyBasePtr->DEBUGLog("充值行为数据记录失败", true);
        MyBasePtr->INFO_Func("Recharge", false, RespJson);
        return;
    }
    MyBasePtr->DEBUGLog("充值行为数据记录完毕", true);

    // Req:{
    //     "User_ID" : 0,
    //     "Processor" : "",
    //     "Change_ID" : 0,
    //     "Change_Num" : 0,
    //     "Change_Type" : "Add/Sub",
    //     "Change_Explain" : ""
    // }
    TempReq.clear();
    TempReq["User_ID"] = 0;
    TempReq["Processor"] = "system";
    TempReq["Change_ID"] = ReqJson["User_ID"].asInt();
    TempReq["Change_Num"] = Integral_Num;
    TempReq["Change_Type"] = "Add";
    TempReq["Change_Explain"] = "用户充值获得积分(积分 = 金额 * "+to_string(data["Recharge"].asInt())+")";
    MyBasePtr->DEBUGLog("开始充值", true);
    if(!Change_User_Integral(TempReq,TempResp))
    {
        RespJson["Result"] = false;
        MyBasePtr->DEBUGLog("充值失败", true);
        RespJson["ErrorMsg"] = TempReq["ErrorMsg"];
        RespJson["ErrorMsg"].append("充值失败");
        MyBasePtr->INFO_Func("Recharge", false, RespJson);
        return;
    }
    RespJson["Result"] = true;
    MyBasePtr->DEBUGLog("充值成功", true);
    MyBasePtr->INFO_Func("Recharge", false, RespJson);
    return;
}

// 根据用户账号查找相关行为
/*
    根据用户账号查找相关行为的接口
Req:{
    "User_ID"       :   0,// 用户ID
    "Para"          :   {"User_ID":"","Login_Status":""} 执行者
}
Resp:{
    "ErrorMsg"  :   [],         // 失败返回的错误信息
    "Result"    :   true/false, // 操作结果
    "Action_List" : [], // 行为数据
}
*/
void MyDBService::Search_Action_By_UserID(Json::Value &ReqJson, Json::Value &RespJson)
{
    auto MyBasePtr = app().getPlugin<MyBase>();
    auto MyJsonPtr = app().getPlugin<MyJson>();
    MyBasePtr->INFO_Func("Search_Action_By_UserID", true, ReqJson);

    auto dbclientPrt = drogon::app().getDbClient();
    Mapper<drogon_model::novel::Action> ActionMgr(dbclientPrt);

    int UserID, NoteID, CommentID;
    Json::Value ParaJson;

    // 检查搜索数据是否合法
    try
    {
        MyBasePtr->DEBUGLog("开始检查数据是否合法", true);
        // "User_ID"       :   0,// 用户ID
        std::map<string, MyJson::ColType> ColMap;
        ColMap["User_ID"] = MyJson::ColType::INT;
        ColMap["Para"] = MyJson::ColType::JSON;
        MyJsonPtr->checkMemberAndTypeInMap(ReqJson, RespJson, ColMap);
        MyBasePtr->DEBUGLog("数据合法", true);
        MyBasePtr->DEBUGLog("开始检查Para数据是否合法", true);
        ColMap.clear();
        ParaJson = ReqJson["Para"];
        ColMap["User_ID"] = MyJson::ColType::STRING;
        ColMap["Login_Status"] = MyJson::ColType::STRING;
        MyJsonPtr->checkMemberAndTypeInMap(ParaJson, RespJson, ColMap);
        MyBasePtr->DEBUGLog("Para数据合法", true);

        MyBasePtr->DEBUGLog("开始检查操作权限", true);
        string LoginStatus = ParaJson["Login_Status"].asString();
        if (LoginStatus != "admin" && LoginStatus != "root" && atoi(ParaJson["User_ID"].asString().c_str()) != ReqJson["User_ID"].asInt())
        {
            RespJson["ErrorMsg"].append("权限不足，请联系管理员");
            throw RespJson;
        }
        UserID = ReqJson["User_ID"].asInt();
        MyBasePtr->DEBUGLog("操作权限检测通过", true);
    }
    catch (Json::Value &e)
    {
        RespJson["ErrorMsg"] = e["ErrorMsg"];
        RespJson["Result"] = false;
        MyBasePtr->INFO_Func("Search_Action_By_UserID", false, RespJson);
        return;
    }
    catch (...)
    {
        RespJson["Result"] = false;
        RespJson["ErrorMsg"].append("MyDBService::Search_Action_By_UserID::Error");
        MyBasePtr->INFO_Func("Search_Action_By_UserID", false, RespJson);
        return;
    }

    // 查询指定帖子
    try
    {
        // 制作筛选条件
        Criteria UserID_cri = Criteria(drogon_model::novel::Action::Cols::_User_ID, CompareOperator::EQ, UserID);
        
        MyBasePtr->DEBUGLog("开始查询指定用户行为信息", true);
        std::vector<drogon_model::novel::Action> vecAction = ActionMgr.findBy(UserID_cri);
        Json::Value ActionList,TempAction;
        for(auto &action : vecAction)
        {
            ParseAction(TempAction,action.toJson());
            
            ActionList.append(TempAction);
        }
        MyBasePtr->DEBUGLog("指定用户行为信息查询完毕", true);
        RespJson["Result"] = true;
        RespJson["Action_List"] = ActionList;
        MyBasePtr->INFO_Func("Search_Action_By_UserID", false, RespJson);
        return;
    }
    catch (...)
    {
        RespJson["Result"] = false;
        RespJson["ErrorMsg"].append("指定用户行为信息查询失败");
        MyBasePtr->INFO_Func("Search_Action_By_UserID", false, RespJson);
        return;
    }
}

// 搜索点赞
/*
    搜索点赞的接口
Req:{
    "User_ID"       :   0,// 用户ID
    "Note_ID"       :   0,// 帖子ID
    "Floor_ID"    :   0,// 楼层ID// 0 代表楼主即帖子的点赞, 其余表示评论的点赞
}
Resp:{
    "ErrorMsg"  :   [],         // 失败返回的错误信息
    "Result"    :   true/false, // 操作结果
    "Agree"     :   true/false, // 成功返回是否点赞
    "Agree_ID"  :   0,          // 点赞ID
// 如果Result假Agree假则表示程序错误
// 如果Result假Agree真则表示没有此数据，需要插入
// 如果Result真Agree假/真 表示用户点赞状态
}
*/
void MyDBService::Search_Agree(Json::Value &ReqJson, Json::Value &RespJson)
{
    auto MyBasePtr = app().getPlugin<MyBase>();
    auto MyJsonPtr = app().getPlugin<MyJson>();
    MyBasePtr->INFO_Func("Search_Agree", true, ReqJson);

    auto dbclientPrt = drogon::app().getDbClient();
    Mapper<drogon_model::novel::Agree> AgreeMgr(dbclientPrt);

    int UserID, NoteID, CommentID;

    // 检查搜索点赞的条件是否合法
    try
    {
        MyBasePtr->DEBUGLog("开始检查搜索点赞的条件是否合法", true);
        // "User_ID"       :   0,// 用户ID
        // "Note_ID"       :   0,// 帖子ID
        // "Floor_ID"    :   0,// 楼层ID// 0 代表楼主即帖子的点赞, 其余表示评论的点赞
        std::map<string, MyJson::ColType> ColMap;
        ColMap["User_ID"] = MyJson::ColType::INT;
        ColMap["Note_ID"] = MyJson::ColType::INT;
        ColMap["Floor_ID"] = MyJson::ColType::INT;
        MyJsonPtr->checkMemberAndTypeInMap(ReqJson, RespJson, ColMap);
        MyBasePtr->DEBUGLog("搜索点赞的条件合法", true);
    }
    catch (Json::Value &RespJson)
    {
        RespJson["Result"] = false;
        RespJson["Agree"] = false;
        MyBasePtr->INFO_Func("Search_Agree", false, RespJson);
        return;
    }
    catch (...)
    {
        RespJson["Result"] = false;
        RespJson["Agree"] = false;
        RespJson["ErrorMsg"].append("MyDBService::Search_Agree::Error");
        MyBasePtr->INFO_Func("Search_Agree", false, RespJson);
        return;
    }

    // 查询指定帖子
    try
    {
        // 制作筛选条件
        Criteria UserID_cri = Criteria(drogon_model::novel::Agree::Cols::_User_ID, CompareOperator::EQ, ReqJson["User_ID"].asInt());
        Criteria NoteID_cri = Criteria(drogon_model::novel::Agree::Cols::_Note_ID, CompareOperator::EQ, ReqJson["Note_ID"].asInt());
        Criteria FloorID_cri = Criteria(drogon_model::novel::Agree::Cols::_Floor_ID, CompareOperator::EQ, ReqJson["Floor_ID"].asInt());

        MyBasePtr->DEBUGLog("开始查询指定点赞信息", true);
        std::vector<drogon_model::novel::Agree> vecAgree = AgreeMgr.findBy(UserID_cri && NoteID_cri && FloorID_cri);
        if (vecAgree.size() == 0)
        {
            // 不存在此数据
            RespJson["Result"] = false;
            RespJson["Agree"] = true;
            MyBasePtr->INFO_Func("Search_Agree", false, RespJson);
            return;
        }
        else if (vecAgree.size() > 1)
        {
            // 程序错误
            RespJson["Result"] = false;
            RespJson["Agree"] = false;
            RespJson["ErrorMsg"].append("用户点赞信息不止一条,请联系管理员修复此错误");
            MyBasePtr->INFO_Func("Search_Agree", false, RespJson);
            return;
        }

        MyBasePtr->DEBUGLog("指定点赞信息查询完毕", true);
        RespJson["Result"] = true;
        RespJson["Agree"] = vecAgree[0].getValueOfStatus() == 0 ? false : true;
        RespJson["Agree_ID"] = vecAgree[0].getValueOfAgreeId();
        MyBasePtr->INFO_Func("Search_Agree", false, RespJson);
        return;
    }
    catch (...)
    {
        RespJson["Result"] = false;
        RespJson["Agree"] = false;
        RespJson["ErrorMsg"].append("帖子查询失败");
        MyBasePtr->INFO_Func("Search_Agree", false, RespJson);
        return;
    }
}

// 根据行为类型查询当天此行为的次数
/*
    根据用户账号查找相关行为的接口
Req:{
    "User_ID"       :   0,// 用户ID
    "Para"          :   {"User_ID":"","Login_Status":""} 执行者
}
Resp:{
    "ErrorMsg"  :   [],         // 失败返回的错误信息
    "Result"    :   true/false, // 操作结果
    "Action_Count" : {}, // 行为次数
}
*/
void MyDBService::Search_Resource_Action_Count(Json::Value &ReqJson, Json::Value &RespJson)
{
    auto MyBasePtr = app().getPlugin<MyBase>();
    auto MyJsonPtr = app().getPlugin<MyJson>();
    MyBasePtr->INFO_Func("Search_Action_Count", true, ReqJson);

    auto dbclientPrt = drogon::app().getDbClient();
    Mapper<drogon_model::novel::Action> ActionMgr(dbclientPrt);

    int UserID, NoteID, CommentID;
    Json::Value ParaJson;

    // 检查搜索数据是否合法
    try
    {
        MyBasePtr->DEBUGLog("开始检查数据是否合法", true);
        // "User_ID"       :   0,// 用户ID
        std::map<string, MyJson::ColType> ColMap;
        ColMap["User_ID"] = MyJson::ColType::INT;
        ColMap["Para"] = MyJson::ColType::JSON;
        MyJsonPtr->checkMemberAndTypeInMap(ReqJson, RespJson, ColMap);
        MyBasePtr->DEBUGLog("数据合法", true);
        MyBasePtr->DEBUGLog("开始检查Para数据是否合法", true);
        ColMap.clear();
        ParaJson = ReqJson["Para"];
        ColMap["User_ID"] = MyJson::ColType::STRING;
        ColMap["Login_Status"] = MyJson::ColType::STRING;
        MyJsonPtr->checkMemberAndTypeInMap(ParaJson, RespJson, ColMap);
        MyBasePtr->DEBUGLog("Para数据合法", true);

        MyBasePtr->DEBUGLog("开始检查操作权限", true);
        string LoginStatus = ParaJson["Login_Status"].asString();
        if (LoginStatus != "admin" && LoginStatus != "root" && atoi(ParaJson["User_ID"].asString().c_str()) != ReqJson["User_ID"].asInt())
        {
            RespJson["ErrorMsg"].append("权限不足，请联系管理员");
            throw RespJson;
        }
        UserID = ReqJson["User_ID"].asInt();
        MyBasePtr->DEBUGLog("操作权限检测通过", true);
    }
    catch (Json::Value &e)
    {
        RespJson["ErrorMsg"] = e["ErrorMsg"];
        RespJson["Result"] = false;
        MyBasePtr->INFO_Func("Search_Action_Count", false, RespJson);
        return;
    }
    catch (...)
    {
        RespJson["Result"] = false;
        RespJson["ErrorMsg"].append("MyDBService::Search_Action_Count::Error");
        MyBasePtr->INFO_Func("Search_Action_Count", false, RespJson);
        return;
    }

    // 查询指定帖子
    try
    {
        // 制作筛选条件
        Criteria UserID_cri = Criteria(drogon_model::novel::Action::Cols::_User_ID, CompareOperator::EQ, UserID);
        Criteria Type_cri = Criteria(drogon_model::novel::Action::Cols::_Type, CompareOperator::EQ, "Resource_Upload");

        trantor::Date Time = trantor::Date::now();
        string BeforeTime = Time.toCustomedFormattedStringLocal("%Y-%m-%d 23:59:59",false);//'%Y-%m-%d 23:59:59'
        string AfterTime = Time.toCustomedFormattedStringLocal("%Y-%m-%d 00:00:00",false);//'%Y-%m-%d 00:00:00'
        Criteria BeforeTime_cri = Criteria(drogon_model::novel::Action::Cols::_Time, CompareOperator::LE, BeforeTime);// <=
        Criteria AfterTime_cri = Criteria(drogon_model::novel::Action::Cols::_Time, CompareOperator::GE, AfterTime);// >=
        
        MyBasePtr->DEBUGLog("开始查询指定用户行为次数", true);
        vector<drogon_model::novel::Action> vecAction = ActionMgr.findBy(UserID_cri&&Type_cri&&BeforeTime_cri&&AfterTime_cri);
        Json::Value temp;
        string Type ;
        int old_book_new = 0,old_book_old=0,new_book=0,UploadLN=0;
        for(auto &var : vecAction)
        {
            temp.clear();
            temp = var.toJson();
            MyJsonPtr->JsonstrToJson(temp["Memo"],temp["Memo"].asString());
            Type = temp["Memo"]["Upload_Type"].asString();
            //cout << temp.toStyledString() << endl;
            if(Type == "old_book_new")
            {
                old_book_new++;
            }
            else if(Type == "old_book_old")
            {
                old_book_old++;
            }
            else if(Type == "new_book")
            {
                new_book++;
            }
            else if(Type == "UploadLN")
            {
                UploadLN++;
            }
        }
        MyBasePtr->DEBUGLog("指定用户行为次数查询完毕", true);
        RespJson["Result"] = true;
        RespJson["Action_Count"]["old_book_new"] = old_book_new;
        RespJson["Action_Count"]["old_book_old"] = old_book_old;
        RespJson["Action_Count"]["new_book"] = new_book;
        RespJson["Action_Count"]["UploadLN"] = UploadLN;
        MyBasePtr->INFO_Func("Search_Action_Count", false, RespJson);
        return;
    }
    catch (...)
    {
        RespJson["Result"] = false;
        RespJson["ErrorMsg"].append("指定用户行为次数查询失败");
        MyBasePtr->INFO_Func("Search_Action_Count", false, RespJson);
        return;
    }
}

// 搜索图书
/*
    搜索图书的接口
Req:{
    "Book_Name"  :   "",// 书名
    "Author"  :   "",// 作者名
    "Publisher"  :   "",// 出版方
    ["Limit"]         :   0, //
    ["Offset"]        :   0, //
}
Resp:{
    "ErrorMsg"  :   [],         // 失败返回的错误信息
    "Result"    :   true/false, // 操作结果
    "Book_List" :   [],         // 成功返回的图书信息
}
*/
void MyDBService::Search_Book(Json::Value &ReqJson, Json::Value &RespJson)
{

    auto MyBasePtr = app().getPlugin<MyBase>();
    auto MyJsonPtr = app().getPlugin<MyJson>();
    MyBasePtr->INFO_Func("Search_Book", true, ReqJson);

    auto dbclientPrt = drogon::app().getDbClient();
    Mapper<drogon_model::novel::Book> BookMgr(dbclientPrt);

    int limit = 0, offset = 0;
    string BookName = "", Author = "", Publisher = "";

    // 检查ReqJson数据是否合法
    try
    {
        MyBasePtr->DEBUGLog("开始检查ReqJson数据是否合法", true);
        // "Note_KeyWord"  :   "",// 关键字,在标题和内容中查找
        std::map<string, MyJson::ColType> ColMap;
        ColMap["Book_Name"] = MyJson::ColType::STRING;
        ColMap["Author"] = MyJson::ColType::STRING;
        ColMap["Publisher"] = MyJson::ColType::STRING;
        MyJsonPtr->checkMemberAndTypeInMap(ReqJson, RespJson, ColMap);
        MyBasePtr->DEBUGLog("ReqJson数据合法", true);
        BookName = ReqJson["Book_Name"].asString();
        Author = ReqJson["Author"].asString();
        Publisher = ReqJson["Publisher"].asString();
        if (ReqJson.isMember("Limit"))
        {
            MyJsonPtr->checkColType(ReqJson, RespJson, "Limit", MyJson::ColType::INT);
            limit = ReqJson["Limit"].asInt();
        }
        if (ReqJson.isMember("Offset"))
        {
            MyJsonPtr->checkColType(ReqJson, RespJson, "Offset", MyJson::ColType::INT);
            offset = ReqJson["Offset"].asInt();
        }
    }
    catch (Json::Value &RespJson)
    {
        RespJson["Result"] = false;
        MyBasePtr->INFO_Func("Search_Book", false, RespJson);
        return;
    }
    catch (...)
    {
        RespJson["Result"] = false;
        RespJson["ErrorMsg"].append("MyDBService::Search_Book::Error");
        MyBasePtr->INFO_Func("Search_Book", false, RespJson);
        return;
    }

    // 查询指定帖子
    try
    {   
        // 制作筛选条件
        Criteria BookName_cri = Criteria(drogon_model::novel::Book::Cols::_Book_Name, CompareOperator::Like, "%" + BookName + "%");
        Criteria Author_cri = Criteria(drogon_model::novel::Book::Cols::_Author, CompareOperator::Like, "%" + Author + "%");
        Criteria Publisher_cri = Criteria(drogon_model::novel::Book::Cols::_Publisher, CompareOperator::Like, "%" + Publisher + "%");

        if (offset > 0)
        {
            BookMgr.offset(offset);
        }

        if (limit > 0)
        {
            BookMgr.limit(limit);
        }

        MyBasePtr->DEBUGLog("开始查询指定图书", true);
        std::vector<drogon_model::novel::Book> vecBook = BookMgr.findBy(BookName_cri && Author_cri && Publisher_cri);
        Json::Value vecJsonVal, TempBookJson;
        for (auto &book : vecBook)
        {
            TempBookJson.clear();
            TempBookJson = book.toJson();
            MyJsonPtr->JsonstrToJson(TempBookJson["Memo"],TempBookJson["Memo"].asString());
            vecJsonVal.append(TempBookJson);
            // std::cout << user.toJson().toStyledString() <<std::endl;
        }
        MyBasePtr->DEBUGLog("查询指定图书完毕", true);
        RespJson["Result"] = true;
        RespJson["Book_List"] = vecJsonVal;
        MyBasePtr->INFO_Func("Search_Book", false, RespJson);
        return;
    }
    catch (...)
    {
        RespJson["Result"] = false;
        RespJson["ErrorMsg"].append("图书查询失败");
        MyBasePtr->INFO_Func("Search_Book", false, RespJson);
        return;
    }

}


// 搜索图书
/*
    搜索图书的接口
Req:{
    "Book_Name"  :   "",// 书名
    "Author"  :   "",// 作者名
    "Publisher"  :   "",// 出版方
    ["Limit"]         :   0, //
    ["Offset"]        :   0, //
}
Resp:{
    "ErrorMsg"  :   [],         // 失败返回的错误信息
    "Result"    :   true/false, // 操作结果
    "Book_List" :   [],         // 成功返回的图书信息
}
*/
void MyDBService::Search_Book_Accuracy(Json::Value &ReqJson, Json::Value &RespJson)
{

    auto MyBasePtr = app().getPlugin<MyBase>();
    auto MyJsonPtr = app().getPlugin<MyJson>();
    MyBasePtr->INFO_Func("Search_Book_Accuracy", true, ReqJson);

    auto dbclientPrt = drogon::app().getDbClient();
    Mapper<drogon_model::novel::Book> BookMgr(dbclientPrt);

    int limit = 0, offset = 0;
    string BookName = "", Author = "", Publisher = "";

    // 检查ReqJson数据是否合法
    try

    
    {
        MyBasePtr->DEBUGLog("开始检查ReqJson数据是否合法", true);
        // "Note_KeyWord"  :   "",// 关键字,在标题和内容中查找
        std::map<string, MyJson::ColType> ColMap;
        ColMap["Book_Name"] = MyJson::ColType::STRING;
        ColMap["Author"] = MyJson::ColType::STRING;
        ColMap["Publisher"] = MyJson::ColType::STRING;
        MyJsonPtr->checkMemberAndTypeInMap(ReqJson, RespJson, ColMap);
        MyBasePtr->DEBUGLog("ReqJson数据合法", true);
        BookName = ReqJson["Book_Name"].asString();
        Author = ReqJson["Author"].asString();
        Publisher = ReqJson["Publisher"].asString();
        if (ReqJson.isMember("Limit"))
        {
            MyJsonPtr->checkColType(ReqJson, RespJson, "Limit", MyJson::ColType::INT);
            limit = ReqJson["Limit"].asInt();
        }
        if (ReqJson.isMember("Offset"))
        {
            MyJsonPtr->checkColType(ReqJson, RespJson, "Offset", MyJson::ColType::INT);
            offset = ReqJson["Offset"].asInt();
        }
    }
    catch (Json::Value &RespJson)
    {
        RespJson["Result"] = false;
        MyBasePtr->INFO_Func("Search_Book_Accuracy", false, RespJson);
        return;
    }
    catch (...)
    {
        RespJson["Result"] = false;
        RespJson["ErrorMsg"].append("MyDBService::Search_Book_Accuracy::Error");
        MyBasePtr->INFO_Func("Search_Book_Accuracy", false, RespJson);
        return;
    }

    // 查询指定帖子
    try
    {   
        // 制作筛选条件
        Criteria BookName_cri = Criteria(drogon_model::novel::Book::Cols::_Book_Name, CompareOperator::EQ, BookName);
        Criteria Author_cri = Criteria(drogon_model::novel::Book::Cols::_Author, CompareOperator::Like, "%" + Author + "%");
        Criteria Publisher_cri = Criteria(drogon_model::novel::Book::Cols::_Publisher, CompareOperator::Like, "%" + Publisher + "%");

        if (offset > 0)
        {
            BookMgr.offset(offset);
        }

        if (limit > 0)
        {
            BookMgr.limit(limit);
        }

        MyBasePtr->DEBUGLog("开始查询指定图书", true);
        std::vector<drogon_model::novel::Book> vecBook = BookMgr.findBy(BookName_cri && Author_cri && Publisher_cri);
        Json::Value vecJsonVal, TempBookJson;
        for (auto &book : vecBook)
        {
            TempBookJson.clear();
            TempBookJson = book.toJson();
            MyJsonPtr->JsonstrToJson(TempBookJson["Memo"],TempBookJson["Memo"].asString());
            vecJsonVal.append(TempBookJson);
            // std::cout << user.toJson().toStyledString() <<std::endl;
        }
        MyBasePtr->DEBUGLog("查询指定图书完毕", true);
        RespJson["Result"] = true;
        RespJson["Book_List"] = vecJsonVal;
        MyBasePtr->INFO_Func("Search_Book_Accuracy", false, RespJson);
        return;
    }
    catch (...)
    {
        RespJson["Result"] = false;
        RespJson["ErrorMsg"].append("图书查询失败");
        MyBasePtr->INFO_Func("Search_Book_Accuracy", false, RespJson);
        return;
    }

}

// 通过BookID查询图书数据
/*
    通过BookID查询图书数据
Req:{
    "Book_ID":0//图书编号
}
Resp:{
    "ErrorMsg":[],
    "Result" : true/false
    "Book_Data":{}
}
*/
void MyDBService::Search_Book_By_BookID(Json::Value &ReqJson, Json::Value &RespJson)
{
    auto MyBasePtr = app().getPlugin<MyBase>();
    auto MyJsonPtr = app().getPlugin<MyJson>();
    MyBasePtr->INFO_Func("Search_Book_By_BookID", true, ReqJson);

    auto dbclientPrt = drogon::app().getDbClient();
    Mapper<drogon_model::novel::Book> BookMgr(dbclientPrt);

    // 检查ReqJson数据是否合法
    try
    {
        MyBasePtr->DEBUGLog("开始检查ReqJson数据是否合法", true);
        // "Book_ID":0,//图书编号
        std::map<string, MyJson::ColType> ColMap;
        ColMap["Book_ID"] = MyJson::ColType::INT;
        MyJsonPtr->checkMemberAndTypeInMap(ReqJson, RespJson, ColMap);
        MyBasePtr->DEBUGLog("ReqJson数据合法", true);
    }
    catch (Json::Value &e)
    {
        RespJson["ErrorMsg"] = e["ErrorMsg"];
        RespJson["ErrorMsg"].append("ReqJson数据不合法");
        RespJson["Result"] = false;
        MyBasePtr->INFO_Func("Search_Book_By_BookID", false, RespJson);
        return;
    }

    // 查询图书
    try
    {
        MyBasePtr->DEBUGLog("开始查询图书", true);
        drogon_model::novel::Book TBook = BookMgr.findByPrimaryKey(ReqJson["Book_ID"].asInt());
        MyBasePtr->DEBUGLog("查询图书完毕", true);
        RespJson["Result"] = true;
        RespJson["Book_Data"] = TBook.toJson();
        MyBasePtr->INFO_Func("Search_Book_By_BookID", false, RespJson);
        return;
    }
    catch (const drogon::orm::DrogonDbException &e)
    {

        if (e.base().what() == string("0 rows found"))
        {
            RespJson["ErrorMsg"].append("此图书不存在");
        }
        else if (e.base().what() == string("Found more than one row"))
        {
            RespJson["ErrorMsg"].append("图书ID重复,请联系管理员");
        }
        else
        {
            RespJson["ErrorMsg"].append(e.base().what());
        }
        RespJson["Result"] = false;
        RespJson["ErrorMsg"].append("图书查询失败");
        MyBasePtr->INFO_Func("Search_Note", false, RespJson);
        return;
    }
    catch (...)
    {
        RespJson["Result"] = false;
        RespJson["ErrorMsg"].append("图书查询失败");
        MyBasePtr->INFO_Func("Search_Note", false, RespJson);
        return;
    }
}



// 通过BookID BookName查询图书章节目录数据
/*
    通过BookID查询图书数据
Req:{
    "Book_ID":0//图书编号
    "Book_Name":""//图书名称
}
Resp:{
    "ErrorMsg":[],
    "Result" : true/false,
    "Chapter_List":[],
    "Book_Data":{}
}
*/
void MyDBService::Search_BookMenu_By_BookIDAndName(Json::Value &ReqJson, Json::Value &RespJson)
{

    auto MyBasePtr = app().getPlugin<MyBase>();
    auto MyJsonPtr = app().getPlugin<MyJson>();
    MyBasePtr->INFO_Func("Search_BookMenu_By_BookIDAndName", true, ReqJson);

    auto dbclientPrt = drogon::app().getDbClient();
    Mapper<drogon_model::novel::Chapter> ChapterMgr(dbclientPrt);

    // 检查ReqJson数据是否合法
    try
    {
        MyBasePtr->DEBUGLog("开始检查ReqJson数据是否合法", true);
        // "Book_ID":0//图书编号
        // "Book_Name":""//图书名称
        std::map<string, MyJson::ColType> ColMap;
        ColMap["Book_ID"] = MyJson::ColType::INT;
        ColMap["Book_Name"] = MyJson::ColType::STRING;
        MyJsonPtr->checkMemberAndTypeInMap(ReqJson, RespJson, ColMap);
        MyBasePtr->DEBUGLog("ReqJson数据合法", true);
    }
    catch (Json::Value &e)
    {
        RespJson["ErrorMsg"] = e["ErrorMsg"];
        RespJson["ErrorMsg"].append("ReqJson数据不合法");
        RespJson["Result"] = false;
        MyBasePtr->INFO_Func("Search_BookMenu_By_BookIDAndName", false, RespJson);
        return;
    }

    // 检查图书是否存在
    Search_Book_By_BookID(ReqJson,RespJson);
    if(!RespJson["Result"].asBool())
    {
        RespJson["ErrorMsg"].append("图书ID指向的图书不存在");
        RespJson["Result"] = false;
        MyBasePtr->INFO_Func("Search_BookMenu_By_BookIDAndName", false, RespJson);
        return;
    }

    drogon_model::novel::Book book;
    book.updateByJson(RespJson["Book_Data"]);
    if(book.getValueOfBookName() != ReqJson["Book_Name"].asString())
    {
        RespJson["ErrorMsg"].append("图书ID指向的图书与目标图书名称不符合");
        RespJson["Result"] = false;
        MyBasePtr->INFO_Func("Search_BookMenu_By_BookIDAndName", false, RespJson);
        return;
    }

    // 开始查询章节
    Criteria Valid_cri(drogon_model::novel::Chapter::Cols::_Valid,CompareOperator::EQ,true);
    Criteria BookID_cri(drogon_model::novel::Chapter::Cols::_Book_ID,CompareOperator::EQ,ReqJson["Book_ID"].asInt());
    ChapterMgr.orderBy(drogon_model::novel::Chapter::Cols::_Part_Num,SortOrder::ASC);
    ChapterMgr.orderBy(drogon_model::novel::Chapter::Cols::_Chapter_Num,SortOrder::ASC);
    vector<drogon_model::novel::Chapter> verChapter = ChapterMgr.findBy(BookID_cri && Valid_cri);

    Json::Value ChapterList,ChapterContent;
    for(auto chapter : verChapter)
    {
        ChapterContent.clear();
        ChapterContent["VolNum"] = chapter.getValueOfPartNum();
        ChapterContent["ChapterNum"] = chapter.getValueOfChapterNum();
        ChapterContent["ChapterTitle"] = chapter.getValueOfTitle();
        ChapterList.append(ChapterContent);
    }
    RespJson["Chapter_List"] = ChapterList;
    RespJson["Book_Data"] = book.toJson();
    RespJson["Result"] = true;
    MyBasePtr->INFO_Func("Search_BookMenu_By_BookIDAndName", false, RespJson);
    return;
}


// 通过Note_ID查询帖子下的评论
/*
    通过Note_ID查询帖子下的评论
Req:{
    "Note_ID":0//帖子编号
}
Resp:{
    "ErrorMsg":[],
    "Result" : true/false
    "Comment_List":[]
}
*/
void MyDBService::Search_Comment(Json::Value &ReqJson, Json::Value &RespJson)
{
    auto MyBasePtr = app().getPlugin<MyBase>();
    auto MyJsonPtr = app().getPlugin<MyJson>();
    MyBasePtr->INFO_Func("Search_Comment", true, ReqJson);

    auto dbclientPrt = drogon::app().getDbClient();
    Mapper<drogon_model::novel::Comment> CommentMgr(dbclientPrt);
    Mapper<drogon_model::novel::Agree> AgreeMgr(dbclientPrt);

    // 检查ReqJson数据是否合法
    try
    {
        MyBasePtr->DEBUGLog("开始检查ReqJson数据是否合法", true);
        // "Note_ID":0,//帖子编号
        std::map<string, MyJson::ColType> ColMap;
        ColMap["Note_ID"] = MyJson::ColType::INT;
        MyJsonPtr->checkMemberAndTypeInMap(ReqJson, RespJson, ColMap);
        MyBasePtr->DEBUGLog("ReqJson数据合法", true);
    }
    catch (Json::Value &e)
    {
        RespJson["ErrorMsg"] = e["ErrorMsg"];
        RespJson["ErrorMsg"].append("ReqJson数据不合法");
        RespJson["Result"] = false;
        MyBasePtr->INFO_Func("Search_Comment", false, RespJson);
        return;
    }

    // 查询评论
    try
    {
        Search_Note_By_NoteID(ReqJson,RespJson);
        if(!RespJson["Result"].asBool())
        {
            RespJson["ErrorMsg"] = "要查询评论的帖子不存在";
        }
        MyBasePtr->DEBUGLog("开始查询评论", true);
        Criteria NoteID_cri(drogon_model::novel::Comment::Cols::_Note_ID,CompareOperator::EQ,ReqJson["Note_ID"].asInt());
        Criteria AgreeNoteID_cri(drogon_model::novel::Agree::Cols::_Note_ID,CompareOperator::EQ,ReqJson["Note_ID"].asInt());
        Criteria AgreeStatus_cri(drogon_model::novel::Agree::Cols::_Status,CompareOperator::EQ,true);
        vector<drogon_model::novel::Comment> vecComment = CommentMgr.findBy(NoteID_cri);
        MyBasePtr->DEBUGLog("查询评论完毕", true);
        Json::Value tempJson;
        for(int i = 0 ; i < vecComment.size() ; i++)
        {
            tempJson.clear();
            tempJson = vecComment[i].toJson();
            MyJsonPtr->JsonstrToJson(tempJson["Comment_Content"],tempJson["Comment_Content"].asString());
            tempJson["Agree_Num"] = (int)AgreeMgr.count(AgreeNoteID_cri && AgreeStatus_cri && Criteria(drogon_model::novel::Agree::Cols::_Floor_ID,CompareOperator::EQ,vecComment[i].getValueOfFloorId()));
            RespJson["Comment_List"].append(tempJson);
        }
        RespJson["Result"] = true;
        MyBasePtr->INFO_Func("Search_Comment", false, RespJson);
        return;
    }
    catch(Json::Value &e)
    {
        RespJson["Result"] = false;
        RespJson["ErrorMsg"] = e["ErrorMsg"];
        RespJson["ErrorMsg"].append("评论查询失败");
        MyBasePtr->INFO_Func("Search_Comment", false, RespJson);
        return;
    }
    catch (...)
    {
        RespJson["Result"] = false;
        RespJson["ErrorMsg"].append("评论查询失败");
        RespJson["ErrorMsg"].append("Search_Comment::Error");
        MyBasePtr->INFO_Func("Search_Comment", false, RespJson);
        return;
    }
}


// 通过Comment_ID查询帖子下的评论
/*
    通过Comment_ID查询帖子下的评论
Req:{
    "Comment_ID":0//帖子编号
}
Resp:{
    "ErrorMsg":[],
    "Result" : true/false
    "Comment_Data":{}
}
*/
void MyDBService::Search_Comment_By_CommentID(Json::Value &ReqJson, Json::Value &RespJson)
{
////----
}

// 搜索章节所有版本
/*
    搜索章节所有版本的接口
Req:{
    "Book_ID":0,
    "Part_Num"0,
    "Chapter_Num":0
}
Resp:{
    "ErrorMsg"  :   [],         // 失败返回的错误信息
    "Result"    :   true/false, // 操作结果
    "Chapter_List" :   [],         // 成功返回的章节信息
}
*/
void MyDBService::Search_Chapter_All_Version(Json::Value &ReqJson, Json::Value &RespJson)
{

    auto MyBasePtr = app().getPlugin<MyBase>();
    auto MyJsonPtr = app().getPlugin<MyJson>();
    MyBasePtr->INFO_Func("Search_Chapter_All_Version", true, ReqJson);

    auto dbclientPrt = drogon::app().getDbClient();
    Mapper<drogon_model::novel::Chapter> ChapterMgr(dbclientPrt);
    Mapper<drogon_model::novel::Book> BookMgr(dbclientPrt);

    int limit = 0, offset = 0, BookID,PartNum,ChapterNum;

    // 检查ReqJson数据是否合法
    try
    {
        MyBasePtr->DEBUGLog("开始检查ReqJson数据是否合法", true);
        std::map<string, MyJson::ColType> ColMap;
        ColMap["Book_ID"] = MyJson::ColType::INT;
        ColMap["Part_Num"] = MyJson::ColType::INT;
        ColMap["Chapter_Num"] = MyJson::ColType::INT;
        MyJsonPtr->checkMemberAndTypeInMap(ReqJson, RespJson, ColMap);
        MyBasePtr->DEBUGLog("ReqJson数据合法", true);
        BookID = atoi(ReqJson["Book_ID"].asString().c_str());
        PartNum = atoi(ReqJson["Part_Num"].asString().c_str());
        ChapterNum = atoi(ReqJson["Chapter_Num"].asString().c_str());
        if (ReqJson.isMember("Limit"))
        {
            MyJsonPtr->checkColType(ReqJson, RespJson, "Limit", MyJson::ColType::INT);
            limit = ReqJson["Limit"].asInt();
        }
        if (ReqJson.isMember("Offset"))
        {
            MyJsonPtr->checkColType(ReqJson, RespJson, "Offset", MyJson::ColType::INT);
            offset = ReqJson["Offset"].asInt();
        }

    }
    catch (Json::Value &RespJson)
    {
        RespJson["Result"] = false;
        MyBasePtr->INFO_Func("Search_Chapter_All_Version", false, RespJson);
        return;
    }
    catch (...)
    {
        RespJson["Result"] = false;
        RespJson["ErrorMsg"].append("MyDBService::Search_Chapter_All_Version::Error");
        MyBasePtr->INFO_Func("Search_Chapter_All_Version", false, RespJson);
        return;
    }


    // 判断图书是否存在
    try
    {
        // 检查图书是否已存在
        MyBasePtr->DEBUGLog("开始检查图书是否已存在", true);
        Criteria BookID_cri(drogon_model::novel::Book::Cols::_Book_ID, CompareOperator::EQ, BookID);
        vector<drogon_model::novel::Book> vecBook = BookMgr.findBy(BookID_cri);

        // 如果不是新书，则将Upload状态改为已处理且拒绝插入
        if (vecBook.size() == 0)
        {
            RespJson["ErrorMsg"].append("图书不存在");
            RespJson["Result"] = false;
            MyBasePtr->INFO_Func("Search_Chapter_All_Version", false, RespJson);
            return;
        }
        MyBasePtr->DEBUGLog("图书存在", true);
    }
    catch (Json::Value &e)
    {
        RespJson["ErrorMsg"] = e["ErrorMsg"];
        RespJson["ErrorMsg"].append("图书不存在");
        RespJson["Result"] = false;
        MyBasePtr->INFO_Func("Search_Chapter_All_Version", false, RespJson);
        return;
    }
    catch (const drogon::orm::DrogonDbException &e)
    {
        RespJson["ErrorMsg"].append(e.base().what());
        RespJson["ErrorMsg"].append("图书不存在");
        RespJson["Result"] = false;
        MyBasePtr->INFO_Func("Search_Chapter_All_Version", false, RespJson);
        return;
    }



    // 查询指定图书章节
    try
    {
        // 制作筛选条件
        Criteria BookID_cri = Criteria(drogon_model::novel::Chapter::Cols::_Book_ID, CompareOperator::EQ, BookID);
        Criteria PartNum_cri = Criteria(drogon_model::novel::Chapter::Cols::_Part_Num, CompareOperator::EQ, PartNum);
        Criteria ChapterNum_cri = Criteria(drogon_model::novel::Chapter::Cols::_Chapter_Num, CompareOperator::EQ, ChapterNum);
        ChapterMgr.orderBy(drogon_model::novel::Chapter::Cols::_Version,SortOrder::ASC);
        if (offset > 0)
        {
            ChapterMgr.offset(offset);
        }

        if (limit > 0)
        {
            ChapterMgr.limit(limit);
        }

        MyBasePtr->DEBUGLog("开始查询指定图书的章节的所有版本", true);
        std::vector<drogon_model::novel::Chapter> vecChapter = ChapterMgr.findBy(BookID_cri && PartNum_cri && ChapterNum_cri);
        Json::Value vecJsonVal, TempChapterJson;
        for (auto &chapter : vecChapter)
        {
            TempChapterJson.clear();
            TempChapterJson = chapter.toJson();
            vecJsonVal.append(TempChapterJson);
            // std::cout << user.toJson().toStyledString() <<std::endl;
        }
        MyBasePtr->DEBUGLog("查询指定图书的章节的所有版本完毕", true);
        RespJson["Result"] = true;
        RespJson["Chapter_List"] = vecJsonVal;
        MyBasePtr->INFO_Func("Search_Chapter_All_Version", false, RespJson);
        return;
    }
    catch (...)
    {
        RespJson["Result"] = false;
        RespJson["ErrorMsg"].append("指定图书的章节的所有版本查询失败");
        MyBasePtr->INFO_Func("Search_Chapter_All_Version", false, RespJson);
        return;
    }
}


// 根据图书ID搜索章节
/*
    根据图书ID搜索章节的接口
Req:{
    "Book_ID":0
}
Resp:{
    "ErrorMsg"  :   [],         // 失败返回的错误信息
    "Result"    :   true/false, // 操作结果
    "Chapter_List" :   [],         // 成功返回的章节信息
}
*/
void MyDBService::Search_Chapter_By_BookID(Json::Value &ReqJson, Json::Value &RespJson)
{

    auto MyBasePtr = app().getPlugin<MyBase>();
    auto MyJsonPtr = app().getPlugin<MyJson>();
    MyBasePtr->INFO_Func("Search_Chapter_By_BookID", true, ReqJson);

    auto dbclientPrt = drogon::app().getDbClient();
    Mapper<drogon_model::novel::Chapter> ChapterMgr(dbclientPrt);
    Mapper<drogon_model::novel::Book> BookMgr(dbclientPrt);

    int limit = 0, offset = 0, BookID;

    // 检查ReqJson数据是否合法
    try
    {
        MyBasePtr->DEBUGLog("开始检查ReqJson数据是否合法", true);
        std::map<string, MyJson::ColType> ColMap;
        ColMap["Book_ID"] = MyJson::ColType::INT;
        MyJsonPtr->checkMemberAndTypeInMap(ReqJson, RespJson, ColMap);
        MyBasePtr->DEBUGLog("ReqJson数据合法", true);
        BookID = atoi(ReqJson["Book_ID"].asString().c_str());
        if (ReqJson.isMember("Limit"))
        {
            MyJsonPtr->checkColType(ReqJson, RespJson, "Limit", MyJson::ColType::INT);
            limit = ReqJson["Limit"].asInt();
        }
        if (ReqJson.isMember("Offset"))
        {
            MyJsonPtr->checkColType(ReqJson, RespJson, "Offset", MyJson::ColType::INT);
            offset = ReqJson["Offset"].asInt();
        }

    }
    catch (Json::Value &RespJson)
    {
        RespJson["Result"] = false;
        MyBasePtr->INFO_Func("Search_Chapter_By_BookID", false, RespJson);
        return;
    }
    catch (...)
    {
        RespJson["Result"] = false;
        RespJson["ErrorMsg"].append("MyDBService::Search_Chapter_By_BookID::Error");
        MyBasePtr->INFO_Func("Search_Chapter_By_BookID", false, RespJson);
        return;
    }


    // 判断图书是否存在
    Json::Value SearchBookReq,SearchBookResp;
    SearchBookReq["Book_ID"] = ReqJson["Book_ID"];
    Search_Book_By_BookID(SearchBookReq,SearchBookResp);
    if(!SearchBookResp["Result"].asBool())
    {
        RespJson["ErrorMsg"] = SearchBookResp["ErrorMsg"];
        RespJson["ErrorMsg"].append("图书不存在");
        RespJson["Result"] = false;
        MyBasePtr->INFO_Func("Search_Chapter_By_BookID", false, RespJson);
        return;
    }

    // 查询指定图书章节
    try
    {
        // 制作筛选条件
        Criteria BookID_cri = Criteria(drogon_model::novel::Chapter::Cols::_Book_ID, CompareOperator::EQ, BookID);
        Criteria Valid_cri = Criteria(drogon_model::novel::Chapter::Cols::_Valid, CompareOperator::EQ, true);
        ChapterMgr.orderBy(drogon_model::novel::Chapter::Cols::_Part_Num,SortOrder::ASC);
        ChapterMgr.orderBy(drogon_model::novel::Chapter::Cols::_Chapter_Num,SortOrder::ASC);
        if (offset > 0)
        {
            ChapterMgr.offset(offset);
        }

        if (limit > 0)
        {
            ChapterMgr.limit(limit);
        }

        MyBasePtr->DEBUGLog("开始查询指定图书的章节", true);
        std::vector<drogon_model::novel::Chapter> vecChapter = ChapterMgr.findBy(BookID_cri && Valid_cri);
        Json::Value vecJsonVal, TempChapterJson;
        for (auto &chapter : vecChapter)
        {
            TempChapterJson.clear();
            TempChapterJson = chapter.toJson();
            vecJsonVal.append(TempChapterJson);
            // std::cout << user.toJson().toStyledString() <<std::endl;
        }
        MyBasePtr->DEBUGLog("查询指定图书的章节完毕", true);
        RespJson["Result"] = true;
        RespJson["Chapter_List"] = vecJsonVal;
        MyBasePtr->INFO_Func("Search_Chapter_By_BookID", false, RespJson);
        return;
    }
    catch (...)
    {
        RespJson["Result"] = false;
        RespJson["ErrorMsg"].append("指定图书的章节查询失败");
        MyBasePtr->INFO_Func("Search_Chapter_By_BookID", false, RespJson);
        return;
    }
}


// 根据章节ID查询章节
/*
    根据章节ID查询章节
Req:{
    "Chapter_ID":0
}
Resp:{
    "ErrorMsg"  :   [],         // 失败返回的错误信息
    "Result"    :   true/false, // 操作结果
    "Chapter_Data" : {},         // 成功返回的章节信息
}
*/
void MyDBService::Search_Chapter_By_ChapterID(Json::Value &ReqJson, Json::Value &RespJson)
{

    auto MyBasePtr = app().getPlugin<MyBase>();
    auto MyJsonPtr = app().getPlugin<MyJson>();
    MyBasePtr->INFO_Func("Search_Chapter_By_ChapterID", true, ReqJson);

    auto dbclientPrt = drogon::app().getDbClient();
    Mapper<drogon_model::novel::Chapter> ChapterMgr(dbclientPrt);
    
    // 检查ReqJson数据是否合法
    try
    {
        MyBasePtr->DEBUGLog("开始检查ReqJson数据是否合法", true);
        std::map<string, MyJson::ColType> ColMap;
        ColMap["Chapter_ID"] = MyJson::ColType::INT;
        MyJsonPtr->checkMemberAndTypeInMap(ReqJson, RespJson, ColMap);
        MyBasePtr->DEBUGLog("ReqJson数据合法", true);
    }
    catch (Json::Value &RespJson)
    {
        RespJson["Result"] = false;
        MyBasePtr->INFO_Func("Search_Chapter_By_ChapterID", false, RespJson);
        return;
    }
    catch (...)
    {
        RespJson["Result"] = false;
        RespJson["ErrorMsg"].append("MyDBService::Search_Chapter_By_ChapterID::Error");
        MyBasePtr->INFO_Func("Search_Chapter_By_ChapterID", false, RespJson);
        return;
    }

    // 查询指定图书章节
    try
    {
        // 制作筛选条件
        Criteria ChapterID_cri = Criteria(drogon_model::novel::Chapter::Cols::_Chapter_ID, CompareOperator::EQ, ReqJson["Chapter_ID"].asInt());

        MyBasePtr->DEBUGLog("开始查询指定ID的章节", true);
        std::vector<drogon_model::novel::Chapter> vecChapter = ChapterMgr.findBy(ChapterID_cri);
        
        MyBasePtr->DEBUGLog("查询指定图书的章节完毕", true);
        if(vecChapter.size() == 0)
        {
            RespJson["ErrorMsg"].append("章节ID不存在");
            throw RespJson;
        }
        else if(vecChapter.size() > 1)
        {
            RespJson["ErrorMsg"].append("数据异常,章节ID不唯一");
            throw RespJson;
        }
        RespJson["Result"] = true;
        RespJson["Chapter_Data"] = vecChapter[0].toJson();
        MyBasePtr->INFO_Func("Search_Chapter_By_ChapterID", false, RespJson);
        return;
    }
    catch (Json::Value &e)
    {
        int size = e["ErrorMsg"].size();
        RespJson["Result"] = false;
        RespJson["ErrorMsg"].append("章节数据异常("+e["ErrorMsg"][size - 1].asString()+")");
        MyBasePtr->INFO_Func("Search_Chapter_By_ChapterID", false, RespJson);
        return;
    }
    catch (...)
    {
        RespJson["Result"] = false;
        RespJson["ErrorMsg"].append("指定图书的章节查询失败");
        MyBasePtr->INFO_Func("Search_Chapter_By_ChapterID", false, RespJson);
        return;
    }
}


// 根据图书ID 分卷数 章节数 查询章节内容
/*
    根据图书ID搜索章节的接口
Req:{
    "Book_ID":0,
    "Part_Num":0,
    "Chapter_Num":0
}
Resp:{
    "ErrorMsg"  :   [],         // 失败返回的错误信息
    "Result"    :   true/false, // 操作结果
    "Chapter_Content" :   [],         // 成功返回的章节内容信息
    "Chapter_ID":0,//章节ID
}
*/
void MyDBService::Search_ChapterContent(Json::Value &ReqJson, Json::Value &RespJson)
{

    auto MyBasePtr = app().getPlugin<MyBase>();
    auto MyJsonPtr = app().getPlugin<MyJson>();
    MyBasePtr->INFO_Func("Search_ChapterContent", true, ReqJson);

    auto dbclientPrt = drogon::app().getDbClient();
    Mapper<drogon_model::novel::Chapter> ChapterMgr(dbclientPrt);
    Mapper<drogon_model::novel::Book> BookMgr(dbclientPrt);

    int BookID,PartNum,ChapterNum;

    // 检查ReqJson数据是否合法
    try
    {
        MyBasePtr->DEBUGLog("开始检查ReqJson数据是否合法", true);
        std::map<string, MyJson::ColType> ColMap;
        ColMap["Book_ID"] = MyJson::ColType::INT;
        ColMap["Part_Num"] = MyJson::ColType::INT;
        ColMap["Chapter_Num"] = MyJson::ColType::INT;
        MyJsonPtr->checkMemberAndTypeInMap(ReqJson, RespJson, ColMap);
        MyBasePtr->DEBUGLog("ReqJson数据合法", true);
        BookID = atoi(ReqJson["Book_ID"].asString().c_str());
        PartNum = atoi(ReqJson["Part_Num"].asString().c_str());
        ChapterNum = atoi(ReqJson["Chapter_Num"].asString().c_str());
    }
    catch (Json::Value &RespJson)
    {
        RespJson["Result"] = false;
        MyBasePtr->INFO_Func("Search_ChapterContent", false, RespJson);
        return;
    }
    catch (...)
    {
        RespJson["Result"] = false;
        RespJson["ErrorMsg"].append("MyDBService::Search_ChapterContent::Error");
        MyBasePtr->INFO_Func("Search_ChapterContent", false, RespJson);
        return;
    }


    // 判断图书是否存在
    Json::Value SearchBookReq,SearchBookResp;
    SearchBookReq["Book_ID"] = ReqJson["Book_ID"];
    Search_Book_By_BookID(SearchBookReq,SearchBookResp);
    if(!SearchBookResp["Result"].asBool())
    {
        RespJson["ErrorMsg"] = SearchBookResp["ErrorMsg"];
        RespJson["ErrorMsg"].append("图书不存在");
        RespJson["Result"] = false;
        MyBasePtr->INFO_Func("Search_ChapterContent", false, RespJson);
        return;
    }

    // 查询指定图书章节
    try
    {
        // 制作筛选条件
        Criteria BookID_cri = Criteria(drogon_model::novel::Chapter::Cols::_Book_ID, CompareOperator::EQ, BookID);
        Criteria PartNum_cri = Criteria(drogon_model::novel::Chapter::Cols::_Part_Num, CompareOperator::EQ, PartNum);
        Criteria ChapterNum_cri = Criteria(drogon_model::novel::Chapter::Cols::_Chapter_Num, CompareOperator::EQ, ChapterNum);
        Criteria Valid_cri = Criteria(drogon_model::novel::Chapter::Cols::_Valid, CompareOperator::EQ, true);
 
        MyBasePtr->DEBUGLog("开始查询指定图书章节的内容", true);
        std::vector<drogon_model::novel::Chapter> vecChapter = ChapterMgr.findBy(BookID_cri && PartNum_cri && ChapterNum_cri && Valid_cri);
        Json::Value ChapterContentJson;
        if(vecChapter.size() < 1)
        {
            RespJson["ErrorMsg"] = SearchBookResp["ErrorMsg"];
            RespJson["ErrorMsg"].append("章节不存在");
            RespJson["Result"] = false;
            MyBasePtr->INFO_Func("Search_ChapterContent", false, RespJson);
            return;
        }
        if(vecChapter.size() > 1)
        {
            RespJson["ErrorMsg"] = SearchBookResp["ErrorMsg"];
            RespJson["ErrorMsg"].append("章节不唯一");
            RespJson["Result"] = false;
            MyBasePtr->INFO_Func("Search_ChapterContent", false, RespJson);
            return;
        }

        MyBasePtr->DEBUGLog("查询指定图书章节的内容完毕", true);
        RespJson["Result"] = true;
        MyJsonPtr->JsonstrToJson(ChapterContentJson,vecChapter[0].getValueOfContent());
        RespJson["Chapter_Content"] = ChapterContentJson;
        RespJson["Chapter_ID"] = vecChapter[0].getValueOfChapterId();
        MyBasePtr->INFO_Func("Search_ChapterContent", false, RespJson);
        return;
    }
    catch (...)
    {
        RespJson["Result"] = false;
        RespJson["ErrorMsg"].append("指定图书章节的内容查询失败");
        MyBasePtr->INFO_Func("Search_ChapterContent", false, RespJson);
        return;
    }
}

// 搜索帖子
/*
    搜索帖子的接口
Req:{
    "Note_KeyWord"  :   "",// 关键字,在标题和内容中查找
    ["User_ID"]       :   0,// 创建者ID，保存第一个创建图书者的ID
    ["Note_Type"]     :   "",// 帖子类型，(Help/Resource)
    ["Limit"]         :   0, //
    ["Offset"]        :   0, //
}
Resp:{
    "ErrorMsg"  :   [],         // 失败返回的错误信息
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

    int limit = 0, offset = 0;
    string KeyWord = "", NoteType = "", UserID = "";

    // 检查ReqJson数据是否合法
    try
    {
        MyBasePtr->DEBUGLog("开始检查ReqJson数据是否合法", true);
        // "Note_KeyWord"  :   "",// 关键字,在标题和内容中查找
        std::map<string, MyJson::ColType> ColMap;
        ColMap["Note_KeyWord"] = MyJson::ColType::STRING;
        MyJsonPtr->checkMemberAndTypeInMap(ReqJson, RespJson, ColMap);
        MyBasePtr->DEBUGLog("ReqJson数据合法", true);
        KeyWord = ReqJson["Note_KeyWord"].asString();
        if (ReqJson.isMember("Limit"))
        {
            MyJsonPtr->checkColType(ReqJson, RespJson, "Limit", MyJson::ColType::INT);
            limit = ReqJson["Limit"].asInt();
        }
        if (ReqJson.isMember("Offset"))
        {
            MyJsonPtr->checkColType(ReqJson, RespJson, "Offset", MyJson::ColType::INT);
            offset = ReqJson["Offset"].asInt();
        }
        if (ReqJson.isMember("User_ID"))
        {
            MyJsonPtr->checkColType(ReqJson, RespJson, "User_ID", MyJson::ColType::INT);
            UserID = ReqJson["User_ID"].asString();
        }
        if (ReqJson.isMember("Note_Type"))
        {
            MyJsonPtr->checkColType(ReqJson, RespJson, "Note_Type", MyJson::ColType::STRING);
            NoteType = ReqJson["Note_Type"].asString();
        }
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
            MyJsonPtr->JsonstrToJson(TempNoteJson["Content"],TempNoteJson["Content"].asString());
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

// 通过NoteID查找Note数据
/*
    通过NoteID查找Note数据
Req:{
    "Note_ID"  :   0,
}
Resp:{
    "ErrorMsg"  :   [],         // 失败返回的错误信息
    "Result"    :   true/false, // 操作结果
    "Note_Data" :   [],         // 成功返回的帖子信息
}
*/
void MyDBService::Search_Note_By_NoteID(Json::Value &ReqJson, Json::Value &RespJson)
{
    auto MyBasePtr = app().getPlugin<MyBase>();
    auto MyJsonPtr = app().getPlugin<MyJson>();
    MyBasePtr->INFO_Func("Search_Note_By_NoteID", true, ReqJson);

    auto dbclientPrt = drogon::app().getDbClient();
    Mapper<drogon_model::novel::Note> NoteMgr(dbclientPrt);

    int limit, offset;
    string KeyWord, NoteType, UserID;

    // 检查ReqJson数据是否合法
    try
    {
        MyBasePtr->DEBUGLog("开始检查ReqJson数据是否合法", true);
        // "Offset"        :   0, //
        std::map<string, MyJson::ColType> ColMap;
        ColMap["Note_ID"] = MyJson::ColType::INT;
        MyJsonPtr->checkMemberAndTypeInMap(ReqJson, RespJson, ColMap);
        MyBasePtr->DEBUGLog("ReqJson数据合法", true);
    }
    catch (Json::Value &RespJson)
    {
        RespJson["Result"] = false;
        MyBasePtr->INFO_Func("Search_Note_By_NoteID", false, RespJson);
        return;
    }
    catch (...)
    {
        RespJson["Result"] = false;
        RespJson["ErrorMsg"].append("MyDBService::Search_Note_By_NoteID::Error");
        MyBasePtr->INFO_Func("Search_Note_By_NoteID", false, RespJson);
        return;
    }

    // 查询指定帖子
    try
    {
        MyBasePtr->DEBUGLog("开始查询指定帖子", true);
        drogon_model::novel::Note Note = NoteMgr.findByPrimaryKey(ReqJson["Note_ID"].asInt());
        MyBasePtr->DEBUGLog("查询指定帖子完毕", true);
        RespJson["Result"] = true;
        RespJson["Note_Data"] = Note.toJson();
        MyJsonPtr->JsonstrToJson(RespJson["Note_Data"]["Content"],RespJson["Note_Data"]["Content"].asString());
            
        MyBasePtr->INFO_Func("Search_Note_By_NoteID", false, RespJson);
        return;
    }
    catch (const drogon::orm::DrogonDbException &e)
    {
        if (e.base().what() == string("0 rows found"))
        {
            RespJson["ErrorMsg"].append("帖子不存在");
        }
        else if (e.base().what() == string("Found more than one row"))
        {
            RespJson["ErrorMsg"].append("要更改的资源ID重复,请联系管理员");
        }
        else
        {
            RespJson["ErrorMsg"].append(e.base().what());
        }
        RespJson["Result"] = false;
        MyBasePtr->INFO_Func("Search_Note_By_NoteID", false, RespJson);
        return;
    }
    catch (...)
    {
        RespJson["Result"] = false;
        RespJson["ErrorMsg"].append("帖子查询失败");
        MyBasePtr->INFO_Func("Search_Note_By_NoteID", false, RespJson);
        return;
    }
}


// 根据用户ID返回用户封号时间
/*
Req:{
    "User_ID":0
}
Resp:{
    "ErrorMsg":[],//查询失败的时候返回错误信息
    "Ban_Time":"",//查询成功的时候返回封号时间
    "Result"  :true/false,// 无论是用户不是封号状态还是其他没查询到的情况都是false
}
*/
void MyDBService::Search_User_Ban_Time(Json::Value &ReqJson, Json::Value &RespJson)
{

    auto MyBasePtr = app().getPlugin<MyBase>();
    auto MyJsonPtr = app().getPlugin<MyJson>();
    MyBasePtr->INFO_Func("Search_User_Ban_Time", true, ReqJson);

    auto dbclientPrt = drogon::app().getDbClient();
    Mapper<drogon_model::novel::User> UserMgr(dbclientPrt);
    Mapper<drogon_model::novel::Action> ActionMgr(dbclientPrt);
    ActionMgr.orderBy("Time",SortOrder::DESC);
    ActionMgr.limit(1);
    int UserID;
    // 检查ReqJson数据是否合法
    try
    {
        MyBasePtr->DEBUGLog("开始检查ReqJson数据是否合法", true);
        // "User_ID"   :   0,
        std::map<string, MyJson::ColType> ColMap;
        ColMap["User_ID"] = MyJson::ColType::INT;
        MyJsonPtr->checkMemberAndTypeInMap(ReqJson, RespJson, ColMap);
        MyBasePtr->DEBUGLog("ReqJson数据合法", true);

        UserID = ReqJson["User_ID"].asInt();
        MyBasePtr->DEBUGLog("开始查询用户 : " + ReqJson["User_ID"].asString(), true);
        drogon_model::novel::User user = UserMgr.findByPrimaryKey(UserID);
        MyBasePtr->DEBUGLog("用户查询完毕 : " + user.toJson().toStyledString(), true);

        if(user.getValueOfStatus() != "Normal" && user.getValueOfStatus() != "Ban")
        {
            RespJson["ErrorMsg"].append("用户状态异常");
            throw RespJson;
        }
        if(user.getValueOfStatus() != "Ban")
        {
            RespJson["ErrorMsg"].append("用户并未处于封号状态");
            throw RespJson;
        }
        MyBasePtr->DEBUGLog("开始查询用户(ID=" + ReqJson["User_ID"].asString() + ")的封号截止时间", true);
        Criteria Type_cri(drogon_model::novel::Action::Cols::_Type,CompareOperator::EQ,"User_Status");
        vector<drogon_model::novel::Action> vecaction = ActionMgr.findBy(Type_cri);
        
        MyBasePtr->DEBUGLog("用户封号截止时间查询完毕", true);
        MyBasePtr->DEBUGLog("vecaction.size : "+ to_string(vecaction.size()), true);
        if(vecaction.size() == 0)
        {
            RespJson["ErrorMsg"].append("并未查询到有关用户状态变更的行为");
            throw RespJson;
        }
        Json::Value MemoJson;
        MyJsonPtr->JsonstrToJson(MemoJson,vecaction[0].getValueOfMemo());
        MyBasePtr->DEBUGLog("vecaction[0].getValueOfMemo : "+ MemoJson.toStyledString(), true);
        
        if(MemoJson["Type"].asString() != "Ban")
        {
            RespJson["ErrorMsg"].append("用户并未处于封号状态");
            throw RespJson;
        }
        RespJson["Ban_Time"] = MemoJson["Ban_Time"].asString();
        RespJson["Result"]  =  true;
    }
    catch(const drogon::orm::DrogonDbException &e)
    {
	    RespJson["ErrorMsg"].append(e.base().what());
        RespJson["Result"]  =  false;
        MyBasePtr->INFO_Func("Search_User_PersonalData", false, RespJson);
        return;
    }
    catch(Json::Value &e)
    {
        RespJson["ErrorMsg"] = e["ErrorMsg"];
        RespJson["ErrorMsg"].append("查询封号截止时间失败");
        RespJson["Result"]  =  false;
        MyBasePtr->INFO_Func("Search_User_PersonalData", false, RespJson);
        return;
    }
    catch(...)
    {
        RespJson["ErrorMsg"].append("查询封号截止时间失败");
        RespJson["Result"]  =  false;
        MyBasePtr->INFO_Func("Search_User_PersonalData", false, RespJson);
        return;
    }
    MyBasePtr->INFO_Func("Search_User_PersonalData", false, RespJson);
    return ;
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
    try{
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
    catch (Json::Value &e)
    {
        RespJson["ErrorMsg"] = e["ErrorMsg"];
        RespJson["Result"] = false;
        MyBasePtr->INFO_Func("Search_User_PersonalData", false, RespJson);
        return;
    }
    try
    {
        MyBasePtr->DEBUGLog("开始查询用户 : " + ParaJson["User_ID"].asString(), true);
        drogon_model::novel::User user = UserMgr.findByPrimaryKey(UserID);
        MyBasePtr->DEBUGLog("用户查询完毕 : " + user.toJson().toStyledString(), true);

        RespJson["User_Data"] = user.toJson();
        RespJson["Result"] = true;
        MyBasePtr->INFO_Func("Search_User_PersonalData", false, RespJson);
    }
    catch(...)
    {
        RespJson["ErrorMsg"].append("用户查询发生错误");
        RespJson["Result"] = false;
        MyBasePtr->INFO_Func("Search_User_PersonalData", false, RespJson);
        return;
    }
}



// 通过Upload_ID查询上传申请的数据
/*
    通过Upload_ID查询上传申请的数据
Req:{
    "Upload_ID":0//上传编号
}
Resp:{
    "ErrorMsg":[],
    "Result" : true/false
    "Upload_Data":{}
}
*/
void MyDBService::Search_Upload_By_UploadID(Json::Value &ReqJson, Json::Value &RespJson)
{
    
    auto MyBasePtr = app().getPlugin<MyBase>();
    auto MyJsonPtr = app().getPlugin<MyJson>();
    MyBasePtr->INFO_Func("Search_Upload_By_UploadID", true, ReqJson);

    auto dbclientPrt = drogon::app().getDbClient();
    Mapper<drogon_model::novel::Upload> UploadMgr(dbclientPrt);

    // 检查ReqJson数据是否合法
    try
    {
        MyBasePtr->DEBUGLog("开始检查ReqJson数据是否合法", true);
        // "Upload_ID":0,//上传编号
        std::map<string, MyJson::ColType> ColMap;
        ColMap["Upload_ID"] = MyJson::ColType::INT;
        MyJsonPtr->checkMemberAndTypeInMap(ReqJson, RespJson, ColMap);
        MyBasePtr->DEBUGLog("ReqJson数据合法", true);
    }
    catch (Json::Value &e)
    {
        RespJson["ErrorMsg"] = e["ErrorMsg"];
        RespJson["ErrorMsg"].append("ReqJson数据不合法");
        RespJson["Result"] = false;
        MyBasePtr->INFO_Func("Search_Upload_By_UploadID", false, RespJson);
        return;
    }

    // 查询上传
    try
    {
        MyBasePtr->DEBUGLog("开始查询上传", true);
        drogon_model::novel::Upload TUpload = UploadMgr.findByPrimaryKey(ReqJson["Upload_ID"].asInt());
        MyBasePtr->DEBUGLog("查询上传完毕", true);
        RespJson["Result"] = true;
        RespJson["Upload_Data"] = TUpload.toJson();
        MyBasePtr->INFO_Func("Search_Upload_By_UploadID", false, RespJson);
        return;
    }
    catch (const drogon::orm::DrogonDbException &e)
    {

        if (e.base().what() == string("0 rows found"))
        {
            RespJson["ErrorMsg"].append("此上传数据不存在");
        }
        else if (e.base().what() == string("Found more than one row"))
        {
            RespJson["ErrorMsg"].append("上传ID重复,请联系管理员");
        }
        else
        {
            RespJson["ErrorMsg"].append(e.base().what());
        }
        RespJson["Result"] = false;
        RespJson["ErrorMsg"].append("上传数据查询失败");
        MyBasePtr->INFO_Func("Search_Upload_By_UploadID", false, RespJson);
        return;
    }
    catch (...)
    {
        RespJson["Result"] = false;
        RespJson["ErrorMsg"].append("图书查询失败");
        MyBasePtr->INFO_Func("Search_Upload_By_UploadID", false, RespJson);
        return;
    }
}


// 指定章节的某一版本的有效
/*
    指定章节的某一版本的有效
Req:{
    "Book_ID":0,
    "Part_Num"0,
    "Chapter_Num":0
    "Version":0
}
Resp:{
    "ErrorMsg"  :   [],         // 失败返回的错误信息
    "Result"    :   true/false, // 操作结果
}
*/
void MyDBService::Select_Chapter_Version_Valid(Json::Value &ReqJson, Json::Value &RespJson)
{

    auto MyBasePtr = app().getPlugin<MyBase>();
    auto MyJsonPtr = app().getPlugin<MyJson>();
    MyBasePtr->INFO_Func("Select_Chapter_Version_Valid", true, ReqJson);

    auto dbclientPrt = drogon::app().getDbClient();
    Mapper<drogon_model::novel::Chapter> ChapterMgr(dbclientPrt);
    Mapper<drogon_model::novel::Book> BookMgr(dbclientPrt);

    int limit = 0, offset = 0, BookID,PartNum,ChapterNum,Version;

    // 检查ReqJson数据是否合法
    try
    {
        MyBasePtr->DEBUGLog("开始检查ReqJson数据是否合法", true);
        std::map<string, MyJson::ColType> ColMap;
        ColMap["Book_ID"] = MyJson::ColType::INT;
        ColMap["Part_Num"] = MyJson::ColType::INT;
        ColMap["Chapter_Num"] = MyJson::ColType::INT;
        ColMap["Version"] = MyJson::ColType::INT;
        MyJsonPtr->checkMemberAndTypeInMap(ReqJson, RespJson, ColMap);
        MyBasePtr->DEBUGLog("ReqJson数据合法", true);
        BookID = atoi(ReqJson["Book_ID"].asString().c_str());
        PartNum = atoi(ReqJson["Part_Num"].asString().c_str());
        ChapterNum = atoi(ReqJson["Chapter_Num"].asString().c_str());
        Version = atoi(ReqJson["Version"].asString().c_str());

    }
    catch (Json::Value &RespJson)
    {
        RespJson["Result"] = false;
        MyBasePtr->INFO_Func("Select_Chapter_Version_Valid", false, RespJson);
        return;
    }
    catch (...)
    {
        RespJson["Result"] = false;
        RespJson["ErrorMsg"].append("MyDBService::Select_Chapter_Version_Valid::Error");
        MyBasePtr->INFO_Func("Select_Chapter_Version_Valid", false, RespJson);
        return;
    }

    // 查询章节所有版本
    Json::Value TempResp;
    
    Search_Chapter_All_Version(ReqJson,TempResp);
    if(!TempResp["Result"])
    {
        RespJson["Result"] = false;
        RespJson["ErrorMsg"] = TempResp["ErrorMsg"];
        MyBasePtr->INFO_Func("Select_Chapter_Version_Valid", false, RespJson);
        return;
    }
    
    if(TempResp["Chapter_List"].isNull())
    {
        RespJson["Result"] = false;
        RespJson["ErrorMsg"] = TempResp["ErrorMsg"];
        RespJson["ErrorMsg"].append("不存在目标章节");
        MyBasePtr->INFO_Func("Select_Chapter_Version_Valid", false, RespJson);
        return;
    }
    // 处理章节数据
    bool find =false;
    for (auto &obj : TempResp["Chapter_List"])
    {
        if(obj["Valid"].asBool() == false && obj["Version"].asInt() != Version)
            continue;
        if(obj["Version"].asInt() == Version)
        {
            if(obj["Valid"].asBool())
            {
                RespJson["Result"] = false;
                RespJson["ErrorMsg"].append("目标章节本来就生效");
                MyBasePtr->INFO_Func("Select_Chapter_Version_Valid", false, RespJson);
                return;
            }
            find = true;
            break;
        }
        // std::cout << user.toJson().toStyledString() <<std::endl;
    }
    
    if(!find)
    {
        RespJson["Result"] = false;
        RespJson["ErrorMsg"].append("不存在目标章节");
        MyBasePtr->INFO_Func("Select_Chapter_Version_Valid", false, RespJson);
        return;
    }


    try
    {
        MyBasePtr->DEBUGLog("开始指定图书的章节的版本生效完毕", true);
        Json::Value vecJsonVal, TempChapterJson;
        for (auto &obj : TempResp["Chapter_List"])
        {
            if(obj["Valid"].asBool() == false && obj["Version"].asInt() != Version)
                continue;
            drogon_model::novel::Chapter chapter;
            chapter.updateByJson(obj);
            if(chapter.getValueOfVersion() == Version)
            {
                chapter.setValid(true);
            }
            else
                chapter.setValid(false);
                    
            MyBasePtr->DEBUGLog("开始更新章节数据", true);
            int row = ChapterMgr.update(chapter);
            if (row != 1)
            {
                RespJson["ErrorMsg"].append("章节数据更新失败");
                throw RespJson;
            }
            // std::cout << user.toJson().toStyledString() <<std::endl;
        }

        MyBasePtr->DEBUGLog("指定图书的章节的版本生效完毕", true);
        RespJson["Result"] = true;
        MyBasePtr->INFO_Func("Select_Chapter_Version_Valid", false, RespJson);
        return;
    }
    catch (Json::Value &e)
    {
        RespJson["Result"] = false;
        RespJson["ErrorMsg"] = e["ErrorMsg"];
        MyBasePtr->INFO_Func("Select_Chapter_Version_Valid", false, RespJson);
        return;
    }
    catch (...)
    {
        RespJson["Result"] = false;
        RespJson["ErrorMsg"].append("指定图书的章节的版本生效失败");
        MyBasePtr->INFO_Func("Select_Chapter_Version_Valid", false, RespJson);
        return;
    }
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
        Criteria BookName_cri(drogon_model::novel::Book::Cols::_Book_Name, CompareOperator::EQ, Book_Name);
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
        //   "Vol_Num":1,// 分卷数
        //   "Chapter_Num":3,// 章节数
        // "Chapter_Title"   :   "", //  章节标题
        // "Content_Array"   :   []  //  章节内容
        // 创建检查列表 key是字段名 value 是字段类型

        std::map<string, MyJson::ColType> ColMap;
        ColMap["Vol_Num"] = MyJson::ColType::INT;
        ColMap["Chapter_Num"] = MyJson::ColType::INT;
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
        ColMap["Change_Col"] = MyJson::ColType::ARRAY;
        MyJsonPtr->checkMemberAndTypeInMap(ReqJson, RespJson, ColMap);
        ColMap.clear();
        for(int i = 0 ;i< ReqJson["Change_Col"].size() ; i++)
        {
            ColMap[ReqJson["Change_Col"][i].asString()] = MyJson::ColType::STRING;
        }

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
        RespJson["ErrorMsg"] = e["ErrorMsg"];
        RespJson["ErrorMsg"].append("数据不合法");
        MyBasePtr->INFO_Func("Update_User_PersonalData", false, RespJson);
        throw RespJson;
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
    RespJson["User_Data"] = user.toJson();
    RespJson["User_Data"].removeMember("Password");

    MyBasePtr->DEBUGLog("用户数据更新完成: " + user.toJson().toStyledString(), true);
    MyBasePtr->INFO_Func("Update_User_PersonalData", false, RespJson);
}

// 解析用户行为
void MyDBService::ParseAction(Json::Value &Resp,const Json::Value &Action)
{
    auto MyBasePtr = app().getPlugin<MyBase>();
    auto MyJsonPtr = app().getPlugin<MyJson>();
    Json::Value temp = Action;
    MyBasePtr->INFO_Func("ParseAction", true, temp);
    try
    {
        Json::Value Memo;
        MyJsonPtr->JsonstrToJson(Memo,Action["Memo"].asString());
        string type = Action["Type"].asString();
        Resp["Time"] = Action["Time"].asString();
        if(type == "Resource_Upload")// 资源上传行为
        {
            Resp["Type"] = "资源上传";
            Resp["Memo"]["解释"] = Memo["Explain"].asString();
            Resp["Memo"]["处理者"] = Memo["Processor"].asString();
            Resp["Memo"]["上传ID"] = Memo["Upload_ID"].asInt();
            Resp["Memo"]["上传类型"] = Memo["Upload_Type"].asString();
        }
        else if(type == "Examine_Upload")// 审核行为
        {
            Resp["Type"] = "资源上传审核";
            Resp["Memo"]["审核结果"] = Memo["Explain"].asString();
            Resp["Memo"]["处理者"] = Memo["Processor"].asString();
            Resp["Memo"]["上传ID"] = Memo["Upload_ID"].asInt();
            Resp["Memo"]["结果类型"] = Memo["Type"].asString();
        }
        else if(type == "User_Integral")// 用户积分相关
        {
            // Memo:{
            //     "Explain" : "用户上传资源成功,1章奖励1积分",
            //     "New_Data" : {
            //         "Integral" : 135,
            //         "TotalIntegral" : 135
            //     },
            //     "Num" : 1,
            //     "Old_Data" : {
            //         "Integral" : 134,
            //         "TotalIntegral" : 134
            //     },
            //     "Processor" : "system(0)",
            //     "Type" : "Add"
            // },
            Resp["Type"] = "积分变化";
            Resp["Memo"]["说明"] = Memo["Explain"];
            Resp["Memo"]["积分数"] = Memo["Num"];
            Resp["Memo"]["旧数据"] = Memo["Old_Data"];
            Resp["Memo"]["新数据"] = Memo["New_Data"];
            Resp["Memo"]["处理者"] = Memo["Processor"];
            Resp["Memo"]["变化类型"] = Memo["Type"];
        }
        else if(type == "Money")// 用户积分充值
        {
            Resp["Type"] = "积分充值";
            Resp["Memo"]["说明"] = Memo["Explain"].asString();
            Resp["Memo"]["积分数"] = Memo["Integral_Num"].asInt();
            Resp["Memo"]["金额"] = to_string(Memo["Money_Num"].asInt())+"元";
            Resp["Memo"]["处理者"] = Memo["Processor"].asString();
        }
    }
    catch(Json::Value &e)
    {
        Resp["Memo"] = "详情解析发生错误";
    }
    MyBasePtr->INFO_Func("ParseAction", false, Resp);
}






// 用户下载已有的书的章节_private
/*

Req:{
    "Book_ID":0,
    "Chapter_Num":[],
    "Para" : {}
}
Resp:{
    "Book_Data":{},// 图书资源数据
    "Chapter_Data":[{}]//章节资源数据
    "ErrorMsg":[],// 错误信息
}
*/
bool MyDBService::Download_Resource_Private(Json::Value &ReqJson, Json::Value &RespJson)
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
    
    ParaJson = ReqJson["Para"];
    UserID = std::atoi(ParaJson["User_ID"].asString().c_str());
    LoginStatus = ParaJson["Login_Status"].asString();
    Book_ID = ReqJson["Book_ID"].asInt();

    vector<int> vecChapter_Num;
    drogon_model::novel::Book TBook; // 目标图书
    Json::Value TBookMemo;
    Json::Value TBookMemoChapter;      //  Memo/Chapter
    Json::Value TBookMemoExtraChapter; //  Memo/ExtraChapter
    try
    {
        MyBasePtr->DEBUGLog("开始查询用户 : " + to_string(UserID), true);
        drogon_model::novel::User user = UserMgr.findByPrimaryKey(UserID);
        MyBasePtr->DEBUGLog("用户查询完毕 : " + user.toJson().toStyledString(), true);

        // 验证用户
        if (user.getValueOfUserId() != UserID)
        {
            RespJson["ErrorMsg"].append("登入的UserID(" + to_string(UserID) + ")与查询到的UserID(" + to_string(user.getValueOfUserId()) + ")不同");
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
                throw RespJson;
            }
            MyBasePtr->DEBUGLog("要下载的图书存在", true);
            TBook = vecBook[0];
        }
    }
    catch (Json::Value &e)
    {
        RespJson["ErrorMsg"] = e["ErrorMsg"];
        MyBasePtr->INFO_Func("Download_Resource", false, RespJson);
        return false;
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
        RespJson["ErrorMsg"] = e["ErrorMsg"];
        MyBasePtr->INFO_Func("Download_Resource", false, RespJson);
        return false;
    }

    // 保存图书信息
    try
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
    catch(...)
    {
        RespJson["ErrorMsg"].append("保存图书信息发生错误");
        MyBasePtr->INFO_Func("Download_Resource", false, RespJson);
        return false;
    }

    // 获取要下载的图书的章节ID
    try
    {

        MyBasePtr->DEBUGLog("开始获取要下载的图书的章节ID", true);
        int ChapterSize = ReqJson["Chapter_Num"].size();
        for (int i = 0; i < ChapterSize; i++)
        {
            vecChapter_Num.push_back(ReqJson["Chapter_Num"][i].asInt());
        }
        MyBasePtr->DEBUGLog("要下载的图书的章节ID获取完成", true);
    }
    catch(...)
    {
        RespJson["ErrorMsg"].append("获取要下载的图书的章节ID发生错误");
        MyBasePtr->INFO_Func("Download_Resource", false, RespJson);
        return false;
    }

    // 下载并保存章节信息
    try
    {
        MyBasePtr->DEBUGLog("开始获取指定的图书的章节数据", true);
        Criteria BookID_cri(drogon_model::novel::Chapter::Cols::_Book_ID, CompareOperator::EQ, Book_ID);
        Criteria ChapterNum_cri(drogon_model::novel::Chapter::Cols::_Chapter_Num, CompareOperator::In, vecChapter_Num);
        Criteria Valid_cri(drogon_model::novel::Chapter::Cols::_Valid, CompareOperator::EQ, true);
        // MyBasePtr->DEBUGLog("使用的SQL语句为 " + ChapterNum_cri.criteriaString(), true);
        vector<drogon_model::novel::Chapter> vecChapter = ChapterMgr.findBy(BookID_cri && ChapterNum_cri && Valid_cri);
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
    catch(Json::Value &e)
    {
        RespJson["ErrorMsg"] = e["ErrorMsg"];
        RespJson["ErrorMsg"].append("下载并保存章节信息发生错误");
        MyBasePtr->INFO_Func("Download_Resource", false, RespJson);
        return false;
    }
    MyBasePtr->INFO_Func("Download_Resource", false, RespJson);
    return true;
}

// 数据库插入行为记录
/*
    数据库插入行为记录  return bool // 代表插入过程是否正常，数据异常时返回false
Req:{
    "User_ID"   :   0,// 用户ID
    "Action_Type"      :   "",
    "Action_Memo"      :   {}
}
Resp:{
    "ErrorMsg"[失败]    :   [],//失败原因(数据异常)
    "Action_Data"[成功] :   {},//新建行为的数据
}
*/
bool MyDBService::Insert_Action(Json::Value &ReqJson, Json::Value &RespJson)
{
    auto MyJsonPtr = app().getPlugin<MyJson>();
    auto MyBasePtr = app().getPlugin<MyBase>();
    auto MyToolsPtr = app().getPlugin<MyTools>();
    MyBasePtr->INFO_Func("Insert_Action", true, ReqJson);

    auto dbclientPrt = drogon::app().getDbClient();
    Mapper<drogon_model::novel::Action> ActionMgr(dbclientPrt);

    // 插入前检查图书数据是否合法
    try
    {
        MyBasePtr->DEBUGLog("插入前检查行为数据是否合法", true);
        // "User_ID"   :   0,// 0 代表system 其余表示用户ID
        // "Action_Type"      :   "",
        // "Action_Memo"      :   {}
        std::map<string, MyJson::ColType> ColMap;
        ColMap["User_ID"] = MyJson::ColType::INT;
        ColMap["Action_Type"] = MyJson::ColType::STRING;
        ColMap["Action_Memo"] = MyJson::ColType::JSON;
        MyJsonPtr->checkMemberAndTypeInMap(ReqJson, RespJson, ColMap);
        MyBasePtr->DEBUGLog("插入前检查显示行为数据合法", true);
    }
    catch (Json::Value &e)
    {
        RespJson["ErrorMsg"].append("要记录的行为数据不合法");
        MyBasePtr->INFO_Func("Insert_Action", false, RespJson);
        return false;
    }

    drogon_model::novel::Action NewAction;
    Json::Value ActionMemo;

    // 准备要创建的行为的数据
    {
        MyBasePtr->DEBUGLog("开始准备行为数据", true);
        NewAction.setMemo(ReqJson["Action_Memo"].toStyledString());
        NewAction.setType(ReqJson["Action_Type"].asString());
        NewAction.setUserId(ReqJson["User_ID"].asInt());
        MyBasePtr->DEBUGLog("准备行为数据完成", true);
    }

    // 准备插入行为数据
    try
    {
        MyBasePtr->DEBUGLog("准备插入的行为数据为 : " + NewAction.toJson().toStyledString(), true);
        MyBasePtr->DEBUGLog("准备插入行为数据", true);
        ActionMgr.insert(NewAction);
        MyBasePtr->DEBUGLog("插入行为数据完毕", true);
        MyBasePtr->DEBUGLog("插入的行为数据为 : " + NewAction.toJson().toStyledString(), true);
    }
    // 新建行为失败
    catch (...)
    {
        RespJson["ErrorMsg"].append("行为记录失败(数据异常)");
        MyBasePtr->INFO_Func("Insert_Action", false, RespJson);
        LOG_DEBUG << NewAction.toJson().toStyledString();
        return false;
    }
    RespJson["Action_Data"] = NewAction.toJson();
    MyJsonPtr->JsonstrToJson(RespJson["Action_Data"]["Memo"],RespJson["Action_Data"]["Memo"].asString());
    MyBasePtr->INFO_Func("Insert_Action", false, RespJson);
    return true;
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
        Criteria BookName_cri(drogon_model::novel::Book::Cols::_Book_Name, CompareOperator::EQ, BookName);
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
        NewBook.setBookName(BookName);           // 书名
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
    RespJson["Upload_Status"] = "已成功";
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
    "Vol_Num"         :   1,  //  分卷数      
    "Chapter_Num"     :   3,  //  章节数
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
        // "Vol_Num"         :   1,  //  分卷数      
        // "Chapter_Num"     :   3,  //  章节数
        // "Chapter_Title"   :   "", //  章节标题
        // "Content_Array"   :   []  //  章节内容
        // 创建检查列表 key是字段名 value 是字段类型
        std::map<string, MyJson::ColType> ColMap;
        ColMap["User_ID"] = MyJson::ColType::INT;
        ColMap["Book_ID"] = MyJson::ColType::INT;
        ColMap["Vol_Num"] = MyJson::ColType::INT;
        ColMap["Chapter_Num"] = MyJson::ColType::INT;
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
    string ChapterTitle = ReqJson["Chapter_Title"].asString(); // 章节名
    int Part_Num = ReqJson["Vol_Num"].asInt();              // 章节卷
    int Chapter_Num = ReqJson["Chapter_Num"].asInt();       // 章节数
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

        // Part_Num = ChapterJson.size() - 1;
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
            while(ChapterJson.size() <= Part_Num)
            {
                ChapterJson.append(0);
            }
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
    MyJsonPtr->JsonstrToJson(RespJson["Chapter_Data"]["Content"],RespJson["Chapter_Data"]["Content"].asString());
    MyJsonPtr->JsonstrToJson(RespJson["Memo"]["Content"],RespJson["Chapter_Data"]["Memo"].asString());
    MyBasePtr->INFO_Func("Insert_Chapter", false, RespJson);
    return true;
}



// 创建意见反馈
/*
    创建意见反馈
Req:{
    "Para"    : {}
    "Type"    :   "", // 意见类型
    "Memo"    : {},// 详细参数
}
Resp:{
    "ErrorMsg"  :   [],//失败返回的错误信息
    "Result"    :   true/false,// 操作结果
    "Idea_Data" :   {},// 成功返回的意见信息
}
*/
void MyDBService::Insert_Idea(Json::Value &ReqJson, Json::Value &RespJson)
{
    auto MyJsonPtr = app().getPlugin<MyJson>();
    auto MyBasePtr = app().getPlugin<MyBase>();
    auto MyToolsPtr = app().getPlugin<MyTools>();
    MyBasePtr->INFO_Func("Insert_Idea", true, ReqJson);

    auto dbclientPrt = drogon::app().getDbClient();
    Mapper<drogon_model::novel::Idea> IdeaMgr(dbclientPrt);
    drogon_model::novel::Idea NewIdea;

    int UserID;
    string LoginStatus;
    Json::Value ParaJson, CommentContentJson, TempJson;
    // 插入前检查帖子数据是否合法
    try
    {
        MyBasePtr->DEBUGLog("创建意见前检查意见数据是否合法", true);
        // "Type"    :   "", // 意见类型
        // "Memo"    : {},// 详细参数
        // "Para"      :{},// 用户携带的Token数据
        std::map<string, MyJson::ColType> ColMap;
        ColMap["Type"] = MyJson::ColType::STRING;
        ColMap["Memo"] = MyJson::ColType::JSON;
        ColMap["Para"] = MyJson::ColType::JSON;
        MyJsonPtr->checkMemberAndTypeInMap(ReqJson, RespJson, ColMap);
        MyBasePtr->DEBUGLog("创建意见前检查意见数据显示合法", true);

        MyBasePtr->DEBUGLog("开始检查Para数据", true);
        ColMap.clear();
        ParaJson = ReqJson["Para"];
        ColMap["User_ID"] = MyJson::ColType::STRING;
        ColMap["Login_Status"] = MyJson::ColType::STRING;
        MyJsonPtr->checkMemberAndTypeInMap(ParaJson, RespJson, ColMap);
        MyBasePtr->DEBUGLog("检查Para数据完成", true);

        MyBasePtr->DEBUGLog("开始检查操作权限", true);
        UserID = std::atoi(ParaJson["User_ID"].asString().c_str());
        LoginStatus = ParaJson["Login_Status"].asString();
        if ((LoginStatus != "user") && (LoginStatus != "admin") && (LoginStatus != "root") && (LoginStatus != "system"))
        {
            RespJson["ErrorMsg"].append("账户权限错误,请联系管理员");
            throw RespJson;
        }
        MyBasePtr->DEBUGLog("检查操作权限通过", true);
    }
    catch (Json::Value &e)
    {
        RespJson["ErrorMsg"] = e["ErrorMsg"];
        RespJson["ErrorMsg"].append("意见反馈失败(数据格式不合法)");
        RespJson["Result"] = false;
        MyBasePtr->INFO_Func("Insert_Idea", false, RespJson);
        return;
    }


    // 准备要创建的数据
    {
        MyBasePtr->DEBUGLog("开始准备新意见数据", true);

        NewIdea.setUserId(UserID);
        NewIdea.setMemo(ReqJson["Memo"].toStyledString());
        NewIdea.setType(ReqJson["Type"].asString());
        NewIdea.setStatus("待处理");
        NewIdea.setIsmanage(false);
        

        MyBasePtr->DEBUGLog("准备新意见数据完成", true);
    }

    // 准备插入意见数据
    try
    {
        MyBasePtr->DEBUGLog("准备插入的新意见数据为 : " + NewIdea.toJson().toStyledString(), true);
        MyBasePtr->DEBUGLog("准备插入新意见数据", true);
        IdeaMgr.insert(NewIdea);
        MyBasePtr->DEBUGLog("插入新意见数据完毕", true);
        MyBasePtr->DEBUGLog("插入的新数据为 : " + NewIdea.toJson().toStyledString(), true);
    }
    // 新建意见失败
    catch (const drogon::orm::DrogonDbException &e)
    {
        RespJson["Result"] = false;
        RespJson["ErrorMsg"].append("创建新意见失败(" + string(e.base().what()) + ")");
        MyBasePtr->INFO_Func("Insert_Idea", false, RespJson);
        return;
    }
    catch (...)
    {
        RespJson["Result"] = false;
        RespJson["ErrorMsg"].append("创建新意见失败(数据异常)");
        MyBasePtr->INFO_Func("Insert_Idea", false, RespJson);
        return;
    }
    RespJson["Result"] = true;
    RespJson["Idea_Data"] = NewIdea.toJson();
    MyJsonPtr->JsonstrToJson(RespJson["Idea_Data"]["Memo"],RespJson["Idea_Data"]["Memo"].asString());
    MyBasePtr->INFO_Func("Insert_Idea", false, RespJson);
    return;
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

// 更新用户数据
/*
    更新用户数据
Req:{
    "User_ID"           :   0,
    "Processor"         :   "",
    "Change_ID"         :   0 ,
    "Change_Name"       :   "",
    "Change_Password"   :   "",
    "Change_Sex"        :   "",
    "Change_Col"        :   ["Change_Name","Change_Password","Change_Sex"]
}

Resp:{
    "ErrorMsg" : []
}
*/
bool MyDBService::Update_User(Json::Value &ReqJson, Json::Value &RespJson)
{
    auto MyJsonPtr = app().getPlugin<MyJson>();
    auto MyBasePtr = app().getPlugin<MyBase>();
    auto MyToolsPtr = app().getPlugin<MyTools>();
    MyBasePtr->INFO_Func("Update_User", true, ReqJson);

    auto dbclientPrt = drogon::app().getDbClient();
    Mapper<drogon_model::novel::User> UserMgr(dbclientPrt);
    Mapper<drogon_model::novel::Upload> UploadMgr(dbclientPrt);
    Mapper<drogon_model::novel::Book> BookMgr(dbclientPrt);
    drogon_model::novel::User user;

    // 检查ReqJson数据是否合法
    try
    {
        MyBasePtr->DEBUGLog("开始检查ReqJson数据是否合法", true);
        // "User_ID"           :   0,
        // "Processor"         :   "",
        // "Change_ID"         :   0 ,
        // "Change_Name"       :   "",
        // "Change_Password"   :   "",
        // "Change_Sex"        :   "",
        // "Change_Col"        :   ["Change_Name","Change_Password","Change_Sex"]
        std::map<string, MyJson::ColType> ColMap;
        ColMap["User_ID"] = MyJson::ColType::INT;
        ColMap["Processor"] = MyJson::ColType::STRING;
        ColMap["Change_ID"] = MyJson::ColType::INT;
        ColMap["Change_Name"] = MyJson::ColType::STRING;
        ColMap["Change_Password"] = MyJson::ColType::STRING;
        ColMap["Change_Sex"] = MyJson::ColType::STRING;
        ColMap["Change_Col"] = MyJson::ColType::ARRAY;
        MyJsonPtr->checkMemberAndTypeInMap(ReqJson, RespJson, ColMap);
        MyBasePtr->DEBUGLog("ReqJson数据合法", true);
    }
    catch (Json::Value &e)
    {
        RespJson["ErrorMsg"] = e["ErrorMsg"];
        RespJson["ErrorMsg"].append("ReqJson数据不合法");
        MyBasePtr->INFO_Func("Update_User", false, RespJson);
        return false;
    }

    int ChangeColSize, ChangeCount = 0;
    try
    {
        MyBasePtr->DEBUGLog("开始查询用户 : " + to_string(ReqJson["Change_ID"].asInt()), true);
        user = UserMgr.findByPrimaryKey(ReqJson["Change_ID"].asInt());
        MyBasePtr->DEBUGLog("用户查询完毕 : " + user.toJson().toStyledString(), true);

        ChangeColSize = ReqJson["Change_Col"].size();
        string ChangeColName, ChangeIntegralType;
        for (int j = 0; j < ChangeColSize; j++)
        {
            ChangeColName = ReqJson["Change_Col"][j].asString();
            MyBasePtr->DEBUGLog("开始修改用户数据(" + ChangeColName + ")", true);
            if (ChangeColName == "Change_Name")
            {
                if (user.getValueOfName() == ReqJson["Change_Name"].asString())
                {
                    MyBasePtr->DEBUGLog("字段(" + ChangeColName + ")没有产生变化", true);
                    continue;
                }
                user.setName(ReqJson["Change_Name"].asString());
                ChangeCount++;
            }
            else if (ChangeColName == "Change_Password")
            {
                if (user.getValueOfPassword() == ReqJson["Change_Password"].asString())
                {
                    MyBasePtr->DEBUGLog("字段(" + ChangeColName + ")没有产生变化", true);
                    continue;
                }
                user.setPassword(ReqJson["Change_Password"].asString());
                ChangeCount++;
            }
            else if (ChangeColName == "Change_Sex")
            {
                if (user.getValueOfSex() == ReqJson["Change_Sex"].asString())
                {
                    MyBasePtr->DEBUGLog("字段(" + ChangeColName + ")没有产生变化", true);
                    continue;
                }
                user.setSex(ReqJson["Change_Sex"].asString());
                ChangeCount++;
            }
            else
            {
                MyBasePtr->DEBUGLog("Change_Col包含错误字段(" + ChangeColName + ")", true);
            }
        }
        // 统计更新的字段数
        MyBasePtr->DEBUGLog("用户更新的字段数为" + to_string(ChangeCount), true);

        if (ChangeCount == 0)
        {
            RespJson["ErrorMsg"].append("用户数据无需更新");
            MyBasePtr->INFO_Func("Update_User", false, RespJson);
            return false;
        }

        MyBasePtr->DEBUGLog("开始更新用户(ID=" + to_string(ReqJson["Change_ID"].asInt()) + ")数据", true);
        int row = UserMgr.update(user);
        if (row != 1)
        {
            RespJson["ErrorMsg"].append("用户资料更新失败");
            MyBasePtr->INFO_Func("Update_User", false, RespJson);
            return false;
        }
        MyBasePtr->DEBUGLog("用户数据更新完成: " + user.toJson().toStyledString(), true);

        MyBasePtr->DEBUGLog("开始记录行为数据", true);
        Json::Value ActionJson;
        // Memo : {
        //     Explain : "", 
        //     Processor : "system/admin:xxx", 
        //     Change_ID : 0, 
        //     Change_Col : []
        // } 
        ActionJson["User_ID"] = ReqJson["Change_ID"].asInt();
        ActionJson["Action_Type"] = "User_Update";
        ActionJson["Action_Memo"]["Explain"] = ReqJson["Processor"].asString() + "修改用户数据";
        ActionJson["Action_Memo"]["Processor"] = ReqJson["Processor"].asString()  + "("+ ReqJson["User_ID"].asString() +")";
        ActionJson["Action_Memo"]["Change_Col"] = ReqJson["Change_Col"];
        if (!Insert_Action(ActionJson, RespJson))
        {
            LOG_DEBUG << "行为数据记录失败";
            MyBasePtr->DEBUGLog("行为数据记录失败", true);
            MyBasePtr->INFO_Func("Update_User", false, RespJson);
        }
        MyBasePtr->DEBUGLog("行为数据记录完毕", true);
    }
    catch (const drogon::orm::DrogonDbException &e)
    {
        if (e.base().what() == string("0 rows found"))
        {
            RespJson["ErrorMsg"].append("要更改的用户不存在");
        }
        else if (e.base().what() == string("Found more than one row"))
        {
            RespJson["ErrorMsg"].append("要更改的用户ID重复,请联系管理员");
        }
        else
        {
            RespJson["ErrorMsg"].append(e.base().what());
        }
        MyBasePtr->INFO_Func("Update_User", false, RespJson);
        return false;
    }

    MyBasePtr->INFO_Func("Update_User", false, RespJson);
    return true;
}

