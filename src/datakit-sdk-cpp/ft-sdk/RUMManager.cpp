#include "pch.h"
#include "RUMManager.h"
#include "FTSDKConstants.h"
#include "LoggerManager.h"
#include "Utils.h"
#include "TraceManager.h"
#include "DataSyncManager.h"
#include "FTSDKConfigManager.h"
#include <iterator>
#include <locale>
#include <regex>
#include "RumDataEntity.h"
#include "CacheDBManager.h"
#include "Constants.h"
#include "TraceHeader.h"
#include "MonitorManager.h"
#include "InternalStructs.h"
#include "FTSDKError.h"

namespace com::ft::sdk::internal
{
    const std::int64_t MAX_RESTING_TIME = 900000000000;
    const std::int64_t SESSION_EXPIRE_TIME = 14400000000000;
    const int FILTER_CAPACITY = 5;
    const int ACTION_TIMEOUT = 5;

    void RUMManager::clear()
    {
        RUMApplication::getInstance().reset();
    }

    RUMManager::RUMManager() : AbstractManager(__func__)
    {
        m_lastSessionTime = utils::getCurrentNanoTime();
        m_lastActionTime = m_lastSessionTime;
    }

    void RUMManager::init()
    {
        clear();
        checkSessionKeep();
    }

    void RUMManager::addLongTask(std::string log, std::int64_t duration)
    {
        CHECK_SDK_CONDITION((m_pActiveView != nullptr), "View is required");

        LoggerManager::getInstance().logDebug("add long task: " + log);

        if (m_pActiveAction == nullptr || m_pActiveAction->isClose())
        {
            RUMLongTask& res = m_pActiveView->addLongTask(m_pActiveView, log);
        }
        else
        {
            RUMLongTask& res = m_pActiveAction->addLongTask(m_pActiveAction, log);
        }

        try
        {
            std::int64_t time = internal::utils::getCurrentNanoTime();
            auto tags = internal::FTSDKConfigManager::getInstance().getRUMPublicDynamicTags();
            internal::RUMManager::getInstance().attachRUMRelative(tags, true);

            FieldMap fields;
            fields[constants::KEY_RUM_LONG_TASK_DURATION] = duration;
            fields[constants::KEY_RUM_LONG_TASK_STACK] = log;

            CacheDBManager::getInstance().insertRUMItem(constants::FT_MEASUREMENT_RUM_LONG_TASK, tags, fields);
        }
        catch (std::exception e)
        {
            internal::LoggerManager::getInstance().logError("failed to add long task: ");
        }
    }

    void RUMManager::addError(std::string log, std::string message, RUMErrorType errorType, AppState state)
    {
        CHECK_SDK_CONDITION((m_pActiveView != nullptr), "View is required");

        LoggerManager::getInstance().logDebug("add error: " + log + " , " + message);

        if (m_pActiveAction == nullptr || m_pActiveAction->isClose())
        {
            RUMError& res = m_pActiveView->addError(m_pActiveView, log);
        }
        else
        {
            RUMError& res = m_pActiveAction->addError(m_pActiveAction, log);
        }

        try {
            auto tags = internal::FTSDKConfigManager::getInstance().getRUMPublicDynamicTags();
            internal::RUMManager::getInstance().attachRUMRelative(tags, true);

            tags[constants::KEY_RUM_ERROR_TYPE] = utils::convertToLowerCase(EnumToString(errorType));
            tags[constants::KEY_RUM_ERROR_SOURCE] = utils::convertToLowerCase(EnumToString(ErrorSource::LOGGER));
            tags[constants::KEY_RUM_ERROR_SITUATION] = EnumToString(state);

            FieldMap fields;
            fields[constants::KEY_RUM_ERROR_MESSAGE] = message;
            fields[constants::KEY_RUM_ERROR_STACK] = log;

            tags[constants::KEY_DEVICE_CARRIER] = "NONE";//DeviceUtils.getCarrier(FTApplication.getApplication());
            std::locale curLoc;
            tags[constants::KEY_DEVICE_LOCALE] = curLoc.name();

            /* // not enable monitor the environment params so far
            if (FTMonitorManager.get().isErrorMonitorType(ErrorMonitorType.MEMORY)) {
                double[] memory = DeviceUtils.getRamData(FTApplication.getApplication());
                tags[constants::KEY_MEMORY_TOTAL] = memory[0] + "GB";
                msg.fields[constants::KEY_MEMORY_USE] = memory[1];
            }

            if (FTMonitorManager.get().isErrorMonitorType(ErrorMonitorType.CPU)) {
                msg.fields[constants::KEY_CPU_USE] = DeviceUtils.getCpuUsage();
            }
            if (FTMonitorManager.get().isErrorMonitorType(ErrorMonitorType.BATTERY)) {
                msg.fields[constants::KEY_BATTERY_USE] = (float)BatteryUtils.getBatteryInfo(FTApplication.getApplication()).getBr();
            }
            */

            //std::int64_t dateline = internal::utils::getCurrentNanoTime();
            //FTTrackInner.getInstance().rum(dateline, constants::FT_MEASUREMENT_RUM_ERROR, tags, fields);
            CacheDBManager::getInstance().insertRUMItem(constants::FT_MEASUREMENT_RUM_ERROR, tags, fields);
            //increaseError(tags);

        }
        catch (std::exception e) 
        {
            internal::LoggerManager::getInstance().logError("failed to add error: ");
        }
    }

