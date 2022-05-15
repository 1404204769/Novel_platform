#include "Root.h"
//add definition of your processing function here

void Root::Instructions(const HttpRequestPtr &req,std::function<void (const HttpResponsePtr &)> &&callback) const
{
    Json::Value ReqVal, RespVal,ResultData;
    drogon::HttpResponsePtr Result;
    auto MyBasePtr = app().getPlugin<MyBase>();
    auto MyJsonPtr = app().getPlugin<MyJson>();
    auto *MyRootPtr = app().getPlugin<MyRoot>();
    const unordered_map<string, string> umapPara = req->getParameters();
    MyBasePtr->TRACELog("Root::Instructions::body" + string(req->getBody()), true);


    try{
        // 读取Json数据
        Json::Value ReqVal=*req->getJsonObject();
        MyJsonPtr->checkMember(ReqVal,RespVal,"Instruction");
        if(ReqVal["Instruction"].asString() == "restart")
        {
            MyRootPtr->restart();
            RespVal["Result"]="重启成功";
        }
        if(ReqVal["Instruction"].asString() == "close")
        {
            MyRootPtr->close();
            RespVal["Result"]="关机成功";
        }
        
        MyBasePtr->DEBUGLog("RespVal::" + RespVal.toStyledString(), true);

        // 设置返回格式
        ResultData["Result"] = true;
        ResultData["Message"] = RespVal["Result"];
        ResultData["Data"] = RespVal;

        Result = HttpResponse::newHttpJsonResponse(ResultData);
    }
    catch(Json::Value &RespVal)
    {
	    RespVal["Result"] = "指令错误";
        MyBasePtr->TRACE_ERROR(RespVal["ErrorMsg"]);
        // 设置返回格式
        int ErrorSize = RespVal["ErrorMsg"].size();
        ResultData["Result"] = false;
        ResultData["Message"] = RespVal["ErrorMsg"][ErrorSize - 1];

        Result = HttpResponse::newHttpJsonResponse(ResultData);
    }
    catch(const drogon::orm::DrogonDbException &e)
    {
	    RespVal["Result"] = "指令错误";
        RespVal["ErrorMsg"].append(e.base().what());
        MyBasePtr->TRACE_ERROR(RespVal["ErrorMsg"]);
        // 设置返回格式
        int ErrorSize = RespVal["ErrorMsg"].size();
        ResultData["Result"] = false;
        ResultData["Message"] = RespVal["ErrorMsg"][ErrorSize - 1];

        Result = HttpResponse::newHttpJsonResponse(ResultData);
    }

    Result->setStatusCode(k200OK);
    Result->setContentTypeCode(CT_TEXT_HTML);
    callback(Result);
}

