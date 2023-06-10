/*****************************************************************//**
 * \file   MonitorManager.h
 * \brief  Monitor the system metrics for the specified view
 * 
 * \author Zhou Guangyong
 * \date   April 2023
 *********************************************************************/
#pragma once
#ifndef _DATAKIT_SDK_MONITOR_MANAGER_H_
#define _DATAKIT_SDK_MONITOR_MANAGER_H_

#include "Singleton.h"
#include "RumDataEntity.h"
#include "InternalEnums.h"

#include <map>
#include <mutex>
#include <thread>
#include <atomic>

namespace com::ft::sdk::internal
{
	struct MonitorMetrics
	{
		int count = 0;

		double avgUsage = 0.0f;
		double minUsage = 0.0f;
		double maxUsage = 0.0f;
	};
	struct MonitoredViewContainer
	{
		MonitorMetrics cpuMetrics;
		MonitorMetrics memoryMetrics;
		MonitorMetrics fpsMetrics;
		MonitorMetrics batteryMetrics;
		DetectFrequency detectFreq;

		RUMView* pView;

		MonitoredViewContainer(RUMView* pView);

		void start();
		void stop();
		void collectMetrics();

	private:
		std::mutex m_mtx;
		std::thread* m_pThread;
		std::atomic<bool> m_stop;
	};

	class MonitorManager : public Singleton<MonitorManager>
	{
	public:
		void init();

		bool isErrorMonitorType(ErrorMonitorType errorMonitorType);
		bool isDeviceMetricsMonitorType(DeviceMetricsMonitorType deviceMetricsMonitorType);

		void addMonitor(RUMView* view);
		void removeMonitor(const std::string viewId);

	private:
		MonitorManager() {};

	private:
		ErrorMonitorType errorMonitorType = ErrorMonitorType::NO_SET;
		DeviceMetricsMonitorType deviceMetricsMonitorType = DeviceMetricsMonitorType::NO_SET;

		std::map<std::string, MonitoredViewContainer*> m_mapViewContainer;

		ENABLE_SINGLETON();
	};

}

#endif // !_DATAKIT_SDK_MONITOR_MANAGER_H_