    void RUMManager::addError(std::string log, RUMMap& tags, FieldMap& fields)
    {
        CHECK_SDK_CONDITION((m_pActiveView != nullptr), "View is required");

        LoggerManager::getInstance().logDebug("add internal error: " + log);

        if (m_pActiveAction == nullptr || m_pActiveAction->isClose())
        {
            RUMError& res = m_pActiveView->addError(m_pActiveView, log);
        }
        else
        {
            RUMError& res = m_pActiveAction->addError(m_pActiveAction, log);
        }

        CacheDBManager::getInstance().insertRUMItem(constants::FT_MEASUREMENT_RUM_ERROR, tags, fields);

    }

    void RUMManager::addAction(std::string actionName, std::string actionType, std::int64_t duration, std::int64_t startTime)
    {
        CHECK_SDK_CONDITION((m_pActiveView != nullptr), "View is required");
        LoggerManager::getInstance().logDebug("add action:" + actionName + " , " + actionType);

        checkSessionRefresh();
        auto action = &(m_pActiveView->addAction(actionName, actionType));
        action->close();
        action->setStartTime(startTime);
        action->setEndTime(startTime + duration);
        this->m_lastActionTime = action->getStartTime();

        flushRUMData();
    }

    void RUMManager::startAction(std::string actionName, std::string actionType)
    {
        CHECK_SDK_CONDITION((m_pActiveView != nullptr), "View is required");
        LoggerManager::getInstance().logDebug("start action:" + actionName + " , " + actionType);

        std::string viewId = m_pActiveView != nullptr ? m_pActiveView->getId() : "";
        std::string viewName = m_pActiveView != nullptr ? m_pActiveView->getName() : "";
        std::string viewReferrer = getViewReferrerName(m_pActiveView);

        checkSessionRefresh();
        checkActionClose();
        if (m_pActiveAction == nullptr || m_pActiveAction->isClose()) {
            initAction(actionName, actionType);

            new std::thread(&RUMManager::checkForActionTimeout, this);
        }
    }

    void RUMManager::stopAction()
    {
        LoggerManager::getInstance().logDebug("stop action");

        if (m_pActiveAction->isNeedWaitAction()) {
            m_pActiveAction->close();
        }

        if (m_pActiveAction != nullptr)
        {
            closeAction(*m_pActiveAction, false);
        }
    }

    RUMAction& RUMManager::initAction(std::string actionName, std::string actionType)
    {
        m_pActiveAction = &(m_pActiveView->addAction(actionName, actionType));
        this->m_lastActionTime = m_pActiveAction->getStartTime();

        return *m_pActiveAction;
    }

    void RUMManager::addResource(std::string resourceId, ResourceParams params, NetStatus netStatusBean)
    {
        CHECK_SDK_CONDITION((m_pActiveView != nullptr), "View is required");
        setTransformContent(resourceId, params);
        setNetState(resourceId, netStatusBean);
    }

    void RUMManager::setNetState(std::string resourceId, NetStatus netStatusBean)
    {
        auto resIT = m_mapResource.find(resourceId);

        if (resIT == m_mapResource.end())
        {
            return ;
        }

        RUMResource* pRes = m_mapResource[resourceId];
        pRes->resourceDNS = NetStatusHelper::getDNSTime(netStatusBean);
        pRes->resourceSSL = NetStatusHelper::getSSLTime(netStatusBean);
        pRes->resourceTCP = NetStatusHelper::getTcpTime(netStatusBean);

        pRes->resourceTrans = NetStatusHelper::getResponseTime(netStatusBean);
        pRes->resourceTTFB = NetStatusHelper::getTTFB(netStatusBean);
        long resourceLoad = NetStatusHelper::getHoleRequestTime(netStatusBean);
        pRes->resourceLoad = resourceLoad > 0 ? resourceLoad : pRes->endTime - pRes->startTime;
        pRes->resourceFirstByte = NetStatusHelper::getFirstByteTime(netStatusBean);
        pRes->netStateSet = true;

        checkToAddResource(resourceId, pRes);
    }

    void RUMManager::setTransformContent(std::string resourceId, ResourceParams params)
    {
        auto pHeader = TraceManager::getInstance().getHeader(resourceId);
        std::string spanId = "";
        std::string traceId = "";
        if (pHeader != nullptr) {
            spanId = pHeader->getSpanID();
            traceId = pHeader->getTraceID();
        }

        RUMResource* pRes = m_mapResource[resourceId];

        if (pRes == nullptr || params.resourceStatus < HTTP_STATUS::HTTP_OK) 
        {
            return;
        }
        try 
        {
            PropagationUrl url = PropagationUrl::parse(params.url);
            pRes->url = params.url;
            pRes->urlHost = url.getHost();
            pRes->urlPath = url.getPath();
            pRes->resourceUrlQuery = url.getQuery();
        }
        catch (std::exception ex) 
        {
            internal::LoggerManager::getInstance().logError("Failed to parse url: " + params.url);
        }

        pRes->requestHeader = params.requestHeader;
        pRes->responseHeader = params.responseHeader;
        int responseHeaderSize = pRes->responseHeader.size(); // TODO: ASCII or UNICODE?
        pRes->responseContentType = params.responseContentType;
        pRes->responseConnection = params.responseConnection;
        pRes->resourceMethod = params.resourceMethod;
        pRes->responseContentEncoding = params.responseContentEncoding;
        pRes->resourceType = "network"; // pRes->responseContentType;
        pRes->resourceStatus = params.resourceStatus;
        if (pRes->resourceStatus >= HTTP_STATUS::HTTP_BAD_REQUEST) 
        {
            pRes->errorStack = params.responseBody;
        }

        pRes->resourceSize = params.responseBody.size();
        pRes->resourceSize += responseHeaderSize;
        if (FTSDKConfigManager::getInstance().getTraceConfig().getEnableLinkRUMData()) 
        {
            pRes->traceId = traceId;
            pRes->spanId = spanId;
        }
        pRes->contentSet = true;
        checkToAddResource(resourceId, pRes);
    }

