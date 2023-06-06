#pragma once
#ifndef _DATAKIT_SDK_INTERNAL_CONST_H_
#define _DATAKIT_SDK_INTERNAL_CONST_H_

#include <string>

// TODO: divide this into two pieces: internal and external
namespace com::ft::sdk::internal::in_constants
{
#ifdef _WIN32
	const std::string SDK_NAME = "df_windows_rum_sdk";
#elif __linux__
	const std::string SDK_NAME = "df_linux_rum_sdk";
#endif // _WIN32

	const std::int64_t ACTION_NEED_WAIT_TIME_OUT = 5000000000L;
	const std::int64_t ACTION_NORMAL_TIME_OUT = 100000000L;

	const std::string PACKAGE_UUID = "NA";
	const std::string AGENT_VERSION = "1.4";
	const std::string DEVICE_UUID = "NA";

	const int RUM_ITEM_LIMIT_SIZE = 10;
}

#endif // _DATAKIT_SDK_INTERNAL_CONST_H_