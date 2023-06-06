#include "pch.h"
#include "TraceHeader.h"
#include "TraceIDGenerator.h"

namespace com::ft::sdk::internal
{

    TraceHeader::TraceHeader(bool m_enableTrace, TraceType traceType)
    {
        this->m_enableTrace = m_enableTrace;
        this->m_traceType = traceType;
    }

    std::map<std::string, std::string> TraceHeader::getTraceHeader(HttpUrl& httpUrl)
    {
        std::map<std::string, std::string> headers;
        //if (m_traceConfig == null)
        //{
        //    return headers;
        //}
        //this.httpUrl = httpUrl;

        std::string sampled;
        //抓取数据内容
        if (m_enableTrace) 
        {
            sampled = "1";
        }
        else 
        {
            sampled = "0";
        }

        //在数据中添加标记
        if (m_traceType == TraceType::ZIPKIN_MULTI_HEADER
            || m_traceType == TraceType::ZIPKIN_SINGLE_HEADER
            || m_traceType == TraceType::JAEGER || m_traceType == TraceType::TRACEPARENT) 
        {
            m_traceID = m_traceIdGenerator.getZipKinNewTraceId();
            m_spanID = m_traceIdGenerator.getZipKinNewSpanId();
        }
        else if (m_traceType == TraceType::DDTRACE) 
        {
            m_traceID = m_traceIdGenerator.getDDtraceNewTraceId();
            m_spanID = m_traceIdGenerator.getDDtraceNewTraceId();
        }

        if (m_traceType == TraceType::ZIPKIN_MULTI_HEADER) 
        {
            headers[ZIPKIN_SPAN_ID] = m_spanID;
            headers[ZIPKIN_TRACE_ID] = m_traceID;
            headers[ZIPKIN_SAMPLED] = sampled;
        }
        else if (m_traceType == TraceType::ZIPKIN_SINGLE_HEADER) 
        {
            headers[ZIPKIN_B3_HEADER] = m_traceID + "-" + m_spanID + "-" + sampled;
        }
        else if (m_traceType == TraceType::TRACEPARENT) 
        {
            std::string version = "00";
            std::string sampledStr = "0" + sampled;
            std::string parentID = m_spanID;
            headers[W3C_TRACEPARENT_KEY] = version + "-" + m_traceID + "-" + parentID + "-" + sampledStr;
        }
        else if (m_traceType == TraceType::JAEGER) 
        {
            std::string parentSpanID = "0";
            headers[JAEGER_KEY] = m_traceID + ":" + m_spanID + ":" + parentSpanID + ":" + sampled;
        }
        else if (m_traceType == TraceType::DDTRACE) 
        {
            m_traceID = m_traceIdGenerator.getDDtraceNewTraceId();
            m_spanID = m_traceIdGenerator.getDDtraceNewTraceId();
            headers[DD_TRACE_ORIGIN_KEY] = "rum";
            headers[DD_TRACE_SAMPLING_PRIORITY_KEY] = m_enableTrace ? "2" : "-1";
            headers[DD_TRACE_SAMPLED] = sampled;
            headers[DD_TRACE_PARENT_SPAN_ID_KEY] = m_spanID;
            headers[DD_TRACE_TRACE_ID_KEY] = m_traceID;
        }
        else if (m_traceType == TraceType::SKYWALKING) 
        {
            ////SkyWalkingUtils skyWalkingUtils = new SkyWalkingUtils(SkyWalkingUtils.SkyWalkingVersion.V3, sampled, requestTime, httpUrl, config);
            ////m_traceID = skyWalkingUtils.getNewTraceId();
            ////m_spanID = skyWalkingUtils.getNewParentTraceId() + "0";
            ////headers[SKYWALKING_V3_SW_8] = skyWalkingUtils.getSw();
            headers[SKYWALKING_V3_SW_8] = m_traceIdGenerator.getSkyWalkingSW(SkyWalkingVersion::V3, sampled, requestTime, httpUrl);
        }
        //        else if (FTHttpConfig.get().traceType == TraceType::SKYWALKING_V2) {
        //            SkyWalkingUtils skyWalkingUtils = new SkyWalkingUtils(SkyWalkingUtils.SkyWalkingVersion.V2, sampled, requestTime, httpUrl);
        //            m_traceID = skyWalkingUtils.getNewTraceId();
        //            m_spanID = skyWalkingUtils.getNewParentTraceId() + "0";
        //            headers[SKYWALKING_V3_SW_6, skyWalkingUtils.getSw());
        //        }
        return headers;
    }
}