    void RUMManager::checkToAddResource(std::string key, RUMResource* pRes)
    {
        if (pRes->contentSet && pRes->netStateSet)
        {
            putRUMResourcePerformance(key);
        }
    }
    void RUMManager::putRUMResourcePerformance(std::string resourceId)
    {
        auto resIT = m_mapResource.find(resourceId);

        if (resIT == m_mapResource.end())
        {
            return;
        }

        RUMResource* pRes = m_mapResource[resourceId];

        if (pRes->resourceStatus < HTTP_STATUS::HTTP_OK)
        {
            //EventConsumerThreadPool.get().execute(() -> {
            m_mapResource.erase(resourceId);
            TraceManager::getInstance().removeByAddResource(resourceId);
            //});

            return;
        }
        long time = internal::utils::getCurrentNanoTime();
        std::string actionId = pRes->actionId;
        std::string viewId = pRes->viewId;
        std::string actionName = pRes->actionName;
        std::string viewName = pRes->viewName;
        std::string viewReferrer = pRes->viewReferrer;
        std::string sessionId = pRes->sessionId;

        try 
        {
            auto tags = FTSDKConfigManager::getInstance().getRUMPublicDynamicTags();

            tags[constants::KEY_RUM_ACTION_ID] = actionId;
            tags[constants::KEY_RUM_ACTION_NAME] = actionName;
            tags[constants::KEY_RUM_VIEW_ID] = viewId;
            tags[constants::KEY_RUM_VIEW_NAME] = viewName;
            tags[constants::KEY_RUM_VIEW_REFERRER] = viewReferrer;
            tags[constants::KEY_RUM_SESSION_ID] = sessionId;
            tags[constants::KEY_RUM_RESOURCE_URL_HOST] = pRes->urlHost;

            if (pRes->resourceType != "") 
            {
                tags[constants::KEY_RUM_RESOURCE_TYPE] = pRes->resourceType;
            }
            tags[constants::KEY_RUM_RESPONSE_CONNECTION] = pRes->responseConnection;
            tags[constants::KEY_RUM_RESPONSE_CONTENT_TYPE] = pRes->responseContentType;
            tags[constants::KEY_RUM_RESPONSE_CONTENT_ENCODING] = pRes->responseContentEncoding;
            tags[constants::KEY_RUM_RESOURCE_METHOD] = pRes->resourceMethod;
            tags[constants::KEY_RUM_RESOURCE_TRACE_ID] = pRes->traceId;
            tags[constants::KEY_RUM_RESOURCE_SPAN_ID] = pRes->spanId;

            int resourceStatus = pRes->resourceStatus;
            std::string resourceStatusGroup = "";
            if (resourceStatus > 0) 
            {
                tags[constants::KEY_RUM_RESOURCE_STATUS] = std::to_string(resourceStatus);
                long statusGroupPrefix = pRes->resourceStatus / 100;
                resourceStatusGroup = std::to_string(statusGroupPrefix) + "xx";
                tags[constants::KEY_RUM_RESOURCE_STATUS_GROUP] = resourceStatusGroup;
            }

            FieldMap fields;

            if (pRes->resourceSize > 0) 
            {
                fields[constants::KEY_RUM_RESOURCE_SIZE] = pRes->resourceSize;
            }
            if (pRes->resourceLoad > 0) 
            {
                fields[constants::KEY_RUM_RESOURCE_DURATION] = pRes->resourceLoad;
            }

            if (pRes->resourceDNS > 0) 
            {
                fields[constants::KEY_RUM_RESOURCE_DNS] = pRes->resourceDNS;
            }
            if (pRes->resourceTCP > 0) 
            {
                fields[constants::KEY_RUM_RESOURCE_TCP] = pRes->resourceTCP;
            }
            if (pRes->resourceSSL > 0) 
            {
                fields[constants::KEY_RUM_RESOURCE_SSL] = pRes->resourceSSL;
            }
            if (pRes->resourceTTFB > 0) 
            {
                fields[constants::KEY_RUM_RESOURCE_TTFB] = pRes->resourceTTFB;
            }

            if (pRes->resourceTrans > 0) 
            {
                fields[constants::KEY_RUM_RESOURCE_TRANS] = pRes->resourceTrans;
            }

            if (pRes->resourceFirstByte > 0) 
            {
                fields[constants::KEY_RUM_RESOURCE_FIRST_BYTE] = pRes->resourceFirstByte;

            }
            std::string urlPath = pRes->urlPath;
            std::string urlPathGroup = "";

            if (urlPath != "") 
            {
                std::regex rgx("\\/([^\\/]*)\\d([^\\/]*)");
                urlPathGroup = std::regex_replace(urlPath, rgx, "/?");
                tags[constants::KEY_RUM_RESOURCE_URL_PATH] = urlPath;
                tags[constants::KEY_RUM_RESOURCE_URL_PATH_GROUP] = urlPathGroup;
            }


            tags[constants::KEY_RUM_RESOURCE_URL] = pRes->url;
            fields[constants::KEY_RUM_REQUEST_HEADER] = pRes->requestHeader;
            fields[constants::KEY_RUM_RESPONSE_HEADER] = pRes->responseHeader;

            CacheDBManager::getInstance().insertRUMItem(constants::FT_MEASUREMENT_RUM_RESOURCE, tags, fields);

            if (pRes->resourceStatus >= HTTP_STATUS::HTTP_BAD_REQUEST) 
            {
                auto errorTags = FTSDKConfigManager::getInstance().getRUMPublicDynamicTags();
                FieldMap errorField;
                errorTags[constants::KEY_RUM_ERROR_TYPE] = utils::convertToLowerCase(EnumToString(RUMErrorType::NETWORK_ERROR));
                errorTags[constants::KEY_RUM_ERROR_SOURCE] = utils::convertToLowerCase(EnumToString(ErrorSource::NETWORK));
                errorTags[constants::KEY_RUM_ERROR_SITUATION] = EnumToString(AppState::RUN);
                errorTags[constants::KEY_RUM_ACTION_ID] = actionId;
                errorTags[constants::KEY_RUM_ACTION_NAME] = actionName;
                errorTags[constants::KEY_RUM_VIEW_ID] = viewId;
                errorTags[constants::KEY_RUM_VIEW_NAME] = viewName;
                errorTags[constants::KEY_RUM_VIEW_REFERRER] = viewReferrer;
                errorTags[constants::KEY_RUM_SESSION_ID] = sessionId;

                if (resourceStatus > 0) 
                {
                    errorTags[constants::KEY_RUM_RESOURCE_STATUS] = std::to_string(resourceStatus);
                    errorTags[constants::KEY_RUM_RESOURCE_STATUS_GROUP] = resourceStatusGroup;
                }
                errorTags[constants::KEY_RUM_RESOURCE_URL] = pRes->url;
                errorTags[constants::KEY_RUM_RESOURCE_URL_HOST] = pRes->urlHost;
                errorTags[constants::KEY_RUM_RESOURCE_METHOD] = pRes->resourceMethod;

                if (urlPath != "") 
                {
                    errorTags[constants::KEY_RUM_RESOURCE_URL_PATH] = urlPath;
                    errorTags[constants::KEY_RUM_RESOURCE_URL_PATH_GROUP] = urlPathGroup;
                }
                std::string errorMsg = "[" + std::to_string(pRes->resourceStatus) + "][" + pRes->url + "]";

                errorField[constants::KEY_RUM_ERROR_MESSAGE] = errorMsg;
                errorField[constants::KEY_RUM_ERROR_STACK] = pRes->errorStack;

                addError("http error", errorTags, errorField);
                //CacheDBManager::getInstance().insertRUMItem(constants::FT_MEASUREMENT_RUM_ERROR, errorTags, errorField);
                //increaseError(tags);
            }
        }
        catch (std::exception ex) 
        {
            internal::LoggerManager::getInstance().logError("Failed to put resource performance measurement");
        }

        //EventConsumerThreadPool.get().execute(() -> {
        m_mapResource.erase(resourceId);
        TraceManager::getInstance().removeByAddResource(resourceId);
        //});
    }

