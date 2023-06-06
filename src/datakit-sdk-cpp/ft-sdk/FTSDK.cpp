/*****************************************************************//**
 * \file   FTSDK.cpp
 * \brief  Datakit SDK Interface: it's basically a transparent proxy,
 *		   forwarding all the SDK operations to the internal managers.
 * 
 * \author Zhou Guangyong
 * \date   October 2022
 *********************************************************************/
#include "pch.h"
#include "Include/FTSDK.h"
#include "DataSyncManager.h"
#include "FTSDKConfigManager.h"
#include "CommunicationManager.h"
#include "LoggerManager.h"
#include "TrackNodeImpl.h"
#include "TraceManager.h"
#include "InternalStructs.h"
#include "RUMManager.h"
#include "LogPipeManager.h"
#include "Platform.h"
#include "ConfigFileHandler.h"
#include "MonitorManager.h"
#include "LineDBManager.h"
#include "CacheDBManager.h"
#include "FTSDKConstants.h"
#include "FTSDKError.h"

namespace com::ft::sdk
{
	FTSDK::FTSDK(const std::string& configJson)
	{
		// load config
		internal::FTSDKConfigManager::getInstance().setConfigFileName(configJson);
		internal::ConfigFileHandler::getInstance().load();
	}

	void FTSDK::init()
	{
		internal::platform::registerSystemExceptionHandler();

		FTSDK_LOG_EXCEPTION(internal::LoggerManager::getInstance().init());
		_LOG_INFO("SDK initializing...");

		FTSDK_LOG_EXCEPTION(internal::LineDBManager::getInstance().init());
		FTSDK_LOG_EXCEPTION(internal::FTSDKConfigManager::getInstance().setSDKInited(true));
	}

	void FTSDK::deinit()
	{
		CHECK_SDK_INITED();
		_LOG_INFO("SDK uninitializing...");

		FTSDK_LOG_EXCEPTION(internal::DataSyncManager::getInstance().deinit());
		FTSDK_LOG_EXCEPTION(internal::CacheDBManager::getInstance().deinit());
	}

	std::string FTSDK::getVersionString()
	{
		return constants::DATAKIT_SDK_VERSION;
	}

	FTSDK&& FTSDK::install(FTSDKConfig& config)
	{
		CHECK_SDK_INITED();
		FTSDKConfig cpConfig = config;
		if (cpConfig.getAppVersion() == "")
		{
			cpConfig.setAppVersion(constants::DEFAULT_APP_VERSION);
		}

		if (cpConfig.getServiceName() == "")
		{
			cpConfig.setServiceName(constants::DEFAULT_LOG_SERVICE_NAME);
		}

		FTSDK_LOG_EXCEPTION(internal::FTSDKConfigManager::getInstance().setGeneralConfig(cpConfig));
		FTSDK_LOG_EXCEPTION(internal::DataSyncManager::getInstance().init());
		FTSDK_LOG_EXCEPTION(internal::CommunicationManager::getInstance().initialize(cpConfig));

		_LOG_INFO("initialized general config.");

		return std::move(*this);
	}

	FTSDK&& FTSDK::bindUserData(UserData& config)
	{
		CHECK_SDK_INITED();
		FTSDK_LOG_EXCEPTION(internal::FTSDKConfigManager::getInstance().bindUserData(config));

		_LOG_INFO("initialized user config.");

		return std::move(*this);
	}

	void FTSDK::unbindUserData()
	{
		CHECK_SDK_INITED();
		FTSDK_LOG_EXCEPTION(internal::FTSDKConfigManager::getInstance().unbindUserData());
		FTSDK_LOG_EXCEPTION(internal::ConfigFileHandler::getInstance().updateConfig(internal::UpdateConfigType::user, true));
	}

	FTSDK&& FTSDK::initRUMWithConfig(FTRUMConfig& config)
	{
		CHECK_SDK_INITED();
		FTSDK_LOG_EXCEPTION(internal::FTSDKConfigManager::getInstance().setRUMConfig(config));
		FTSDK_LOG_EXCEPTION(internal::MonitorManager::getInstance().init());
		FTSDK_LOG_EXCEPTION(internal::RUMManager::getInstance().init());
		_LOG_INFO("initialized RUM config.");

		return std::move(*this);
	}

	FTSDK&& FTSDK::initTraceWithConfig(FTTraceConfig& config)
	{
		CHECK_SDK_INITED();
		FTSDK_LOG_EXCEPTION(internal::FTSDKConfigManager::getInstance().setTraceConfig(config));
		_LOG_INFO("initialized trace config.");
		FTSDK_LOG_EXCEPTION(internal::TraceManager::getInstance().initialize(config));

		return std::move(*this);
	}

