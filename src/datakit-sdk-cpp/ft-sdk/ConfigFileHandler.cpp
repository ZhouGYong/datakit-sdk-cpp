#include "pch.h"
#include "ConfigFileHandler.h"
#include "LoggerManager.h"
#include <fstream>
#include <iostream>
#include "Include/Datakit_UUID.h"
#include "FTSDKConfigManager.h"
#include "Utils.h"

namespace com::ft::sdk::internal
{
	const std::string SECTION_GENERAL_CONFIG = "general_config";
	const std::string SECTION_HTTP_CONFIG = "http_config";
	const std::string SECTION_USER_CONFIG = "user_data";

	bool ConfigFileHandler::load(const std::string& filename)
	{
		if (filename == "")
		{
			m_configFilePath = FTSDKConfigManager::getInstance().getConfigFilePath();
		}
		std::fstream instream;

		instream.open(m_configFilePath.c_str());
		if (!instream.is_open())
		{
			//std::cout << "failed to load config file: " << m_configFilePath << std::endl;

			return false;
		}

		nlohmann::json j;

		try
		{
			instream >> j; /* Load config file to json structure */
			instream.close();

			if (j.count(SECTION_GENERAL_CONFIG) > 0)
			{
				//m_application_uuid = j[SECTION_GENERAL_CONFIG]["application_uuid"];
				if (j[SECTION_GENERAL_CONFIG].count("enable_sdk_log") > 0)
				{
					m_enableSDKLog = j[SECTION_GENERAL_CONFIG]["enable_sdk_log"].get<bool>();
					LoggerManager::getInstance().enableLog(m_enableSDKLog);
				}
			}
			
			m_serverUrl = tryGetValue(j, { SECTION_HTTP_CONFIG, "serverUrl"});

			if (j.count(SECTION_USER_CONFIG) > 0)
			{
				m_username = tryGetValue(j, { SECTION_USER_CONFIG, "name" });
				m_userId = tryGetValue(j, { SECTION_USER_CONFIG, "id" });
				m_userEmail = tryGetValue(j, { SECTION_USER_CONFIG, "email" });

				UserData& uc = FTSDKConfigManager::getInstance().getUserData();
				uc.setName(m_username);
				uc.setId(m_userId);
				uc.setEmail(m_userEmail);

				auto& exts = uc.getExts();
				exts.clear();
				auto map = utils::convertJsonArray2Map(j[SECTION_USER_CONFIG]["extdata"].get<std::string>());
				for (auto& kv : map)
				{
					uc.addCustomizeItem(kv.first, kv.second);
				}
			}
			m_inited = true;
		}
		//catch (nlohmann::detail::parse_error e)
		catch (std::exception e)
		{
			LoggerManager::getInstance().logError("failed to load config file: " + m_configFilePath);
			instream.close();
			return false;
		}

		return true;
	}

	bool ConfigFileHandler::updateConfig(UpdateConfigType type, bool isClear)
	{
		//const std::string filePath = FTSDKConfigManager::getInstance().getConfigFilePath();
		std::ifstream istream(m_configFilePath.c_str(), std::ios::in);

		nlohmann::json j;
		if (istream.is_open())
		{
			try
			{
				istream >> j; 
				istream.close();

			}
			catch (nlohmann::detail::parse_error e)
			{
				j = "{}"_json;
				LoggerManager::getInstance().logError("failed to load config file to update: " + m_configFilePath);
				istream.close();

			}
		}

		//j[SECTION_GENERAL_CONFIG]["application_uuid"] = DATAKIT_APP_UUID;
		j[SECTION_GENERAL_CONFIG]["enable_sdk_log"] = m_enableSDKLog;

		if (type == UpdateConfigType::user)
		{
			UserData uc = FTSDKConfigManager::getInstance().getUserData();
			if (isClear)
			{
				j[SECTION_USER_CONFIG].clear();
				uc.reset();
			}
			else
			{
				j[SECTION_USER_CONFIG]["name"] = uc.getName();
				j[SECTION_USER_CONFIG]["id"] = uc.getId();
				j[SECTION_USER_CONFIG]["email"] = uc.getEmail();
				auto& exts = uc.getExts();
				j[SECTION_USER_CONFIG]["extdata"] = utils::convertMap2JsonArray(exts);
			}
		}

		if (type == UpdateConfigType::http)
		{
			if (isClear)
			{
				j[SECTION_HTTP_CONFIG].clear();
			}
			else
			{
				FTSDKConfig hc = FTSDKConfigManager::getInstance().getGeneralConfig();
				j[SECTION_HTTP_CONFIG]["serverUrl"] = hc.getServerUrl();
			}
		}

		std::ofstream ostream(m_configFilePath.c_str(), std::ios::out);
		try
		{
			if (ostream.is_open())
			{
				ostream << std::setw(4) << j;
				ostream.close();
			}
		}
		catch (std::exception e)
		{
			LoggerManager::getInstance().logError("failed to save config file: " + m_configFilePath);
			ostream.close();
			return false;
		}

		return true;
	}

	std::string ConfigFileHandler::tryGetValue(nlohmann::json& j, std::vector<std::string> keys)
	{
		std::string val;

		bool found = true;
		nlohmann::json tempJ = j;
		for (auto& key : keys)
		{
			if (tempJ.count(key) > 0)
			{
				tempJ = tempJ[key];
			}
			else
			{
				found = false;
				break;
			}
		}

		if (found)
		{
			val = tempJ.get<std::string>();
		}

		return val;
	}
}