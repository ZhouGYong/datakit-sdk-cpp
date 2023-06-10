/*****************************************************************//**
 * \file   TraceIDGenerator.h
 * \brief  Helper class to generate trace id
 * 
 * \author Zhou Guangyong
 * \date   December 2022
 *********************************************************************/
#pragma once
#ifndef _DATAKIT_TRACEID_HELPER_H_
#define _DATAKIT_TRACEID_HELPER_H_

#include <string>
#include <memory>
#include <atomic>
#include "InternalStructs.h"
#include "Singleton.h"

namespace com::ft::sdk::internal
{

	class TraceIDGenerator : public Singleton<TraceIDGenerator>
	{
	public:
		std::string getZipKinNewTraceId();
		std::string getZipKinNewSpanId();

		std::string getDDtraceNewTraceId();

		std::string getSkyWalkingNewTraceId();
		std::string getSkyWalkingNewSpanId();
		std::string getSkyWalkingSW(SkyWalkingVersion ver, const std::string& sampled, std::int64_t requestTime, HttpUrl url);

	private:
		TraceIDGenerator();

		std::string createSw8Head(std::string sampled, std::int64_t requestTime, HttpUrl url);
		std::string createSw6Head(std::string sampled, std::int64_t requestTime, HttpUrl url);

		//std::string generateUUID();
		std::uint64_t generate64BitsRandomNumber();
		std::string convertNumberToId(std::uint64_t num);

	private:
		std::atomic<int> m_increasingInt = -1;
		std::atomic<std::int64_t> m_increasingLong = 0;
		std::string m_skyWalkingTraceUUID;
		std::string m_skyWalkingParentServiceUUID;

#if defined(WIN32)
		const char* ZIPKIN_SPAN_ID_FMT = "%016llx"; // or apple
#else // Linux, gcc
		const char* ZIPKIN_SPAN_ID_FMT = "%016lx";
#endif

		const int INCREASING_NUMBER_MAX = 9999;

		ENABLE_SINGLETON();
	};

}
#endif // !_DATAKIT_TRACEID_HELPER_H_



