/**
 *
 *  MyTools.h
 *
 */

#pragma once

#include <drogon/plugins/Plugin.h>
#include <drogon/drogon.h>
#include <plugins/MyBase.h>
#include <locale>
#include <codecvt>
using namespace std;
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
    wstring StringToWString(const string& str);
    string WStringToString(const wstring& wstr);
    pair<int,string> getTitleNumAndTitleStr(const string &Title);
  private:
    vector<string> SpiteStringCharacter(const string &ChineseStr);
    int ChineseNumToAlabNum(const vector<string> &ChineseNum);
    vector<string> NumberOfChaptersExtracted(vector<string> &Title);
    string TitleOfChaptersExtracted(vector<string> &Title);
};

