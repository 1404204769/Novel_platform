#include "Test.h"
//add definition of your processing function here
void Test::ChineseStr(const HttpRequestPtr &req,std::function<void (const HttpResponsePtr &)> &&callback) const
{
	auto *MyToolPtr = app().getPlugin<MyTools>();//获取MyJson插件
	auto *MyBasePtr = app().getPlugin<MyBase>();//获取MyJson插件
    MyBasePtr->TRACELog("req body" + string(req->getBody()),true);
	Json::Value RespVal;
	auto jsonptr=req->getJsonObject();
    Json::Value json=*jsonptr;
    pair<int,string> TitleVal = MyToolPtr->getTitleNumAndTitleStr(json["Title"].asString());
    RespVal["TitleNum"] = TitleVal.first;
    RespVal["TitleStr"] = TitleVal.second;
    if(MyBasePtr->IsStatus("DEBUG"))
    {
        cout << "TitleNum : " << RespVal["TitleNum"].asString() << endl;
        cout << "TitleStr : " << RespVal["TitleStr"].asString() << endl;
    }
    drogon::HttpResponsePtr result=HttpResponse::newHttpJsonResponse(RespVal);

    result->setStatusCode(k200OK);
    result->setContentTypeCode(CT_TEXT_HTML);
    callback(result);
}

// 修改系统输出级别
void Test::SysOutLevel(const HttpRequestPtr &req,std::function<void (const HttpResponsePtr &)> &&callback) const
{
	auto *MyBasePtr = app().getPlugin<MyBase>();//获取MyJson插件
    MyBasePtr->TRACELog("req body" + string(req->getBody()),true);
	Json::Value RespVal;
	auto jsonptr=req->getJsonObject();
    Json::Value json=*jsonptr;
    string ChangeLevel = json["Level"].asString();
    MyBasePtr->ChangeStatus(ChangeLevel);
    drogon::HttpResponsePtr result=HttpResponse::newHttpJsonResponse(RespVal);

    result->setStatusCode(k200OK);
    result->setContentTypeCode(CT_TEXT_HTML);
    callback(result);
}