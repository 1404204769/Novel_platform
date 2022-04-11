/**
 *
 *  Comment.h
 *  DO NOT EDIT. This file is generated by drogon_ctl
 *
 */

#pragma once
#include <drogon/orm/Result.h>
#include <drogon/orm/Row.h>
#include <drogon/orm/Field.h>
#include <drogon/orm/SqlBinder.h>
#include <drogon/orm/Mapper.h>
#ifdef __cpp_impl_coroutine
#include <drogon/orm/CoroMapper.h>
#endif
#include <trantor/utils/Date.h>
#include <trantor/utils/Logger.h>
#include <json/json.h>
#include <string>
#include <memory>
#include <vector>
#include <tuple>
#include <stdint.h>
#include <iostream>

using namespace drogon::orm;
namespace drogon
{
namespace orm
{
class DbClient;
using DbClientPtr = std::shared_ptr<DbClient>;
}
}
namespace drogon_model
{
namespace novel 
{

class Comment
{
  public:
    struct Cols
    {
        static const std::string _Comment_ID;
        static const std::string _Note_ID;
        static const std::string _Floor_ID;
        static const std::string _User_ID;
        static const std::string _Comment_Content;
        static const std::string _Create_Time;
        static const std::string _Reply_Floor_ID;
    };

    const static int primaryKeyNumber;
    const static std::string tableName;
    const static bool hasPrimaryKey;
    const static std::string primaryKeyName;
    using PrimaryKeyType = int32_t;
    const PrimaryKeyType &getPrimaryKey() const;

    /**
     * @brief constructor
     * @param r One row of records in the SQL query result.
     * @param indexOffset Set the offset to -1 to access all columns by column names, 
     * otherwise access all columns by offsets.
     * @note If the SQL is not a style of 'select * from table_name ...' (select all 
     * columns by an asterisk), please set the offset to -1.
     */
    explicit Comment(const Row &r, const ssize_t indexOffset = 0) noexcept;

    /**
     * @brief constructor
     * @param pJson The json object to construct a new instance.
     */
    explicit Comment(const Json::Value &pJson) noexcept(false);

    /**
     * @brief constructor
     * @param pJson The json object to construct a new instance.
     * @param pMasqueradingVector The aliases of table columns.
     */
    Comment(const Json::Value &pJson, const std::vector<std::string> &pMasqueradingVector) noexcept(false);

    Comment() = default;
    
    void updateByJson(const Json::Value &pJson) noexcept(false);
    void updateByMasqueradedJson(const Json::Value &pJson,
                                 const std::vector<std::string> &pMasqueradingVector) noexcept(false);
    static bool validateJsonForCreation(const Json::Value &pJson, std::string &err);
    static bool validateMasqueradedJsonForCreation(const Json::Value &,
                                                const std::vector<std::string> &pMasqueradingVector,
                                                    std::string &err);
    static bool validateJsonForUpdate(const Json::Value &pJson, std::string &err);
    static bool validateMasqueradedJsonForUpdate(const Json::Value &,
                                          const std::vector<std::string> &pMasqueradingVector,
                                          std::string &err);
    static bool validJsonOfField(size_t index,
                          const std::string &fieldName,
                          const Json::Value &pJson, 
                          std::string &err, 
                          bool isForCreation);

    /**  For column Comment_ID  */
    ///Get the value of the column Comment_ID, returns the default value if the column is null
    const int32_t &getValueOfCommentId() const noexcept;
    ///Return a shared_ptr object pointing to the column const value, or an empty shared_ptr object if the column is null
    const std::shared_ptr<int32_t> &getCommentId() const noexcept;
    ///Set the value of the column Comment_ID
    void setCommentId(const int32_t &pCommentId) noexcept;

    /**  For column Note_ID  */
    ///Get the value of the column Note_ID, returns the default value if the column is null
    const int32_t &getValueOfNoteId() const noexcept;
    ///Return a shared_ptr object pointing to the column const value, or an empty shared_ptr object if the column is null
    const std::shared_ptr<int32_t> &getNoteId() const noexcept;
    ///Set the value of the column Note_ID
    void setNoteId(const int32_t &pNoteId) noexcept;

    /**  For column Floor_ID  */
    ///Get the value of the column Floor_ID, returns the default value if the column is null
    const int32_t &getValueOfFloorId() const noexcept;
    ///Return a shared_ptr object pointing to the column const value, or an empty shared_ptr object if the column is null
    const std::shared_ptr<int32_t> &getFloorId() const noexcept;
    ///Set the value of the column Floor_ID
    void setFloorId(const int32_t &pFloorId) noexcept;

    /**  For column User_ID  */
    ///Get the value of the column User_ID, returns the default value if the column is null
    const int32_t &getValueOfUserId() const noexcept;
    ///Return a shared_ptr object pointing to the column const value, or an empty shared_ptr object if the column is null
    const std::shared_ptr<int32_t> &getUserId() const noexcept;
    ///Set the value of the column User_ID
    void setUserId(const int32_t &pUserId) noexcept;