    void RUMManager::startResource(std::string resourceId)
    {
        CHECK_SDK_CONDITION((m_pActiveView != nullptr), "View is required");
        LoggerManager::getInstance().logDebug("start resource: " + resourceId);

        if (m_pActiveAction == nullptr || m_pActiveAction->isClose())
        {
            RUMResource& res = m_pActiveView->addResource(m_pActiveView, resourceId);
            attachRUMRelativeForResource(res);
            m_mapResource[resourceId] = &res;
        }
        else
        {
            RUMResource& res = m_pActiveAction->addResource(m_pActiveAction, resourceId);
            attachRUMRelativeForResource(res);
            m_mapResource[resourceId] = &res;
        }
    }

    void RUMManager::stopResource(std::string resourceId)
    {
        if (m_mapResource.find(resourceId) != m_mapResource.end()) {
            RUMResource* res = m_mapResource[resourceId];
            res->endTime = utils::getCurrentNanoTime();
            //increaseResourceCount(viewId, actionId);

            //EventConsumerThreadPool.get().execute(() -> {
            //    FTDBManager.get().reduceViewPendingResource(viewId);
            //    FTDBManager.get().reduceActionPendingResource(actionId);
                TraceManager::getInstance().removeByStopResource(resourceId);
            //});
            //checkResourceStop(resourceId);
        }
    }

