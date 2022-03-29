/**
 *
 *  Comment.cc
 *  DO NOT EDIT. This file is generated by drogon_ctl
 *
 */

#include "Comment.h"
#include <drogon/utils/Utilities.h>
#include <string>

using namespace drogon;
using namespace drogon_model::novel;

const std::string Comment::Cols::_Comment_ID = "Comment_ID";
const std::string Comment::Cols::_Note_ID = "Note_ID";
const std::string Comment::Cols::_Floor_ID = "Floor_ID";
const std::string Comment::Cols::_User_ID = "User_ID";
const std::string Comment::Cols::_Content = "Content";
const std::string Comment::Cols::_Create = "Create";
const std::string Comment::Cols::_Reply_Floor_ID = "Reply_Floor_ID";
const std::string Comment::primaryKeyName = "Comment_ID";
const bool Comment::hasPrimaryKey = true;
const std::string Comment::tableName = "comment";

const std::vector<typename Comment::MetaData> Comment::metaData_={
{"Comment_ID","int32_t","int(10)",4,1,1,1},
{"Note_ID","int32_t","int(10)",4,0,0,1},
{"Floor_ID","int32_t","int(10)",4,0,0,1},
{"User_ID","int32_t","int(10)",4,0,0,1},
{"Content","std::string","text",0,0,0,1},
{"Create","::trantor::Date","timestamp",0,0,0,1},
{"Reply_Floor_ID","int32_t","int(10)",4,0,0,1}
};
const std::string &Comment::getColumnName(size_t index) noexcept(false)
{
    assert(index < metaData_.size());
    return metaData_[index].colName_;
}
Comment::Comment(const Row &r, const ssize_t indexOffset) noexcept
{
    if(indexOffset < 0)
    {
        if(!r["Comment_ID"].isNull())
        {
            commentId_=std::make_shared<int32_t>(r["Comment_ID"].as<int32_t>());
        }
        if(!r["Note_ID"].isNull())
        {
            noteId_=std::make_shared<int32_t>(r["Note_ID"].as<int32_t>());
        }
        if(!r["Floor_ID"].isNull())
        {
            floorId_=std::make_shared<int32_t>(r["Floor_ID"].as<int32_t>());
        }
        if(!r["User_ID"].isNull())
        {
            userId_=std::make_shared<int32_t>(r["User_ID"].as<int32_t>());
        }
        if(!r["Content"].isNull())
        {
            content_=std::make_shared<std::string>(r["Content"].as<std::string>());
        }
        if(!r["Create"].isNull())
        {
            auto timeStr = r["Create"].as<std::string>();
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
                create_=std::make_shared<::trantor::Date>(t*1000000+decimalNum);
            }
        }
        if(!r["Reply_Floor_ID"].isNull())
        {
            replyFloorId_=std::make_shared<int32_t>(r["Reply_Floor_ID"].as<int32_t>());
        }
    }
    else
    {
        size_t offset = (size_t)indexOffset;
        if(offset + 7 > r.size())
        {
            LOG_FATAL << "Invalid SQL result for this model";
            return;
        }
        size_t index;
        index = offset + 0;
        if(!r[index].isNull())
        {
            commentId_=std::make_shared<int32_t>(r[index].as<int32_t>());
        }
        index = offset + 1;
        if(!r[index].isNull())
        {
            noteId_=std::make_shared<int32_t>(r[index].as<int32_t>());
        }
        index = offset + 2;
        if(!r[index].isNull())
        {
            floorId_=std::make_shared<int32_t>(r[index].as<int32_t>());
        }
        index = offset + 3;
        if(!r[index].isNull())
        {
            userId_=std::make_shared<int32_t>(r[index].as<int32_t>());
        }
        index = offset + 4;
        if(!r[index].isNull())
        {
            content_=std::make_shared<std::string>(r[index].as<std::string>());
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
                create_=std::make_shared<::trantor::Date>(t*1000000+decimalNum);
            }
        }
        index = offset + 6;
        if(!r[index].isNull())
        {
            replyFloorId_=std::make_shared<int32_t>(r[index].as<int32_t>());
        }
    }

}

