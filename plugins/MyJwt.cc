/**
 *
 *  MyJwt.cc
 *
 */

#include "MyJwt.h"

using namespace drogon;

void MyJwt::initAndStart(const Json::Value &config)
{
    /// Initialize and start the plugin
    string str = config["secret"].toStyledString();
    this->secret = str.substr(1,str.size()-3);

    cout << "JWT initAndStart" << endl;
    // cout << "JWT config : " << config << endl;
    // cout << "config secret size: " << str.size() << endl;
    // cout << "JWT secret : " << this->secret << endl;
}

void MyJwt::shutdown() 
{
    /// Shutdown the plugin
    cout << "JWT shutdown" << endl;
}

string MyJwt::encode(const Json::Value &param)
{
	auto *mytools = drogon::app().getPlugin<MyTools>();//获取mytools插件
    /*
        iss: jwt签发者
        sub: jwt所面向的用户
        aud: 接收jwt的一方
        exp: jwt的过期时间，这个过期时间必须要大于签发时间
        nbf: 定义在什么时间之前，该jwt都是不可用的.
        iat: jwt的签发时间
        jti: jwt的唯一身份标识，主要用来作为一次性token,从而回避重放攻击。
    */
    jwt::jwt_object obj{jwt::params::algorithm(jwt::algorithm::HS256),
                    jwt::params::secret(this->secret),jwt::params::payload(mytools->jsonstr2map(param.toStyledString()))};
    
    // cout << "secret: " << this->secret << endl;
    // cout << "Header: " << obj.header() << endl;
    // cout << "Payload: " << obj.payload() << endl;
    
    string enstr = obj.signature();
    int i = 0;
    array<jwt::string_view,3UL> arr = obj.three_parts(enstr);
    for(int i = 0; i < arr.size() ; i ++ ){
        cout << "three_parts " << i+1 << " : " << arr[i] << endl;
    }

    return enstr;
}

Json::Value MyJwt::decode(const string &token)
{
    error_code ec;
	Json::Reader Reader;
	Json::Value DevJson,RespVal;
  	auto jwt_obj = jwt::decode(token, jwt::params::algorithms({"HS256"}), ec, 
	  								jwt::params::secret(this->secret), jwt::params::verify(true));
	
	string strHeader = jwt::to_json_str(jwt_obj.header()); 
	//cout<< "header:" << strHeader << endl;

	string strPayload = jwt::to_json_str(jwt_obj.payload()); 
	//cout<< "payload:" << strPayload << endl;

	Reader.parse(strHeader,DevJson);
	RespVal["header"] = DevJson;

	Reader.parse(strPayload,DevJson);
	RespVal["payload"] = DevJson;

	cout<< "RespVal:" << RespVal << endl;
    return RespVal;
}

bool MyJwt::verify(const string &token)
{
    jwt::verify_func_t();
    //jwt::verify_result_t;
    //jwt::VerificationErrc;
    //jwt::VerificationError;
    //jwt::VerificationErrorCategory;
    //jwt::theVerificationErrorCategory;
    try{
        array<jwt::string_view,3UL> old_arr = jwt::jwt_object::three_parts(token);
        auto *mytools = drogon::app().getPlugin<MyTools>();//获取mytools插件

        jwt::verify_result_t result;
        jwt::jwt_signature signature;
        jwt::jwt_header header = old_arr[0];
        jwt::jwt_payload payload = old_arr[1];
        jwt::string_view old_sign = old_arr[2];
        
        // cout<< "header:" << header << endl;
        // cout<< "payload:" << payload << endl;
        // cout<< "old_token:" << token << endl;
        // cout<< "old_sign:" << old_sign << endl;
    
        stringstream sspayload;
        string strpayload;
        sspayload << payload;
        sspayload >> strpayload;
        jwt::jwt_object obj{jwt::params::algorithm(header.algo()),
                      jwt::params::secret(this->secret),jwt::params::payload(mytools->jsonstr2map(strpayload))};
        string new_token = obj.signature();
        array<jwt::string_view,3UL> new_arr = jwt::jwt_object::three_parts(new_token);
        string new_sign = new_arr[2].data();
        
        // cout<< "secret:" << this->secret << endl;
        // cout<< "new_token:" << new_token << endl;
        // cout<< "new_sign:" << new_sign << endl;
    
        if(new_sign == old_sign)
        {
            //cout << "Token Verify Sucessfully" << endl;
            return true;
        }else
        {
            //cout << "Token Verify Fail" << endl;
            return false;
        }
    }catch(jwt::MemoryAllocationException){
        cout << "MemoryAllocationException" << endl;
        return false;
    }
}