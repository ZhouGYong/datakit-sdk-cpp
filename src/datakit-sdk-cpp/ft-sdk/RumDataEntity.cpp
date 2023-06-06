#include "pch.h"
#include "RumDataEntity.h"
#include "Utils.h"
#include <algorithm>
#include <sstream>
#include "FTSDKConfigManager.h"

namespace com::ft::sdk::internal
{

    template<typename T>
    void deleteRUMItemList(std::vector<T*>& vtRUMItem)
    {
        std::for_each(vtRUMItem.begin(), vtRUMItem.end(), [](T* pItem) {
            if (pItem != nullptr)
            {
                delete pItem;
            }
        });
    }


    RUMItem::RUMItem()
    {
        m_itemId = "";
        m_parentId = "";
        m_pParentItem = nullptr;
    }

    RUMItem::RUMItem(RUMItem* pParentItem)
    {
        m_itemId = internal::utils::generateRandomUUIDWithDash();
        m_parentId = pParentItem->getId();
        m_pParentItem = pParentItem;
    }

    std::string RUMItem::toString()
    {
        std::stringstream ss;
        ss << "name=" << m_itemName << ",id=" << m_itemId << ", parent id=" << m_parentId << std::endl;

        return ss.str();
    }

    RUMItemContainer::RUMItemContainer()
    {

    }

    std::string RUMItemContainer::toString()
    {
        std::stringstream ss;
        ss << RUMItem::toString();
        ss << "items=[" << std::endl;
        for (auto it : m_rumItems)
        {
            ss << "    " << it->toString();
        }
        ss << "]" << std::endl;

        return ss.str();
    }    
    
    void RUMItemContainer::reset()
    {
        m_rumItems.clear();
    }
    RUMItemContainer::RUMItemContainer(RUMItem* pParentItem) : RUMItem(pParentItem)
    {

    }

    void RUMItemContainer::addItem(RUMItem* item)
    {
        m_rumItems.push_back(item);
    }

    RUMApplication::RUMApplication() : m_rumSession(this)
    {
        m_itemId = internal::FTSDKConfigManager::getInstance().getRUMConfig().getRumAppId();
        m_parentId = ROOT_RUM_ID;
        m_itemName = "APP";
    }

    RUMApplication::~RUMApplication()
    {

    }

    //RUMApplication::RUMApplication(const std::string& parentId) : RUMItemContainer(parentId)
    //{
    //}

    const std::string& RUMApplication::getSessionId()
    {
        return m_rumSession.getId();
    }

    void RUMApplication::refreshSessionId()
    {
        m_rumSession.refreshId();
    }

    std::vector<RUMView*> RUMApplication::getViewList()
    {
        std::vector<RUMView*> vtView;
        for (auto& item : m_rumItems)
        {
            vtView.push_back(static_cast<RUMView *>(item));
        }

        return vtView;
    }

    RUMSession& RUMApplication::createSession()
    {
        RUMSession* pSession = new RUMSession(this);
        this->addItem(pSession);

        return (RUMSession&)(*(this->m_rumItems.back()));
    }

    RUMView& RUMApplication::addView(std::string viewName)
    {
        RUMView* pView = new RUMView(this, viewName);

        if (this->m_rumItems.size() > 0)
        {
            pView->setPrevRUMView(static_cast<RUMView*>(this->m_rumItems.back()));
        }
        this->addItem(pView);

        return *pView;
    }

    void RUMApplication::checkViewCapacity()
    {
        if (m_rumItems.size() > 2)
        {
            auto it = m_rumItems.begin();
            if (*it != nullptr)
            {
                delete (*it);
            }

            m_rumItems.erase(it);
        }
    }

    RUMSession::RUMSession(RUMItem* pParentItem) : RUMItem(pParentItem)
    {
        this->m_parentId = pParentItem->getId();
    }

    void RUMSession::refreshId()
    {
        m_itemId = internal::utils::generateRandomUUIDWithDash();
    }

    RUMView::RUMView(RUMItem* pParentItem, const std::string& viewName) : RUMItemContainer(pParentItem)
    {
        this->m_itemName = viewName;
    }

    RUMAction& RUMView::addAction(const std::string& actionName, const std::string& actionType)
    {
        RUMAction* pAction = new RUMAction(this, actionName, actionType);
        this->m_rumItems.push_back(pAction);

        return *pAction;
    }

    std::vector<RUMAction*> RUMView::getActionList()
    {
        std::vector<RUMAction*> vtAction;
        for (auto& item : m_rumItems)
        {
            vtAction.push_back((RUMAction*)item);
        }

        return vtAction;
    }

    int RUMView::getLongTaskCount()
    {
        int size = this->getLongTaskList().size();
        for (auto& act : this->getActionList())
        {
            size += act->getLongTaskList().size();
        }

        return size;
    }

    int RUMView::getErrorCount()
    {
        int size = this->getErrorList().size();
        for (auto& act : this->getActionList())
        {
            size += act->getErrorList().size();
        }

        return size;
    }

