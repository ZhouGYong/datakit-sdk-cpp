/*****************************************************************//**
 * \file   CommunicationManager.h
 * \brief  HTTP Communication encapsulation
 * 
 * \author Zhou Guangyong
 * \date   Dec 2022
 *********************************************************************/
#pragma once
#ifndef _DATAKIT_SDK_COMM_MANAGER_H_
#define _DATAKIT_SDK_COMM_MANAGER_H_

#include <map>
#include <string>
#include "restclient-cpp/restclient.h"
#include "restclient-cpp/connection.h"
#include "Include/FTSDKConfig.h"
#include "InternalEnums.h"
#include "Singleton.h"

namespace com::ft::sdk::internal
{
	struct ResponseData
	{
		int code;
		std::string message;
	};

	class CommunicationManager : public Singleton<CommunicationManager>
	{
	private:
		CommunicationManager() {}
		std::string calculateDate();

	public:
		void initialize(FTSDKConfig& config);
		void deinitialize();

		void addHeadParam(const std::string& key, const std::string& value, bool overwrite = true)
		{
			if (overwrite || m_headParams.find(key) == m_headParams.end())
			{
				m_headParams[key] = value;
			}
		}
		void setHeadParam();
		ResponseData post(DataType dt, std::string& data);

	private:
		FTSDKConfig m_generalConfig;
		std::shared_ptr<RestClient::Connection> m_pConnection;

		std::map<std::string, std::string> m_headParams;

		ENABLE_SINGLETON();
	};

}

#endif // _DATAKIT_SDK_COMM_MANAGER_H_