    void RUMManager::attachRUMRelativeForResource(RUMResource& res)
    {
        res.viewId = m_pActiveView->getId();
        res.viewName = m_pActiveView->getName();
        res.viewReferrer = getViewReferrerName(m_pActiveView);
        res.sessionId = getSessionId();
        if (m_pActiveAction == nullptr || m_pActiveAction->isClose()) 
        {
            res.actionId = m_pActiveAction->getId();
            res.actionName = m_pActiveAction->getName();
        }
    }

    void RUMManager::setActiveView(RUMView* pActiveView)
    {
        m_pActiveView = pActiveView;
    }

    void RUMManager::setActiveAction(RUMAction* pActiveAction)
    {
        m_pActiveAction = pActiveAction;
    }

	void RUMManager::attachRUMRelative(std::map<std::string, std::string>& tags, bool withAction)
	{
        try {
            tags[constants::KEY_RUM_VIEW_ID] = m_pActiveView == nullptr ? "" : m_pActiveView->getId();

            tags[constants::KEY_RUM_VIEW_NAME] = m_pActiveView == nullptr ? "" : m_pActiveView->getName();
            std::string viewReferrer = getViewReferrerName(m_pActiveView);
            tags[constants::KEY_RUM_VIEW_REFERRER] = viewReferrer;
            tags[constants::KEY_RUM_SESSION_ID] = getSessionId();
            if (withAction) 
            {
                if (m_pActiveAction != nullptr && !m_pActiveAction->isClose())
                {
                    tags[constants::KEY_RUM_ACTION_ID] = m_pActiveAction->getId();
                    tags[constants::KEY_RUM_ACTION_NAME] = m_pActiveAction->getName();
                }
            }
        }
        catch (std::exception e) 
        {
            LoggerManager::getInstance().logError("failed to attachRUMRelative.");
        }
	}

    void RUMManager::checkSessionRefresh()
    {
        std::int64_t now = utils::getCurrentNanoTime();
        bool longResting = now - m_lastActionTime > MAX_RESTING_TIME;
        bool longTimeSession = now - m_lastSessionTime > SESSION_EXPIRE_TIME;
        if (longTimeSession || longResting) 
        {
            RUMApplication::getInstance().refreshSessionId();
        }
    }

    void RUMManager::startView(const std::string& viewName)
    {
        RUMView& view = initView(viewName);

        checkSessionRefresh();

        // close the previous view
        if (m_pActiveView != nullptr && !m_pActiveView->isClose()) 
        {
            m_pActiveView->close();
            closeView(*m_pActiveView);
        }

        RUMApplication::getInstance().checkViewCapacity();

        //long loadTime = -1;
        //if (preActivityDuration.get(viewName) != null) {
        //    loadTime = preActivityDuration.get(viewName);
        //    preActivityDuration.remove(viewName);
        //}
        std::string viewReferrer = getViewReferrerName(&view);
        setActiveView(&view);
        //activeView = new ActiveViewBean(viewName, viewReferrer, loadTime, sessionId);
        //FTMonitorManager.get().addMonitor(activeView.getId());
        //FTMonitorManager.get().attachMonitorData(activeView);

        // start to monitor the performance metric
    }

    void RUMManager::stopView()
    {
        checkActionClose();

        // stop monitor the performance metric
        //FTMonitorManager.get().attachMonitorData(activeView);
        //FTMonitorManager.get().removeMonitor(activeView.getId());

        closeView(*m_pActiveView);

        m_pActiveView = nullptr;
    }

    RUMView& RUMManager::initView(const std::string& viewName) 
    {
        LoggerManager::getInstance().logDebug("started view:" + viewName);

        //ViewBean bean = activeViewBean.convertToViewBean();
        //EventConsumerThreadPool.get().execute(() -> {
        //    FTDBManager.get().initSumView(bean);
        //});

        RUMView& view = RUMApplication::getInstance().addView(viewName);
        return view;
    }

    void RUMManager::closeView(RUMView& view)
    {
        LoggerManager::getInstance().logDebug("closed View:" + view.getName());

        view.close();
        //ViewBean viewBean = activeViewBean.convertToViewBean();
        //std::string viewId = viewBean.getId();
        //long timeSpent = viewBean.getTimeSpent();
        //EventConsumerThreadPool.get().execute(() -> {
        //    FTDBManager.get().closeView(viewId, timeSpent, viewBean.getAttrJsonString());
        //});

        flushRUMData();
    }

    /**
     * 1. Walk through the RUM tree from the root, report the final statistic data to the datakit agent if it's closed.
     * 2. Setting the sync flag once it's uploaded. 
     * 3. The view/action wont be cleaned up at this moment. It will be handled when the next view is added.
     */
    void RUMManager::flushRUMData()
    {
        LoggerManager::getInstance().logTrace("DUMP DB before Flush: \n" + RUMApplication::getInstance().toString());

        auto rumPublicTags = FTSDKConfigManager::getInstance().getRUMPublicDynamicTags();

        // query the rum application, flush the closed items
        auto& viewList = RUMApplication::getInstance().getItems();
        for (auto viewIt : viewList)
        {
            RUMView* vw = (RUMView*)(viewIt);
            for (auto actIt : vw->getItems())
            {
                RUMAction* act = (RUMAction*)actIt;
                if (act->isDirty() && act->isClose() && !act->isSynced())
                {
                    generateActionSum(*act, rumPublicTags);
                    act->setDirty(false);
                    act->setSynced(true);
                }
            }

            if (vw->isDirty() && vw->isClose() && !vw->isSynced())
            {
                generateViewSum(*vw, rumPublicTags);
                vw->setDirty(false);
                vw->setSynced(true);
            }
        }

        LoggerManager::getInstance().logTrace("DUMP DB after Flush: \n" + RUMApplication::getInstance().toString());
    }

