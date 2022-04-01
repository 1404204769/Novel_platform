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
    vector<string> Title = MyToolPtr->SpiteStringCharacter(json["context"].asString());
    for(auto &a:Title)
    {
        RespVal["Title"].append(a);
    }
    vector<string> Extracted = MyToolPtr->NumberOfChaptersExtracted(Title);
    for(auto &a:Extracted)
    {
        RespVal["Extracted"].append(a);
    }
    int AlabNum = MyToolPtr->ChineseNumToAlabNum(Extracted);
    RespVal["AlabNum"] = AlabNum;
    if(MyBasePtr->IsStatus("DEBUG"))
    {
        cout << "Title : ";
        for(auto &a:Title)
            cout << a << " ";
        cout << endl;
        cout << "Extracted : ";
        for(auto &a:Extracted)
            cout << a << " ";
        cout << endl;
        cout << "AlabNum : " << to_string(AlabNum) << endl;
    }
    drogon::HttpResponsePtr result=HttpResponse::newHttpJsonResponse(RespVal);

    result->setStatusCode(k200OK);
    result->setContentTypeCode(CT_TEXT_HTML);
    callback(result);
}

// 修改系统输出级别
void Test::SysOutLevel(const HttpRequestPtr &req,std::function<void (const HttpResponsePtr &)> &&callback) const
{
    std::cout<< "req body" << req->getBody()<<std::endl;
	auto *MyBasePtr = app().getPlugin<MyBase>();//获取MyBase插件
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