/**
 *
 *  MyJson.cc
 *
 */

#include "MyJson.h"

using namespace drogon;

void MyJson::initAndStart(const Json::Value &config)
{
    /// Initialize and start the plugin
    cout << "MyJson initAndStart" << endl;
}

void MyJson::shutdown() 
{
    /// Shutdown the plugin
    cout << "MyJson shutdown" << endl;
}


// 从文件中读取数据
void MyJson::readFileJson(Json::Value &ConfigJson,const string &openFilePath)
{
	Json::Reader reader;
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
void MyJson::SaveConfig(Json::Value &config, const string &openFilePath)
{
	
	cout << "开始写入config" << endl;
	try{
        writeFileJson(config, openFilePath);
	}catch(Json::Value e)
	{
		std::cout << "ErrorMsg : " << e["ErrorMsg"] << std::endl;
		cout << "写入config失败" << endl;
		return;
	}
	cout << "写入config成功" << endl;
}

// 将数据写入文件
void MyJson::writeFileJson(Json::Value &ConfigJson,const string &openFilePath)
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
	os.open(openFilePath, std::ios::out);
	if (!os.is_open())
	{
		ConfigJson["ErrorMsg"] = "无法找到或无法创建文件\"./config.json\"\n" ;
		throw ConfigJson;
	}
	os << sw.write(ConfigJson);
	os.close();
}

string MyJson::itoa_self(int i)
{
	stringstream ss;
	ss << i;
	return ss.str();
}

// 将Json字符串转为Map
map<string,string> MyJson::jsonstr2map(const string& json)
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
