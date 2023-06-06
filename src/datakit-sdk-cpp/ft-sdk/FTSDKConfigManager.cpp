#include "pch.h"

#include "FTSDKConfigManager.h"
#include "Platform.h"
#include "Constants.h"
#include "Utils.h"
#include "RumDataEntity.h"
#include "ConfigFileHandler.h"
#include "Include/Datakit_UUID.h"
#include "FTSDKConstants.h"

namespace com::ft::sdk::internal
{
	FTSDKConfigManager::FTSDKConfigManager()
	{
		m_workingDir = utils::getExecutablePath();
	}

	FTSDKConfigManager&& FTSDKConfigManager::setGeneralConfig(FTSDKConfig& config)
	{
		m_generalConfig = config;

		// init general config context
		initGlobalContext(m_generalConfig);
		
		return std::move(*this);
	}	
	
	FTSDKConfigManager&& FTSDKConfigManager::bindUserData(UserData& config)
	{
		m_userData = config;
		m_isUserBinded = true;

		ConfigFileHandler::getInstance().updateConfig(internal::UpdateConfigType::user);

		return std::move(*this);
	}

	FTSDKConfigManager&& FTSDKConfigManager::unbindUserData()
	{
		m_isUserBinded = false;
		m_userData.reset();

		return std::move(*this);
	}

	FTSDKConfigManager&& FTSDKConfigManager::setRUMConfig(FTRUMConfig& config)
	{
		m_rumConfig = config;

		// init rum global context
		initRUMGlobalContext(m_rumConfig);

		// pass the config to rum manager
		return std::move(*this);
	}

	FTSDKConfigManager&& FTSDKConfigManager::setTraceConfig(FTTraceConfig& config)
	{
		m_traceConfig = config;

		return std::move(*this);
	}

	FTSDKConfigManager&& FTSDKConfigManager::setLogPipeConfig(FTLogConfig& config)
	{
		m_logPipeConfig = config;

		return std::move(*this);
	}

	FTSDKConfigManager&& FTSDKConfigManager::enableRUMUserBinding(bool enabled)
	{
		m_isUserBinded = enabled;

		return std::move(*this);
	}

	void FTSDKConfigManager::initRUMGlobalContext(FTRUMConfig& config)
	{
		std::map<std::string, std::string> rumGlobalContext = config.getGlobalContext();
		std::vector<std::string> customKeys;
		for (auto& entry : rumGlobalContext) 
		{
			customKeys.push_back(entry.first);
			//String key = entry.getKey();
			//customKeys.add(key);
			//Object value = entry.getValue();
			//config.addGlobalContext(key, value);
		}

		config.addGlobalContext(constants::KEY_RUM_CUSTOM_KEYS, utils::convertVector2Json(customKeys));
		config.addGlobalContext(constants::KEY_RUM_APP_ID, config.getRumAppId());
		config.addGlobalContext(constants::KEY_RUM_SESSION_TYPE, "user");
		config.addGlobalContext(constants::KEY_DEVICE_OS, platform::getOSName());
		platform::DeviceInfo devInfo = platform::getDeviceInfo();
		config.addGlobalContext(constants::KEY_DEVICE_DEVICE_BAND, devInfo.band); // DeviceUtils.getDeviceBand());
		config.addGlobalContext(constants::KEY_DEVICE_DEVICE_MODEL, devInfo.model); // DeviceUtils.getDeviceModel());
		config.addGlobalContext(constants::KEY_DEVICE_DISPLAY, devInfo.screen_size); // DeviceUtils.getDisplay(context));

		std::string osVersion = platform::getOSVersion();
		config.addGlobalContext(constants::KEY_DEVICE_OS_VERSION, osVersion);
		std::string osVersionMajor = platform::getMajorVersion(osVersion);
		config.addGlobalContext(constants::KEY_DEVICE_OS_VERSION_MAJOR, osVersionMajor);
	}

	std::map<std::string, std::string> FTSDKConfigManager::getRUMPublicDynamicTags(bool includeRUMStatic)
	{
		std::map<std::string, std::string> vtTrackNodes;

		if (includeRUMStatic) 
		{
			auto& rumGlobalContext = m_rumConfig.getGlobalContext();
			for (auto& entry : rumGlobalContext) 
			{
				vtTrackNodes[entry.first] = entry.second;
			}
		}

		vtTrackNodes[constants::KEY_SERVICE] = internal::FTSDKConfigManager::getInstance().getGeneralConfig().getServiceName();
		vtTrackNodes[constants::KEY_RUM_NETWORK_TYPE] = platform::getNetworkType();
		vtTrackNodes[constants::KEY_RUM_IS_SIGN_IN] = this->m_isUserBinded ? "T" : "F"; 

		if (isUserDataBinded())
		{
			vtTrackNodes[constants::KEY_RUM_USER_ID] = m_userData.getId();
			vtTrackNodes[constants::KEY_RUM_USER_NAME] = m_userData.getName();
			vtTrackNodes[constants::KEY_RUM_USER_EMAIL] = m_userData.getEmail();

			for (auto& item : m_userData.getExts())
			{
				vtTrackNodes[item.first] = item.second;
			}
		}
		else
		{
			vtTrackNodes[constants::KEY_RUM_USER_ID] = RUMApplication::getInstance().getSessionId();
		}

		return vtTrackNodes;
	}

	void FTSDKConfigManager::initGlobalContext(FTSDKConfig& config)
	{
		std::string appVersion = m_generalConfig.getAppVersion();
		if (isDefaultAppVersion())
		{
			appVersion = platform::getAppVersionName();
		}

		config.addGlobalContext(constants::KEY_APP_VERSION_NAME, appVersion);
		config.addGlobalContext(constants::KEY_SDK_NAME, in_constants::SDK_NAME);
		//config.addGlobalContext(constants::KEY_APPLICATION_UUID, ConfigFileHandler::getInstance().getApplicationUuid());
		config.addGlobalContext(constants::KEY_APPLICATION_UUID, DATAKIT_APP_UUID);
		config.addGlobalContext(constants::KEY_ENV, utils::convertToLowerCase(EnumToString(getGeneralConfig().getEnv())));
		config.addGlobalContext(constants::KEY_DEVICE_UUID, platform::getDeviceUUID());

		config.addGlobalContext(constants::KEY_SDK_VERSION, constants::DATAKIT_SDK_VERSION);
	}

	bool FTSDKConfigManager::isDefaultAppVersion()
	{
		return m_generalConfig.getAppVersion() == constants::DEFAULT_APP_VERSION;
	}
}