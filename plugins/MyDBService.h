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

class MyDBService : public drogon::Plugin<MyDBService>
{
  public:
    MyDBService() {}
    virtual void initAndStart(const Json::Value &config) override;
    virtual void shutdown() override;
  public:
    void User_Upload_New_Book(Json::Value &ReqJson, Json::Value &RespJson);
    void User_Upload_Exist_Book_New_Chapter(Json::Value &ReqJson, Json::Value &RespJson);
  private:

};
