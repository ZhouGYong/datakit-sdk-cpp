/*****************************************************************//**
 * \file   TraceHeader.h
 * \brief  Trace Header Generator
 * 
 * \author Zhou Guangyong
 * \date   December 2022
 *********************************************************************/
#pragma once
#ifndef _DATAKIT_SDK_TRACE_HEADER_H_
#define _DATAKIT_SDK_TRACE_HEADER_H_

#include <string>
#include <map>
#include "Utils.h"
#include "TraceIDGenerator.h"

namespace com::ft::sdk::internal
{

	class TraceHeader
	{
    public:
        const std::string ZIPKIN_TRACE_ID = "X-B3-TraceId";
        const std::string ZIPKIN_SPAN_ID = "X-B3-SpanId";
        const std::string ZIPKIN_SAMPLED = "X-B3-Sampled";
        const std::string ZIPKIN_B3_HEADER = "b3";
        const std::string W3C_TRACEPARENT_KEY = "traceparent";
        const std::string JAEGER_KEY = "uber-trace-id";
        const std::string SKYWALKING_V3_SW_8 = "sw8";
        const std::string SKYWALKING_V3_SW_6 = "sw6";

        const std::string DD_TRACE_TRACE_ID_KEY = "x-datadog-trace-id";

        const std::string DD_TRACE_PARENT_SPAN_ID_KEY = "x-datadog-parent-id";
        const std::string DD_TRACE_SAMPLING_PRIORITY_KEY = "x-datadog-sampling-priority";
        const std::string DD_TRACE_SAMPLED = "x-datadog-sampled";
        const std::string DD_TRACE_ORIGIN_KEY = "x-datadog-origin";

    public:
        TraceHeader(bool m_enableTrace, TraceType traceType);

        //是否可以采样
        bool m_enableTrace;
        //请求开始时间
        std::int64_t requestTime = utils::getCurrentNanoTime();
        std::string m_traceID = "";
        std::string m_spanID = "";

        //private HttpUrl httpUrl;
        //private final FTTraceConfig config;

        TraceType m_traceType;

        std::string getTraceID() {
            return m_traceID;
        }

        std::string getSpanID() {
            return m_spanID;
        }

        //public HttpUrl getUrl() {
        //    return httpUrl;
        //}

        std::map<std::string, std::string> getTraceHeader(HttpUrl& httpUrl);

    private:
        TraceIDGenerator& m_traceIdGenerator = TraceIDGenerator::getInstance();
	};

}

#endif // _DATAKIT_SDK_TRACE_HEADER_H_