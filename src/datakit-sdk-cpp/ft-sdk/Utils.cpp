#include "Utils.h"
#include <chrono>
#include <stduuid/uuid.h>
#include <random>
#include <sstream>
#include <thread>
#include <cmath>
#include <algorithm>
#include <regex>
#include <nlohmann/json.hpp>
#include "FTSDKConstants.h"

#include <cstdio>
#include <iostream>
#include <memory>
#include <stdexcept>
#include <string>
#include <array>

namespace com::ft::sdk::internal::utils
{
#ifdef _WIN32
	// wchar_t to string
	void Wchar_tToString(std::string& szDst, wchar_t* wchar)
	{
		wchar_t* wText = wchar;
		DWORD dwNum = WideCharToMultiByte(CP_OEMCP, NULL, wText, -1, NULL, 0, NULL, FALSE);// WideCharToMultiByte的运用
		char* psText; // psText为char*的临时数组，作为赋值给std::string的中间变量
		psText = new char[dwNum];
		WideCharToMultiByte(CP_OEMCP, NULL, wText, -1, psText, dwNum, NULL, FALSE);// WideCharToMultiByte的再次运用
		szDst = psText;// std::string赋值
		delete[]psText;// psText的清除
	}

	// string to wstring
	void StringToWstring(std::wstring& szDst, std::string str)
	{
		std::string temp = str;
		int len = MultiByteToWideChar(CP_ACP, 0, (LPCSTR)temp.c_str(), -1, NULL, 0);
		wchar_t* wszUtf8 = new wchar_t[len + 1];
		memset(wszUtf8, 0, len * 2 + 2);
		MultiByteToWideChar(CP_ACP, 0, (LPCSTR)temp.c_str(), -1, (LPWSTR)wszUtf8, len);
		szDst = wszUtf8;
		std::wstring r = wszUtf8;
		delete[] wszUtf8;
	}


	std::string dirnameOf(const std::string& fname)
	{
		size_t pos = fname.find_last_of("\\/");
		return (std::string::npos == pos) ? "" : fname.substr(0, pos);
	}

	std::string getExecutablePath() 
	{
		WCHAR result[512];
		GetModuleFileName(NULL, result, sizeof(result));
		std::string fname;
		Wchar_tToString(fname, result);
		return dirnameOf(fname);
	}
#elif __linux__
	std::string getExecutablePath()
	{
		// Get current executable file name
		char result[256] = {0};
		ssize_t count = readlink("/proc/self/exe", result, sizeof(result));
		const char* path;
		if (count != -1) {
			path = dirname(result);
		}
		return path;
	}
#endif
	std::int64_t getCurrentNanoTime()
	{
		auto clock = std::chrono::high_resolution_clock::now();

		return (std::int64_t)(std::chrono::duration_cast<std::chrono::nanoseconds>(clock.time_since_epoch()).count());
	}

	bool enableTraceSamplingRate(float sampleRate) 
	{
		return generateRandomNumber() <= sampleRate * 100.0f;
	}

	double generateRandomNumber() 
	{
		//Random random = new Random();
		//return Math.floor(random.nextDouble() * 100);

		std::uniform_real_distribution<double> unif(0.0f, 100.0f);
		std::default_random_engine re;
		double a_random_double = unif(re);

		return std::floor(a_random_double);
	}

	std::uint64_t getCurrentThreadID()
	{
		std::stringstream ss;
		ss << std::this_thread::get_id();
		uint64_t id = std::stoull(ss.str());

		return id;
	}

	std::string base64Encode(const std::string& in) 
	{
		std::string out;

		int val = 0, valb = -6;
		for (unsigned char c : in) {
			val = (val << 8) + c;
			valb += 8;
			while (valb >= 0) {
				out.push_back("ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/"[(val >> valb) & 0x3F]);
				valb -= 6;
			}
		}
		if (valb > -6) out.push_back("ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/"[((val << 8) >> (valb + 8)) & 0x3F]);
		while (out.size() % 4) out.push_back('=');
		return out;
	}