void Root::AdminControl(const HttpRequestPtr &req,std::function<void (const HttpResponsePtr &)> &&callback) const
{
    Json::Value ReqVal,RespVal,ParaJson,ResultData;
    drogon::HttpResponsePtr Result;
    auto MyBasePtr = app().getPlugin<MyBase>();
    auto MyJsonPtr = app().getPlugin<MyJson>();
    auto MyDBSPtr = app().getPlugin<MyDBService>();
    const unordered_map<string,string>umapPara = req->getParameters();
    MyBasePtr->TRACELog("Root::AdminControl::body" + string(req->getBody()), true);
    
    try
    {
        // 读取Json数据
        ReqVal = *req->getJsonObject();
        MyBasePtr->DEBUGLog("ReqVal::" + ReqVal.toStyledString(), true);
        MyJsonPtr->UnMapToJson(ReqVal, umapPara, "Para");
        // 检查数据完整性
        {
            MyBasePtr->DEBUGLog("开始检查数据完整性", true);
            std::map<string, MyJson::ColType> ColMap;
            ColMap["User_ID"] = MyJson::ColType::INT;
            MyJsonPtr->checkMemberAndTypeInMap(ReqVal, RespVal, ColMap);
            MyBasePtr->DEBUGLog("检查数据完整性完成", true);
            
            MyBasePtr->DEBUGLog("开始检查Para数据", true);
            ColMap.clear();
            ParaJson = ReqVal["Para"];
            ColMap["User_ID"] = MyJson::ColType::STRING;
            ColMap["Login_Status"] = MyJson::ColType::STRING;
            MyJsonPtr->checkMemberAndTypeInMap(ParaJson, RespVal, ColMap);
            MyBasePtr->DEBUGLog("检查Para数据完成", true);


            if (ParaJson["Login_Status"].asString() != "root")
            {
                RespVal["ErrorMsg"].append("账户权限错误,请联系超级管理员");
                RespVal["Result"]   = false;
                throw RespVal;
            }
        }

        // 开始管理用户
        
        auto dbclientPrt = drogon::app().getDbClient();
        Mapper<drogon_model::novel::User> UserMgr(dbclientPrt);
        Criteria UserID_cri = Criteria(drogon_model::novel::User::Cols::_User_ID,CompareOperator::EQ,ReqVal["User_ID"].asInt());
        vector<drogon_model::novel::User> vecUser = UserMgr.findBy(UserID_cri);

        if(vecUser.size() == 0)
        {
            RespVal["ErrorMsg"].append("用户不存在");
            throw RespVal;
        }

        if(vecUser.size() > 1)
        {
            RespVal["ErrorMsg"].append("用户不唯一");
            throw RespVal;
        }

        drogon_model::novel::User user = vecUser[0];
        int Power = user.getValueOfPower();
        if(Power >= 10000)
        {
            user.setPower(Power - 10000);
            ResultData["Message"] = "用户(ID:"+to_string(user.getValueOfUserId())+")取消管理员";
        }
        else
        {
            user.setPower(Power + 10000);
            ResultData["Message"] = "用户(ID:"+to_string(user.getValueOfUserId())+")设为管理员";
        }
        int row = UserMgr.update(user);
        if(row != 1)
        {
            ResultData["Result"] = false;
            ResultData["Message"] = ResultData["Message"].asString() + "失败";
        }else
        {
            ResultData["Result"] = true;
            ResultData["Message"] = ResultData["Message"].asString() + "成功";
        }
        // 设置返回格式
        ResultData["Data"] = user.toJson();
        ResultData["Data"].removeMember("Password");

        Result = HttpResponse::newHttpJsonResponse(ResultData);

        MyBasePtr->DEBUGLog("RespVal::" + ResultData.toStyledString(), true);
    }
    catch (Json::Value &RespVal)
    {
        RespVal["Result"] = false;
        MyBasePtr->TRACE_ERROR(RespVal["ErrorMsg"]);
        // 设置返回格式
        int ErrorSize = RespVal["ErrorMsg"].size();
        ResultData["Result"] = false;
        ResultData["Message"] = RespVal["ErrorMsg"][ErrorSize - 1];

        Result = HttpResponse::newHttpJsonResponse(ResultData);
    }
    catch (const drogon::orm::DrogonDbException &e)
    {
        RespVal["Result"] = false;
        RespVal["ErrorMsg"].append(e.base().what());
        MyBasePtr->TRACE_ERROR(RespVal["ErrorMsg"]);
        // 设置返回格式
        int ErrorSize = RespVal["ErrorMsg"].size();
        ResultData["Result"] = false;
        ResultData["Message"] = RespVal["ErrorMsg"][ErrorSize - 1];

        Result = HttpResponse::newHttpJsonResponse(ResultData);
    }
    catch (...)
    {
        RespVal["Result"] = false;
        RespVal["ErrorMsg"].append("Root::AdminControl::Error");
        MyBasePtr->TRACE_ERROR(RespVal["ErrorMsg"]);
        // 设置返回格式
        int ErrorSize = RespVal["ErrorMsg"].size();
        ResultData["Result"] = false;
        ResultData["Message"] = RespVal["ErrorMsg"][ErrorSize - 1];

        Result = HttpResponse::newHttpJsonResponse(ResultData);
    }


    Result->setStatusCode(k200OK);
    Result->setContentTypeCode(CT_TEXT_HTML);
    callback(Result);
}

