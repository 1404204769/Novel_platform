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
    auto MyBasePtr = drogon::app().getPlugin<MyBase>();
    MyBasePtr->TRACELog("MyJson initAndStart",true);
}

void MyJson::shutdown() 
{
    /// Shutdown the plugin
    auto MyBasePtr = drogon::app().getPlugin<MyBase>();
    MyBasePtr->TRACELog("MyJson shutdown",true);
}


// 从文件中读取数据
void MyJson::readFileJson(Json::Value &ConfigJson,const string &openFilePath)
{
	Json::Reader reader;
    auto MyBasePtr = drogon::app().getPlugin<MyBase>();
	//从文件中读取，保证当前文件有demo.json文件  
    MyBasePtr->DEBUGLog("openFilePath : " + openFilePath,true);
	ifstream in(openFilePath, ios::binary);
 
	if (!in.is_open())
	{
		ConfigJson["ErrorMsg"].append("打开配置文件失败");
    	MyBasePtr->TRACELog("MyJson::readFileJson::Error : 打开配置文件失败",true);
		throw ConfigJson;
	}
 
	if (!reader.parse(in, ConfigJson))
	{
		ConfigJson["ErrorMsg"].append("Json解析错误");
    	MyBasePtr->TRACELog("MyJson::readFileJson::Error : Json解析错误",true);
		throw ConfigJson;
	}
 
	in.close();
}

void MyJson::SaveConfig(Json::Value &config, const string &openFilePath)
{
    auto MyBasePtr = drogon::app().getPlugin<MyBase>();
	MyBasePtr->TRACELog("开始写入config",true);
	try{
        writeFileJson(config, openFilePath);
	}catch(Json::Value &e)
	{
		MyBasePtr->TRACELog("写入config失败",true);
		MyBasePtr->TRACE_ERROR(e["ErrorMsg"]);
		return;
	}
	MyBasePtr->TRACELog("写入config成功",true);
}

// 将数据写入文件
void MyJson::writeFileJson(Json::Value &ConfigJson,const string &openFilePath)
{
	Json::StyledWriter sw;
    auto MyBasePtr = drogon::app().getPlugin<MyBase>();
	if(MyBasePtr->IsStatus("DEBUG"))
	{
		//直接输出  
		cout << "FastWriter:" << endl;
		Json::FastWriter fw;
		cout << fw.write(ConfigJson) << endl << endl;
	
		//缩进输出  
		cout << "StyledWriter:" << endl;
		cout << sw.write(ConfigJson) << endl << endl;
	}

	//输出到文件  
	ofstream os;
	os.open(openFilePath, std::ios::out);
	if (!os.is_open())
	{
		ConfigJson["ErrorMsg"].append("无法找到或无法创建文件\"./config.json\"\n");
		MyBasePtr->TRACELog("MyJson::writeFileJson::Error : 无法找到或无法创建文件\"./config.json\"\n",true);
		throw ConfigJson;
	}
	os << sw.write(ConfigJson);
	os.close();
}

