#include "pch.h"
#include "LogPipeManager.h"
#include "Include/FTSDKConfig.h"
#include "FTSDKConfigManager.h"
#include "LoggerManager.h"
#include "CacheDBManager.h"
#include "Utils.h"
#include "RUMManager.h"
#include "FTSDKConstants.h"

namespace com::ft::sdk::internal
{
    void LogPipeManager::init(FTLogConfig& config)
    {
        m_pLogPipeConfig = &(FTSDKConfigManager::getInstance().getLogPipeConfig());
    }

    bool LogPipeManager::checkLogLevel(LogLevel level)
    {
        //return m_pLogPipeConfig->checkLogLevel(level);
        return true;
    }

	void LogPipeManager::addLog(std::string content, LogLevel level)
	{
        //if (!checkConfig())return;

        if (!m_pLogPipeConfig->getEnableCustomLog())
        {
            return;
        }

        LoggerManager::getInstance().logDebug("add log: " + content);
        try
        {
            if (checkLogLevel(level)) {
                std::int64_t time = internal::utils::getCurrentNanoTime();

                std::string logMsg = utils::escapeStringFieldValue(content);
                if (content.size() > LOG_LIMIT_SIZE) 
                {
                    logMsg = content.substr(0, LOG_LIMIT_SIZE);
                    LoggerManager::getInstance().logDebug("truncated log: " + logMsg);
                }


                std::map<std::string, std::string> tags;
                tags[constants::KEY_SERVICE] = internal::FTSDKConfigManager::getInstance().getGeneralConfig().getServiceName();
                tags[constants::KEY_STATUS] = utils::convertToLowerCase(EnumToString(level));

                FieldMap fields;
                fields[constants::KEY_MESSAGE] = logMsg;

                if (m_pLogPipeConfig->getEnableLinkRumData())
                {
                    auto publicTags = internal::FTSDKConfigManager::getInstance().getRUMPublicDynamicTags(true);
                    for (auto tag : publicTags)
                    {
                        tags[tag.first] = tag.second;
                    }
                    internal::RUMManager::getInstance().attachRUMRelative(tags, true);
                }

                if (utils::enableTraceSamplingRate(m_pLogPipeConfig->getSamplingRate()))
                {
                    // TODO: check log synchronization policy
                    CacheDBManager::getInstance().insertLogItem(constants::FT_LOG_DEFAULT_MEASUREMENT, tags, fields);
                }
            }
        }
        catch (std::exception e)
        {
            internal::LoggerManager::getInstance().logError("failed to add long task: ");
        }
	}

    // TODO: 
    //void judgeLogCachePolicy(@NonNull List<SyncJsonData> recordDataList) {
    //    //如果 OP 类型不等于 LOG 则直接进行数据库操作；否则执行同步策略，根据同步策略返回结果判断是否需要执行数据库操作
    //    int length = recordDataList.size();
    //    int policyStatus = FTDBCachePolicy.get().optLogCachePolicy(length);
    //    if (policyStatus >= 0) {//执行同步策略
    //        if (policyStatus > 0) {
    //            for (int i = 0; i < policyStatus && i < length; i++) {
    //                recordDataList.remove(0);
    //            }
    //        }
    //        boolean result = FTDBManager.get().insertFtOptList(recordDataList);
    //        LogUtils.d(TAG, "judgeLogCachePolicy:insert-result=" + result);
    //        SyncTaskManager.get().executeSyncPoll();
    //    }
    //}
}