	FTSDK&& FTSDK::initLogWithConfig(FTLogConfig& config)
	{
		CHECK_SDK_INITED();
		FTSDK_LOG_EXCEPTION(internal::FTSDKConfigManager::getInstance().setLogPipeConfig(config));
		_LOG_INFO("initialized Log Pipe config.");
		FTSDK_LOG_EXCEPTION(internal::LogPipeManager::getInstance().init(config));

		return std::move(*this);
	}

#ifdef _SUPPORT_CUSTOM_TRACK_
	void FTSDK::addCustomTrack(const std::shared_ptr<TrackNode> pTrackNodes)
	{
		CHECK_SDK_INITED();
		auto pTN = std::dynamic_pointer_cast<internal::TrackNodeImpl>(pTrackNodes);

		internal::DataMsg msg;
		msg.dataType = DataType::RUM_APP;
		internal::Measurement& msm = msg.addMeasurement();
		msm.measurementName = pTN->getTrackName();
		
		for (auto nd : pTN->getTrackPropertyList())
		{
			msm.fields[nd.first] = nd.second;
		}
		
		auto publicTags = internal::FTSDKConfigManager::getInstance().getRUMPublicDynamicTags();
		for (auto nd : publicTags)
		{
			msm.tags[nd.first] = nd.second;
		}
		auto globalTags = internal::FTSDKConfigManager::getInstance().getRUMConfig().getGlobalContext();
		for (auto& gt : globalTags)
		{
			msm.tags[gt.first] = gt.second;
		}
		FTSDK_LOG_EXCEPTION(internal::DataSyncManager::getInstance().sendDataMessage(msg));
	}
#endif // _SUPPORT_CUSTOM_TRACK_

	PropagationHeader FTSDK::generateTraceHeader(const std::string& urlStr)
	{
		CHECK_SDK_INITED();
		FTSDK_CHECK_RETURN(PropagationHeader, internal::TraceManager::getInstance().getTraceHeader(urlStr));
	}

	PropagationHeader FTSDK::generateTraceHeader(const std::string& resourceId, const std::string& urlStr)
	{
		CHECK_SDK_INITED();
		FTSDK_CHECK_RETURN(PropagationHeader, internal::TraceManager::getInstance().getTraceHeader(resourceId, urlStr));
	}

	void FTSDK::addLog(std::string content, LogLevel level)
	{
		CHECK_SDK_INITED();
		FTSDK_LOG_EXCEPTION(internal::LogPipeManager::getInstance().addLog(content, level));
	}

	// --- RUM interface
	void FTSDK::addLongTask(std::string log, long duration)
	{
		CHECK_SDK_INITED();
		FTSDK_LOG_EXCEPTION(internal::RUMManager::getInstance().addLongTask(log, duration));
	}

	void FTSDK::addError(std::string log, std::string message, RUMErrorType errorType, AppState state)
	{
		CHECK_SDK_INITED();
		FTSDK_LOG_EXCEPTION(internal::RUMManager::getInstance().addError(log, message, errorType, state));
	}

	void FTSDK::addResource(std::string resourceId, ResourceParams params, NetStatus netStatusBean)
	{
		CHECK_SDK_INITED();
		FTSDK_LOG_EXCEPTION(internal::RUMManager::getInstance().addResource(resourceId, params, netStatusBean));
	}

	void FTSDK::startResource(std::string resourceId)
	{
		CHECK_SDK_INITED();
		FTSDK_LOG_EXCEPTION(internal::RUMManager::getInstance().startResource(resourceId));
	}

	void FTSDK::stopResource(std::string resourceId)
	{
		CHECK_SDK_INITED();
		FTSDK_LOG_EXCEPTION(internal::RUMManager::getInstance().stopResource(resourceId));
	}

	void FTSDK::addAction(std::string actionName, std::string actionType, long duration, long startTime)
	{
		CHECK_SDK_INITED();
		FTSDK_LOG_EXCEPTION(internal::RUMManager::getInstance().addAction(actionName, actionType, duration, startTime));
	}

	void FTSDK::startAction(std::string actionName, std::string actionType)
	{
		CHECK_SDK_INITED();
		FTSDK_LOG_EXCEPTION(internal::RUMManager::getInstance().startAction(actionName, actionType));
	}

	void FTSDK::stopAction()
	{
		CHECK_SDK_INITED();
		FTSDK_LOG_EXCEPTION(internal::RUMManager::getInstance().stopAction());
	}

	void FTSDK::startView(std::string viewName)
	{
		CHECK_SDK_INITED();
		FTSDK_LOG_EXCEPTION(internal::RUMManager::getInstance().startView(viewName));
	}

	void FTSDK::stopView()
	{
		CHECK_SDK_INITED();
		FTSDK_LOG_EXCEPTION(internal::RUMManager::getInstance().stopView());
	}
	// --- end RUM interface
}