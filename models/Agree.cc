/**
 *
 *  Agree.cc
 *  DO NOT EDIT. This file is generated by drogon_ctl
 *
 */

#include "Agree.h"
#include <drogon/utils/Utilities.h>
#include <string>

using namespace drogon;
using namespace drogon_model::novel;

const std::string Agree::Cols::_Agree_ID = "Agree_ID";
const std::string Agree::Cols::_User_ID = "User_ID";
const std::string Agree::Cols::_Note_ID = "Note_ID";
const std::string Agree::Cols::_Comment_ID = "Comment_ID";
const std::string Agree::Cols::_Status = "Status";
const std::string Agree::Cols::_Time = "Time";
const std::string Agree::primaryKeyName = "Agree_ID";
const bool Agree::hasPrimaryKey = true;
const std::string Agree::tableName = "agree";

const std::vector<typename Agree::MetaData> Agree::metaData_={
{"Agree_ID","int32_t","int(10)",4,1,1,1},
{"User_ID","int32_t","int(10)",4,0,0,1},
{"Note_ID","int32_t","int(10)",4,0,0,1},
{"Comment_ID","int32_t","int(10)",4,0,0,1},
{"Status","int8_t","tinyint(1)",1,0,0,1},
{"Time","::trantor::Date","timestamp",0,0,0,1}
};
const std::string &Agree::getColumnName(size_t index) noexcept(false)
{
    assert(index < metaData_.size());
    return metaData_[index].colName_;
}
Agree::Agree(const Row &r, const ssize_t indexOffset) noexcept
{
    if(indexOffset < 0)
    {
        if(!r["Agree_ID"].isNull())
        {
            agreeId_=std::make_shared<int32_t>(r["Agree_ID"].as<int32_t>());
        }
        if(!r["User_ID"].isNull())
        {
            userId_=std::make_shared<int32_t>(r["User_ID"].as<int32_t>());
        }
        if(!r["Note_ID"].isNull())
        {
            noteId_=std::make_shared<int32_t>(r["Note_ID"].as<int32_t>());
        }
        if(!r["Comment_ID"].isNull())
        {
            commentId_=std::make_shared<int32_t>(r["Comment_ID"].as<int32_t>());
        }
        if(!r["Status"].isNull())
        {
            status_=std::make_shared<int8_t>(r["Status"].as<int8_t>());
        }
        if(!r["Time"].isNull())
        {
            auto timeStr = r["Time"].as<std::string>();
            struct tm stm;
            memset(&stm,0,sizeof(stm));
            auto p = strptime(timeStr.c_str(),"%Y-%m-%d %H:%M:%S",&stm);
            time_t t = mktime(&stm);
            size_t decimalNum = 0;
            if(p)
            {
                if(*p=='.')
                {
                    std::string decimals(p+1,&timeStr[timeStr.length()]);
                    while(decimals.length()<6)
                    {
                        decimals += "0";
                    }
                    decimalNum = (size_t)atol(decimals.c_str());
                }
                time_=std::make_shared<::trantor::Date>(t*1000000+decimalNum);
            }
        }
    }
    else
    {
        size_t offset = (size_t)indexOffset;
        if(offset + 6 > r.size())
        {
            LOG_FATAL << "Invalid SQL result for this model";
            return;
        }
        size_t index;
        index = offset + 0;
        if(!r[index].isNull())
        {
            agreeId_=std::make_shared<int32_t>(r[index].as<int32_t>());
        }
        index = offset + 1;
        if(!r[index].isNull())
        {
            userId_=std::make_shared<int32_t>(r[index].as<int32_t>());
        }
        index = offset + 2;
        if(!r[index].isNull())
        {
            noteId_=std::make_shared<int32_t>(r[index].as<int32_t>());
        }
        index = offset + 3;
        if(!r[index].isNull())
        {
            commentId_=std::make_shared<int32_t>(r[index].as<int32_t>());
        }
        index = offset + 4;
        if(!r[index].isNull())
        {
            status_=std::make_shared<int8_t>(r[index].as<int8_t>());
        }
        index = offset + 5;
        if(!r[index].isNull())
        {
            auto timeStr = r[index].as<std::string>();
            struct tm stm;
            memset(&stm,0,sizeof(stm));
            auto p = strptime(timeStr.c_str(),"%Y-%m-%d %H:%M:%S",&stm);
            time_t t = mktime(&stm);
            size_t decimalNum = 0;
            if(p)
            {
                if(*p=='.')
                {
                    std::string decimals(p+1,&timeStr[timeStr.length()]);
                    while(decimals.length()<6)
                    {
                        decimals += "0";
                    }
                    decimalNum = (size_t)atol(decimals.c_str());
                }
                time_=std::make_shared<::trantor::Date>(t*1000000+decimalNum);
            }
        }
    }

}

