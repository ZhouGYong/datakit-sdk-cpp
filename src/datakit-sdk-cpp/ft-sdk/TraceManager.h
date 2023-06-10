/*****************************************************************//**
 * \file   TraceManager.h
 * \brief  Generate trace header for the specified resource
 * 
 * \author Zhou Guangyong
 * \date   December 2022
 *********************************************************************/
#pragma once
#ifndef _DATAKIT_SDK_TRACE_MANAGER_H_
#define _DATAKIT_SDK_TRACE_MANAGER_H_

#include "AbstractManager.h"
#include "Singleton.h"
#include <map>
#include <string>
#include "Include/FTSDKConfig.h"
#include "InternalStructs.h"
#include "TraceHeader.h"
#include "FTSDKConfigManager.h"

namespace com::ft::sdk::internal
{
    struct TraceHeaderContainer 
    {
        bool addResourced = false;
        bool resourceStop = false;
        std::int64_t startTime = 0;

        const int TIME_OUT = 60000;//暂不考虑长链接情况

        std::shared_ptr<TraceHeader> header;

        TraceHeaderContainer(std::shared_ptr<TraceHeader> header)
        {
            this->header = header;
            addResourced = !FTSDKConfigManager::getInstance().getRUMConfig().isRumEnable();
            resourceStop = addResourced;
            startTime = internal::utils::getCurrentNanoTime();
        }

        /**
         * 未避免错误调用造成内存溢出
         *
         * @return
         */
        bool isTimeOut() {
            return internal::utils::getCurrentNanoTime() - startTime > TIME_OUT;
        }
    };

	class TraceManager :
		public AbstractManager, public Singleton<TraceManager>
	{
	public:
		void initialize(FTTraceConfig& config);
		void deinitialize();

		std::map<std::string, std::string> getTraceHeader(const std::string& urlStr);
		std::map<std::string, std::string> getTraceHeader(const std::string& key, const std::string& urlStr);

        std::shared_ptr<TraceHeader> getHeader(std::string resourceId);
        void removeByAddResource(std::string key);
        void removeByStopResource(std::string key);
        void checkToRemove(std::string key, std::shared_ptr<TraceHeaderContainer> container);

    private:
        TraceManager() : AbstractManager(__func__) {}

	private:
        std::map<std::string, std::shared_ptr<TraceHeaderContainer>> m_mapTraceHeaderContainer;
		std::string m_traceID = "";
		std::string m_spanID = "";

		//是否可以采样
		bool m_enableTrace = false;

		FTTraceConfig m_traceConfig;

        friend class Singleton;
	};

}

#endif //_DATAKIT_SDK_TRACE_MANAGER_H_