Comment::Comment(const Json::Value &pJson, const std::vector<std::string> &pMasqueradingVector) noexcept(false)
{
    if(pMasqueradingVector.size() != 7)
    {
        LOG_ERROR << "Bad masquerading vector";
        return;
    }
    if(!pMasqueradingVector[0].empty() && pJson.isMember(pMasqueradingVector[0]))
    {
        dirtyFlag_[0] = true;
        if(!pJson[pMasqueradingVector[0]].isNull())
        {
            commentId_=std::make_shared<int32_t>((int32_t)pJson[pMasqueradingVector[0]].asInt64());
        }
    }
    if(!pMasqueradingVector[1].empty() && pJson.isMember(pMasqueradingVector[1]))
    {
        dirtyFlag_[1] = true;
        if(!pJson[pMasqueradingVector[1]].isNull())
        {
            noteId_=std::make_shared<int32_t>((int32_t)pJson[pMasqueradingVector[1]].asInt64());
        }
    }
    if(!pMasqueradingVector[2].empty() && pJson.isMember(pMasqueradingVector[2]))
    {
        dirtyFlag_[2] = true;
        if(!pJson[pMasqueradingVector[2]].isNull())
        {
            floorId_=std::make_shared<int32_t>((int32_t)pJson[pMasqueradingVector[2]].asInt64());
        }
    }
    if(!pMasqueradingVector[3].empty() && pJson.isMember(pMasqueradingVector[3]))
    {
        dirtyFlag_[3] = true;
        if(!pJson[pMasqueradingVector[3]].isNull())
        {
            userId_=std::make_shared<int32_t>((int32_t)pJson[pMasqueradingVector[3]].asInt64());
        }
    }
    if(!pMasqueradingVector[4].empty() && pJson.isMember(pMasqueradingVector[4]))
    {
        dirtyFlag_[4] = true;
        if(!pJson[pMasqueradingVector[4]].isNull())
        {
            content_=std::make_shared<std::string>(pJson[pMasqueradingVector[4]].asString());
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
                create_=std::make_shared<::trantor::Date>(t*1000000+decimalNum);
            }
        }
    }
    if(!pMasqueradingVector[6].empty() && pJson.isMember(pMasqueradingVector[6]))
    {
        dirtyFlag_[6] = true;
        if(!pJson[pMasqueradingVector[6]].isNull())
        {
            replyFloorId_=std::make_shared<int32_t>((int32_t)pJson[pMasqueradingVector[6]].asInt64());
        }
    }
}

Comment::Comment(const Json::Value &pJson) noexcept(false)
{
    if(pJson.isMember("Comment_ID"))
    {
        dirtyFlag_[0]=true;
        if(!pJson["Comment_ID"].isNull())
        {
            commentId_=std::make_shared<int32_t>((int32_t)pJson["Comment_ID"].asInt64());
        }
    }
    if(pJson.isMember("Note_ID"))
    {
        dirtyFlag_[1]=true;
        if(!pJson["Note_ID"].isNull())
        {
            noteId_=std::make_shared<int32_t>((int32_t)pJson["Note_ID"].asInt64());
        }
    }
    if(pJson.isMember("Floor_ID"))
    {
        dirtyFlag_[2]=true;
        if(!pJson["Floor_ID"].isNull())
        {
            floorId_=std::make_shared<int32_t>((int32_t)pJson["Floor_ID"].asInt64());
        }
    }
    if(pJson.isMember("User_ID"))
    {
        dirtyFlag_[3]=true;
        if(!pJson["User_ID"].isNull())
        {
            userId_=std::make_shared<int32_t>((int32_t)pJson["User_ID"].asInt64());
        }
    }
    if(pJson.isMember("Content"))
    {
        dirtyFlag_[4]=true;
        if(!pJson["Content"].isNull())
        {
            content_=std::make_shared<std::string>(pJson["Content"].asString());
        }
    }
    if(pJson.isMember("Create"))
    {
        dirtyFlag_[5]=true;
        if(!pJson["Create"].isNull())
        {
            auto timeStr = pJson["Create"].asString();
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
                create_=std::make_shared<::trantor::Date>(t*1000000+decimalNum);
            }
        }
    }
    if(pJson.isMember("Reply_Floor_ID"))
    {
        dirtyFlag_[6]=true;
        if(!pJson["Reply_Floor_ID"].isNull())
        {
            replyFloorId_=std::make_shared<int32_t>((int32_t)pJson["Reply_Floor_ID"].asInt64());
        }
    }
}

