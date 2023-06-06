#include "pch.h"
#include "CommunicationManager.h"
#include "FTSDKConstants.h"
#include <ctime>
#include <iostream>
#include <cstring>
#include "LoggerManager.h"
#include "FTSDKConfigManager.h"

namespace com::ft::sdk::internal
{

	void CommunicationManager::initialize(FTSDKConfig& config)
	{
		m_generalConfig = config;
		setHeadParam();

		// initialize RestClient
		RestClient::init();

		// get a connection object
		m_pConnection = std::make_shared<RestClient::Connection>(m_generalConfig.getServerUrl());

		// configure basic auth
		//m_pConnection->SetBasicAuth("WarMachine68", "WARMACHINEROX");

		// set connection timeout to 5s
		m_pConnection->SetTimeout(5);

		// set custom user agent
		// (this will result in the UA "foo/cool restclient-cpp/VERSION")
		m_pConnection->SetUserAgent(constants::USER_AGENT);

		// enable following of redirects (default is off)
		m_pConnection->FollowRedirects(true);
		// and limit the number of redirects (default is -1, unlimited)
		m_pConnection->FollowRedirects(true, 3);

		// set headers
		RestClient::HeaderFields headers;
		headers["Accept"] = "text/plain";
		m_pConnection->SetHeaders(headers);

		// append additional headers
		for (auto& item : m_headParams)
		{
			m_pConnection->AppendHeader(item.first, item.second);
		}

		// if using a non-standard Certificate Authority (CA) trust file
		//m_pConnection->SetCAInfoFilePath("/etc/custom-ca.crt");

		m_pConnection->AppendHeader("Content-Type", "text/plain");
	}

	void CommunicationManager::deinitialize()
	{
		if (nullptr != m_pConnection)
		{
			m_pConnection->Terminate();
			m_pConnection = nullptr;
		}
		// deinit RestClient. After calling this you have to call RestClient::init()
		// again before you can use it
		RestClient::disable();
	}

	void CommunicationManager::setHeadParam()
	{
		//addHeadParam("X-Datakit-UUID", constants::X_DATAKIT_UUID);
		//addHeadParam("User-Agent", constants::USER_AGENT +
		//	";agent_" + constants::AGENT_VERSION +
		//	";autotrack_" + constants::PLUGIN_VERSION +
		//	";native"
		//);
		addHeadParam("User-Agent", constants::USER_AGENT +
			";agent_" + constants::DATAKIT_SDK_VERSION);

		addHeadParam("Accept-Language", "zh-CN");
		addHeadParam("Content-Type", constants::CONTENT_TYPE, false);

		addHeadParam("charset", constants::CHARSET);
		addHeadParam("Date", calculateDate());
	}

	std::string CommunicationManager::calculateDate()
	{
		//Date currentTime = new Date();
		//SimpleDateFormat sdf = new SimpleDateFormat("EEE, dd MMM yyyy HH:mm:ss 'GMT'", Locale.UK);
		//sdf.setTimeZone(TimeZone.getTimeZone("GMT"));
		//return sdf.format(currentTime);

		std::time_t time = std::time({});
		const int sz = std::size("yyyy-mm-ddThh:mm:ssZ");
		char timeString[sz];
		std::memset(timeString, 0x0, sz);
		std::strftime(std::data(timeString), std::size(timeString), "%FT%TZ", std::gmtime(&time));
		//std::cout << timeString << '\n';
		return std::string(timeString);
	}

	ResponseData CommunicationManager::post(DataType dt, std::string& data)
	{
		internal::LoggerManager::getInstance().logInfo("posting the event (" + data + ") to datakit agent...");

		std::string uri = "";
		switch (dt)
		{
		case DataType::RUM_APP:
		case DataType::RUM_WEBVIEW:
			uri = constants::URL_MODEL_RUM;
			break;
		case DataType::LOG:
			uri = constants::URL_MODEL_LOG;
			break;
		case DataType::TRACE:
			uri = constants::URL_MODEL_TRACING;
			break;
		default:
			break;
		}

		if (FTSDKConfigManager::getInstance().isOfflineMode())
		{
			std::cout << uri << " -- \n" << data << std::endl;
			ResponseData resDt = { HTTP_STATUS::HTTP_OK , "Test" };

			return resDt;
		}
		else
		{
			try
			{
				RestClient::Response r = m_pConnection->post(uri, data);
				ResponseData resDt = { r.code, r.body };
				return resDt;
			}
			catch (std::exception ex)
			{
				ResponseData resDt = { HTTP_STATUS::HTTP_BAD_REQUEST, ex.what() };

				return resDt;
			}

		}
	}

}