    int RUMView::getResourceCount()
    {
        int size = this->getResourceList().size();
        for (auto& act : this->getActionList())
        {
            size += act->getResourceList().size();
        }

        return size;
    }

    //todo: stop monitor the performance metric
    void RUMView::close()
    {
        RUMIndicatorHost::close();

        //ViewBean viewBean = activeViewBean.convertToViewBean();
        //String viewId = viewBean.getId();
        //long timeSpent = viewBean.getTimeSpent();
        //EventConsumerThreadPool.get().execute(() -> {
        //    FTDBManager.get().closeView(viewId, timeSpent, viewBean.getAttrJsonString());
        //});
    }

    std::string RUMView::toString()
    {
        std::stringstream ss;
        ss << "RUMView:";
        ss << RUMItemContainer::toString();
        ss << RUMIndicatorHost::toString();

        return ss.str();
    }

    RUMResource& RUMView::addResource(RUMItem* pParentItem, const std::string& name)
    {
        RUMResource& res = RUMIndicatorHost::addResource(pParentItem, name);

        res.viewId = this->getId();
        res.viewName = this->getName();
        res.viewReferrer = getPrevRUMView() == nullptr ? ROOT_RUM_VIEW_ID : getPrevRUMView()->getName();

        return res;
    }

    RUMIndicatorHost::RUMIndicatorHost()
    {
        m_startTime = utils::getCurrentNanoTime();
    }

    RUMItemContainer::~RUMItemContainer()
    {
        deleteRUMItemList<RUMItem>(m_rumItems);
    }

    RUMIndicatorHost::~RUMIndicatorHost()
    {
        deleteRUMItemList<RUMItem>(m_vtResource);
        deleteRUMItemList<RUMItem>(m_vtError);
        deleteRUMItemList<RUMItem>(m_vtLongTask);
    }

    RUMResource& RUMIndicatorHost::addResource(RUMItem* pParentItem, const std::string& name)
    {
        RUMResource* pRes = new RUMResource(pParentItem, name);
        this->m_vtResource.push_back(pRes);

        return *pRes;
        //return (RUMResource&)(this->m_vtResource.back());
    }

    RUMError& RUMIndicatorHost::addError(RUMItem* pParentItem, const std::string& name)
    {
        RUMError* pError = new RUMError(pParentItem, name);
        this->m_vtError.push_back(pError);

        return *pError;
        //return (RUMError&)(this->m_vtError.back());
    }

    RUMLongTask& RUMIndicatorHost::addLongTask(RUMItem* pParentItem, const std::string& name)
    {
        RUMLongTask* pLongTask = new RUMLongTask(pParentItem, name);
        this->m_vtLongTask.push_back(pLongTask);

        return *pLongTask;
        //return (RUMLongTask&)(this->m_vtLongTask.back());
    }

    std::string RUMIndicatorHost::toString()
    {
        std::stringstream ss;
        ss << "    LongTask=[" << std::endl;
        for (auto it : m_vtLongTask)
        {
            ss << it->toString();
        }
        ss << "    ]" << std::endl;

        ss << "    Error=[" << std::endl;
        for (auto it : m_vtError)
        {
            ss << it->toString();
        }
        ss << "    ]" << std::endl;

        ss << "    Resource=[" << std::endl;
        for (auto it : m_vtResource)
        {
            ss << it->toString();
        }
        ss << "    ]" << std::endl;

        return ss.str();
    }


    RUMAction::RUMAction(RUMItem* pParentItem, const std::string& actionName, const std::string& actionType) : RUMItemContainer(pParentItem)
    {
        this->m_itemName = actionName;
        this->m_actionType = actionType;
    }

    std::string RUMAction::toString()
    {
        std::stringstream ss;
        ss << "RUMAction:";
        ss << RUMItemContainer::toString();
        ss << RUMIndicatorHost::toString();

        return ss.str();
    }

    RUMResource& RUMAction::addResource(RUMItem* pParentItem, const std::string& name)
    {
        RUMResource& res = RUMIndicatorHost::addResource(pParentItem, name);

        res.actionId = this->getId();
        res.actionName = this->getName();

        RUMView* pView = (RUMView*)(this->getParent());
        res.viewId = pView->getId();
        res.viewName = pView->getName();
        res.viewReferrer = pView->getPrevRUMView() == nullptr ? ROOT_RUM_VIEW_ID : pView->getPrevRUMView()->getName();

        return res;
    }

    RUMResource::RUMResource(RUMItem* pParentItem, const std::string& name) : RUMItem(pParentItem)
    {
        m_itemName = name;
    }

    RUMError::RUMError(RUMItem* pParentItem, const std::string& name) : RUMItem(pParentItem)
    {
        m_itemName = name;
    }

    RUMLongTask::RUMLongTask(RUMItem* pParentItem, const std::string& name) : RUMItem(pParentItem)
    {
        m_itemName = name;
    }
}