/*****************************************************************//**
 * \file   ConfigFileHandler.h
 * \brief  Config file handler: support loading the sdk settings from the 
 *		   config file.
 * 
 * \author Zhou Guangyong
 * \date   March 2023
 *********************************************************************/
#pragma once
#ifndef _DATAKIT_SDK_CONFIG_FILE_HANDLER_H_
#define _DATAKIT_SDK_CONFIG_FILE_HANDLER_H_

#include "Singleton.h"
#include "Constants.h"
#include <string>
#include <nlohmann/json.hpp>

namespace com::ft::sdk::internal
{
	enum class UpdateConfigType
	{
		genernal = 0x0,
		http,
		user,
		rum,
		trace,
		log
	};

	class ConfigFileHandler : public Singleton<ConfigFileHandler>
	{
	public:

		bool load(const std::string& filePath = "");
		bool updateConfig(UpdateConfigType type, bool isClear = false);

		std::string getApplicationUuid()
		{
			return m_application_uuid;
		}

		std::string getUsername()
		{
			return m_serverUsername;
		}

		std::string getPassword()
		{
			return m_serverPassword;
		}
		void setUsername(const std::string& name)
		{
			this->m_serverUsername = name;
		}
		void setPassword(const std::string& pwd)
		{
			this->m_serverPassword = pwd;
		}
		void setServerUrl(const std::string& url)
		{
			this->m_serverUrl = url;
		}

		bool isInited()
		{
			return m_inited;
		}

		bool isEnableSDKLog()
		{
			return m_enableSDKLog;
		}
	private:
		ConfigFileHandler() {}
		std::string tryGetValue(nlohmann::json& j, std::vector<std::string> keys);

	private:
		std::string m_application_uuid = in_constants::PACKAGE_UUID;

		std::string m_serverUrl;
		std::string m_serverUsername;
		std::string m_serverPassword;

		std::string m_username;
		std::string m_userId;
		std::string m_userEmail;

		bool m_inited = false;

		std::string m_configFilePath;

		bool m_enableSDKLog = false;

		ENABLE_SINGLETON();
	};
}

#endif // !_DATAKIT_SDK_CONFIG_FILE_HANDLER_H_



