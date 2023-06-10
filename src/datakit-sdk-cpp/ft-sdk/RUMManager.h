/*****************************************************************//**
 * \file   RUMManager.h
 * \brief  The unified entry for all RUM operations
 * 
 * \author Zhou Guangyong
 * \date   October 2022
 *********************************************************************/
#pragma once
#ifndef _DATAKIT_SDK_RUM_MANAGER_H_
#define _DATAKIT_SDK_RUM_MANAGER_H_

#include "Singleton.h"
#include "AbstractManager.h"
#include <map>
#include <string>
#include "Include/FTSDKDataContracts.h"
#include "RumDataEntity.h"
#include "DataSyncManager.h"

namespace com::ft::sdk::internal
{
    typedef std::map<std::string, std::string> RUMMap;

    class RUMManager :
        public AbstractManager, public Singleton<RUMManager>
    {
    public:
        void addLongTask(std::string log, std::int64_t duration);
        void addError(std::string log, std::string message, RUMErrorType errorType, AppState state);
        void addError(std::string log, RUMMap& tags, FieldMap& fields);

        void addAction(std::string actionName, std::string actionType, std::int64_t duration, std::int64_t startTime);
        void startAction(std::string actionName, std::string actionType);
        void stopAction();

        void addResource(std::string resourceId, ResourceParams params, NetStatus netStatusBean);
        void startResource(std::string resourceId);
        void stopResource(std::string resourceId);

        void startView(const std::string& viewName);
        void stopView();

        void init();
        void clear();
    public:
        void attachRUMRelative(std::map<std::string, std::string>& tags, bool withAction);

        //void setActiveView(RUMView* pActiveView);
        //void setActiveAction(RUMAction* pActiveAction);


    private:
        RUMManager();
        void setTransformContent(std::string resourceId, ResourceParams params);
        void setNetState(std::string resourceId, NetStatus netStatusBean);
        void putRUMResourcePerformance(std::string resourceId);
        void checkToAddResource(std::string key, RUMResource* pRes);
        void checkResourceStop(std::string resourceId);
        void checkSessionRefresh();
        void checkActionClose();
        void closeAction(RUMAction& action, bool force);
        RUMAction& initAction(std::string actionName, std::string actionType);

        RUMView& initView(const std::string& viewName);
        void closeView(RUMView& view);
        std::string getViewReferrerName(RUMView* pView);

        void attachRUMRelativeForResource(RUMResource& res);

        void flushRUMData();
        void generateViewSum(RUMView& view, std::map<std::string, std::string>& globalTags);
        void generateActionSum(RUMAction& action, std::map<std::string, std::string>& globalTags);

        std::string getLastViewName();
        std::string getSessionId();

        void setActiveView(RUMView* pActiveView);
        void setActiveAction(RUMAction* pActiveAction);

        bool checkSessionWillCollect(std::string sessionId);
        void checkSessionKeep();
        void checkForActionTimeout();
    private:
        std::map<std::string, bool> m_mapCollectStatus;
        RUMView* m_pActiveView = nullptr;
        RUMAction* m_pActiveAction = nullptr;

        std::int64_t m_lastSessionTime = 0;
        std::int64_t m_lastActionTime = 0;
        std::map<std::string, RUMResource*> m_mapResource;

        ENABLE_SINGLETON();
    };

}

#endif // !_DATAKIT_SDK_RUM_MANAGER_H_