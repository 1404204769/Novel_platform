/**
 *
 *  Action.cc
 *  DO NOT EDIT. This file is generated by drogon_ctl
 *
 */

#include "Action.h"
#include <drogon/utils/Utilities.h>
#include <string>

using namespace drogon;
using namespace drogon_model::novel;

const std::string Action::Cols::_Action_ID = "Action_ID";
const std::string Action::Cols::_User_ID = "User_ID";
const std::string Action::Cols::_Type = "Type";
const std::string Action::Cols::_Memo = "Memo";
const std::string Action::primaryKeyName = "Action_ID";
const bool Action::hasPrimaryKey = true;
const std::string Action::tableName = "action";

const std::vector<typename Action::MetaData> Action::metaData_={
{"Action_ID","int32_t","int(10)",4,1,1,1},
{"User_ID","int32_t","int(10)",4,0,0,1},
{"Type","std::string","varchar(255)",255,0,0,1},
{"Memo","std::string","text",0,0,0,1}
};
const std::string &Action::getColumnName(size_t index) noexcept(false)
{
    assert(index < metaData_.size());
    return metaData_[index].colName_;
}
Action::Action(const Row &r, const ssize_t indexOffset) noexcept
{
    if(indexOffset < 0)
    {
        if(!r["Action_ID"].isNull())
        {
            actionId_=std::make_shared<int32_t>(r["Action_ID"].as<int32_t>());
        }
        if(!r["User_ID"].isNull())
        {
            userId_=std::make_shared<int32_t>(r["User_ID"].as<int32_t>());
        }
        if(!r["Type"].isNull())
        {
            type_=std::make_shared<std::string>(r["Type"].as<std::string>());
        }
        if(!r["Memo"].isNull())
        {
            memo_=std::make_shared<std::string>(r["Memo"].as<std::string>());
        }
    }
    else
    {
        size_t offset = (size_t)indexOffset;
        if(offset + 4 > r.size())
        {
            LOG_FATAL << "Invalid SQL result for this model";
            return;
        }
        size_t index;
        index = offset + 0;
        if(!r[index].isNull())
        {
            actionId_=std::make_shared<int32_t>(r[index].as<int32_t>());
        }
        index = offset + 1;
        if(!r[index].isNull())
        {
            userId_=std::make_shared<int32_t>(r[index].as<int32_t>());
        }
        index = offset + 2;
        if(!r[index].isNull())
        {
            type_=std::make_shared<std::string>(r[index].as<std::string>());
        }
        index = offset + 3;
        if(!r[index].isNull())
        {
            memo_=std::make_shared<std::string>(r[index].as<std::string>());
        }
    }

}

Action::Action(const Json::Value &pJson, const std::vector<std::string> &pMasqueradingVector) noexcept(false)
{
    if(pMasqueradingVector.size() != 4)
    {
        LOG_ERROR << "Bad masquerading vector";
        return;
    }
    if(!pMasqueradingVector[0].empty() && pJson.isMember(pMasqueradingVector[0]))
    {
        dirtyFlag_[0] = true;
        if(!pJson[pMasqueradingVector[0]].isNull())
        {
            actionId_=std::make_shared<int32_t>((int32_t)pJson[pMasqueradingVector[0]].asInt64());
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
            type_=std::make_shared<std::string>(pJson[pMasqueradingVector[2]].asString());
        }
    }
    if(!pMasqueradingVector[3].empty() && pJson.isMember(pMasqueradingVector[3]))
    {
        dirtyFlag_[3] = true;
        if(!pJson[pMasqueradingVector[3]].isNull())
        {
            memo_=std::make_shared<std::string>(pJson[pMasqueradingVector[3]].asString());
        }
    }
}

