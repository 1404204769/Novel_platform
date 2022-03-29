/**
 *
 *  MyTools.h
 *
 */

#pragma once

#include <drogon/plugins/Plugin.h>
#include <drogon/drogon.h>
#include <models/Action.h>
#include <models/Agree.h>
#include <models/Book.h>
#include <models/Chapter.h>
#include <models/Comment.h>
#include <models/Idea.h>
#include <models/Note.h>
#include <models/Upload.h>
#include <models/User.h>
#include <sstream>
#include <fstream>
#include <unistd.h>
using namespace std;
using namespace drogon_model::novel;
class MyTools : public drogon::Plugin<MyTools>
{
  public:
    MyTools() {}
    /// This method must be called by drogon to initialize and start the plugin.
    /// It must be implemented by the user.
    virtual void initAndStart(const Json::Value &config) override;

    /// This method must be called by drogon to shutdown the plugin.
    /// It must be implemented by the user.
    virtual void shutdown() override;
  public:
    enum ColType{
      STRING,INT,BOOL
    };
    enum Instructions{
      RESTART = 1,
    };
  public:
    string itoa_self(int i);
    map<string,string> jsonstr2map(const string& json);
    void SaveConfig();
    void readFileJson(Json::Value &ConfigJson);
    void writeFileJson(Json::Value &ConfigJson);
    void checkMember(Json::Value &ReqVal, Json::Value &RespVal, string colName);
    void checkColType(Json::Value &ReqVal, Json::Value &RespVal, string colName, ColType colType);
    void checkMemberAndType(Json::Value &ReqVal, Json::Value &RespVal, string colName, ColType colType);
    void checkMemberAndTypeInMap(Json::Value &ReqVal, Json::Value &RespVal, std::map<string,MyTools::ColType> &ColMap);
    bool restart();
    void close();
  private:
    Json::Value config;
    string WorkDirectory;
};