    void RUMManager::generateViewSum(RUMView& view, std::map<std::string, std::string>& globalTags) 
    {
        TagMap tags;
        
        if (FTSDKConfigManager::getInstance().isUserDataBinded())
        {
            tags = FTSDKConfigManager::getInstance().getRUMPublicDynamicTags();
        }
        FieldMap fields;
        std::int64_t time = utils::getCurrentNanoTime();

        try 
        {
            tags[constants::KEY_RUM_SESSION_ID] = getSessionId();
            tags[constants::KEY_RUM_VIEW_NAME] = view.getName();
            tags[constants::KEY_RUM_VIEW_REFERRER] = getViewReferrerName(&view);
            tags[constants::KEY_RUM_VIEW_ID] = view.getId();
            //if (pRes->getLoadTime() > 0) {
            //    fields[constants::KEY_RUM_VIEW_LOAD] = pRes->getLoadTime();
            //}
            fields[constants::KEY_RUM_VIEW_ACTION_COUNT] = view.getActionList().size();
            fields[constants::KEY_RUM_VIEW_RESOURCE_COUNT] = view.getResourceCount();
            fields[constants::KEY_RUM_VIEW_ERROR_COUNT] = view.getErrorCount();
            if (view.isClose()) 
            {
                fields[constants::KEY_RUM_VIEW_TIME_SPENT] = view.getTimeSpent();
            }
            else 
            {
                fields[constants::KEY_RUM_VIEW_TIME_SPENT] = time - view.getStartTime();
            }
            fields[constants::KEY_RUM_VIEW_LONG_TASK_COUNT] = view.getLongTaskCount();
            fields[constants::KEY_RUM_VIEW_IS_ACTIVE] = !view.isClose();

            if (MonitorManager::getInstance().isDeviceMetricsMonitorType(DeviceMetricsMonitorType::CPU)) {
                double cpuTickCountPerSecond = view.getCpuTickCountPerSecond();
                long cpuTickCount = view.getCpuTickCount();
                if (cpuTickCountPerSecond > -1) {
                    fields[constants::KEY_CPU_TICK_COUNT_PER_SECOND] = cpuTickCountPerSecond;
                }
                if (cpuTickCount > -1) {
                    fields[constants::KEY_CPU_TICK_COUNT] = cpuTickCount;
                }
            }
            if (MonitorManager::getInstance().isDeviceMetricsMonitorType(DeviceMetricsMonitorType::MEMORY)) {
                fields[constants::KEY_MEMORY_MAX] = view.getMemoryMax();
                fields[constants::KEY_MEMORY_AVG] = view.getMemoryAvg();
            }
            if (MonitorManager::getInstance().isDeviceMetricsMonitorType(DeviceMetricsMonitorType::BATTERY)) {
                fields[constants::KEY_BATTERY_CURRENT_AVG] = view.getBatteryCurrentAvg();
                fields[constants::KEY_BATTERY_CURRENT_MAX] = view.getBatteryCurrentMax();
            }
            if (MonitorManager::getInstance().isDeviceMetricsMonitorType(DeviceMetricsMonitorType::FPS)) 
            {
                fields[constants::KEY_FPS_AVG] = view.getFpsAvg();
                fields[constants::KEY_FPS_MINI] = view.getFpsMini();

            }

        }
        catch (std::exception e) 
        {
            internal::LoggerManager::getInstance().logError("failed to generate view sum: ");
        }

        CacheDBManager::getInstance().insertRUMItem(constants::FT_MEASUREMENT_RUM_VIEW, tags, fields);

        //internal::LoggerManager::getInstance().logInfo("Generated View Summary: \n" + view.toString());
    }

    void RUMManager::generateActionSum(RUMAction& action, std::map<std::string, std::string>& globalTags)
    {
            FieldMap fields;
            TagMap tags;

            if (FTSDKConfigManager::getInstance().isUserDataBinded())
            {
                tags = FTSDKConfigManager::getInstance().getRUMPublicDynamicTags();
            }

            try
            {
                RUMView* pView = (RUMView*)action.getParent();
                tags[constants::KEY_RUM_VIEW_NAME] = pView->getName();
                tags[constants::KEY_RUM_VIEW_REFERRER] = getViewReferrerName(pView);
                tags[constants::KEY_RUM_VIEW_ID] = pView->getId();
                tags[constants::KEY_RUM_ACTION_NAME] = action.getName();
                tags[constants::KEY_RUM_ACTION_ID] = action.getId();
                tags[constants::KEY_RUM_ACTION_TYPE] = action.getActionType();
                tags[constants::KEY_RUM_SESSION_ID] = getSessionId();
                fields[constants::KEY_RUM_ACTION_LONG_TASK_COUNT] = action.getLongTaskCount();
                fields[constants::KEY_RUM_ACTION_RESOURCE_COUNT] = action.getResourceCount();
                fields[constants::KEY_RUM_ACTION_ERROR_COUNT] = action.getErrorCount();
                fields[constants::KEY_RUM_ACTION_DURATION] = action.getTimeSpent();// action.getDuration();

                CacheDBManager::getInstance().insertRUMItem(constants::FT_MEASUREMENT_RUM_ACTION, tags, fields);
            }
            catch (std::exception e) {
                internal::LoggerManager::getInstance().logError("failed to generate action sum: ");
            }

            internal::LoggerManager::getInstance().logInfo("Generated Action Summary: " + action.toString());
    }

