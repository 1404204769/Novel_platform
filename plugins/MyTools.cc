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
    cout << "MyTools initAndStart" << endl;
}

void MyTools::shutdown() 
{
    /// Shutdown the plugin
    cout << "MyTools shutdown" << endl;
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