	std::string base64Decode(const std::string& in) 
	{

		std::string out;

		std::vector<int> T(256, -1);
		for (int i = 0; i < 64; i++) T["ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/"[i]] = i;

		int val = 0, valb = -8;
		for (unsigned c : in) {
			if (T[c] == -1) break;
			val = (val << 6) + T[c];
			valb += 6;
			if (valb >= 0) {
				out.push_back(char((val >> valb) & 0xFF));
				valb -= 8;
			}
		}
		return out;
	}

	std::string formatInt(const char* format, int num)
	{
		char buff[10];
		snprintf(buff, sizeof(buff), format, num);
		return std::string(buff);
	}

	std::string convertToLowerCase(std::string str)
	{
			std::transform(str.begin(), str.end(), str.begin(),
				[](unsigned char c) 
				{ 
					return std::tolower(c); 
				} 
			);
			return str;
	}

	std::string getLocalIPAddr()
	{
		return "127.0.0.1";
	}

	std::string generateRandomUUIDWithDash()
	{
		std::random_device rd;
		auto seed_data = std::array<int, std::mt19937::state_size> {};
		std::generate(std::begin(seed_data), std::end(seed_data), std::ref(rd));
		std::seed_seq seq(std::begin(seed_data), std::end(seed_data));
		std::mt19937 generator(seq);
		uuids::uuid_random_generator gen{ generator };

		uuids::uuid const id = gen();
		std::string uuid_str = uuids::to_string(id);

		return uuid_str;
	}

	std::string generateRandomUUID()
	{
		std::string uuid_str = generateRandomUUIDWithDash();
		//return uuid_str.replace(uuid_str.begin(), uuid_str.end(), "-", "");
		return std::regex_replace(uuid_str, std::regex("-"), "");
	}

	std::string replaceString(std::string strOrigin, std::string strToReplace, std::string strNewChar)
	{
		std::string strFinal = strOrigin;
		if (strFinal.empty())
		{
			return strFinal;
		}

		if (strNewChar.empty())
		{
			size_t pos = std::string::npos;

			// Search for the substring in string in a loop until nothing is found
			while ((pos = strFinal.find(strToReplace)) != std::string::npos)
			{
				// If found then erase it from string
				strFinal.erase(pos, strToReplace.length());
			}
		}
		else
		{
			for (std::string::size_type pos(0); pos != std::string::npos; pos += strNewChar.length())
			{
				pos = strFinal.find(strToReplace, pos);
				if (pos != std::string::npos)
					strFinal.replace(pos, strToReplace.length(), strNewChar);
				else
					break;
			}
		}
		return strFinal;
	}

	/*
	* In string field values, you must escape:
	*	double quotes
	*	backslash character	 
	*/
	std::string escapeStringFieldValue(std::string oldStr) 
	{
		oldStr = escapeSpecialCharacters("\\", oldStr);
		oldStr = escapeSpecialCharacters("\"", oldStr);
		return oldStr;
		//return "\"" + oldStr + "\"";
	}

	/**
	 * 转译特殊字符
	 *
	 * @param special
	 * @param oldStr
	 */
	std::string escapeSpecialCharacters(std::string special, std::string oldStr) 
	{
		if (oldStr.find(special) > 0) {
			return replaceString(oldStr, special, "\\" + special);
		}
		return oldStr;
	}

	/*
	* In measurements, you must escape:
	*	commas
	*	spaces
	*/
	std::string escapeMeasurements(std::string oldStr) 
	{
		oldStr = escapeSpecialCharacters(",", oldStr);
		return escapeSpecialCharacters(" ", oldStr);
	}

