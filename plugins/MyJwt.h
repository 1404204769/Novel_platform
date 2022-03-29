/**
 *
 *  MyJwt.h
 *
 */

#pragma once

#include <drogon/plugins/Plugin.h>
#include <drogon/drogon.h>
#include <plugins/MyTools.h>
#include <dependence/cpp-jwt/include/jwt/jwt.hpp>
using namespace std;
class MyJwt : public drogon::Plugin<MyJwt>
{
  public:
    MyJwt() {}
    /// This method must be called by drogon to initialize and start the plugin.
    /// It must be implemented by the user.
    virtual void initAndStart(const Json::Value &config) override;

    /// This method must be called by drogon to shutdown the plugin.
    /// It must be implemented by the user.
    virtual void shutdown() override;

  public:
    string      encode(const Json::Value &param);
    Json::Value decode(const string &token);
    bool        verify(const string &token);

  private:
    string secret;
};