Action::Action(const Json::Value &pJson) noexcept(false)
{
    if(pJson.isMember("Action_ID"))
    {
        dirtyFlag_[0]=true;
        if(!pJson["Action_ID"].isNull())
        {
            actionId_=std::make_shared<int32_t>((int32_t)pJson["Action_ID"].asInt64());
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
    if(pJson.isMember("Type"))
    {
        dirtyFlag_[2]=true;
        if(!pJson["Type"].isNull())
        {
            type_=std::make_shared<std::string>(pJson["Type"].asString());
        }
    }
    if(pJson.isMember("Memo"))
    {
        dirtyFlag_[3]=true;
        if(!pJson["Memo"].isNull())
        {
            memo_=std::make_shared<std::string>(pJson["Memo"].asString());
        }
    }
}

void Action::updateByMasqueradedJson(const Json::Value &pJson,
                                            const std::vector<std::string> &pMasqueradingVector) noexcept(false)
{
    if(pMasqueradingVector.size() != 4)
    {
        LOG_ERROR << "Bad masquerading vector";
        return;
    }
    if(!pMasqueradingVector[0].empty() && pJson.isMember(pMasqueradingVector[0]))
    {
        if(!pJson[pMasqueradingVector[0]].isNull())
        {
            actionId_=std::make_shared<int32_t>((int32_t)pJson[pMasqueradingVector[0]].asInt64());
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
            type_=std::make_shared<std::string>(pJson[pMasqueradingVector[2]].asString());
        }
    }
    if(!pMasqueradingVector[3].empty() && pJson.isMember(pMasqueradingVector[3]))
    {
        dirtyFlag_[3] = true;
        if(!pJson[pMasqueradingVector[3]].isNull())
        {
            memo_=std::make_shared<std::string>(pJson[pMasqueradingVector[3]].asString());
        }
    }
}
                                                                    
void Action::updateByJson(const Json::Value &pJson) noexcept(false)
{
    if(pJson.isMember("Action_ID"))
    {
        if(!pJson["Action_ID"].isNull())
        {
            actionId_=std::make_shared<int32_t>((int32_t)pJson["Action_ID"].asInt64());
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
    if(pJson.isMember("Type"))
    {
        dirtyFlag_[2] = true;
        if(!pJson["Type"].isNull())
        {
            type_=std::make_shared<std::string>(pJson["Type"].asString());
        }
    }
    if(pJson.isMember("Memo"))
    {
        dirtyFlag_[3] = true;
        if(!pJson["Memo"].isNull())
        {
            memo_=std::make_shared<std::string>(pJson["Memo"].asString());
        }
    }
}

const int32_t &Action::getValueOfActionId() const noexcept
{
    const static int32_t defaultValue = int32_t();
    if(actionId_)
        return *actionId_;
    return defaultValue;
}
const std::shared_ptr<int32_t> &Action::getActionId() const noexcept
{
    return actionId_;
}
void Action::setActionId(const int32_t &pActionId) noexcept
{
    actionId_ = std::make_shared<int32_t>(pActionId);
    dirtyFlag_[0] = true;
}
const typename Action::PrimaryKeyType & Action::getPrimaryKey() const
{
    assert(actionId_);
    return *actionId_;
}

const int32_t &Action::getValueOfUserId() const noexcept
{
    const static int32_t defaultValue = int32_t();
    if(userId_)
        return *userId_;
    return defaultValue;
}
const std::shared_ptr<int32_t> &Action::getUserId() const noexcept
{
    return userId_;
}
void Action::setUserId(const int32_t &pUserId) noexcept
{
    userId_ = std::make_shared<int32_t>(pUserId);
    dirtyFlag_[1] = true;
}

const std::string &Action::getValueOfType() const noexcept
{
    const static std::string defaultValue = std::string();
    if(type_)
        return *type_;
    return defaultValue;
}
const std::shared_ptr<std::string> &Action::getType() const noexcept
{
    return type_;
}
void Action::setType(const std::string &pType) noexcept
{
    type_ = std::make_shared<std::string>(pType);
    dirtyFlag_[2] = true;
}
void Action::setType(std::string &&pType) noexcept
{
    type_ = std::make_shared<std::string>(std::move(pType));
    dirtyFlag_[2] = true;
}

const std::string &Action::getValueOfMemo() const noexcept
{
    const static std::string defaultValue = std::string();
    if(memo_)
        return *memo_;
    return defaultValue;
}
const std::shared_ptr<std::string> &Action::getMemo() const noexcept
{
    return memo_;
}
void Action::setMemo(const std::string &pMemo) noexcept
{
    memo_ = std::make_shared<std::string>(pMemo);
    dirtyFlag_[3] = true;
}
void Action::setMemo(std::string &&pMemo) noexcept
{
    memo_ = std::make_shared<std::string>(std::move(pMemo));
    dirtyFlag_[3] = true;
}

void Action::updateId(const uint64_t id)
{
    actionId_ = std::make_shared<int32_t>(static_cast<int32_t>(id));
}

const std::vector<std::string> &Action::insertColumns() noexcept
{
    static const std::vector<std::string> inCols={
        "User_ID",
        "Type",
        "Memo"
    };
    return inCols;
}

void Action::outputArgs(drogon::orm::internal::SqlBinder &binder) const
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
        if(getType())
        {
            binder << getValueOfType();
        }
        else
        {
            binder << nullptr;
        }
    }
    if(dirtyFlag_[3])
    {
        if(getMemo())
        {
            binder << getValueOfMemo();
        }
        else
        {
            binder << nullptr;
        }
    }
}

const std::vector<std::string> Action::updateColumns() const
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
    return ret;
}

void Action::updateArgs(drogon::orm::internal::SqlBinder &binder) const
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
        if(getType())
        {
            binder << getValueOfType();
        }
        else
        {
            binder << nullptr;
        }
    }
    if(dirtyFlag_[3])
    {
        if(getMemo())
        {
            binder << getValueOfMemo();
        }
        else
        {
            binder << nullptr;
        }
    }
}
Json::Value Action::toJson() const
{
    Json::Value ret;
    if(getActionId())
    {
        ret["Action_ID"]=getValueOfActionId();
    }
    else
    {
        ret["Action_ID"]=Json::Value();
    }
    if(getUserId())
    {
        ret["User_ID"]=getValueOfUserId();
    }
    else
    {
        ret["User_ID"]=Json::Value();
    }
    if(getType())
    {
        ret["Type"]=getValueOfType();
    }
    else
    {
        ret["Type"]=Json::Value();
    }
    if(getMemo())
    {
        ret["Memo"]=getValueOfMemo();
    }
    else
    {
        ret["Memo"]=Json::Value();
    }
    return ret;
}

