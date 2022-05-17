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
	getLevelList();
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
		//
		pid_t thread = fork();
		if(thread == 0)
		{
			cout << "系统延迟1s" << endl;
			usleep(1000);
			cout << "系统延迟完成" << endl;
			system("sh ../start.sh close");
		}
		//app().quit();
    	MyBasePtr->TRACELog("系统是否正在运行 : " + app().isRunning(),true);
    	MyBasePtr->TRACELog("应用停止成功",true);
	}catch(Json::Value &e)
	{
    	MyBasePtr->TRACELog("MyRoot::close::Error : ",true);
		MyBasePtr->TRACE_ERROR(e["ErrorMsg"]);
	}
}

void MyRoot::restart()
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
		//app().loadConfigFile("../config.json");
		//Run HTTP framework,the method will block in the internal event loop
		//增加响应头
		//app().run();
    	MyBasePtr->TRACELog("应用启动成功",true);
	}catch(Json::Value &e)
	{
    	MyBasePtr->TRACELog("MyRoot::restart::Error : ",true);
		MyBasePtr->TRACE_ERROR(e["ErrorMsg"]);
	}
}

/*获取用户权限代表的用户类型*/
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

/*获取用户等级相关配置*/
Json::Value MyRoot::getUserLevelConfig()
{
	return this->config["User"]["Level"];
}

/*获取等级配置列表*/
Json::Value MyRoot::getLevelList()
{
	if(!LevelList.isNull())
	{
		return LevelList;
	}
	cout << "开始编译LevelList" << endl;
	Json::Value LevelConfig = getUserLevelConfig();
	Json::Value TLevel;
	int Level = 0 , Power = 0, Integral = 0,Totle_Book = 0,Chapter_Application = 0,Totle_Integral = 0;
	// 自动生成当前等级配置
	for(int i = 1;i <= LevelConfig["Max_Level"].asInt();i++)
	{
		if(i == 1)
		{
			Level=LevelConfig["Init"]["Level"].asInt();
			Power=LevelConfig["Init"]["Power"].asInt();
			Integral=LevelConfig["Init"]["Integral"].asInt();
			Totle_Book=LevelConfig["Init"]["Totle_Book"].asInt();
			Chapter_Application=LevelConfig["Init"]["Chapter_Application"].asInt();
		}
		else
		{
		 	Level++;
			Power += LevelConfig["Incremental_Per_LevelUp"]["Power"].asInt();
			Integral += LevelConfig["Incremental_Per_LevelUp"]["Integral"].asInt();
			Totle_Book += LevelConfig["Incremental_Per_LevelUp"]["Totle_Book"].asInt();
			Chapter_Application += LevelConfig["Incremental_Per_LevelUp"]["Chapter_Application"].asInt();
		}
		TLevel.clear();
		TLevel["Level"] = Level;
		TLevel["Power"] = Power;
		TLevel["Integral"] = Totle_Integral + Integral;
		TLevel["Totle_Book"] = Totle_Book;
		TLevel["Chapter_Application"] = Chapter_Application;
		LevelList.append(TLevel);
		Totle_Integral = TLevel["Integral"].asInt();
	}
	return LevelList;
}

/*获取初始等级的属性*/
Json::Value MyRoot::getInitLevelConfig()
{
	Json::Value data;
	getLevelList();
	for(int i =0 ;i<LevelList.size();i++)
	{
		if(LevelList[i]["Level"] == 1)
		{
			data = LevelList[i];
			break;
		}
	}
	return data;
}

/*根据当前总积分判断所处的等级*/
Json::Value MyRoot::getCurrentLevelConfig(int TotalNum)
{
	Json::Value data;
	getLevelList();
	for(int i =0 ;i<LevelList.size();i++)
	{
		if(LevelList[i]["Integral"] > TotalNum)
		{
			if(i == 0)break;
			data = LevelList[i-1];
			break;
		}
	}
	return data;
}

Json::Value MyRoot::getIntegralConfig()
{
	return this->config["Integral"];
}
void MyRoot::ChangeSysConfig(Json::Value json)
{
	this->config["Integral"]["Upload"] = json["Integral"]["Upload"].asInt();
	this->config["Integral"]["Download"] = json["Integral"]["Download"].asInt();
	this->config["Integral"]["Recharge"] = json["Integral"]["Recharge"].asInt();
	this->config["User"]["Level"]["Init"]["Level"] = json["Level"]["Init"]["Level"].asInt();
	this->config["User"]["Level"]["Init"]["Power"] = json["Level"]["Init"]["Power"].asInt();
	this->config["User"]["Level"]["Init"]["Integral"] = json["Level"]["Init"]["Integral"].asInt();
	this->config["User"]["Level"]["Init"]["Totle_Book"] = json["Level"]["Init"]["Totle_Book"].asInt();
	this->config["User"]["Level"]["Init"]["Chapter_Application"] = json["Level"]["Init"]["Chapter_Application"].asInt();
	this->config["User"]["Level"]["Incremental_Per_LevelUp"]["Power"] = json["Level"]["Incremental_Per_LevelUp"]["Power"].asInt();
	this->config["User"]["Level"]["Incremental_Per_LevelUp"]["Integral"] = json["Level"]["Incremental_Per_LevelUp"]["Integral"].asInt();
	this->config["User"]["Level"]["Incremental_Per_LevelUp"]["Totle_Book"] = json["Level"]["Incremental_Per_LevelUp"]["Totle_Book"].asInt();
	this->config["User"]["Level"]["Incremental_Per_LevelUp"]["Chapter_Application"] = json["Level"]["Incremental_Per_LevelUp"]["Chapter_Application"].asInt();
    this->config["User"]["Level"]["Max_Level"] = json["Level"]["Max_Level"].asInt();
	this->LevelList.clear();// 清空以便其他函数获取最新配置
}