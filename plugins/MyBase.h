/**
 *
 *  MyBase.h
 *
 */
#ifndef _MYBASE_H
#define _MYBASE_H

#pragma once

#include <drogon/plugins/Plugin.h>
#include <drogon/drogon.h>
#include <string>
using namespace std;
class MyBase : public drogon::Plugin<MyBase>
{
  public:
    MyBase() {}
    /// This method must be called by drogon to initialize and start the plugin.
    /// It must be implemented by the user.
    virtual void initAndStart(const Json::Value &config) override;

    /// This method must be called by drogon to shutdown the plugin.
    /// It must be implemented by the user.
    virtual void shutdown() override;
  private:
    bool TRACE = true;
    bool DEBUG = true;
    bool INFO = true;
    bool WARN = false;
  public:
    void TRACELog(const string &str,const  bool Line_feed);
    void DEBUGLog(const string &str,const  bool Line_feed);
    void INFOLog(const string &str,const  bool Line_feed);
    void WARNLog(const string &str,const  bool Line_feed);
    void INFO_Mark(const string &Func_Name);
    void INFO_Func(const string &Func_Name,const bool &Is_Req,Json::Value &JsonVal);
    void ChangeStatus(const string &level);
    bool IsStatus(const string &level);
    void TRACE_ERROR(Json::Value &ERROR);
    void printLine(const string Str);
};

#endif