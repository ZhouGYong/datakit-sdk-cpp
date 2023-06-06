#include "pch.h"
#include "TraceManager.h"
#include "TraceHeader.h"

namespace com::ft::sdk::internal
{
    void TraceManager::initialize(FTTraceConfig& config)
    {
        m_traceConfig = config;

        m_enableTrace = utils::enableTraceSamplingRate(config.getSamplingRate());
    }

    void TraceManager::deinitialize()
    {

    }

    std::map<std::string, std::string> TraceManager::getTraceHeader(const std::string& urlStr)
    {
        PropagationUrl url = internal::PropagationUrl::parse(urlStr);
        HttpUrl httpUrl{ url.getHost(), url.getPath(), url.getPort() };

        TraceHeader th(m_enableTrace, m_traceConfig.getTraceType());

        return th.getTraceHeader(httpUrl);
    }

    std::map<std::string, std::string> TraceManager::getTraceHeader(const std::string& key, const std::string& urlStr)
    {
        PropagationUrl url = internal::PropagationUrl::parse(urlStr);
        HttpUrl httpUrl{ url.getHost(), url.getPath(), url.getPort() };

        std::shared_ptr<TraceHeader> pTH = std::make_shared<TraceHeader>(m_enableTrace, m_traceConfig.getTraceType());
        m_mapTraceHeaderContainer[key] = std::make_shared<TraceHeaderContainer>(pTH);

        std::map<std::string, std::string> header = pTH->getTraceHeader(httpUrl);
        return header;
    }

    std::shared_ptr<TraceHeader> TraceManager::getHeader(std::string resourceId)
    {
        auto pContainer = m_mapTraceHeaderContainer[resourceId];
        if (pContainer != nullptr)
        {
            return pContainer->header;
        }

        return nullptr;
    }

    void TraceManager::removeByAddResource(std::string key)
    {
        auto pContainer = m_mapTraceHeaderContainer[key];
        if (pContainer != nullptr) 
        {
            pContainer->addResourced = true;
            checkToRemove(key, pContainer);
        }
    }

    void TraceManager::removeByStopResource(std::string key)
    {
        auto pContainer = m_mapTraceHeaderContainer[key];
        if (pContainer != nullptr)
        {
            pContainer->resourceStop = true;
            checkToRemove(key, pContainer);
        }
    }

    void TraceManager::checkToRemove(std::string key, std::shared_ptr<TraceHeaderContainer> pContainer)
    {
        if (pContainer->addResourced && pContainer->resourceStop
            || pContainer->isTimeOut()) 
        {
            m_mapTraceHeaderContainer.erase(key);
        }
    }
}