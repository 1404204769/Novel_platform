#include "Gpi.h"
//add definition of your processing function here

void Gpi::Login(const HttpRequestPtr &req,std::function<void (const HttpResponsePtr &)> &&callback) const
{
    drogon::HttpResponsePtr Result;
	Json::Value ReqVal, RespVal,ParaVal;
    auto JWTPtr = app().getPlugin<MyJwt>();
    auto MyBasePtr = app().getPlugin<MyBase>();
    auto MyJsonPtr = app().getPlugin<MyJson>();
    auto MyRootPtr = app().getPlugin<MyRoot>();
    const unordered_map<string, string> umapPara = req->getParameters();
    MyBasePtr->TRACELog("Gpi::Login::body" + string(req->getBody()), true);
    
    try{

        // 读取Json数据
	    ReqVal=*req->getJsonObject();
        MyBasePtr->TRACELog("Gpi::Login::ReqVal" + ReqVal.toStyledString(), true);

        // 检查数据完整性
        {
            // 创建检查列表
            // 判断是否存在User_ID 判断User_ID是否是Int
            // 判断是否存在User_Pwd 判断User_Pwd是否是String
            std::map<string,MyJson::ColType>ColMap;
            ColMap["User_ID"]=MyJson::ColType::INT;
            ColMap["User_Pwd"]=MyJson::ColType::STRING;
            MyJsonPtr->checkMemberAndTypeInMap(ReqVal,RespVal,ColMap);
        }

        // 读取UserID UserPwd数据
        auto UserID=ReqVal["User_ID"].asInt();
		auto UserPwd=ReqVal["User_Pwd"].asString();
        auto dbclientPrt=drogon::app().getDbClient();
        Mapper<drogon_model::novel::User>UserMgr(dbclientPrt);
        
        MyBasePtr->DEBUGLog("开始查询用户", true);
        drogon_model::novel::User user = UserMgr.findByPrimaryKey(UserID);
        MyBasePtr->DEBUGLog("用户查询完毕", true);

        // 判断用户密码是否正确
        MyBasePtr->DEBUGLog("开始验证密码", true);
        if(user.getValueOfPassword() != UserPwd)
        {
            RespVal["ErrorMsg"] = "用户密码错误";
            throw RespVal;
        }
        MyBasePtr->DEBUGLog("密码验证通过", true);

        // 创建Token
        MyBasePtr->DEBUGLog("开始创建Token", true);
        ParaVal["User_ID"] = UserID;
        ParaVal["Login_Status"] = MyRootPtr->getUserType(user.getValueOfPower());

        RespVal["Token"] = JWTPtr->encode(ParaVal);
        MyBasePtr->DEBUGLog("Token创建成功", true);

        RespVal["Result"] = "登入成功";
        RespVal["User_Data"] = user.toJson();
        MyBasePtr->DEBUGLog("RespVal::" + RespVal.toStyledString(), true);

        Result=HttpResponse::newHttpJsonResponse(RespVal);
    }
    catch(Json::Value RespVal)
    {
	    RespVal["Result"] = "登入失败";
        MyBasePtr->TRACELog("ErrorMsg::" + RespVal["ErrorMsg"].asString(), true);

        Result=HttpResponse::newHttpJsonResponse(RespVal);
    }
    catch(const drogon::orm::DrogonDbException &e)
    {
	    RespVal["Result"] = "登入失败";
        if(e.base().what() == string("0 rows found"))
        {
            RespVal["ErrorMsg"] = "此用户不存在";
        }
        else if(e.base().what() == string("Found more than one row"))
        {
            RespVal["ErrorMsg"] = "用户ID重复,请联系管理员";
        }
        MyBasePtr->TRACELog("ErrorMsg::" + RespVal["ErrorMsg"].asString(), true);
        Result=HttpResponse::newHttpJsonResponse(RespVal);
    }

    Result->setStatusCode(k200OK);
    Result->setContentTypeCode(CT_TEXT_HTML);
    callback(Result);
}

void Gpi::Register(const HttpRequestPtr &req,std::function<void (const HttpResponsePtr &)> &&callback) const
{
    Json::Value ReqVal, RespVal;
    drogon::HttpResponsePtr Result;
    auto MyBasePtr = app().getPlugin<MyBase>();
    auto MyJsonPtr = app().getPlugin<MyJson>();
    const unordered_map<string, string> umapPara = req->getParameters();
    MyBasePtr->TRACELog("Gpi::Register::body" + string(req->getBody()), true);

    try{
        // 读取Json数据
        ReqVal=*req->getJsonObject();
        MyBasePtr->TRACELog("Admin::User::List::ReqVal" + ReqVal.toStyledString(), true);

        // 检查数据完整性
        {
            // 创建检查列表
            // "User_Name":""
            // "User_ID":0
            // "User_Pwd":""
            // "User_Sex":""
            std::map<string,MyJson::ColType>ColMap;
            ColMap["User_Name"]=MyJson::ColType::STRING;
            ColMap["User_ID"]=MyJson::ColType::INT;
            ColMap["User_Pwd"]=MyJson::ColType::STRING;
            ColMap["User_Sex"]=MyJson::ColType::STRING;
            MyJsonPtr->checkMemberAndTypeInMap(ReqVal,RespVal,ColMap);
        }


        // 读取UserID UserPwd数据
        auto dbclientPrt=drogon::app().getDbClient();
        Mapper<drogon_model::novel::User>UserMgr(dbclientPrt);
        Criteria cri_User_ID(drogon_model::novel::User::Cols::_User_ID, CompareOperator::EQ, ReqVal["User_ID"].asInt());
        
        MyBasePtr->DEBUGLog("开始注册新用户", true);
        drogon_model::novel::User newUser;
        newUser.setUserId(ReqVal["User_ID"].asInt());
        newUser.setName(ReqVal["User_Name"].asString());
        newUser.setPassword(ReqVal["User_Pwd"].asString());
        newUser.setSex(ReqVal["User_Sex"].asString());
        newUser.setLevel(1);
        newUser.setIntegral(0);
        newUser.setTotalIntegral(0);
        newUser.setPower(1);
        newUser.setStatus("正常");
		UserMgr.insert(newUser);
        RespVal["Result"] = "注册成功";
        MyBasePtr->DEBUGLog("注册新用户完成", true);
        MyBasePtr->DEBUGLog("RespVal::" + RespVal.toStyledString(), true);

        Result=HttpResponse::newHttpJsonResponse(RespVal);
    }
    catch(Json::Value RespVal)
    {
	    RespVal["Result"] = "注册失败";
        MyBasePtr->TRACELog("ErrorMsg::" + RespVal["ErrorMsg"].asString(), true);

        Result=HttpResponse::newHttpJsonResponse(RespVal);
    }
    catch(const drogon::orm::DrogonDbException &e)
    {
        // 判断是否是UserID冲突导致的插入失败
        // Duplicate entry '911222' for key 'PRIMARY'
	    RespVal["Result"] = "注册失败";
        if(std::regex_match(e.base().what(), std::regex("Duplicate entry '.*' for key 'PRIMARY'")))
            RespVal["ErrorMsg"] = "注册失败/此UserID已存在";
        else
            RespVal["ErrorMsg"] = "注册失败/" + string(e.base().what());
        MyBasePtr->TRACELog("ErrorMsg::" + RespVal["ErrorMsg"].asString(), true);

        Result=HttpResponse::newHttpJsonResponse(RespVal);
    }

    Result->setStatusCode(k200OK);
    Result->setContentTypeCode(CT_TEXT_HTML);
    callback(Result);
}