Agree::Agree(const Json::Value &pJson, const std::vector<std::string> &pMasqueradingVector) noexcept(false)
{
    if(pMasqueradingVector.size() != 6)
    {
        LOG_ERROR << "Bad masquerading vector";
        return;
    }
    if(!pMasqueradingVector[0].empty() && pJson.isMember(pMasqueradingVector[0]))
    {
        dirtyFlag_[0] = true;
        if(!pJson[pMasqueradingVector[0]].isNull())
        {
            agreeId_=std::make_shared<int32_t>((int32_t)pJson[pMasqueradingVector[0]].asInt64());
        }
    }
    if(!pMasqueradingVector[1].empty() && pJson.isMember(pMasqueradingVector[1]))
    {
        dirtyFlag_[1] = true;
        if(!pJson[pMasqueradingVector[1]].isNull())
        {
            userId_=std::make_shared<int32_t>((int32_t)pJson[pMasqueradingVector[1]].asInt64());
        }
    }
    if(!pMasqueradingVector[2].empty() && pJson.isMember(pMasqueradingVector[2]))
    {
        dirtyFlag_[2] = true;
        if(!pJson[pMasqueradingVector[2]].isNull())
        {
            noteId_=std::make_shared<int32_t>((int32_t)pJson[pMasqueradingVector[2]].asInt64());
        }
    }
    if(!pMasqueradingVector[3].empty() && pJson.isMember(pMasqueradingVector[3]))
    {
        dirtyFlag_[3] = true;
        if(!pJson[pMasqueradingVector[3]].isNull())
        {
            commentId_=std::make_shared<int32_t>((int32_t)pJson[pMasqueradingVector[3]].asInt64());
        }
    }
    if(!pMasqueradingVector[4].empty() && pJson.isMember(pMasqueradingVector[4]))
    {
        dirtyFlag_[4] = true;
        if(!pJson[pMasqueradingVector[4]].isNull())
        {
            status_=std::make_shared<int8_t>((int8_t)pJson[pMasqueradingVector[4]].asInt64());
        }
    }
    if(!pMasqueradingVector[5].empty() && pJson.isMember(pMasqueradingVector[5]))
    {
        dirtyFlag_[5] = true;
        if(!pJson[pMasqueradingVector[5]].isNull())
        {
            auto timeStr = pJson[pMasqueradingVector[5]].asString();
            struct tm stm;
            memset(&stm,0,sizeof(stm));
            auto p = strptime(timeStr.c_str(),"%Y-%m-%d %H:%M:%S",&stm);
            time_t t = mktime(&stm);
            size_t decimalNum = 0;
            if(p)
            {
                if(*p=='.')
                {
                    std::string decimals(p+1,&timeStr[timeStr.length()]);
                    while(decimals.length()<6)
                    {
                        decimals += "0";
                    }
                    decimalNum = (size_t)atol(decimals.c_str());
                }
                time_=std::make_shared<::trantor::Date>(t*1000000+decimalNum);
            }
        }
    }
}