void Root::SysOutLevelSearch(const HttpRequestPtr &req,std::function<void (const HttpResponsePtr &)> &&callback) const
{
    Json::Value ReqVal,RespVal,TempJson,ResultData;
    drogon::HttpResponsePtr Result;
    auto MyBasePtr = app().getPlugin<MyBase>();
    auto MyJsonPtr = app().getPlugin<MyJson>();
    auto MyDBSPtr = app().getPlugin<MyDBService>();
    const unordered_map<string,string>umapPara = req->getParameters();
    MyBasePtr->TRACELog("Root::SysOutLevelSearch::body" + string(req->getBody()), true);
    
    try
    {
        // 读取Json数据
        ReqVal = *req->getJsonObject();
        MyBasePtr->DEBUGLog("ReqVal::" + ReqVal.toStyledString(), true);

        // 设置返回格式
        TempJson["DEBUG"] = MyBasePtr->IsStatus("DEBUG");
        TempJson["TRACE"] = MyBasePtr->IsStatus("TRACE");
        TempJson["INFO"] = MyBasePtr->IsStatus("INFO");
        TempJson["WARN"] = MyBasePtr->IsStatus("WARN");
        ResultData["Data"] = TempJson;

        ResultData["Result"] = true;
        ResultData["Message"] = "系统正常运行中";
        Result = HttpResponse::newHttpJsonResponse(ResultData);

        MyBasePtr->DEBUGLog("RespVal::" + ResultData.toStyledString(), true);
    }
    catch (Json::Value &RespVal)
    {
        RespVal["Result"] = false;
        MyBasePtr->TRACE_ERROR(RespVal["ErrorMsg"]);
        // 设置返回格式
        int ErrorSize = RespVal["ErrorMsg"].size();
        ResultData["Result"] = false;
        ResultData["Message"] = "系统运行异常";

        Result = HttpResponse::newHttpJsonResponse(ResultData);
    }

    Result->setStatusCode(k200OK);
    Result->setContentTypeCode(CT_TEXT_HTML);
    callback(Result);
}

void Root::SysOutLevelUpdate(const HttpRequestPtr &req,std::function<void (const HttpResponsePtr &)> &&callback) const
{
    Json::Value ReqVal,RespVal,TempJson,ResultData;
    drogon::HttpResponsePtr Result;
    auto MyBasePtr = app().getPlugin<MyBase>();
    auto MyJsonPtr = app().getPlugin<MyJson>();
    auto MyDBSPtr = app().getPlugin<MyDBService>();
    const unordered_map<string,string>umapPara = req->getParameters();
    MyBasePtr->TRACELog("Root::SysOutLevelUpdate::body" + string(req->getBody()), true);
    
    try
    {
        // 读取Json数据
        ReqVal = *req->getJsonObject();
        MyBasePtr->DEBUGLog("ReqVal::" + ReqVal.toStyledString(), true);
        string Level = ReqVal["Level"].asString();
        // 检测参数
        {
            std::map<string,MyJson::ColType>ColMap;
            ColMap["Level"] = MyJson::ColType::STRING;
            MyJsonPtr->checkMemberAndTypeInMap(ReqVal,RespVal,ColMap);
            
            if(Level!="DEBUG" && Level!="TRACE" && Level!="INFO" && Level!="WARN")
            {
                RespVal["ErrorMsg"].append("要修改的日志级别格式错误");
                throw RespVal;
            }
        }
        

        MyBasePtr->TRACELog("开始改变输出模式", true);
        MyBasePtr->ChangeStatus(Level);
        MyBasePtr->TRACELog("改变输出模式成功", true);

        // 设置返回格式
        TempJson["DEBUG"] = MyBasePtr->IsStatus("DEBUG");
        TempJson["TRACE"] = MyBasePtr->IsStatus("TRACE");
        TempJson["INFO"] = MyBasePtr->IsStatus("INFO");
        TempJson["WARN"] = MyBasePtr->IsStatus("WARN");
        ResultData["Data"] = TempJson;

        ResultData["Result"] = true;
        ResultData["Message"] = "改变日志状态成功";
        Result = HttpResponse::newHttpJsonResponse(ResultData);

        MyBasePtr->DEBUGLog("RespVal::" + ResultData.toStyledString(), true);
    }
    catch (Json::Value &RespVal)
    {
        RespVal["Result"] = false;
        MyBasePtr->TRACE_ERROR(RespVal["ErrorMsg"]);
        // 设置返回格式
        int ErrorSize = RespVal["ErrorMsg"].size();
        ResultData["Result"] = false;
        ResultData["Message"] = "改变日志状态失败("+RespVal["ErrorMsg"][ErrorSize - 1].asString()+")";

        Result = HttpResponse::newHttpJsonResponse(ResultData);
    }

    Result->setStatusCode(k200OK);
    Result->setContentTypeCode(CT_TEXT_HTML);
    callback(Result);

}
    

