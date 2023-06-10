/*****************************************************************//**
 * \file   LineDBManager.h
 * \brief  File DB Cache for line protocol message
 * 
 * \author Zhou Guangyong
 * \date   March 2023
 *********************************************************************/
#pragma once
#ifndef _DATAKIT_SDK_LINE_DB_MANAGER_H_
#define _DATAKIT_SDK_LINE_DB_MANAGER_H_

#include "Singleton.h"

#ifdef __linux__
#if __GNUC__ < 8
#define SQLITECPP_HAVE_STD_EXPERIMENTAL_FILESYSTEM 1
#endif
#endif

#include <SQLiteCpp/SQLiteCpp.h>
#include <string>
#include "DataSyncManager.h"

namespace com::ft::sdk::internal
{
	enum LineStatus : char
	{
		ready = 0x0,
		sent,
		failed
	};

	class LineDBManager : public Singleton<LineDBManager>
	{
	public:
		void init();

		void clearDB();
		void uninit();

		bool insertLine(DataMsg& log);
		std::vector<std::shared_ptr<DataMsg>> queryLineFromDB(DataType type);

	private:
		LineDBManager();

		int executeSQL(std::string sql);
		std::string getTableName(DataType type);
		int executeSQLs(std::string sql, std::vector<int> vtIds);

	private:
		std::shared_ptr<SQLite::Database> m_pDB;
		std::string DB_FILE = "";

		ENABLE_SINGLETON();
	};

}

#endif // !_DATAKIT_SDK_LINE_DB_MANAGER_H_



