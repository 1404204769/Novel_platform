/**
 *
 *  MyTools.cc
 *
 */

#include "MyTools.h"

using namespace drogon;

void MyTools::initAndStart(const Json::Value &config)
{
    /// Initialize and start the plugin
	try{
    	cout << "MyTools initAndStart" << endl;
		char buffer[256]; 
		char *val = getcwd(buffer, sizeof(buffer)); 
		if (val) { 
			WorkDirectory = buffer;
			std::cout << "getDocumentRoot : " << app().getDocumentRoot() << endl;
			std::cout << "当前工作目录为 " << WorkDirectory << std::endl; 
		}else{
			this->config["ErrorMsg"] = "工作目录读取失败";
			throw this->config;
		}
		cout << "开始读取config" << endl;
		readFileJson(this->config);
	}catch(Json::Value e)
	{
		std::cout << "ErrorMsg : " << e["ErrorMsg"] << std::endl;
		cout << "读取config失败" << endl;
		return;
	}
	cout << "读取config成功" << endl;
	cout << "config : " << this->config.toStyledString() << endl;
}

void MyTools::shutdown() 
{
    /// Shutdown the plugin
    cout << "MyTools shutdown" << endl;
}

// 从文件中读取数据
void MyTools::readFileJson(Json::Value &ConfigJson)
{
	Json::Reader reader;
	string openFilePath = this->WorkDirectory+"/../plugins/config.json";
	cout << "openFilePath : " << openFilePath << endl;
	//从文件中读取，保证当前文件有demo.json文件  
	ifstream in(openFilePath, ios::binary);
 
	if (!in.is_open())
	{
		ConfigJson["ErrorMsg"] = "打开配置文件失败\n";
		throw ConfigJson;
	}
 
	if (!reader.parse(in, ConfigJson))
	{
		ConfigJson["ErrorMsg"] = "Json解析错误\n" ;
		throw ConfigJson;
	}
 
	in.close();
}
void MyTools::close()
{
	try{
		cout << "准备开始保存配置文件 : " << endl;
		SaveConfig();
		cout << "准备开始停止应用" << endl;
		drogon::app().quit();
		cout << "系统是否正在运行 : " << app().isRunning() << endl;
		cout << "应用停止成功" << endl;
	}catch(Json::Value &e)
	{
		std::cout << "ErrorMsg : " << e["ErrorMsg"] << std::endl;
		cout << "写入config失败" << endl;
	}
}
bool MyTools::restart()
{
	try{
		cout << "准备开始保存配置文件 : " << endl;
		SaveConfig();
		cout << "准备开始停止应用" << endl;
		drogon::app().quit();
		cout << "应用停止成功" << endl;
		cout << "系统是否正在运行 : " << app().isRunning() << endl;
		cout << "准备开始启动应用" << endl;
		app().loadConfigFile("../config.json");
		//Run HTTP framework,the method will block in the internal event loop
		//增加响应头
		drogon::app().run();
		cout << "应用启动成功" << endl;
	}catch(Json::Value &e)
	{
		std::cout << "ErrorMsg : " << e["ErrorMsg"] << std::endl;
		cout << "写入config失败" << endl;
		return false;
	}
	return true;
}

void MyTools::SaveConfig()
{
	
	cout << "开始写入config" << endl;
	try{
		writeFileJson(this->config);
	}catch(Json::Value e)
	{
		std::cout << "ErrorMsg : " << e["ErrorMsg"] << std::endl;
		cout << "写入config失败" << endl;
		return;
	}
	cout << "写入config成功" << endl;
}

// 将数据写入文件
void MyTools::writeFileJson(Json::Value &ConfigJson)
{
	//直接输出  
	//cout << "FastWriter:" << endl;
	//Json::FastWriter fw;
	//cout << fw.write(root) << endl << endl;
 
	//缩进输出  
	cout << "StyledWriter:" << endl;
	Json::StyledWriter sw;
	cout << sw.write(ConfigJson) << endl << endl;
 
	//输出到文件  
	ofstream os;
	os.open("./config.json", std::ios::out | std::ios::app);
	if (!os.is_open())
	{
		ConfigJson["ErrorMsg"] = "无法找到或无法创建文件\"./config.json\"\n" ;
		throw ConfigJson;
	}
	os << sw.write(ConfigJson);
	os.close();
}

string MyTools::itoa_self(int i)
{
	stringstream ss;
	ss << i;
	return ss.str();
}

// 将Json字符串转为Map
map<string,string> MyTools::jsonstr2map(const string& json)
{
	Json::Reader reader;
	Json::Value value;
	map<string, string> maps;
 
	if (json.length() > 0)
	{
		if (reader.parse(json, value))
		{
			Json::Value::Members members = value.getMemberNames();
			for (Json::Value::Members::iterator it = members.begin(); it != members.end(); it++)
			{
				Json::ValueType vt = value[*it].type();
				switch (vt)
				{
				case Json::stringValue:
					{
						maps.insert(pair<string, string>(*it, value[*it].asString()));
						break;
					}
				case Json::intValue:
					{
						int intTmp = value[*it].asInt();
						maps.insert(pair<string, string>(*it, itoa_self(intTmp)));
						break;
					}
				case Json::arrayValue:
					{
						std::string strid;
						for (unsigned int i = 0; i < value[*it].size(); i++)
						{
							strid +=value[*it][i].asString();
							strid +=",";
						}
						if(!strid.empty())
						{
							strid = strid.substr(0,strid.size()-1);
						}
						maps.insert(pair<string, string>(*it, strid));
						break;
					}
				default:
					{
						break;
					}
				}//end switch
			}//end for
		}//end if
	}
 
	return maps;
}

// 检查Json成员
void MyTools::checkMember(Json::Value &ReqVal, Json::Value &RespVal, string colName)
{
	if(!ReqVal.isMember(colName))
	{
		RespVal["ErrorMsg"] = "不存在"+colName+"字段";
		throw RespVal;
	}
}

// 检查Json成员类型
void MyTools::checkColType(Json::Value &ReqVal, Json::Value &RespVal, string colName, ColType colType)
{
	switch(colType)
	{
		case ColType::STRING:
		{
			if(ReqVal[colName].isString())
				return;
			RespVal["ErrorMsg"] = colName+"字段不是String类型";

		}break;
		case ColType::INT:
		{
			if(ReqVal[colName].isInt())
				return;
			RespVal["ErrorMsg"] = colName+"字段不是Int类型";
		}break;
		case ColType::BOOL:
		{
			if(ReqVal[colName].isBool())
				return;
			RespVal["ErrorMsg"] = colName+"字段不是Bool类型";
		}break;
	}
	throw RespVal;
}

// 检查Json成员以及类型
void MyTools::checkMemberAndType(Json::Value &ReqVal, Json::Value &RespVal, string colName, ColType colType)
{
	checkMember(ReqVal,RespVal,colName);
	checkColType(ReqVal,RespVal,colName,colType);
}

// 检查Json中关于Map涉及的成员以及类型
void MyTools::checkMemberAndTypeInMap(Json::Value &ReqVal, Json::Value &RespVal, std::map<string,MyTools::ColType> &ColMap)
{
	for(auto one:ColMap)
	{
		checkMember(ReqVal,RespVal,one.first);
		checkColType(ReqVal,RespVal,one.first,one.second);
	}
}
