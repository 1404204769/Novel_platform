/**
 *
 *  MyJson.h
 *
 */

#pragma once

#include <drogon/plugins/Plugin.h>
#include <sstream>
#include <fstream>
#include <unistd.h>

using namespace std;
class MyJson : public drogon::Plugin<MyJson>
{
  public:
    MyJson() {}
    /// This method must be called by drogon to initialize and start the plugin.
    /// It must be implemented by the user.
    virtual void initAndStart(const Json::Value &config) override;

    /// This method must be called by drogon to shutdown the plugin.
    /// It must be implemented by the user.
    virtual void shutdown() override;
  public:
    map<string,string> jsonstr2map(const string& JsonStr);
    void MapToJson(Json::Value &JsonValue, map<string,string>& map_info,const string &DataName);
    void UnMapToJson(Json::Value &JsonValue,const unordered_map<string,string>& umap_info,const string &DataName);
    void JsonstrToJson(Json::Value &JsonValue,const string& JsonStr);
    void SaveConfig(Json::Value &config, const string &openFilePath);
    void readFileJson(Json::Value &ConfigJson,const string &openFilePath);
    void writeFileJson(Json::Value &ConfigJson,const string &openFilePath);
};