void Comment::updateByMasqueradedJson(const Json::Value &pJson,
                                            const std::vector<std::string> &pMasqueradingVector) noexcept(false)
{
    if(pMasqueradingVector.size() != 7)
    {
        LOG_ERROR << "Bad masquerading vector";
        return;
    }
    if(!pMasqueradingVector[0].empty() && pJson.isMember(pMasqueradingVector[0]))
    {
        if(!pJson[pMasqueradingVector[0]].isNull())
        {
            commentId_=std::make_shared<int32_t>((int32_t)pJson[pMasqueradingVector[0]].asInt64());
        }
    }
    if(!pMasqueradingVector[1].empty() && pJson.isMember(pMasqueradingVector[1]))
    {
        dirtyFlag_[1] = true;
        if(!pJson[pMasqueradingVector[1]].isNull())
        {
            noteId_=std::make_shared<int32_t>((int32_t)pJson[pMasqueradingVector[1]].asInt64());
        }
    }
    if(!pMasqueradingVector[2].empty() && pJson.isMember(pMasqueradingVector[2]))
    {
        dirtyFlag_[2] = true;
        if(!pJson[pMasqueradingVector[2]].isNull())
        {
            floorId_=std::make_shared<int32_t>((int32_t)pJson[pMasqueradingVector[2]].asInt64());
        }
    }
    if(!pMasqueradingVector[3].empty() && pJson.isMember(pMasqueradingVector[3]))
    {
        dirtyFlag_[3] = true;
        if(!pJson[pMasqueradingVector[3]].isNull())
        {
            userId_=std::make_shared<int32_t>((int32_t)pJson[pMasqueradingVector[3]].asInt64());
        }
    }
    if(!pMasqueradingVector[4].empty() && pJson.isMember(pMasqueradingVector[4]))
    {
        dirtyFlag_[4] = true;
        if(!pJson[pMasqueradingVector[4]].isNull())
        {
            content_=std::make_shared<std::string>(pJson[pMasqueradingVector[4]].asString());
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
                create_=std::make_shared<::trantor::Date>(t*1000000+decimalNum);
            }
        }
    }
    if(!pMasqueradingVector[6].empty() && pJson.isMember(pMasqueradingVector[6]))
    {
        dirtyFlag_[6] = true;
        if(!pJson[pMasqueradingVector[6]].isNull())
        {
            replyFloorId_=std::make_shared<int32_t>((int32_t)pJson[pMasqueradingVector[6]].asInt64());
        }
    }
}
                                                                    
