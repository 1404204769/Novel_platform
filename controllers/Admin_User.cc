#include "Admin_User.h"
//add definition of your processing function here
void Admin::User::List(const HttpRequestPtr &req,std::function<void (const HttpResponsePtr &)> &&callback) const
{
    	//write your application logic here
	std::cout<< "req body" << req->getBody()<<std::endl;
	Json::Value RespVal;
    drogon::HttpResponsePtr result;
    try{
        // 读取数据库数据
        auto dbclientPrt=drogon::app().getDbClient();
        Mapper<drogon_model::novel::User>UserMgr(dbclientPrt);
        
        std::cout << "开始查询所有用户" <<std::endl;
        std::vector<drogon_model::novel::User> vecUser = UserMgr.findAll();
        Json::Value vecJsonVal;
        for(auto &user:vecUser)
        {
            vecJsonVal.append(user.toJson());
            // std::cout << user.toJson().toStyledString() <<std::endl;
        }
        RespVal["UserList"] = vecJsonVal;
        result=HttpResponse::newHttpJsonResponse(RespVal);
	    std::cout << "RespVal : "<< RespVal.toStyledString() <<std::endl;
    }
    catch(const drogon::orm::DrogonDbException &e)
    {
        RespVal["ErrorMsg"] = e.base().what();
        std::cout << RespVal["ErrorMsg"] <<std::endl;
        result=HttpResponse::newHttpJsonResponse(RespVal);
    }

    result->setStatusCode(k200OK);
    result->setContentTypeCode(CT_TEXT_HTML);
    callback(result);
}

void Admin::User::Search(const HttpRequestPtr &req,std::function<void (const HttpResponsePtr &)> &&callback) const
{
    
	//write your application logic here
	std::cout<< "req body" << req->getBody()<<std::endl;
	Json::Value RespVal;
    drogon::HttpResponsePtr result;
    auto *MyToolPtr = app().getPlugin<MyTools>();
	RespVal["Result"] = "false";
	auto jsonptr=req->getJsonObject();
    std::vector<drogon_model::novel::User> vecUser;

    try{
        // 读取Json数据
        Json::Value json=*jsonptr;
		// std::cout<< json.toStyledString()<<std::endl;
        MyToolPtr->checkMember(json, RespVal, "UserSearch");
        Json::Value SearchJson=json["UserSearch"];
        // 检查搜索条件
        {
            // 制作筛选条件
            string UserID = "";
            if(SearchJson.isMember("UserID"))
            {
                MyToolPtr->checkColType(SearchJson, RespVal, "UserID", MyTools::ColType::INT);
                UserID = SearchJson["UserID"].asString();
                // std::cout << "UserID : " << UserID <<std::endl;
            }
            Criteria UserID_cri(drogon_model::novel::User::Cols::_User_ID, CompareOperator::Like, "%"+UserID+"%");

            string UserSex = "";
            if(SearchJson.isMember("UserSex"))
            {
                MyToolPtr->checkColType(SearchJson, RespVal, "UserSex", MyTools::ColType::STRING);
                UserSex = SearchJson["UserSex"].asString();
            }
            Criteria UserSex_cri(drogon_model::novel::User::Cols::_Sex, CompareOperator::Like, "%"+UserSex+"%");
            
            string UserName = "";
            if(SearchJson.isMember("UserName"))
            {
                MyToolPtr->checkColType(SearchJson, RespVal, "UserName", MyTools::ColType::STRING);
                UserName = SearchJson["UserName"].asString();
            }
            Criteria UserName_cri(drogon_model::novel::User::Cols::_Name, CompareOperator::Like, "%"+UserName+"%");

            auto dbclientPrt=drogon::app().getDbClient();
            Mapper<drogon_model::novel::User>UserMgr(dbclientPrt);
            
            std::cout << "开始查询指定用户" <<std::endl;
            vecUser = UserMgr.findBy(UserID_cri&&UserName_cri&&UserSex_cri);
            RespVal["Result"] = "success";
        }

        Json::Value vecJsonVal;
        for(auto &user:vecUser)
        {
            vecJsonVal.append(user.toJson());
            // std::cout << user.toJson().toStyledString() <<std::endl;
        }
        RespVal["UserList"] = vecJsonVal;
        result=HttpResponse::newHttpJsonResponse(RespVal);
	    // std::cout << "RespVal : "<< RespVal.toStyledString() <<std::endl;
    }
    catch(Json::Value e)
    {
        std::cout << e["ErrorMsg"] <<std::endl;
        result=HttpResponse::newHttpJsonResponse(e);
    }
    catch(const drogon::orm::DrogonDbException &e)
    {
        RespVal["ErrorMsg"] = e.base().what();
        std::cout << RespVal["ErrorMsg"] <<std::endl;
        result=HttpResponse::newHttpJsonResponse(RespVal);
    }

    result->setStatusCode(k200OK);
    result->setContentTypeCode(CT_TEXT_HTML);
    callback(result);
}