// 将Json字符串转为Map
map<string,string> MyJson::jsonstr2map(const string& JsonStr)
{
	Json::Reader reader;
	Json::Value value;
	map<string, string> maps;
	
	if (JsonStr.length() > 0)
	{
		if (reader.parse(JsonStr, value))
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
						maps.insert(pair<string, string>(*it, to_string(intTmp)));
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

// 将Map转为Json
void MyJson::MapToJson(Json::Value &JsonValue, map<string,string>& map_info,const string &DataName)
{
	Json::Value JObject;
    auto MyBasePtr = drogon::app().getPlugin<MyBase>();

	try{
		for (auto iter = map_info.begin(); iter != map_info.end(); ++iter)
		{
			JObject[iter->first] = iter->second;
		}
	}catch(...)
	{
		JsonValue["ErrorMsg"].append("Map 转为 Json 失败");
    	MyBasePtr->TRACELog("MyJson::MapToJson::Error : Map 转为 Json 失败",true);

		throw JsonValue;
	}
	JsonValue[DataName] = JObject;
}

// 将UnMap转为Json
void MyJson::UnMapToJson(Json::Value &JsonValue,const unordered_map<string,string>& umap_info,const string &DataName)
{
	Json::Value JObject;
    auto MyBasePtr = drogon::app().getPlugin<MyBase>();
	try{
		for (auto iter = umap_info.begin(); iter != umap_info.end(); ++iter)
		{
			JObject[iter->first] = iter->second;
		}
	}catch(...)
	{
		JsonValue["ErrorMsg"].append("unordered_map 转为 Json 失败");
    	MyBasePtr->TRACELog("MyJson::UnMapToJson::Error : unordered_map 转为 Json 失败",true);
		throw JsonValue;
	}
	JsonValue[DataName] = JObject;
}

// 将Json字符串转为Json::Value
void MyJson::JsonstrToJson(Json::Value &JsonValue,const string& JsonStr)
{
	Json::Reader reader;
	Json::Value value;
    auto MyBasePtr = drogon::app().getPlugin<MyBase>();
	bool ret = reader.parse(JsonStr, JsonValue);
	if(!ret)
	{
		JsonValue["ErrorMsg"].append("Json字符串解析出错");
    	MyBasePtr->TRACELog("MyJson::JsonstrToJson::Error : Json字符串解析出错",true);
		throw JsonValue;
	}
}


// 检查Json成员
/*
	检查Json成员是否存在
Resp:{
	"ErrorMsg" 	: 	[],// 错误信息
	"Result"	:	true/false,//执行结果
}
*/
void MyJson::checkMember(Json::Value &ReqVal, Json::Value &RespVal, string colName)
{
    auto MyBasePtr = drogon::app().getPlugin<MyBase>();
	RespVal["Result"] = false;
	if(!ReqVal.isMember(colName))
	{
		RespVal["ErrorMsg"].append("不存在"+colName+"字段");
    	MyBasePtr->TRACELog("MyJson::checkMember::Error : 不存在"+colName+"字段",true);
		throw RespVal;
	}
	RespVal["Result"] = true;
}

// 检查Json成员类型
/*
	检查Json成员类型
Resp:{
	"ErrorMsg" 	: 	[],// 错误信息
	"Result"	:	true/false,//执行结果
}
*/
void MyJson::checkColType(Json::Value &ReqVal, Json::Value &RespVal, string colName, ColType colType)
{
    auto MyBasePtr = drogon::app().getPlugin<MyBase>();
	RespVal["Result"] = true;
	switch(colType)
	{
		case ColType::STRING:
		{
			if(ReqVal[colName].isString())
				return;
			RespVal["ErrorMsg"].append(colName+"字段不是String类型");
    		MyBasePtr->TRACELog("MyJson::checkColType::Error : "+colName+"字段不是String类型",true);

		}break;
		case ColType::INT:
		{
			if(ReqVal[colName].isInt())
				return;
			RespVal["ErrorMsg"].append(colName+"字段不是Int类型");
    		MyBasePtr->TRACELog("MyJson::checkColType::Error : "+colName+"字段不是Int类型",true);
		}break;
		case ColType::BOOL:
		{
			if(ReqVal[colName].isBool())
				return;
			RespVal["ErrorMsg"].append(colName+"字段不是Bool类型");
    		MyBasePtr->TRACELog("MyJson::checkColType::Error : "+colName+"字段不是Bool类型",true);
		}break;
		case ColType::JSON:
		{
			if(ReqVal[colName].isObject() && !ReqVal[colName].isNull())
				return;
			RespVal["ErrorMsg"].append(colName+"字段不是Object类型");
    		MyBasePtr->TRACELog("MyJson::checkColType::Error : "+colName+"字段不是Object类型",true);
		}break;
		case ColType::ARRAY:
		{
			if(ReqVal[colName].isArray())
				return;
			RespVal["ErrorMsg"].append(colName+"字段不是Array类型");
    		MyBasePtr->TRACELog("MyJson::checkColType::Error : "+colName+"字段不是Array类型",true);
		}break;
	}
	RespVal["Result"] = false;
	throw RespVal;
}

// 检查Json成员以及类型
/*
	检查Json成员以及类型
Resp:{
	"ErrorMsg" 	: 	[],// 错误信息
	"Result"	:	true/false,//执行结果
}
*/
void MyJson::checkMemberAndType(Json::Value &ReqVal, Json::Value &RespVal, string colName, ColType colType)
{
	checkMember(ReqVal,RespVal,colName);
	checkColType(ReqVal,RespVal,colName,colType);
}

// 检查Json中关于Map涉及的成员以及类型
/*
	检查Json中关于Map涉及的成员以及类型
Resp:{
	"ErrorMsg" 	: 	[],// 错误信息
	"Result"	:	true/false,//执行结果
}
*/
void MyJson::checkMemberAndTypeInMap(Json::Value &ReqVal, Json::Value &RespVal, std::map<string,ColType> &ColMap)
{
	for(auto one:ColMap)
	{
		checkMember(ReqVal,RespVal,one.first);
		checkColType(ReqVal,RespVal,one.first,one.second);
	}
}