void Comment::updateByJson(const Json::Value &pJson) noexcept(false)
{
    if(pJson.isMember("Comment_ID"))
    {
        if(!pJson["Comment_ID"].isNull())
        {
            commentId_=std::make_shared<int32_t>((int32_t)pJson["Comment_ID"].asInt64());
        }
    }
    if(pJson.isMember("Note_ID"))
    {
        dirtyFlag_[1] = true;
        if(!pJson["Note_ID"].isNull())
        {
            noteId_=std::make_shared<int32_t>((int32_t)pJson["Note_ID"].asInt64());
        }
    }
    if(pJson.isMember("Floor_ID"))
    {
        dirtyFlag_[2] = true;
        if(!pJson["Floor_ID"].isNull())
        {
            floorId_=std::make_shared<int32_t>((int32_t)pJson["Floor_ID"].asInt64());
        }
    }
    if(pJson.isMember("User_ID"))
    {
        dirtyFlag_[3] = true;
        if(!pJson["User_ID"].isNull())
        {
            userId_=std::make_shared<int32_t>((int32_t)pJson["User_ID"].asInt64());
        }
    }
    if(pJson.isMember("Content"))
    {
        dirtyFlag_[4] = true;
        if(!pJson["Content"].isNull())
        {
            content_=std::make_shared<std::string>(pJson["Content"].asString());
        }
    }
    if(pJson.isMember("Create"))
    {
        dirtyFlag_[5] = true;
        if(!pJson["Create"].isNull())
        {
            auto timeStr = pJson["Create"].asString();
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
                create_=std::make_shared<::trantor::Date>(t*1000000+decimalNum);
            }
        }
    }
    if(pJson.isMember("Reply_Floor_ID"))
    {
        dirtyFlag_[6] = true;
        if(!pJson["Reply_Floor_ID"].isNull())
        {
            replyFloorId_=std::make_shared<int32_t>((int32_t)pJson["Reply_Floor_ID"].asInt64());
        }
    }
}

const int32_t &Comment::getValueOfCommentId() const noexcept
{
    const static int32_t defaultValue = int32_t();
    if(commentId_)
        return *commentId_;
    return defaultValue;
}
const std::shared_ptr<int32_t> &Comment::getCommentId() const noexcept
{
    return commentId_;
}
void Comment::setCommentId(const int32_t &pCommentId) noexcept
{
    commentId_ = std::make_shared<int32_t>(pCommentId);
    dirtyFlag_[0] = true;
}
const typename Comment::PrimaryKeyType & Comment::getPrimaryKey() const
{
    assert(commentId_);
    return *commentId_;
}

const int32_t &Comment::getValueOfNoteId() const noexcept
{
    const static int32_t defaultValue = int32_t();
    if(noteId_)
        return *noteId_;
    return defaultValue;
}
const std::shared_ptr<int32_t> &Comment::getNoteId() const noexcept
{
    return noteId_;
}
void Comment::setNoteId(const int32_t &pNoteId) noexcept
{
    noteId_ = std::make_shared<int32_t>(pNoteId);
    dirtyFlag_[1] = true;
}

const int32_t &Comment::getValueOfFloorId() const noexcept
{
    const static int32_t defaultValue = int32_t();
    if(floorId_)
        return *floorId_;
    return defaultValue;
}
const std::shared_ptr<int32_t> &Comment::getFloorId() const noexcept
{
    return floorId_;
}
void Comment::setFloorId(const int32_t &pFloorId) noexcept
{
    floorId_ = std::make_shared<int32_t>(pFloorId);
    dirtyFlag_[2] = true;
}

const int32_t &Comment::getValueOfUserId() const noexcept
{
    const static int32_t defaultValue = int32_t();
    if(userId_)
        return *userId_;
    return defaultValue;
}
const std::shared_ptr<int32_t> &Comment::getUserId() const noexcept
{
    return userId_;
}
void Comment::setUserId(const int32_t &pUserId) noexcept
{
    userId_ = std::make_shared<int32_t>(pUserId);
    dirtyFlag_[3] = true;
}

const std::string &Comment::getValueOfContent() const noexcept
{
    const static std::string defaultValue = std::string();
    if(content_)
        return *content_;
    return defaultValue;
}
const std::shared_ptr<std::string> &Comment::getContent() const noexcept
{
    return content_;
}
void Comment::setContent(const std::string &pContent) noexcept
{
    content_ = std::make_shared<std::string>(pContent);
    dirtyFlag_[4] = true;
}
void Comment::setContent(std::string &&pContent) noexcept
{
    content_ = std::make_shared<std::string>(std::move(pContent));
    dirtyFlag_[4] = true;
}

const ::trantor::Date &Comment::getValueOfCreate() const noexcept
{
    const static ::trantor::Date defaultValue = ::trantor::Date();
    if(create_)
        return *create_;
    return defaultValue;
}
const std::shared_ptr<::trantor::Date> &Comment::getCreate() const noexcept
{
    return create_;
}
void Comment::setCreate(const ::trantor::Date &pCreate) noexcept
{
    create_ = std::make_shared<::trantor::Date>(pCreate);
    dirtyFlag_[5] = true;
}

