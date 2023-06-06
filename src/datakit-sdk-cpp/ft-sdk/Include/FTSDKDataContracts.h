#pragma once
#ifndef _DATAKIT_CPP_DATA_CONTRACT_H_
#define _DATAKIT_CPP_DATA_CONTRACT_H_

#include <string>
#include <vector>
#include <stdexcept>
#include "datakit_exports.h"

#define ENUM_MACRO10(name, v1, v2, v3, v4, v5, v6, v7, v8, v9, v10)\
    enum class name { v1, v2, v3, v4, v5, v6, v7, v8, v9, v10 };\
    const char *name##Strings[] = { #v1, #v2, #v3, #v4, #v5, #v6, #v7, #v8, #v9, #v10};\
    template<typename T>\
    constexpr const char *name##ToString(T value) { return name##Strings[static_cast<int>(value)]; }

#define ENUM_MACRO2(name, v1, v2)\
    enum class name { v1, v2};\
	namespace name##Strings {	\
		static const char *name##Map[] = { #v1, #v2};\
	}	\
    inline const char *EnumToString(name value) { return name##Strings::name##Map[static_cast<int>(value)]; }

#define ENUM_MACRO3(name, v1, v2, v3)\
    enum class name { v1, v2, v3};\
	namespace name##Strings {	\
		static const char *name##Map[] = { #v1, #v2, #v3};\
	}	\
    inline const char *EnumToString(name value) { return name##Strings::name##Map[static_cast<int>(value)]; }

#define ENUM_MACRO4(name, v1, v2, v3, v4)\
    enum class name { v1, v2, v3, v4};\
	namespace name##Strings {	\
		static const char *name##Map[] = { #v1, #v2, #v3, #v4};\
	}	\
    inline const char *EnumToString(name value) { return name##Strings::name##Map[static_cast<int>(value)]; }

#define ENUM_MACRO5(name, v1, v2, v3, v4, v5)\
    enum class name { v1, v2, v3, v4, v5};\
	namespace name##Strings {	\
		static const char *name##Map[] = { #v1, #v2, #v3, #v4, #v5};\
	}	\
    inline const char *EnumToString(name value) { return name##Strings::name##Map[static_cast<int>(value)]; }

namespace com::ft::sdk
{
	/**
	 * Trace 类型
	 */
	enum class TraceType : char {
		/**
		 * datadog trace
		 *
		 * x-datadog-trace-id
		 * x-datadog-parent-id
		 * x-datadog-sampling-priority
		 * x-datadog-origin
		 */
		DDTRACE = 0x0,

		/**
		 * zipkin multi header
		 *
		 * X-B3-TraceId
		 * X-B3-SpanId
		 * X-B3-Sampled
		 */
		 ZIPKIN_MULTI_HEADER,

		 /**
		  * zipkin single header b3
		 */
		 ZIPKIN_SINGLE_HEADER,

		 /**
		  * w3c traceparent
		 */
		 TRACEPARENT,

		 /**
		  * skywalking 8.0+
		 */
		 SKYWALKING,

		 /**
		  * jaeger,header uber-trace-id
		 */
		 JAEGER
	};

	/**
	 * Sky Walking Trace 版本
	 */
	enum class SkyWalkingVersion : char
	{
		V2,
		V3
	};

	/**
	 * 程序运行状态
	 */
	ENUM_MACRO3(AppState, UNKNOWN, STARTUP, RUN)

	/**
	 * RUM 错误类型
	 */
	ENUM_MACRO4(RUMErrorType, NATIVE_CRASH, JAVA_CRASH, FLUTTER_CRASH, NETWORK_ERROR)

	/**
	 * 错误源
	 */
	ENUM_MACRO2(ErrorSource, LOGGER, NETWORK)

	/**
	 * 日志级别
	 */
	ENUM_MACRO5(LogLevel, INFO, WARNING, ERR, CRITICAL, OK)

	/**
	 * 数据传输环境
	 */
	ENUM_MACRO5(EnvType, PROD, GRAY, PRE, COMMON, LOCAL)


    /**
     * 网络耗时统计，支持方式：
     * 1. 时间段
     * 2. 时间点
     */
	struct FTSDK_EXPORT NetStatus {
		//----------时间段参数-------------------
        /** tcp 连接耗时 */
        long tcpTime = -1;
        /** dns 解析耗时 */
        long dnsTime = -1;
        /** ssl 连接耗时 */
        long sslTime = -1;
        /** dns 解析到接收到第一个数据包的总时长 */
        long firstByteTime = -1;
        /** 请求响应时间，开始发送请求到接收到响应首包的时长 */
        long ttfb = -1;
        /** 响应内容传输耗时 */
        long responseTime = -1;

		//----------时间点参数-------------------
        /** 请求开始时间 */
		long fetchStartTime = -1;
        /** tcp 连接时间 */
		long tcpStartTime = -1;
        /** tcp 结束时间 */
		long tcpEndTime = -1;
        /** dns 开始时间 */
		long dnsStartTime = -1;
        /** dns 结束时间 */
		long dnsEndTime = -1;
        /** 响应开始时间 */
		long responseStartTime = -1;
        /** 响应结束时间 */
		long responseEndTime = -1;
        /** ssl 开始时间 */
		long sslStartTime = -1;
        /** ssl 结束时间 */
		long sslEndTime = -1;

	};

    /**
     * 网络传输参数
     */
	struct FTSDK_EXPORT ResourceParams {
        /** 网络地址 */
		std::string url = "";
        /** 请求头参数 */
		std::string requestHeader = "";
        /** 响应头参数 */
		std::string responseHeader = "";
        /** 响应 connection */
		std::string responseConnection = "";
        /** 响应 ContentType */
		std::string responseContentType = "";
        /** 响应 ContentEncoding */
		std::string responseContentEncoding = "";
        /** 响应 ContentEncoding */
		std::string resourceMethod = "";
        /** 返回 body 内容 */
		std::string responseBody = "";
        /** 响应 HTTP 状态 */
		int resourceStatus = -1;
	};

	class FTSDK_EXPORT FTSDKException : public std::exception
	{
	public:
		FTSDKException(const std::string& file, int line, const std::string& source, const std::string& message)
			: std::exception(std::logic_error(source + " : " + message))
		{
		}
	};

}

#endif // !_DATAKIT_CPP_DATA_CONTRACT_H_

