/*****************************************************************//**
 * \file   Platform.h
 * \brief  Encapsulate the platform dependent operations
 * 
 * \author Zhou Guangyong
 * \date   February 2023
 *********************************************************************/
#pragma once
#ifndef _DATAKIT_SDK_PLATFORM_H_
#define _DATAKIT_SDK_PLATFORM_H_

#include <string>

namespace com::ft::sdk::internal::platform
{
	std::string getOSName();
	std::string getOSVersion();
	std::string getDeviceUUID();

	std::string getMajorVersion(const std::string& versionString);

#ifdef _WIN32
	typedef struct _dmi_header
	{
		BYTE type;
		BYTE length;
		WORD handle;
	}dmi_header;



	typedef struct _RawSMBIOSData
	{
		BYTE    Used20CallingMethod;
		BYTE    SMBIOSMajorVersion;
		BYTE    SMBIOSMinorVersion;
		BYTE    DmiRevision;
		DWORD   Length;
		BYTE    SMBIOSTableData[];
	}RawSMBIOSData;

	bool dmi_system_uuid(const BYTE* p, short ver, std::string* pStrUuid);
	const char* dmi_string(const dmi_header* dm, BYTE s);
	unsigned long long FileTimeToInt64(const FILETIME& ft);
#endif

	float CalculateCPULoad(unsigned long long idleTicks, unsigned long long totalTicks);
	float GetCPULoad();

	float getMemoryLoad();

	std::string getNetworkType();
	std::string getAppVersionName();

	struct DeviceInfo
	{
		std::string band = "Unknown";
		std::string model = "Unknown";
		std::string screen_size = "";
	};
	DeviceInfo getDeviceInfo();

	void registerSystemExceptionHandler();
}
#endif // _DATAKIT_SDK_PLATFORM_H_