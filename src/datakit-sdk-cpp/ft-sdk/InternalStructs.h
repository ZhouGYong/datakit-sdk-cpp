#pragma once
#ifndef _INTERNAL_STRUCT_H_
#define _INTERNAL_STRUCT_H_

#include <string>
#include "Include/FTSDKDataContracts.h"

namespace com::ft::sdk::internal
{
	class PropagationUrl
	{
	public:
		std::string protocol;
		std::string domain;
		std::string path;
		std::string query;
		int port;

		PropagationUrl();

		/// <summary>
		/// parse the input url to generate the sub parts
		/// </summary>
		/// <param name="url"></param>
		static PropagationUrl parse(const std::string& url);

		std::string& getHost() { return domain; }
		std::string& getPath() { return path; }
		std::string& getQuery() { return query; }
		int getPort() { return port; }

	};

	struct HttpUrl {
		std::string domain;
		std::string path;
		int port;
		std::string holeUrl;

		std::string& getHost() { return domain; }
		std::string& getPath() { return path; }
		int getPort() { return port; }
	};

	class NetStatusHelper
	{
	public:
		static long getTcpTime(NetStatus& netStatus);
		static long getDNSTime(NetStatus& netStatus);
		static long getResponseTime(NetStatus& netStatus);
		static long getTTFB(NetStatus& netStatus);
		static long getFirstByteTime(NetStatus& netStatus);
		static long getHoleRequestTime(NetStatus& netStatus);
		static long getSSLTime(NetStatus& netStatus);
		static void reset(NetStatus& netStatus);
	};
}
#endif // !_INTERNAL_STRUCT_H_