	/*
	* In tag keys, tag values, and field keys, you must escape:
	*	commas
	*	equal signs
	*	spaces
	*/
	std::string escapeTagAndKeyValue(std::string oldStr) 
	{
		//oldStr = replaceString(oldStr, special, "\\\\" + special); oldStr.replace("\n", " ");
		std::string escStr = escapeSpecialCharacters(",", oldStr);
		escStr = escapeSpecialCharacters("=", escStr);
		return escapeSpecialCharacters(" ", escStr);
	}

	std::string escapeSpecialChar(const std::string& src, const char* esc)
	{
		std::stringstream lines_;

		size_t pos = 0, start = 0;
		while ((pos = src.find_first_of(esc, start)) != std::string::npos) {
			lines_.write(src.c_str() + start, pos - start);
			lines_ << '\\' << src[pos];
			start = ++pos;
		}
		lines_.write(src.c_str() + start, src.length() - start);

		return lines_.str();
	}

	std::string identifyRequest(const std::string& method, const std::string& url, const std::string& contentType, const std::string& body)
	{
		if (body == "") 
		{
			return method + "_" + url;
		}
		else 
		{
			long contentLength = body.size();
			return method + "_" + url + "_" + contentType + "_" + std::to_string(contentLength);

		}
	}

	std::map<std::string, std::string> convertJsonArray2Map(const std::string& jsonStr)
	{
		std::map<std::string, std::string> strMap;

		auto js = nlohmann::json::parse(jsonStr);
		for (nlohmann::json::iterator it = js.begin(); it != js.end(); ++it) {
			strMap[it.key()] = it.value();
		}

		return strMap;
	}

	std::string convertMap2JsonArray(std::map<std::string, std::string> tobeConverted)
	{
		nlohmann::json js(tobeConverted);

		return js.dump();

		//std::string strHdr = "[";
		//for (auto& item : tobeConverted)
		//{
		//	strHdr.append("[\"" + item.first + "\",\"" + item.second + "\"]");
		//	strHdr.append(", ");
		//}
		//strHdr = strHdr.substr(0, strHdr.size() - 2);
		//strHdr.append("]");

		//return strHdr;
	}

	std::string convertMap2Json(std::map<std::string, std::string> tobeConverted)
	{
		std::string strHdr = "{";
		for (auto& item : tobeConverted)
		{
			strHdr.append("\"" + item.first + "\":");
			strHdr.append("\"" + item.second + "\"");
			strHdr.append(", ");
		}
		strHdr = strHdr.substr(0, strHdr.size() - 2);
		strHdr.append("}");

		return strHdr;
	}

	std::string convertVector2Json(std::vector<std::string> tobeConverted)
	{
		std::string strHdr = "[";
		for (auto& item : tobeConverted)
		{
			strHdr.append("\"" + item + "\"");
			strHdr.append(",");
		}
		strHdr = strHdr.substr(0, strHdr.size() - 1);
		strHdr.append("]");

		return strHdr;
	}

	std::string execShellCmd(const std::string& cmd)
	{
#ifdef _WIN32
		throw std::exception("not supported yet");
#elif __linux__
		std::array<char, 128> buffer;
		std::string result;
		std::unique_ptr<FILE, decltype(&pclose)> pipe(popen(cmd.c_str(), "r"), pclose);
		if (!pipe)
		{
			throw std::runtime_error("popen() failed!");
		}
		while (fgets(buffer.data(), buffer.size(), pipe.get()) != nullptr)
		{
			result += buffer.data();
		}
		return result;
#endif 
	}

	std::string trimRet(const std::string& str)
	{
		std::string trimmed = utils::replaceString(str, "\r", "");
		trimmed = utils::replaceString(trimmed, "\n", "");

		return trimmed;
	}

	std::vector<std::string> splitStringByChar(const std::string& str, const char ch)
	{
		auto result = std::vector<std::string>{};
		auto ss = std::stringstream{ str };

		for (std::string line; std::getline(ss, line, ch);)
			result.push_back(line);

		return result;
	}
}