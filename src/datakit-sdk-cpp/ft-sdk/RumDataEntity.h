/*****************************************************************//**
 * \file   RumDataEntity.h
 * \brief  All classes for RUM entities
 * 
 * \author Zhou Guangyong
 * \date   November 2022
 *********************************************************************/
#pragma once
#ifndef _DATAKIT_SDK_RUM_DATA_ENTITY_H_
#define _DATAKIT_SDK_RUM_DATA_ENTITY_H_

#include <string>
#include <vector>
#include "Utils.h"

#define ROOT_RUM_ID "rum_root_id"
#define ROOT_RUM_VIEW_ID "root"

namespace com::ft::sdk::internal
{
	class RUMItem;
	class RUMView;
	class RUMSession;
	class RUMAction;
	class RUMResource;
	class RUMError;
	class RUMLongTask;

	typedef std::vector<RUMItem *> RUMItemList;

	class RUMItem
	{
	public:
		RUMItem(RUMItem* pParentItem);

		const std::string& getId()
		{
			return m_itemId;
		}

		const std::string& getName()
		{
			return m_itemName;
		}

		const std::string& getParentId()
		{
			return m_parentId;
		}

		RUMItem* getParent()
		{
			return m_pParentItem;
		}

		virtual std::string toString();

	protected:
		RUMItem();

	protected:
		std::string m_itemName;
		std::string m_itemId;
		std::string m_parentId;

		RUMItem* m_pParentItem;
	};

	class RUMItemContainer : public RUMItem
	{
	public:
		RUMItemContainer(RUMItem* pParentItem);
		RUMItemList& getItems()
		{
			return m_rumItems;
		}
		std::string toString();
		void reset();
	protected:
		RUMItemContainer();
		~RUMItemContainer();

		void addItem(RUMItem* item);

		RUMItemList m_rumItems;
	};

	class RUMSession : public RUMItem
	{
	public:
		RUMSession(RUMItem* pParentItem);

		void refreshId();
	};

	class RUMApplication : public RUMItemContainer
	{
	public:
		static RUMApplication& getInstance()
		{
			static RUMApplication instance;
			return instance;
		}

		//RUMApplication(const std::string& parentId);
		RUMSession& createSession();
		RUMView& addView(std::string viewName);
	
		void checkViewCapacity();

		const std::string& getSessionId();
		std::vector<RUMView*> getViewList();
		void refreshSessionId();
	private:
		RUMApplication();
		~RUMApplication();
		RUMSession m_rumSession;
	};

	/// <summary>
	/// TODO: revise to a better name
	/// </summary>
	class RUMIndicatorHost
	{
	public:
		RUMIndicatorHost();
		~RUMIndicatorHost();

		virtual RUMResource& addResource(RUMItem* pParentItem, const std::string& name);
		virtual RUMError& addError(RUMItem* pParentItem, const std::string& name);
		virtual RUMLongTask& addLongTask(RUMItem* pParentItem, const std::string& name);

		std::string toString();

		RUMItemList& getResourceList()
		{
			return m_vtResource;
		}
		RUMItemList& getErrorList()
		{
			return m_vtError;
		}
		RUMItemList& getLongTaskList()
		{
			return m_vtLongTask;
		}

		int getLongTaskCount()
		{
			return m_vtLongTask.size();
		}

		int getResourceCount()
		{
			return m_vtResource.size();
		}

		int getErrorCount()
		{
			return m_vtError.size();
		}


		bool isClose()
		{
			return m_isClose;
		}

		virtual void close()
		{
			m_isClose = true;
			m_endTime = utils::getCurrentNanoTime();
		}

		bool isDirty()
		{
			return m_isDirty;
		}

		void setDirty(bool dirty)
		{
			m_isDirty = dirty;
		}

		bool isSynced()
		{
			return m_isSynced;
		}

		void setSynced(bool sync)
		{
			m_isSynced = sync;
		}

		void setStartTime(std::int64_t startTime)
		{
			m_startTime = startTime;
		}

		std::int64_t getStartTime()
		{
			return m_startTime;
		}

		void setEndTime(std::int64_t endTime)
		{
			m_endTime = endTime;
		}

		std::int64_t getEndTime()
		{
			return m_endTime;
		}