const int32_t &Comment::getValueOfReplyFloorId() const noexcept
{
    const static int32_t defaultValue = int32_t();
    if(replyFloorId_)
        return *replyFloorId_;
    return defaultValue;
}
const std::shared_ptr<int32_t> &Comment::getReplyFloorId() const noexcept
{
    return replyFloorId_;
}
void Comment::setReplyFloorId(const int32_t &pReplyFloorId) noexcept
{
    replyFloorId_ = std::make_shared<int32_t>(pReplyFloorId);
    dirtyFlag_[6] = true;
}

void Comment::updateId(const uint64_t id)
{
    commentId_ = std::make_shared<int32_t>(static_cast<int32_t>(id));
}

const std::vector<std::string> &Comment::insertColumns() noexcept
{
    static const std::vector<std::string> inCols={
        "Note_ID",
        "Floor_ID",
        "User_ID",
        "Content",
        "Create",
        "Reply_Floor_ID"
    };
    return inCols;
}

void Comment::outputArgs(drogon::orm::internal::SqlBinder &binder) const
{
    if(dirtyFlag_[1])
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
    if(dirtyFlag_[2])
    {
        if(getFloorId())
        {
            binder << getValueOfFloorId();
        }
        else
        {
            binder << nullptr;
        }
    }
    if(dirtyFlag_[3])
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
    if(dirtyFlag_[4])
    {
        if(getContent())
        {
            binder << getValueOfContent();
        }
        else
        {
            binder << nullptr;
        }
    }
    if(dirtyFlag_[5])
    {
        if(getCreate())
        {
            binder << getValueOfCreate();
        }
        else
        {
            binder << nullptr;
        }
    }
    if(dirtyFlag_[6])
    {
        if(getReplyFloorId())
        {
            binder << getValueOfReplyFloorId();
        }
        else
        {
            binder << nullptr;
        }
    }
}

const std::vector<std::string> Comment::updateColumns() const
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
    if(dirtyFlag_[6])
    {
        ret.push_back(getColumnName(6));
    }
    return ret;
}

void Comment::updateArgs(drogon::orm::internal::SqlBinder &binder) const
{
    if(dirtyFlag_[1])
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
    if(dirtyFlag_[2])
    {
        if(getFloorId())
        {
            binder << getValueOfFloorId();
        }
        else
        {
            binder << nullptr;
        }
    }
    if(dirtyFlag_[3])
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
    if(dirtyFlag_[4])
    {
        if(getContent())
        {
            binder << getValueOfContent();
        }
        else
        {
            binder << nullptr;
        }
    }
    if(dirtyFlag_[5])
    {
        if(getCreate())
        {
            binder << getValueOfCreate();
        }
        else
        {
            binder << nullptr;
        }
    }
    if(dirtyFlag_[6])
    {
        if(getReplyFloorId())
        {
            binder << getValueOfReplyFloorId();
        }
        else
        {
            binder << nullptr;
        }
    }
}
Json::Value Comment::toJson() const
{
    Json::Value ret;
    if(getCommentId())
    {
        ret["Comment_ID"]=getValueOfCommentId();
    }
    else
    {
        ret["Comment_ID"]=Json::Value();
    }
    if(getNoteId())
    {
        ret["Note_ID"]=getValueOfNoteId();
    }
    else
    {
        ret["Note_ID"]=Json::Value();
    }
    if(getFloorId())
    {
        ret["Floor_ID"]=getValueOfFloorId();
    }
    else
    {
        ret["Floor_ID"]=Json::Value();
    }
    if(getUserId())
    {
        ret["User_ID"]=getValueOfUserId();
    }
    else
    {
        ret["User_ID"]=Json::Value();
    }
    if(getContent())
    {
        ret["Content"]=getValueOfContent();
    }
    else
    {
        ret["Content"]=Json::Value();
    }
    if(getCreate())
    {
        ret["Create"]=getCreate()->toDbStringLocal();
    }
    else
    {
        ret["Create"]=Json::Value();
    }
    if(getReplyFloorId())
    {
        ret["Reply_Floor_ID"]=getValueOfReplyFloorId();
    }
    else
    {
        ret["Reply_Floor_ID"]=Json::Value();
    }
    return ret;
}

