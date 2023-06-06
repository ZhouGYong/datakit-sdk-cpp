#include "pch.h"
#include "MonitorManager.h"
#include "FTSDKConfigManager.h"
#include "Platform.h"
#include "FTSDKError.h"

#include <thread>
#include <algorithm>

namespace com::ft::sdk::internal
{
	MonitoredViewContainer::MonitoredViewContainer(RUMView* pView)
	{
		m_stop = false;
		this->pView = pView;
#ifdef _SUPPORT_ADVANCE_FEATURE_
		detectFreq = FTSDKConfigManager::getInstance().getRUMConfig().getDeviceMetricsDetectFrequency();
#endif // _SUPPORT_ADVANCE_FEATURE_
	}

	void MonitoredViewContainer::start()
	{
		m_pThread = new std::thread(&MonitoredViewContainer::collectMetrics, this);
	}

	void MonitoredViewContainer::stop()
	{
		m_stop = true;
	}

	void MonitoredViewContainer::collectMetrics()
	{
		BEGIN_THREAD();

		while (!m_stop)
		{
			// collect cpu usage
			float cpuUsage = platform::GetCPULoad();
			if (cpuMetrics.count == 0) {
				cpuMetrics.minUsage = cpuUsage;
			}
			else {
				cpuMetrics.maxUsage = cpuUsage;
			}
			cpuMetrics.count++;

			// collect memory usage
			double memUsage = platform::getMemoryLoad();
			int count = memoryMetrics.count + 1;
			memoryMetrics.avgUsage = (memUsage + (memoryMetrics.count * memoryMetrics.avgUsage)) / count;
			memoryMetrics.maxUsage = (std::max)(memUsage, memoryMetrics.maxUsage);
			memoryMetrics.minUsage = (std::min)(memUsage, memoryMetrics.minUsage);
			memoryMetrics.count = count;

			std::this_thread::sleep_for(std::chrono::milliseconds((int)detectFreq));
		}

		END_THREAD();
	}

	void MonitorManager::init()
	{
#ifdef _SUPPORT_ADVANCE_FEATURE_
		errorMonitorType = FTSDKConfigManager::getInstance().getRUMConfig().getExtraMonitorTypeWithError();
		deviceMetricsMonitorType = FTSDKConfigManager::getInstance().getRUMConfig().getDeviceMetricsMonitorType();
#endif // _SUPPORT_ADVANCE_FEATURE_
	}

	bool MonitorManager::isErrorMonitorType(ErrorMonitorType errorMonitorType)
	{
		//判断某一种监控项是否开启
		return ((int)this->errorMonitorType | (int)errorMonitorType) == (int)this->errorMonitorType;
	}

	bool MonitorManager::isDeviceMetricsMonitorType(DeviceMetricsMonitorType deviceMetricsMonitorType)
	{
		return ((int)this->deviceMetricsMonitorType | (int)deviceMetricsMonitorType) == (int)this->deviceMetricsMonitorType;

	}

	void MonitorManager::addMonitor(RUMView* view)
	{
		if (deviceMetricsMonitorType == DeviceMetricsMonitorType::NO_SET)
		{
			return;
		}

		m_mapViewContainer[view->getId()] = new MonitoredViewContainer(view);

		// start monitor
		m_mapViewContainer[view->getId()]->start();
	}

	void MonitorManager::removeMonitor(const std::string viewId)
	{
		if (deviceMetricsMonitorType == DeviceMetricsMonitorType::NO_SET)
		{
			return;
		}

		if (m_mapViewContainer.find(viewId) != m_mapViewContainer.end())
		{
			MonitoredViewContainer* pContainer = m_mapViewContainer[viewId];
			// stop monitor
			pContainer->stop();

			m_mapViewContainer.erase(viewId);
		}
	}
}