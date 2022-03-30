#include "Admin_Economics.h"
using namespace Admin;
void Economics::asyncHandleHttpRequest(const HttpRequestPtr& req, std::function<void (const HttpResponsePtr &)> &&callback)
{
    //write your application logic here
    auto MyJsonPtr = app().getPlugin<MyJson>();
    const unordered_map<string,string>parameters = req->getParameters();
    Json::Value RespVal;
    MyJsonPtr->UnMapToJson(RespVal, parameters, "parameters");
    RespVal["简介"] = "管理员查看系统经济报表接口";
    cout << RespVal.toStyledString() << endl;
    auto resp=HttpResponse::newHttpJsonResponse(RespVal);
    resp->setStatusCode(k200OK);
    resp->setContentTypeCode(CT_TEXT_HTML);
    callback(resp);
}