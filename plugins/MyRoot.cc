/**
 *
 *  MyRoot.cc
 *
 */

#include "MyRoot.h"

using namespace drogon;

void MyRoot::initAndStart(const Json::Value &config)
{
    /// Initialize and start the plugin
    auto MyBasePtr = drogon::app().getPlugin<MyBase>();
	try{
    	MyBasePtr->TRACELog("MyRoot initAndStart",true);
        auto *MyJsonPtr = app().getPlugin<MyJson>();
		char buffer[256]; 
		char *val = getcwd(buffer, sizeof(buffer)); 
		if (val) { 
			this->WorkDirectory = buffer;
			MyBasePtr->DEBUGLog("当前工作目录为 " + WorkDirectory,true);
		}else{
			this->config["ErrorMsg"].append("工作目录读取失败");
			throw this->config;
		}
		MyBasePtr->TRACELog("开始读取config",true);
		
		this->ConfigDirectory = this->WorkDirectory + "/../config";
		MyBasePtr->DEBUGLog("ConfigDirectory  : " + WorkDirectory,true);
		this->SystemConfigDirectory = this->ConfigDirectory + "/system_config.json";
		MyBasePtr->DEBUGLog("SystemConfigDirectory  : " + WorkDirectory,true);
		MyJsonPtr->readFileJson(this->config, this->SystemConfigDirectory);
	}catch(Json::Value e)
	{
    	MyBasePtr->TRACELog("MyRoot::initAndStart::Error:",true);
		MyBasePtr->TRACE_ERROR( e["ErrorMsg"]);
		return;
	}
	MyBasePtr->TRACELog("读取config成功",true);
	MyBasePtr->DEBUGLog("config : " + this->config.toStyledString(),true);
}

void MyRoot::shutdown() 
{
    /// Shutdown the plugin
    auto MyBasePtr = drogon::app().getPlugin<MyBase>();
    MyBasePtr->TRACELog("MyRoot shutdown",true);
}

void MyRoot::close()
{
    auto MyBasePtr = drogon::app().getPlugin<MyBase>();
	try{
        auto *MyJsonPtr = app().getPlugin<MyJson>();
    	MyBasePtr->TRACELog("准备开始保存配置文件",true);
		MyJsonPtr->SaveConfig(this->config, this->SystemConfigDirectory);
    	MyBasePtr->TRACELog("准备开始停止应用",true);
		app().quit();
    	MyBasePtr->TRACELog("系统是否正在运行 : " + app().isRunning(),true);
    	MyBasePtr->TRACELog("应用停止成功",true);
	}catch(Json::Value &e)
	{
    	MyBasePtr->TRACELog("MyRoot::close::Error : ",true);
		MyBasePtr->TRACE_ERROR(e["ErrorMsg"]);
	}
}

bool MyRoot::restart()
{
    auto MyBasePtr = drogon::app().getPlugin<MyBase>();
	try{
        auto *MyJsonPtr = app().getPlugin<MyJson>();
    	MyBasePtr->TRACELog("准备开始保存配置文件",true);
		MyJsonPtr->SaveConfig(this->config, this->SystemConfigDirectory);
    	MyBasePtr->TRACELog("准备开始停止应用",true);
		app().quit();
    	MyBasePtr->TRACELog("应用停止成功",true);
    	MyBasePtr->TRACELog("系统是否正在运行 : " + app().isRunning(),true);
    	MyBasePtr->TRACELog("准备开始启动应用",true);
		app().loadConfigFile("../config.json");
		//Run HTTP framework,the method will block in the internal event loop
		//增加响应头
		app().run();
    	MyBasePtr->TRACELog("应用启动成功",true);
	}catch(Json::Value &e)
	{
    	MyBasePtr->TRACELog("MyRoot::restart::Error : ",true);
		MyBasePtr->TRACE_ERROR(e["ErrorMsg"]);
		return false;
	}
	return true;
}

string MyRoot::getUserType(int UserPower)
{
    auto MyBasePtr = drogon::app().getPlugin<MyBase>();
	try{
		Json::Value UserType = this->config["SystemBase"]["UserType"];
    	MyBasePtr->DEBUGLog(UserType.toStyledString(),true);
		if(UserPower >= UserType["root"].asInt())
			return "root";
		else if(UserPower >= UserType["admin"].asInt())
			return "admin";
		else if(UserPower >= UserType["user"].asInt())
			return "user";	
	}
	catch(...){
		Json::Value JsonValue;
		JsonValue["ErrorMsg"].append("获取用户类型异常，请联系管理员");
    	MyBasePtr->TRACELog("MyRoot::getUserType::Error : 获取用户类型异常，请联系管理员",true);
		throw JsonValue;
	}
	return "error";
}

