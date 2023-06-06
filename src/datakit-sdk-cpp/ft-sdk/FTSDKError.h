/*****************************************************************//**
 * \file   FTSDKError.h
 * \brief  Handle Internal Exception
 * 
 * \author Zhou Guangyong
 * \date   June 2023
 *********************************************************************/
#pragma once
#ifndef _DATAKIT_SDK_ERROR_H_
#define _DATAKIT_SDK_ERROR_H_

#include "LoggerManager.h"

#define FTSDK_THROW_EXCEPTION(message)	    \
    throw FTSDKException(__FILE__, __LINE__, __func__, message);

#define CHECK_SDK_CONDITION(code, message)	    \
    if (!(code))		\
			FTSDK_THROW_EXCEPTION(#message)

#define CHECK_SDK_INITED()	    \
    if (!internal::FTSDKConfigManager::getInstance().isSDKInited())		\
			FTSDK_THROW_EXCEPTION("SDK is not initialized!")

#define FTSDK_LOG_EXCEPTION(code)                                                 \
    try {           \
        (code);           \
    }       \
    catch (const std::exception &e) {       \
        com::ft::sdk::internal::process_error(__FILE__, __LINE__, #code, e.what(), true, false);        \
    }

#define FTSDK_CHECK_RETURN(type, code)                                                 \
    try {           \
        return (code);           \
    }       \
    catch (const std::exception &e) {       \
        internal::process_error(__FILE__, __LINE__, #code, e.what(), true, false);        \
        return type();      \
    }
                       
#define _LOG_DBG(logMsg)   \
    FTSDK_LOG_EXCEPTION(com::ft::sdk::internal::LoggerManager::getInstance().logDebug(logMsg))

#define _LOG_WRN(logMsg)   \
    FTSDK_LOG_EXCEPTION(com::ft::sdk::internal::LoggerManager::getInstance().logWarn(logMsg))

#define _LOG_INFO(logMsg)   \
    FTSDK_LOG_EXCEPTION(com::ft::sdk::internal::LoggerManager::getInstance().logInfo(logMsg))

#define _LOG_TRC(logMsg)   \
    FTSDK_LOG_EXCEPTION(com::ft::sdk::internal::LoggerManager::getInstance().logTrace(logMsg))

#define _LOG_ERR(logMsg)   \
    FTSDK_LOG_EXCEPTION(com::ft::sdk::internal::LoggerManager::getInstance().logError(logMsg))

#define BEGIN_THREAD()  try {       \

#define END_THREAD()    		}   \
        catch (const std::exception& ex)        \
        {       \
            std::string errStr = "Thread exception: ";      \
            errStr.append(ex.what());       \
            _LOG_ERR(errStr);       \
        }

namespace com::ft::sdk::internal
{

    inline void process_error(const std::string& file, int line, const std::string& code, 
        const std::string& message, bool enable_log, bool enable_raise) 
    {
        if (enable_log || enable_raise)
        {
            std::string err = "INTERFACE EXCEPTION: " + file + "(line:" + std::to_string(line) + "):" + \
                code + ":" + message;
            _LOG_ERR(err);
        }
        if (enable_raise)
        {
            std::terminate();
        }
    }

    template<typename T>
    inline T process_error_return(const std::string& file, int line, const std::string& code,
        const std::string& message, T ret, bool enable_log, bool enable_raise)
    {
        if (enable_log || enable_raise)
        {
            std::string err = "INTERFACE EXCEPTION: " + file + "(line:" + std::to_string(line) + "):" + \
                code + ":" + message;
            _LOG_ERR(err);
        }
        if (enable_raise)
        {
            std::terminate();
        }
    }
}

#endif // !_DATAKIT_SDK_ERROR_H_
