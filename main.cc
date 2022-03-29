#include <drogon/drogon.h>

using namespace drogon;
void addHeader(const HttpRequestPtr &req, const HttpResponsePtr &resp)
{
    resp->addHeader("Access-Control-Allow-Origin",req->getHeader("Origin"));
    resp->addHeader("Access-Control-Allow-Method","GET,POST,OPTIONS");
    resp->addHeader("Access-Control-Allow-Headers","*");
    resp->addHeader("Access-Control-Expose-Headers","*");
    resp->addHeader("Access-Control-Allow-Credentials", "true");
    resp->addHeader("Access-Control-Max-Age", "3600");
}
int main() {
    //Set HTTP listener address and port
    //drogon::app().addListener("0.0.0.0",8080);
    //Load config file
    app().loadConfigFile("../config.json");
    //Run HTTP framework,the method will block in the internal event loop
    //增加响应头
    app().registerPostHandlingAdvice(addHeader);
    app().run();
    return 0;
}
