#include "Gpi.h"
//add definition of your processing function here

void Gpi::Login(const HttpRequestPtr &req,std::function<void (const HttpResponsePtr &)> &&callback) const
{
	//write your application logic here
	std::cout<< "req body" << req->getBody()<<std::endl;
	auto *MyToolPtr = app().getPlugin<MyTools>();//获取MyTools插件
	Json::Value RespVal;
    drogon::HttpResponsePtr result;
	RespVal["Result"] = "false";
	auto jsonptr=req->getJsonObject();
    try{

        // 读取Json数据
        Json::Value json=*jsonptr;
		std::cout<< json.toStyledString()<<std::endl;

        // 检查数据完整性
        {
            // 创建检查列表
            std::map<string,MyTools::ColType>ColMap;
            // 判断是否存在UserID 判断UserID是否是Int
            ColMap["UserID"]=MyTools::ColType::INT;
            // 判断是否存在UserPwd 判断UserPwd是否是String
            ColMap["UserPwd"]=MyTools::ColType::STRING;
            MyToolPtr->checkMemberAndTypeInMap(json,RespVal,ColMap);
        }

        // 读取UserID UserPwd数据
        auto UserID=json["UserID"].asInt();
		auto UserPwd=json["UserPwd"].asString();
        auto dbclientPrt=drogon::app().getDbClient();
        Mapper<drogon_model::novel::User>UserMgr(dbclientPrt);
        
        std::cout << "开始查询用户" <<std::endl;
        drogon_model::novel::User user = UserMgr.findByPrimaryKey(UserID);

        // 判断用户密码是否正确
        if(user.getValueOfPassword() != UserPwd)
        {
            std::cout << "用户密码不正确" <<std::endl;
            RespVal["ErrorMsg"] = "用户密码不正确";
            throw RespVal;
        }

        // 创建Token
		auto *JWTPtr = app().getPlugin<MyJwt>();//获取MyJwt插件
        Json::Value param;
        param["user"] = "admin";
        param["Result"] = "success";
        RespVal["Token"] = JWTPtr->encode(param);

        RespVal["Result"] = "true";
		RespVal["msg"]="登入成功";

        result=HttpResponse::newHttpJsonResponse(RespVal);
	    std::cout << "login result "<< RespVal["Result"].asString() <<std::endl;
    }
    catch(Json::Value e)
    {
        std::cout << e["ErrorMsg"] <<std::endl;
        result=HttpResponse::newHttpJsonResponse(e);
    }
    catch(const drogon::orm::DrogonDbException &e)
    {
        if(e.base().what() == string("0 rows found"))
        {
            RespVal["ErrorMsg"] = "此用户不存在";
        }
        else if(e.base().what() == string("Found more than one row"))
        {
            RespVal["ErrorMsg"] = "用户ID重复,请联系管理员";
        }
        std::cout << RespVal["ErrorMsg"] <<std::endl;
        result=HttpResponse::newHttpJsonResponse(RespVal);
    }

    result->setStatusCode(k200OK);
    result->setContentTypeCode(CT_TEXT_HTML);
    callback(result);
}

void Gpi::Register(const HttpRequestPtr &req,std::function<void (const HttpResponsePtr &)> &&callback) const
{
    std::cout<< "req body" << req->getBody()<<std::endl;
	auto *MyToolPtr = app().getPlugin<MyTools>();//获取MyTools插件
	Json::Value RespVal;
    drogon::HttpResponsePtr result;
	RespVal["Result"] = "false";
	auto jsonptr=req->getJsonObject();

    try{
        // 读取Json数据
        Json::Value json=*jsonptr;
		std::cout<< json.toStyledString()<<std::endl;

        // 检查数据完整性
        {
            // 创建检查列表
            std::map<string,MyTools::ColType>ColMap;
            // 判断是否存在UserName 判断UserName是否是String
            ColMap["UserName"]=MyTools::ColType::STRING;
            // 判断是否存在UserID 判断UserID是否是Int
            ColMap["UserID"]=MyTools::ColType::INT;
            // 判断是否存在UserPwd 判断UserPwd是否是String
            ColMap["UserPwd"]=MyTools::ColType::STRING;
            // 判断是否存在UserSex 判断UserSex是否是String
            ColMap["UserSex"]=MyTools::ColType::STRING;
            MyToolPtr->checkMemberAndTypeInMap(json,RespVal,ColMap);
        }


        // 读取UserID UserPwd数据
        auto UserID=json["UserID"].asInt();
		auto UserName=json["UserName"].asString();
		auto UserPwd=json["UserPwd"].asString();
        auto UserSex=json["UserSex"].asString();
        auto UserLevel=1;
        auto UserIntegral=0;
        auto UserPower=1;
        auto UserStatus="正常";
        auto dbclientPrt=drogon::app().getDbClient();
        Mapper<drogon_model::novel::User>UserMgr(dbclientPrt);
        Criteria cri_User_ID(drogon_model::novel::User::Cols::_User_ID, CompareOperator::EQ, UserID);
        
        std::cout << "开始注册新用户" <<std::endl;
        drogon_model::novel::User newUser;
        newUser.setUserId(UserID);
        newUser.setName(UserName);
        newUser.setPassword(UserPwd);
        newUser.setSex(UserSex);
        newUser.setLevel(UserLevel);
        newUser.setIntegral(UserIntegral);
        newUser.setPower(UserPower);
        newUser.setStatus(UserStatus);
		UserMgr.insert(newUser);
		RespVal["msg"]="注册成功";
        RespVal["Result"] = "true";

        result=HttpResponse::newHttpJsonResponse(RespVal);
	    std::cout << "register result "<< RespVal["Result"].asString() <<std::endl;
    }
    catch(Json::Value e)
    {
        std::cout << e["ErrorMsg"] <<std::endl;
        result=HttpResponse::newHttpJsonResponse(e);
    }
    catch(const drogon::orm::DrogonDbException &e)
    {
        // 判断是否是UserID冲突导致的插入失败
        // Duplicate entry '911222' for key 'PRIMARY'
        if(std::regex_match(e.base().what(), std::regex("Duplicate entry '.*' for key 'PRIMARY'")))
            RespVal["ErrorMsg"] = "注册失败/此UserID已存在";
        else
            RespVal["ErrorMsg"] = "注册失败";
        std::cout << RespVal["ErrorMsg"] <<std::endl;
        RespVal["DrogonDbException"] = e.base().what();
        result=HttpResponse::newHttpJsonResponse(RespVal);
    }
    result->setStatusCode(k200OK);
    result->setContentTypeCode(CT_TEXT_HTML);
    callback(result);
}