Json::Value Comment::toMasqueradedJson(
    const std::vector<std::string> &pMasqueradingVector) const
{
    Json::Value ret;
    if(pMasqueradingVector.size() == 7)
    {
        if(!pMasqueradingVector[0].empty())
        {
            if(getCommentId())
            {
                ret[pMasqueradingVector[0]]=getValueOfCommentId();
            }
            else
            {
                ret[pMasqueradingVector[0]]=Json::Value();
            }
        }
        if(!pMasqueradingVector[1].empty())
        {
            if(getNoteId())
            {
                ret[pMasqueradingVector[1]]=getValueOfNoteId();
            }
            else
            {
                ret[pMasqueradingVector[1]]=Json::Value();
            }
        }
        if(!pMasqueradingVector[2].empty())
        {
            if(getFloorId())
            {
                ret[pMasqueradingVector[2]]=getValueOfFloorId();
            }
            else
            {
                ret[pMasqueradingVector[2]]=Json::Value();
            }
        }
        if(!pMasqueradingVector[3].empty())
        {
            if(getUserId())
            {
                ret[pMasqueradingVector[3]]=getValueOfUserId();
            }
            else
            {
                ret[pMasqueradingVector[3]]=Json::Value();
            }
        }
        if(!pMasqueradingVector[4].empty())
        {
            if(getContent())
            {
                ret[pMasqueradingVector[4]]=getValueOfContent();
            }
            else
            {
                ret[pMasqueradingVector[4]]=Json::Value();
            }
        }
        if(!pMasqueradingVector[5].empty())
        {
            if(getCreate())
            {
                ret[pMasqueradingVector[5]]=getCreate()->toDbStringLocal();
            }
            else
            {
                ret[pMasqueradingVector[5]]=Json::Value();
            }
        }
        if(!pMasqueradingVector[6].empty())
        {
            if(getReplyFloorId())
            {
                ret[pMasqueradingVector[6]]=getValueOfReplyFloorId();
            }
            else
            {
                ret[pMasqueradingVector[6]]=Json::Value();
            }
        }
        return ret;
    }
    LOG_ERROR << "Masquerade failed";
    if(getCommentId())
    {
        ret["Comment_ID"]=getValueOfCommentId();
    }
    else
    {
        ret["Comment_ID"]=Json::Value();
    }
    if(getNoteId())
    {
        ret["Note_ID"]=getValueOfNoteId();
    }
    else
    {
        ret["Note_ID"]=Json::Value();
    }
    if(getFloorId())
    {
        ret["Floor_ID"]=getValueOfFloorId();
    }
    else
    {
        ret["Floor_ID"]=Json::Value();
    }
    if(getUserId())
    {
        ret["User_ID"]=getValueOfUserId();
    }
    else
    {
        ret["User_ID"]=Json::Value();
    }
    if(getContent())
    {
        ret["Content"]=getValueOfContent();
    }
    else
    {
        ret["Content"]=Json::Value();
    }
    if(getCreate())
    {
        ret["Create"]=getCreate()->toDbStringLocal();
    }
    else
    {
        ret["Create"]=Json::Value();
    }
    if(getReplyFloorId())
    {
        ret["Reply_Floor_ID"]=getValueOfReplyFloorId();
    }
    else
    {
        ret["Reply_Floor_ID"]=Json::Value();
    }
    return ret;
}

