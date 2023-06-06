#include "pch.h"
#include "CacheDBManager.h"
#include "LoggerManager.h"
#include "FTSDKConstants.h"
#include "FTSDKConfigManager.h"
#include "LineDBManager.h"
#include "FTSDKError.h"
#include "Constants.h"
#include <chrono>
#include <thread>
#include <iterator>


namespace com::ft::sdk::internal
{
	CacheDBManager::CacheDBManager()
	{
		m_enableFileCache = FTSDKConfigManager::getInstance().getGeneralConfig().getEnableFileDBCache();
		new std::thread(&CacheDBManager::processData, this);
	}

	void CacheDBManager::deinit()
	{
		m_stopping = true;
	}

	void CacheDBManager::updateRUMCollect(bool collect)
	{
		m_enableRUMCollect = collect;
	}

	void CacheDBManager::insertRUMItem(std::string measurementName, std::map<std::string, std::string> tags, std::map<std::string, std::any> fields)
	{
		if (!m_enableRUMCollect)
		{
			LoggerManager::getInstance().logDebug("CACHE DB: collect status - false.");
			return;
		}

		LoggerManager::getInstance().logDebug("CACHE DB: insert RUM " + measurementName);

		// add global context
		auto& rumGlobalContext = FTSDKConfigManager::getInstance().getRUMConfig().getGlobalContext();
		addGlobalContexts(tags, rumGlobalContext);

		Measurement msm{ measurementName, tags, fields };
		{
			std::unique_lock <std::mutex> lck(m_dataQueMtx);
			m_dqMeasurement.push_back(msm);
		}
	}

	void CacheDBManager::insertLogItem(std::string measurementName, std::map<std::string, std::string> tags, std::map<std::string, std::any> fields)
	{
		LoggerManager::getInstance().logDebug("CACHE DB: insert LOG " + measurementName);

		// add global context
		auto& logGlobalContext = FTSDKConfigManager::getInstance().getLogPipeConfig().getGlobalContext();
		addGlobalContexts(tags, logGlobalContext);

		Measurement msm{ measurementName, tags, fields };
		{
			std::unique_lock <std::mutex> lck(m_dataQueMtx);
			m_dqMeasurement.push_back(msm);
		}
	}

	void CacheDBManager::insertTraceItem(std::string measurementName, std::map<std::string, std::string> tags, std::map<std::string, std::any> fields)
	{
		LoggerManager::getInstance().logDebug("CACHE DB: insert TRACE " + measurementName);

		// add global context
		//auto& traceGlobalContext = FTSDKConfigManager::getInstance().getTraceConfig().getGlobalContext();
		//addGlobalContexts(tags, traceGlobalContext);

		Measurement msm{ measurementName, tags, fields };
		{
			std::unique_lock <std::mutex> lck(m_dataQueMtx);
			m_dqMeasurement.push_back(msm);
		}
	}

	/**
	 * Consumer thread, for the tracking item in the message queue.
	 * 1. if enabling the file DB cache, get the item from message queue, and push it to file cache 
	 * 2. if not enabling the file DB cache, get the item from message queue, and forward it to sync manager
	 * 
	 */
	void CacheDBManager::processData()
	{
		BEGIN_THREAD();

		std::vector<Measurement> vtMeasurement(10);
		while (!m_stopping)
		{
			try
			{
				vtMeasurement.clear();
				int size = m_dqMeasurement.size();
				if (size > 0)
				{
					{
						std::unique_lock <std::mutex> lck(m_dataQueMtx);
						int to_copy = size > in_constants::RUM_ITEM_LIMIT_SIZE ? in_constants::RUM_ITEM_LIMIT_SIZE : size;
						std::copy_n(m_dqMeasurement.begin(), to_copy, std::back_inserter(vtMeasurement));
						m_dqMeasurement.erase(m_dqMeasurement.begin(), m_dqMeasurement.begin() + to_copy);
					}

					DataMsg log;
					log.dataType = DataType::LOG;
					DataMsg msg;
					msg.dataType = DataType::RUM_APP;

					for (auto msm : vtMeasurement)
					{
						if (msm.measurementName == constants::FT_LOG_DEFAULT_MEASUREMENT)
						{
							log.vtLine.push_back(msm);
						}
						else // TODO: handle trace type
						{
							msg.vtLine.push_back(msm);
						}
					}

					if (msg.vtLine.size() > 0)
					{
						if (m_enableFileCache)
						{
							LineDBManager::getInstance().insertLine(msg);
						}
						else
						{
							DataSyncManager::getInstance().sendDataMessage(msg);
						}
					}

					if (log.vtLine.size() > 0)
					{
						if (m_enableFileCache)
						{
							LineDBManager::getInstance().insertLine(log);
						}
						else
						{
							DataSyncManager::getInstance().sendDataMessage(log);
						}
					}
				}
			}
			catch (std::exception err)
			{
				LoggerManager::getInstance().logInfo("added measurements to Message Queue: " + std::string(err.what()));
			}

			std::this_thread::sleep_for(std::chrono::milliseconds(1000));
		}

		END_THREAD();
	}

	void CacheDBManager::addGlobalContexts(TagMap& tags, TagMap& src)
	{
		auto& gc = FTSDKConfigManager::getInstance().getGeneralConfig().getGlobalContext();
		for (auto& entry : gc)
		{
			tags[entry.first] = entry.second;
		}

		for (auto& entry : src)
		{
			tags[entry.first] = entry.second;
		}
	}
}