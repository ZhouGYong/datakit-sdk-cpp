/*****************************************************************//**
 * \file   DataSyncManager.h
 * \brief  Data synchronization, sending the collected data to datakit agent
 * 
 * \author Zhou Guangyong
 * \date   Dec 2022
 *********************************************************************/
#pragma once
#ifndef _DATAKIT_SDK_DATA_SYNC_MANAGER_H_
#define _DATAKIT_SDK_DATA_SYNC_MANAGER_H_

#include <string>
#include <deque>
#include <mutex>
#include <vector>
#include <map>
#include <any>
#include <condition_variable>
#include "Singleton.h"
#include "FTSDKNode.h"
#include "TrackNodeImpl.h"
#include "InternalEnums.h"

namespace com::ft::sdk::internal
{
	typedef std::map<std::string, std::string> TagMap;
	typedef std::map<std::string, std::any> FieldMap;

	struct Measurement
	{
		std::string measurementName;
		std::map<std::string, std::string> tags;
		std::map<std::string, std::any> fields;
	};

	struct DataMsg
	{
	public:
		DataType dataType;
		std::vector<Measurement> vtLine;
		//double basetime;
		std::string rawLine;

		Measurement& addMeasurement();
		bool isConverted()
		{
			return rawLine != "";
		}
	};

	class DataSyncManager : public Singleton<DataSyncManager>
	{
	public:
		void init();
		void deinit();
		void sendDataMessage(DataMsg& dMsg);
		void sendDataMessageFromDB(DataMsg& bgMsg);
		void notifyNewLinefromDB();

	private:
		DataSyncManager();
		void processData();
		void getDataFromDB();
		DataMsg receiveDataMessage(void);

		bool handleRUMEvent(DataMsg& dMsg);
		bool handleLogEvent(DataMsg& dMsg);
		bool handleTraceEvent(DataMsg& dMsg);
	private:
		bool m_stopping = false;

		/* message queue to pass the data to working thread */
		std::deque<DataMsg> m_dataMsgQue;	
		/* Mutex to protect the queue */
		std::mutex m_dataQueMtx;
		/* the Queue is not empty, i.e. there is at least one Message in queue */
		std::condition_variable m_dataQueNotEmpty; 
		/* the Queue is full, has to wait for processing data */
		std::condition_variable m_dataQueFull; 

		/* Mutex to protect the queue */
		std::mutex m_dbDataMtx;	
		/* new data inserted in DB */
		std::condition_variable m_dataIncomingFromDB; 
		bool m_hasIncomingData = false;

		static const int MAX_DATA_QUEUE_SIZE = 100;

		bool m_enableFileCache = false;

		ENABLE_SINGLETON();
	};

}

#endif // _DATAKIT_SDK_DATA_SYNC_MANAGER_H_