		std::int64_t getTimeSpent()
		{
			return m_endTime - m_startTime;
		}
	private:
		RUMItemList m_vtResource;
		RUMItemList m_vtError;
		RUMItemList m_vtLongTask;

		bool m_isClose = false;
		bool m_isDirty = true;
		bool m_isSynced = false;
		std::int64_t m_startTime = 0;
		std::int64_t m_endTime = 0;
	};

	class RUMView : public RUMItemContainer, public RUMIndicatorHost
	{
	public:
		RUMView(RUMItem* pParentItem, const std::string& viewName);
		void close();
		RUMAction& addAction(const std::string& actionName, const std::string& actionType);
		RUMResource& addResource(RUMItem* pParentItem, const std::string& name);
		void setPrevRUMView(RUMView* pView)
		{
			m_pPrevRumView = pView;
		}
		RUMView* getPrevRUMView()
		{
			return m_pPrevRumView;
		}
		
		std::string toString();

		int getLongTaskCount();
		int getErrorCount();
		int getResourceCount();

		std::vector<RUMAction*> getActionList();

		double getFpsAvg()
		{
			return m_fpsAvg;
		}

		double getFpsMini()
		{
			return m_fpsMini;
		}

		double getCpuTickCountPerSecond()
		{
			return m_cpuTickCountPerSecond;
		}

		long getCpuTickCount()
		{
			return m_cpuTickCount;
		}

		long getMemoryMax()
		{
			return m_memoryMax;
		}

		long getMemoryAvg()
		{
			return m_memoryAvg;
		}

		int getBatteryCurrentAvg()
		{
			return m_batteryCurrentAvg;
		}

		int getBatteryCurrentMax()
		{
			return m_batteryCurrentMax;
		}
	private:
		RUMView* m_pPrevRumView = nullptr;

		double m_fpsMini;
		double m_fpsAvg;

		double m_cpuTickCountPerSecond = -1;
		long m_cpuTickCount = -1;

		long m_memoryAvg;
		long m_memoryMax;

		int m_batteryCurrentAvg;
		int m_batteryCurrentMax;
	};

	class RUMAction : public RUMItemContainer, public RUMIndicatorHost
	{
	public:
		RUMAction(RUMItem* pParentItem, const std::string& actionName, const std::string& actionType);
		RUMResource& addResource(RUMItem* pParentItem, const std::string& name);

		std::string& getActionType()
		{
			return m_actionType;
		}
		bool isNeedWaitAction()
		{
			return m_isNeedWaitAction;
		}
		void setNeedWaitAction(bool isNeedWaitAction)
		{
			m_isNeedWaitAction = isNeedWaitAction;
		}

		std::string toString();
	private:
		std::string m_actionType;
		bool m_isNeedWaitAction = false;
	};

	class RUMResource : public RUMItem
	{
	public:
		RUMResource(RUMItem* pParentItem, const std::string& name);

	public:
		std::string url = "";
		std::string urlHost = "";
		std::string urlPath = "";
		std::string resourceType = "";
		std::string requestHeader = "";
		std::string responseHeader = "";
		std::string responseConnection = "";
		std::string responseContentType = "";
		std::string responseContentEncoding = "";
		std::string resourceMethod = "";
		std::string resourceUrlQuery = "";
		std::string traceId = "";
		std::string spanId = "";
		std::string errorStack = "";

		int resourceStatus = -1;

		std::int64_t resourceSize = -1;
		std::int64_t resourceLoad = -1;
		std::int64_t resourceDNS = -1;
		std::int64_t resourceTCP = -1;
		std::int64_t resourceSSL = -1;
		std::int64_t resourceTTFB = -1;
		std::int64_t resourceTrans = -1;
		std::int64_t resourceFirstByte = -1;

		std::int64_t startTime = utils::getCurrentNanoTime();
		std::int64_t endTime = -1;

		std::string sessionId;
		std::string viewId;
		std::string viewName;
		std::string viewReferrer;
		std::string actionId;
		std::string actionName;

		bool netStateSet = false;
		bool contentSet = false;
	};

	class RUMError : public RUMItem
	{
	public:
		RUMError(RUMItem* pParentItem, const std::string& name);
	};

	class RUMLongTask : public RUMItem
	{
	public:
		RUMLongTask(RUMItem* pParentItem, const std::string& name);
	};
}

#endif // !_DATAKIT_SDK_RUM_DATA_ENTITY_H_



