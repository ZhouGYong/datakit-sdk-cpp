#include "pch.h"
#include "DataSyncManager.h"
#include "LineProtocolBuilder.h"
#include <thread>
#include "CommunicationManager.h"
#include "LoggerManager.h"
#include "LineDBManager.h"
#include "FTSDKConfigManager.h"
#include "FTSDKError.h"

namespace com::ft::sdk::internal
{
	const int HTTP_RETRY_COUNT = 3;

	Measurement& DataMsg::addMeasurement()
	{
		vtLine.push_back(Measurement());

		return vtLine.back();
	}

	DataSyncManager::DataSyncManager()
	{
		new std::thread(&DataSyncManager::processData, this);
	}

	void DataSyncManager::init()
	{
		m_enableFileCache = FTSDKConfigManager::getInstance().getGeneralConfig().getEnableFileDBCache();
		if (m_enableFileCache)
		{
			new std::thread(&DataSyncManager::getDataFromDB, this);
		}
	}

	void DataSyncManager::deinit()
	{
		m_stopping = true;

		m_dataIncomingFromDB.notify_one();
		m_dataQueNotEmpty.notify_one();
	}

	/**
	 * working thread for processing the data message.
	 * 1. get data message from queue
	 * 2. dispatch the message to the corresponding handler
	 * 3. send the message and handle the retry
	 * 
	 */
	void DataSyncManager::processData()
	{
		BEGIN_THREAD();

		while (!m_stopping)
		{
			DataMsg dataMsg = receiveDataMessage();
			int retryCount = 0;
			bool needRetry = false;

			while ((retryCount < HTTP_RETRY_COUNT) && !m_stopping)
			{
				switch (dataMsg.dataType)
				{
				case DataType::RUM_APP:
				case DataType::RUM_WEBVIEW:
					needRetry = handleRUMEvent(dataMsg);
					break;

				case DataType::LOG:
					needRetry = handleLogEvent(dataMsg);
					break;

				case DataType::TRACE:
					needRetry = handleTraceEvent(dataMsg);
					break;

				default:
					internal::LoggerManager::getInstance().logError("wrong data type: {}, ignoring it!", (int)(dataMsg.dataType));
					break;
				}

				if (!needRetry)
				{
					break;
				}
				else
				{
					internal::LoggerManager::getInstance().logInfo("try to resend the data message...");
					retryCount++;
				}
			}

			if (!m_stopping && needRetry && retryCount >= HTTP_RETRY_COUNT)
			{
				internal::LoggerManager::getInstance().logError("failed to send the data message for 3 times, abandon it...");
			}
		}

		internal::LoggerManager::getInstance().logInfo("DataSyncManager: working thread exited.");

		END_THREAD();
	}

	void DataSyncManager::sendDataMessage(DataMsg& bgMsg)
	{	
		{
			std::unique_lock <std::mutex> lck(m_dataQueMtx);
			m_dataQueFull.wait(lck, [=] { return !(m_dataMsgQue.size() == MAX_DATA_QUEUE_SIZE); });
			m_dataMsgQue.push_back(bgMsg);

			internal::LoggerManager::getInstance().logInfo("added new data message to Message Queue.");
		}
		m_dataQueNotEmpty.notify_one();
	}

	void DataSyncManager::notifyNewLinefromDB()
	{
		std::lock_guard<std::mutex> lck(m_dbDataMtx);
		m_hasIncomingData = true;
		m_dataIncomingFromDB.notify_one();
	}

	/**
	 * working thread for querying the data message from file DB
	 * 
	 */
	void DataSyncManager::getDataFromDB()
	{
		BEGIN_THREAD();

		while (!m_stopping)
		{
			std::unique_lock<std::mutex> lck(m_dbDataMtx);
			m_dataIncomingFromDB.wait(lck, [=] { return m_hasIncomingData || m_stopping; });
			
			if (m_stopping)
			{
				break;
			}

			DataType allTypes[] = { DataType::RUM_APP, DataType::LOG };

			for (DataType tp : allTypes)
			{
				auto lines = LineDBManager::getInstance().queryLineFromDB(tp);
				for (auto ln : lines)
				{
					sendDataMessage(*ln);
				}
			}
			m_hasIncomingData = false;
		}

		internal::LoggerManager::getInstance().logInfo("DataSyncManager: working thread 2 exited.");

		END_THREAD();
	}

	void DataSyncManager::sendDataMessageFromDB(DataMsg& bgMsg)
	{
		{
			std::unique_lock <std::mutex> lck(m_dataQueMtx);
			m_dataQueFull.wait(lck, [=] { return !(m_dataMsgQue.size() == MAX_DATA_QUEUE_SIZE); });
			m_dataMsgQue.push_back(bgMsg);

			internal::LoggerManager::getInstance().logInfo("added new DB message to Message Queue.");
		}
		m_dataQueNotEmpty.notify_one();
	}

	DataMsg DataSyncManager::receiveDataMessage(void)
	{
		DataMsg dataMsg = { };
		{
			std::unique_lock <std::mutex> lck(m_dataQueMtx);

			m_dataQueNotEmpty.wait(lck, [=] { return !m_dataMsgQue.empty() || m_stopping; });

			if (m_stopping)
			{
				return dataMsg;
			}
			else
			{
				dataMsg = m_dataMsgQue.front();
				m_dataMsgQue.pop_front();

				internal::LoggerManager::getInstance().logInfo("retreived a data message from Message Queue.");
			}
		}
		m_dataQueFull.notify_one();

		return dataMsg;
	}

	bool DataSyncManager::handleRUMEvent(DataMsg& dMsg)
	{
		bool needRetry = false;
		std::string linePtl = dMsg.rawLine;
		if (!dMsg.isConverted())
		{
			linePtl = LineProtocolBuilder::getInstance().encode(dMsg);
		}
		ResponseData response = CommunicationManager::getInstance().post(dMsg.dataType, linePtl);
		if (response.code == 200)
		{
			internal::LoggerManager::getInstance().logInfo("successfully posted the rum event to datakit agent.");
		}
		else if (response.code == 400 || response.code == 500)
		{
			internal::LoggerManager::getInstance().logError("failed to post the rum event to datakit agent due to network issue, giving up: code={}, message={}", std::to_string(response.code), response.message);
		}
		else
		{
			needRetry = true;
			internal::LoggerManager::getInstance().logError("failed to post the rum event to datakit agent: code={}, message={}", std::to_string(response.code), response.message);
		}

		return needRetry;
	}

	bool DataSyncManager::handleLogEvent(DataMsg& dMsg)
	{
		bool needRetry = false;
		std::string linePtl = dMsg.rawLine;
		if (!dMsg.isConverted())
		{
			linePtl = LineProtocolBuilder::getInstance().encode(dMsg);
		}
		ResponseData response = CommunicationManager::getInstance().post(dMsg.dataType, linePtl);
		if (response.code == 200)
		{
			internal::LoggerManager::getInstance().logInfo("successfully posted the log event to datakit agent.");
		}
		else if (response.code == 400 || response.code == 500)
		{
			internal::LoggerManager::getInstance().logError("failed to post the log event to datakit agent due to network issue, giving up: code={}, message={}", std::to_string(response.code), response.message);
		}
		else
		{
			needRetry = true;
			internal::LoggerManager::getInstance().logError("failed to post the log event to datakit agent: code={}, message={}", std::to_string(response.code), response.message);
		}

		return needRetry;
	}

	bool DataSyncManager::handleTraceEvent(DataMsg& dMsg)
	{
		return true;
	}
}