/**
 *
 *  MyBase.cc
 *
 */

#include "MyBase.h"

using namespace drogon;

void MyBase::initAndStart(const Json::Value &config)
{
    /// Initialize and start the plugin
    TRACELog("MyBase initAndStart",true);
    cout << "TRACE 输出 " ;
    if(this->TRACE)
        cout << "已开启" << endl;
    else
        cout << "已关闭" << endl;

    cout << "DEBUG 输出 " ;
    if(this->DEBUG)
        cout << "已开启" << endl;
    else
        cout << "已关闭" << endl;

    cout << "INFO 输出 " ;
    if(this->INFO)
        cout << "已开启" << endl;
    else
        cout << "已关闭" << endl;

    cout << "WARN 输出 " ;
    if(this->WARN)
        cout << "已开启" << endl;
    else
        cout << "已关闭" << endl;
}

void MyBase::shutdown() 
{
    /// Shutdown the plugin
    TRACELog("MyBase shutdown",true);
}

bool MyBase::IsStatus(const string &level)
{
    if(level == "TRACE"){
        return TRACE;
    }
    else if(level == "DEBUG"){
        return DEBUG;
    }
    else if(level == "INFO"){
        return INFO;
    }
    else if(level == "WARN"){
        return WARN;
    }
    return false;
}
void MyBase::ChangeStatus(const string &level)
{
    if(level == "TRACE"){
        this->TRACE = !this->TRACE;
        cout << "TRACE 输出 " ;
        if(this->TRACE)
            cout << "已开启" << endl;
        else
            cout << "已关闭" << endl;
    }
    else if(level == "DEBUG"){
        this->DEBUG = !this->DEBUG;
        cout << "DEBUG 输出 " ;
        if(this->DEBUG)
            cout << "已开启" << endl;
        else
            cout << "已关闭" << endl;
    }
    else if(level == "INFO"){
        this->INFO = !this->INFO;
        cout << "INFO 输出 " ;
        if(this->INFO)
            cout << "已开启" << endl;
        else
            cout << "已关闭" << endl;
    }
    else if(level == "WARN"){
        this->WARN = !this->WARN;
        cout << "WARN 输出 " ;
        if(this->WARN)
            cout << "已开启" << endl;
        else
            cout << "已关闭" << endl;
    }
}

void MyBase::TRACELog(const string &str,const  bool Line_feed)
{
    if(!TRACE)return;
    if(Line_feed)
        cout << str << endl;
    else
        cout << str;
    LOG_TRACE<<"";
    LOG_TRACE << str;
}

void MyBase::DEBUGLog(const string &str,const  bool Line_feed)
{
    if(!DEBUG)return;
    if(Line_feed)
        cout << str << endl;
    else
        cout << str;
    LOG_DEBUG<<"";
    LOG_DEBUG << str;
}

void MyBase::INFOLog(const string &str,const  bool Line_feed)
{
    if(!INFO)return;
    if(Line_feed)
        cout << str << endl;
    else
        cout << str;
    LOG_INFO<<"";
    LOG_INFO << str;
}

void MyBase::WARNLog(const string &str,const  bool Line_feed)
{
    if(!WARN)return;
    if(Line_feed)
        cout << str << endl;
    else
        cout << str;
    LOG_WARN<<"";
    LOG_WARN << str;
}