void Root::ReportForm(const HttpRequestPtr &req,std::function<void (const HttpResponsePtr &)> &&callback) const
{
    Json::Value ReqVal,RespVal,TempJson,ResultData;
    drogon::HttpResponsePtr Result;
    auto MyBasePtr = app().getPlugin<MyBase>();
    auto MyJsonPtr = app().getPlugin<MyJson>();
    auto MyDBSPtr = app().getPlugin<MyDBService>();
    const unordered_map<string,string>umapPara = req->getParameters();
    MyBasePtr->TRACELog("Root::ReportForm::body" + string(req->getBody()), true);
    
    try
    {
        // 读取Json数据
        ReqVal = *req->getJsonObject();
        MyBasePtr->DEBUGLog("ReqVal::" + ReqVal.toStyledString(), true);
        
        Json::Value DataJson,TempJson;
        
        auto dbclientPrt = drogon::app().getDbClient();
        Mapper<drogon_model::novel::Action> ActionMgr(dbclientPrt);
        Criteria Type_cri = Criteria(drogon_model::novel::Action::Cols::_Type,CompareOperator::EQ,"Money");
        vector<drogon_model::novel::Action> vecAction = ActionMgr.findBy(Type_cri);
        //cout << "开始处理数据" << endl;
        for(auto& item : vecAction)
        {
            cout << item.toJson().toStyledString() << endl;
            TempJson.clear();
            TempJson["Time"] = item.getValueOfTime().toCustomedFormattedStringLocal("%Y-%m-%d %H:%M:%S",false);
            TempJson["UserID"] = item.getValueOfUserId();
            //"Money_Num" 
            TempJson["Memo"] = item.getValueOfMemo();
            //cout << "开始处理Memo数据: " << TempJson["Memo"] << endl;
            MyJsonPtr->JsonstrToJson(TempJson["Memo"],TempJson["Memo"].asCString());
            //cout << "处理Memo数据完成: " << TempJson["Memo"] << endl;
            TempJson["Money_Num"] = TempJson["Memo"]["Money_Num"];
            TempJson.removeMember("Memo");
            TempJson.removeMember("UserID");
            //cout << "Temp数据: " << TempJson << endl;
            DataJson.append(TempJson);
        }
        ResultData["Result"] = true;
        ResultData["Message"] = "系统报表查询成功";
        ResultData["Data"]["Report_Array"] = DataJson;
        Result = HttpResponse::newHttpJsonResponse(ResultData);

        MyBasePtr->DEBUGLog("RespVal::" + ResultData.toStyledString(), true);
    }
    catch (Json::Value &RespVal)
    {
        RespVal["Result"] = false;
        MyBasePtr->TRACE_ERROR(RespVal["ErrorMsg"]);
        // 设置返回格式
        int ErrorSize = RespVal["ErrorMsg"].size();
        ResultData["Result"] = false;
        ResultData["Message"] = RespVal["ErrorMsg"][ErrorSize - 1].asString();

        Result = HttpResponse::newHttpJsonResponse(ResultData);
    }

    Result->setStatusCode(k200OK);
    Result->setContentTypeCode(CT_TEXT_HTML);
    callback(Result);

}


void Root::SysConfigSearch(const HttpRequestPtr &req,std::function<void (const HttpResponsePtr &)> &&callback) const
{
    Json::Value ReqVal,RespVal,TempJson,ResultData;
    drogon::HttpResponsePtr Result;
    auto MyBasePtr = app().getPlugin<MyBase>();
    auto MyJsonPtr = app().getPlugin<MyJson>();
    auto MyRootPtr = app().getPlugin<MyRoot>();
    auto MyDBSPtr = app().getPlugin<MyDBService>();
    const unordered_map<string,string>umapPara = req->getParameters();
    MyBasePtr->TRACELog("Root::SysConfigSearch::body" + string(req->getBody()), true);
    
    try
    {
        // 读取Json数据
        ReqVal = *req->getJsonObject();
        MyBasePtr->DEBUGLog("ReqVal::" + ReqVal.toStyledString(), true);

        // 设置返回格式
        TempJson["Level"] = MyRootPtr->getUserLevelConfig();
        TempJson["Integral"] = MyRootPtr->getIntegralConfig();
        ResultData["Data"] = TempJson;

        ResultData["Result"] = true;
        ResultData["Message"] = "系统正常运行中";
        Result = HttpResponse::newHttpJsonResponse(ResultData);

        MyBasePtr->DEBUGLog("RespVal::" + ResultData.toStyledString(), true);
    }
    catch (Json::Value &RespVal)
    {
        RespVal["Result"] = false;
        MyBasePtr->TRACE_ERROR(RespVal["ErrorMsg"]);
        // 设置返回格式
        int ErrorSize = RespVal["ErrorMsg"].size();
        ResultData["Result"] = false;
        ResultData["Message"] = "系统运行异常";

        Result = HttpResponse::newHttpJsonResponse(ResultData);
    }

    Result->setStatusCode(k200OK);
    Result->setContentTypeCode(CT_TEXT_HTML);
    callback(Result);

}