Agree::Agree(const Json::Value &pJson) noexcept(false)
{
    if(pJson.isMember("Agree_ID"))
    {
        dirtyFlag_[0]=true;
        if(!pJson["Agree_ID"].isNull())
        {
            agreeId_=std::make_shared<int32_t>((int32_t)pJson["Agree_ID"].asInt64());
        }
    }
    if(pJson.isMember("User_ID"))
    {
        dirtyFlag_[1]=true;
        if(!pJson["User_ID"].isNull())
        {
            userId_=std::make_shared<int32_t>((int32_t)pJson["User_ID"].asInt64());
        }
    }
    if(pJson.isMember("Note_ID"))
    {
        dirtyFlag_[2]=true;
        if(!pJson["Note_ID"].isNull())
        {
            noteId_=std::make_shared<int32_t>((int32_t)pJson["Note_ID"].asInt64());
        }
    }
    if(pJson.isMember("Comment_ID"))
    {
        dirtyFlag_[3]=true;
        if(!pJson["Comment_ID"].isNull())
        {
            commentId_=std::make_shared<int32_t>((int32_t)pJson["Comment_ID"].asInt64());
        }
    }
    if(pJson.isMember("Status"))
    {
        dirtyFlag_[4]=true;
        if(!pJson["Status"].isNull())
        {
            status_=std::make_shared<int8_t>((int8_t)pJson["Status"].asInt64());
        }
    }
    if(pJson.isMember("Time"))
    {
        dirtyFlag_[5]=true;
        if(!pJson["Time"].isNull())
        {
            auto timeStr = pJson["Time"].asString();
            struct tm stm;
            memset(&stm,0,sizeof(stm));
            auto p = strptime(timeStr.c_str(),"%Y-%m-%d %H:%M:%S",&stm);
            time_t t = mktime(&stm);
            size_t decimalNum = 0;
            if(p)
            {
                if(*p=='.')
                {
                    std::string decimals(p+1,&timeStr[timeStr.length()]);
                    while(decimals.length()<6)
                    {
                        decimals += "0";
                    }
                    decimalNum = (size_t)atol(decimals.c_str());
                }
                time_=std::make_shared<::trantor::Date>(t*1000000+decimalNum);
            }
        }
    }
}

void Agree::updateByMasqueradedJson(const Json::Value &pJson,
                                            const std::vector<std::string> &pMasqueradingVector) noexcept(false)
{
    if(pMasqueradingVector.size() != 6)
    {
        LOG_ERROR << "Bad masquerading vector";
        return;
    }
    if(!pMasqueradingVector[0].empty() && pJson.isMember(pMasqueradingVector[0]))
    {
        if(!pJson[pMasqueradingVector[0]].isNull())
        {
            agreeId_=std::make_shared<int32_t>((int32_t)pJson[pMasqueradingVector[0]].asInt64());
        }
    }
    if(!pMasqueradingVector[1].empty() && pJson.isMember(pMasqueradingVector[1]))
    {
        dirtyFlag_[1] = true;
        if(!pJson[pMasqueradingVector[1]].isNull())
        {
            userId_=std::make_shared<int32_t>((int32_t)pJson[pMasqueradingVector[1]].asInt64());
        }
    }
    if(!pMasqueradingVector[2].empty() && pJson.isMember(pMasqueradingVector[2]))
    {
        dirtyFlag_[2] = true;
        if(!pJson[pMasqueradingVector[2]].isNull())
        {
            noteId_=std::make_shared<int32_t>((int32_t)pJson[pMasqueradingVector[2]].asInt64());
        }
    }
    if(!pMasqueradingVector[3].empty() && pJson.isMember(pMasqueradingVector[3]))
    {
        dirtyFlag_[3] = true;
        if(!pJson[pMasqueradingVector[3]].isNull())
        {
            commentId_=std::make_shared<int32_t>((int32_t)pJson[pMasqueradingVector[3]].asInt64());
        }
    }
    if(!pMasqueradingVector[4].empty() && pJson.isMember(pMasqueradingVector[4]))
    {
        dirtyFlag_[4] = true;
        if(!pJson[pMasqueradingVector[4]].isNull())
        {
            status_=std::make_shared<int8_t>((int8_t)pJson[pMasqueradingVector[4]].asInt64());
        }
    }
    if(!pMasqueradingVector[5].empty() && pJson.isMember(pMasqueradingVector[5]))
    {
        dirtyFlag_[5] = true;
        if(!pJson[pMasqueradingVector[5]].isNull())
        {
            auto timeStr = pJson[pMasqueradingVector[5]].asString();
            struct tm stm;
            memset(&stm,0,sizeof(stm));
            auto p = strptime(timeStr.c_str(),"%Y-%m-%d %H:%M:%S",&stm);
            time_t t = mktime(&stm);
            size_t decimalNum = 0;
            if(p)
            {
                if(*p=='.')
                {
                    std::string decimals(p+1,&timeStr[timeStr.length()]);
                    while(decimals.length()<6)
                    {
                        decimals += "0";
                    }
                    decimalNum = (size_t)atol(decimals.c_str());
                }
                time_=std::make_shared<::trantor::Date>(t*1000000+decimalNum);
            }
        }
    }
}
                                                                    
