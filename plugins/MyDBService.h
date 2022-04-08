/**
 *
 *  MyDBService.h
 *  向上提供数据库业务接口
 */

#pragma once

#include <drogon/plugins/Plugin.h>
#include <plugins/MyBase.h>
#include <plugins/MyJson.h>
#include <plugins/MyTools.h>

#include <models/User.h>
#include <models/Upload.h>
#include <models/Book.h>
#include <models/Chapter.h>
#include <models/Note.h>

class MyDBService : public drogon::Plugin<MyDBService>
{
  public:
    MyDBService() {}
    virtual void initAndStart(const Json::Value &config) override;
    virtual void shutdown() override;
  public:
    
    void Admin_Search_Upload(Json::Value &ReqJson, Json::Value &RespJson);
    void Admin_Search_User(Json::Value &ReqJson, Json::Value &RespJson);
    void Admin_Update_Resource(Json::Value &ReqJson, Json::Value &RespJson);
    void Admin_Update_User(Json::Value &ReqJson, Json::Value &RespJson);
    
    void Auto_Examine_Chapter_Update(Json::Value &ReqJson, Json::Value &RespJson);

    void Create_Note(Json::Value &ReqJson, Json::Value &RespJson);

    void Download_Resource(Json::Value &ReqJson, Json::Value &RespJson);

    void Examine_Upload(Json::Value &ReqJson, Json::Value &RespJson);

    string Get_Upload_Type(int Upload_ID);

    void Search_Note(Json::Value &ReqJson, Json::Value &RespJson);
    void Search_User_PersonalData(Json::Value &ReqJson, Json::Value &RespJson);

    void Upload_Book(Json::Value &ReqJson, Json::Value &RespJson);
    void Upload_Chapter(Json::Value &ReqJson, Json::Value &RespJson);
    void Update_User_PersonalData(Json::Value &ReqJson, Json::Value &RespJson);


  private:
    bool Insert_Book(Json::Value &ReqJson, Json::Value &RespJson);
    bool Insert_Chapter(Json::Value &ReqJson, Json::Value &RespJson);
    bool Update_Chapter(Json::Value &ReqJson, Json::Value &RespJson);
};