void Root::SysConfigUpdate(const HttpRequestPtr &req,std::function<void (const HttpResponsePtr &)> &&callback) const
{

    Json::Value ReqVal,RespVal,TempJson,ResultData;
    drogon::HttpResponsePtr Result;
    auto MyBasePtr = app().getPlugin<MyBase>();
    auto MyJsonPtr = app().getPlugin<MyJson>();
    auto MyRootPtr = app().getPlugin<MyRoot>();
    auto MyDBSPtr = app().getPlugin<MyDBService>();
    const unordered_map<string,string>umapPara = req->getParameters();
    MyBasePtr->TRACELog("Root::SysConfigUpdate::body" + string(req->getBody()), true);
    
    try
    {
        // 读取Json数据
        ReqVal = *req->getJsonObject();
        MyBasePtr->DEBUGLog("ReqVal::" + ReqVal.toStyledString(), true);
        // 检测参数
        // 开始检测第一层
        std::map<string,MyJson::ColType>ColMap;
        ColMap["Level"] = MyJson::ColType::JSON;
        ColMap["Integral"] = MyJson::ColType::JSON;
        MyJsonPtr->checkMemberAndTypeInMap(ReqVal,RespVal,ColMap);
        ColMap.clear();
        // 开始检测第二层
        ColMap["Init"] = MyJson::ColType::JSON;
        ColMap["Incremental_Per_LevelUp"] = MyJson::ColType::JSON;
        ColMap["Max_Level"] = MyJson::ColType::INT;
        MyJsonPtr->checkMemberAndTypeInMap(ReqVal["Level"],RespVal,ColMap);
        ColMap.clear();
        ColMap["Upload"] = MyJson::ColType::INT;
        ColMap["Download"] = MyJson::ColType::INT;
        ColMap["Recharge"] = MyJson::ColType::INT;
        MyJsonPtr->checkMemberAndTypeInMap(ReqVal["Integral"],RespVal,ColMap);
        ColMap.clear();
        // 开始检测第三层
        ColMap["Level"] = MyJson::ColType::INT;
        ColMap["Power"] = MyJson::ColType::INT;
        ColMap["Integral"] = MyJson::ColType::INT;
        ColMap["Totle_Book"] = MyJson::ColType::INT;
        ColMap["Chapter_Application"] = MyJson::ColType::INT;
        MyJsonPtr->checkMemberAndTypeInMap(ReqVal["Level"]["Init"],RespVal,ColMap);
        ColMap.clear();
        ColMap["Power"] = MyJson::ColType::INT;
        ColMap["Integral"] = MyJson::ColType::INT;
        ColMap["Totle_Book"] = MyJson::ColType::INT;
        ColMap["Chapter_Application"] = MyJson::ColType::INT;
        MyJsonPtr->checkMemberAndTypeInMap(ReqVal["Level"]["Incremental_Per_LevelUp"],RespVal,ColMap);
        ColMap.clear();
        

        MyBasePtr->TRACELog("开始更新配置", true);
        MyRootPtr->ChangeSysConfig(ReqVal);
        MyBasePtr->TRACELog("更新配置成功", true);

        // 设置返回格式
        TempJson["Level"] = MyRootPtr->getUserLevelConfig();
        TempJson["Integral"] = MyRootPtr->getIntegralConfig();
        ResultData["Data"] = TempJson;

        ResultData["Result"] = true;
        ResultData["Message"] = "修改系统配置成功";
        Result = HttpResponse::newHttpJsonResponse(ResultData);

        MyBasePtr->DEBUGLog("RespVal::" + ResultData.toStyledString(), true);
    }
    catch (Json::Value &e)
    {
        e["Result"] = false;
        MyBasePtr->TRACE_ERROR(e["ErrorMsg"]);
        // 设置返回格式
        int ErrorSize = e["ErrorMsg"].size();
        ResultData["Result"] = false;
        ResultData["Message"] = "修改系统配置失败("+ e["ErrorMsg"][ErrorSize-1].asString() +")";

        Result = HttpResponse::newHttpJsonResponse(ResultData);
    }

    Result->setStatusCode(k200OK);
    Result->setContentTypeCode(CT_TEXT_HTML);
    callback(Result);
}





