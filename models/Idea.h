/**
 *
 *  Idea.h
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

class Idea
{
  public:
    struct Cols
    {
        static const std::string _Idea_ID;
        static const std::string _User_ID;
        static const std::string _Type;
        static const std::string _Memo;
        static const std::string _Status;
        static const std::string _Create;
        static const std::string _Update;
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
    explicit Idea(const Row &r, const ssize_t indexOffset = 0) noexcept;

    /**
     * @brief constructor
     * @param pJson The json object to construct a new instance.
     */
    explicit Idea(const Json::Value &pJson) noexcept(false);

    /**
     * @brief constructor
     * @param pJson The json object to construct a new instance.
     * @param pMasqueradingVector The aliases of table columns.
     */
    Idea(const Json::Value &pJson, const std::vector<std::string> &pMasqueradingVector) noexcept(false);

    Idea() = default;
    
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

    /**  For column Idea_ID  */
    ///Get the value of the column Idea_ID, returns the default value if the column is null
    const int32_t &getValueOfIdeaId() const noexcept;
    ///Return a shared_ptr object pointing to the column const value, or an empty shared_ptr object if the column is null
    const std::shared_ptr<int32_t> &getIdeaId() const noexcept;
    ///Set the value of the column Idea_ID
    void setIdeaId(const int32_t &pIdeaId) noexcept;

    /**  For column User_ID  */
    ///Get the value of the column User_ID, returns the default value if the column is null
    const int32_t &getValueOfUserId() const noexcept;
    ///Return a shared_ptr object pointing to the column const value, or an empty shared_ptr object if the column is null
    const std::shared_ptr<int32_t> &getUserId() const noexcept;
    ///Set the value of the column User_ID
    void setUserId(const int32_t &pUserId) noexcept;

    /**  For column Type  */
    ///Get the value of the column Type, returns the default value if the column is null
    const std::string &getValueOfType() const noexcept;
    ///Return a shared_ptr object pointing to the column const value, or an empty shared_ptr object if the column is null
    const std::shared_ptr<std::string> &getType() const noexcept;
    ///Set the value of the column Type
    void setType(const std::string &pType) noexcept;
    void setType(std::string &&pType) noexcept;

    /**  For column Memo  */
    ///Get the value of the column Memo, returns the default value if the column is null
    const std::string &getValueOfMemo() const noexcept;
    ///Return a shared_ptr object pointing to the column const value, or an empty shared_ptr object if the column is null
    const std::shared_ptr<std::string> &getMemo() const noexcept;
    ///Set the value of the column Memo
    void setMemo(const std::string &pMemo) noexcept;
    void setMemo(std::string &&pMemo) noexcept;

    /**  For column Status  */
    ///Get the value of the column Status, returns the default value if the column is null
    const std::string &getValueOfStatus() const noexcept;
    ///Return a shared_ptr object pointing to the column const value, or an empty shared_ptr object if the column is null
    const std::shared_ptr<std::string> &getStatus() const noexcept;
    ///Set the value of the column Status
    void setStatus(const std::string &pStatus) noexcept;
    void setStatus(std::string &&pStatus) noexcept;

    /**  For column Create  */
    ///Get the value of the column Create, returns the default value if the column is null
    const ::trantor::Date &getValueOfCreate() const noexcept;
    ///Return a shared_ptr object pointing to the column const value, or an empty shared_ptr object if the column is null
    const std::shared_ptr<::trantor::Date> &getCreate() const noexcept;
    ///Set the value of the column Create
    void setCreate(const ::trantor::Date &pCreate) noexcept;

    /**  For column Update  */
    ///Get the value of the column Update, returns the default value if the column is null
    const ::trantor::Date &getValueOfUpdate() const noexcept;
    ///Return a shared_ptr object pointing to the column const value, or an empty shared_ptr object if the column is null
    const std::shared_ptr<::trantor::Date> &getUpdate() const noexcept;
    ///Set the value of the column Update
    void setUpdate(const ::trantor::Date &pUpdate) noexcept;


    static size_t getColumnNumber() noexcept {  return 7;  }
    static const std::string &getColumnName(size_t index) noexcept(false);

    Json::Value toJson() const;
    Json::Value toMasqueradedJson(const std::vector<std::string> &pMasqueradingVector) const;
    /// Relationship interfaces
  private:
    friend Mapper<Idea>;
#ifdef __cpp_impl_coroutine
    friend CoroMapper<Idea>;
#endif
    static const std::vector<std::string> &insertColumns() noexcept;
    void outputArgs(drogon::orm::internal::SqlBinder &binder) const;
    const std::vector<std::string> updateColumns() const;
    void updateArgs(drogon::orm::internal::SqlBinder &binder) const;
    ///For mysql or sqlite3
    void updateId(const uint64_t id);
    std::shared_ptr<int32_t> ideaId_;
    std::shared_ptr<int32_t> userId_;
    std::shared_ptr<std::string> type_;
    std::shared_ptr<std::string> memo_;
    std::shared_ptr<std::string> status_;
    std::shared_ptr<::trantor::Date> create_;
    std::shared_ptr<::trantor::Date> update_;
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
        static const std::string sql="select * from " + tableName + " where Idea_ID = ?";
        return sql;                   
    }

    static const std::string &sqlForDeletingByPrimaryKey()
    {
        static const std::string sql="delete from " + tableName + " where Idea_ID = ?";
        return sql;                   
    }
    std::string sqlForInserting(bool &needSelection) const
    {
        std::string sql="insert into " + tableName + " (";
        size_t parametersCount = 0;
        needSelection = false;
            sql += "Idea_ID,";
            ++parametersCount;
        if(dirtyFlag_[1])
        {
            sql += "User_ID,";
            ++parametersCount;
        }
        if(dirtyFlag_[2])
        {
            sql += "Type,";
            ++parametersCount;
        }
        if(dirtyFlag_[3])
        {
            sql += "Memo,";
            ++parametersCount;
        }
        if(dirtyFlag_[4])
        {
            sql += "Status,";
            ++parametersCount;
        }
        sql += "Create,";
        ++parametersCount;
        if(!dirtyFlag_[5])
        {
            needSelection=true;
        }
        sql += "Update,";
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