#include "User_Resource.h"
using namespace User;
//add definition of your processing function here

// 图书上传接口
void Resource::Upload(const HttpRequestPtr &req,std::function<void (const HttpResponsePtr &)> &&callback) const
{
    auto MyJsonPtr = app().getPlugin<MyJson>();
    auto MyBasePtr = app().getPlugin<MyBase>();
    auto MyToolsPtr = app().getPlugin<MyTools>();
    drogon::HttpResponsePtr result;
    const unordered_map<string,string>parameters = req->getParameters();
    Json::Value RespVal;
    
    MyJsonPtr->UnMapToJson(RespVal, parameters, "parameters");
    RespVal["简介"] = "图书上传接口";
    MyBasePtr->DEBUGLog(RespVal.toStyledString(), true);
    auto ReqJson=req->getJsonObject();
    try
    {
        // 读取Json数据
        Json::Value ReqJsonValue=*ReqJson;
        MyBasePtr->DEBUGLog(ReqJsonValue.toStyledString(), true);

        // 检查数据完整性
        {    
            MyBasePtr->DEBUGLog("开始检查数据完整性", true);
            // "Book_ID"   :   0,
            // "Content"   :   "",
            // "UploadType":   ""
            // 创建检查列表 key是字段名 value 是字段类型
            std::map<string,MyJson::ColType>ColMap;
            ColMap["Book_ID"]=MyJson::ColType::INT;
            ColMap["Content"]=MyJson::ColType::JSON;
            ColMap["UploadType"]=MyJson::ColType::STRING;
            MyJsonPtr->checkMemberAndTypeInMap(ReqJsonValue,RespVal,ColMap);
            MyBasePtr->DEBUGLog("检查数据完整性完成", true);
        }

        // 读取UserID LoginStatus数据
        auto UserID=std::atoi(RespVal["parameters"]["UserID"].asString().c_str());
        auto BookID=ReqJsonValue["Book_ID"].asInt();
        auto UploadType=ReqJsonValue["UploadType"].asString();
        auto Content=ReqJsonValue["Content"].toStyledString();
        MyBasePtr->DEBUGLog("上传内容为 : " + Content, true);

        auto dbclientPrt=drogon::app().getDbClient();
        Mapper<drogon_model::novel::User>UserMgr(dbclientPrt);
        MyBasePtr->DEBUGLog("开始查询用户 : " + to_string(UserID), true);

        drogon_model::novel::User user = UserMgr.findByPrimaryKey(UserID);
        MyBasePtr->DEBUGLog("用户查询完毕 : " + user.toJson().toStyledString(), true);

        // 准备好要处理的新数据
        MyBasePtr->DEBUGLog("开始准备要处理的新数据", true);
        Mapper<drogon_model::novel::Upload>UploadMgr(dbclientPrt);
        drogon_model::novel::Upload NewUpload;
        NewUpload.setBookId(BookID);
        NewUpload.setContent(Content);
        NewUpload.setUserId(user.getValueOfUserId());
        NewUpload.setIsmanage(false);
        NewUpload.setStatus("待处理");
        MyBasePtr->DEBUGLog("准备要处理的新数据完成", true);
        MyBasePtr->DEBUGLog("要处理的新数据为 : " + NewUpload.toJson().toStyledString(), true);
        
        // 纯粹的新书，直接插入图书类即可
        if(UploadType == "new_book"){
            MyBasePtr->DEBUGLog("准备插入新数据", true);
            UploadMgr.insert(NewUpload);
            MyBasePtr->DEBUGLog("插入新数据完毕", true);

            // 准备新图书的数据
            Json::Value ContentJson = ReqJsonValue["Content"];
            {    
                MyBasePtr->DEBUGLog("开始检查新图书的数据", true);
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
                MyJsonPtr->checkMemberAndTypeInMap(ContentJson,RespVal,ColMap);
                MyBasePtr->DEBUGLog("检查新图书的数据完成", true);
            }

            MyBasePtr->DEBUGLog("开始准备新图书数据", true);
            Mapper<drogon_model::novel::Book>BookMgr(dbclientPrt);
            drogon_model::novel::Book NewBook;
            NewBook.setAuthor(ContentJson["Book_Author"].asString()); // 作者
            NewBook.setName(ContentJson["Book_Name"].asString()); // 书名
            NewBook.setPublisher(ContentJson["Book_Publisher"].asString()); // 出版方
            NewBook.setStatus("连载中"); // 状态
            NewBook.setSynopsis(ContentJson["Book_Synopsis"].asString()); // 提要
            // NewBook.setCreate(trantor::Date::now());
            // NewBook.setUpdate(trantor::Date::now());
            Json::Value MemoValue;
            MemoValue["Chapter"].append(0);
            NewBook.setMemo(MemoValue.toStyledString()); // 详细参数
            MyBasePtr->DEBUGLog("MemoValue : "+ MemoValue.toStyledString(), true);
            MyBasePtr->DEBUGLog("准备新图书数据完成", true);
            MyBasePtr->DEBUGLog("准备新图书数据为 : " + NewBook.toJson().toStyledString(), true);
        

            MyBasePtr->DEBUGLog("准备插入新图书数据", true);
            bool Need_0 = false,Need_1 = true;
            MyBasePtr->DEBUGLog("准备插入数据库语句 0 : "+NewBook.sqlForInserting(Need_0), true);
            MyBasePtr->DEBUGLog("准备插入数据库语句 1 : "+NewBook.sqlForInserting(Need_1), true);
            BookMgr.insert(NewBook);
            MyBasePtr->DEBUGLog("插入新图书数据完毕", true);


            MyBasePtr->DEBUGLog("开始更新Upload数据", true);
            NewUpload.setProcessor("System");
            NewUpload.setStatus("已通过");
            NewUpload.setIsmanage(true);
            NewUpload.setBookId(NewBook.getValueOfBookId());
            int count = UploadMgr.update(NewUpload);
            if(!count)
            {
                RespVal["ErrorMsg"] = "更新Upload数据失败";
                throw RespVal;
            }
            MyBasePtr->DEBUGLog("更新Upload数据完毕", true);
        }
        // 已存在的书上传新的章节
        else if(UploadType == "old_book_new"){
            UploadMgr.insert(NewUpload);
            MyBasePtr->DEBUGLog("插入新数据完毕", true);
            std::cout<< "插入的新数据的ID为 " << NewUpload.getValueOfUploadId() <<std::endl;

        }
        // 已存在的书更正章节
        else if(UploadType == "old_book_old"){

        }
        else{
            
        }
    }
    catch(Json::Value e)
    {
        MyBasePtr->TRACELog(e["ErrorMsg"].asString(),true);
        result=HttpResponse::newHttpJsonResponse(e);
    }
    catch(const drogon::orm::DrogonDbException &e)
    {
        RespVal["ErrorMsg"] = e.base().what();
        RespVal["DrogonDbException"] = e.base().what();
        MyBasePtr->TRACELog(RespVal["ErrorMsg"].asString(),true);
        result=HttpResponse::newHttpJsonResponse(RespVal);
    }
    catch(...)
    {
        RespVal["ErrorMsg"] = "Resource::Upload::Error";
        MyBasePtr->TRACELog(RespVal["ErrorMsg"].asString(),true);
        result=HttpResponse::newHttpJsonResponse(RespVal);
    }
    
    result=HttpResponse::newHttpJsonResponse(RespVal);

    auto resp=HttpResponse::newHttpJsonResponse(RespVal);
    resp->setStatusCode(k200OK);
    resp->setContentTypeCode(CT_TEXT_HTML);
    callback(resp);
}