void Agree::updateByJson(const Json::Value &pJson) noexcept(false)
{
    if(pJson.isMember("Agree_ID"))
    {
        if(!pJson["Agree_ID"].isNull())
        {
            agreeId_=std::make_shared<int32_t>((int32_t)pJson["Agree_ID"].asInt64());
        }
    }
    if(pJson.isMember("User_ID"))
    {
        dirtyFlag_[1] = true;
        if(!pJson["User_ID"].isNull())
        {
            userId_=std::make_shared<int32_t>((int32_t)pJson["User_ID"].asInt64());
        }
    }
    if(pJson.isMember("Note_ID"))
    {
        dirtyFlag_[2] = true;
        if(!pJson["Note_ID"].isNull())
        {
            noteId_=std::make_shared<int32_t>((int32_t)pJson["Note_ID"].asInt64());
        }
    }
    if(pJson.isMember("Comment_ID"))
    {
        dirtyFlag_[3] = true;
        if(!pJson["Comment_ID"].isNull())
        {
            commentId_=std::make_shared<int32_t>((int32_t)pJson["Comment_ID"].asInt64());
        }
    }
    if(pJson.isMember("Status"))
    {
        dirtyFlag_[4] = true;
        if(!pJson["Status"].isNull())
        {
            status_=std::make_shared<int8_t>((int8_t)pJson["Status"].asInt64());
        }
    }
    if(pJson.isMember("Time"))
    {
        dirtyFlag_[5] = true;
        if(!pJson["Time"].isNull())
        {
            auto timeStr = pJson["Time"].asString();
            struct tm stm;
            memset(&stm,0,sizeof(stm));
            auto p = strptime(timeStr.c_str(),"%Y-%m-%d %H:%M:%S",&stm);
            time_t t = mktime(&stm);
            size_t decimalNum = 0;
            if(p)
            {
                if(*p=='.')
                {
                    std::string decimals(p+1,&timeStr[timeStr.length()]);
                    while(decimals.length()<6)
                    {
                        decimals += "0";
                    }
                    decimalNum = (size_t)atol(decimals.c_str());
                }
                time_=std::make_shared<::trantor::Date>(t*1000000+decimalNum);
            }
        }
    }
}

const int32_t &Agree::getValueOfAgreeId() const noexcept
{
    const static int32_t defaultValue = int32_t();
    if(agreeId_)
        return *agreeId_;
    return defaultValue;
}
const std::shared_ptr<int32_t> &Agree::getAgreeId() const noexcept
{
    return agreeId_;
}
void Agree::setAgreeId(const int32_t &pAgreeId) noexcept
{
    agreeId_ = std::make_shared<int32_t>(pAgreeId);
    dirtyFlag_[0] = true;
}
const typename Agree::PrimaryKeyType & Agree::getPrimaryKey() const
{
    assert(agreeId_);
    return *agreeId_;
}

const int32_t &Agree::getValueOfUserId() const noexcept
{
    const static int32_t defaultValue = int32_t();
    if(userId_)
        return *userId_;
    return defaultValue;
}
const std::shared_ptr<int32_t> &Agree::getUserId() const noexcept
{
    return userId_;
}
void Agree::setUserId(const int32_t &pUserId) noexcept
{
    userId_ = std::make_shared<int32_t>(pUserId);
    dirtyFlag_[1] = true;
}

const int32_t &Agree::getValueOfNoteId() const noexcept
{
    const static int32_t defaultValue = int32_t();
    if(noteId_)
        return *noteId_;
    return defaultValue;
}
const std::shared_ptr<int32_t> &Agree::getNoteId() const noexcept
{
    return noteId_;
}
void Agree::setNoteId(const int32_t &pNoteId) noexcept
{
    noteId_ = std::make_shared<int32_t>(pNoteId);
    dirtyFlag_[2] = true;
}