Json::Value Action::toMasqueradedJson(
    const std::vector<std::string> &pMasqueradingVector) const
{
    Json::Value ret;
    if(pMasqueradingVector.size() == 4)
    {
        if(!pMasqueradingVector[0].empty())
        {
            if(getActionId())
            {
                ret[pMasqueradingVector[0]]=getValueOfActionId();
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
            if(getType())
            {
                ret[pMasqueradingVector[2]]=getValueOfType();
            }
            else
            {
                ret[pMasqueradingVector[2]]=Json::Value();
            }
        }
        if(!pMasqueradingVector[3].empty())
        {
            if(getMemo())
            {
                ret[pMasqueradingVector[3]]=getValueOfMemo();
            }
            else
            {
                ret[pMasqueradingVector[3]]=Json::Value();
            }
        }
        return ret;
    }
    LOG_ERROR << "Masquerade failed";
    if(getActionId())
    {
        ret["Action_ID"]=getValueOfActionId();
    }
    else
    {
        ret["Action_ID"]=Json::Value();
    }
    if(getUserId())
    {
        ret["User_ID"]=getValueOfUserId();
    }
    else
    {
        ret["User_ID"]=Json::Value();
    }
    if(getType())
    {
        ret["Type"]=getValueOfType();
    }
    else
    {
        ret["Type"]=Json::Value();
    }
    if(getMemo())
    {
        ret["Memo"]=getValueOfMemo();
    }
    else
    {
        ret["Memo"]=Json::Value();
    }
    return ret;
}

bool Action::validateJsonForCreation(const Json::Value &pJson, std::string &err)
{
    if(pJson.isMember("Action_ID"))
    {
        if(!validJsonOfField(0, "Action_ID", pJson["Action_ID"], err, true))
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
    if(pJson.isMember("Type"))
    {
        if(!validJsonOfField(2, "Type", pJson["Type"], err, true))
            return false;
    }
    else
    {
        err="The Type column cannot be null";
        return false;
    }
    if(pJson.isMember("Memo"))
    {
        if(!validJsonOfField(3, "Memo", pJson["Memo"], err, true))
            return false;
    }
    else
    {
        err="The Memo column cannot be null";
        return false;
    }
    return true;
}
bool Action::validateMasqueradedJsonForCreation(const Json::Value &pJson,
                                                const std::vector<std::string> &pMasqueradingVector,
                                                std::string &err)
{
    if(pMasqueradingVector.size() != 4)
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
    }
    catch(const Json::LogicError &e) 
    {
      err = e.what();
      return false;
    }
    return true;
}
bool Action::validateJsonForUpdate(const Json::Value &pJson, std::string &err)
{
    if(pJson.isMember("Action_ID"))
    {
        if(!validJsonOfField(0, "Action_ID", pJson["Action_ID"], err, false))
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
    if(pJson.isMember("Type"))
    {
        if(!validJsonOfField(2, "Type", pJson["Type"], err, false))
            return false;
    }
    if(pJson.isMember("Memo"))
    {
        if(!validJsonOfField(3, "Memo", pJson["Memo"], err, false))
            return false;
    }
    return true;
}
bool Action::validateMasqueradedJsonForUpdate(const Json::Value &pJson,
                                              const std::vector<std::string> &pMasqueradingVector,
                                              std::string &err)
{
    if(pMasqueradingVector.size() != 4)
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
    }
    catch(const Json::LogicError &e) 
    {
      err = e.what();
      return false;
    }
    return true;
}
bool Action::validJsonOfField(size_t index,
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
            if(!pJson.isString())
            {
                err="Type error in the "+fieldName+" field";
                return false;                
            }
            // asString().length() creates a string object, is there any better way to validate the length?
            if(pJson.isString() && pJson.asString().length() > 255)
            {
                err="String length exceeds limit for the " +
                    fieldName +
                    " field (the maximum value is 255)";
                return false;               
            }

            break;
        case 3:
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
