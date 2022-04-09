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


void MyBase::INFO_Mark(const string &Func_Name)
{
    if(!INFO)return;
    printLine(Func_Name);
}

void MyBase::INFO_Func(const string &Func_Name,const bool &Is_Req,Json::Value &JsonVal)
{
    if(!INFO)return;
    if(Is_Req)
    {
        // 说明是函数首部
        INFO_Mark("进入"+Func_Name+"函数");
        INFOLog("ReqVal::" + JsonVal.toStyledString(), true);
        return;
    }
    INFOLog("\nRespVal::" + JsonVal.toStyledString(), true);
    INFO_Mark("退出"+Func_Name+"函数");
}

void MyBase::TRACE_ERROR(Json::Value &ERROR)
{
    if(!TRACE)return;
    printLine("ERROR_ARRAY_BEGIN");

    int size = ERROR.size();
    for(int i = 0 ;i < size ;i++)
    {
        cout << ERROR[i].asString() << endl;
    }
    
    printLine("ERROR_ARRAY_END");
}
void MyBase::printLine(const string Str)
{
    int size = Str.size();
    int num = (100 - size)/2;
    for(int i = 0;i < num ;i++)
        cout << "-";
    cout << Str;
    for(int i = 0;i < num ;i++)
        cout << "-";
    cout << endl;
}