// 图书下载接口
void Resource::Download(const HttpRequestPtr &req,std::function<void (const HttpResponsePtr &)> &&callback) const
{
    auto MyJsonPtr = app().getPlugin<MyJson>();
    const unordered_map<string,string>parameters = req->getParameters();
    Json::Value RespVal;
    MyJsonPtr->UnMapToJson(RespVal, parameters, "parameters");
    RespVal["简介"] = "图书下载接口";
    cout << RespVal.toStyledString() << endl;
    auto resp=HttpResponse::newHttpJsonResponse(RespVal);
    resp->setStatusCode(k200OK);
    resp->setContentTypeCode(CT_TEXT_HTML);
    callback(resp);
}

// 图书查找接口
void Resource::Search(const HttpRequestPtr &req,std::function<void (const HttpResponsePtr &)> &&callback) const
{
    auto MyJsonPtr = app().getPlugin<MyJson>();
    const unordered_map<string,string>parameters = req->getParameters();
    Json::Value RespVal;
    MyJsonPtr->UnMapToJson(RespVal, parameters, "parameters");
    RespVal["简介"] = "图书查找接口";
    cout << RespVal.toStyledString() << endl;
    auto resp=HttpResponse::newHttpJsonResponse(RespVal);
    resp->setStatusCode(k200OK);
    resp->setContentTypeCode(CT_TEXT_HTML);
    callback(resp);
}