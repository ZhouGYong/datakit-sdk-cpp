/*****************************************************************//**
 * \file   utils.h
 * \brief  Global Utilities
 * 
 * \author Zhou Guangyong
 * \date   October 2022
 *********************************************************************/
#pragma once
#ifndef _DATAKIT_UTILS_H_
#define _DATAKIT_UTILS_H_

#include "pch.h"
#include <string>
#include <map>
#include <vector>

#if defined(WIN32)
#include <windows.h>
#include <io.h>
#elif __linux__
#include <unistd.h>
#include <sys/types.h>
#include <dirent.h>
#include <dlfcn.h>
#include <stdlib.h>
#include <libgen.h>
#endif

namespace com::ft::sdk::internal::utils
{
#ifdef _WIN32
	// wchar_t to string
	void Wchar_tToString(std::string& szDst, wchar_t* wchar);
	// string to wstring
	void StringToWstring(std::wstring& szDst, std::string str);
	std::string dirnameOf(const std::string& fname);
#endif

	std::string getExecutablePath();

	std::int64_t getCurrentNanoTime();

	bool enableTraceSamplingRate(float sampleRate);
	double generateRandomNumber();

	std::uint64_t getCurrentThreadID();

	std::string base64Encode(const std::string& in);
	std::string base64Decode(const std::string& in);

	std::string formatInt(const char* format, int num);
	std::string convertToLowerCase(std::string str); 

	// TODO: define the behavior when there are multiple network adapters
	std::string getLocalIPAddr();

	std::string generateRandomUUIDWithDash();
	std::string generateRandomUUID();

	std::string replaceString(std::string strOrigin, std::string strToReplace, std::string strNewChar);
	std::string escapeSpecialCharacters(std::string special, std::string oldStr);
	std::string escapeMeasurements(std::string oldStr);
	std::string escapeTagAndKeyValue(std::string oldStr);
	std::string escapeStringFieldValue(std::string oldStr);

	std::string escapeSpecialChar(const std::string& src, const char* esc);

	std::string identifyRequest(const std::string& method, const std::string& url, const std::string& contentType, const std::string& body);

	std::map<std::string, std::string> convertJsonArray2Map(const std::string& jsonStr);
	std::string convertMap2JsonArray(std::map<std::string, std::string> tobeConverted);

	std::string convertMap2Json(std::map<std::string, std::string> tobeConverted);
	std::string convertVector2Json(std::vector<std::string> tobeConverted);

	std::string execShellCmd(const std::string& cmd);
	std::string trimRet(const std::string& str);

	std::vector<std::string> splitStringByChar(const std::string& str, const char ch);
}


#endif // !_DATAKIT_UTILS_H_