    std::string RUMManager::getLastViewName()
    {
        auto view = internal::RUMApplication::getInstance().getViewList().back();
        return view->getName();
    }

    std::string RUMManager::getSessionId()
    {
        return internal::RUMApplication::getInstance().getSessionId();
    }

    void RUMManager::checkActionClose() 
    {
        if (m_pActiveAction == nullptr)
        {
            return;
        }

        std::int64_t now = utils::getCurrentNanoTime();
        std::int64_t lastActionTime = m_pActiveAction->getStartTime();
        bool waiting = m_pActiveAction->isNeedWaitAction() && (m_pActiveView != nullptr && !m_pActiveView->isClose());
        bool timeOut = now - lastActionTime > in_constants::ACTION_NEED_WAIT_TIME_OUT;
        bool needClose = !waiting
            && (now - lastActionTime > in_constants::ACTION_NORMAL_TIME_OUT)
            || timeOut || (m_pActiveView != nullptr && m_pActiveView->getId() != (m_pActiveAction->getParentId()));

        if (needClose) 
        {
            if (!m_pActiveAction->isClose()) 
            {
                m_pActiveAction->close();
                closeAction(*m_pActiveAction, timeOut);
            }
        }
    }

    void RUMManager::closeAction(RUMAction& action, bool force) 
    {
        std::string actionId = action.getId();
        std::int64_t duration = action.getTimeSpent();
        //EventConsumerThreadPool.get().execute(() -> {
        //    FTDBManager.get().closeAction(actionId, duration, force);
        //});
        action.close();
        flushRUMData();
    }

    std::string RUMManager::getViewReferrerName(RUMView* pView)
    {
        return (pView == nullptr || pView->getPrevRUMView() == nullptr) ? ROOT_RUM_VIEW_ID : pView->getPrevRUMView()->getName();
    }
        
