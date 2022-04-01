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
	try{
    	cout << "MyRoot initAndStart" << endl;
        auto *MyJsonPtr = app().getPlugin<MyJson>();
		char buffer[256]; 
		char *val = getcwd(buffer, sizeof(buffer)); 
		if (val) { 
			this->WorkDirectory = buffer;
			//std::cout << "当前工作目录为 " << WorkDirectory << std::endl; 
		}else{
			this->config["ErrorMsg"] = "工作目录读取失败";
			throw this->config;
		}
		cout << "开始读取config" << endl;
		
		this->ConfigDirectory = this->WorkDirectory + "/../config";
		std::cout << "ConfigDirectory : " << this->ConfigDirectory << std::endl; 
		this->SystemConfigDirectory = this->ConfigDirectory + "/system_config.json";
		std::cout << "SystemConfigDirectory : " << this->SystemConfigDirectory << std::endl; 
		MyJsonPtr->readFileJson(this->config, this->SystemConfigDirectory);
	}catch(Json::Value e)
	{
		std::cout << "ErrorMsg : " << e["ErrorMsg"] << std::endl;
		cout << "读取config失败" << endl;
		return;
	}
	cout << "读取config成功" << endl;
	// cout << "config : " << this->config.toStyledString() << endl;
}

void MyRoot::shutdown() 
{
    /// Shutdown the plugin
    cout << "MyRoot shutdown" << endl;
}

void MyRoot::close()
{
	try{
        auto *MyJsonPtr = app().getPlugin<MyJson>();
		cout << "准备开始保存配置文件 : " << endl;
		MyJsonPtr->SaveConfig(this->config, this->SystemConfigDirectory);
		cout << "准备开始停止应用" << endl;
		app().quit();
		cout << "系统是否正在运行 : " << app().isRunning() << endl;
		cout << "应用停止成功" << endl;
	}catch(Json::Value &e)
	{
		cout << "ErrorMsg : " << e["ErrorMsg"] << endl;
		cout << "写入config失败" << endl;
	}
}

bool MyRoot::restart()
{
	try{
        auto *MyJsonPtr = app().getPlugin<MyJson>();
		cout << "准备开始保存配置文件 : " << endl;
		MyJsonPtr->SaveConfig(this->config, this->SystemConfigDirectory);
		cout << "准备开始停止应用" << endl;
		app().quit();
		cout << "应用停止成功" << endl;
		cout << "系统是否正在运行 : " << app().isRunning() << endl;
		cout << "准备开始启动应用" << endl;
		app().loadConfigFile("../config.json");
		//Run HTTP framework,the method will block in the internal event loop
		//增加响应头
		app().run();
		cout << "应用启动成功" << endl;
	}catch(Json::Value &e)
	{
		cout << "ErrorMsg : " << e["ErrorMsg"] << endl;
		cout << "写入config失败" << endl;
		return false;
	}
	return true;
}

string MyRoot::getUserType(int UserPower)
{
	try{
		Json::Value UserType = this->config["SystemBase"]["UserType"];
		cout << UserType.toStyledString() << endl;
		if(UserPower >= UserType["root"].asInt())
			return "root";
		else if(UserPower >= UserType["admin"].asInt())
			return "admin";
		else if(UserPower >= UserType["user"].asInt())
			return "user";	
	}
	catch(...){
		Json::Value JsonValue;
		JsonValue["ErrorMsg"] = "获取用户类型异常，请联系管理员";
		throw JsonValue;
	}
	return "error";
}