const int32_t &Agree::getValueOfCommentId() const noexcept
{
    const static int32_t defaultValue = int32_t();
    if(commentId_)
        return *commentId_;
    return defaultValue;
}
const std::shared_ptr<int32_t> &Agree::getCommentId() const noexcept
{
    return commentId_;
}
void Agree::setCommentId(const int32_t &pCommentId) noexcept
{
    commentId_ = std::make_shared<int32_t>(pCommentId);
    dirtyFlag_[3] = true;
}

const int8_t &Agree::getValueOfStatus() const noexcept
{
    const static int8_t defaultValue = int8_t();
    if(status_)
        return *status_;
    return defaultValue;
}
const std::shared_ptr<int8_t> &Agree::getStatus() const noexcept
{
    return status_;
}
void Agree::setStatus(const int8_t &pStatus) noexcept
{
    status_ = std::make_shared<int8_t>(pStatus);
    dirtyFlag_[4] = true;
}

const ::trantor::Date &Agree::getValueOfTime() const noexcept
{
    const static ::trantor::Date defaultValue = ::trantor::Date();
    if(time_)
        return *time_;
    return defaultValue;
}
const std::shared_ptr<::trantor::Date> &Agree::getTime() const noexcept
{
    return time_;
}
void Agree::setTime(const ::trantor::Date &pTime) noexcept
{
    time_ = std::make_shared<::trantor::Date>(pTime);
    dirtyFlag_[5] = true;
}

void Agree::updateId(const uint64_t id)
{
    agreeId_ = std::make_shared<int32_t>(static_cast<int32_t>(id));
}

const std::vector<std::string> &Agree::insertColumns() noexcept
{
    static const std::vector<std::string> inCols={
        "User_ID",
        "Note_ID",
        "Comment_ID",
        "Status",
        "Time"
    };
    return inCols;
}

void Agree::outputArgs(drogon::orm::internal::SqlBinder &binder) const
{
    if(dirtyFlag_[1])
    {
        if(getUserId())
        {
            binder << getValueOfUserId();
        }
        else
        {
            binder << nullptr;
        }
    }
    if(dirtyFlag_[2])
    {
        if(getNoteId())
        {
            binder << getValueOfNoteId();
        }
        else
        {
            binder << nullptr;
        }
    }
    if(dirtyFlag_[3])
    {
        if(getCommentId())
        {
            binder << getValueOfCommentId();
        }
        else
        {
            binder << nullptr;
        }
    }
    if(dirtyFlag_[4])
    {
        if(getStatus())
        {
            binder << getValueOfStatus();
        }
        else
        {
            binder << nullptr;
        }
    }
    if(dirtyFlag_[5])
    {
        if(getTime())
        {
            binder << getValueOfTime();
        }
        else
        {
            binder << nullptr;
        }
    }
}

const std::vector<std::string> Agree::updateColumns() const
{
    std::vector<std::string> ret;
    if(dirtyFlag_[1])
    {
        ret.push_back(getColumnName(1));
    }
    if(dirtyFlag_[2])
    {
        ret.push_back(getColumnName(2));
    }
    if(dirtyFlag_[3])
    {
        ret.push_back(getColumnName(3));
    }
    if(dirtyFlag_[4])
    {
        ret.push_back(getColumnName(4));
    }
    if(dirtyFlag_[5])
    {
        ret.push_back(getColumnName(5));
    }
    return ret;
}

void Agree::updateArgs(drogon::orm::internal::SqlBinder &binder) const
{
    if(dirtyFlag_[1])
    {
        if(getUserId())
        {
            binder << getValueOfUserId();
        }
        else
        {
            binder << nullptr;
        }
    }
    if(dirtyFlag_[2])
    {
        if(getNoteId())
        {
            binder << getValueOfNoteId();
        }
        else
        {
            binder << nullptr;
        }
    }
    if(dirtyFlag_[3])
    {
        if(getCommentId())
        {
            binder << getValueOfCommentId();
        }
        else
        {
            binder << nullptr;
        }
    }
    if(dirtyFlag_[4])
    {
        if(getStatus())
        {
            binder << getValueOfStatus();
        }
        else
        {
            binder << nullptr;
        }
    }
    if(dirtyFlag_[5])
    {
        if(getTime())
        {
            binder << getValueOfTime();
        }
        else
        {
            binder << nullptr;
        }
    }
}
Json::Value Agree::toJson() const
{
    Json::Value ret;
    if(getAgreeId())
    {
        ret["Agree_ID"]=getValueOfAgreeId();
    }
    else
    {
        ret["Agree_ID"]=Json::Value();
    }
    if(getUserId())
    {
        ret["User_ID"]=getValueOfUserId();
    }
    else
    {
        ret["User_ID"]=Json::Value();
    }
    if(getNoteId())
    {
        ret["Note_ID"]=getValueOfNoteId();
    }
    else
    {
        ret["Note_ID"]=Json::Value();
    }
    if(getCommentId())
    {
        ret["Comment_ID"]=getValueOfCommentId();
    }
    else
    {
        ret["Comment_ID"]=Json::Value();
    }
    if(getStatus())
    {
        ret["Status"]=getValueOfStatus();
    }
    else
    {
        ret["Status"]=Json::Value();
    }
    if(getTime())
    {
        ret["Time"]=getTime()->toDbStringLocal();
    }
    else
    {
        ret["Time"]=Json::Value();
    }
    return ret;
}