    /**  For column Comment_Content  */
    ///Get the value of the column Comment_Content, returns the default value if the column is null
    const std::string &getValueOfCommentContent() const noexcept;
    ///Return a shared_ptr object pointing to the column const value, or an empty shared_ptr object if the column is null
    const std::shared_ptr<std::string> &getCommentContent() const noexcept;
    ///Set the value of the column Comment_Content
    void setCommentContent(const std::string &pCommentContent) noexcept;
    void setCommentContent(std::string &&pCommentContent) noexcept;

    /**  For column Create_Time  */
    ///Get the value of the column Create_Time, returns the default value if the column is null
    const ::trantor::Date &getValueOfCreateTime() const noexcept;
    ///Return a shared_ptr object pointing to the column const value, or an empty shared_ptr object if the column is null
    const std::shared_ptr<::trantor::Date> &getCreateTime() const noexcept;
    ///Set the value of the column Create_Time
    void setCreateTime(const ::trantor::Date &pCreateTime) noexcept;

    /**  For column Reply_Floor_ID  */
    ///Get the value of the column Reply_Floor_ID, returns the default value if the column is null
    const int32_t &getValueOfReplyFloorId() const noexcept;
    ///Return a shared_ptr object pointing to the column const value, or an empty shared_ptr object if the column is null
    const std::shared_ptr<int32_t> &getReplyFloorId() const noexcept;
    ///Set the value of the column Reply_Floor_ID
    void setReplyFloorId(const int32_t &pReplyFloorId) noexcept;


    static size_t getColumnNumber() noexcept {  return 7;  }
    static const std::string &getColumnName(size_t index) noexcept(false);

    Json::Value toJson() const;
    Json::Value toMasqueradedJson(const std::vector<std::string> &pMasqueradingVector) const;
    /// Relationship interfaces
  private:
    friend Mapper<Comment>;
#ifdef __cpp_impl_coroutine
    friend CoroMapper<Comment>;
#endif
    static const std::vector<std::string> &insertColumns() noexcept;
    void outputArgs(drogon::orm::internal::SqlBinder &binder) const;
    const std::vector<std::string> updateColumns() const;
    void updateArgs(drogon::orm::internal::SqlBinder &binder) const;
    ///For mysql or sqlite3
    void updateId(const uint64_t id);
    std::shared_ptr<int32_t> commentId_;
    std::shared_ptr<int32_t> noteId_;
    std::shared_ptr<int32_t> floorId_;
    std::shared_ptr<int32_t> userId_;
    std::shared_ptr<std::string> commentContent_;
    std::shared_ptr<::trantor::Date> createTime_;
    std::shared_ptr<int32_t> replyFloorId_;
    struct MetaData
    {
        const std::string colName_;
        const std::string colType_;
        const std::string colDatabaseType_;
        const ssize_t colLength_;
        const bool isAutoVal_;
        const bool isPrimaryKey_;
        const bool notNull_;
    };
    static const std::vector<MetaData> metaData_;
    bool dirtyFlag_[7]={ false };
  public:
    static const std::string &sqlForFindingByPrimaryKey()
    {
        static const std::string sql="select * from " + tableName + " where Comment_ID = ?";
        return sql;                   
    }

    static const std::string &sqlForDeletingByPrimaryKey()
    {
        static const std::string sql="delete from " + tableName + " where Comment_ID = ?";
        return sql;                   
    }
    std::string sqlForInserting(bool &needSelection) const
    {
        std::string sql="insert into " + tableName + " (";
        size_t parametersCount = 0;
        needSelection = false;
            sql += "Comment_ID,";
            ++parametersCount;
        if(dirtyFlag_[1])
        {
            sql += "Note_ID,";
            ++parametersCount;
        }
        sql += "Floor_ID,";
        ++parametersCount;
        if(!dirtyFlag_[2])
        {
            needSelection=true;
        }
        if(dirtyFlag_[3])
        {
            sql += "User_ID,";
            ++parametersCount;
        }
        if(dirtyFlag_[4])
        {
            sql += "Comment_Content,";
            ++parametersCount;
        }
        sql += "Create_Time,";
        ++parametersCount;
        if(!dirtyFlag_[5])
        {
            needSelection=true;
        }
        sql += "Reply_Floor_ID,";
        ++parametersCount;
        if(!dirtyFlag_[6])
        {
            needSelection=true;
        }
        needSelection=true;
        if(parametersCount > 0)
        {
            sql[sql.length()-1]=')';
            sql += " values (";
        }
        else
            sql += ") values (";
        
        sql +="default,";
        if(dirtyFlag_[1])
        {
            sql.append("?,");

        } 
        if(dirtyFlag_[2])
        {
            sql.append("?,");

        } 
        else
        {
            sql +="default,";
        }
        if(dirtyFlag_[3])
        {
            sql.append("?,");

        } 
        if(dirtyFlag_[4])
        {
            sql.append("?,");

        } 
        if(dirtyFlag_[5])
        {
            sql.append("?,");

        } 
        else
        {
            sql +="default,";
        }
        if(dirtyFlag_[6])
        {
            sql.append("?,");

        } 
        else
        {
            sql +="default,";
        }
        if(parametersCount > 0)
        {
            sql.resize(sql.length() - 1);
        }
        sql.append(1, ')');
        LOG_TRACE << sql;
        return sql;   
    }
};
} // namespace novel
} // namespace drogon_model
