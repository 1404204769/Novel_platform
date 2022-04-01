/**
 *
 *  MyTools.cc
 *
 */

#include "MyTools.h"

using namespace drogon;

void MyTools::initAndStart(const Json::Value &config)
{
    /// Initialize and start the plugin
    auto MyBasePtr = drogon::app().getPlugin<MyBase>();
    MyBasePtr->TRACELog("MyTools initAndStart",true);
}

void MyTools::shutdown() 
{
    /// Shutdown the plugin
    auto MyBasePtr = drogon::app().getPlugin<MyBase>();
    MyBasePtr->TRACELog("MyTools shutdown",true);
}

wstring MyTools::StringToWString(const string& str)
{
    using convert_typeX = codecvt_utf8<wchar_t>;
    wstring_convert<convert_typeX, wchar_t> converterX;

    return converterX.from_bytes(str);
}

string MyTools::WStringToString(const wstring& wstr)
{
    using convert_typeX = codecvt_utf8<wchar_t>;
    wstring_convert<convert_typeX, wchar_t> converterX;

    return converterX.to_bytes(wstr);
}

// 将中文字符串分割成单个中文
vector<string> MyTools::SpiteStringCharacter(const string &ChineseStr)
{
    vector<string> res;
 
    wstring wContext = StringToWString(ChineseStr);
    for (int i = 0; i < wContext.length(); ++i)
    {
        wstring tmp = wContext.substr(i, 1);
        res.push_back(WStringToString(tmp));
    }
 
    return res;
}

// 从标题中提取章节数
vector<string> MyTools::NumberOfChaptersExtracted(vector<string> &Title)
{
    auto MyBasePtr = drogon::app().getPlugin<MyBase>();
    vector<string>ans;
    int len = Title.size();
    MyBasePtr->DEBUGLog("进入提取函数",true);
    for(int i = 0 ; i < len ; i++){
        if(Title[i] != "第")
            continue;
        MyBasePtr->DEBUGLog("index : " + to_string(i) + "  开始查找章节数",true);
        bool find = false;
        int j = i + 1 ; // 从第的后一个开始找
        for(; j < len ; j++)
        {
            if(Title[j] == "第")
            {// 如果是第xx第yyy章 ,那么需要从最靠近'章'的那个'第'开始提取
                i = j - 1;
                break;
            }
            if(Title[j] == "章")
            {
                find = true;
                MyBasePtr->DEBUGLog("index : " + to_string(j) + "  章节数查找完毕",true);
                break;
            }
        }
        if(find)
        {   // 如果找到第xxx章 开始复制xxx
            for(int a = i+1; a < j ;a++)
                ans.push_back(Title[a]);
            if(MyBasePtr->IsStatus("DEBUG"))
            {
                cout << "index_begin : " << i+1 << "  index_end : " << j-1 << "  开始复制章节数" << endl;
                cout << "开始展示复制的章节数" << endl;
                for(auto s:ans)
                    cout << s << " ";
                cout << endl;
            }
            break;
        }
    }
    return ans;// 防止xxx内还有第yy章，需要二次过滤
}

// 将中文数字转化为阿拉伯数字
int MyTools::ChineseNumToAlabNum(const vector<string> &ChineseNum)
{
    int Num = 0;
    string unit = "十百千万", number = "零一二三四五六七八九十";
    vector<string>unit_v = SpiteStringCharacter(unit);
    vector<string>number_v = SpiteStringCharacter(number);
    int tem; //存放单位
    for (int i = 0; i <= ChineseNum.size(); i++)
    {
        int tete = ChineseNum.size();
        tem = 1;
        if (i == ChineseNum.size() - 1)
        {
            for (int j = 0; j < 11; j++)
            {
                if (number_v[j] == ChineseNum[i])
                {
                    Num += j;
                    break;
                }
            }
            break;
        }
        for (int k = 0; k < 11; k++)
        {
            if (number_v[k] == ChineseNum[i])
            {
                if (k == 0)
                {
                    break;
                }
                else if (k == 10)
                {
                    Num += 10;
                    break;
                }
                else
                {
                    for (int l = 0; l < 5; l++)
                    {
                        if (unit_v[l] == ChineseNum[i + 1])
                        {
                            switch (l)
                            {
                            case 0:
                                Num += k * 10;
                                i++;
                                break;
                            case 1:
                                Num += k * 100;
                                i++;
                                break;
                            case 2:
                                Num += k * 1000;
                                i++;
                                break;
                            case 3:
                                Num += k * 10000;
                                i++;
                                break;
                            }
                            break; //退出寻找单位循环
                        }
                    }
                    break; //退出当前字符转换，开始下一个
                }
            }
        }
    }
    return Num;
}