bool Comment::validateJsonForCreation(const Json::Value &pJson, std::string &err)
{
    if(pJson.isMember("Comment_ID"))
    {
        if(!validJsonOfField(0, "Comment_ID", pJson["Comment_ID"], err, true))
            return false;
    }
    if(pJson.isMember("Note_ID"))
    {
        if(!validJsonOfField(1, "Note_ID", pJson["Note_ID"], err, true))
            return false;
    }
    else
    {
        err="The Note_ID column cannot be null";
        return false;
    }
    if(pJson.isMember("Floor_ID"))
    {
        if(!validJsonOfField(2, "Floor_ID", pJson["Floor_ID"], err, true))
            return false;
    }
    else
    {
        err="The Floor_ID column cannot be null";
        return false;
    }
    if(pJson.isMember("User_ID"))
    {
        if(!validJsonOfField(3, "User_ID", pJson["User_ID"], err, true))
            return false;
    }
    else
    {
        err="The User_ID column cannot be null";
        return false;
    }
    if(pJson.isMember("Content"))
    {
        if(!validJsonOfField(4, "Content", pJson["Content"], err, true))
            return false;
    }
    else
    {
        err="The Content column cannot be null";
        return false;
    }
    if(pJson.isMember("Create"))
    {
        if(!validJsonOfField(5, "Create", pJson["Create"], err, true))
            return false;
    }
    if(pJson.isMember("Reply_Floor_ID"))
    {
        if(!validJsonOfField(6, "Reply_Floor_ID", pJson["Reply_Floor_ID"], err, true))
            return false;
    }
    else
    {
        err="The Reply_Floor_ID column cannot be null";
        return false;
    }
    return true;
}
bool Comment::validateMasqueradedJsonForCreation(const Json::Value &pJson,
                                                 const std::vector<std::string> &pMasqueradingVector,
                                                 std::string &err)
{
    if(pMasqueradingVector.size() != 7)
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
      if(!pMasqueradingVector[6].empty())
      {
          if(pJson.isMember(pMasqueradingVector[6]))
          {
              if(!validJsonOfField(6, pMasqueradingVector[6], pJson[pMasqueradingVector[6]], err, true))
                  return false;
          }
        else
        {
            err="The " + pMasqueradingVector[6] + " column cannot be null";
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
bool Comment::validateJsonForUpdate(const Json::Value &pJson, std::string &err)
{
    if(pJson.isMember("Comment_ID"))
    {
        if(!validJsonOfField(0, "Comment_ID", pJson["Comment_ID"], err, false))
            return false;
    }
    else
    {
        err = "The value of primary key must be set in the json object for update";
        return false;
    }
    if(pJson.isMember("Note_ID"))
    {
        if(!validJsonOfField(1, "Note_ID", pJson["Note_ID"], err, false))
            return false;
    }
    if(pJson.isMember("Floor_ID"))
    {
        if(!validJsonOfField(2, "Floor_ID", pJson["Floor_ID"], err, false))
            return false;
    }
    if(pJson.isMember("User_ID"))
    {
        if(!validJsonOfField(3, "User_ID", pJson["User_ID"], err, false))
            return false;
    }
    if(pJson.isMember("Content"))
    {
        if(!validJsonOfField(4, "Content", pJson["Content"], err, false))
            return false;
    }
    if(pJson.isMember("Create"))
    {
        if(!validJsonOfField(5, "Create", pJson["Create"], err, false))
            return false;
    }
    if(pJson.isMember("Reply_Floor_ID"))
    {
        if(!validJsonOfField(6, "Reply_Floor_ID", pJson["Reply_Floor_ID"], err, false))
            return false;
    }
    return true;
}
bool Comment::validateMasqueradedJsonForUpdate(const Json::Value &pJson,
                                               const std::vector<std::string> &pMasqueradingVector,
                                               std::string &err)
{
    if(pMasqueradingVector.size() != 7)
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
      if(!pMasqueradingVector[6].empty() && pJson.isMember(pMasqueradingVector[6]))
      {
          if(!validJsonOfField(6, pMasqueradingVector[6], pJson[pMasqueradingVector[6]], err, false))
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
bool Comment::validJsonOfField(size_t index,
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
            if(!pJson.isString())
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
        case 6:
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
     
        default:
            err="Internal error in the server";
            return false;
            break;
    }
    return true;
}
