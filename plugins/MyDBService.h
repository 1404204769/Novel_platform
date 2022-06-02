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
#include <plugins/MyRoot.h>

#include <models/Action.h>
#include <models/Agree.h>
#include <models/Book.h>
#include <models/Chapter.h>
#include <models/Comment.h>
#include <models/Note.h>
#include <models/User.h>
#include <models/Upload.h>
#include <models/Idea.h>

class MyDBService : public drogon::Plugin<MyDBService>
{
  public:
    MyDBService() {}
    virtual void initAndStart(const Json::Value &config) override;
    virtual void shutdown() override;
  public:
    
    void Admin_Search_Idea(Json::Value &ReqJson, Json::Value &RespJson);
    void Admin_Search_Upload(Json::Value &ReqJson, Json::Value &RespJson);
    void Admin_Search_User(Json::Value &ReqJson, Json::Value &RespJson);
    void Admin_Update_Resource(Json::Value &ReqJson, Json::Value &RespJson);
    void Admin_Update_User(Json::Value &ReqJson, Json::Value &RespJson);
    
    
    void Agree(Json::Value &ReqJson, Json::Value &RespJson);

    void Auto_Examine_Chapter_Update(Json::Value &ReqJson, Json::Value &RespJson);

    bool Change_User_Integral(Json::Value &ReqJson, Json::Value &RespJson);
    bool Change_Book_Status(Json::Value &ReqJson, Json::Value &RespJson);
    bool Change_User_Status(Json::Value &ReqJson, Json::Value &RespJson);
    bool Change_Idea_Status(Json::Value &ReqJson, Json::Value &RespJson);

    void Create_Comment(Json::Value &ReqJson, Json::Value &RespJson);
    void Create_Note(Json::Value &ReqJson, Json::Value &RespJson);
    void Create_Suggestion(Json::Value &ReqJson, Json::Value &RespJson);
    void Create_Report(Json::Value &ReqJson, Json::Value &RespJson);

    
    void Download_Resource_Public(Json::Value &ReqJson, Json::Value &RespJson);

    void Examine_Upload(Json::Value &ReqJson, Json::Value &RespJson);

    string Get_Upload_Type(int Upload_ID);
    bool Is_Book_Exist(Json::Value &ReqJson, Json::Value &RespJson);
    bool Is_Comment_Exist(Json::Value &ReqJson, Json::Value &RespJson);
    bool Is_CommentID_Exist(Json::Value &ReqJson, Json::Value &RespJson);
    bool Is_Note_Exist(Json::Value &ReqJson, Json::Value &RespJson);
    bool Is_User_Exist(Json::Value &ReqJson, Json::Value &RespJson);

    void Log_Resource_Upload(const int &User_ID,const int &Upload_ID,const string &Upload_Type,const string &Explain);


    void Read_Resource(Json::Value &ReqJson, Json::Value &RespJson);
    void Recharge(Json::Value &ReqJson, Json::Value &RespJson);

    void Search_Action_By_UserID(Json::Value &ReqJson, Json::Value &RespJson);
    void Search_Agree(Json::Value &ReqJson, Json::Value &RespJson);
    void Search_Resource_Action_Count(Json::Value &ReqJson, Json::Value &RespJson);
    void Search_Book(Json::Value &ReqJson, Json::Value &RespJson);
    void Search_Book_Accuracy(Json::Value &ReqJson, Json::Value &RespJson);
    void Search_Book_By_BookID(Json::Value &ReqJson, Json::Value &RespJson);
    void Search_BookMenu_By_BookIDAndName(Json::Value &ReqJson, Json::Value &RespJson);
    void Search_Comment(Json::Value &ReqJson, Json::Value &RespJson);
    void Search_Comment_By_CommentID(Json::Value &ReqJson, Json::Value &RespJson);
    void Search_Chapter_All_Version(Json::Value &ReqJson, Json::Value &RespJson);
    void Search_Chapter_By_BookID(Json::Value &ReqJson, Json::Value &RespJson);
    void Search_Chapter_By_ChapterID(Json::Value &ReqJson, Json::Value &RespJson);
    void Search_ChapterContent(Json::Value &ReqJson, Json::Value &RespJson);
    void Search_Note(Json::Value &ReqJson, Json::Value &RespJson);
    void Search_Note_By_NoteID(Json::Value &ReqJson, Json::Value &RespJson);
    void Search_User_Ban_Time(Json::Value &ReqJson, Json::Value &RespJson);
    void Search_User_PersonalData(Json::Value &ReqJson, Json::Value &RespJson);
    void Search_Upload_By_UploadID(Json::Value &ReqJson, Json::Value &RespJson);

    void Select_Chapter_Version_Valid(Json::Value &ReqJson, Json::Value &RespJson);



    void Upload_Book(Json::Value &ReqJson, Json::Value &RespJson);
    void Upload_Chapter(Json::Value &ReqJson, Json::Value &RespJson);
    void Update_User_PersonalData(Json::Value &ReqJson, Json::Value &RespJson);



    void ParseAction(Json::Value &Resp,const Json::Value &Action);
  private:
    bool Download_Resource_Private(Json::Value &ReqJson, Json::Value &RespJson);

    bool Insert_Action(Json::Value &ReqJson, Json::Value &RespJson);
    bool Insert_Book(Json::Value &ReqJson, Json::Value &RespJson);
    bool Insert_Chapter(Json::Value &ReqJson, Json::Value &RespJson);
    void Insert_Idea(Json::Value &ReqJson, Json::Value &RespJson);
    bool Update_Chapter(Json::Value &ReqJson, Json::Value &RespJson);
    bool Update_User(Json::Value &ReqJson, Json::Value &RespJson);
};