Json::Value Agree::toMasqueradedJson(
    const std::vector<std::string> &pMasqueradingVector) const
{
    Json::Value ret;
    if(pMasqueradingVector.size() == 6)
    {
        if(!pMasqueradingVector[0].empty())
        {
            if(getAgreeId())
            {
                ret[pMasqueradingVector[0]]=getValueOfAgreeId();
            }
            else
            {
                ret[pMasqueradingVector[0]]=Json::Value();
            }
        }
        if(!pMasqueradingVector[1].empty())
        {
            if(getUserId())
            {
                ret[pMasqueradingVector[1]]=getValueOfUserId();
            }
            else
            {
                ret[pMasqueradingVector[1]]=Json::Value();
            }
        }
        if(!pMasqueradingVector[2].empty())
        {
            if(getNoteId())
            {
                ret[pMasqueradingVector[2]]=getValueOfNoteId();
            }
            else
            {
                ret[pMasqueradingVector[2]]=Json::Value();
            }
        }
        if(!pMasqueradingVector[3].empty())
        {
            if(getCommentId())
            {
                ret[pMasqueradingVector[3]]=getValueOfCommentId();
            }
            else
            {
                ret[pMasqueradingVector[3]]=Json::Value();
            }
        }
        if(!pMasqueradingVector[4].empty())
        {
            if(getStatus())
            {
                ret[pMasqueradingVector[4]]=getValueOfStatus();
            }
            else
            {
                ret[pMasqueradingVector[4]]=Json::Value();
            }
        }
        if(!pMasqueradingVector[5].empty())
        {
            if(getTime())
            {
                ret[pMasqueradingVector[5]]=getTime()->toDbStringLocal();
            }
            else
            {
                ret[pMasqueradingVector[5]]=Json::Value();
            }
        }
        return ret;
    }
    LOG_ERROR << "Masquerade failed";
    if(getAgreeId())
    {
        ret["Agree_ID"]=getValueOfAgreeId();
    }
    else
    {
        ret["Agree_ID"]=Json::Value();
    }
    if(getUserId())
    {
        ret["User_ID"]=getValueOfUserId();
    }
    else
    {
        ret["User_ID"]=Json::Value();
    }
    if(getNoteId())
    {
        ret["Note_ID"]=getValueOfNoteId();
    }
    else
    {
        ret["Note_ID"]=Json::Value();
    }
    if(getCommentId())
    {
        ret["Comment_ID"]=getValueOfCommentId();
    }
    else
    {
        ret["Comment_ID"]=Json::Value();
    }
    if(getStatus())
    {
        ret["Status"]=getValueOfStatus();
    }
    else
    {
        ret["Status"]=Json::Value();
    }
    if(getTime())
    {
        ret["Time"]=getTime()->toDbStringLocal();
    }
    else
    {
        ret["Time"]=Json::Value();
    }
    return ret;
}

