/**
 *
 *  MyRoot.h
 *
 */

#pragma once

#include <drogon/plugins/Plugin.h>
#include <plugins/MyTools.h>
#include <plugins/MyJson.h>
using namespace std;
class MyRoot : public drogon::Plugin<MyRoot>
{
  public:
    MyRoot() {}
    /// This method must be called by drogon to initialize and start the plugin.
    /// It must be implemented by the user.
    virtual void initAndStart(const Json::Value &config) override;

    /// This method must be called by drogon to shutdown the plugin.
    /// It must be implemented by the user.
    virtual void shutdown() override;
    
    enum Instructions{
      RESTART = 1,
    };
  public:
    bool restart();
    void close();
  private:
    Json::Value config;
    string WorkDirectory;
    string ConfigDirectory;
    string SystemConfigDirectory;
};

