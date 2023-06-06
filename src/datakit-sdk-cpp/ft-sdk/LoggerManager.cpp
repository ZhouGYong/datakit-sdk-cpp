#include "pch.h"

#include <iostream>
#include <sstream>
#include "LoggerManager.h"
#include "Utils.h"


#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/sinks/daily_file_sink.h>
#ifdef WIN32
#include <io.h>
#include <direct.h>
#else
//#include <spdlog/sinks/ansicolor_sink.h>
//#include <spdlog/sinks/file_sinks.h>
#include<unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/mount.h>
#endif // WIN32

#define ENABLE_CONSOLE_LOG 1

namespace com::ft::sdk::internal
{
	static std::shared_ptr<spdlog::logger> logger;
	static std::shared_ptr<spdlog::logger> generalFileLogger;

	void LoggerManager::init()
	{
		if (!m_enableLog)
		{
			return;
		}

        std::string logFolder = utils::getExecutablePath() + "/logs";
#ifdef WIN32
        // determine if the app can read and write the folder
        if (_access(logFolder.c_str(), 0) == -1)
        {
            _mkdir(logFolder.c_str());
        }
#endif
		// Setup logging
		setupLogger(logFolder + "/datakit.log");
	}

	void LoggerManager::setupLogger(std::string appLogFileName) 
	{
		//logger->set_pattern("[%^%l%$] %v");
		spdlog::set_pattern("[%Y-%m-%d %H:%M:%S.%e] [%n] [%^%L%$] [t=%t] %v");
		spdlog::flush_every(std::chrono::seconds(1));

#if ENABLE_CONSOLE_LOG
//#ifdef WIN32
		if (logger == nullptr)
		{
			logger = spdlog::stdout_color_mt("FTSDK");
		}
//#elif __Linux__
//		logger = spdlog::ansicolor_stdout_sink_mt("FTSDK");
//#endif
		logger->set_level(spdlog::level::debug);
#endif // ENABLE_CONSOLE_LOG

		auto init_hour = 0;
		auto init_minite = 0;

		if (generalFileLogger == nullptr)
		{
			generalFileLogger = spdlog::daily_logger_mt(LOGGER_NAME, appLogFileName.c_str(), init_hour, init_minite);
		}
	}

	int LoggerManager::putLog(int severity, char const* component, char const* message) 
	{
		if (!m_enableLog)
		{
			return 0;
		}

		if (!component || component[0] == 0)
		{
			generalFileLogger->log((spdlog::level::level_enum)severity, message);
#if ENABLE_CONSOLE_LOG
			logger->log((spdlog::level::level_enum)severity, message);
#endif //ENABLE_CONSOLE_LOG
		}
		else
		{
			generalFileLogger->log((spdlog::level::level_enum)severity, std::string(component) + ": " + message);
#if ENABLE_CONSOLE_LOG
			logger->log((spdlog::level::level_enum)severity, message);
#endif //ENABLE_CONSOLE_LOG
		}

		return 0;
	}

	//template<typename... Args>
	//void LoggerManager::logTrace(const std::string& fmt, Args const& ... args)
	//{
	//	//std::cout << "INFO: " << info << std::endl;
	//	std::string info = fmt::format(fmt, args...);
	//	putLog(SPDLOG_LEVEL_TRACE, m_component.c_str(), info.c_str());
	//}

	//template<typename... Args>
	//void LoggerManager::logInfo(const std::string& fmt, Args const& ... args)
	//{
	//	//std::cout << "INFO: " << info << std::endl;
	//	std::string info = fmt::format(fmt, args...);
	//	putLog(SPDLOG_LEVEL_INFO, m_component.c_str(), info.c_str());
	//}

	//template<typename... Args>
	//void LoggerManager::logError(const std::string& fmt, Args const& ... args)
	//{
	//	//std::cout << "ERROR: " << info << std::endl;
	//	std::string info = fmt::format(fmt, args...);
	//	putLog(SPDLOG_LEVEL_ERROR, m_component.c_str(), info.c_str());
	//}

	//template<typename... Args>
	//void LoggerManager::logDebug(const std::string& fmt, Args const& ... args)
	//{
	//	//std::cout << "DEBUG: " << info << std::endl;
	//	std::string info = fmt::format(fmt, args...);
	//	putLog(SPDLOG_LEVEL_DEBUG, m_component.c_str(), info.c_str());
	//}

	//template<typename... Args>
	//void LoggerManager::logWarn(const std::string& fmt, Args const& ... args)
	//{
	//	//std::cout << "WARN: " << info << std::endl;
	//	std::string info = fmt::format(fmt, args...);
	//	putLog(SPDLOG_LEVEL_WARN, m_component.c_str(), info.c_str());
	//}
}