bool Agree::validateJsonForCreation(const Json::Value &pJson, std::string &err)
{
    if(pJson.isMember("Agree_ID"))
    {
        if(!validJsonOfField(0, "Agree_ID", pJson["Agree_ID"], err, true))
            return false;
    }
    if(pJson.isMember("User_ID"))
    {
        if(!validJsonOfField(1, "User_ID", pJson["User_ID"], err, true))
            return false;
    }
    else
    {
        err="The User_ID column cannot be null";
        return false;
    }
    if(pJson.isMember("Note_ID"))
    {
        if(!validJsonOfField(2, "Note_ID", pJson["Note_ID"], err, true))
            return false;
    }
    else
    {
        err="The Note_ID column cannot be null";
        return false;
    }
    if(pJson.isMember("Comment_ID"))
    {
        if(!validJsonOfField(3, "Comment_ID", pJson["Comment_ID"], err, true))
            return false;
    }
    else
    {
        err="The Comment_ID column cannot be null";
        return false;
    }
    if(pJson.isMember("Status"))
    {
        if(!validJsonOfField(4, "Status", pJson["Status"], err, true))
            return false;
    }
    else
    {
        err="The Status column cannot be null";
        return false;
    }
    if(pJson.isMember("Time"))
    {
        if(!validJsonOfField(5, "Time", pJson["Time"], err, true))
            return false;
    }
    return true;
}
bool Agree::validateMasqueradedJsonForCreation(const Json::Value &pJson,
                                               const std::vector<std::string> &pMasqueradingVector,
                                               std::string &err)
{
    if(pMasqueradingVector.size() != 6)
    {
        err = "Bad masquerading vector";
        return false;
    }
    try {
      if(!pMasqueradingVector[0].empty())
      {
          if(pJson.isMember(pMasqueradingVector[0]))
          {
              if(!validJsonOfField(0, pMasqueradingVector[0], pJson[pMasqueradingVector[0]], err, true))
                  return false;
          }
      }
      if(!pMasqueradingVector[1].empty())
      {
          if(pJson.isMember(pMasqueradingVector[1]))
          {
              if(!validJsonOfField(1, pMasqueradingVector[1], pJson[pMasqueradingVector[1]], err, true))
                  return false;
          }
        else
        {
            err="The " + pMasqueradingVector[1] + " column cannot be null";
            return false;
        }
      }
      if(!pMasqueradingVector[2].empty())
      {
          if(pJson.isMember(pMasqueradingVector[2]))
          {
              if(!validJsonOfField(2, pMasqueradingVector[2], pJson[pMasqueradingVector[2]], err, true))
                  return false;
          }
        else
        {
            err="The " + pMasqueradingVector[2] + " column cannot be null";
            return false;
        }
      }
      if(!pMasqueradingVector[3].empty())
      {
          if(pJson.isMember(pMasqueradingVector[3]))
          {
              if(!validJsonOfField(3, pMasqueradingVector[3], pJson[pMasqueradingVector[3]], err, true))
                  return false;
          }
        else
        {
            err="The " + pMasqueradingVector[3] + " column cannot be null";
            return false;
        }
      }
      if(!pMasqueradingVector[4].empty())
      {
          if(pJson.isMember(pMasqueradingVector[4]))
          {
              if(!validJsonOfField(4, pMasqueradingVector[4], pJson[pMasqueradingVector[4]], err, true))
                  return false;
          }
        else
        {
            err="The " + pMasqueradingVector[4] + " column cannot be null";
            return false;
        }
      }
      if(!pMasqueradingVector[5].empty())
      {
          if(pJson.isMember(pMasqueradingVector[5]))
          {
              if(!validJsonOfField(5, pMasqueradingVector[5], pJson[pMasqueradingVector[5]], err, true))
                  return false;
          }
      }
    }
    catch(const Json::LogicError &e) 
    {
      err = e.what();
      return false;
    }
    return true;
}
bool Agree::validateJsonForUpdate(const Json::Value &pJson, std::string &err)
{
    if(pJson.isMember("Agree_ID"))
    {
        if(!validJsonOfField(0, "Agree_ID", pJson["Agree_ID"], err, false))
            return false;
    }
    else
    {
        err = "The value of primary key must be set in the json object for update";
        return false;
    }
    if(pJson.isMember("User_ID"))
    {
        if(!validJsonOfField(1, "User_ID", pJson["User_ID"], err, false))
            return false;
    }
    if(pJson.isMember("Note_ID"))
    {
        if(!validJsonOfField(2, "Note_ID", pJson["Note_ID"], err, false))
            return false;
    }
    if(pJson.isMember("Comment_ID"))
    {
        if(!validJsonOfField(3, "Comment_ID", pJson["Comment_ID"], err, false))
            return false;
    }
    if(pJson.isMember("Status"))
    {
        if(!validJsonOfField(4, "Status", pJson["Status"], err, false))
            return false;
    }
    if(pJson.isMember("Time"))
    {
        if(!validJsonOfField(5, "Time", pJson["Time"], err, false))
            return false;
    }
    return true;
}
bool Agree::validateMasqueradedJsonForUpdate(const Json::Value &pJson,
                                             const std::vector<std::string> &pMasqueradingVector,
                                             std::string &err)
{
    if(pMasqueradingVector.size() != 6)
    {
        err = "Bad masquerading vector";
        return false;
    }
    try {
      if(!pMasqueradingVector[0].empty() && pJson.isMember(pMasqueradingVector[0]))
      {
          if(!validJsonOfField(0, pMasqueradingVector[0], pJson[pMasqueradingVector[0]], err, false))
              return false;
      }
    else
    {
        err = "The value of primary key must be set in the json object for update";
        return false;
    }
      if(!pMasqueradingVector[1].empty() && pJson.isMember(pMasqueradingVector[1]))
      {
          if(!validJsonOfField(1, pMasqueradingVector[1], pJson[pMasqueradingVector[1]], err, false))
              return false;
      }
      if(!pMasqueradingVector[2].empty() && pJson.isMember(pMasqueradingVector[2]))
      {
          if(!validJsonOfField(2, pMasqueradingVector[2], pJson[pMasqueradingVector[2]], err, false))
              return false;
      }
      if(!pMasqueradingVector[3].empty() && pJson.isMember(pMasqueradingVector[3]))
      {
          if(!validJsonOfField(3, pMasqueradingVector[3], pJson[pMasqueradingVector[3]], err, false))
              return false;
      }
      if(!pMasqueradingVector[4].empty() && pJson.isMember(pMasqueradingVector[4]))
      {
          if(!validJsonOfField(4, pMasqueradingVector[4], pJson[pMasqueradingVector[4]], err, false))
              return false;
      }
      if(!pMasqueradingVector[5].empty() && pJson.isMember(pMasqueradingVector[5]))
      {
          if(!validJsonOfField(5, pMasqueradingVector[5], pJson[pMasqueradingVector[5]], err, false))
              return false;
      }
    }
    catch(const Json::LogicError &e) 
    {
      err = e.what();
      return false;
    }
    return true;
}
bool Agree::validJsonOfField(size_t index,
                             const std::string &fieldName,
                             const Json::Value &pJson, 
                             std::string &err, 
                             bool isForCreation)
{
    switch(index)
    {
        case 0:
            if(pJson.isNull())
            {
                err="The " + fieldName + " column cannot be null";
                return false;
            }
            if(isForCreation)
            {
                err="The automatic primary key cannot be set";
                return false;
            }        
            if(!pJson.isInt())
            {
                err="Type error in the "+fieldName+" field";
                return false;
            }
            break;
        case 1:
            if(pJson.isNull())
            {
                err="The " + fieldName + " column cannot be null";
                return false;
            }
            if(!pJson.isInt())
            {
                err="Type error in the "+fieldName+" field";
                return false;
            }
            break;
        case 2:
            if(pJson.isNull())
            {
                err="The " + fieldName + " column cannot be null";
                return false;
            }
            if(!pJson.isInt())
            {
                err="Type error in the "+fieldName+" field";
                return false;
            }
            break;
        case 3:
            if(pJson.isNull())
            {
                err="The " + fieldName + " column cannot be null";
                return false;
            }
            if(!pJson.isInt())
            {
                err="Type error in the "+fieldName+" field";
                return false;
            }
            break;
        case 4:
            if(pJson.isNull())
            {
                err="The " + fieldName + " column cannot be null";
                return false;
            }
            if(!pJson.isInt())
            {
                err="Type error in the "+fieldName+" field";
                return false;
            }
            break;
        case 5:
            if(pJson.isNull())
            {
                err="The " + fieldName + " column cannot be null";
                return false;
            }
            if(!pJson.isString())
            {
                err="Type error in the "+fieldName+" field";
                return false;                
            }
            break;
     
        default:
            err="Internal error in the server";
            return false;
            break;
    }
    return true;
}
