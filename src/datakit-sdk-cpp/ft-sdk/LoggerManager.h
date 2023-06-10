/*****************************************************************//**
 * \file   LoggerManager.h
 * \brief  General Logger Encapsulation for SDK
 * 
 * \author Zhou Guangyong
 * \date   October 2022
 *********************************************************************/
#pragma once
#ifndef _DATAKIT_SDK_LOG_MANAGER_H_
#define _DATAKIT_SDK_LOG_MANAGER_H_

#include <string>
#include <fmt/format.h>
#include <spdlog/spdlog.h>
#include "AbstractManager.h"
#include "Singleton.h"

namespace com::ft::sdk::internal
{
    class LoggerManager :
        public AbstractManager, public Singleton<LoggerManager>
    {
    public:
        void init();
        template<typename... Args> 
        void logDebug(const std::string& logContent, Args const& ... args)
        {
            //std::cout << "DEBUG: " << info << std::endl;
            std::string info = logContent;
            if (sizeof...(args) > 0)
            {
                info = fmt::format(logContent.c_str(), args...);
            }
            putLog(SPDLOG_LEVEL_DEBUG, m_component.c_str(), info.c_str());
        }

        template<typename... Args>
        void logInfo(const std::string& logContent, Args const& ... args)
        {
            //std::cout << "INFO: " << info << std::endl;
            std::string info = logContent;
            if (sizeof...(args) > 0)
            {
                info = fmt::format(logContent.c_str(), args...);
            }
            putLog(SPDLOG_LEVEL_INFO, m_component.c_str(), info.c_str());
        }

        template<typename... Args>
        void logWarn(const std::string& logContent, Args const& ... args)
        {
            //std::cout << "WARN: " << info << std::endl;
            std::string info = logContent;
            if (sizeof...(args) > 0)
            {
                info = fmt::format(logContent.c_str(), args...);
            }
            putLog(SPDLOG_LEVEL_WARN, m_component.c_str(), info.c_str());
        }

        template<typename... Args>
        void logError(const std::string& logContent, Args const& ... args)
        {
            //std::cout << "ERROR: " << info << std::endl;
            std::string info = logContent;
            if (sizeof...(args) > 0)
            {
                info = fmt::format(logContent.c_str(), args...);
            }
            putLog(SPDLOG_LEVEL_ERROR, m_component.c_str(), info.c_str());
        }

        template<typename... Args>
        void logTrace(const std::string& logContent, Args const& ... args)
        {
            //std::cout << "INFO: " << info << std::endl;
            std::string info = logContent;
            if (sizeof...(args) > 0)
            {
                info = fmt::format(logContent.c_str(), args...);
            }
            putLog(SPDLOG_LEVEL_TRACE, m_component.c_str(), info.c_str());
        }

        void enableLog(bool enabled)
        {
            m_enableLog = enabled;
        }
    private:
        LoggerManager() : AbstractManager(__func__) {}
        void setupLogger(std::string appLogFileName);
        int putLog(int severity, char const* component, char const* message);

    private:
        const std::string LOGGER_NAME = "DataKit";

        std::string m_component = "";
        bool m_enableLog = false;

        friend class Singleton;
    };

}
#endif // !_DATAKIT_SDK_LOG_MANAGER_H_