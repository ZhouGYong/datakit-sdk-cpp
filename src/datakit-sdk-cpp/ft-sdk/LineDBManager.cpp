#include "pch.h"
#include "LineDBManager.h"
#include "LoggerManager.h"
#include "FTSDKConfigManager.h"
#include "LineProtocolBuilder.h"
#include "Constants.h"
#include <iostream>
#include <sstream>

#define   LINEDB_TABLE_CREATE_LINE_RUM_SQL      "CREATE TABLE IF NOT EXISTS line_rum(id INTEGER PRIMARY KEY NOT NULL, \
              created_time TimeStamp NOT NULL DEFAULT (datetime('now','localtime')), packet TEXT NOT NULL, \
              status INTEGER NOT NULL default 0, line_cnt INTEGER NOT NULL default 0, retry_cnt INTEGER NOT NULL default 0);"

#define   LINEDB_TABLE_CREATE_LINE_LOG_SQL      "CREATE TABLE IF NOT EXISTS line_log(id INTEGER PRIMARY KEY NOT NULL, \
              created_time TimeStamp NOT NULL DEFAULT (datetime('now','localtime')), packet TEXT NOT NULL, \
              status INTEGER NOT NULL default 0, line_cnt INTEGER NOT NULL default 0, retry_cnt INTEGER NOT NULL default 0);"

#define   LINEDB_TABLE_CREATE_LINE_TRACE_SQL      "CREATE TABLE IF NOT EXISTS line_trace(id INTEGER PRIMARY KEY NOT NULL, \
              created_time TimeStamp NOT NULL DEFAULT (datetime('now','localtime')), packet TEXT NOT NULL, \
              status INTEGER NOT NULL default 0, line_cnt INTEGER NOT NULL default 0, retry_cnt INTEGER NOT NULL default 0);"

namespace com::ft::sdk::internal
{
	LineDBManager::LineDBManager()
	{
        DB_FILE = FTSDKConfigManager::getInstance().getWorkingDir() + "/" + "datakit.db";
	}

	void LineDBManager::init()
	{
        try
        {
            m_pDB = std::make_shared< SQLite::Database>(DB_FILE, SQLite::OPEN_READWRITE | SQLite::OPEN_CREATE);

            SQLite::Transaction transaction(*m_pDB);

            executeSQL(LINEDB_TABLE_CREATE_LINE_RUM_SQL);
            LoggerManager::getInstance().logInfo("Line DB - RUM is created.");

            executeSQL(LINEDB_TABLE_CREATE_LINE_LOG_SQL);
            LoggerManager::getInstance().logInfo("Line DB - LOG is created.");

            executeSQL(LINEDB_TABLE_CREATE_LINE_TRACE_SQL);
            LoggerManager::getInstance().logInfo("Line DB - TRACE is created.");

            transaction.commit();

            LoggerManager::getInstance().logInfo("Line DB is initialized.");

            if (FTSDKConfigManager::getInstance().getGeneralConfig().getClearDBWhenStartUp())
            {
                clearDB();
            }
        }
        catch (std::exception& e)
        {
            LoggerManager::getInstance().logError("SQLite exception during table creation: {}", e.what());
        }
	}

    void LineDBManager::uninit()
    {

    }

    void LineDBManager::clearDB()
    {
        LoggerManager::getInstance().logDebug("clear Line DB.");

        executeSQL("delete from line_rum");
        executeSQL("delete from line_log");
        executeSQL("delete from line_trace");
    }

    bool LineDBManager::insertLine(DataMsg& msg)
    {
        DataType type = msg.dataType;

        try
        {
            std::string linePtl = LineProtocolBuilder::getInstance().encode(msg);

            const std::string sql = "insert into " + getTableName(type) + "(line_cnt, packet, status, retry_cnt) values("
                    + std::to_string((int)msg.vtLine.size()) + ",'" + linePtl + "'," + std::to_string(LineStatus::ready) + ", 0);";

            int rows = executeSQL(sql);

            if (rows > 0)
            {
                DataSyncManager::getInstance().notifyNewLinefromDB();
            }

            return rows > 0;
        }
        catch (std::exception& e)
        {
            LoggerManager::getInstance().logError("SQLite exception during insertion: {}", e.what());
        }

        return false;
    }

    /**
     * Query line protocol message from file DB
     * 1. query message by data type
     * 2. delete the message from DB after retrieval
     * 
     * @param type
     * @return 
     */
    std::vector<std::shared_ptr<DataMsg>> LineDBManager::queryLineFromDB(DataType type)
    {
        std::vector<std::shared_ptr<DataMsg>> vtLine;
        try
        {
            std::string table = getTableName(type);
            SQLite::Statement  query(*m_pDB, "SELECT * FROM " + table + " order by id asc limit 1000"); // TODO: depends on the global strategy

            std::vector<int> vtId;
            std::vector<std::string> vtRawLine;
            int accumulatedCnt = 0;
            std::string oneLine = "";
            while (query.executeStep())
            {
                int id = query.getColumn(0);
                const char* createdTime = query.getColumn(1);
                const char* packet = query.getColumn(2);
                int status = query.getColumn(3);
                int line_cnt = query.getColumn(4);
                int retry_cnt = query.getColumn(5);

                if (accumulatedCnt + line_cnt > in_constants::RUM_ITEM_LIMIT_SIZE)
                {
                    vtRawLine.push_back(oneLine);
                    accumulatedCnt = 0;
                    oneLine = "";
                }
                
                vtId.push_back(id);
                oneLine = oneLine + packet;
                accumulatedCnt += line_cnt;

                std::stringstream info;
                info << "id: " << id << ", line_cnt: " << line_cnt << ", acc_line: " << accumulatedCnt << std::endl;
                LoggerManager::getInstance().logDebug(info.str());
            }

            if (oneLine.size() > 0)
            {
                vtRawLine.push_back(oneLine);
            }

            for (auto ln : vtRawLine)
            {
                std::shared_ptr<DataMsg> dm = std::make_shared<DataMsg>();
                dm->dataType = type;
                dm->rawLine = ln;

                vtLine.push_back(dm);
            }

            // delete the retrieved items
            executeSQLs("delete from " + table + " where id=?", vtId);
        }
        catch (std::exception& e)
        {
            LoggerManager::getInstance().logError("exception: {}", e.what());
        }

        return vtLine;
    }

    int LineDBManager::executeSQL(std::string sql)
    {
        LoggerManager::getInstance().logTrace("Execute SQL: " + sql);
        return m_pDB->exec(sql);
    }

    int LineDBManager::executeSQLs(std::string sql, std::vector<int> vtIds)
    {
        SQLite::Statement query{ *m_pDB, sql };

        int cnt = 0;
        for (const auto& v : vtIds)
        {
            query.bind(1, v);
            cnt += query.exec();
            query.reset();
        }

        if (cnt > 0)
        {
            LoggerManager::getInstance().logDebug("Execute SQL: " + sql);
        }

        return cnt;
    }

    std::string LineDBManager::getTableName(DataType type)
    {
        std::string tableName = "";

        switch (type)
        {
        case DataType::RUM_APP:
        case DataType::RUM_WEBVIEW:
            tableName = "line_rum";
            break;
        case DataType::LOG:
            tableName = "line_log";
            break;
        case DataType::TRACE:
            tableName = "line_trace";
            break;
        default:
            break;
        }

        return tableName;
    }
}