    void RUMManager::checkResourceStop(std::string resourceId)
    {
        if (m_mapResource.find(resourceId) != m_mapResource.end())
        {
            RUMResource* pRes = m_mapResource[resourceId];
            if (pRes->endTime > 0)
            {
                auto tags = internal::FTSDKConfigManager::getInstance().getRUMPublicDynamicTags();
                internal::RUMManager::getInstance().attachRUMRelative(tags, true);

                FieldMap fields;

                RUMAction* pAction = (RUMAction*)(pRes->getParent());
                tags[constants::KEY_RUM_ACTION_ID] = m_pActiveAction == nullptr ? "" : m_pActiveAction->getId();
                tags[constants::KEY_RUM_ACTION_NAME] = m_pActiveAction == nullptr ? "" : m_pActiveAction->getName();

                tags[constants::KEY_RUM_VIEW_ID] = m_pActiveView->getId();
                tags[constants::KEY_RUM_VIEW_NAME] = m_pActiveView->getName();
                tags[constants::KEY_RUM_VIEW_REFERRER] = getViewReferrerName(m_pActiveView);
                tags[constants::KEY_RUM_SESSION_ID] = getSessionId();


                tags[constants::KEY_RUM_RESOURCE_URL_HOST] = "http://test.com";

                //if (pRes->resourceType != null && !pRes->resourceType.isEmpty()) 
                //{
                //    tags[constants::KEY_RUM_RESOURCE_TYPE] = "resType";
                //}
                /*tags[constants::KEY_RUM_RESPONSE_CONNECTION, pRes->responseConnection);
                tags[constants::KEY_RUM_RESPONSE_CONTENT_TYPE, pRes->responseContentType);
                tags[constants::KEY_RUM_RESPONSE_CONTENT_ENCODING, pRes->responseContentEncoding);
                tags[constants::KEY_RUM_RESOURCE_METHOD, pRes->resourceMethod);
                tags[constants::KEY_RUM_RESOURCE_TRACE_ID, pRes->traceId);
                tags[constants::KEY_RUM_RESOURCE_SPAN_ID, pRes->spanId);

                int resourceStatus = pRes->resourceStatus;
                std::string resourceStatusGroup = "";
                if (resourceStatus > 0) {
                    tags[constants::KEY_RUM_RESOURCE_STATUS, resourceStatus);
                    long statusGroupPrefix = pRes->resourceStatus / 100;
                    resourceStatusGroup = statusGroupPrefix + "xx";
                    tags[constants::KEY_RUM_RESOURCE_STATUS_GROUP, resourceStatusGroup);
                }
                */

                //if (pRes->resourceSize > 0) 
                {
                    fields[constants::KEY_RUM_RESOURCE_SIZE] = 100;
                }
                //if (pRes->resourceLoad > 0) 
                {
                    fields[constants::KEY_RUM_RESOURCE_DURATION] = 200000;
                }

                /*
                if (pRes->resourceDNS > 0) {
                    fields[constants::KEY_RUM_RESOURCE_DNS, pRes->resourceDNS);
                }
                if (pRes->resourceTCP > 0) {
                    fields[constants::KEY_RUM_RESOURCE_TCP, pRes->resourceTCP);
                }
                if (pRes->resourceSSL > 0) {
                    fields[constants::KEY_RUM_RESOURCE_SSL, pRes->resourceSSL);
                }
                if (pRes->resourceTTFB > 0) {
                    fields[constants::KEY_RUM_RESOURCE_TTFB, pRes->resourceTTFB);
                }

                if (pRes->resourceTrans > 0) {
                    fields[constants::KEY_RUM_RESOURCE_TRANS, pRes->resourceTrans);
                }

                if (pRes->resourceFirstByte > 0) {
                    fields[constants::KEY_RUM_RESOURCE_FIRST_BYTE, pRes->resourceFirstByte);

                }
                std::string urlPath = pRes->urlPath;
                std::string urlPathGroup = "";

                if (!urlPath.isEmpty()) {
                    urlPathGroup = urlPath.replaceAll("\\/([^\\/]*)\\d([^\\/]*)", "/?");
                    tags[constants::KEY_RUM_RESOURCE_URL_PATH, urlPath);
                    tags[constants::KEY_RUM_RESOURCE_URL_PATH_GROUP, urlPathGroup);
                }


                tags[constants::KEY_RUM_RESOURCE_URL, pRes->url);
                fields[constants::KEY_RUM_REQUEST_HEADER, pRes->requestHeader);
                fields[constants::KEY_RUM_RESPONSE_HEADER, pRes->responseHeader);*/

                CacheDBManager::getInstance().insertRUMItem(constants::FT_MEASUREMENT_RUM_RESOURCE, tags, fields);


                /*if (pRes->resourceStatus >= HttpsURLConnection.HTTP_BAD_REQUEST) {
                    JSONObject errorTags = FTRUMConfigManager.get().getRUMPublicDynamicTags();
                    JSONObject errorField = new JSONObject();
                    errortags[constants::KEY_RUM_ERROR_TYPE, ErrorType.NETWORK.toString());
                    errortags[constants::KEY_RUM_ERROR_SOURCE, ErrorSource.NETWORK.toString());
                    errortags[constants::KEY_RUM_ERROR_SITUATION, AppState.RUN.toString());
                    errortags[constants::KEY_RUM_ACTION_ID, actionId);
                    errortags[constants::KEY_RUM_ACTION_NAME, actionName);
                    errortags[constants::KEY_RUM_VIEW_ID, viewId);
                    errortags[constants::KEY_RUM_VIEW_NAME, viewName);
                    errortags[constants::KEY_RUM_VIEW_REFERRER, viewReferrer);
                    errortags[constants::KEY_RUM_SESSION_ID, sessionId);

                    if (resourceStatus > 0) {
                        errortags[constants::KEY_RUM_RESOURCE_STATUS, resourceStatus);
                        errortags[constants::KEY_RUM_RESOURCE_STATUS_GROUP, resourceStatusGroup);
                    }
                    errortags[constants::KEY_RUM_RESOURCE_URL, pRes->url);
                    errortags[constants::KEY_RUM_RESOURCE_URL_HOST, pRes->urlHost);
                    errortags[constants::KEY_RUM_RESOURCE_METHOD, pRes->resourceMethod);

                    if (!urlPath.isEmpty()) {
                        errortags[constants::KEY_RUM_RESOURCE_URL_PATH, urlPath);
                        errortags[constants::KEY_RUM_RESOURCE_URL_PATH_GROUP, urlPathGroup);
                    }
                    std::string errorMsg = "[" + pRes->resourceStatus + "]" + "[" + pRes->url + "]";

                    errorField.put(constants::KEY_RUM_ERROR_MESSAGE, errorMsg);
                    errorField.put(constants::KEY_RUM_ERROR_STACK, pRes->errorStack);

                    FTTrackInner.getInstance().rum(time, constants::FT_MEASUREMENT_RUM_ERROR, errorTags, errorField);
                    increaseError(tags);*/
                //}
                
            }
        }
    }

    bool RUMManager::checkSessionWillCollect(std::string sessionId) 
    {
        return m_mapCollectStatus[sessionId];
    }

    void RUMManager::checkSessionKeep()
    {
        float sampleRate = FTSDKConfigManager::getInstance().getRUMConfig().getSamplingRate();
        std::string sessionId = getSessionId();

        bool collect = utils::enableTraceSamplingRate(sampleRate);
        if (!collect) 
        {
            if (m_mapCollectStatus.size() + 1 > FILTER_CAPACITY) {
                m_mapCollectStatus.erase(std::begin(m_mapCollectStatus));
            }
            m_mapCollectStatus[sessionId] = collect;

        }

        CacheDBManager::getInstance().updateRUMCollect(collect);
    }

    void RUMManager::checkForActionTimeout()
    {
        try
        {
            std::this_thread::sleep_for(std::chrono::seconds(ACTION_TIMEOUT));
            checkActionClose();
        }
        catch (const std::exception& ex)
        {
            std::string err = "Exception in checkForActionTimeout : ";
            internal::LoggerManager::getInstance().logError(err.append(ex.what()));
        }
    }
}